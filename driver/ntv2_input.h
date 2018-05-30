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

#ifndef NTV2_INPUT_H
#define NTV2_INPUT_H

#include "ntv2_common.h"

struct ntv2_features;
struct ntv2_register;
struct ntv2_input_config;
struct ntv2_source_config;
struct ntv2_hdmiin;
struct ntv2_hdmiin4;

struct ntv2_sdi_input_state {
	struct ntv2_sdi_input_status	lock_status;
	struct ntv2_sdi_input_status	last_status;
	int								lock_count;
	int								unlock_count;
	bool							locked;
	bool							changed;
};

struct ntv2_input {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	struct ntv2_features			*features;
	struct ntv2_register			*vid_reg;
	struct timer_list 				monitor_timer;
	spinlock_t 						state_lock;
	enum ntv2_task_state			monitor_state;

	int								num_sdi_inputs;
	int								num_hdmi_inputs;
	int								num_aes_inputs;

	int								num_hdmi0_inputs;
	int								num_hdmi4_inputs;
	
	struct ntv2_sdi_input_state		sdi_input_state[NTV2_MAX_SDI_INPUTS];
	struct ntv2_hdmiin				*hdmi0_input[NTV2_MAX_HDMI_INPUTS];
	struct ntv2_hdmiin4				*hdmi4_input[NTV2_MAX_HDMI_INPUTS];
};

struct ntv2_input *ntv2_input_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index);
void ntv2_input_close(struct ntv2_input *ntv2_inp);

int ntv2_input_configure(struct ntv2_input *ntv2_inp,
						 struct ntv2_features *features,
						 struct ntv2_register *vid_reg);

int ntv2_input_enable(struct ntv2_input *ntv2_inp);
int ntv2_input_disable(struct ntv2_input *ntv2_inp);

int ntv2_input_set_timecode_dbb(struct ntv2_input *ntv2_inp,
								struct ntv2_input_config *config,
								u32 dbb);

int ntv2_input_get_input_format(struct ntv2_input *ntv2_inp,
								struct ntv2_input_config *config,
								struct ntv2_input_format *format);

int ntv2_input_get_source_format(struct ntv2_input *ntv2_inp,
								 struct ntv2_source_config *config,
								 struct ntv2_source_format *format);

#endif	

