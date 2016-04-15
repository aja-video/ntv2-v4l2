/*
 * NTV2 Northwest Logic register constants
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

#ifndef NTV2_NWLREG_H
#define NTV2_NWLREG_H

#include "ntv2_common.h"

/* nwl dma engine capabilities registers */
NTV2_REG(ntv2_nwldma_reg_capabilities,					0x0, 0x40, 0x80, 0xc0, 0x800, 0x840, 0x880, 0x8c0);
NTV2_FLD(ntv2_nwldma_fld_present,						1,	0);		/* Indicates DMA Engine present at this location */
NTV2_FLD(ntv2_nwldma_fld_engine_direction,				1,	1);		/* Indicates DMA data flow direction */
NTV2_CON(ntv2_nwldma_con_system_to_card,				0);			/* DMA engine transfers from system to card */
NTV2_CON(ntv2_nwldma_con_card_to_system,				1);			/* DMA engine transfers from card to system */
NTV2_FLD(ntv2_nwldma_fld_engine_type,					2,	4);		/* Indicates the type of DMA Engine that this register set controls */
NTV2_CON(ntv2_nwldma_con_block_dma,						0);			/* DMA engine transfers data in block mode */
NTV2_CON(ntv2_nwldma_con_packet_dma,					1);			/* DMA engine transfers data in packet mode */
NTV2_FLD(ntv2_nwldma_fld_engine_number,					8,	8);		/* Identifies which DMA engine this register set controls */
NTV2_FLD(ntv2_nwldma_fld_address_size,					8,	16);	/* Identifies the size of the card address space */

/* control interrupt generation and report dma engine status registers */
NTV2_REG(ntv2_nwldma_reg_engine_control_status,			0x1, 0x41, 0x81, 0xc1, 0x801, 0x841, 0x881, 0x8c1);
NTV2_FLD(ntv2_nwldma_fld_interrupt_enable,				1,	0);		/* Enables/disables the generation of interrupts for this DMA Engine */
NTV2_FLD(ntv2_nwldma_fld_interrupt_active,				1,	1);		/* Interrupt_Active (write a 1 to clear) */
NTV2_FLD(ntv2_nwldma_fld_chain_start,					1,	8);		/* Setting this bit starts the Descriptor Engine */
NTV2_FLD(ntv2_nwldma_fld_status_dma_reset_request,		1,	9);		/* Software sets this bit to abort a DMA operation in process */
NTV2_FLD(ntv2_nwldma_fld_chain_running,					1,	10);	/* 1 = DMA descriptor chain running */
NTV2_FLD(ntv2_nwldma_fld_chain_complete,				1,	11);	/* 1 = DMA descriptor chain complete */
NTV2_FLD(ntv2_nwldma_fld_chain_error_short,				1,	12);	/* Set whenever a chain completes due to an error */
NTV2_FLD(ntv2_nwldma_fld_chain_software_short,			1,	13);	/* Set whenever a chain completes due to a software requested stop */
NTV2_FLD(ntv2_nwldma_fld_chain_hardware_short,			1,	14);	/* Set whenever a chain completes due to a software requested stop */
NTV2_FLD(ntv2_nwldma_fld_alignment_error,				1,	15);	/* Set whenever Chain_Start_Descriptor_Pointer is not aligned */
NTV2_FLD(ntv2_nwldma_fld_dma_reset,						1,	23);	/* Write 1 to reset the DMA Engine */

/* first descriptor low address registers */
NTV2_REG(ntv2_nwldma_reg_chain_start_address_low,		0x2, 0x42, 0x82, 0xc2, 0x802, 0x842, 0x882, 0x8c2);
/* first descriptor high address registers */
NTV2_REG(ntv2_nwldma_reg_chain_start_address_high,		0x3, 0x43, 0x83, 0xc3, 0x803, 0x843, 0x883, 0x8c3);
/* hardware dma time (nanoseconds) registers */
NTV2_REG(ntv2_nwldma_reg_hardware_time,					0x4, 0x44, 0x84, 0xc4, 0x804, 0x844, 0x884, 0x8c4);
/* hardware dma bytes transferred registers */
NTV2_REG(ntv2_nwldma_reg_chain_complete_byte_count,		0x5, 0x45, 0x85, 0xc5, 0x805, 0x845, 0x885, 0x8c5);

