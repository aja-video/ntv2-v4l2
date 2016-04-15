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

#ifndef NTV2_VIDEO_H
#define NTV2_VIDEO_H

#include "ntv2_common.h"

struct ntv2_features;

struct ntv2_vb2buf {
	struct vb2_buffer			vb2_buffer;	/* must be first */

	int							index;
	struct list_head			list;
	struct ntv2_video			*ntv2_vid;

	bool						init;
	struct sg_table				*sgtable;
	struct sg_table				vmalloc_table;
	int							num_pages;
};

struct ntv2_video {
	int							index;
	char						name[NTV2_STRING_SIZE];
	struct list_head			list;
	struct ntv2_device			*ntv2_dev;

	struct ntv2_features 		*features;
	struct ntv2_channel 		*ntv2_chn;
	struct ntv2_input			*ntv2_inp;
	struct ntv2_nwldma			*dma_engine;
	bool						init;

	struct ntv2_channel_stream	*vid_str;
	struct ntv2_channel_stream	*aud_str;
	struct v4l2_device			v4l2_dev;
	struct v4l2_ctrl_handler	ctrl_handler;
	struct video_device			video_dev;
	struct mutex				video_mutex;
	bool						v4l2_init;
	bool						ctrl_init;
	bool						video_init;
	spinlock_t 					state_lock;
	struct tasklet_struct		transfer_task;
	enum ntv2_task_state		transfer_state;
	enum ntv2_task_state		task_state;
	atomic_t					video_ref;

	v4l2_std_id					v4l2_std;
	struct v4l2_dv_timings		v4l2_timings;
	struct v4l2_pix_format		v4l2_format;
	u32							v4l2_input;

	struct ntv2_pixel_format	pixel_format;
	struct ntv2_video_format	video_format;
	struct ntv2_input_format	input_format;

	struct vb2_queue			vb2_queue;
	struct mutex				vb2_mutex;
	spinlock_t 					vb2_lock;
	bool						vb2_init;
	bool						vb2_start;

	struct list_head 			vb2buf_list;
	int							vb2buf_index;
	u64 						vb2buf_sequence;

	struct ntv2_vb2buf			*dma_vb2buf;
	struct ntv2_stream_data		*dma_vidbuf;
	bool						dma_start;
	bool						dma_done;
	int							dma_result;
	bool						input_changed;
};

struct ntv2_video *ntv2_video_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index);
void ntv2_video_close(struct ntv2_video *ntv2_vid);

int ntv2_video_configure(struct ntv2_video *ntv2_vid,
						 struct ntv2_features *features,
						 struct ntv2_channel *ntv2_chn,
						 struct ntv2_input *ntv2_inp,
						 struct ntv2_nwldma *ntv2_nwl);

int ntv2_video_enable(struct ntv2_video *ntv2_vid);
int ntv2_video_disable(struct ntv2_video *ntv2_vid);

int ntv2_video_start(struct ntv2_video *ntv2_vid);
int ntv2_video_stop(struct ntv2_video *ntv2_vid);
int ntv2_video_flush(struct ntv2_video *ntv2_vid);

bool ntv2_video_compatible_input_format(struct ntv2_input_format *inpf,
										struct ntv2_video_format *vidf);

#endif
