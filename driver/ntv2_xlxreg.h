/*
 * NTV2 Xilinx PCI register constants
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

#ifndef NTV2_XLXREG_H
#define NTV2_XLXREG_H

#include "ntv2_common.h"

/* xilinx target register space id and sizes */
NTV2_CON(ntv2_xlxdma_con_target_size,						0x0400);
NTV2_CON(ntv2_xlxdma_con_channel_size,						0x0040);
NTV2_CON(ntv2_xlxdma_con_subsystem_id,						0x01fc);

/* xilinx subsystem target offsets */
NTV2_CON(ntv2_xlxdma_con_target_channel_s2c,				0x0);
NTV2_CON(ntv2_xlxdma_con_target_channel_c2s,				0x1);
NTV2_CON(ntv2_xlxdma_con_target_irq,						0x2);
NTV2_CON(ntv2_xlxdma_con_target_config,						0x3);
NTV2_CON(ntv2_xlxdma_con_target_sgdma_s2c,					0x4);
NTV2_CON(ntv2_xlxdma_con_target_sgdma_c2s,					0x5);
NTV2_CON(ntv2_xlxdma_con_target_sgdma_common,				0x6);
NTV2_CON(ntv2_xlxdma_con_target_msix,						0x8);

/* xilinx s2c identifier register */
NTV2_REG(ntv2_xlxdma_reg_chn_identifier,					0x0000, 0x0040, 0x0080, 0x00c0, 0x0400, 0x0440, 0x0480, 0x04c0);
NTV2_FLD(ntv2_xlxdma_fld_chn_version,						8,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_id_target,						4,	8);
NTV2_FLD(ntv2_xlxdma_fld_chn_stream_interface,				1,	15);
NTV2_FLD(ntv2_xlxdma_fld_chn_target,						4,	16);
NTV2_FLD(ntv2_xlxdma_fld_chn_subsystem_id,					12,	20);

/* xilinx s2c control registers */
NTV2_REG(ntv2_xlxdma_reg_chn_control,						0x0001, 0x0041, 0x0081, 0x00c1, 0x0401, 0x0441, 0x0481, 0x04c1);
NTV2_REG(ntv2_xlxdma_reg_chn_control_w1s,					0x0002, 0x0042, 0x0082, 0x00c2, 0x0402, 0x0442, 0x0482, 0x04c2);
NTV2_REG(ntv2_xlxdma_reg_chn_control_w1c,					0x0003, 0x0043, 0x0083, 0x00c3, 0x0403, 0x0443, 0x0483, 0x04c3);
NTV2_FLD(ntv2_xlxdma_fld_chn_run,							1,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_desc_stop,						1,	1);
NTV2_FLD(ntv2_xlxdma_fld_chn_desc_complete,					1,	2);
NTV2_FLD(ntv2_xlxdma_fld_chn_align_mismatch,				1,	3);
NTV2_FLD(ntv2_xlxdma_fld_chn_magic_stop,					1,	4);
NTV2_FLD(ntv2_xlxdma_fld_chn_invalid_length,				1,	5);
NTV2_FLD(ntv2_xlxdma_fld_chn_idle_stop,						1,	6);
NTV2_FLD(ntv2_xlxdma_fld_chn_read_error,					5,	9);
NTV2_FLD(ntv2_xlxdma_fld_chn_write_error,					5,	14);
NTV2_FLD(ntv2_xlxdma_fld_chn_desc_error,					5,	19);
NTV2_FLD(ntv2_xlxdma_fld_chn_non_inc_address_mode,			1,	25);
NTV2_FLD(ntv2_xlxdma_fld_chn_poll_mode_write_enable,		1,	26);
NTV2_FLD(ntv2_xlxdma_fld_chn_stream_write_disable,			1,	27);

/* xilinx s2c status registers */
NTV2_REG(ntv2_xlxdma_reg_chn_status,						0x0010, 0x0050, 0x0090, 0x00d0, 0x0410, 0x0450, 0x0490, 0x04d0);
NTV2_REG(ntv2_xlxdma_reg_chn_status_rc,						0x0011, 0x0051, 0x0091, 0x00d1, 0x0411, 0x0451, 0x0491, 0x04d1);

/* xilinx s2c descriptor complete count register */
NTV2_REG(ntv2_xlxdma_reg_chn_desc_complete_count,			0x0012, 0x0052, 0x0092, 0x00d2, 0x0412, 0x0452, 0x0492, 0x04d2);

/* xilinx s2c transfer alignment register */
NTV2_REG(ntv2_xlxdma_reg_chn_alignments,					0x0013, 0x0053, 0x0093, 0x00d3, 0x0413, 0x0453, 0x0493, 0x04d3);
NTV2_FLD(ntv2_xlxdma_fld_chn_address_bits,					8,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_transfer_alignment,			8,	8);
NTV2_FLD(ntv2_xlxdma_fld_chn_address_alignment,				8,	16);

/* xilinx s2c poll mode address registers */
NTV2_REG(ntv2_xlxdma_reg_chn_poll_mode_address_low,			0x0022, 0x0062, 0x00a2, 0x00e2, 0x0422, 0x0462, 0x04a2, 0x04e2);
NTV2_REG(ntv2_xlxdma_reg_chn_poll_mode_address_high,		0x0023, 0x0063, 0x00a3, 0x00e3, 0x0423, 0x0463, 0x04a3, 0x04e3);

