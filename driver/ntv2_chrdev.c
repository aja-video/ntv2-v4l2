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

#include "ntv2_chrdev.h"
#include "ntv2_features.h"
#include "ntv2_register.h"
#include "ntv2_konareg.h"

struct ntv2_register_access {
    u32			number;
    u32			value;
	u32			mask;
	u32			shift;
};

#define NTV2_DEVICE_TYPE		0xbb
#define IOCTL_NTV2_WRITE_REGISTER \
	_IOW(NTV2_DEVICE_TYPE, 48, struct ntv2_register_access)
#define IOCTL_NTV2_READ_REGISTER \
	_IOWR(NTV2_DEVICE_TYPE, 49 , struct ntv2_register_access)


static int ntv2_open(struct inode *inode, struct file *file)
{
    struct ntv2_chrdev *ntv2_chr = container_of(inode->i_cdev, struct ntv2_chrdev, cdev);

	NTV2_MSG_CHRDEV_STATE("%s: file open\n", ntv2_chr->name);

    file->private_data = ntv2_chr;

	return 0;	
}

static int ntv2_release(struct inode *inode, struct file *file)
{
    struct ntv2_chrdev *ntv2_chr = (struct ntv2_chrdev *)file->private_data;

	NTV2_MSG_CHRDEV_STATE("%s: file release\n", ntv2_chr->name);

	return 0;
}

static long ntv2_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct ntv2_chrdev *ntv2_chr = (struct ntv2_chrdev *)file->private_data;
	struct ntv2_register_access ra;
	u32 read_value;
	u32 write_value;

//	NTV2_MSG_CHRDEV_STATE("%s: file ioctl cmd %08x\n", ntv2_chr->name, cmd);

	switch (cmd) {
	case IOCTL_NTV2_WRITE_REGISTER:
		if(copy_from_user((void*)&ra, (const void*)arg, sizeof(struct ntv2_register_access)))
			return -EFAULT;

		if (ra.number == NTV2_DEBUG_REGISTER) {
			ntv2_module_info()->debug_mask = ra.value;
			break;
		}

		write_value = ra.value;
		if (ra.mask != 0xffffffff) {
			read_value = ntv2_register_read(ntv2_chr->vid_reg, ra.number);
			read_value &= ~ra.mask;
			write_value = (write_value << ra.shift) & ra.mask;
			write_value |= read_value;
		}
		ntv2_register_write(ntv2_chr->vid_reg, ra.number, write_value);
		break;

	case IOCTL_NTV2_READ_REGISTER:
		if(copy_from_user((void*)&ra, (const void*)arg, sizeof(struct ntv2_register_access)))
			return -EFAULT;

		if (ra.number == NTV2_DEBUG_REGISTER) {
			ra.value = ntv2_module_info()->debug_mask;
		} else if (ra.number == 70) {
			ra.value = 0;
		} else {
			ra.value = ntv2_register_read(ntv2_chr->vid_reg, ra.number);
			ra.value = (ra.value & ra.mask) >> ra.shift;
		}

		if(copy_to_user((void*)arg, (const void*)&ra, sizeof(struct ntv2_register_access)))
			return -EFAULT;
		break;
	default:
		return -EFAULT;
	}

	return 0;
}

static int ntv2_mmap(struct file *file, struct vm_area_struct* vma)
{
	return 0;
}

static loff_t ntv2_llseek(struct file *file, loff_t off, int whence)
{
	return 0;
}

static ssize_t ntv2_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static ssize_t ntv2_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static struct file_operations ntv2_file_ops =
{
	.owner 			= THIS_MODULE,
	.open			= ntv2_open,
	.release		= ntv2_release,
	.unlocked_ioctl	= ntv2_ioctl,
	.mmap			= ntv2_mmap,
	.llseek			= ntv2_llseek,
	.read			= ntv2_read,
	.write			= ntv2_write
};

struct ntv2_chrdev *ntv2_chrdev_open(struct ntv2_object *ntv2_obj,
								 const char *name, int index)
{
	struct ntv2_chrdev *ntv2_chr = NULL;

	ntv2_chr = kzalloc(sizeof(struct ntv2_chrdev), GFP_KERNEL);
	if (ntv2_chr == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_chrdev instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_chr->index = index;
	snprintf(ntv2_chr->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_chr->list);
	ntv2_chr->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_chr->state_lock);

	NTV2_MSG_CHRDEV_INFO("%s: open ntv2_chrdev\n", ntv2_chr->name);

	return ntv2_chr;
}

void ntv2_chrdev_close(struct ntv2_chrdev *ntv2_chr)
{
	if (ntv2_chr == NULL)
		return;

	NTV2_MSG_CHRDEV_INFO("%s: close ntv2_chrdev\n", ntv2_chr->name);

	ntv2_chrdev_disable(ntv2_chr);

	if (ntv2_chr->cdev.ops != NULL) {
		cdev_del(&ntv2_chr->cdev);
		ntv2_chr->cdev.ops = NULL;
	}

	memset(ntv2_chr, 0, sizeof(struct ntv2_chrdev));
	kfree(ntv2_chr);
}

int ntv2_chrdev_configure(struct ntv2_chrdev *ntv2_chr,
						struct ntv2_features *features,
						struct ntv2_register *vid_reg)
{
	struct ntv2_module *ntv2_mod = ntv2_module_info();
	int index;
	int res;

	if ((ntv2_chr == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_CHRDEV_INFO("%s: configure cdev\n", ntv2_chr->name);

	ntv2_chr->features = features;
	ntv2_chr->vid_reg = vid_reg;

	/* get next character device index */
	index = atomic_inc_return(&ntv2_mod->cdev_index) - 1;
	if (index >= ntv2_mod->cdev_max) {
		NTV2_MSG_CHRDEV_ERROR("%s: ntv2_cdev too many character devices %d\n", ntv2_chr->name, index);
		return -ENOMEM;
	}

	/* add new character device */
	cdev_init(&ntv2_chr->cdev, &ntv2_file_ops);
	ntv2_chr->cdev.owner = THIS_MODULE;

	res = cdev_add(&ntv2_chr->cdev,
				   MKDEV(MAJOR(ntv2_mod->cdev_number), index),
				   1);
	if (res < 0) {
		NTV2_MSG_CHRDEV_ERROR("%s: *error* cdev_add()  device %d  failed code %d\n",
							  ntv2_chr->name, index, res);
		return res;
	}

	return 0;
}

int ntv2_chrdev_enable(struct ntv2_chrdev *ntv2_chr)
{
	unsigned long flags;

	if (ntv2_chr == NULL)
		return -EPERM;

	if (ntv2_chr->task_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_CHRDEV_STATE("%s: file ops enable\n", ntv2_chr->name);

	spin_lock_irqsave(&ntv2_chr->state_lock, flags);
	ntv2_chr->task_state = ntv2_task_state_enable;
	spin_unlock_irqrestore(&ntv2_chr->state_lock, flags);

	return 0;
}

int ntv2_chrdev_disable(struct ntv2_chrdev *ntv2_chr)
{
	unsigned long flags;

	if (ntv2_chr == NULL)
		return -EPERM;
	
	if (ntv2_chr->task_state != ntv2_task_state_enable)
		return 0;

	NTV2_MSG_INPUT_STATE("%s: file ops disable\n", ntv2_chr->name);

	spin_lock_irqsave(&ntv2_chr->state_lock, flags);
	ntv2_chr->task_state = ntv2_task_state_disable;
	spin_unlock_irqrestore(&ntv2_chr->state_lock, flags);

	return 0;
}


