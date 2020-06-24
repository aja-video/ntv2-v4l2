/*
 * NTV2 v4l2 device interface
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

#include "ntv2_video.h"
#include "ntv2_features.h"
#include "ntv2_vb2ops.h"
#include "ntv2_v4l2ops.h"
#include "ntv2_channel.h"
#include "ntv2_pci.h"
#include "ntv2_input.h"
#include "ntv2_konareg.h"
#include "ntv2_timecode.h"


#define NTV2_VIDEO_TRANSFER_TIMEOUT		(100000)

static bool ntv2_video_compare_input_format(struct ntv2_input_format *format_a,
											struct ntv2_input_format *format_b);
static void ntv2_video_transfer_task(unsigned long data);
static void ntv2_video_dma_callback(unsigned long data, int result);
static void ntv2_video_channel_callback(unsigned long data);
static void ntv2_video_stream_to_buffer(struct ntv2_video *ntv2_vid,
										struct ntv2_stream_data *data,
										struct ntv2_vb2buf *buffer);

struct ntv2_video *ntv2_video_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index)
{
	struct ntv2_video *ntv2_vid = NULL;

	if (ntv2_obj == NULL)
		return NULL;

	ntv2_vid = kzalloc(sizeof(struct ntv2_video), GFP_KERNEL);
	if (ntv2_vid == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_video instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_vid->index = index;
	snprintf(ntv2_vid->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_vid->list);
	ntv2_vid->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_vid->state_lock);
	spin_lock_init(&ntv2_vid->vb2_lock);
	mutex_init(&ntv2_vid->video_mutex);
	mutex_init(&ntv2_vid->vb2_mutex);
	atomic_set(&ntv2_vid->video_ref, (-1));

	INIT_LIST_HEAD(&ntv2_vid->vb2buf_list);

	/* dma task */
	tasklet_init(&ntv2_vid->transfer_task,
				 ntv2_video_transfer_task,
				 (unsigned long)ntv2_vid);

	ntv2_vid->init = true;

	NTV2_MSG_VIDEO_INFO("%s: open ntv2_video\n", ntv2_vid->name);

	return ntv2_vid;
}

void ntv2_video_close(struct ntv2_video *ntv2_vid)
{
	if (ntv2_vid == NULL) 
		return;

	NTV2_MSG_VIDEO_INFO("%s: close ntv2_video\n", ntv2_vid->name);

	/* stop the queue */
	ntv2_video_disable(ntv2_vid);

	tasklet_kill(&ntv2_vid->transfer_task);

	if (ntv2_vid->video_init) {
		video_unregister_device(&ntv2_vid->video_dev);
		ntv2_vid->video_init = false;
	}

	if (ntv2_vid->ctrl_init) {
		v4l2_ctrl_handler_free(&ntv2_vid->ctrl_handler);
		ntv2_vid->ctrl_init = false;
	}

	if (ntv2_vid->v4l2_init) {
		v4l2_device_unregister(&ntv2_vid->v4l2_dev);
		ntv2_vid->v4l2_init = false;
	}

	if (ntv2_vid->vb2_init) {
		vb2_queue_release(&ntv2_vid->vb2_queue);
		ntv2_vid->vb2_init = false;
	}

	memset(ntv2_vid, 0, sizeof(struct ntv2_video));
	kfree(ntv2_vid);
}

int ntv2_video_configure(struct ntv2_video *ntv2_vid,
						 struct ntv2_features *features,
						 struct ntv2_channel *ntv2_chn,
						 struct ntv2_input *ntv2_inp,
						 struct ntv2_pci *ntv2_pci)
{
	struct video_device *video_dev;
	int result;

	if ((ntv2_vid == NULL) ||
		(features == NULL) ||
		(ntv2_chn == NULL) ||
		(ntv2_inp == NULL) ||
		(ntv2_pci == NULL))
		return -EPERM;

	NTV2_MSG_VIDEO_INFO("%s: configure video device\n", ntv2_vid->name);

	ntv2_vid->features = features;
	ntv2_vid->ntv2_chn = ntv2_chn;
	ntv2_vid->ntv2_inp = ntv2_inp;
	ntv2_vid->ntv2_pci = ntv2_pci;

	ntv2_vid->vid_str = ntv2_channel_stream(ntv2_chn, ntv2_stream_type_vidin);
	ntv2_vid->aud_str = ntv2_channel_stream(ntv2_chn, ntv2_stream_type_audin);

