/*
 * NTV2 enhanced csc structures
 *
 * Copyright 2021 AJA Video Systems Inc. All rights reserved.
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

#ifndef NTV2_ENHCSC_H
#define NTV2_ENHCSC_H

#include "ntv2_common.h"
#include "ntv2_fixedpoint.h"

struct ntv2_csc_matrix {
	ntv2_fp16 a0;
	ntv2_fp16 a1;
	ntv2_fp16 a2;

	ntv2_fp16 b0;
	ntv2_fp16 b1;
	ntv2_fp16 b2;

	ntv2_fp16 c0;
	ntv2_fp16 c1;
	ntv2_fp16 c2;

	s16 pre_offset0;
	s16 pre_offset1;
	s16 pre_offset2;

	s16 post_offsetA;
	s16 post_offsetB;
	s16 post_offsetC;

	int	preset;
};

struct ntv2_enhanced_csc {
	int	input_pixel_format;
	int output_pixel_format;
	int chroma_filter_select;
	int chroma_edge_control;
	int key_source;
	int key_output_range;
	s16 key_input_offset;
	u32 key_output_offset;
	ntv2_fp16 key_gain;
	struct ntv2_csc_matrix matrix;
};

NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_rec709,							0);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_rec601,							1);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_custom,							2);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_unity,							3);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_unity_smpte,					4);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec709,		5);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec601,		6);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec709,		7);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec601,		8);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec709,		9);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec601,		10);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec709,		11);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec601,		12);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_rec601_to_ycbcr_rec709,	13);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_rec709_to_ycbcr_rec601,	14);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_gbr_smpte,			15);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_gbr_full,			16);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec2020,		17);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec2020,		18);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec2020,		19);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec2020,		20);
NTV2_CON(ntv2_kona_enhanced_csc_matrix_type_invalid,						21);

void ntv2_csc_matrix_initialize(struct ntv2_csc_matrix *matrix, int matrix_type);
void ntv2_csc_matrix_set_hue_rotate(struct ntv2_csc_matrix *matrix, ntv2_fp16 degrees);
void ntv2_csc_matrix_set_gain(struct ntv2_csc_matrix *matrix,
							  ntv2_fp16 gain0, ntv2_fp16 gain1, ntv2_fp16 gain2);
void ntv2_csc_matrix_pre_multiply(struct ntv2_csc_matrix *matrix,
								  const struct ntv2_csc_matrix *pre);
void ntv2_csc_matrix_post_multiply(struct ntv2_csc_matrix *matrix,
								   const struct ntv2_csc_matrix *post);
#endif
