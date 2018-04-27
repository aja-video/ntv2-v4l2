/*
 * NTV2 Xilinx DMA interface
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

#define NTV2_REG_CONST
#include "ntv2_xlxdma.h"
#undef NTV2_REG_CONST
#include "ntv2_register.h"
#include "ntv2_xlxreg.h"


#define NTV2_XLXDMA_MAX_TRANSFER_SIZE		(64 * 1024 * 1024)
#define NTV2_XLXDMA_MAX_SEGMENT_SIZE		(15*4096)

#define NTV2_XLXDMA_TRANSFER_TIMEOUT		(100000)
#define NTV2_XLXDMA_STATISTIC_INTERVAL		(5000000)

#define NTV2_XLXDMA_MAX_FRAME_SIZE			(2048 * 1080 * 4 * 6)
#define NTV2_XLXDMA_MAX_PAGES				(NTV2_XLXDMA_MAX_FRAME_SIZE / PAGE_SIZE)

#define NTV2_XLXDMA_MAX_ADJACENT_COUNT		15

static void ntv2_xlxdma_task(unsigned long data);
static int ntv2_xlxdma_dodma(struct ntv2_xlxdma_task *ntv2_task);
static void ntv2_xlxdma_dpc(unsigned long data);
#ifdef NTV2_USE_TIMER_SETUP
static void ntv2_xlxdma_timeout(struct timer_list *timer);
#else
static void ntv2_xlxdma_timeout(unsigned long data);
#endif
static void ntv2_xlxdma_cleanup(struct ntv2_xlxdma *ntv2_xlx);
static void ntv2_xlxdma_stop(struct ntv2_xlxdma *ntv2_xlx);


struct ntv2_xlxdma *ntv2_xlxdma_open(struct ntv2_object *ntv2_obj,
									 const char *name, int index)
{
	struct ntv2_xlxdma *ntv2_xlx;

	if (ntv2_obj == NULL) 
		return NULL;

	ntv2_xlx = kzalloc(sizeof(struct ntv2_xlxdma), GFP_KERNEL);
	if (ntv2_xlx == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_xlxdma instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_xlx->index = index;
	snprintf(ntv2_xlx->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_xlx->list);
	ntv2_xlx->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_xlx->state_lock);

	/* dodma task */
	tasklet_init(&ntv2_xlx->engine_task,
				 ntv2_xlxdma_task,
				 (unsigned long)ntv2_xlx);

	/* interrupt dpc */
	tasklet_init(&ntv2_xlx->engine_dpc,
				 ntv2_xlxdma_dpc,
				 (unsigned long)ntv2_xlx);

	/* timeout timer */
#ifdef NTV2_USE_TIMER_SETUP	
	timer_setup(&ntv2_xlx->engine_timer,
				ntv2_xlxdma_timeout,
				0);
#else
	setup_timer(&ntv2_xlx->engine_timer,
				ntv2_xlxdma_timeout,
				(unsigned long)ntv2_xlx);
#endif
	
	return ntv2_xlx;
}

void ntv2_xlxdma_close(struct ntv2_xlxdma *ntv2_xlx)
{
	if (ntv2_xlx == NULL)
		return;

	/* stop the queue */
	ntv2_xlxdma_disable(ntv2_xlx);

	/* stop the dma engine and cleanup */
	ntv2_xlxdma_stop(ntv2_xlx);
	ntv2_xlxdma_cleanup(ntv2_xlx);

	/* stop the tasks */
	tasklet_kill(&ntv2_xlx->engine_dpc);
	tasklet_kill(&ntv2_xlx->engine_task);

	/* free the descriptor memory */
	if (ntv2_xlx->descriptor != NULL) {
		pci_free_consistent(ntv2_xlx->ntv2_dev->pci_dev,
							ntv2_xlx->descriptor_memsize,
							ntv2_xlx->descriptor,
							ntv2_xlx->dma_descriptor);
	}		

	kfree(ntv2_xlx);
}

