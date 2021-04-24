/*
 * NTV2 device parameters
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

#define NTV2_REG_CONST
#include "ntv2_common.h"
#undef NTV2_REG_CONST
#include "ntv2_register.h"
#include "ntv2_nwlreg.h"
#include "ntv2_xlxreg.h"

static struct ntv2_module ntv2_mod_info;

static const char* stream_type_name[ntv2_stream_type_size] = {
	"unknown", "video input", "video output", "audio input", "audio output"
};

static const char* pci_type_name[ntv2_pci_type_size] = {
	"unknown", "nwl", "xlx"
};


/*
 * Initialize module info
 */
void ntv2_module_initialize(void)
{
	struct ntv2_module *ntv2_mod = &ntv2_mod_info;

	memset(ntv2_mod, 0, sizeof(struct ntv2_module));

	ntv2_mod->name = NTV2_MODULE_NAME;
	ntv2_mod->version = NTV2_DRV_VERSION;
	ntv2_mod->uart_max = NTV2_MAX_UARTS;
	ntv2_mod->cdev_max = NTV2_MAX_CDEVS;
	ntv2_mod->cdev_name = NTV2_CDEV_NAME;

	ntv2_mod->debug_mask =
		NTV2_DEBUG_INPUT_STATE |
//		NTV2_DEBUG_HDMIIN_STATE |
//		NTV2_DEBUG_HDMIIN_DETECT |
		NTV2_DEBUG_VIDEO_STATE |
//		NTV2_DEBUG_VIDEO_STREAM |
//		NTV2_DEBUG_CHANNEL_STATE |
//		NTV2_DEBUG_CHANNEL_STREAM |
//		NTV2_DEBUG_CHANNEL_STATISTICS |
//		NTV2_DEBUG_DMA_STATE |
//		NTV2_DEBUG_DMA_STREAM |
//		NTV2_DEBUG_DMA_DESCRIPTOR |
//		NTV2_DEBUG_AUDIO_STATE |
//		NTV2_DEBUG_DMA_STATISTICS |
		NTV2_DEBUG_INFO |
		NTV2_DEBUG_ERROR;

	INIT_LIST_HEAD(&ntv2_mod->device_list);
	spin_lock_init(&ntv2_mod->device_lock);
	atomic_set(&ntv2_mod->device_index, 0);
	atomic_set(&ntv2_mod->uart_index, 0);

	ntv2_mod->init = true;
}

void ntv2_module_release(void)
{
}

/*
 * Get module info singleton
 */
struct ntv2_module *ntv2_module_info(void)
{
	return &ntv2_mod_info;
}

s64 ntv2_system_time(void)
{
#ifdef NTV2_USE_KTIME
	struct timespec64 ts64;
	ktime_get_real_ts64(&ts64);

	return ((s64)ts64.tv_sec * 1000000) + (ts64.tv_nsec / 1000);
#else	
	struct timeval tv;
	do_gettimeofday(&tv);

	return ((s64)tv.tv_sec * 1000000 + tv.tv_usec);
#endif	
}

int ntv2_wait(int *event, int state, int timeout)
{
	if (event == NULL)
		return -EPERM;

	while (timeout > 0) {
		if (*event == state)
			break;
		msleep(1);
		timeout -= 1000;
	}
	if (timeout < 0)
		return -ETIME;

	return 0;
}

const char* ntv2_stream_name(enum ntv2_stream_type type)
{
	if (type >= ntv2_stream_type_size) return "bad stream type";
	
	return stream_type_name[type];
}


const char* ntv2_pci_name(enum ntv2_pci_type type)
{
	if (type >= ntv2_pci_type_size) return "bad pci type";

	return pci_type_name[type];
}


int ntv2_alloc_scatterlist(struct sg_table *sgt, u8* vm_buffer, u32 vm_size)
{
	struct scatterlist *sg;
	struct page *pg;
	u8 *buf;
	unsigned long nents;
	int i;
	int ret;

	if ((sgt == NULL) ||
		(vm_buffer == NULL) ||
		(vm_size == 0))
		return -EPERM;

	/* free the old scatterlist */
	ntv2_free_scatterlist(sgt);

	/* allocate the scatter buffer */
	nents = PAGE_ALIGN(vm_size) >> PAGE_SHIFT;
	ret = sg_alloc_table(sgt, nents, GFP_KERNEL);
	if (ret < 0) {
		return ret;
	}

	/* generate the scatter list */
	buf = vm_buffer;
	for_each_sg(sgt->sgl, sg, sgt->nents, i) {
		pg = vmalloc_to_page(buf);
		if (pg == NULL) {
			sg_free_table(sgt);
			return -ENOMEM;
		}
		sg_set_page(sg, pg, PAGE_SIZE, 0);
		buf += PAGE_SIZE;
	}

	return 0;
}

void ntv2_free_scatterlist(struct sg_table *sgt)
{
	if (sgt == NULL)
		return;

	sg_free_table(sgt);
}

