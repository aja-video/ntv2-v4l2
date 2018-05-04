/*
 * NTV2 HDMI4 EDID
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

#ifndef NTV2_HDMIEDID_H
#define NTV2_HDMIEDID_H

#include "ntv2_common.h"

#define NTV2_HDMI_EDID_SIZE			512


struct ntv2_hdmiedid {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	enum ntv2_edid_type				edid_type;
	u32								port_index;

	u8								edid_data[NTV2_HDMI_EDID_SIZE];
	u32								edid_size;
};


struct ntv2_hdmiedid *ntv2_hdmiedid_open(struct ntv2_object *ntv2_obj,
										 const char *name, int index);
void ntv2_hdmiedid_close(struct ntv2_hdmiedid *ntv2_hed);

int ntv2_hdmiedid_configure(struct ntv2_hdmiedid *ntv2_hed,
							enum ntv2_edid_type type,
							u32	port_index);

u8 *ntv2_hdmi_get_edid_data(struct ntv2_hdmiedid *ntv2_hed);
u32 ntv2_hdmi_get_edid_size(struct ntv2_hdmiedid *ntv2_hed);

#endif