int ntv2_xlxdma_configure(struct ntv2_xlxdma *ntv2_xlx, struct ntv2_register *xlx_reg)
{
	u32 max_channels;
	u32 value;
	u32 subsystem;
	u32 target;
	enum ntv2_transfer_mode	mode;
	u32	engine;
	u32	mask;
	u32 i;

	if ((ntv2_xlx == NULL) || (xlx_reg == NULL))
		return -EPERM;

	NTV2_MSG_DMA_INFO("%s: configure xlx dma engine\n", ntv2_xlx->name);

	/* check num engines */
	max_channels = NTV2_REG_COUNT(ntv2_xlxdma_reg_chn_identifier);
	if (ntv2_xlx->index >= max_channels) {
		NTV2_MSG_DMA_ERROR("%s: *error* dma engine index %d out of range\n",
						   ntv2_xlx->name, ntv2_xlx->index);
		return -EPERM;
	}
	ntv2_xlx->xlx_reg = xlx_reg;

	/* find channels */
	for (i = 0; i < max_channels; i++)
	{
		value = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_identifier, i);
		subsystem = NTV2_FLD_GET(ntv2_xlxdma_fld_chn_subsystem_id, value);
		target = NTV2_FLD_GET(ntv2_xlxdma_fld_chn_target, value);
		
		if (subsystem != ntv2_xlxdma_con_subsystem_id)
			continue;

		if (target == ntv2_xlxdma_con_target_channel_s2c) {
			mode = ntv2_transfer_mode_s2c;
			engine = ntv2_xlx->s2c_channels;
			mask = ((u32)0x1) << ntv2_xlx->s2c_channels;
			ntv2_xlx->s2c_channels++;
		}
		else if (target == ntv2_xlxdma_con_target_channel_c2s) {
			mode = ntv2_transfer_mode_c2s;
			engine = ntv2_xlx->c2s_channels;
			mask = ((u32)0x1) << (ntv2_xlx->s2c_channels + ntv2_xlx->c2s_channels);
			ntv2_xlx->c2s_channels++;
		}
		else {
			continue;
		}
		
		ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_control, i, 0x0);

		if (i == ntv2_xlx->index) {
			ntv2_xlx->mode = mode;
			ntv2_xlx->engine_number = engine;
			ntv2_xlx->interrupt_mask = mask;
		}
	}

	if (mode == ntv2_transfer_mode_unknown) {
		NTV2_MSG_DMA_ERROR("%s: *error* dma engine index %d not present\n",
						   ntv2_xlx->name, ntv2_xlx->index);
		return -EPERM;
	}

	/* configure engine constants */
	value = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_alignments, i);
	ntv2_xlx->card_address_size = ((u64)0x1) << NTV2_FLD_GET(ntv2_xlxdma_fld_chn_address_bits, value);
	ntv2_xlx->max_transfer_size = NTV2_XLXDMA_MAX_TRANSFER_SIZE;
	ntv2_xlx->max_pages = NTV2_XLXDMA_MAX_PAGES;
	ntv2_xlx->max_descriptors = NTV2_XLXDMA_MAX_PAGES * 2;

	/* allocate descriptor memory */
	ntv2_xlx->descriptor_memsize = ntv2_xlx->max_descriptors * sizeof(struct ntv2_xlxdma_descriptor);
	ntv2_xlx->descriptor = pci_alloc_consistent(ntv2_xlx->ntv2_dev->pci_dev,
												ntv2_xlx->descriptor_memsize,
												&ntv2_xlx->dma_descriptor);
	if (ntv2_xlx->descriptor == NULL) {
		NTV2_MSG_DMA_ERROR("%s: *error* descriptor memory allocation failed\n", ntv2_xlx->name);
		return -ENOMEM;
	}

	NTV2_MSG_DMA_INFO("%s: configure card address size 0x%08x  max transfer size 0x%08x\n",
					  ntv2_xlx->name,
					  ntv2_xlx->card_address_size,
					  ntv2_xlx->max_transfer_size);

	/* ready for dma transfer */
	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: idle\n", ntv2_xlx->name);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_idle;
	ntv2_xlx->dma_state = ntv2_task_state_disable;

	return 0;
}

