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

#ifndef NTV2_REGISTER_H
#define NTV2_REGISTER_H

#include "ntv2_common.h"

struct ntv2_register {
	int					index;
	char				name[NTV2_STRING_SIZE];
	struct list_head	list;
	struct ntv2_device	*ntv2_dev;

	spinlock_t 			rmw_lock;
	void __iomem		*base;
	u32					size;
	bool				enable;
};

struct ntv2_register *ntv2_register_open(struct ntv2_object *ntv2_obj,
										 const char *name, int index);
void ntv2_register_close(struct ntv2_register *ntv2_reg);

int ntv2_register_configure(struct ntv2_register *ntv2_reg,
							void __iomem *base,
							u32 size);

int ntv2_register_enable(struct ntv2_register *ntv2_reg);
int ntv2_register_disable(struct ntv2_register *ntv2_reg);

/* access by register number */
u32 ntv2_register_read(struct ntv2_register *ntv2_reg, u32 regnum);
void ntv2_register_write(struct ntv2_register *ntv2_reg, u32 regnum, u32 data);
u32 ntv2_register_rmw(struct ntv2_register *ntv2_reg, u32 regnum, u32 data, u32 mask);

/* access by indexed register */
u32 ntv2_reg_read(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index);
void ntv2_reg_write(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index, u32 data);
u32 ntv2_reg_rmw(struct ntv2_register *ntv2_reg, const u32 *reg, u32 index, u32 data, u32 mask);

#endif
