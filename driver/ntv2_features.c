/*
 * NTV2 device features
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

#include "ntv2_features.h"
#include "ntv2_konareg.h"

static bool ntv2_features_init = false;

static void ntv2_features_initialize(void);
static void ntv2_features_corvid44(struct ntv2_features *features);
static void ntv2_features_corvid88(struct ntv2_features *features);
static void ntv2_features_kona4(struct ntv2_features *features);
static void ntv2_features_corvidhdbt(struct ntv2_features *features);


struct ntv2_features *ntv2_features_open(struct ntv2_object *ntv2_obj,
										 const char *name, int index)
{
	struct ntv2_features *features;

	if (ntv2_obj == NULL) 
		return NULL;

	features = kzalloc(sizeof(struct ntv2_features), GFP_KERNEL);
	if (features == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_feature instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	features->index = index;
	snprintf(features->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&features->list);
	features->ntv2_dev = ntv2_obj->ntv2_dev;
	spin_lock_init(&features->state_lock);

	if (!ntv2_features_init) {
		ntv2_features_initialize();
		ntv2_features_init = true;
	}

	return features;
}

void ntv2_features_close(struct ntv2_features *features)
{
	if (features == NULL)
		return;

	memset(features, 0, sizeof(struct ntv2_features));
	kfree(features);
}

int ntv2_features_configure(struct ntv2_features *features, u32 id)
{
	if (features == NULL)
		return -EPERM;

	switch(id) {
	case 0x10565400: ntv2_features_corvid44(features); break;
	case 0x10538200: ntv2_features_corvid88(features); break;
	case 0X10518400: ntv2_features_kona4(features); break;
	case 0x10668200: ntv2_features_corvidhdbt(features); break;
	default:
		return -ENODEV;
	}

	features->device_id = id;
	return 0;
}

struct ntv2_video_config
*ntv2_features_get_video_config(struct ntv2_features *features,
								int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->video_config[channel_index];
}

struct ntv2_audio_config
*ntv2_features_get_audio_config(struct ntv2_features *features,
								int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->audio_config[channel_index];
}

struct ntv2_input_config
*ntv2_features_get_input_config(struct ntv2_features *features,
								int channel_index,
								int input_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS) ||
		(input_index < 0) || (input_index >= NTV2_MAX_INPUT_CONFIGS))
		return NULL;

	return features->input_config[channel_index][input_index];
}

u32 ntv2_features_num_input_configs(struct ntv2_features *features,
									int channel_index)
{
	int num;

	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return 0;

	for (num = 0; num < NTV2_MAX_INPUT_CONFIGS; num++)
	{
		if (features->input_config[channel_index][num] == NULL)
			break;
	}

	return num;
}

struct ntv2_input_config
*ntv2_features_get_default_input_config(struct ntv2_features *features,
										int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->input_config[channel_index][0];
}

struct ntv2_source_config
*ntv2_features_get_source_config(struct ntv2_features *features,
								int channel_index,
								int source_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS) ||
		(source_index < 0) || (source_index >= NTV2_MAX_SOURCE_CONFIGS))
		return NULL;

	return features->source_config[channel_index][source_index];
}

u32 ntv2_features_num_source_configs(struct ntv2_features *features,
									int channel_index)
{
	int num;

	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return 0;

	for (num = 0; num < NTV2_MAX_SOURCE_CONFIGS; num++)
	{
		if (features->source_config[channel_index][num] == NULL)
			break;
	}

	return num;
}

struct ntv2_source_config
*ntv2_features_get_default_source_config(struct ntv2_features *features,
										 int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->source_config[channel_index][0];
}

struct ntv2_pixel_format
*ntv2_features_get_pixel_format(struct ntv2_features *features,
								int channel_index,
								int format_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS) ||
		(format_index < 0) || (format_index >= NTV2_MAX_PIXEL_FORMATS))
		return NULL;

	return features->pixel_formats[format_index];
}

u32 ntv2_features_num_pixel_formats(struct ntv2_features *features,
									int channel_index)
{
	int num;

	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return 0;

	for (num = 0; num < NTV2_MAX_PIXEL_FORMATS; num++)
	{
		if (features->pixel_formats[num] == NULL)
			break;
	}

	return num;
}

struct ntv2_pixel_format
*ntv2_features_get_default_pixel_format(struct ntv2_features *features,
										int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->pixel_formats[0];
}

struct ntv2_video_format
*ntv2_features_get_video_format(struct ntv2_features *features,
								int channel_index,
								int format_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS) ||
		(format_index < 0) || (format_index >= NTV2_MAX_VIDEO_FORMATS))
		return NULL;

	return features->video_formats[format_index];
}

u32 ntv2_features_num_video_formats(struct ntv2_features *features,
									int channel_index)
{
	int num;

	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return 0;

	for (num = 0; num < NTV2_MAX_VIDEO_FORMATS; num++)
	{
		if (features->video_formats[num] == NULL)
			break;
	}

	return num;
}

struct ntv2_video_format
*ntv2_features_get_default_video_format(struct ntv2_features *features,
										int channel_index)
{
	if ((features == NULL) ||
		(channel_index < 0) || (channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	return features->video_formats[0];
}


struct ntv2_source_config
*ntv2_features_find_source_config(struct ntv2_features *features,
								  int channel_index,
								  enum ntv2_input_type input_type,
								  int input_index)
{
	int i;

	if ((features == NULL) ||
		(channel_index >= NTV2_MAX_CHANNELS))
		return NULL;

	for (i = 0; i < NTV2_MAX_SOURCE_CONFIGS; i++) {
		if (features->source_config[channel_index][i] == NULL)
			break;
		if ((features->source_config[channel_index][i]->type == input_type) &&
			(features->source_config[channel_index][i]->input_index == input_index))
			return features->source_config[channel_index][i];
	}

	return NULL;
}

void ntv2_features_gen_input_format(struct ntv2_input_config *config,
									struct ntv2_video_format *vidf,
									struct ntv2_pixel_format *pixf,
									struct ntv2_input_format *inpf)
{

	if ((config == NULL) ||
		(vidf == NULL) ||
		(pixf == NULL) ||
		(inpf == NULL))
		return;

	inpf->type = config->type;
	inpf->video_standard = vidf->video_standard;
	inpf->frame_rate = vidf->frame_rate;
	inpf->frame_flags = vidf->frame_flags;
	inpf->pixel_flags = pixf->pixel_flags;
	inpf->input_index = config->input_index;
	inpf->num_inputs = config->num_inputs;
	inpf->num_streams = config->num_inputs;

	return;
}

void ntv2_features_gen_source_format(struct ntv2_source_config *config,
									 struct ntv2_source_format *souf)
{
	if ((config == NULL) ||
		(souf == NULL))
		return;

	souf->type = config->type;
	souf->audio_source = config->audio_source;
	souf->input_index = config->input_index;
	souf->num_inputs = config->num_inputs;

	return;
}

u32 ntv2_features_line_pitch(struct ntv2_pixel_format *format, u32 pixels)
{
	u32 width;
	u32 pitch;

	if ((format == NULL) ||
		(format->cadence_pixels == 0) ||
		(format->pitch_alignment == 0))
		return 0;

	width = (pixels + format->cadence_pixels - 1) / format->cadence_pixels * format->cadence_bytes;
	pitch = (width + format->pitch_alignment - 1) / format->pitch_alignment * format->pitch_alignment;

	return pitch;
}

u32 ntv2_features_ntv2_frame_size(struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf)
{
	u32 pitch;

	if ((vidf == NULL) ||
		(pixf == NULL))
		return 0;

	pitch = ntv2_features_line_pitch(pixf, ntv2_frame_geometry_width(vidf->frame_geometry));
	return pitch * ntv2_frame_geometry_height(vidf->frame_geometry);
}

u32 ntv2_features_v4l2_frame_size(struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf)
{
	u32 pitch;

	if ((vidf == NULL) ||
		(pixf == NULL))
		return 0;

	pitch = ntv2_features_line_pitch(pixf, vidf->v4l2_timings.bt.width);
	return pitch * vidf->v4l2_timings.bt.height;
}

int ntv2_features_get_frame_range(struct ntv2_features *features,
								  struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf,
								  u32 index,
								  u32 *first,
								  u32 *last,
								  u32 *size)
{
	u32 fst;
	u32 lst;
	u32 sz;

	if ((features == NULL) ||
		(vidf == NULL) ||
		(pixf == NULL))
		return -EPERM;

	/* simple for now */
	if ((vidf->frame_flags & ntv2_kona_frame_square_division) != 0) {
		index /= 4;
		fst = 8 * index;
		lst = (8 * index) + 7;
		sz = 0x2000000;
	} else {
		fst = 8 * index;
		lst = (8 * index) + 7;
		sz = 0x800000;
	}

	if (first != NULL) {
		*first = fst;
	}
	if (last != NULL) {
		*last = lst;
	}
	if (size != NULL) {
		*size = sz;
	}

	return 0;
}