int ntv2_xlxdma_enable(struct ntv2_xlxdma *ntv2_xlx)
{
	unsigned long flags;
	int result;
	int i;

	if (ntv2_xlx == NULL)
		return -EPERM;

	if (ntv2_xlx->dma_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_DMA_STATE("%s: xlx dma task enable\n", ntv2_xlx->name);

	/* initialize task lists */
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	INIT_LIST_HEAD(&ntv2_xlx->dmatask_ready_list);
	INIT_LIST_HEAD(&ntv2_xlx->dmatask_done_list);
	for (i = 0; i < NTV2_XLXDMA_MAX_TASKS; i++) {
		ntv2_xlx->dmatask_array[i].index = i;
		INIT_LIST_HEAD(&ntv2_xlx->dmatask_array[i].list);
		ntv2_xlx->dmatask_array[i].ntv2_xlx = ntv2_xlx;
		list_add_tail(&ntv2_xlx->dmatask_array[i].list, &ntv2_xlx->dmatask_done_list);
	}
	ntv2_xlx->stat_transfer_count = 0;
	ntv2_xlx->stat_transfer_bytes = 0;
	ntv2_xlx->stat_transfer_time = 0;
	ntv2_xlx->stat_descriptor_count = 0;
	ntv2_xlx->stat_last_display_time = ntv2_system_time();
	ntv2_xlx->soft_transfer_time = 0;
	ntv2_xlx->soft_dma_time = 0;
	ntv2_xlx->dma_state = ntv2_task_state_enable;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* schedule the engine task */
	tasklet_schedule(&ntv2_xlx->engine_task);

	/* wait for engine task start */
	result = ntv2_wait((int*)&ntv2_xlx->task_state,
					   (int)ntv2_task_state_enable,
					   NTV2_XLXDMA_TRANSFER_TIMEOUT);
	if (result != 0) {
		NTV2_MSG_DMA_ERROR("%s: *error* timeout waiting for engine task start\n",
						   ntv2_xlx->name);
		return result;
	}

	return 0;
}

int ntv2_xlxdma_disable(struct ntv2_xlxdma *ntv2_xlx)
{
	unsigned long flags;
	int result;

	if (ntv2_xlx == NULL)
		return -EPERM;

	if (ntv2_xlx->dma_state == ntv2_task_state_disable)
		return 0;

	NTV2_MSG_DMA_STATE("%s: xlx dma task disable\n", ntv2_xlx->name);

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->dma_state = ntv2_task_state_disable;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* schedule the dma task */
	tasklet_schedule(&ntv2_xlx->engine_task);

	/* wait for task stop */
	result = ntv2_wait((int*)&ntv2_xlx->task_state,
					   (int)ntv2_task_state_disable,
					   NTV2_XLXDMA_TRANSFER_TIMEOUT);
	if (result != 0) {
		NTV2_MSG_DMA_ERROR("%s: *error* timeout waiting for engine task stop\n", ntv2_xlx->name);
		return -ETIME;
	}

	/* shutdown dma engine */
	ntv2_xlxdma_abort(ntv2_xlx);

	/* wait for dma engine idle */
	result = ntv2_wait((int*)&ntv2_xlx->engine_state,
					   (int)ntv2_xlxdma_state_idle,
					   NTV2_XLXDMA_TRANSFER_TIMEOUT);
	if (result != 0) {
		NTV2_MSG_DMA_ERROR("%s: *error* timeout waiting for dma engine idle\n", ntv2_xlx->name);
		return -ETIME;
	}

	return 0;
}

int ntv2_xlxdma_transfer(struct ntv2_xlxdma *ntv2_xlx,
						 struct ntv2_transfer *ntv2_trn)
{
	struct ntv2_xlxdma_task *task = NULL;
	unsigned long flags;
	int task_index = 9999;

	if ((ntv2_trn == NULL) ||
		(ntv2_trn->sg_list == NULL) ||
		(ntv2_trn->sg_pages == 0) ||
		(ntv2_trn->card_size[0] == 0))
		return -EINVAL;

	/* get the next task */
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	if ((ntv2_xlx->dma_state == ntv2_task_state_enable) &&
		(!list_empty(&ntv2_xlx->dmatask_done_list))) {
		task = list_first_entry(&ntv2_xlx->dmatask_done_list, struct ntv2_xlxdma_task, list);
	
		task->mode = ntv2_trn->mode;
		task->sg_list = ntv2_trn->sg_list;
		task->sg_pages = ntv2_trn->sg_pages;
		task->sg_offset = ntv2_trn->sg_offset;
		task->card_address[0] = ntv2_trn->card_address[0];
		task->card_address[1] = ntv2_trn->card_address[1];
		task->card_size[0] = ntv2_trn->card_size[0];
		task->card_size[1] = ntv2_trn->card_size[1];
		task->callback_func = ntv2_trn->callback_func;
		task->callback_data = ntv2_trn->callback_data;
		task->dma_start = false;
		task->dma_done = false;
		task->dma_result = 0;

		task_index = task->index;
	
		list_del_init(&task->list);
		list_add_tail(&task->list, &ntv2_xlx->dmatask_ready_list);
	}
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (task != NULL) {
		NTV2_MSG_DMA_STREAM("%s: dma task queue %d  addr0 0x%08x  size0 %d  addr1 0x%08x  size1 %d\n",
							ntv2_xlx->name, task_index,
							ntv2_trn->card_address[0], ntv2_trn->card_size[0],
							ntv2_trn->card_address[1], ntv2_trn->card_size[1]);
	} else {
		NTV2_MSG_DMA_ERROR("%s: *error* dma transfer could not be queued\n",
						   ntv2_xlx->name);
		return -EAGAIN;
	}
	
	/* schedule the engine task */
	tasklet_schedule(&ntv2_xlx->engine_task);

	return 0;
}

static void ntv2_xlxdma_task(unsigned long data)
{
	struct ntv2_xlxdma *ntv2_xlx = (struct ntv2_xlxdma *)data;
	struct ntv2_xlxdma_task *task;
	unsigned long flags;
	int task_index;
	int result;
	int i;

	if (ntv2_xlx == NULL)
		return;

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->task_state = ntv2_xlx->dma_state;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (ntv2_xlx->task_state != ntv2_task_state_enable)
		return;

	for(i = 0; i < NTV2_XLXDMA_MAX_TASKS; i++) {
		task = NULL;
		task_index = 999;

		spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
		if (!list_empty(&ntv2_xlx->dmatask_ready_list)) {
			task = list_first_entry(&ntv2_xlx->dmatask_ready_list, struct ntv2_xlxdma_task, list);
			task_index = task->index;
		}
		spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

		if (task == NULL)
			return;

		/* task done */
		if (task->dma_done) {
			NTV2_MSG_DMA_STREAM("%s: dma task done %d\n",
								ntv2_xlx->name, task_index);

			if (task->callback_func != NULL)
				(*task->callback_func)(task->callback_data, task->dma_result);

			spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
			list_del_init(&task->list);
			list_add_tail(&task->list, &ntv2_xlx->dmatask_done_list);
			spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);
			continue;
		}
	
		/* task already started */
		if (task->dma_start)
			return;

		result = ntv2_xlxdma_dodma(task);
		if (result != 0) {
			if (task->callback_func != NULL)
				(*task->callback_func)(task->callback_data, result);

			spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
			list_del_init(&task->list);
			list_add_tail(&task->list, &ntv2_xlx->dmatask_done_list);
			spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);
			continue;
		}
		return;
	}

	NTV2_MSG_DMA_ERROR("%s: *error* dma task process reached max frames\n",
					   ntv2_xlx->name);
}