	/* initialize state */
	ntv2_vid->video_format = *ntv2_features_get_default_video_format(features, ntv2_chn->index);
	ntv2_vid->pixel_format = *ntv2_features_get_default_pixel_format(features, ntv2_chn->index);
	ntv2_features_gen_input_format(ntv2_features_get_default_input_config(features, ntv2_chn->index),
								   &ntv2_vid->video_format,
								   &ntv2_vid->pixel_format,
								   &ntv2_vid->input_format);
	ntv2_vid->drop_frame = ntv2_frame_rate_drop(ntv2_vid->video_format.frame_rate);
	
	/* register the v4l2 device */
	result = v4l2_device_register(&ntv2_vid->ntv2_dev->pci_dev->dev, &ntv2_vid->v4l2_dev);
	if (result != 0) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* v4l2_device_register() failed code %d\n",
							 ntv2_vid->name, result);
		return result;
	}
	ntv2_vid->v4l2_init = true;

	NTV2_MSG_VIDEO_INFO("%s: register v4l2 device: %s\n",
						ntv2_vid->name, ntv2_vid->v4l2_dev.name);

	/* configure vb2 ops */
	result = ntv2_vb2ops_configure(ntv2_vid);
	if (result != 0) 
		return result;

	/* configure v4l2 ops */
	result = ntv2_v4l2ops_configure(ntv2_vid);
	if (result != 0) 
		return result;

	/* initialize the video_device */
	video_dev = &ntv2_vid->video_dev;
	strlcpy(video_dev->name, NTV2_MODULE_NAME, sizeof(video_dev->name));
	video_set_drvdata(video_dev, ntv2_vid);
#ifdef NTV2_USE_V4L2_FH
	set_bit(V4L2_FL_USES_V4L2_FH, &video_dev->flags);
#else
	set_bit(V4L2_FL_USE_FH_PRIO, &video_dev->flags);
#endif
	/* null release function for now */
	video_dev->release = video_device_release_empty;
#ifdef NTV2_VIDEO_DEVICE_CAPABILITES	
	video_dev->device_caps = V4L2_CAP_VIDEO_CAPTURE |
		V4L2_CAP_READWRITE |
		V4L2_CAP_STREAMING;
#endif
	/* assign queue and v4l2 device */
	video_dev->queue = &ntv2_vid->vb2_queue;
	video_dev->v4l2_dev = &ntv2_vid->v4l2_dev;
	video_dev->lock = &ntv2_vid->video_mutex;

	/* fixes gstreamer */
	if (video_dev->tvnorms == 0) {
		v4l2_disable_ioctl(video_dev, VIDIOC_G_STD);
		v4l2_disable_ioctl(video_dev, VIDIOC_S_STD);
		v4l2_disable_ioctl(video_dev, VIDIOC_ENUMSTD);
	}

	NTV2_MSG_VIDEO_INFO("%s: register video device: %s\n",
						ntv2_vid->name, video_dev->name);

	/* register the video device */
	result = video_register_device(video_dev, VFL_TYPE_GRABBER, -1);
	if (result) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* register_video_device() failed code %d\n",
							 ntv2_vid->name, result);
		return result;
	}

	ntv2_vid->video_init = true;

	return 0;
}

void ntv2_video_update(struct ntv2_video *ntv2_vid)
{
	if (ntv2_vid == NULL)
		return;
	
	ntv2_channel_set_input_format(ntv2_vid->vid_str,
								  &ntv2_vid->input_format);
	ntv2_channel_set_video_format(ntv2_vid->vid_str,
								  &ntv2_vid->video_format);
	ntv2_channel_set_pixel_format(ntv2_vid->vid_str,
								  &ntv2_vid->pixel_format);
}