u32 ntv2_features_get_audio_capture_address(struct ntv2_features *features, u32 index)
{
	if (features == NULL)
		return 0;

	return (features->frame_buffer_size - 0x800000*(index + 1) + 0x400000);
}

u32 ntv2_features_get_audio_playback_address(struct ntv2_features *features, u32 index)
{
	if (features == NULL)
		return 0;

	return (features->frame_buffer_size - 0x800000*(index + 1));
}

int ntv2_features_acquire_sdi_component(struct ntv2_features *features,
										int index, int num, unsigned long owner)
{
	unsigned long flags;
	int i;

	if ((features == NULL) ||
		(index < 0) ||
		((index + num) > NTV2_MAX_SDI_COMPONENTS) ||
		(owner == 0))
		return -EPERM;

	spin_lock_irqsave(&features->state_lock, flags);

	for (i = 0; i < num; i++) {
		if (features->sdi_owner[index + i] != 0) {
			spin_unlock_irqrestore(&features->state_lock, flags);
			return -EBUSY;
		}
	}

	for (i = 0; i < num; i++)
		features->sdi_owner[index + i] = owner;

	spin_unlock_irqrestore(&features->state_lock, flags);

	return 0;
}

int ntv2_features_release_sdi_component(struct ntv2_features *features,
										int index, int num, unsigned long owner)
{
	unsigned long flags;
	int i;

	if ((features == NULL) ||
		(index < 0) ||
		((index + num) > NTV2_MAX_SDI_COMPONENTS) ||
		(owner == 0))
		return -EPERM;

	spin_lock_irqsave(&features->state_lock, flags);

	for (i = 0; i < num; i++) {
		if (features->sdi_owner[index + i] != owner) {
			spin_unlock_irqrestore(&features->state_lock, flags);
			return -EINVAL;
		}
	}

	for (i = 0; i < num; i++)
		features->sdi_owner[index] = 0;

	spin_unlock_irqrestore(&features->state_lock, flags);

	return 0;
}

void ntv2_features_release_components(struct ntv2_features *features, unsigned long owner)
{
	unsigned long flags;
	int i;

	if (features == NULL)
		return;

	spin_lock_irqsave(&features->state_lock, flags);

	for (i = 0; i < NTV2_MAX_SDI_COMPONENTS; i++) {
		if (features->sdi_owner[i] == owner)
			features->sdi_owner[i] = 0;
	}

	spin_unlock_irqrestore(&features->state_lock, flags);
}

bool ntv2_features_valid_dv_timings(struct ntv2_features *features,
									const struct v4l2_dv_timings *t,
									const struct v4l2_dv_timings_cap *dvcap)
{
	const struct v4l2_bt_timings *bt = &t->bt;
	const struct v4l2_bt_timings_cap *cap = &dvcap->bt;
	u32 caps = cap->capabilities;

	if (t->type != V4L2_DV_BT_656_1120)
		return false;
	if (t->type != dvcap->type ||
	    bt->height < cap->min_height ||
	    bt->height > cap->max_height ||
	    bt->width < cap->min_width ||
	    bt->width > cap->max_width ||
	    bt->pixelclock < cap->min_pixelclock ||
	    bt->pixelclock > cap->max_pixelclock ||
	    (cap->standards && !(bt->standards & cap->standards)) ||
	    (bt->interlaced && !(caps & V4L2_DV_BT_CAP_INTERLACED)) ||
	    (!bt->interlaced && !(caps & V4L2_DV_BT_CAP_PROGRESSIVE)))
		return false;
	return true;
}

int ntv2_features_enum_dv_timings_cap(struct ntv2_features *features,
									  struct v4l2_enum_dv_timings *t,
									  const struct v4l2_dv_timings_cap *cap)
{
	u32 i, idx;

	if (features == NULL)
		return -EPERM;

	memset(t->reserved, 0, sizeof(t->reserved));
	for (i = idx = 0; i < NTV2_MAX_VIDEO_FORMATS; i++) {
		if (features->v4l2_timings[i] == NULL)
			break;
		if (ntv2_features_valid_dv_timings(features, features->v4l2_timings[i], cap) &&
		    idx++ == t->index) {
			t->timings = *features->v4l2_timings[i];
			return 0;
		}
	}
	return -EINVAL;
}

bool ntv2_features_find_dv_timings_cap(struct ntv2_features *features,
									   struct v4l2_dv_timings *t,
									   const struct v4l2_dv_timings_cap *cap,
									   unsigned pclock_delta)
{
	int i;

	if (features == NULL)
		return false;

	if (!ntv2_features_valid_dv_timings(features, t, cap))
		return false;

	for (i = 0; NTV2_MAX_VIDEO_FORMATS; i++) {
		if (features->v4l2_timings[i] == NULL)
			break;
		if (ntv2_features_valid_dv_timings(features, features->v4l2_timings[i], cap) &&
		    ntv2_features_match_dv_timings(t, features->v4l2_timings[i], pclock_delta)) {
			*t = *features->v4l2_timings[i];
			return true;
		}
	}
	return false;
}

bool ntv2_features_match_dv_timings(const struct v4l2_dv_timings *t1,
									const struct v4l2_dv_timings *t2,
									unsigned pclock_delta)
{
	if (t1->type != t2->type || t1->type != V4L2_DV_BT_656_1120)
		return false;
	if (t1->bt.width == t2->bt.width &&
	    t1->bt.height == t2->bt.height &&
	    t1->bt.interlaced == t2->bt.interlaced &&
	    t1->bt.polarities == t2->bt.polarities &&
	    t1->bt.pixelclock >= t2->bt.pixelclock - pclock_delta &&
	    t1->bt.pixelclock <= t2->bt.pixelclock + pclock_delta &&
	    t1->bt.hfrontporch == t2->bt.hfrontporch &&
	    t1->bt.vfrontporch == t2->bt.vfrontporch &&
	    t1->bt.vsync == t2->bt.vsync &&
	    t1->bt.vbackporch == t2->bt.vbackporch &&
	    (!t1->bt.interlaced ||
		(t1->bt.il_vfrontporch == t2->bt.il_vfrontporch &&
		 t1->bt.il_vsync == t2->bt.il_vsync &&
		 t1->bt.il_vbackporch == t2->bt.il_vbackporch)))
		return true;
	return false;
}