static int ntv2_xlxdma_dodma(struct ntv2_xlxdma_task *ntv2_task)
{
	struct ntv2_xlxdma *ntv2_xlx;
	struct ntv2_register *xlx_reg;
	struct scatterlist *sgentry;
	struct ntv2_xlxdma_descriptor *desc;
	struct ntv2_xlxdma_descriptor *desc_opt;
	struct ntv2_xlxdma_descriptor *desc_last;
	enum ntv2_xlxdma_state	state;
	unsigned long flags;
	u32		status;
	u32		control;
	u32		contig;
	u64		card_address;
	u64		system_address;
	u64		desc_next;
	u32		desc_count;
	u32		total_size;
	u32		data_size;
	u32		byte_count;
	u32		value;
	int		result;
	int		index;
	int		i;

	if ((ntv2_task == NULL) ||
		(ntv2_task->ntv2_xlx == NULL) ||
		(ntv2_task->sg_list == NULL) ||
		(ntv2_task->ntv2_xlx->xlx_reg == NULL))
		return -EPERM;

	ntv2_xlx = ntv2_task->ntv2_xlx;
	xlx_reg = ntv2_xlx->xlx_reg;
	index = ntv2_xlx->index;
	
	if (ntv2_task->mode != ntv2_xlx->mode) {
		NTV2_MSG_DMA_ERROR("%s: *error* transfer mode %d does not match engine mode %d\n",
						   ntv2_xlx->name, ntv2_task->mode, ntv2_xlx->mode);
		ntv2_xlx->error_count++;
		return -EINVAL;
	}

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	state = ntv2_xlx->engine_state;
	if (ntv2_xlx->engine_state == ntv2_xlxdma_state_idle) {
		ntv2_xlx->engine_state = ntv2_xlxdma_state_start;
	}
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (state != ntv2_xlxdma_state_idle)
		return -EBUSY;

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: start\n", ntv2_xlx->name);

	/* record the transfer start time */
	ntv2_xlx->soft_transfer_time_start = ntv2_system_time();

	/* count transfers */
	ntv2_xlx->transfer_start_count++;

	/* get dma parameters */
	ntv2_xlx->dma_task = ntv2_task;

	NTV2_MSG_DMA_STREAM("%s: xlx dma transfer card addr[0] 0x%08x  size[0] %d addr[1] 0x%08x  size[1] %d\n",
						ntv2_xlx->name,
						ntv2_task->card_address[0],
						ntv2_task->card_size[0],
						ntv2_task->card_address[1],
						ntv2_task->card_size[1]);

	if (ntv2_task->card_size[0] == 0) {
		NTV2_MSG_DMA_ERROR("%s: *error* transfer size is zero\n", ntv2_xlx->name);
		ntv2_xlxdma_cleanup(ntv2_xlx);
		ntv2_xlx->error_count++;
		result = -EINVAL;
		goto error_idle;
	}

	if ((ntv2_task->sg_list == NULL) ||
		(ntv2_task->sg_pages == 0)) {
		NTV2_MSG_DMA_ERROR("%s: *error* no scatter list\n", ntv2_xlx->name);
		ntv2_xlxdma_cleanup(ntv2_xlx);
		ntv2_xlx->error_count++;
		result = -EINVAL;
		goto error_idle;
	}

	if (ntv2_task->sg_pages >= ntv2_xlx->max_descriptors) {
		NTV2_MSG_DMA_ERROR("%s: *error* too many descriptor entries %d > %d\n",
						   ntv2_xlx->name,
						   ntv2_task->sg_pages,
						   ntv2_xlx->max_descriptors);
		ntv2_xlxdma_cleanup(ntv2_xlx);
		ntv2_xlx->error_count++;
		result = -EINVAL;
		goto error_idle;
	}

	/* read dma engine status register */
	status = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_status, index);

	/* make sure that engine is not running */
	if ((status & NTV2_FLD_MASK(ntv2_xlxdma_fld_chn_run)) != 0) {
		NTV2_MSG_DMA_ERROR("%s: *warn* dma running before start  status 0x%08x\n",
						   ntv2_xlx->name, status);
		ntv2_xlxdma_stop(ntv2_xlx);
		status = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_status, index);
		if ((status & NTV2_FLD_MASK(ntv2_xlxdma_fld_chn_run)) != 0)
		{
			NTV2_MSG_DMA_ERROR("%s: *error* dma running before start *again*  status 0x%08x\n",
							   ntv2_xlx->name, status);
			ntv2_xlxdma_cleanup(ntv2_xlx);
			ntv2_xlx->error_count++;
			result = -EAGAIN;
			goto error_idle;
		}
	}

	/* initialize descriptor generation */
	sgentry = ntv2_task->sg_list;
	card_address = ntv2_task->card_address[0];
	desc = ntv2_xlx->descriptor;
	desc_opt = desc;
	desc_last = desc;
	desc_next = ntv2_xlx->dma_descriptor + sizeof(struct ntv2_xlxdma_descriptor);
	desc_count = 0;
	data_size = 0;
	total_size = ntv2_task->card_size[0] + ntv2_task->card_size[1];
	ntv2_xlx->descriptor_count = 0;
	ntv2_xlx->descriptor_bytes = 0;
	control = NTV2_FLD_SET(ntv2_xlxdma_fld_desc_control_magic,
						   ntv2_xlxdma_con_desc_control_magic);

	for (i = 0; i < ntv2_task->sg_pages; i++) {
		system_address = sg_dma_address(sgentry);
		byte_count = sg_dma_len(sgentry);

		/* limit transfer to total size */
		if ((data_size + byte_count) > total_size)
			byte_count = total_size - data_size;

		/* handle split transfer */
		if ((ntv2_task->card_size[1] != 0) &&
			(data_size < ntv2_task->card_size[0]) &&
			((data_size + byte_count) >= ntv2_task->card_size[0])) {
			/* xlx can fetch up to 16 descriptors at once if they do not span pages */
			contig = (PAGE_SIZE - (((u32)desc_next) & 0xfff)) / sizeof(struct ntv2_xlxdma_descriptor);
			if (contig > 0)
				contig--;
			if (contig > NTV2_XLXDMA_MAX_ADJACENT_COUNT)
				contig = NTV2_XLXDMA_MAX_ADJACENT_COUNT;
			/* write descriptor for first fragment */
			desc->control = control;
			desc->control |= NTV2_FLD_SET(ntv2_xlxdma_fld_desc_control_count, contig);
			desc->byte_count = ntv2_task->card_size[0] - data_size;
			if (ntv2_xlx->mode == ntv2_transfer_mode_s2c) {
				desc->src_address = system_address;
				desc->dst_address = card_address;
			} else {
				desc->src_address = card_address;
				desc->dst_address = system_address;
			}
			desc->nxt_address = desc_next;
			/* setup for next fragment */
			system_address += desc->byte_count;
			card_address = ntv2_task->card_address[1];
			byte_count -= desc->byte_count;
			data_size += desc->byte_count;
			/* setup for next descriptor */
			desc_last = desc;
			desc++;
			desc_next += sizeof(struct ntv2_xlxdma_descriptor);
			if (desc_count > NTV2_XLXDMA_MAX_ADJACENT_COUNT)
				desc_opt++;
			desc_count++;
			if (desc_count >= ntv2_xlx->max_descriptors)
				break;
		}

		if (byte_count != 0) {
			/* xlx can fetch up to 16 descriptors at once if they do not span pages */
			contig = (PAGE_SIZE - (((u32)desc_next) & 0xfff)) / sizeof(struct ntv2_xlxdma_descriptor);
			if (contig > 0)
				contig--;
			if (contig > NTV2_XLXDMA_MAX_ADJACENT_COUNT)
				contig = NTV2_XLXDMA_MAX_ADJACENT_COUNT;
			/* write the descriptor */
			desc->control = control;
			desc->control |= NTV2_FLD_SET(ntv2_xlxdma_fld_desc_control_count, contig);
			desc->byte_count = byte_count;
			if (ntv2_xlx->mode == ntv2_transfer_mode_s2c) {
				desc->src_address = system_address;
				desc->dst_address = card_address;
			} else {
				desc->src_address = card_address;
				desc->dst_address = system_address;
			}
			desc->nxt_address = desc_next;
			/* log some descriptors */
			if (i < 5) {
				NTV2_MSG_DMA_DESCRIPTOR("%s: con %08x cnt %08x src %08x:%08x dst %08x:%08x nxt %08x:%08x\n",
										ntv2_xlx->name,
										desc->control,
										desc->byte_count,
										NTV2_U64_HIGH(desc->src_address),
										NTV2_U64_LOW(desc->src_address),
										NTV2_U64_HIGH(desc->dst_address),
										NTV2_U64_LOW(desc->dst_address),
										NTV2_U64_HIGH(desc->nxt_address),
										NTV2_U64_LOW(desc->nxt_address));
			}
			/* update card address and size */
			card_address += byte_count;
			data_size += byte_count;
			/* setup for next descriptor */
			desc_last = desc;
			desc++;
			desc_next += sizeof(struct ntv2_xlxdma_descriptor);
			if (desc_count > NTV2_XLXDMA_MAX_ADJACENT_COUNT)
				desc_opt++;
			desc_count++;
			if (desc_count >= ntv2_xlx->max_descriptors)
				break;
		}

		sgentry = sg_next(sgentry);
	}

	if (data_size == total_size) {
		/* zero final contig counts */
		desc = desc_opt;
		while(desc != desc_last) {
			desc->control = control;
			desc++;
		}
		/* last descriptor generates interrupt */
		desc_last->control = control;
		desc_last->control |= NTV2_FLD_SET(ntv2_xlxdma_fld_desc_control_stop, 1);
		desc_last->control |= NTV2_FLD_SET(ntv2_xlxdma_fld_desc_control_completion, 1);
		desc_last->nxt_address = 0;
		ntv2_xlx->descriptor_count = desc_count;
		ntv2_xlx->descriptor_bytes = data_size;
	} else {
		NTV2_MSG_DMA_ERROR("%s: *error* descriptor generation not complete\n",
						   ntv2_xlx->name);
		ntv2_xlxdma_cleanup(ntv2_xlx);
		ntv2_xlx->error_count++;
		result = -EINVAL;
		goto error_idle;
	}

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: transfer\n", ntv2_xlx->name);
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_transfer;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* write dma engine descriptor start */
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_seg_desc_address_low, index, 
				   NTV2_U64_LOW(ntv2_xlx->dma_descriptor));
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_seg_desc_address_high, index, 
				   NTV2_U64_HIGH(ntv2_xlx->dma_descriptor));
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_seg_desc_adjacent, ntv2_xlx->index, 
				   (desc_count > NTV2_XLXDMA_MAX_ADJACENT_COUNT)?NTV2_XLXDMA_MAX_ADJACENT_COUNT:0);

	/* record the dma start */
	ntv2_task->dma_start = true;
	ntv2_xlx->soft_dma_time_start = ntv2_system_time();

	/* clear dma status */
	ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_status_rc, index);
	/* enable pci irq */
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_irq_chn_interrupt_enable_w1s, index, 
				   ntv2_xlx->interrupt_mask);
	/* enable performance counts */
	value = NTV2_FLD_SET(ntv2_xlxdma_fld_chn_perf_auto, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_perf_clear, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_perf_run, 1);
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_perf_control, index, value);
	/* enable channel interrupt */
	value = NTV2_FLD_SET(ntv2_xlxdma_fld_chn_desc_stop, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_align_mismatch, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_magic_stop, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_read_error, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_desc_error, 1);
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_interrupt_enable_w1s, index, value);
	/* start dma engine */
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_run, 1);
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_control, index, value);

	/* start the dma timeout timer */
	mod_timer(&ntv2_xlx->engine_timer, jiffies +
			  usecs_to_jiffies(NTV2_XLXDMA_TRANSFER_TIMEOUT));

	return 0;

