/*
 * NTV2 hardware input monitor
 *
 * Copyright 2016 AJA Video Systems Inc. All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ntv2_input.h"
#include "ntv2_features.h"
#include "ntv2_konareg.h"
#include "ntv2_hdmiin.h"

#define NTV2_INPUT_LOCK_COUNT			3
#define NTV2_INPUT_UNLOCK_COUNT			2
#define NTV2_INPUT_MONITOR_INTERVAL		100000

static void ntv2_input_monitor(unsigned long data);
static bool ntv2_compare_sdi_input_status(struct ntv2_sdi_input_status *status_a,
										  struct ntv2_sdi_input_status *status_b);
static int ntv2_sdi_single_stream_to_format(struct ntv2_sdi_input_status *status,
											struct ntv2_input_format *format);
static int ntv2_sdi_dual_stream_to_format(struct ntv2_sdi_input_status *status,
										  struct ntv2_input_format *format);
static int ntv2_sdi_quad_stream_to_format(struct ntv2_sdi_input_status *status,
										  struct ntv2_input_format *format);

struct ntv2_input *ntv2_input_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index)
{
	struct ntv2_input *ntv2_inp = NULL;

	ntv2_inp = kzalloc(sizeof(struct ntv2_input), GFP_KERNEL);
	if (ntv2_inp == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_input instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_inp->index = index;
	snprintf(ntv2_inp->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_inp->list);
	ntv2_inp->ntv2_dev = ntv2_obj->ntv2_dev;

	/* hardware monitor */
	setup_timer(&ntv2_inp->monitor_timer,
				ntv2_input_monitor,
				(unsigned long)ntv2_inp);

	spin_lock_init(&ntv2_inp->state_lock);

	NTV2_MSG_INPUT_INFO("%s: open ntv2_input\n", ntv2_inp->name);

	return ntv2_inp;
}

void ntv2_input_close(struct ntv2_input *ntv2_inp)
{
	int i;

	if (ntv2_inp == NULL) 
		return;

	NTV2_MSG_INPUT_INFO("%s: close ntv2_input\n", ntv2_inp->name);

	ntv2_input_disable(ntv2_inp);

	for (i = 0; i < ntv2_inp->num_hdmi_inputs; i++) {
		ntv2_hdmiin_close(ntv2_inp->hdmi_inputs[i]);
	}

	memset(ntv2_inp, 0, sizeof(struct ntv2_input));
	kfree(ntv2_inp);
}

int ntv2_input_configure(struct ntv2_input *ntv2_inp,
						 struct ntv2_features *features,
						 struct ntv2_register *vid_reg)
{
	int i;
	int result;