#ifndef V4L2_DV_BT_CEA_3840X2160P24
#define V4L2_DV_BT_CEA_3840X2160P24 { \
	.type = V4L2_DV_BT_656_1120, \
	V4L2_INIT_BT_TIMINGS(3840, 2160, 0, V4L2_DV_HSYNC_POS_POL, \
		297000000, 1276, 88, 296, 8, 10, 72, 0, 0, 0, \
		V4L2_DV_BT_STD_CEA861, V4L2_DV_FL_CAN_REDUCE_FPS) \
}
#endif
#ifndef V4L2_DV_BT_CEA_3840X2160P25
#define V4L2_DV_BT_CEA_3840X2160P25 { \
	.type = V4L2_DV_BT_656_1120, \
	V4L2_INIT_BT_TIMINGS(3840, 2160, 0, V4L2_DV_HSYNC_POS_POL, \
		297000000, 1056, 88, 296, 8, 10, 72, 0, 0, 0, \
		V4L2_DV_BT_STD_CEA861, 0) \
}
#endif
#ifndef V4L2_DV_BT_CEA_3840X2160P30
#define V4L2_DV_BT_CEA_3840X2160P30 { \
	.type = V4L2_DV_BT_656_1120, \
	V4L2_INIT_BT_TIMINGS(3840, 2160, 0, V4L2_DV_HSYNC_POS_POL, \
		297000000, 176, 88, 296, 8, 10, 72, 0, 0, 0, \
		V4L2_DV_BT_STD_CEA861, V4L2_DV_FL_CAN_REDUCE_FPS) \
}
#endif
#ifndef V4L2_DV_BT_CEA_3840X2160P50
#define V4L2_DV_BT_CEA_3840X2160P50 { \
	.type = V4L2_DV_BT_656_1120, \
	V4L2_INIT_BT_TIMINGS(3840, 2160, 0, V4L2_DV_HSYNC_POS_POL, \
		594000000, 1056, 88, 296, 8, 10, 72, 0, 0, 0, \
		V4L2_DV_BT_STD_CEA861, 0) \
}
#endif
#ifndef V4L2_DV_BT_CEA_3840X2160P60
#define V4L2_DV_BT_CEA_3840X2160P60 { \
	.type = V4L2_DV_BT_656_1120, \
	V4L2_INIT_BT_TIMINGS(3840, 2160, 0, V4L2_DV_HSYNC_POS_POL, \
		594000000, 176, 88, 296, 8, 10, 72, 0, 0, 0, \
		V4L2_DV_BT_STD_CEA861, V4L2_DV_FL_CAN_REDUCE_FPS) \
}
#endif

static const struct v4l2_dv_timings_cap ntv2_timings_cap_sdi_single = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		720, 1920,				/* min/max width */
		480, 1080,				/* min/max height */
		13500000, 148500000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_INTERLACED
	)
};

static const struct v4l2_dv_timings_cap ntv2_timings_cap_sdi_dual = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		1920, 3840,				/* min/max width */
		1080, 2160,				/* min/max height */
		74250000, 297000000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE
	)
};

static const struct v4l2_dv_timings_cap ntv2_timings_cap_sdi_quad = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		3840, 3840,				/* min/max width */
		2160, 2160,				/* min/max height */
		297000000, 594000000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE
	)
};

static const struct v4l2_dv_timings_cap ntv2_timings_cap_hdmi13 = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		720, 1920,				/* min/max width */
		480, 1080,				/* min/max height */
		13500000, 148500000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_INTERLACED
	)
};

static const struct v4l2_dv_timings_cap ntv2_timings_cap_hdmi14 = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		720, 3840,				/* min/max width */
		480, 2160,				/* min/max height */
		13500000, 297000000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_INTERLACED
	)
};

static const struct v4l2_dv_timings_cap ntv2_timings_cap_hdmi20 = {
	.type = V4L2_DV_BT_656_1120,
	/* keep this initialization for compatibility with GCC < 4.4.6 */
	.reserved = { 0 },
	V4L2_INIT_BT_TIMINGS(
		720, 3840,				/* min/max width */
		480, 2160,				/* min/max height */
		13500000, 600000000,	/* min/max pixelclock*/
		V4L2_DV_BT_STD_CEA861,	/* Supported standards */
		/* capabilities */
		V4L2_DV_BT_CAP_PROGRESSIVE | V4L2_DV_BT_CAP_INTERLACED
	)
};

static struct v4l2_dv_timings		dvt_720x480i5994 = V4L2_DV_BT_CEA_720X480I59_94;
static struct v4l2_dv_timings		dvt_720x576i50 = V4L2_DV_BT_CEA_720X576I50;
static struct v4l2_dv_timings		dvt_1280x720p50 = V4L2_DV_BT_CEA_1280X720P50;
static struct v4l2_dv_timings		dvt_1280x720p60 = V4L2_DV_BT_CEA_1280X720P60;
static struct v4l2_dv_timings		dvt_1920x1080i50 = V4L2_DV_BT_CEA_1920X1080I50;
static struct v4l2_dv_timings		dvt_1920x1080i60 = V4L2_DV_BT_CEA_1920X1080I60;
static struct v4l2_dv_timings		dvt_1920x1080p24 = V4L2_DV_BT_CEA_1920X1080P24;
static struct v4l2_dv_timings		dvt_1920x1080p25 = V4L2_DV_BT_CEA_1920X1080P25;
static struct v4l2_dv_timings		dvt_1920x1080p30 = V4L2_DV_BT_CEA_1920X1080P30;
static struct v4l2_dv_timings		dvt_1920x1080p50 = V4L2_DV_BT_CEA_1920X1080P50;
static struct v4l2_dv_timings		dvt_1920x1080p60 = V4L2_DV_BT_CEA_1920X1080P60;
static struct v4l2_dv_timings		dvt_3840x2160p24 = V4L2_DV_BT_CEA_3840X2160P24;
static struct v4l2_dv_timings		dvt_3840x2160p25 = V4L2_DV_BT_CEA_3840X2160P25;
static struct v4l2_dv_timings		dvt_3840x2160p30 = V4L2_DV_BT_CEA_3840X2160P30;
static struct v4l2_dv_timings		dvt_3840x2160p50 = V4L2_DV_BT_CEA_3840X2160P50;
static struct v4l2_dv_timings		dvt_3840x2160p60 = V4L2_DV_BT_CEA_3840X2160P60;

static struct ntv2_video_config		nvc_capture;
static struct ntv2_video_config		nvc_playback;
static struct ntv2_video_config		nvc_both;
static struct ntv2_audio_config		nac_capture;
static struct ntv2_audio_config		nac_playback;
static struct ntv2_audio_config		nac_both;
static struct ntv2_serial_config	nsc_uartlite;

