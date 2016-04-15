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

#include "ntv2_konai2c.h"
#include "ntv2_konareg.h"
#include "ntv2_register.h"

#define NTV2_BUSY_TIMEOUT		10000
#define NTV2_WRITE_TIMEOUT		2000
#define NTV2_WAIT_TIME_MIN		500
#define NTV2_WAIT_TIME_MAX		1000
#define NTV2_READ_TIMEOUT		200000
#define NTV2_READ_TIME_MIN		5000
#define NTV2_READ_TIME_MAX		10000
#define NTV2_RESET_TIME_MIN		1000
#define NTV2_RESET_TIME_MAX		10000

static const u8		ntv2_subaddress_all = 0xff;

static int ntv2_konai2c_wait_for_busy(struct ntv2_konai2c *ntv2_i2c, u32 timeout);
static int ntv2_konai2c_wait_for_write(struct ntv2_konai2c *ntv2_i2c, u32 timeout);
static int ntv2_konai2c_wait_for_read(struct ntv2_konai2c *ntv2_i2c, u32 timeout);
static void ntv2_konai2c_reset(struct ntv2_konai2c *ntv2_i2c);

struct ntv2_konai2c *ntv2_konai2c_open(struct ntv2_object *ntv2_obj,
									   const char *name, int index)
{
	struct ntv2_konai2c *ntv2_i2c = NULL;

	ntv2_i2c = kzalloc(sizeof(struct ntv2_konai2c), GFP_KERNEL);
	if (ntv2_i2c == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_konai2c instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_i2c->index = index;
	snprintf(ntv2_i2c->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_i2c->list);
	ntv2_i2c->ntv2_dev = ntv2_obj->ntv2_dev;

	return ntv2_i2c;
}

void ntv2_konai2c_close(struct ntv2_konai2c *ntv2_i2c)
{
	if (ntv2_i2c == NULL)
		return;

	memset(ntv2_i2c, 0, sizeof(struct ntv2_konai2c));
	kfree(ntv2_i2c);
}

int ntv2_konai2c_configure(struct ntv2_konai2c *ntv2_i2c,
						   struct ntv2_register *ntv2_reg,
						   u32 control,
						   u32 data)
{
	if (ntv2_i2c == NULL)
		return -EPERM;

	NTV2_MSG_NWLDMA_INFO("%s: configure konai2c io\n", ntv2_i2c->name);

	if (ntv2_reg == NULL) {
		NTV2_MSG_KONAI2C_ERROR("%s: *error* bad configuration\n", ntv2_i2c->name);
		return -EPERM;
	}

	ntv2_i2c->kona_reg = ntv2_reg;
	ntv2_i2c->kona_control = control;
	ntv2_i2c->kona_data = data;

	return 0;
}

void ntv2_konai2c_set_device(struct ntv2_konai2c *ntv2_i2c, u8 device)
{
	u32 val;

	if (ntv2_i2c == NULL)
		return;

	ntv2_i2c->i2c_device = device;

	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, ntv2_i2c->i2c_device);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);
}

u8 ntv2_konai2c_get_device(struct ntv2_konai2c *ntv2_i2c)
{
	if (ntv2_i2c == NULL)
		return 0;

	return ntv2_i2c->i2c_device;
}

int ntv2_konai2c_write(struct ntv2_konai2c *ntv2_i2c, u8 address, u8 data)
{
	u32 val;
	int res;

	if (ntv2_i2c == NULL)
		return -EPERM;

	NTV2_MSG_KONAI2C_WRITE("%s: write dev %02x  add %02x  data %02x\n",
						   ntv2_i2c->name, ntv2_i2c->i2c_device, address, data);

	res = ntv2_konai2c_wait_for_busy(ntv2_i2c, NTV2_BUSY_TIMEOUT);
	if (res < 0)
		return res;

	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, ntv2_i2c->i2c_device);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, address);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_data_out, data);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_data, val);

	res = ntv2_konai2c_wait_for_write(ntv2_i2c, NTV2_WRITE_TIMEOUT);
	if (res < 0)
		return res;

	return 0;
}

int ntv2_konai2c_cache_update(struct ntv2_konai2c *ntv2_i2c)
{
	u32 val;
	int res;

	if (ntv2_i2c == NULL)
		return -EPERM;

	NTV2_MSG_KONAI2C_READ("%s: update device %02x read cache\n",
						  ntv2_i2c->name, ntv2_i2c->i2c_device);

	res = ntv2_konai2c_wait_for_busy(ntv2_i2c, NTV2_BUSY_TIMEOUT);
	if (res < 0)
		return res;

	/* enable i2c reads */
	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, ntv2_i2c->i2c_device);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, ntv2_subaddress_all);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 0);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	res = ntv2_konai2c_wait_for_read(ntv2_i2c, NTV2_READ_TIMEOUT);
	if (res < 0)
		return res;

	/* disable i2c reads */
	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, ntv2_i2c->i2c_device);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, ntv2_subaddress_all);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	res = ntv2_konai2c_wait_for_busy(ntv2_i2c, NTV2_BUSY_TIMEOUT);
	if (res < 0)
		return res;

	return 0;
}