	if ((ntv2_inp == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	ntv2_inp->features = features;
	ntv2_inp->vid_reg = vid_reg;
	ntv2_inp->num_sdi_inputs = features->num_sdi_inputs;
	ntv2_inp->num_hdmi_inputs = features->num_hdmi_inputs;
	ntv2_inp->num_aes_inputs = features->num_aes_inputs;

	for (i = 0; i < ntv2_inp->num_hdmi_inputs; i++) {
		ntv2_inp->hdmi_inputs[i] = ntv2_hdmiin_open((struct ntv2_object*)ntv2_inp, "hin", i); 
		if (ntv2_inp->hdmi_inputs[i] == NULL)
			return -ENOMEM;
		result = ntv2_hdmiin_configure(ntv2_inp->hdmi_inputs[i],
									   ntv2_inp->features,
									   ntv2_inp->vid_reg);
		if (result < 0)
			return result;
	}

	return 0;
}

int ntv2_input_enable(struct ntv2_input *ntv2_inp)
{
	unsigned long flags;
	int i;

	if (ntv2_inp == NULL)
		return -EPERM;

	if (ntv2_inp->monitor_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_INPUT_STATE("%s: input monitor task enable\n", ntv2_inp->name);

	spin_lock_irqsave(&ntv2_inp->state_lock, flags);
	ntv2_inp->monitor_state = ntv2_task_state_enable;
	spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);

	/* start the device monitor */
	mod_timer(&ntv2_inp->monitor_timer, jiffies +
			  usecs_to_jiffies(NTV2_INPUT_MONITOR_INTERVAL));

	/* enable the hdmi input monitors */
	for (i = 0; i < ntv2_inp->num_hdmi_inputs; i++) {
		ntv2_hdmiin_enable(ntv2_inp->hdmi_inputs[i]);
	}

	return 0;
}

int ntv2_input_disable(struct ntv2_input *ntv2_inp)
{
	unsigned long flags;
	int i;

	if (ntv2_inp == NULL)
		return -EPERM;
	
	if (ntv2_inp->monitor_state != ntv2_task_state_enable)
		return 0;

	NTV2_MSG_INPUT_STATE("%s: input monitor task disable\n", ntv2_inp->name);

	/* disable the hdmi input monitors */
	for (i = 0; i < ntv2_inp->num_hdmi_inputs; i++) {
		ntv2_hdmiin_disable(ntv2_inp->hdmi_inputs[i]);
	}

	/* stop the device monitor */
	del_timer_sync(&ntv2_inp->monitor_timer);

	spin_lock_irqsave(&ntv2_inp->state_lock, flags);
	ntv2_inp->monitor_state = ntv2_task_state_disable;
	memset(&ntv2_inp->sdi_input_state, 0, NTV2_MAX_SDI_COMPONENTS*sizeof(struct ntv2_sdi_input_state));
	spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);

	return 0;
}

int ntv2_input_get_input_format(struct ntv2_input *ntv2_inp,
								struct ntv2_input_config *config,
								struct ntv2_input_format *format)
{
	struct ntv2_sdi_input_status status[4];
	struct ntv2_hdmiin_format hdmi_format;
	unsigned long flags;
	int result = -EINVAL;
	int i;

	if ((ntv2_inp == NULL) ||
		(config == NULL) ||
		(format == NULL))
		return -EPERM;

	format->type = config->type;
	format->input_index = config->input_index;
	format->num_inputs = config->num_inputs;

	if (config->type == ntv2_input_type_sdi) {

		/* validate config parameters */
		if ((config->input_index >= ntv2_inp->num_sdi_inputs) ||
			(config->num_inputs < 1) ||
			(config->num_inputs > 4) ||
			((config->input_index + config->num_inputs) > ntv2_inp->num_sdi_inputs))
			goto done;

		/* get current sdi input status */
		spin_lock_irqsave(&ntv2_inp->state_lock, flags);
		for (i = 0; i < config->num_inputs; i++) {
			if (!ntv2_inp->sdi_input_state[config->input_index + i].locked) {
				spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);
				goto done;
			}
			status[i] = ntv2_inp->sdi_input_state[config->input_index + i].lock_status;
		}
		spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);

		/* sdi input status must be identical for multiple inputs */
		for (i = 1; i < config->num_inputs; i++) {
			if (!ntv2_compare_sdi_input_status(&status[0], &status[i]))
				goto done;
		}

		/* convert sdi input status to input format */
		if (config->num_inputs == 1) {
			result = ntv2_sdi_single_stream_to_format(&status[0], format);
		} if (config->num_inputs == 2) {
			result = ntv2_sdi_dual_stream_to_format(&status[0], format);
		} if (config->num_inputs == 4) {
			result = ntv2_sdi_quad_stream_to_format(&status[0], format);
		}
	}

	if (config->type == ntv2_input_type_hdmi) {

		/* validate config parameters */
		if ((config->input_index >= ntv2_inp->num_hdmi_inputs) ||
			(config->num_inputs != 1))
			goto done;

		/* get current hdmi input format */
		ntv2_hdmiin_get_input_format(ntv2_inp->hdmi_inputs[config->input_index], &hdmi_format);

		format->type = config->type;
		format->video_standard = hdmi_format.video_standard;
		format->frame_rate = hdmi_format.frame_rate;
		format->frame_flags = hdmi_format.frame_flags;
		format->pixel_flags = hdmi_format.pixel_flags;

		/* fpga converts 4k hdmi to square division */
		if (format->video_standard == ntv2_kona_video_standard_3840x2160p) {
			format->video_standard = ntv2_kona_video_standard_1080p;
			format->num_streams = 4;
			format->frame_flags |= ntv2_kona_frame_square_division;
		} else if (format->video_standard == ntv2_kona_video_standard_4096x2160p) {
			format->video_standard = ntv2_kona_video_standard_2048x1080p;
			format->num_streams = 4;
			format->frame_flags |= ntv2_kona_frame_square_division;
		} else {
			format->num_streams = 1;
		}
		result = 0;
	}