int ntv2_video_enable(struct ntv2_video *ntv2_vid)
{
	unsigned long flags;
	int result;

	if ((ntv2_vid == NULL) ||
		(ntv2_vid->ntv2_chn == NULL) ||
		(ntv2_vid->ntv2_pci == NULL))
		return -EPERM;

	if (ntv2_vid->transfer_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_VIDEO_STATE("%s: video transfer task enable\n", ntv2_vid->name);

	spin_lock_irqsave(&ntv2_vid->state_lock, flags);
	ntv2_vid->dma_vb2buf = NULL;
	ntv2_vid->dma_vidbuf = NULL;
	ntv2_vid->dma_start = false;
	ntv2_vid->dma_done = false;
	ntv2_vid->dma_result = 0;
	ntv2_vid->input_changed = false;
	ntv2_vid->transfer_state = ntv2_task_state_enable;
	spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);

	ntv2_channel_set_input_format(ntv2_vid->vid_str,
								  &ntv2_vid->input_format);
	ntv2_channel_set_video_format(ntv2_vid->vid_str,
								  &ntv2_vid->video_format);
	ntv2_channel_set_pixel_format(ntv2_vid->vid_str,
								  &ntv2_vid->pixel_format);
	ntv2_channel_set_frame_callback(ntv2_vid->vid_str,
									ntv2_video_channel_callback,
									(unsigned long)ntv2_vid);

	result = ntv2_channel_enable(ntv2_vid->vid_str);
	if (result != 0) {
		spin_lock_irqsave(&ntv2_vid->state_lock, flags);
		ntv2_vid->transfer_state = ntv2_task_state_disable;
		spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);
		return result;
	}

	/* schedule the transfer task */
	tasklet_schedule(&ntv2_vid->transfer_task);

	return 0;
}

int ntv2_video_disable(struct ntv2_video *ntv2_vid)
{
	unsigned long flags;
	int result;

	if ((ntv2_vid == NULL) || (ntv2_vid->ntv2_chn == NULL))
		return -EPERM;
	
	if (ntv2_vid->transfer_state != ntv2_task_state_enable)
		return 0;

	NTV2_MSG_VIDEO_STATE("%s: video transfer task disable\n", ntv2_vid->name);

	ntv2_channel_set_frame_callback(ntv2_vid->vid_str,
									NULL, 0);

	spin_lock_irqsave(&ntv2_vid->state_lock, flags);
	ntv2_vid->transfer_state = ntv2_task_state_disable;
	spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);

	/* schedule the transfer task */
	tasklet_schedule(&ntv2_vid->transfer_task);

	/* wait for transfer task stop */
	result = ntv2_wait((int*)&ntv2_vid->task_state,
					   (int)ntv2_task_state_disable,
					   NTV2_VIDEO_TRANSFER_TIMEOUT);
	if (result != 0) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* timeout waiting for transfer task stop\n",
							 ntv2_vid->name);
		return result;
	}

	ntv2_channel_flush(ntv2_vid->vid_str);
	ntv2_channel_disable(ntv2_vid->vid_str);

	return 0;
}