static struct ntv2_input_config		nic_sdi_single_1;
static struct ntv2_input_config		nic_sdi_single_2;
static struct ntv2_input_config		nic_sdi_single_3;
static struct ntv2_input_config		nic_sdi_single_4;
static struct ntv2_input_config		nic_sdi_single_5;
static struct ntv2_input_config		nic_sdi_single_6;
static struct ntv2_input_config		nic_sdi_single_7;
static struct ntv2_input_config		nic_sdi_single_8;
static struct ntv2_input_config		nic_sdi_dual_12;
static struct ntv2_input_config		nic_sdi_dual_34;
static struct ntv2_input_config		nic_sdi_dual_56;
static struct ntv2_input_config		nic_sdi_dual_78;
static struct ntv2_input_config		nic_sdi_quad_1234;
static struct ntv2_input_config		nic_sdi_quad_5678;
static struct ntv2_input_config		nic_hdmi14_1;
static struct ntv2_input_config		nic_hdmi20_1;
static struct ntv2_input_config		nic_hdmi20_2;
static struct ntv2_input_config		nic_hdmi13_3;
static struct ntv2_input_config		nic_hdmi13_4;

static struct ntv2_source_config	asc_auto;
static struct ntv2_source_config	asc_sdi_1;
static struct ntv2_source_config	asc_sdi_2;
static struct ntv2_source_config	asc_sdi_3;
static struct ntv2_source_config	asc_sdi_4;
static struct ntv2_source_config	asc_sdi_5;
static struct ntv2_source_config	asc_sdi_6;
static struct ntv2_source_config	asc_sdi_7;
static struct ntv2_source_config	asc_sdi_8;
static struct ntv2_source_config	asc_aes;
static struct ntv2_source_config	asc_analog;
static struct ntv2_source_config	asc_hdmi_1;
static struct ntv2_source_config	asc_hdmi_2;
static struct ntv2_source_config	asc_hdmi_3;
static struct ntv2_source_config	asc_hdmi_4;

static struct ntv2_video_format 	nvf_525i5994;
static struct ntv2_video_format 	nvf_625i5000;
static struct ntv2_video_format 	nvf_720p5000;
static struct ntv2_video_format 	nvf_720p5994;
static struct ntv2_video_format 	nvf_720p6000;
static struct ntv2_video_format 	nvf_1080p2398;
static struct ntv2_video_format 	nvf_1080p2400;
static struct ntv2_video_format 	nvf_1080p2500;
static struct ntv2_video_format 	nvf_1080p2997;
static struct ntv2_video_format 	nvf_1080p3000;
static struct ntv2_video_format 	nvf_1080p5000;
static struct ntv2_video_format 	nvf_1080p5994;
static struct ntv2_video_format 	nvf_1080p6000;
static struct ntv2_video_format 	nvf_1080i5000;
static struct ntv2_video_format 	nvf_1080i5994;
static struct ntv2_video_format 	nvf_1080i6000;
static struct ntv2_video_format 	nvf_2160p2398_sqd;
static struct ntv2_video_format 	nvf_2160p2400_sqd;
static struct ntv2_video_format 	nvf_2160p2500_sqd;
static struct ntv2_video_format 	nvf_2160p2997_sqd;
static struct ntv2_video_format 	nvf_2160p3000_sqd;
static struct ntv2_video_format 	nvf_2160p5000_sqd;
static struct ntv2_video_format 	nvf_2160p5994_sqd;
static struct ntv2_video_format 	nvf_2160p6000_sqd;
static struct ntv2_video_format 	nvf_2160p2398_tsi;
static struct ntv2_video_format 	nvf_2160p2400_tsi;
static struct ntv2_video_format 	nvf_2160p2500_tsi;
static struct ntv2_video_format 	nvf_2160p2997_tsi;
static struct ntv2_video_format 	nvf_2160p3000_tsi;
static struct ntv2_video_format 	nvf_2160p5000_tsi;
static struct ntv2_video_format 	nvf_2160p5994_tsi;
static struct ntv2_video_format 	nvf_2160p6000_tsi;

static struct ntv2_pixel_format 	npf_uyvy;
static struct ntv2_pixel_format 	npf_yuyv;
static struct ntv2_pixel_format 	npf_rgb;
static struct ntv2_pixel_format 	npf_bgr;
static struct ntv2_pixel_format 	npf_rgba;
static struct ntv2_pixel_format 	npf_bgra;

