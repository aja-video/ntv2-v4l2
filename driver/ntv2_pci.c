/*
 * NTV2 generic pci interface
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

#include "ntv2_pci.h"
#include "ntv2_register.h"
#include "ntv2_nwldma.h"
#include "ntv2_nwlreg.h"

static void ntv2_pci_nwl_enable(struct ntv2_pci *ntv2_pci);
static void ntv2_pci_nwl_disable(struct ntv2_pci *ntv2_pci);
static struct ntv2_nwldma* ntv2_pci_nwl_config(struct ntv2_pci *ntv2_pci, int index);


struct ntv2_pci *ntv2_pci_open(struct ntv2_object *ntv2_obj,
							   const char *name, int index)
{
	struct ntv2_pci *ntv2_pci;

	if (ntv2_obj == NULL) 
		return NULL;

	ntv2_pci = kzalloc(sizeof(struct ntv2_pci), GFP_KERNEL);
	if (ntv2_pci == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_pci instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_pci->index = index;
	snprintf(ntv2_pci->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_pci->list);
	ntv2_pci->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_pci->state_lock);

	NTV2_MSG_PCI_INFO("%s: open ntv2_pci\n", ntv2_pci->name);

	return ntv2_pci;
}

void ntv2_pci_close(struct ntv2_pci *ntv2_pci)
{
	int i;

	if (ntv2_pci == NULL)
		return;

	NTV2_MSG_PCI_INFO("%s: close ntv2_pci\n", ntv2_pci->name);

	/* close all dma engines */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		for (i = 0; i < NTV2_MAX_DMA_ENGINES; i++)
		{
			if (ntv2_pci->nwl_engine[i] != NULL) {
				ntv2_nwldma_close(ntv2_pci->nwl_engine[i]);
				ntv2_pci->nwl_engine[i] = NULL;
			}
		}
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}
	
	kfree(ntv2_pci);
}

int ntv2_pci_configure(struct ntv2_pci *ntv2_pci,
					   enum ntv2_pci_type pci_type,
					   struct ntv2_register *pci_reg)
{
	if ((ntv2_pci == NULL) || (pci_reg == NULL))
		return -EPERM;

	NTV2_MSG_PCI_INFO("%s: configure pci interrupts and dma engines\n", ntv2_pci->name);

	ntv2_pci->pci_type = pci_type;
	ntv2_pci->pci_reg = pci_reg;

	/* get dma engines */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		ntv2_pci_nwl_disable(ntv2_pci);
		ntv2_pci->nwl_engine[0] = ntv2_pci_nwl_config(ntv2_pci, 4);
		if (ntv2_pci->nwl_engine[0] == NULL)
			return -EPERM;
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}
	
	return 0;
}

int ntv2_pci_enable(struct ntv2_pci *ntv2_pci)
{
	unsigned long flags;
	int i;

	if ((ntv2_pci == NULL) || (ntv2_pci->pci_reg == NULL))
		return -EPERM;

	if (ntv2_pci->pci_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_PCI_INFO("%s: enable interrupts\n", ntv2_pci->name);

	spin_lock_irqsave(&ntv2_pci->state_lock, flags);
	ntv2_pci->pci_state = ntv2_task_state_enable;

	/* enable dma engines and interrupts */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		ntv2_pci_nwl_enable(ntv2_pci);
		for (i = 0; i < NTV2_MAX_DMA_ENGINES; i++)
		{
			if (ntv2_pci->nwl_engine[i] != NULL) {
				ntv2_nwldma_enable(ntv2_pci->nwl_engine[i]);
			}
		}
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}

	spin_unlock_irqrestore(&ntv2_pci->state_lock, flags);

	return 0;
}

int ntv2_pci_disable(struct ntv2_pci *ntv2_pci)
{
	unsigned long flags;
	int i;

	if ((ntv2_pci == NULL) || (ntv2_pci->pci_reg == NULL))
		return -EPERM;

	if (ntv2_pci->pci_state == ntv2_task_state_disable)
		return 0;

	NTV2_MSG_PCI_INFO("%s: disable interrupts\n", ntv2_pci->name);

	spin_lock_irqsave(&ntv2_pci->state_lock, flags);

	/* disable dma engines and interrupts */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		for (i = 0; i < NTV2_MAX_DMA_ENGINES; i++)
		{
			if (ntv2_pci->nwl_engine[i] != NULL) {
				ntv2_nwldma_disable(ntv2_pci->nwl_engine[i]);
			}
		}
		ntv2_pci_nwl_disable(ntv2_pci);
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}

	ntv2_pci->pci_state = ntv2_task_state_disable;

	spin_unlock_irqrestore(&ntv2_pci->state_lock, flags);

	return 0;
}

