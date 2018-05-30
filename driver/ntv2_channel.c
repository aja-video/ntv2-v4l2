/*
 * NTV2 hardware channel interface
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

#include "ntv2_channel.h"
#include "ntv2_features.h"
#include "ntv2_register.h"
#include "ntv2_konareg.h"
#include "ntv2_videoops.h"
#include "ntv2_audioops.h"


static int ntv2_streamops_nop(struct ntv2_channel_stream *stream);
static void ntv2_streamops_initialize(struct ntv2_stream_ops *ops);
static void ntv2_channel_dpc(unsigned long data);

struct ntv2_channel *ntv2_channel_open(struct ntv2_object *ntv2_obj,
									   const char *name, int index)
{
	struct ntv2_channel *ntv2_chn = NULL;

	ntv2_chn = kzalloc(sizeof(struct ntv2_channel), GFP_KERNEL);
	if (ntv2_chn == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_channel instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_chn->index = index;
	snprintf(ntv2_chn->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_chn->list);
	ntv2_chn->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_chn->state_lock);
	spin_lock_init(&ntv2_chn->int_lock);

	tasklet_init(&ntv2_chn->int_dpc,
				 ntv2_channel_dpc,
				 (unsigned long)ntv2_chn);

	NTV2_MSG_CHANNEL_INFO("%s: open ntv2_channel\n", ntv2_chn->name);

	return ntv2_chn;
}

void ntv2_channel_close(struct ntv2_channel *ntv2_chn)
{
	int i;

	if (ntv2_chn == NULL)
		return;

	NTV2_MSG_CHANNEL_INFO("%s: close ntv2_channel\n", ntv2_chn->name);

	ntv2_channel_disable_all(ntv2_chn);

	tasklet_kill(&ntv2_chn->int_dpc);

	for (i = 0; i < ntv2_stream_type_size; i++) {
		if (ntv2_chn->streams[i] != NULL)
			kfree(ntv2_chn->streams[i]);
	}

	memset(ntv2_chn, 0, sizeof(struct ntv2_channel));
	kfree(ntv2_chn);
}

int ntv2_channel_configure(struct ntv2_channel *ntv2_chn,
						   struct ntv2_features *features,
						   struct ntv2_register *vid_reg)
{
	struct ntv2_channel_stream *stream;
	if ((ntv2_chn == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_CHANNEL_INFO("%s: configure hardware channel\n", ntv2_chn->name);

	ntv2_chn->features = features;
	ntv2_chn->vid_reg = vid_reg;

	stream = kzalloc(sizeof(struct ntv2_channel_stream), GFP_KERNEL);
	if (stream == NULL)
		return -ENOMEM;

	/* configure the video input stream */
	stream->type = ntv2_stream_type_vidin;
	stream->ntv2_chn = ntv2_chn;
	stream->capture = true;
	ntv2_streamops_initialize(&stream->ops);
	stream->ops.setup = ntv2_videoops_setup_capture;
	stream->ops.release = ntv2_videoops_release_capture;
	stream->ops.update_mode = ntv2_videoops_update_mode;
	stream->ops.update_timing = ntv2_videoops_update_timing;
	stream->ops.update_format = ntv2_videoops_update_format;
	stream->ops.update_route = ntv2_videoops_update_route;
	stream->ops.interrupt = ntv2_videoops_interrupt_capture;
	stream->video.video_format = *ntv2_features_get_default_video_format(features, ntv2_chn->index);
	stream->video.pixel_format = *ntv2_features_get_default_pixel_format(features, ntv2_chn->index);
	ntv2_features_gen_input_format(ntv2_features_get_default_input_config(features, ntv2_chn->index),
								   &stream->video.video_format,
								   &stream->video.pixel_format,
								   &stream->video.input_format);
	ntv2_chn->streams[ntv2_stream_type_vidin] = stream;

	/* initialize the video input hardware */
	stream->ops.setup(stream);
	stream->ops.update_mode(stream);
	stream->ops.update_format(stream);
	stream->ops.update_timing(stream);
	stream->ops.update_route(stream);
	stream->ops.release(stream);

	stream = kzalloc(sizeof(struct ntv2_channel_stream), GFP_KERNEL);
	if (stream == NULL)
		return -ENOMEM;

	/* configure the audio input stream */
	stream->type = ntv2_stream_type_audin;
	stream->ntv2_chn = ntv2_chn;
	stream->capture = true;
	ntv2_streamops_initialize(&stream->ops);
	stream->ops.setup = ntv2_audioops_setup_capture;
	stream->ops.update_mode = ntv2_audioops_update_mode;
	stream->ops.update_route = ntv2_audioops_update_route;
	stream->ops.interrupt = ntv2_audioops_interrupt_capture;
	ntv2_features_gen_source_format(
		ntv2_features_get_default_source_config(features, ntv2_chn->index, true),
		&stream->audio.source_format);
	stream->audio.auto_format = stream->audio.source_format;
	ntv2_chn->streams[ntv2_stream_type_audin] = stream;

	/* initialize the audio input hardware */
	stream->ops.setup(stream);
	stream->ops.update_mode(stream);
	stream->ops.update_format(stream);
	stream->ops.update_timing(stream);
	stream->ops.update_route(stream);
	stream->ops.release(stream);

	NTV2_MSG_CHANNEL_STATE("%s: channel state: idle\n", ntv2_chn->name);
	ntv2_chn->state = ntv2_channel_state_idle;

	return 0;
}