static void ntv2_features_initialize(void) {
	struct ntv2_video_config *nvc;
	struct ntv2_video_format *nvf;
	struct ntv2_pixel_format *npf;
	struct ntv2_audio_config *nac;
	struct ntv2_input_config *nic;
	struct ntv2_source_config *nss;
	struct ntv2_serial_config *nsc;

	/* video configuration */
	nvc = &nvc_capture;
	memset(nvc, 0, sizeof(struct ntv2_video_config));
	nvc->capture = true;
	nvc->playback = false;

	nvc = &nvc_playback;
	memset(nvc, 0, sizeof(struct ntv2_video_config));
	nvc->capture = false;
	nvc->playback = true;

	nvc = &nvc_both;
	memset(nvc, 0, sizeof(struct ntv2_video_config));
	nvc->capture = true;
	nvc->playback = true;

	/* audio configuration */
	nac = &nac_capture;
	memset(nac, 0, sizeof(struct ntv2_audio_config));
	nac->capture = true;
	nac->playback = false;
	nac->sample_rate = 48000;
	nac->num_channels = 16;
	nac->sample_size = 4;
	nac->ring_size = 0x3fc000;
	nac->ring_offset_samples = 64;
	nac->sync_tolerance = 10000;

	nac = &nac_playback;
	memset(nac, 0, sizeof(struct ntv2_audio_config));
	nac->capture = false;
	nac->playback = true;
	nac->sample_rate = 48000;
	nac->num_channels = 16;
	nac->sample_size = 4;
	nac->ring_size = 0x3fc000;
	nac->ring_offset_samples = 64;
	nac->sync_tolerance = 10000;

	nac = &nac_both;
	memset(nac, 0, sizeof(struct ntv2_audio_config));
	nac->capture = true;
	nac->playback = true;
	nac->sample_rate = 48000;
	nac->num_channels = 16;
	nac->sample_size = 4;
	nac->ring_size = 0x3fc000;
	nac->ring_offset_samples = 64;
	nac->sync_tolerance = 10000;

	/* serial port configuration */
	nsc = &nsc_uartlite;
	nsc->type = PORT_UARTLITE;
	nsc->fifo_size = 16;

	/* sdi single link inputs */
	nic = &nic_sdi_single_1;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 1";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 0;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_2;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 2";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 1;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_3;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 3";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 2;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_4;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 4";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 3;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_5;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 5";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 4;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_6;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 6";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 5;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_7;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 7";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 6;
	nic->num_inputs = 1;

	nic = &nic_sdi_single_8;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 8";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_single;
	nic->input_index = 7;
	nic->num_inputs = 1;

	/* sdi dual link inputs */
	nic = &nic_sdi_dual_12;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 1-2";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_dual;
	nic->input_index = 0;
	nic->num_inputs = 2;

	nic = &nic_sdi_dual_34;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 3-4";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_dual;
	nic->input_index = 2;
	nic->num_inputs = 2;

	nic = &nic_sdi_dual_56;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 5-6";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_dual;
	nic->input_index = 4;
	nic->num_inputs = 2;

	nic = &nic_sdi_dual_78;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 7-8";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_dual;
	nic->input_index = 6;
	nic->num_inputs = 2;

	/* sdi quad link inputs */
	nic = &nic_sdi_quad_1234;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 1-4";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_quad;
	nic->input_index = 0;
	nic->num_inputs = 4;

	nic = &nic_sdi_quad_5678;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "SDI 5-8";
	nic->type = ntv2_input_type_sdi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_sdi_quad;
	nic->input_index = 4;
	nic->num_inputs = 4;

	/* hdmi inputs */
	nic = &nic_hdmi14_1;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "HDMI 1";
	nic->type = ntv2_input_type_hdmi;
	nic->version = 1;
	nic->v4l2_timings_cap = ntv2_timings_cap_hdmi14;
	nic->input_index = 0;
	nic->num_inputs = 1;

	nic = &nic_hdmi20_1;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "HDMI 1";
	nic->type = ntv2_input_type_hdmi;
	nic->version = 2;
	nic->v4l2_timings_cap = ntv2_timings_cap_hdmi20;
	nic->input_index = 0;
	nic->num_inputs = 1;

	nic = &nic_hdmi20_2;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "HDMI 2";
	nic->type = ntv2_input_type_hdmi;
	nic->version = 2;
	nic->v4l2_timings_cap = ntv2_timings_cap_hdmi20;
	nic->input_index = 0;
	nic->num_inputs = 1;

	nic = &nic_hdmi13_3;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "HDMI 3";
	nic->type = ntv2_input_type_hdmi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_hdmi13;
	nic->input_index = 0;
	nic->num_inputs = 1;

	nic = &nic_hdmi13_4;
	memset(nic, 0, sizeof(struct ntv2_input_config));
	nic->name = "HDMI 4";
	nic->type = ntv2_input_type_hdmi;
	nic->version = 0;
	nic->v4l2_timings_cap = ntv2_timings_cap_hdmi13;
	nic->input_index = 0;
	nic->num_inputs = 1;

	/* audio auto source */
	nss = &asc_auto;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "Auto";
	nss->type = ntv2_input_type_auto;
	nss->version = 0;
	nss->audio_source = 0;
	nss->num_channels = 0;
	nss->input_index = 0;
	nss->num_inputs = 0;

	/* audio aes source */
	nss = &asc_aes;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "AES";
	nss->type = ntv2_input_type_aes;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_aes;
	nss->num_channels = 16;
	nss->input_index = 0;
	nss->num_inputs = 1;

	/* audio sdi source */
	nss = &asc_sdi_1;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 1";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 0;
	nss->num_inputs = 1;

	nss = &asc_sdi_2;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 2";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 1;
	nss->num_inputs = 1;

	nss = &asc_sdi_3;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 3";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 2;
	nss->num_inputs = 2;

	nss = &asc_sdi_4;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 4";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 3;
	nss->num_inputs = 1;

	nss = &asc_sdi_5;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 5";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 4;
	nss->num_inputs = 1;

	nss = &asc_sdi_6;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 6";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 5;
	nss->num_inputs = 1;

	nss = &asc_sdi_7;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 7";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 6;
	nss->num_inputs = 1;

	nss = &asc_sdi_8;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "SDI 8";
	nss->type = ntv2_input_type_sdi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_embedded;
	nss->num_channels = 16;
	nss->input_index = 7;
	nss->num_inputs = 1;

	/* audio analog source */
	nss = &asc_analog;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "Analog";
	nss->type = ntv2_input_type_analog;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_analog;
	nss->num_channels = 2;
	nss->input_index = 0;
	nss->num_inputs = 1;

	/* audio hdmi source */
	nss = &asc_hdmi_1;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "HDMI 1";
	nss->type = ntv2_input_type_hdmi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_hdmi;
	nss->num_channels = 8;
	nss->input_index = 0;
	nss->num_inputs = 1;

	nss = &asc_hdmi_2;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "HDMI 2";
	nss->type = ntv2_input_type_hdmi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_hdmi;
	nss->num_channels = 8;
	nss->input_index = 1;
	nss->num_inputs = 1;

	nss = &asc_hdmi_3;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "HDMI 3";
	nss->type = ntv2_input_type_hdmi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_hdmi;
	nss->num_channels = 8;
	nss->input_index = 2;
	nss->num_inputs = 1;

	nss = &asc_hdmi_4;
	memset(nss, 0, sizeof(struct ntv2_source_config));
	nss->name = "HDMI 4";
	nss->type = ntv2_input_type_hdmi;
	nss->version = 0;
	nss->audio_source = ntv2_kona_audio_source_hdmi;
	nss->num_channels = 8;
	nss->input_index = 3;
	nss->num_inputs = 1;

	/* 525i5994 timing */
	nvf = &nvf_525i5994;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "525i5994";
	nvf->v4l2_timings = dvt_720x480i5994;
	nvf->video_standard = ntv2_kona_video_standard_525i;
	nvf->frame_geometry = ntv2_kona_frame_geometry_720x486;
	nvf->frame_rate = ntv2_kona_frame_rate_2997;
	nvf->frame_flags =
		ntv2_kona_frame_picture_interlaced;

	/* 625i5000 timing */
	nvf = &nvf_625i5000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "625i5000";
	nvf->v4l2_timings = dvt_720x576i50;
	nvf->video_standard = ntv2_kona_video_standard_625i;
	nvf->frame_geometry = ntv2_kona_frame_geometry_720x576;
	nvf->frame_rate = ntv2_kona_frame_rate_2500;
	nvf->frame_flags =
		ntv2_kona_frame_picture_interlaced;

	/* 720p5000 timing */
	nvf = &nvf_720p5000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "720p5000";
	nvf->v4l2_timings = dvt_1280x720p50;
	nvf->video_standard = ntv2_kona_video_standard_720p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1280x720;
	nvf->frame_rate = ntv2_kona_frame_rate_5000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 720p5994 timing */
	nvf = &nvf_720p5994;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "720p5994";
	nvf->v4l2_timings = dvt_1280x720p60;
	nvf->video_standard = ntv2_kona_video_standard_720p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1280x720;
	nvf->frame_rate = ntv2_kona_frame_rate_5994;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 720p6000 timing */
	nvf = &nvf_720p6000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "720p6000";
	nvf->v4l2_timings = dvt_1280x720p60;
	nvf->video_standard = ntv2_kona_video_standard_720p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1280x720;
	nvf->frame_rate = ntv2_kona_frame_rate_6000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p2398 timing */
	nvf = &nvf_1080p2398;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p2398";
	nvf->v4l2_timings = dvt_1920x1080p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2398;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p2400 timing */
	nvf = &nvf_1080p2400;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p2400";
	nvf->v4l2_timings = dvt_1920x1080p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2400;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p2500 timing */
	nvf = &nvf_1080p2500;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p2500";
	nvf->v4l2_timings = dvt_1920x1080p25;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2500;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p2997 timing */
	nvf = &nvf_1080p2997;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p2997";
	nvf->v4l2_timings = dvt_1920x1080p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2997;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p3000 timing */
	nvf = &nvf_1080p3000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p3000";
	nvf->v4l2_timings = dvt_1920x1080p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_3000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p5000 timing */
	nvf = &nvf_1080p5000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p5000";
	nvf->v4l2_timings = dvt_1920x1080p50;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p5994 timing */
	nvf = &nvf_1080p5994;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p5994";
	nvf->v4l2_timings = dvt_1920x1080p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5994;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080p6000 timing */
	nvf = &nvf_1080p6000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080p6000";
	nvf->v4l2_timings = dvt_1920x1080p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_6000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive;

	/* 1080i5000 timing */
	nvf = &nvf_1080i5000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080i5000";
	nvf->v4l2_timings = dvt_1920x1080i50;
	nvf->video_standard = ntv2_kona_video_standard_1080i;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2500;
	nvf->frame_flags =
		ntv2_kona_frame_picture_interlaced;

	/* 1080i5994 timing */
	nvf = &nvf_1080i5994;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080i5994";
	nvf->v4l2_timings = dvt_1920x1080i60;
	nvf->video_standard = ntv2_kona_video_standard_1080i;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2997;
	nvf->frame_flags =
		ntv2_kona_frame_picture_interlaced;

	/* 1080i6000 timing */
	nvf = &nvf_1080i6000;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "1080i6000";
	nvf->v4l2_timings = dvt_1920x1080i60;
	nvf->video_standard = ntv2_kona_video_standard_1080i;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_6000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_interlaced;

	/* 2160p2398 timing */
	nvf = &nvf_2160p2398_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2398";
	nvf->v4l2_timings = dvt_3840x2160p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2398;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p2400 timing */
	nvf = &nvf_2160p2400_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2400";
	nvf->v4l2_timings = dvt_3840x2160p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2400;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p2500 timing */
	nvf = &nvf_2160p2500_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2500";
	nvf->v4l2_timings = dvt_3840x2160p25;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2500;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p2997 timing */
	nvf = &nvf_2160p2997_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2997";
	nvf->v4l2_timings = dvt_3840x2160p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2997;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p3000 timing */
	nvf = &nvf_2160p3000_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p3000";
	nvf->v4l2_timings = dvt_3840x2160p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_3000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p5000 timing */
	nvf = &nvf_2160p5000_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p5000";
	nvf->v4l2_timings = dvt_3840x2160p50;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p5994 timing */
	nvf = &nvf_2160p5994_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p5994";
	nvf->v4l2_timings = dvt_3840x2160p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5994;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p6000 timing */
	nvf = &nvf_2160p6000_sqd;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p6000";
	nvf->v4l2_timings = dvt_3840x2160p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_6000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_square_division;

	/* 2160p2398 timing */
	nvf = &nvf_2160p2398_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2398";
	nvf->v4l2_timings = dvt_3840x2160p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2398;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p2400 timing */
	nvf = &nvf_2160p2400_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2400";
	nvf->v4l2_timings = dvt_3840x2160p24;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2400;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p2500 timing */
	nvf = &nvf_2160p2500_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2500";
	nvf->v4l2_timings = dvt_3840x2160p25;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2500;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p2997 timing */
	nvf = &nvf_2160p2997_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p2997";
	nvf->v4l2_timings = dvt_3840x2160p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_2997;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p3000 timing */
	nvf = &nvf_2160p3000_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p3000";
	nvf->v4l2_timings = dvt_3840x2160p30;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_3000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p5000 timing */
	nvf = &nvf_2160p5000_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p5000";
	nvf->v4l2_timings = dvt_3840x2160p50;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p5994 timing */
	nvf = &nvf_2160p5994_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p5994";
	nvf->v4l2_timings = dvt_3840x2160p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_5994;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* 2160p6000 timing */
	nvf = &nvf_2160p6000_tsi;
	memset(nvf, 0, sizeof(struct ntv2_video_format));
	nvf->name = "2160p6000";
	nvf->v4l2_timings = dvt_3840x2160p60;
	nvf->video_standard = ntv2_kona_video_standard_1080p;
	nvf->frame_geometry = ntv2_kona_frame_geometry_1920x1080;
	nvf->frame_rate = ntv2_kona_frame_rate_6000;
	nvf->frame_flags =
		ntv2_kona_frame_picture_progressive |
		ntv2_kona_frame_sample_interleaved;

	/* UYVY16 format */
	npf = &npf_uyvy;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "UYVY16";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_UYVY;
	npf->ntv2_pixel_format = ntv2_kona_fbf_8bit_ycbcr;
	npf->pixel_flags =
		ntv2_kona_pixel_yuv |
		ntv2_kona_pixel_422 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 2;
	npf->cadence_bytes = 4;
	npf->pitch_alignment = 4;

	/* YUYV16 format */
	npf = &npf_yuyv;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "YUYV16";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_YUYV;
	npf->ntv2_pixel_format = ntv2_kona_fbf_8bit_yuy2;
	npf->pixel_flags =
		ntv2_kona_pixel_yuv |
		ntv2_kona_pixel_422 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 2;
	npf->cadence_bytes = 4;
	npf->pitch_alignment = 4;

	/* RGB24 format */
	npf = &npf_rgb;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "RGB24";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_RGB24;
	npf->ntv2_pixel_format = ntv2_kona_fbf_24bit_rgb;
	npf->pixel_flags =
		ntv2_kona_pixel_rgb |
		ntv2_kona_pixel_444 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 1;
	npf->cadence_bytes = 3;
	npf->pitch_alignment = 4;

	/* BGR24 format */
	npf = &npf_bgr;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "BGR24";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_BGR24;
	npf->ntv2_pixel_format = ntv2_kona_fbf_24bit_bgr;
	npf->pixel_flags =
		ntv2_kona_pixel_rgb |
		ntv2_kona_pixel_444 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 1;
	npf->cadence_bytes = 3;
	npf->pitch_alignment = 4;

	/* RGB32 format */
	npf = &npf_rgba;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "RGB32";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_RGB32;
	npf->ntv2_pixel_format = ntv2_kona_fbf_abgr;
	npf->pixel_flags =
		ntv2_kona_pixel_rgb |
		ntv2_kona_pixel_4444 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 1;
	npf->cadence_bytes = 4;
	npf->pitch_alignment = 4;

	/* BGR32 format */
	npf = &npf_bgra;
	memset(npf, 0, sizeof(struct ntv2_pixel_format));
	npf->name = "BGR32";
	npf->v4l2_pixel_format = V4L2_PIX_FMT_BGR32;
	npf->ntv2_pixel_format = ntv2_kona_fbf_argb;
	npf->pixel_flags =
		ntv2_kona_pixel_rgb |
		ntv2_kona_pixel_4444 |
		ntv2_kona_pixel_8bit;
	npf->cadence_pixels = 1;
	npf->cadence_bytes = 4;
	npf->pitch_alignment = 4;
}