done:
	if (result < 0) {
		memset(format, 0, sizeof(struct ntv2_input_format));
	}

	return result;
}

int ntv2_input_get_source_format(struct ntv2_input *ntv2_inp,
								 struct ntv2_source_config *config,
								 struct ntv2_source_format *format)
{
	struct ntv2_sdi_input_status status[4];
	struct ntv2_hdmiin_format hdmi_format;
	struct ntv2_aes_input_status aes_status;
	unsigned long flags;
	int result = -EINVAL;
	int i;

	if ((ntv2_inp == NULL) ||
		(config == NULL) ||
		(format == NULL))
		return -EPERM;

	format->type = config->type;
	format->audio_source = config->audio_source;
	format->input_index = config->input_index;
	format->num_inputs = config->num_inputs;
	format->audio_detect = 0;

	if (config->type == ntv2_input_type_sdi) {

		/* validate config parameters */
		if ((config->input_index >= ntv2_inp->num_sdi_inputs) ||
			(config->num_inputs < 1) ||
			(config->num_inputs > 4) ||
			((config->input_index + config->num_inputs) > ntv2_inp->num_sdi_inputs))
			goto done;

		/* get current sdi input status */
		spin_lock_irqsave(&ntv2_inp->state_lock, flags);
		for (i = 0; i < config->num_inputs; i++) {
			if (!ntv2_inp->sdi_input_state[config->input_index + i].locked) {
				spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);
				goto done;
			}
			status[i] = ntv2_inp->sdi_input_state[config->input_index + i].lock_status;
		}
		spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);

		/* sdi input status must be identical for multiple inputs */
		for (i = 1; i < config->num_inputs; i++) {
			if (!ntv2_compare_sdi_input_status(&status[0], &status[i]))
				goto done;
		}

		/* set audio detection bits */
		for (i = 0; i < config->num_inputs; i++) {
			format->audio_detect |= (status[i].audio_detect & 0xff) << i*8;
		}

		result = 0;
	}

	if (config->type == ntv2_input_type_hdmi) {

		/* validate config parameters */
		if ((config->input_index >= ntv2_inp->num_hdmi_inputs) ||
			(config->num_inputs != 1))
			goto done;

		/* get current hdmi input format */
		ntv2_hdmiin_get_input_format(ntv2_inp->hdmi_inputs[config->input_index], &hdmi_format);

		/* set audio detection bits */
		format->audio_detect = hdmi_format.audio_detect;

		result = 0;
	}

	if (config->type == ntv2_input_type_aes) {

		/* validate config parameters */
		if ((config->input_index >= ntv2_inp->num_aes_inputs) ||
			(config->num_inputs != 1))
			goto done;

		/* get current aes input status */
		ntv2_read_aes_input_status(ntv2_inp->vid_reg, config->input_index, &aes_status);

		/* set audio detection bits */
		format->audio_detect = aes_status.audio_detect;

		result = 0;
	}

done:
	if (result < 0) {
		memset(format, 0, sizeof(struct ntv2_input_format));
	}

	return result;
}

