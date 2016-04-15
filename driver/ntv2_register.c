/*
 * NTV2 PCI register interface
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

#include "ntv2_register.h"


struct ntv2_register *ntv2_register_open(struct ntv2_object *ntv2_obj,
										 const char *name, int index)
{
	struct ntv2_register *ntv2_reg = NULL;

	ntv2_reg = kzalloc(sizeof(struct ntv2_register), GFP_KERNEL);
	if (ntv2_reg == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_register instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_reg->index = index;
	snprintf(ntv2_reg->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_reg->list);
	ntv2_reg->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_reg->rmw_lock);

	return ntv2_reg;
}

void ntv2_register_close(struct ntv2_register *ntv2_reg)
{
	if (ntv2_reg == NULL)
		return;

	memset(ntv2_reg, 0, sizeof(struct ntv2_register));
	kfree(ntv2_reg);
}

int ntv2_register_configure(struct ntv2_register *ntv2_reg,
							void __iomem *base,
							u32 size)
{
	if (ntv2_reg == NULL)
		return -EPERM;

	NTV2_MSG_REGISTER_INFO("%s: configure register io\n", ntv2_reg->name);

	if ((base == NULL) || (size == 0)) {
		NTV2_MSG_REGISTER_ERROR("%s: *error* bad configuration  address 0x%px  size 0x%08x\n",
								ntv2_reg->name, base, size);
		return -EPERM;
	}

	ntv2_reg->base = base;
	ntv2_reg->size = size;
	ntv2_reg->enable = false;

	return 0;
}

int ntv2_register_enable(struct ntv2_register *ntv2_reg)
{
	unsigned long flags;

	if (ntv2_reg == NULL)
		return -EPERM;

	spin_lock_irqsave(&ntv2_reg->rmw_lock, flags);
	ntv2_reg->enable = true;
	spin_unlock_irqrestore(&ntv2_reg->rmw_lock, flags);

	return 0;
}

int ntv2_register_disable(struct ntv2_register *ntv2_reg)
{
	unsigned long flags;

	if (ntv2_reg == NULL)
		return -EPERM;

	spin_lock_irqsave(&ntv2_reg->rmw_lock, flags);
	ntv2_reg->enable = false;
	spin_unlock_irqrestore(&ntv2_reg->rmw_lock, flags);

	return 0;
}

u32 ntv2_register_read(struct ntv2_register *ntv2_reg, u32 regnum)
{
	void __iomem *address = NULL;
	unsigned long flags;
	u32 data = 0;

	if (ntv2_reg == NULL)
		return data;

	/* check bounds */
	if ((ntv2_reg->base == NULL) ||
		((regnum*4) >= ntv2_reg->size)) {
		NTV2_MSG_REGISTER_ERROR("%s: *error* register read failed regnum %d\n",
								ntv2_reg->name, regnum);
		return data;
	}

	address = (void __iomem*)(((u8*)ntv2_reg->base) + (regnum * 4));
	
	spin_lock_irqsave(&ntv2_reg->rmw_lock, flags);
	if (ntv2_reg->enable)
		data = ioread32(address);
	spin_unlock_irqrestore(&ntv2_reg->rmw_lock, flags);

	NTV2_MSG_REGISTER_READ("%s: read  reg %4d (0x%08x)  data 0x%08x\n",
						   ntv2_reg->name, regnum, regnum*4, data);

	return data;
}

void ntv2_register_write(struct ntv2_register *ntv2_reg, u32 regnum, u32 data)
{
	void __iomem *address = NULL;
	unsigned long flags;

	if (ntv2_reg == NULL)
		return;

	/* check bounds */
	if ((ntv2_reg->base == NULL) ||
		((regnum*4) >= ntv2_reg->size)) {
		NTV2_MSG_REGISTER_ERROR("%s: *error* register read failed regnum %d\n",
								ntv2_reg->name, regnum);
		return;
	}

	address = (void __iomem*)(((u8*)ntv2_reg->base) + (regnum * 4));

	spin_lock_irqsave(&ntv2_reg->rmw_lock, flags);
	if (ntv2_reg->enable)
		iowrite32(data, address);
	spin_unlock_irqrestore(&ntv2_reg->rmw_lock, flags);

	NTV2_MSG_REGISTER_WRITE("%s: write reg %4d (0x%08x)  data 0x%08x\n",
							ntv2_reg->name, regnum, regnum*4, data);
}

u32 ntv2_register_rmw(struct ntv2_register *ntv2_reg, u32 regnum, u32 data, u32 mask)
{
	void __iomem *address = NULL;
	u32 read_data = 0;
	u32 write_data = 0;
	unsigned long flags;

	if (ntv2_reg == NULL)
		return 0;

	/* check bounds */
	if ((ntv2_reg->base == NULL) ||
		((regnum*4) >= ntv2_reg->size)) {
		NTV2_MSG_REGISTER_ERROR("%s: *error* register read failed regnum %d\n",
								ntv2_reg->name, regnum);
		return 0;
	}

	address = (void __iomem*)(((u8*)ntv2_reg->base) + (regnum * 4));

	spin_lock_irqsave(&ntv2_reg->rmw_lock, flags);
	if (ntv2_reg->enable) {
		read_data = ioread32(address);
		write_data = (read_data & ~mask) | (data & mask);
		iowrite32(write_data, address);
	}
	spin_unlock_irqrestore(&ntv2_reg->rmw_lock, flags);

	NTV2_MSG_REGISTER_WRITE("%s: rmw   reg %4d (0x%08x)  data 0x%08x  mask 0x%08x  read 0x%08x  write 0x%08x\n",
							ntv2_reg->name, regnum, regnum*4, data, mask, read_data, write_data);

	return read_data;
}

u32 ntv2_reg_read(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index)
{
	if ((ntv2_reg == NULL) ||
		(reg == NULL) ||
		(index >= NTV2_REG_COUNT(reg)))
		return 0;

	return ntv2_register_read(ntv2_reg, NTV2_REG_NUM(reg, index));
}

void ntv2_reg_write(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index, u32 data)
{
	if ((ntv2_reg == NULL) ||
		(reg == NULL) ||
		(index >= NTV2_REG_COUNT(reg)))
		return;

	ntv2_register_write(ntv2_reg, NTV2_REG_NUM(reg, index), data);
}

u32 ntv2_reg_rmw(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index, u32 data, u32 mask)
{
	if ((ntv2_reg == NULL) ||
		(reg == NULL) ||
		(index >= NTV2_REG_COUNT(reg)))
		return 0;

	return ntv2_register_rmw(ntv2_reg, NTV2_REG_NUM(reg, index), data, mask);
}