static void all_video_formats(struct ntv2_features *features)
{
	features->video_formats[0] = &nvf_525i5994;
	features->video_formats[1] = &nvf_625i5000;
	features->video_formats[2] = &nvf_720p5000;
	features->video_formats[3] = &nvf_720p5994;
	features->video_formats[4] = &nvf_720p6000;
	features->video_formats[5] = &nvf_1080p2398;
	features->video_formats[6] = &nvf_1080p2400;
	features->video_formats[7] = &nvf_1080p2500;
	features->video_formats[8] = &nvf_1080p2997;
	features->video_formats[9] = &nvf_1080p3000;
	features->video_formats[10] = &nvf_1080p5000;
	features->video_formats[11] = &nvf_1080p5994;
	features->video_formats[12] = &nvf_1080p6000;
	features->video_formats[13] = &nvf_1080i5000;
	features->video_formats[14] = &nvf_1080i5994;
	features->video_formats[15] = &nvf_1080i6000;
	features->video_formats[16] = &nvf_2160p2398_sqd;
	features->video_formats[17] = &nvf_2160p2400_sqd;
	features->video_formats[18] = &nvf_2160p2500_sqd;
	features->video_formats[19] = &nvf_2160p2997_sqd;
	features->video_formats[20] = &nvf_2160p3000_sqd;
	features->video_formats[21] = &nvf_2160p5000_sqd;
	features->video_formats[22] = &nvf_2160p5994_sqd;
	features->video_formats[23] = &nvf_2160p6000_sqd;
	features->video_formats[24] = &nvf_2160p2398_tsi;
	features->video_formats[25] = &nvf_2160p2400_tsi;
	features->video_formats[26] = &nvf_2160p2500_tsi;
	features->video_formats[27] = &nvf_2160p2997_tsi;
	features->video_formats[28] = &nvf_2160p3000_tsi;
	features->video_formats[29] = &nvf_2160p5000_tsi;
	features->video_formats[30] = &nvf_2160p5994_tsi;
	features->video_formats[31] = &nvf_2160p6000_tsi;
}