static void ntv2_input_monitor(unsigned long data)
{
	struct ntv2_input *ntv2_inp = (struct ntv2_input *)data;
	struct ntv2_sdi_input_state *state;
	struct ntv2_sdi_input_status input;
	unsigned long flags;
	bool unlock;
	bool lock;
	int i;

	if (ntv2_inp == NULL)
		return;

	for (i = 0; i < ntv2_inp->num_sdi_inputs; i++) {
		state = &ntv2_inp->sdi_input_state[i];
		unlock = false;
		lock = false;

		/* get sdi input status from hardware */
		ntv2_read_sdi_input_status(ntv2_inp->vid_reg, i, &input);
/*
		NTV2_MSG_INPUT_STATE("%s: sdi input %d status  %d %d %d %d %d %08x %08x\n",
							 ntv2_inp->name, i,
							 input.frame_rate,
							 input.input_geometry,
							 (int)input.progressive,
							 (int)input.is3g,
							 (int)input.is3gb,
							 input.vpid_ds1,
							 input.vpid_ds2);
*/
		/* check for unlock */
		if (!ntv2_compare_sdi_input_status(&input, &state->lock_status)) {
			if (state->unlock_count < NTV2_INPUT_UNLOCK_COUNT) {
				state->unlock_count++;
				unlock = state->unlock_count >= NTV2_INPUT_UNLOCK_COUNT;
			}
		} else {
			state->unlock_count = 0;
		}

		/* check for new lock */
		if (ntv2_compare_sdi_input_status(&input, &state->last_status)) {
			if (state->lock_count < NTV2_INPUT_LOCK_COUNT) {
				state->lock_count++;
				lock = state->lock_count >= NTV2_INPUT_LOCK_COUNT;
			}
		} else {
			state->lock_count = 0;
		}

		/* save for next time */
		state->last_status = input;

		/* update lock */
		spin_lock_irqsave(&ntv2_inp->state_lock, flags);

		if (state->locked && unlock) {
			state->locked = false;
			state->changed = true;
			NTV2_MSG_INPUT_STATE("%s: sdi input %d unlocked\n", ntv2_inp->name, i);
		}
		if (!state->locked && lock) {
			state->locked = true;
			state->changed = true;
			state->lock_status = input;
			NTV2_MSG_INPUT_STATE("%s: sdi input %d locked  %s%s @ %s fps\n",
								 ntv2_inp->name, i,
								 ntv2_input_geometry_name(state->lock_status.input_geometry),
								 ((state->lock_status.progressive != 0)?"p":"i"),
								 ntv2_frame_rate_name(state->lock_status.frame_rate));
		}

		spin_unlock_irqrestore(&ntv2_inp->state_lock, flags);
	}
	
	/* restart timer */
	mod_timer(&ntv2_inp->monitor_timer, jiffies +
			  usecs_to_jiffies(NTV2_INPUT_MONITOR_INTERVAL));
}

static bool ntv2_compare_sdi_input_status(struct ntv2_sdi_input_status *status_a,
										  struct ntv2_sdi_input_status *status_b)
{
	bool match;

	if ((status_a == NULL) ||
		(status_b == NULL))
		return false;

	if ((status_a->frame_rate == ntv2_kona_frame_rate_none) ||
		(status_b->frame_rate == ntv2_kona_frame_rate_none))
		return false;

	match =
		(status_a->frame_rate == status_b->frame_rate) &&
		(status_a->input_geometry == status_b->input_geometry) &&
		(status_a->progressive == status_b->progressive) &&
		(status_a->is3g == status_b->is3g) &&
		(status_a->is3gb == status_b->is3gb);

	return match;
}

static int ntv2_sdi_single_stream_to_format(struct ntv2_sdi_input_status *status,
											struct ntv2_input_format *inpf)
{
	u32 standard = ntv2_kona_video_standard_none;
	u32 rate = ntv2_kona_frame_rate_none;
	u32 frame_flags = 0;
	u32 pixel_flags = 0;