void ntv2_channel_disable_all(struct ntv2_channel *ntv2_chn)
{
	int i;

	if (ntv2_chn == NULL)
		return;

	/* disable streams */
	for (i = 0; i < ntv2_stream_type_size; i++) {
		ntv2_channel_disable(ntv2_chn->streams[i]);
	}
}

struct ntv2_channel_stream *ntv2_channel_stream(struct ntv2_channel *ntv2_chn,
												enum ntv2_stream_type stype)
{
	if (ntv2_chn == NULL)
		return NULL;

	return ntv2_chn->streams[stype];
}

int ntv2_channel_set_video_format(struct ntv2_channel_stream *stream,
								  struct ntv2_video_format *vidf)
{
	unsigned long flags;

	if ((stream == NULL) || (vidf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	stream->video.video_format = *vidf;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_get_video_format(struct ntv2_channel_stream *stream,
								  struct ntv2_video_format *vidf)
{
	unsigned long flags;

	if ((stream == NULL) || (vidf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	*vidf = stream->video.video_format;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_set_pixel_format(struct ntv2_channel_stream *stream,
								  struct ntv2_pixel_format *pixf)
{
	unsigned long flags;

	if ((stream == NULL) || (pixf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	stream->video.pixel_format = *pixf;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_get_pixel_format(struct ntv2_channel_stream *stream,
								  struct ntv2_pixel_format *pixf)
{
	unsigned long flags;

	if ((stream == NULL) || (pixf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	*pixf = stream->video.pixel_format;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_set_input_format(struct ntv2_channel_stream *stream,
								  struct ntv2_input_format *inpf)
{
	unsigned long flags;

	if ((stream == NULL) || (inpf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	stream->video.input_format = *inpf;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_get_input_format(struct ntv2_channel_stream *stream,
								  struct ntv2_input_format *inpf)
{
	unsigned long flags;

	if ((stream == NULL) || (inpf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	*inpf = stream->video.input_format;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_set_source_format(struct ntv2_channel_stream *stream,
								   struct ntv2_source_format *souf)
{
	unsigned long flags;

	if ((stream == NULL) || (souf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	stream->audio.source_format = *souf;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_get_source_format(struct ntv2_channel_stream *stream,
								   struct ntv2_source_format *souf)
{
	unsigned long flags;

	if ((stream == NULL) || (souf == NULL))
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	*souf = stream->audio.source_format;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_set_frame_callback(struct ntv2_channel_stream *stream,
									ntv2_channel_callback func,
									unsigned long data)
{
	unsigned long flags;

	if (stream == NULL)
		return -EPERM;

	spin_lock_irqsave(&stream->ntv2_chn->state_lock, flags);
	stream->frame_callback_func = func;
	stream->frame_callback_data = data;
	spin_unlock_irqrestore(&stream->ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_enable(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	unsigned long flags;
	int result;

	if (stream == NULL)
		return -EPERM;

	ntv2_chn = stream->ntv2_chn;

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);

	if (stream->queue_enable) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return 0;
	}

	NTV2_MSG_CHANNEL_STATE("%s: %s state: enable\n",
						   ntv2_chn->name, ntv2_stream_name(stream->type));

	/* enable stream */
	stream->queue_enable = true;
	result = stream->ops.setup(stream);
	if (result != 0) {
		stream->queue_enable = false;
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		NTV2_MSG_VIDEO_ERROR("%s: *error* %s can not acquire hardware\n", 
							 ntv2_chn->name, ntv2_stream_name(stream->type));
		return result;
	}
	stream->ops.update_format(stream);
	stream->ops.update_timing(stream);
	stream->ops.update_route(stream);
	stream->ops.update_mode(stream);

	/* enable vertical interrupts */
	ntv2_video_input_interrupt_enable(ntv2_chn->vid_reg, ntv2_chn->index, true);
	ntv2_video_output_interrupt_enable(ntv2_chn->vid_reg, ntv2_chn->index, true);
	if (ntv2_chn->state == ntv2_channel_state_idle) {
		NTV2_MSG_CHANNEL_STATE("%s: engine state: run\n", ntv2_chn->name);
		ntv2_chn->state = ntv2_channel_state_run;
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_disable(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	unsigned long flags;
	int		i;

	if (stream == NULL)
		return -EPERM;

	ntv2_chn = stream->ntv2_chn;

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);
	
	if (!stream->queue_enable) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return 0;
	}

	NTV2_MSG_CHANNEL_STATE("%s: %s state: disable\n",
						   ntv2_chn->name, ntv2_stream_name(stream->type));

	/* disable stream */
	stream->queue_run = false;
	stream->queue_enable = false;
	stream->ops.update_mode(stream);
	stream->ops.release(stream);
	
	/* disable vertical interrupts if no streams enabled*/
	for (i = 0; i < ntv2_stream_type_size; i++) {
		if ((ntv2_chn->streams[i] != NULL) &&
			(ntv2_chn->streams[i]->queue_enable))
			break;
	}
	if (i == ntv2_stream_type_size) {
		ntv2_video_input_interrupt_enable(ntv2_chn->vid_reg, ntv2_chn->index, false);
		ntv2_video_output_interrupt_enable(ntv2_chn->vid_reg, ntv2_chn->index, false);
		NTV2_MSG_CHANNEL_STATE("%s: engine state: idle\n", ntv2_chn->name);
		ntv2_chn->state = ntv2_channel_state_idle;
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_start(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	unsigned long flags;

	if (stream == NULL)
		return -EPERM;

	ntv2_chn = stream->ntv2_chn;

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);

	if (!stream->queue_enable) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return -EINVAL;
	}

	if (!stream->queue_run) {
		NTV2_MSG_CHANNEL_STATE("%s: %s stream start\n",
							   ntv2_chn->name,
							   ntv2_stream_name(stream->type));
		stream->queue_run = true;
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_stop(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	unsigned long flags;

	if (stream == NULL)
		return -EPERM;

	ntv2_chn = stream->ntv2_chn;

	if (!stream->queue_run)
		return 0;

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);

	if (!stream->queue_enable) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return -EINVAL;
	}

	if (stream->queue_run) {
		NTV2_MSG_CHANNEL_STATE("%s: %s stream stop\n",
							   ntv2_chn->name,
							   ntv2_stream_name(stream->type));
		stream->queue_run = false;
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	return 0;
}

int ntv2_channel_flush(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	struct list_head *ptr;
	struct list_head *next;
	unsigned long flags;

	if (stream == NULL)
		return -EPERM;

	ntv2_chn = stream->ntv2_chn;

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);

	if (!stream->queue_enable) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return -EINVAL;
	}

	NTV2_MSG_CHANNEL_STATE("%s: %s stream flush\n",
						   ntv2_chn->name,
						   ntv2_stream_name(stream->type));

	/* flush queued data */
	list_for_each_safe(ptr, next, &stream->data_ready_list) {
		list_del_init(ptr);
		list_add_tail(ptr, &stream->data_done_list);
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	return 0;
}

struct ntv2_stream_data *ntv2_channel_data_ready(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn;
	struct ntv2_stream_data *data = NULL;
	unsigned long flags;

	if (stream == NULL)
		return NULL;

	ntv2_chn = stream->ntv2_chn;

	/* get the next captured buffer */
	spin_lock_irqsave(&ntv2_chn->state_lock, flags);
	if (stream->queue_enable && !list_empty(&stream->data_ready_list)) {
		data = list_first_entry(&stream->data_ready_list, struct ntv2_stream_data, list);
		list_del_init(&data->list);
	}
	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	if (data != NULL) {
		NTV2_MSG_CHANNEL_STREAM("%s: %s data ready %d\n",
								ntv2_chn->name,
								ntv2_stream_name(stream->type),
								data->index);
	}

	return data;
}

void ntv2_channel_data_done(struct ntv2_stream_data *ntv2_data)
{
	struct ntv2_channel_stream *stream;
	struct ntv2_channel *ntv2_chn;
	unsigned long flags;

	if (ntv2_data == NULL)
		return;

	stream = ntv2_data->ntv2_str;
	if (stream == NULL)
		return;

	ntv2_chn = stream->ntv2_chn;
	if (ntv2_chn == NULL)
		return;

	NTV2_MSG_CHANNEL_STREAM("%s: %s data done %d\n",
							ntv2_chn->name,
							ntv2_stream_name(stream->type),
							ntv2_data->index);

	/* buffer done */
	spin_lock_irqsave(&ntv2_chn->state_lock, flags);
	if (stream->queue_enable) {
		list_add_tail(&ntv2_data->list, &stream->data_done_list);
	}
	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
}

int ntv2_channel_interrupt(struct ntv2_channel *ntv2_chn,
						   struct ntv2_interrupt_status* irq_status)
{
	int index;
	bool input = false;
	bool output = false;
	u32 aud_in = 0;
	u32 aud_out = 0;
	int res = IRQ_NONE;
	unsigned long flags;

	if ((ntv2_chn == NULL) ||
		(irq_status == NULL))
		return IRQ_NONE;

	index = ntv2_chn->index;

	/* is interrupt active */
	if (ntv2_video_input_interrupt_active(irq_status, index)) {
		/* clear interrupt */
		ntv2_video_input_interrupt_clear(ntv2_chn->vid_reg, index);
		res = IRQ_HANDLED;
		/* check field flag */
		if (ntv2_video_input_field_id(irq_status, index) == 0) {
			input = true;
		}
	}
	if (ntv2_video_output_interrupt_active(irq_status, index)) {
		/* clear interrupt */
		ntv2_video_output_interrupt_clear(ntv2_chn->vid_reg, index);
		res = IRQ_HANDLED;
		/* check field flag */
		if (ntv2_video_output_field_id(irq_status, index) == 0) {
			output = true;
		}
	}

	if (!input && !output)
		return res;

	aud_in = ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_audio_input_address, index);
	aud_out = ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_audio_output_address, index);

	/* timestamp for frame */
	spin_lock_irqsave(&ntv2_chn->int_lock, flags);
	ntv2_chn->int_status.interrupt_input = ntv2_chn->int_status.interrupt_input || input;
	ntv2_chn->int_status.interrupt_output = ntv2_chn->int_status.interrupt_output || output;
	ntv2_chn->int_status.interrupt_time = irq_status->v4l2_time;
	ntv2_chn->int_status.audio_input_offset = aud_in;
	ntv2_chn->int_status.audio_output_offset = aud_out;
	spin_unlock_irqrestore(&ntv2_chn->int_lock, flags);

	/* schedule the dpc */
	tasklet_schedule(&ntv2_chn->int_dpc);

	return IRQ_HANDLED;
}

void ntv2_channel_dpc(unsigned long data)
{
	struct ntv2_channel *ntv2_chn = (struct ntv2_channel *)data;
	struct ntv2_channel_stream *stream;
	ntv2_channel_callback callback_func;
	unsigned long callback_data;
	unsigned long flags;
	int i;

	if (ntv2_chn == NULL)
		return;

	spin_lock_irqsave(&ntv2_chn->int_lock, flags);
	ntv2_chn->dpc_status = ntv2_chn->int_status;
	ntv2_chn->int_status.interrupt_input = false;
	ntv2_chn->int_status.interrupt_output = false;
	spin_unlock_irqrestore(&ntv2_chn->int_lock, flags);

	ntv2_chn->dpc_status.interrupt_rate = ntv2_video_output_interrupt_rate(ntv2_chn->vid_reg, ntv2_chn->index);
	ntv2_chn->dpc_status.stat_time = ntv2_system_time();

	spin_lock_irqsave(&ntv2_chn->state_lock, flags);

	/* check active state */
	if (ntv2_chn->state != ntv2_channel_state_run) {
		spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
		return;
	}

	/* interrupt streams */
	for (i = 0; i < ntv2_stream_type_size; i++) {
		stream = ntv2_chn->streams[i];
		if ((stream != NULL) && (stream->queue_enable)) {
			stream->ops.interrupt(stream);
		}
	}

	spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);

	/* do callbacks (this can happen once after a disable) */
	for (i = 0; i < ntv2_stream_type_size; i++) {
		stream = ntv2_chn->streams[i];
		if ((stream != NULL) && (stream->queue_enable)) {
			spin_lock_irqsave(&ntv2_chn->state_lock, flags);
			callback_func = stream->frame_callback_func;
			callback_data = stream->frame_callback_data;
			spin_unlock_irqrestore(&ntv2_chn->state_lock, flags);
			if (callback_func != NULL)
				(*callback_func)(callback_data);
		}
	}
}

static int ntv2_streamops_nop(struct ntv2_channel_stream *stream)
{
	return 0;
}

static void ntv2_streamops_initialize(struct ntv2_stream_ops *ops)
{
	ops->setup = ntv2_streamops_nop;
	ops->release = ntv2_streamops_nop;
	ops->update_mode = ntv2_streamops_nop;
	ops->update_timing = ntv2_streamops_nop;
	ops->update_format = ntv2_streamops_nop;
	ops->update_route = ntv2_streamops_nop;
	ops->interrupt = ntv2_streamops_nop;
}