static void all_yuv_pixel_formats(struct ntv2_features *features)
{
	features->pixel_formats[0] = &npf_uyvy;
	features->pixel_formats[1] = &npf_yuyv;
}

static void all_rgb_pixel_formats(struct ntv2_features *features)
{
	features->pixel_formats[0] = &npf_rgb;
	features->pixel_formats[1] = &npf_bgr;
	features->pixel_formats[2] = &npf_rgba;
	features->pixel_formats[3] = &npf_bgra;
}

static void all_pixel_formats(struct ntv2_features *features)
{
	features->pixel_formats[0] = &npf_uyvy;
	features->pixel_formats[1] = &npf_yuyv;
	features->pixel_formats[2] = &npf_rgb;
	features->pixel_formats[3] = &npf_bgr;
	features->pixel_formats[4] = &npf_rgba;
	features->pixel_formats[5] = &npf_bgra;
}

static void build_v4l2_timings(struct ntv2_features *features)
{
	int itx;
	int i, j;

	itx = 0;
	for (i = 0; i < NTV2_MAX_VIDEO_FORMATS; i++) {
		if ((features->video_formats[i] == NULL) ||
			(itx >= NTV2_MAX_VIDEO_FORMATS))
			break;
		for (j = 0; j < itx; j++) {
			if (ntv2_features_match_dv_timings(&features->video_formats[i]->v4l2_timings,
											   features->v4l2_timings[j], 0)) {
				break;
			}
		}
		if (j == itx) {
			features->v4l2_timings[itx++] = &features->video_formats[i]->v4l2_timings;
		}
	}
}

static void ntv2_features_corvid44(struct ntv2_features *features) 
{
	int i;

	features->device_name = "Corvid44";
	features->pcm_name = "Corvid44 PCM";
	features->pci_vendor = NTV2_VENDOR_ID;
	features->pci_device = NTV2_DEVICE_ID_CORVID44;
	features->pci_subsystem_vendor = PCI_ANY_ID;
	features->pci_subsystem_device = PCI_ANY_ID;
	features->num_video_channels = 4;
	features->num_audio_channels = 4;
	features->num_sdi_inputs = 4;
	features->num_reference_inputs = 1;
	features->frame_buffer_size = 0x40000000;

	for (i = 0; i < features->num_video_channels; i++) {
		features->video_config[i] = &nvc_capture;
	}
	for (i = 0; i < features->num_audio_channels; i++) {
		features->audio_config[i] = &nac_capture;
	}

	features->input_config[0][0] = &nic_sdi_single_1;
	features->input_config[0][1] = &nic_sdi_dual_12;
	features->input_config[0][2] = &nic_sdi_quad_1234;
	features->input_config[1][0] = &nic_sdi_single_2;
	features->input_config[2][0] = &nic_sdi_single_3;
	features->input_config[2][1] = &nic_sdi_dual_34;
	features->input_config[3][0] = &nic_sdi_single_4;

	features->source_config[0][0] = &asc_auto;
	features->source_config[0][1] = &asc_sdi_1;
	features->source_config[0][2] = &asc_sdi_2;
	features->source_config[0][3] = &asc_sdi_3;
	features->source_config[0][4] = &asc_sdi_4;
	features->source_config[1][0] = &asc_auto;
	features->source_config[1][1] = &asc_sdi_1;
	features->source_config[1][2] = &asc_sdi_2;
	features->source_config[1][3] = &asc_sdi_3;
	features->source_config[1][4] = &asc_sdi_4;
	features->source_config[2][0] = &asc_auto;
	features->source_config[2][1] = &asc_sdi_1;
	features->source_config[2][2] = &asc_sdi_2;
	features->source_config[2][3] = &asc_sdi_3;
	features->source_config[2][4] = &asc_sdi_4;
	features->source_config[3][0] = &asc_auto;
	features->source_config[3][1] = &asc_sdi_1;
	features->source_config[3][2] = &asc_sdi_2;
	features->source_config[3][3] = &asc_sdi_3;
	features->source_config[3][4] = &asc_sdi_4;

	all_video_formats(features);
	all_yuv_pixel_formats(features);
	build_v4l2_timings(features);
}

static void ntv2_features_corvid88(struct ntv2_features *features) 
{
	int i;

	features->device_name = "Corvid88";
	features->pcm_name = "Corvid88 PCM";
	features->pci_vendor = NTV2_VENDOR_ID;
	features->pci_device = NTV2_DEVICE_ID_CORVID88;
	features->pci_subsystem_vendor = PCI_ANY_ID;
	features->pci_subsystem_device = PCI_ANY_ID;
	features->num_video_channels = 8;
	features->num_audio_channels = 8;
	features->num_sdi_inputs = 8;
	features->num_reference_inputs = 1;
	features->frame_buffer_size = 0x40000000;

	for (i = 0; i < features->num_video_channels; i++) {
		features->video_config[i] = &nvc_capture;
	}
	for (i = 0; i < features->num_audio_channels; i++) {
		features->audio_config[i] = &nac_capture;
	}

	features->input_config[0][0] = &nic_sdi_single_1;
	features->input_config[0][1] = &nic_sdi_dual_12;
	features->input_config[0][2] = &nic_sdi_quad_1234;
	features->input_config[1][0] = &nic_sdi_single_2;
	features->input_config[2][0] = &nic_sdi_single_3;
	features->input_config[2][1] = &nic_sdi_dual_34;
	features->input_config[3][0] = &nic_sdi_single_4;
	features->input_config[4][0] = &nic_sdi_single_5;
	features->input_config[4][1] = &nic_sdi_dual_56;
	features->input_config[4][2] = &nic_sdi_quad_5678;
	features->input_config[5][0] = &nic_sdi_single_6;
	features->input_config[6][0] = &nic_sdi_single_7;
	features->input_config[6][1] = &nic_sdi_dual_78;
	features->input_config[7][0] = &nic_sdi_single_8;

	features->source_config[0][0] = &asc_auto;
	features->source_config[0][1] = &asc_sdi_1;
	features->source_config[0][2] = &asc_sdi_2;
	features->source_config[0][3] = &asc_sdi_3;
	features->source_config[0][4] = &asc_sdi_4;
	features->source_config[1][0] = &asc_auto;
	features->source_config[1][1] = &asc_sdi_1;
	features->source_config[1][2] = &asc_sdi_2;
	features->source_config[1][3] = &asc_sdi_3;
	features->source_config[1][4] = &asc_sdi_4;
	features->source_config[2][0] = &asc_auto;
	features->source_config[2][1] = &asc_sdi_1;
	features->source_config[2][2] = &asc_sdi_2;
	features->source_config[2][3] = &asc_sdi_3;
	features->source_config[2][4] = &asc_sdi_4;
	features->source_config[3][0] = &asc_auto;
	features->source_config[3][1] = &asc_sdi_1;
	features->source_config[3][2] = &asc_sdi_2;
	features->source_config[3][3] = &asc_sdi_3;
	features->source_config[3][4] = &asc_sdi_4;

	features->source_config[4][0] = &asc_auto;
	features->source_config[4][1] = &asc_sdi_5;
	features->source_config[4][2] = &asc_sdi_6;
	features->source_config[4][3] = &asc_sdi_7;
	features->source_config[4][4] = &asc_sdi_8;
	features->source_config[5][0] = &asc_auto;
	features->source_config[5][1] = &asc_sdi_5;
	features->source_config[5][2] = &asc_sdi_6;
	features->source_config[5][3] = &asc_sdi_7;
	features->source_config[5][4] = &asc_sdi_8;
	features->source_config[6][0] = &asc_auto;
	features->source_config[6][1] = &asc_sdi_5;
	features->source_config[6][2] = &asc_sdi_6;
	features->source_config[6][3] = &asc_sdi_7;
	features->source_config[6][4] = &asc_sdi_8;
	features->source_config[7][0] = &asc_auto;
	features->source_config[7][1] = &asc_sdi_5;
	features->source_config[7][2] = &asc_sdi_6;
	features->source_config[7][3] = &asc_sdi_7;
	features->source_config[7][4] = &asc_sdi_8;

	all_video_formats(features);
	all_yuv_pixel_formats(features);
	build_v4l2_timings(features);
}