int ntv2_video_start(struct ntv2_video *ntv2_vid)
{
	int result;

	if ((ntv2_vid == NULL) || (ntv2_vid->ntv2_chn == NULL))
		return -EPERM;
	
	result = ntv2_channel_start(ntv2_vid->vid_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

int ntv2_video_stop(struct ntv2_video *ntv2_vid)
{
	int result;

	if ((ntv2_vid == NULL) || (ntv2_vid->ntv2_chn == NULL))
		return -EPERM;
	
	result = ntv2_channel_stop(ntv2_vid->vid_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

int ntv2_video_flush(struct ntv2_video *ntv2_vid)
{
	int result;

	if ((ntv2_vid == NULL) || (ntv2_vid->ntv2_chn == NULL))
		return -EPERM;
	
	result = ntv2_channel_flush(ntv2_vid->vid_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

static bool ntv2_video_compare_input_format(struct ntv2_input_format *format_a,
											struct ntv2_input_format *format_b)
{
	bool match;

	if ((format_a->frame_rate == ntv2_kona_frame_rate_none) ||
		(format_b->frame_rate == ntv2_kona_frame_rate_none))
		return false;

	match =
		((format_a->frame_rate == format_b->frame_rate) &&
		(format_a->video_standard == format_b->video_standard) &&
		(format_a->frame_flags == format_b->frame_flags) &&
		(format_a->pixel_flags == format_b->pixel_flags));

	return match;
}

static void ntv2_video_transfer_task(unsigned long data)
{
	struct ntv2_video *ntv2_vid = (struct ntv2_video*)data;
	struct ntv2_input_config *config;
	struct ntv2_input_format inpf;
	struct ntv2_transfer trn;
#ifdef NTV2_USE_V4L2_EVENT
	const struct v4l2_event event = {
		.type = V4L2_EVENT_SOURCE_CHANGE,
		.u.src_change.changes = V4L2_EVENT_SRC_CH_RESOLUTION,
	};
#endif
	unsigned long flags;
	bool dodma = false;
	int result = 0;

	spin_lock_irqsave(&ntv2_vid->state_lock, flags);
	if (!ntv2_vid->dma_start)
		ntv2_vid->task_state = ntv2_vid->transfer_state;
	if (ntv2_vid->task_state != ntv2_task_state_enable) {
		spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);
		return;
	}
	
	/* process dma complete */
	if ((ntv2_vid->dma_done) &&
		(ntv2_vid->dma_vidbuf != NULL) &&
		(ntv2_vid->dma_vb2buf != NULL)) {

		/* copy stream data to vb2 buffer */
		ntv2_video_stream_to_buffer(ntv2_vid, ntv2_vid->dma_vidbuf, ntv2_vid->dma_vb2buf);

		/* mark buffers as done */
		ntv2_channel_data_done(ntv2_vid->dma_vidbuf);
		ntv2_vb2ops_vb2buf_done(ntv2_vid->dma_vb2buf);

		/* clear current dma buffers */
		ntv2_vid->dma_vb2buf = NULL;
		ntv2_vid->dma_vidbuf = NULL;
		ntv2_vid->dma_start = false;
		ntv2_vid->dma_done = false;
		ntv2_vid->dma_result = 0;
	}

	/* look for dma work */
	if (!ntv2_vid->dma_start) {
		ntv2_vid->dma_vb2buf = ntv2_vb2ops_vb2buf_ready(ntv2_vid);
		if (ntv2_vid->dma_vb2buf != NULL) {
			ntv2_vid->dma_vidbuf = ntv2_channel_data_ready(ntv2_vid->vid_str);
			if (ntv2_vid->dma_vidbuf != NULL) {
				ntv2_vid->dma_start = true;
				dodma = true;
			}
		}
	}

	spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);

	/* queue work to dma engine */
	if (dodma) {
		trn.mode = ntv2_transfer_mode_c2s;
		trn.sg_offset = 0;
		if ((ntv2_vid->video_format.v4l2_timings.bt.height == 480) &&
			(ntv2_frame_geometry_height(ntv2_vid->video_format.frame_geometry) == 486)) {
			trn.sg_offset = 3 * ntv2_features_line_pitch(&ntv2_vid->pixel_format,
														 ntv2_frame_geometry_width(ntv2_vid->video_format.frame_geometry));
		}
		trn.sg_list = ntv2_vid->dma_vb2buf->sgtable->sgl;
		trn.sg_pages = ntv2_vid->dma_vb2buf->num_pages;
		trn.card_address[0] = ntv2_vid->dma_vidbuf->video.address + trn.sg_offset;
#ifdef NTV2_USE_VB2_V4L2_BUFFER
		trn.card_size[0] = vb2_plane_size(&ntv2_vid->dma_vb2buf->vb2_v4l2_buffer.vb2_buf, 0);
#else
		trn.card_size[0] = vb2_plane_size(&ntv2_vid->dma_vb2buf->vb2_buffer, 0);
#endif
		trn.card_address[1] = 0;
		trn.card_size[1] = 0;
		trn.callback_func = ntv2_video_dma_callback;
		trn.callback_data = (unsigned long)ntv2_vid;
		result = ntv2_pci_transfer(ntv2_vid->ntv2_pci, &trn);
		if (result != 0) {
			ntv2_vid->dma_done = true;
			ntv2_vid->dma_result = result;
		}
	}

	/* check for input changes */
	if (!ntv2_vid->input_changed) {
		config = ntv2_features_get_input_config(ntv2_vid->features,
												ntv2_vid->index,
												ntv2_vid->v4l2_input);
		if (config != NULL) {
			/* get current input format */
			ntv2_input_get_input_format(ntv2_vid->ntv2_inp, config, &inpf);
			if (!ntv2_video_compare_input_format(&inpf, &ntv2_vid->input_format)) {
				ntv2_vid->input_changed = true;
				NTV2_MSG_VIDEO_STATE("%s: video format changed\n", ntv2_vid->name);
#ifdef NTV2_USE_V4L2_EVENT
				v4l2_event_queue(&ntv2_vid->video_dev, &event);
#endif
			}
		}
	}
}

static void ntv2_video_dma_callback(unsigned long data, int result)
{
	struct ntv2_video *ntv2_vid = (struct ntv2_video *)data;
	unsigned long flags;

	if (ntv2_vid == NULL)
		return;

	/* record dma complete */
	spin_lock_irqsave(&ntv2_vid->state_lock, flags);
	ntv2_vid->dma_done = true;
	ntv2_vid->dma_result = result;
	spin_unlock_irqrestore(&ntv2_vid->state_lock, flags);

	/* schedule the dma task */
	tasklet_schedule(&ntv2_vid->transfer_task);
}

static void ntv2_video_channel_callback(unsigned long data)
{
	struct ntv2_video *ntv2_vid = (struct ntv2_video *)data;

	if (ntv2_vid == NULL)
		return;

	/* schedule the dma task */
	tasklet_schedule(&ntv2_vid->transfer_task);
}

static void ntv2_video_stream_to_buffer(struct ntv2_video *ntv2_vid,
										struct ntv2_stream_data *data,
										struct ntv2_vb2buf *buffer)
{
	struct v4l2_timecode *timecode;
	struct ntv2_timecode_packed pk;
	struct ntv2_timecode_data dt;
	u32 fps = 0;
	u32 type = 0;

	/* copy stream data to vb2 buffer */
#ifdef NTV2_USE_VB2_V4L2_BUFFER
#ifdef NTV2_USE_VB2_BUFFER_TIMESTAMP
	buffer->vb2_v4l2_buffer.vb2_buf.timestamp = data->timestamp;
#else
	buffer->vb2_v4l2_buffer.timestamp = data->timestamp;
#endif
	buffer->vb2_v4l2_buffer.sequence = ntv2_vid->vb2buf_sequence++;
	buffer->vb2_v4l2_buffer.field = ntv2_vid->v4l2_format.field;
#else
	buffer->vb2_buffer.v4l2_buf.timestamp = data->timestamp;
	buffer->vb2_buffer.v4l2_buf.bytesused = ntv2_vid->v4l2_format.sizeimage;
	buffer->vb2_buffer.v4l2_buf.sequence = ntv2_vid->vb2buf_sequence++;
	buffer->vb2_buffer.v4l2_buf.field = ntv2_vid->v4l2_format.field;
#endif	

	/* copy timecode if present */
	if (data->video.timecode_present) {

		/* get timecode frame rate */
		fps = ntv2_timecode_rate(ntv2_vid->input_format.frame_rate);

		switch (fps) {
		case 24:
			type = V4L2_TC_TYPE_24FPS;
			break;
		case 25:
			type = V4L2_TC_TYPE_25FPS;
			break;
		case 30:
			type = V4L2_TC_TYPE_30FPS;
			break;
		default:
			break;
		}

		/* must have a good timecode type */
		if (type != 0) {
#ifdef NTV2_USE_VB2_V4L2_BUFFER
			timecode = &buffer->vb2_v4l2_buffer.timecode;
			buffer->vb2_v4l2_buffer.flags |= V4L2_BUF_FLAG_TIMECODE;
#else
			timecode = &buffer->vb2_buffer.v4l2_buf.timecode;
			buffer->vb2_buffer.v4l2_buf.flags |= V4L2_BUF_FLAG_TIMECODE;
#endif
			/* unpack hardware timecode */
			pk.timecode_low = data->video.timecode_low;
			pk.timecode_high = data->video.timecode_high;
			ntv2_timecode_unpack(&dt, &pk, fps, false);

			/* fill in timecode info */
			timecode->type = type;
			timecode->flags = dt.drop_frame? V4L2_TC_FLAG_DROPFRAME : 0;
			timecode->frames = (u8)dt.frames;
			timecode->seconds = (u8)dt.seconds;
			timecode->minutes = (u8)dt.minutes;
			timecode->hours = (u8)dt.hours;
			timecode->userbits[0] = (u8)(dt.user_bits & 0xff);
			timecode->userbits[1] = (u8)((dt.user_bits >> 8) & 0xff);
			timecode->userbits[2] = (u8)((dt.user_bits >> 16) & 0xff);
			timecode->userbits[3] = (u8)((dt.user_bits >> 24) & 0xff);
		}
	}

	return;
}
