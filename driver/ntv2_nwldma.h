/*
 * NTV2 Northwest Logic DMA interface
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

#ifndef NTV2_NWLDMA_H
#define NTV2_NWLDMA_H

#include "ntv2_common.h"

#define NTV2_MAX_DMA_TASKS			64
#if 0
enum ntv2_nwldma_mode {
	ntv2_nwldma_mode_unknown,
	ntv2_nwldma_mode_s2c,
	ntv2_nwldma_mode_c2s,
	ntv2_nwldma_mode_size
};
#endif
enum ntv2_nwldma_state {
	ntv2_nwldma_state_unknown,
	ntv2_nwldma_state_idle,
	ntv2_nwldma_state_start,
	ntv2_nwldma_state_transfer,
	ntv2_nwldma_state_done,
	ntv2_nwldma_state_timeout,
	ntv2_nwldma_state_abort,
	ntv2_nwldma_state_size
};

/* nwl dma descriptor */
struct ntv2_nwldma_descriptor {
    u32	control;
    u32	byte_count;
    u64	system_address;
    u64	card_address;
    u64	next_address;
};

#if 0
typedef void (*ntv2_nwldma_callback)(unsigned long, int);
#endif

struct ntv2_dmatask {
	int						index;
	struct list_head		list;
	struct ntv2_nwldma		*ntv2_nwl;

	enum ntv2_transfer_mode mode;
	struct scatterlist		*sg_list;
	u32						sg_pages;
	u32						sg_offset;
	u32						card_address[2];
	u32						card_size[2];

	ntv2_transfer_callback	callback_func;
	unsigned long			callback_data;

	bool	dma_start;
	bool	dma_done;
	int		dma_result;
};

struct ntv2_nwldma {
	int						index;
	char					name[NTV2_STRING_SIZE];
	struct list_head		list;
	struct ntv2_device		*ntv2_dev;

	struct ntv2_register	*nwl_reg;
	
	struct tasklet_struct	engine_task;
	struct tasklet_struct	engine_dpc;
	struct timer_list 		engine_timer;
	enum ntv2_nwldma_state	engine_state;

	enum ntv2_transfer_mode	mode;
	spinlock_t 				state_lock;
	enum ntv2_task_state	dma_state;
	enum ntv2_task_state	task_state;

	u32						engine_number;
	u32						card_address_size;
	u32						max_transfer_size;
	u32						max_pages;
	u32						max_descriptors;

	struct ntv2_nwldma_descriptor	*descriptor;
	dma_addr_t						dma_descriptor;
	size_t							descriptor_memsize;
	
	struct ntv2_dmatask		*dma_task;
	u32						dpc_control_status;
	u32						descriptor_bytes;
	u32						descriptor_count;
	
	s64						transfer_start_count;
	s64						transfer_complete_count;
	s64						interrupt_count;
	s64						dpc_count;
	s64						error_count;

    s64						soft_transfer_time_start;
    s64						soft_dma_time_start;

    s64						stat_transfer_count;
    s64						stat_transfer_bytes;
    s64						stat_transfer_time;
    s64						stat_descriptor_count;
    s64						soft_transfer_time;
    s64						soft_dma_time;
    s64						stat_last_display_time;

	struct ntv2_dmatask		dmatask_array[NTV2_MAX_DMA_TASKS];

	struct list_head 		dmatask_ready_list;
	struct list_head 		dmatask_done_list;
};

struct ntv2_nwldma *ntv2_nwldma_open(struct ntv2_object *ntv2_obj,
									 const char *name, int index);
void ntv2_nwldma_close(struct ntv2_nwldma *ntv2_nwl);

int ntv2_nwldma_configure(struct ntv2_nwldma *ntv2_nwl, struct ntv2_register *nwl_reg);

int ntv2_nwldma_enable(struct ntv2_nwldma *ntv2_nwl);
int ntv2_nwldma_disable(struct ntv2_nwldma *ntv2_nwl);

int ntv2_nwldma_transfer(struct ntv2_nwldma *ntv2_nwl,
						 struct ntv2_transfer* ntv2_trn);

int ntv2_nwldma_interrupt(struct ntv2_nwldma *ntv2_nwl);
void ntv2_nwldma_abort(struct ntv2_nwldma *ntv2_nwl);

void ntv2_nwldma_interrupt_enable(struct ntv2_register *nwl_reg);
void ntv2_nwldma_interrupt_disable(struct ntv2_register *nwl_reg);

#endif