static void ntv2_features_kona4(struct ntv2_features *features) 
{
	int i;

	features->device_name = "Kona4";
	features->pcm_name = "Kona4 PCM";
	features->pci_vendor = NTV2_VENDOR_ID;
	features->pci_device = NTV2_DEVICE_ID_KONA4;
	features->pci_subsystem_vendor = PCI_ANY_ID;
	features->pci_subsystem_device = PCI_ANY_ID;
	features->num_video_channels = 4;
	features->num_audio_channels = 4;
	features->num_sdi_inputs = 4;
	features->num_reference_inputs = 1;
	features->frame_buffer_size = 0x37800000;

	for (i = 0; i < features->num_video_channels; i++) {
		features->video_config[i] = &nvc_capture;
	}
	for (i = 0; i < features->num_audio_channels; i++) {
		features->audio_config[i] = &nac_capture;
	}

	features->input_config[0][0] = &nic_sdi_single_1;
	features->input_config[0][1] = &nic_sdi_dual_12;
	features->input_config[0][2] = &nic_sdi_quad_1234;
	features->input_config[1][0] = &nic_sdi_single_2;
	features->input_config[2][0] = &nic_sdi_single_3;
	features->input_config[2][1] = &nic_sdi_dual_34;
	features->input_config[3][0] = &nic_sdi_single_4;

	features->source_config[0][0] = &asc_auto;
	features->source_config[0][1] = &asc_sdi_1;
	features->source_config[0][2] = &asc_sdi_2;
	features->source_config[0][3] = &asc_sdi_3;
	features->source_config[0][4] = &asc_sdi_4;
	features->source_config[1][0] = &asc_auto;
	features->source_config[1][1] = &asc_sdi_1;
	features->source_config[1][2] = &asc_sdi_2;
	features->source_config[1][3] = &asc_sdi_3;
	features->source_config[1][4] = &asc_sdi_4;
	features->source_config[2][0] = &asc_auto;
	features->source_config[2][1] = &asc_sdi_1;
	features->source_config[2][2] = &asc_sdi_2;
	features->source_config[2][3] = &asc_sdi_3;
	features->source_config[2][4] = &asc_sdi_4;
	features->source_config[3][0] = &asc_auto;
	features->source_config[3][1] = &asc_sdi_1;
	features->source_config[3][2] = &asc_sdi_2;
	features->source_config[3][3] = &asc_sdi_3;
	features->source_config[3][4] = &asc_sdi_4;

	all_video_formats(features);
	all_pixel_formats(features);
	build_v4l2_timings(features);
}

static void ntv2_features_corvidhdbt(struct ntv2_features *features) 
{
	features->device_name = "CorvidHB-R";
	features->pcm_name = "CorvidHB-R PCM";
	features->pci_vendor = NTV2_VENDOR_ID;
	features->pci_device = NTV2_DEVICE_ID_CORVIDHDBT;
	features->pci_subsystem_vendor = PCI_ANY_ID;
	features->pci_subsystem_device = PCI_ANY_ID;
	features->num_video_channels = 1;
	features->num_audio_channels = 1;
	features->num_hdmi_inputs = 1;
	features->num_aes_inputs = 1;
	features->frame_buffer_size = 0x20000000;
	features->num_serial_ports = 1;

	features->video_config[0] = &nvc_capture;
	features->audio_config[0] = &nac_capture;

	features->input_config[0][0] = &nic_hdmi14_1;

	features->video_formats[0] = &nvf_525i5994;
	features->video_formats[1] = &nvf_625i5000;
	features->video_formats[2] = &nvf_720p5000;
	features->video_formats[3] = &nvf_720p5994;
	features->video_formats[4] = &nvf_720p6000;
	features->video_formats[5] = &nvf_1080p2398;
	features->video_formats[6] = &nvf_1080p2400;
	features->video_formats[7] = &nvf_1080p2500;
	features->video_formats[8] = &nvf_1080p2997;
	features->video_formats[9] = &nvf_1080p3000;
	features->video_formats[10] = &nvf_1080p5000;
	features->video_formats[11] = &nvf_1080p5994;
	features->video_formats[12] = &nvf_1080p6000;
	features->video_formats[13] = &nvf_1080i5000;
	features->video_formats[14] = &nvf_1080i5994;
	features->video_formats[15] = &nvf_1080i6000;
	features->video_formats[16] = &nvf_2160p2398_sqd;
	features->video_formats[17] = &nvf_2160p2400_sqd;
	features->video_formats[18] = &nvf_2160p2500_sqd;
	features->video_formats[19] = &nvf_2160p2997_sqd;
	features->video_formats[20] = &nvf_2160p3000_sqd;

	features->source_config[0][0] = &asc_auto;
	features->source_config[0][1] = &asc_aes;
	features->source_config[0][2] = &asc_hdmi_1;

	all_pixel_formats(features);
	build_v4l2_timings(features);

	features->serial_config[0] = &nsc_uartlite;
}

static void ntv2_features_konahdmi(struct ntv2_features *features) 
{
	features->device_name = "Kona HDMI";
	features->pcm_name = "Kona HDMI PCM";
	features->pci_vendor = NTV2_VENDOR_ID;
	features->pci_device = NTV2_DEVICE_ID_KONAHDMI;
	features->pci_subsystem_vendor = PCI_ANY_ID;
	features->pci_subsystem_device = PCI_ANY_ID;
	features->num_video_channels = 4;
	features->num_audio_channels = 4;
	features->num_hdmi_inputs = 4;
	features->num_aes_inputs = 0;
	features->frame_buffer_size = 0x20000000;
	features->num_serial_ports = 0;

	features->video_config[0] = &nvc_capture;
	features->audio_config[0] = &nac_capture;

	features->input_config[0][0] = &nic_hdmi20_1;
	features->input_config[1][0] = &nic_hdmi20_2;
	features->input_config[2][0] = &nic_hdmi13_3;
	features->input_config[3][0] = &nic_hdmi13_4;

	features->source_config[0][0] = &asc_auto;
	features->source_config[0][1] = &asc_hdmi_1;
	features->source_config[1][0] = &asc_auto;
	features->source_config[1][1] = &asc_hdmi_2;
	features->source_config[2][0] = &asc_auto;
	features->source_config[2][1] = &asc_hdmi_3;
	features->source_config[3][0] = &asc_auto;
	features->source_config[3][1] = &asc_hdmi_4;

	all_video_formats(features);
	all_pixel_formats(features);
	build_v4l2_timings(features);
}