/* nwl dma common control status register */
NTV2_REG(ntv2_nwldma_reg_common_control_status,			0x1000);
NTV2_FLD(ntv2_nwldma_fld_dma_interrupt_enable,			1,	0);		/* Globally enables/disables interrupts for all DMA Engines */
NTV2_FLD(ntv2_nwldma_fld_dma_interrupt_active,			1,	1);		/* Reflects the state of the DMA interrupt hardware output */
NTV2_FLD(ntv2_nwldma_fld_dma_interrupt_pending,			1,	2);		/* Reflects the state of the DMA interrupt */
NTV2_FLD(ntv2_nwldma_fld_interrupt_mode,				1,	3);		/* 0 = Legacy, 1 = MSI */
NTV2_FLD(ntv2_nwldma_fld_user_interrupt_enable,			1,	4);		/* Enables/disables the generation of user interrupts */
NTV2_FLD(ntv2_nwldma_fld_user_interrupt_active,			1,	5);		/* Reflects the state of user interrupt */
NTV2_FLD(ntv2_nwldma_fld_max_payload_size,				3,	8);		/* Maximum payload size used by the DMA back-end core */
NTV2_CON(ntv2_nwldma_con_playload_size_128_bytes,		0);			/* 128 byte payload */
NTV2_CON(ntv2_nwldma_con_playload_size_256_bytes,		1);			/* 256 byte payload */
NTV2_CON(ntv2_nwldma_con_playload_size_512_bytes,		2);			/* 512 byte payload */
NTV2_FLD(ntv2_nwldma_fld_max_read_request_size,			3,	12);	/* Maximum read request size used by the DMA back-end core */
NTV2_CON(ntv2_nwldma_con_request_size_128_bytes,		0);			/* 128 byte read request size */
NTV2_CON(ntv2_nwldma_con_request_size_256_bytes,		1);			/* 256 byte read request size */
NTV2_CON(ntv2_nwldma_con_request_size_512_bytes,		2);			/* 512 byte read request size */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_0,		1,	16);	/* System to card DMA engine 0 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_1,		1,	17);	/* System to card DMA engine 1 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_2,		1,	18);	/* System to card DMA engine 2 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_3,		1,	19);	/* System to card DMA engine 3 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_4,		1,	20);	/* System to card DMA engine 4 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_5,		1,	21);	/* System to card DMA engine 5 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_6,		1,	22);	/* System to card DMA engine 6 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_s2c_interrupt_status_7,		1,	23);	/* System to card DMA engine 7 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_0,		1,	24);	/* Card to system DMA engine 0 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_1,		1,	25);	/* Card to system DMA engine 1 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_2,		1,	26);	/* Card to system DMA engine 2 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_3,		1,	27);	/* Card to system DMA engine 3 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_4,		1,	28);	/* Card to system DMA engine 4 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_5,		1,	29);	/* Card to system DMA engine 5 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_6,		1,	30);	/* Card to system DMA engine 6 interrupt active bit */
NTV2_FLD(ntv2_nwldma_fld_c2s_interrupt_status_7,		1,	31);	/* Card to system DMA engine 7 interrupt active bit */

/* version registers */
NTV2_REG(ntv2_nwldma_reg_backend_core_version,			0x1001);	/* Version of DMA back-end core */
NTV2_REG(ntv2_nwldma_reg_pci_express_core_version,		0x1002);	/* Version of PCI express core that is connected to the DMA back-end */
NTV2_REG(ntv2_nwldma_reg_user_version,					0x1003);	/* Version of User firmware */

/* nwl dma descriptor control bits */
NTV2_FLD(ntv2_nwldma_fld_control_irq_on_completion,		1,	0);
NTV2_FLD(ntv2_nwldma_fld_control_irq_on_short_err,		1,	1);
NTV2_FLD(ntv2_nwldma_fld_control_irq_on_short_sw,		1,	2);
NTV2_FLD(ntv2_nwldma_fld_control_irq_on_short_hw,		1,	3);
NTV2_FLD(ntv2_nwldma_fld_control_sequence,				1,	4);
NTV2_FLD(ntv2_nwldma_fld_control_continue,				1,	5);

#endif