u8 ntv2_konai2c_cache_read(struct ntv2_konai2c *ntv2_i2c, u8 address)
{
	u32 val;
	u32 data;
	int res;

	if (ntv2_i2c == NULL)
		return 0;

	res = ntv2_konai2c_wait_for_busy(ntv2_i2c, NTV2_BUSY_TIMEOUT);
	if (res < 0) {
		NTV2_MSG_KONAI2C_ERROR("%s: *error* read dev %02x  address %02x  failed\n",
							   ntv2_i2c->name, ntv2_i2c->i2c_device, address);
		return res;
	}

	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, ntv2_i2c->i2c_device);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, address);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_data);
	data = NTV2_FLD_GET(ntv2_kona_fld_hdmiin_data_in, val);

	NTV2_MSG_KONAI2C_READ("%s: read  dev %02x  add %02x  data %02x\n",
						  ntv2_i2c->name, ntv2_i2c->i2c_device, address, data);
	return (u8)data;
}

int ntv2_konai2c_rmw(struct ntv2_konai2c *ntv2_i2c, u8 address, u8 data, u8 mask)
{
	u8 val;

	val = ntv2_konai2c_cache_read(ntv2_i2c, address);
	val = (val & (~mask)) | (data & mask);
	return ntv2_konai2c_write(ntv2_i2c, address, val);
}

static int ntv2_konai2c_wait_for_busy(struct ntv2_konai2c *ntv2_i2c, u32 timeout)
{
	u32 val;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin_i2c_busy);
	int count = timeout / NTV2_WAIT_TIME_MIN;
	int i;

	if (timeout == 0) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		return ((val & mask) == 0)? 0 : -EBUSY;
	}

	for (i = 0; i < count; i++) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		if ((val & mask) == 0)
			return 0;
		usleep_range(NTV2_WAIT_TIME_MIN, NTV2_WAIT_TIME_MAX);
	}
	NTV2_MSG_KONAI2C_ERROR("%s: *error* wait for i2c busy failed - reset count %d\n",
						   ntv2_i2c->name, ntv2_i2c->reset_count);
	ntv2_konai2c_reset(ntv2_i2c);
	return -ETIME;
}

static int ntv2_konai2c_wait_for_write(struct ntv2_konai2c *ntv2_i2c, u32 timeout)
{
	u32 val;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin_write_busy);
	int count = timeout / NTV2_WAIT_TIME_MIN;
	int i;

	if (timeout == 0) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		return ((val & mask) == 0)? 0 : -EBUSY;
	}

	for (i = 0; i < count; i++) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		if ((val & mask) == 0)
			return 0;
		usleep_range(NTV2_WAIT_TIME_MIN, NTV2_WAIT_TIME_MAX);
	}
	NTV2_MSG_KONAI2C_ERROR("%s: *error* wait for i2c write failed - reset count %d\n",
						   ntv2_i2c->name, ntv2_i2c->reset_count);
	ntv2_konai2c_reset(ntv2_i2c);
	return -ETIME;
}

static int ntv2_konai2c_wait_for_read(struct ntv2_konai2c *ntv2_i2c, u32 timeout)
{
	u32 val;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin_ram_data_ready);
	int count = timeout / NTV2_WAIT_TIME_MIN;
	int i;

	if (timeout == 0) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		return ((val & mask) != 0)? 0 : -EBUSY;
	}

	for (i = 0; i < count; i++) {
		val = ntv2_register_read(ntv2_i2c->kona_reg, ntv2_i2c->kona_control);
		if ((val & mask) != 0)
			return 0;
		usleep_range(NTV2_READ_TIME_MIN, NTV2_READ_TIME_MAX);
	}
	ntv2_konai2c_reset(ntv2_i2c);
	NTV2_MSG_KONAI2C_ERROR("%s: *error* wait for i2c read failed - reset count %d\n",
						   ntv2_i2c->name, ntv2_i2c->reset_count);
	return -ETIME;
}

static void ntv2_konai2c_reset(struct ntv2_konai2c *ntv2_i2c)
{
	u32 val;

	ntv2_i2c->reset_count++;

	/* set reset */
	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_i2c_reset, 1);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	usleep_range(NTV2_RESET_TIME_MIN, NTV2_RESET_TIME_MAX);

	/* clear reset */
	val = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_device_address, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_subaddress, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_read_disable, 1);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_i2c_reset, 0);
	ntv2_register_write(ntv2_i2c->kona_reg, ntv2_i2c->kona_control, val);

	usleep_range(NTV2_RESET_TIME_MIN, NTV2_RESET_TIME_MAX);
}