error_idle:
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_idle;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	return result;
}

int ntv2_xlxdma_interrupt(struct ntv2_xlxdma *ntv2_xlx)
{
	struct ntv2_register *xlx_reg;
	u32	request;
	int index;

	if ((ntv2_xlx == NULL) || (ntv2_xlx->xlx_reg == NULL))
		return IRQ_NONE;

	xlx_reg = ntv2_xlx->xlx_reg;
	index = ntv2_xlx->index;
	
	/* read status register */
	request = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_irq_chn_interrupt_request, index);

	/* check for interrupt active */
	if ((request & ntv2_xlx->interrupt_mask) != 0) {
		/* disable pci interrupt */
		ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_irq_chn_interrupt_enable_w1c, index, 
					   ntv2_xlx->interrupt_mask);
		/* disable channel interrupt */
		ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_interrupt_enable, index, 0);
		/* stop the dma engine */
		ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_chn_control, index, 0);

		/* count the interrupts */
		ntv2_xlx->interrupt_count++;

		/* schedule the dpc */
		tasklet_schedule(&ntv2_xlx->engine_dpc);

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void ntv2_xlxdma_dpc(unsigned long data)
{
	struct ntv2_xlxdma *ntv2_xlx = (struct ntv2_xlxdma *)data;
	struct ntv2_register *xlx_reg;
	enum ntv2_xlxdma_state state;
	unsigned long flags;
	bool	done = false;
	u32		status;
	u32		value;
	s64		stat_time;
	int		result = 0;
	int 	index;
	int		i;

	if ((ntv2_xlx == NULL) || (ntv2_xlx->xlx_reg == NULL))
		return;

	xlx_reg = ntv2_xlx->xlx_reg;
	index = ntv2_xlx->index;

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	state = ntv2_xlx->engine_state;
	if (ntv2_xlx->engine_state == ntv2_xlxdma_state_transfer) {
		ntv2_xlx->engine_state = ntv2_xlxdma_state_done;
	}
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (state != ntv2_xlxdma_state_transfer) {
		NTV2_MSG_DMA_ERROR("%s: *error* xlx dma dpc in bad state %d\n",
						   ntv2_xlx->name, state);
		ntv2_xlx->error_count++;
		return;
	}

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: done\n", ntv2_xlx->name);

	/* count dpc */
	ntv2_xlx->dpc_count++;

	/* stop the engine timer */
	del_timer_sync(&ntv2_xlx->engine_timer);

	/* wait for engine stop (can take a couple of register reads) */
	for (i = 0; i < 1000; i++) {
		status = ntv2_reg_read(xlx_reg, ntv2_xlxdma_reg_chn_status, index);
		if ((status & NTV2_FLD_MASK(ntv2_xlxdma_fld_chn_run)) != 0) {
			done = true;
			break;
		}
	}
	
	/* transfer complete */
	ntv2_xlx->transfer_complete_count++;

	/* check for success */
	value = NTV2_FLD_SET(ntv2_xlxdma_fld_chn_align_mismatch, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_magic_stop, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_read_error, 1);
	value |= NTV2_FLD_SET(ntv2_xlxdma_fld_chn_desc_error, 1);
	if ((status & value) == 0) {
		stat_time = ntv2_system_time();
		ntv2_xlx->stat_transfer_count++;
		ntv2_xlx->stat_transfer_bytes += ntv2_xlx->dma_task->card_size[0] + ntv2_xlx->dma_task->card_size[1];
		ntv2_xlx->stat_descriptor_count += ntv2_xlx->descriptor_count;

		ntv2_xlx->soft_transfer_time += stat_time - ntv2_xlx->soft_transfer_time_start;
		ntv2_xlx->soft_dma_time += stat_time - ntv2_xlx->soft_dma_time_start;

		if (stat_time > (ntv2_xlx->stat_last_display_time + NTV2_XLXDMA_STATISTIC_INTERVAL))
		{
			s64 stat_transfer_kbytes = ntv2_xlx->stat_transfer_bytes / 1000;
			s64 stat_transfer_time_us = ntv2_xlx->soft_transfer_time;

			if (ntv2_xlx->stat_transfer_count == 0) {
				ntv2_xlx->stat_transfer_count = 1;
			}
			if (stat_transfer_time_us == 0) {
				stat_transfer_time_us = 1;
			}

			if (NTV2_DEBUG_ACTIVE(NTV2_DEBUG_INFO)) {
				NTV2_MSG_DMA_STATISTICS("%s: dma dir %3s  eng %1d  cnt  %6d  size %6d (kB)  desc %6d\n",
										ntv2_xlx->name,
										(ntv2_xlx->mode == ntv2_transfer_mode_s2c)?"S2C":"C2S",
										ntv2_xlx->engine_number,
										(u32)(ntv2_xlx->stat_transfer_count),
										(u32)(stat_transfer_kbytes / ntv2_xlx->stat_transfer_count),
										(u32)(ntv2_xlx->stat_descriptor_count / ntv2_xlx->stat_transfer_count));

				NTV2_MSG_DMA_STATISTICS("%s: dma dir %3s  eng %1d  strn %6d  sdma %6d (us)  perf %6d (MB/s)\n",
										ntv2_xlx->name,
										(ntv2_xlx->mode == ntv2_transfer_mode_s2c)?"S2C":"C2S",
										ntv2_xlx->engine_number,
										(u32)(ntv2_xlx->soft_transfer_time / ntv2_xlx->stat_transfer_count),
										(u32)(ntv2_xlx->soft_dma_time / ntv2_xlx->stat_transfer_count),
										(u32)(stat_transfer_kbytes*1000 / stat_transfer_time_us));
			} else {
				NTV2_MSG_DMA_STATISTICS("%s: dma dir %3s  eng %1d  cnt %6d  size %6d (kB)  time %6d (us)  perf %6d (MB/s)\n",
										ntv2_xlx->name,
										(ntv2_xlx->mode == ntv2_transfer_mode_s2c)?"S2C":"C2S",
										ntv2_xlx->engine_number,
										(u32)(ntv2_xlx->stat_transfer_count),
										(u32)(stat_transfer_kbytes / ntv2_xlx->stat_transfer_count),
										(u32)(ntv2_xlx->soft_transfer_time / ntv2_xlx->stat_transfer_count),
										(u32)(stat_transfer_kbytes*1000 / stat_transfer_time_us));
			}

			ntv2_xlx->stat_transfer_count = 0;
			ntv2_xlx->stat_transfer_bytes = 0;
			ntv2_xlx->stat_transfer_time = 0;
			ntv2_xlx->stat_descriptor_count = 0;
			ntv2_xlx->stat_last_display_time = stat_time;
			ntv2_xlx->soft_transfer_time = 0;
			ntv2_xlx->soft_dma_time = 0;
		}
	}
	else
	{
		NTV2_MSG_DMA_ERROR("%s: *error* dma error status 0x%08x\n",
						   ntv2_xlx->name, status);
		ntv2_xlx->error_count++;
		result = -EIO;
	}

	/* report task completion status */
	if (ntv2_xlx->dma_task != NULL) {
		ntv2_xlx->dma_task->dma_done = true;
		ntv2_xlx->dma_task->dma_result = result;
	}

	/* release dma resources */
	ntv2_xlxdma_cleanup(ntv2_xlx);

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: idle\n", ntv2_xlx->name);
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_idle;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* schedule the engine task */
	tasklet_schedule(&ntv2_xlx->engine_task);
}

