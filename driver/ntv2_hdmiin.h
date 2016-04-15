/*
 * NTV2 HDMI input control
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

#ifndef NTV2_HDMIIN_H
#define NTV2_HDMIIN_H

#include "ntv2_common.h"

struct ntv2_features;
struct ntv2_register;
struct ntv2_input_config;
struct ntv2_konai2c;

struct ntv2_hdmiin_format {
	u32								video_standard;
	u32								frame_rate;
	u32								frame_flags;
	u32								pixel_flags;
};

struct ntv2_hdmiin {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	struct ntv2_features			*features;
	struct ntv2_register			*vid_reg;
	struct ntv2_konai2c				*i2c_reg;
	spinlock_t 						state_lock;

	struct ntv2_hdmiin_format	 	video_format;

	u32								relock_reports;
	bool							hdmi_mode;
	bool							hdcp_mode;
	bool							derep_mode;
	bool							uhd_mode;
	bool							interlaced_mode;
	bool							pixel_double_mode;
	bool							yuv_mode;
	bool							deep_color_10bit;
	bool							deep_color_12bit;
	bool							prefer_yuv;
	bool							prefer_rgb;

	bool							cable_present;
	bool							clock_present;
	bool							input_locked;
	bool							avi_packet_present;
	bool							vsi_packet_present;

	u32								h_active_pixels;
	u32								h_total_pixels;
	u32								h_front_porch_pixels;
	u32								h_sync_pixels; 
	u32								h_back_porch_pixels;
	u32								v_total_lines0;
	u32								v_total_lines1;
	u32								v_active_lines0;
	u32								v_active_lines1;
	u32								v_sync_lines0;
	u32								v_sync_lines1;
	u32								v_front_porch_lines0;
	u32								v_front_porch_lines1;
	u32								v_back_porch_lines0;
	u32								v_back_porch_lines1;
	u32								v_frequency;
	u32								tmds_frequency;

	struct task_struct 				*monitor_task;
	enum ntv2_task_state			monitor_state;
};

struct ntv2_hdmiin *ntv2_hdmiin_open(struct ntv2_object *ntv2_obj,
									 const char *name, int index);
void ntv2_hdmiin_close(struct ntv2_hdmiin *ntv2_hin);

int ntv2_hdmiin_configure(struct ntv2_hdmiin *ntv2_hin,
						  struct ntv2_features *features,
						  struct ntv2_register *vid_reg);

int ntv2_hdmiin_enable(struct ntv2_hdmiin *ntv2_hin);
int ntv2_hdmiin_disable(struct ntv2_hdmiin *ntv2_hin);

int ntv2_hdmiin_get_video_format(struct ntv2_hdmiin *ntv2_hin,
								 struct ntv2_hdmiin_format *format);

int ntv2_hdmiin_periodic_update(struct ntv2_hdmiin *ntv2_hin);

#endif