	/* verify sdi format */
	if (status->input_geometry == ntv2_kona_input_geometry_525) {
		if (status->progressive || 
			(status->frame_rate != ntv2_kona_frame_rate_2997))
			goto bad_status;
		standard = ntv2_kona_video_standard_525i;
		rate = status->frame_rate;
		frame_flags |= ntv2_kona_frame_sd |
			ntv2_kona_frame_picture_interlaced |
			ntv2_kona_frame_transport_interlaced |
			ntv2_kona_frame_4x3;
		pixel_flags |= ntv2_kona_pixel_yuv |
			ntv2_kona_pixel_rec601 |
			ntv2_kona_pixel_422 |
			ntv2_kona_pixel_smpte |
			ntv2_kona_pixel_10bit;

	} else if (status->input_geometry == ntv2_kona_input_geometry_625) {
		if (status->progressive || 
			(status->frame_rate != ntv2_kona_frame_rate_2500))
			goto bad_status;
		standard = ntv2_kona_video_standard_625i;
		rate = status->frame_rate;
		frame_flags |= ntv2_kona_frame_sd |
			ntv2_kona_frame_picture_interlaced |
			ntv2_kona_frame_transport_interlaced |
			ntv2_kona_frame_4x3;
		pixel_flags |= ntv2_kona_pixel_yuv |
			ntv2_kona_pixel_rec601 |
			ntv2_kona_pixel_422 |
			ntv2_kona_pixel_smpte |
			ntv2_kona_pixel_10bit;

	} else if (status->input_geometry == ntv2_kona_input_geometry_750) {
		if (!status->progressive ||
			((status->frame_rate != ntv2_kona_frame_rate_5000) &&
			 (status->frame_rate != ntv2_kona_frame_rate_5994) &&
			 (status->frame_rate != ntv2_kona_frame_rate_6000)))
			goto bad_status;
		standard = ntv2_kona_video_standard_720p;
		rate = status->frame_rate;
		frame_flags |= ntv2_kona_frame_hd |
			ntv2_kona_frame_picture_progressive |
			ntv2_kona_frame_transport_progressive |
			ntv2_kona_frame_16x9;
		pixel_flags |= ntv2_kona_pixel_yuv |
			ntv2_kona_pixel_rec709 |
			ntv2_kona_pixel_422 |
			ntv2_kona_pixel_smpte |
			ntv2_kona_pixel_10bit;

	} else if (status->input_geometry == ntv2_kona_input_geometry_1125) {
		if (status->progressive) {
			if (status->is3gb)
				goto bad_status;
			standard = ntv2_kona_video_standard_1080p;
			rate = status->frame_rate;
			frame_flags |= ntv2_kona_frame_picture_progressive |
				ntv2_kona_frame_transport_progressive |
				ntv2_kona_frame_16x9;
			if (status->is3g) {
				frame_flags |= ntv2_kona_frame_3g | ntv2_kona_frame_3ga;
			} else {
				frame_flags |= ntv2_kona_frame_hd;
			}
		} else {
			if (status->is3g) {
				if (!status->is3gb)
					goto bad_status;
				standard = ntv2_kona_video_standard_1080p;
				if (status->frame_rate == ntv2_kona_frame_rate_2500) {
					rate = ntv2_kona_frame_rate_5000;
				} else if (status->frame_rate == ntv2_kona_frame_rate_2997) {
					rate = ntv2_kona_frame_rate_5994;
				} else if (status->frame_rate == ntv2_kona_frame_rate_3000) {
					rate = ntv2_kona_frame_rate_6000;
				} else {
					goto bad_status;
				}
				frame_flags |= ntv2_kona_frame_3g |
					ntv2_kona_frame_3gb |
					ntv2_kona_frame_picture_progressive |
					ntv2_kona_frame_transport_interlaced |
					ntv2_kona_frame_line_interleaved |
					ntv2_kona_frame_16x9;
			} else {
				if ((status->frame_rate != ntv2_kona_frame_rate_2500) &&
					(status->frame_rate != ntv2_kona_frame_rate_2997) &&
					(status->frame_rate != ntv2_kona_frame_rate_3000))
					goto bad_status;
				standard = ntv2_kona_video_standard_1080i;
				rate = status->frame_rate;
				frame_flags = ntv2_kona_frame_hd |
					ntv2_kona_frame_picture_interlaced |
					ntv2_kona_frame_transport_interlaced |
					ntv2_kona_frame_16x9;
			}
		}
		pixel_flags |= ntv2_kona_pixel_yuv |
			ntv2_kona_pixel_rec709 |
			ntv2_kona_pixel_422 |
			ntv2_kona_pixel_smpte |
			ntv2_kona_pixel_10bit;

	} else {
		goto bad_status;
	}

	inpf->video_standard = standard;
	inpf->frame_rate = rate;
	inpf->frame_flags = frame_flags;
	inpf->pixel_flags = pixel_flags;
	inpf->num_streams = 1;

	return 0;

bad_status:
	memset(inpf, 0, sizeof(struct ntv2_input_format));

