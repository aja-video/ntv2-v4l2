/*
 * NTV2 character device interface
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

#ifndef NTV2_CHRDEV_H
#define NTV2_CHRDEV_H

#include "ntv2_common.h"

struct ntv2_features;

struct ntv2_chrdev {
	int							index;
	char						name[NTV2_STRING_SIZE];
	struct list_head			list;
	struct ntv2_device			*ntv2_dev;

	struct ntv2_features 		*features;
	struct ntv2_register		*vid_reg;
	bool						init;

	struct cdev		 			cdev;
	spinlock_t 					state_lock;
	enum ntv2_task_state		task_state;
};

struct ntv2_chrdev *ntv2_chrdev_open(struct ntv2_object *ntv2_obj,
									 const char *name, int index);
void ntv2_chrdev_close(struct ntv2_chrdev *ntv2_chr);

int ntv2_chrdev_configure(struct ntv2_chrdev *ntv2_chr,
						struct ntv2_features *features,
						struct ntv2_register *vid_reg);

int ntv2_chrdev_enable(struct ntv2_chrdev *ntv2_chr);
int ntv2_chrdev_disable(struct ntv2_chrdev *ntv2_chr);

#endif
