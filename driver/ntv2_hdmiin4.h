/*
 * NTV2 HDMI4 input control
 *
 * Copyright 2018 AJA Video Systems Inc. All rights reserved.
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

#ifndef NTV2_HDMIIN4_H
#define NTV2_HDMIIN4_H

#include "ntv2_common.h"

struct ntv2_features;
struct ntv2_register;
struct ntv2_input_config;
struct ntv2_konai2c;
struct ntv2_hdmiedid;

#define NTV2_HDMIIN4_STRING_SIZE	80


struct ntv2_hdmiin4_format {
	u32								video_standard;
	u32								frame_rate;
	u32								frame_flags;
	u32								pixel_flags;
	u32								audio_detect;
};

struct ntv2_hdmiin4 {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	struct ntv2_features			*features;
	struct ntv2_register			*vid_reg;
	struct ntv2_hdmiedid			*edid;
	spinlock_t 						state_lock;

	struct ntv2_hdmiin4_format	 	input_format;

	u32								video_control;
	u32								video_detect0;
	u32								video_detect1;
	u32								video_detect2;
	u32								video_detect3;
	u32								video_detect4;
	u32								video_detect5;
	u32								video_detect6;
	u32								video_detect7;
	u32								tmds_rate;

	bool							input_locked;
	bool							hdmi_mode;
	u32								video_standard;
	u32								frame_rate;
	u32								color_space;
	u32								color_depth;

	bool							audio_swap;

	u32								format_clock_count;
	u32								format_raster_count;

	struct task_struct 				*monitor_task;
	enum ntv2_task_state			monitor_state;
};

struct ntv2_hdmiin4 *ntv2_hdmiin4_open(struct ntv2_object *ntv2_obj,
									  const char *name, int index);
void ntv2_hdmiin4_close(struct ntv2_hdmiin4 *ntv2_hin);

int ntv2_hdmiin4_configure(struct ntv2_hdmiin4 *ntv2_hin,
						   struct ntv2_features *features,
						   struct ntv2_register *vid_reg,
						   int port_index);

int ntv2_hdmiin4_enable(struct ntv2_hdmiin4 *ntv2_hin);
int ntv2_hdmiin4_disable(struct ntv2_hdmiin4 *ntv2_hin);

int ntv2_hdmiin4_get_input_format(struct ntv2_hdmiin4 *ntv2_hin,
								  struct ntv2_hdmiin4_format *format);

int ntv2_hdmiin4_periodic_update(struct ntv2_hdmiin4 *ntv2_hin);

#endif