	return -EINVAL;
}

static int ntv2_sdi_dual_stream_to_format(struct ntv2_sdi_input_status *status,
										  struct ntv2_input_format *inpf)
{
	u32 standard = ntv2_kona_video_standard_1080p;
	u32 rate = ntv2_kona_frame_rate_none;
	u32 frame_flags = 0;
	u32 pixel_flags = 0;

	if (status->input_geometry != ntv2_kona_input_geometry_1125)
		goto bad_status;

	if (status->progressive) {
		goto bad_status;
	} else {
		if (status->is3g)
			goto bad_status;
		if (status->frame_rate == ntv2_kona_frame_rate_2500) {
			rate = ntv2_kona_frame_rate_5000;
		} else if (status->frame_rate == ntv2_kona_frame_rate_2997) {
			rate = ntv2_kona_frame_rate_5994;
		} else if (status->frame_rate == ntv2_kona_frame_rate_3000) {
			rate = ntv2_kona_frame_rate_6000;
		} else {
			goto bad_status;
		}
	}

	frame_flags |= ntv2_kona_frame_hd |
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_transport_interlaced |
		ntv2_kona_frame_line_interleaved |
		ntv2_kona_frame_16x9;
	pixel_flags |= ntv2_kona_pixel_yuv |
		ntv2_kona_pixel_rec709 |
		ntv2_kona_pixel_422 |
		ntv2_kona_pixel_smpte |
		ntv2_kona_pixel_10bit;

	inpf->video_standard = standard;
	inpf->frame_rate = rate;
	inpf->frame_flags = frame_flags;
	inpf->pixel_flags = pixel_flags;
	inpf->num_streams = 2;

	return 0;

bad_status:
	memset(inpf, 0, sizeof(struct ntv2_input_format));

	return -EINVAL;
}

static int ntv2_sdi_quad_stream_to_format(struct ntv2_sdi_input_status *status,
										  struct ntv2_input_format *inpf)
{
	u32 standard = ntv2_kona_video_standard_1080p;
	u32 rate = ntv2_kona_frame_rate_none;
	u32 frame_flags = 0;
	u32 pixel_flags = 0;

	if (status->input_geometry != ntv2_kona_input_geometry_1125)
		goto bad_status;

	if (status->progressive) {
		if (status->is3gb)
			goto bad_status;
		rate = status->frame_rate;
		if (status->is3g) {
			frame_flags |= ntv2_kona_frame_3g | ntv2_kona_frame_3ga;
		} else {
			frame_flags |= ntv2_kona_frame_hd;
		}
		frame_flags |= ntv2_kona_frame_picture_progressive |
			ntv2_kona_frame_transport_progressive;
	} else {
		if (!status->is3gb)
			goto bad_status;
		if (status->frame_rate == ntv2_kona_frame_rate_2500) {
			rate = ntv2_kona_frame_rate_5000;
		} else if (status->frame_rate == ntv2_kona_frame_rate_2997) {
			rate = ntv2_kona_frame_rate_5994;
		} else if (status->frame_rate == ntv2_kona_frame_rate_3000) {
			rate = ntv2_kona_frame_rate_6000;
		} else {
			goto bad_status;
		}
		frame_flags |= ntv2_kona_frame_3g |
			ntv2_kona_frame_3gb |
			ntv2_kona_frame_picture_progressive |
			ntv2_kona_frame_transport_interlaced |
			ntv2_kona_frame_line_interleaved |
			ntv2_kona_frame_16x9;
	}

	frame_flags |= ntv2_kona_frame_square_division;
	pixel_flags |= ntv2_kona_pixel_yuv |
		ntv2_kona_pixel_rec709 |
		ntv2_kona_pixel_422 |
		ntv2_kona_pixel_smpte |
		ntv2_kona_pixel_10bit;

	inpf->video_standard = standard;
	inpf->frame_rate = rate;
	inpf->frame_flags = frame_flags;
	inpf->pixel_flags = pixel_flags;
	inpf->num_streams = 4;

	return 0;

bad_status:
	memset(inpf, 0, sizeof(struct ntv2_input_format));

	return -EINVAL;
}