/* xilinx s2c interrupt enable registers */
NTV2_REG(ntv2_xlxdma_reg_chn_interrupt_enable,				0x0024, 0x0064, 0x00a4, 0x00e4, 0x0424, 0x0464, 0x04a4, 0x04e4);
NTV2_REG(ntv2_xlxdma_reg_chn_interrupt_enable_w1s,			0x0025, 0x0065, 0x00a5, 0x00e5, 0x0425, 0x0465, 0x04a5, 0x04e5);
NTV2_REG(ntv2_xlxdma_reg_chn_interrupt_enable_w1c,			0x0026, 0x0066, 0x00a6, 0x00e6, 0x0426, 0x0466, 0x04a6, 0x04e6);

/* xilinx s2c performance control register */
NTV2_REG(ntv2_xlxdma_reg_chn_perf_control,					0x0030, 0x0070, 0x00b0, 0x00f0, 0x0430, 0x0470, 0x04b0, 0x04f0);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_auto,						1,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_clear,					1,	1);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_run,						1,	2);

/* xilinx s2c cycle count registers */
NTV2_REG(ntv2_xlxdma_reg_chn_perf_cycle_count_low,			0x0031, 0x0071, 0x00b1, 0x00f1, 0x0431, 0x0471, 0x04b1, 0x04f1);
NTV2_REG(ntv2_xlxdma_reg_chn_perf_cycle_count_high,			0x0032, 0x0072, 0x00b2, 0x00f2, 0x0432, 0x0472, 0x04b2, 0x04f2);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_cycle_count_bits,			10,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_cycle_count_maxed,		1,	16);

/* xilinx s2c data count registers */
NTV2_REG(ntv2_xlxdma_reg_chn_perf_data_count_low,			0x0033, 0x0073, 0x00b3, 0x00f3, 0x0433, 0x0473, 0x04b3, 0x04f3);
NTV2_REG(ntv2_xlxdma_reg_chn_perf_data_count_high,			0x0034, 0x0074, 0x00b4, 0x00f4, 0x0434, 0x0474, 0x04b4, 0x04f4);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_data_count_bits,			10,	0);
NTV2_FLD(ntv2_xlxdma_fld_chn_perf_data_count_maxed,			1,	16);

/* xilinx irq identifier register */
NTV2_REG(ntv2_xlxdma_reg_irq_identifier,					0x0800);

/* xilinx irq user interrupt enable registers */
NTV2_REG(ntv2_xlxdma_reg_irq_usr_interrupt_enable,			0x0801);
NTV2_REG(ntv2_xlxdma_reg_irq_usr_interrupt_enable_w1s,		0x0802);
NTV2_REG(ntv2_xlxdma_reg_irq_usr_interrupt_enable_w1c,		0x0803);

/* xilinx irq channel interrupt enable registers */
NTV2_REG(ntv2_xlxdma_reg_irq_chn_interrupt_enable,			0x0804);
NTV2_REG(ntv2_xlxdma_reg_irq_chn_interrupt_enable_w1s,		0x0805);
NTV2_REG(ntv2_xlxdma_reg_irq_chn_interrupt_enable_w1c,		0x0806);

/* xilinx irq interrupt request registers */
NTV2_REG(ntv2_xlxdma_reg_irq_usr_interrupt_request,			0x0810);
NTV2_REG(ntv2_xlxdma_reg_irq_chn_interrupt_request,			0x0811);

/* xilinx irq interrupt pending event registers */
NTV2_REG(ntv2_xlxdma_reg_irq_usr_interrupt_pending,			0x0812);
NTV2_REG(ntv2_xlxdma_reg_irq_chn_interrupt_pending,			0x0813);

/* xilinx segment identifier register */
NTV2_REG(ntv2_xlxdma_reg_seg_identifier,					0x1000, 0x1040, 0x1080, 0x10c0, 0x1400, 0x1440, 0x1480, 0x14c0);

/* xilinx segment descriptor address registers */
NTV2_REG(ntv2_xlxdma_reg_seg_desc_address_low,				0x1020, 0x1060, 0x10a0, 0x10e0, 0x1420, 0x1460, 0x14a0, 0x14e0);
NTV2_REG(ntv2_xlxdma_reg_seg_desc_address_high,				0x1021, 0x1061, 0x10a1, 0x10e1, 0x1421, 0x1461, 0x14a1, 0x14e1);

/* xilinx segment descriptor adjacent register */
NTV2_REG(ntv2_xlxdma_reg_seg_desc_adjacent,					0x1022, 0x1062, 0x10a2, 0x10e2, 0x1422, 0x1462, 0x14a2, 0x14e2);

/* xilinx segment descriptor credits register */
NTV2_REG(ntv2_xlxdma_reg_seg_desc_credits,					0x1023, 0x1063, 0x10a3, 0x10e3, 0x1423, 0x1463, 0x14a3, 0x14e3);

/* xilinx descriptor control parameter bit definitions */
NTV2_FLD(ntv2_xlxdma_fld_desc_control_stop,					1,	0);
NTV2_FLD(ntv2_xlxdma_fld_desc_control_completion,			1,	1);
NTV2_FLD(ntv2_xlxdma_fld_desc_control_eop,					1,	4);
NTV2_FLD(ntv2_xlxdma_fld_desc_control_count,				5,	8);
NTV2_FLD(ntv2_xlxdma_fld_desc_control_magic,				16,	16);

/* xilinx descriptor control magic constant */
NTV2_CON(ntv2_xlxdma_con_desc_control_magic,				0xad4b);

#endif
