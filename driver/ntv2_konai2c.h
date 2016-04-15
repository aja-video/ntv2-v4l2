/*
 * NTV2 I2C register interface
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

#ifndef NTV2_KONAI2C_H
#define NTV2_KONAI2C_H

#include "ntv2_common.h"

struct ntv2_regsiter;

struct ntv2_konai2c {
	int						index;
	char					name[NTV2_STRING_SIZE];
	struct list_head		list;
	struct ntv2_device		*ntv2_dev;

	struct ntv2_register	*kona_reg;
	u32						kona_control;
	u32						kona_data;
	u8						i2c_device;
	u32						reset_count;
};

struct ntv2_konai2c *ntv2_konai2c_open(struct ntv2_object *ntv2_obj,
									   const char *name, int index);
void ntv2_konai2c_close(struct ntv2_konai2c *ntv2_i2c);

int ntv2_konai2c_configure(struct ntv2_konai2c *ntv2_i2c,
						   struct ntv2_register *ntv2_reg,
						   u32 control,
						   u32 data);

void ntv2_konai2c_set_device(struct ntv2_konai2c *ntv2_i2c, u8 device);
u8 ntv2_konai2c_get_device(struct ntv2_konai2c *ntv2_i2c);

int ntv2_konai2c_write(struct ntv2_konai2c *ntv2_i2c, u8 address, u8 data);
int ntv2_konai2c_cache_update(struct ntv2_konai2c *ntv2_i2c);
u8 ntv2_konai2c_cache_read(struct ntv2_konai2c *ntv2_i2c, u8 address);
int ntv2_konai2c_rmw(struct ntv2_konai2c *ntv2_i2c, u8 address, u8 data, u8 mask);

#endif