#ifdef NTV2_USE_TIMER_SETUP	
static void ntv2_xlxdma_timeout(struct timer_list *timer)
{
	struct ntv2_xlxdma		*ntv2_xlx = container_of(timer, struct ntv2_xlxdma, engine_timer);
#else
static void ntv2_xlxdma_timeout(unsigned long data)
{
	struct ntv2_xlxdma		*ntv2_xlx = (struct ntv2_xlxdma *)data;
#endif	
	enum ntv2_xlxdma_state	state;
	unsigned long			flags;
	u32						status;

	if (ntv2_xlx == NULL)
		return;

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	state = ntv2_xlx->engine_state;
	if (ntv2_xlx->engine_state == ntv2_xlxdma_state_transfer) {
		ntv2_xlx->engine_state = ntv2_xlxdma_state_timeout;
	}
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (state != ntv2_xlxdma_state_transfer) {
		NTV2_MSG_DMA_ERROR("%s: *error* xlx dma timeout in bad state %d\n",
						   ntv2_xlx->name, state);
		ntv2_xlx->error_count++;
		return;
	}

	/* read status register */
	status = ntv2_reg_read(ntv2_xlx->xlx_reg, ntv2_xlxdma_reg_chn_status, ntv2_xlx->index);
	NTV2_MSG_DMA_ERROR("%s: *error* dma engine state: timeout  status 0x%08x\n",
					   ntv2_xlx->name, status);

	/* stop transfer */
	ntv2_xlxdma_stop(ntv2_xlx);

	/* report task completion status */
	if (ntv2_xlx->dma_task != NULL) {
		ntv2_xlx->dma_task->dma_done = true;
		ntv2_xlx->dma_task->dma_result = -ETIME;
	}

	/* release dma resources */
	ntv2_xlxdma_cleanup(ntv2_xlx);

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: idle\n", ntv2_xlx->name);
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_idle;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* schedule the engine task */
	tasklet_schedule(&ntv2_xlx->engine_task);
}

void ntv2_xlxdma_abort(struct ntv2_xlxdma *ntv2_xlx)
{
	enum ntv2_xlxdma_state	state;
	unsigned long 			flags;

	if (ntv2_xlx == NULL)
		return;

	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	state = ntv2_xlx->engine_state;
	if (ntv2_xlx->engine_state == ntv2_xlxdma_state_transfer) {
		ntv2_xlx->engine_state = ntv2_xlxdma_state_abort;
	}
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	if (state != ntv2_xlxdma_state_transfer)
		return;

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: abort\n", ntv2_xlx->name);

	/* stop transfer */
	ntv2_xlxdma_stop(ntv2_xlx);

	/* report task completion status */
	if (ntv2_xlx->dma_task != NULL) {
		ntv2_xlx->dma_task->dma_done = true;
		ntv2_xlx->dma_task->dma_result = -ECANCELED;
	}

	/* release dma resources */
	ntv2_xlxdma_cleanup(ntv2_xlx);

	NTV2_MSG_DMA_STREAM("%s: xlx dma engine state: idle\n", ntv2_xlx->name);
	spin_lock_irqsave(&ntv2_xlx->state_lock, flags);
	ntv2_xlx->engine_state = ntv2_xlxdma_state_idle;
	spin_unlock_irqrestore(&ntv2_xlx->state_lock, flags);

	/* schedule the engine task */
	tasklet_schedule(&ntv2_xlx->engine_task);
}

static void ntv2_xlxdma_cleanup(struct ntv2_xlxdma *ntv2_xlx)
{
	if (ntv2_xlx == NULL)
		return;

	ntv2_xlx->dma_task = NULL;
	ntv2_xlx->descriptor_bytes = 0;
	ntv2_xlx->descriptor_count = 0;
}

static void ntv2_xlxdma_stop(struct ntv2_xlxdma *ntv2_xlx)
{
	if ((ntv2_xlx == NULL) || (ntv2_xlx->xlx_reg == NULL))
		return;

	/* stop the engine timer */
	del_timer(&ntv2_xlx->engine_timer);

	/* disable pci interrupt */
	ntv2_reg_write(ntv2_xlx->xlx_reg, ntv2_xlxdma_reg_irq_chn_interrupt_enable_w1c,
				   ntv2_xlx->index, ntv2_xlx->interrupt_mask);

	/* disable channel interrupt */
	ntv2_reg_write(ntv2_xlx->xlx_reg, ntv2_xlxdma_reg_chn_interrupt_enable, ntv2_xlx->index, 0);

	/* stop the dma engine */
	ntv2_reg_write(ntv2_xlx->xlx_reg, ntv2_xlxdma_reg_chn_control, ntv2_xlx->index, 0);
}

void ntv2_xlxdma_interrupt_enable(struct ntv2_register *xlx_reg)
{
	if (xlx_reg == NULL)
		return;

	/* enable user interrupts */
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_irq_usr_interrupt_enable, 0, 0x1);
}

void ntv2_xlxdma_interrupt_disable(struct ntv2_register *xlx_reg)
{
	if (xlx_reg == NULL)
		return;

	/* disable xlx and user interrupts */
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_irq_usr_interrupt_enable, 0, 0);
	ntv2_reg_write(xlx_reg, ntv2_xlxdma_reg_irq_chn_interrupt_enable, 0, 0);
}