int ntv2_pci_transfer(struct ntv2_pci *ntv2_pci,
					  struct ntv2_transfer *ntv2_trn)
{
	unsigned long flags;
	int result = -EPERM;

	if (ntv2_pci == NULL)
		return -EPERM;

	spin_lock_irqsave(&ntv2_pci->state_lock, flags);

	if (ntv2_pci->pci_state == ntv2_task_state_disable) {
		spin_unlock_irqrestore(&ntv2_pci->state_lock, flags);
		return 0;
	}

	/* pass transfer to proper dma engine */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		if (ntv2_pci->nwl_engine[0] != NULL)
			result = ntv2_nwldma_transfer(ntv2_pci->nwl_engine[0], ntv2_trn);
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}
	
	spin_unlock_irqrestore(&ntv2_pci->state_lock, flags);

	return result;
}

int ntv2_pci_interrupt(struct ntv2_pci *ntv2_pci)
{
	int result = IRQ_NONE;
	int res;
	int i;

	if (ntv2_pci == NULL)
		return IRQ_NONE;

	/* pass interrupt to dma engines */
	switch (ntv2_pci->pci_type)
	{
	case ntv2_pci_type_nwl:
		for (i = 0; i < NTV2_MAX_DMA_ENGINES; i++)
		{
			if (ntv2_pci->nwl_engine[i] != NULL) {
				res = ntv2_nwldma_interrupt(ntv2_pci->nwl_engine[i]);
				if (res == IRQ_HANDLED)
					result = IRQ_HANDLED;
			}
		}
		break;
	case ntv2_pci_type_xlx:
	default:
		break;
	}

	return result;
}

static void ntv2_pci_nwl_enable(struct ntv2_pci *ntv2_pci)
{
	/* enable nwl and user interrupts */
	ntv2_reg_write(ntv2_pci->pci_reg,
				   ntv2_nwldma_reg_common_control_status, 0,
				   NTV2_FLD_MASK(ntv2_nwldma_fld_dma_interrupt_enable) |
				   NTV2_FLD_MASK(ntv2_nwldma_fld_user_interrupt_enable));
}

static void ntv2_pci_nwl_disable(struct ntv2_pci *ntv2_pci)
{
	int num;
	int i;
	u32 val;
	
	/* disable nwl and user interrupts */
	ntv2_reg_write(ntv2_pci->pci_reg,
				   ntv2_nwldma_reg_common_control_status, 0,
				   0);

	/* disable nwl dma interrupts */
	num = NTV2_REG_COUNT(ntv2_nwldma_reg_capabilities);
	for (i = 0; i < num; i++) {
		val = ntv2_reg_read(ntv2_pci->pci_reg, ntv2_nwldma_reg_capabilities, i);
		if ((val & NTV2_FLD_MASK(ntv2_nwldma_fld_present)) != 0) {
			ntv2_reg_write(ntv2_pci->pci_reg, ntv2_nwldma_reg_engine_control_status, i, 0);
		}
	}
}

static struct ntv2_nwldma* ntv2_pci_nwl_config(struct ntv2_pci *ntv2_pci, int index)
{
	struct ntv2_nwldma *ntv2_nwl;
	int result;
	
	/* open and configure nwl dma engine */
	ntv2_nwl = ntv2_nwldma_open((struct ntv2_object*)ntv2_pci, "nwd", index);
	if (ntv2_nwl == NULL)
			return NULL;

	result = ntv2_nwldma_configure(ntv2_nwl, ntv2_pci->pci_reg);
	if (result != 0) {
		ntv2_nwldma_close(ntv2_nwl);
		return NULL;
	}

	result = ntv2_nwldma_enable(ntv2_nwl);
	if (result != 0) {
		ntv2_nwldma_close(ntv2_nwl);
		return NULL;
	}
	
	return ntv2_nwl;
}

