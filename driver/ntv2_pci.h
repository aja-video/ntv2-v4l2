/*
 * NTV2 pci interface
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

#ifndef NTV2_PCI_H
#define NTV2_PCI_H

#include "ntv2_common.h"

struct ntv2_nwldma;
struct ntv2_xlxdma;

struct ntv2_pci {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	spinlock_t 						state_lock;
	enum ntv2_task_state			pci_state;

	enum ntv2_pci_type				pci_type;
	struct ntv2_register			*pci_reg;

	struct ntv2_nwldma				*nwl_engine[NTV2_MAX_DMA_ENGINES];
	struct ntv2_xlxdma				*xlx_engine[NTV2_MAX_DMA_ENGINES];
	int								num_engines;
};


struct ntv2_pci *ntv2_pci_open(struct ntv2_object *ntv2_obj,
							   const char *name, int index);
void ntv2_pci_close(struct ntv2_pci *ntv2_pci);

int ntv2_pci_configure(struct ntv2_pci *ntv2_pci,
					   enum ntv2_pci_type pci_type,
					   struct ntv2_register *pci_reg);

int ntv2_pci_enable(struct ntv2_pci *ntv2_pci);
int ntv2_pci_disable(struct ntv2_pci *ntv2_pci);

int ntv2_pci_transfer(struct ntv2_pci *ntv2_pci,
					  struct ntv2_transfer *ntv2_trn);

int ntv2_pci_interrupt(struct ntv2_pci *ntv2_pci);

#endif
