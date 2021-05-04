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

#include "ntv2_enhcsc.h"

void ntv2_csc_matrix_initialize(struct ntv2_csc_matrix *matrix, int matrix_type)
{
	if (matrix == NULL)
		return;

	switch(matrix_type) {
		default:
		case ntv2_kona_enhanced_csc_matrix_type_rec709:
		case ntv2_kona_enhanced_csc_matrix_type_rec601:
		case ntv2_kona_enhanced_csc_matrix_type_custom:
		case ntv2_kona_enhanced_csc_matrix_type_unity:
			matrix->a0 = ntv2_fp16_init(1, 0);
			matrix->a1 = ntv2_fp16_init(0, 0);
			matrix->a2 = ntv2_fp16_init(0, 0);
			matrix->b0 = ntv2_fp16_init(0, 0);
			matrix->b1 = ntv2_fp16_init(1, 0);
			matrix->b2 = ntv2_fp16_init(0, 0);
			matrix->c0 = ntv2_fp16_init(0, 0);
			matrix->c1 = ntv2_fp16_init(0, 0);
			matrix->c2 = ntv2_fp16_init(1, 0);

			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_unity_smpte:
			matrix->a0 = ntv2_fp16_init(1, 0);
			matrix->a1 = ntv2_fp16_init(0, 0);
			matrix->a2 = ntv2_fp16_init(0, 0);
			matrix->b0 = ntv2_fp16_init(0, 0);
			matrix->b1 = ntv2_fp16_init(1, 0);
			matrix->b2 = ntv2_fp16_init(0, 0);
			matrix->c0 = ntv2_fp16_init(0, 0);
			matrix->c1 = ntv2_fp16_init(0, 0);
			matrix->c2 = ntv2_fp16_init(1, 0);

			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec709:
			matrix->a0 = 40136;  //  0.612427	 G -> Y
			matrix->a1 = 4052;   //  0.061829	 B -> Y
			matrix->a2 = 11932;  //  0.182068	 R -> Y
			matrix->b0 = -22123; // -0.337585	 G -> Cb
			matrix->b1 = 28699;  //  0.437927	 B -> Cb
			matrix->b2 = -6576;  // -0.100342	 R -> Cb
			matrix->c0 = -26067; // -0.397766	 G -> Cr
			matrix->c1 = -2631;  // -0.040161	 B -> Cr
			matrix->c2 = 28699;  //  0.437927	 R -> Cr

			/* full-range RGB: no offset */
			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec601:
			matrix->a0 = 32941;		//  0.502655	 G -> Y
			matrix->a1 = 6398;		//  0.097626	 B -> Y
			matrix->a2 = 16779;		//  0.256042	 R -> Y
			matrix->b0 = -19014;	// -0.290131	 G -> Cb
			matrix->b1 = 28699;		//  0.437927	 B -> Cb
			matrix->b2 = -9686;		// -0.147797	 R -> Cb
			matrix->c0 = -24031;	// -0.366699	 G -> Cr
			matrix->c1 = -4667;		// -0.071228	 B -> Cr
			matrix->c2 = 28699;		//  0.437927	 R -> Cr

			/* full-range RGB: no offset */
			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec709:
			matrix->a0 = 46872;		//  0.715210	G -> Y
			matrix->a1 = 4732;		//  0.072205	B -> Y
			matrix->a2 = 13931;		//  0.212585	R -> Y
			matrix->b0 = -25835;	// -0.394226	G -> Cb
			matrix->b1 = 33516;		//  0.511414	B -> Cb
			matrix->b2 = -7680;		// -0.117188	R -> Cb
			matrix->c0 = -30441;	// -0.464508	G -> Cr
			matrix->c1 = -3074;		// -0.046906	B -> Cr
			matrix->c2 = 33516;		//  0.511414	R -> Cr

			/* RGB: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x0800;
			matrix->pre_offset2 = 0x0800;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec601:
			matrix->a0 = 38470;		//  0.587006	G -> Y
			matrix->a1 = 7469;		//  0.113983	B -> Y
			matrix->a2 = 19595;		//  0.299011	R -> Y
			matrix->b0 = -22206;	// -0.338837	G -> Cb
			matrix->b1 = 33516;		//  0.511414	B -> Cb
			matrix->b2 = -11310;	// -0.172577	R -> Cb
			matrix->c0 = -28065;	// -0.428253	G -> Cr
			matrix->c1 = -5449;		// -0.083160	B -> Cr
			matrix->c2 = 33516;		//  0.511414	R -> Cr

			/* RGB: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x0800;
			matrix->pre_offset2 = 0x0800;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec709:
			matrix->a0 = 76532;		//  1.167786	Y  -> G
			matrix->a1 = -14018;	// -0.213898	Cb -> G
			matrix->a2 = -35029;	// -0.534515	Cr -> G
			matrix->b0 = 76532;		//  1.167786	Y  -> B
			matrix->b1 = 138843;	//  2.118591	Cb -> B
			matrix->b2 = 0;			//  0.000000	Cr -> B
			matrix->c0 = 76532;		//  1.167786	Y  -> R
			matrix->c1 = 0;			//  0.000000	Cb -> R
			matrix->c2 = 117832;	//  1.797974	Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* full-range RGB: no offset */
			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec601:
			matrix->a0 = 76532;		//  1.167786	Y  -> G
			matrix->a1 = -25751;	// -0.392944	Cb -> G
			matrix->a2 = -53437;	// -0.815399	Cr -> G
			matrix->b0 = 76532;		//  1.167786	Y  -> B
			matrix->b1 = 132590;	//  2.023163	Cb -> B
			matrix->b2 = 0;			//  0.000000	Cr -> B
			matrix->c0 = 76532;		//  1.167786	Y  -> R
			matrix->c1 = 0;			//  0.000000	Cb -> R
			matrix->c2 = 104903;	//  1.600708	Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* full-range RGB: no offset */
			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec709:
			matrix->a0 = 65536;		//  1.000000	Y  -> G
			matrix->a1 = -12004;	// -0.183167	Cb -> G
			matrix->a2 = -29992;	// -0.457642	Cr -> G
			matrix->b0 = 65536;		//  1.000000	Y  -> B
			matrix->b1 = 118892;	//  1.814148	Cb -> B
			matrix->b2 = 0;			//  0.000000	Cr -> B
			matrix->c0 = 65536;		//  1.000000	Y  -> R
			matrix->c1 = 0;			//  0.000000	Cb -> R
			matrix->c2 = 100904;	//  1.539673	Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* RGB: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x0800;
			matrix->post_offsetC = 0x0800;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec601:
			matrix->a0 = 65536;		//  1.000000    Y  -> G
			matrix->a1 = -22048;	// -0.336426	Cb -> G
			matrix->a2 = -45755;	// -0.698181   	Cr -> G
			matrix->b0 = 65536;		//  1.000000    Y  -> B
			matrix->b1 = 113540;	//  1.732483	Cb -> B
			matrix->b2 = 0;			//  0.000000    Cr -> B
			matrix->c0 = 65536;		//  1.000000    Y  -> R
			matrix->c1 = 0;			//  0.000000    Cb -> R
			matrix->c2 = 89832;		//  1.370728	Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* RGB: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x0800;
			matrix->post_offsetC = 0x0800;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_rec601_to_ycbcr_rec709:
			matrix->a0 = 65536;		//  1.00000000		Y  -> Y
			matrix->a1 = -7572;		// -0.11554975		Cb -> Y
			matrix->a2 = -13627;	// -0.20793764		Cr -> Y
			matrix->b0 = 0;			//  0.00000000		Y  -> Cb
			matrix->b1 = 66757;		//  1.01863972		Cb -> Cb
			matrix->b2 = 7511;		//  0.11461795		Cr -> Cb
			matrix->c0 = 0;			//  0.00000000		Y  -> Cr
			matrix->c1 = 4918;		//  0.07504945		Cb -> Cr
			matrix->c2 = 67195;		//  1.02532707		Cr -> Cr

			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_rec709_to_ycbcr_rec601:
			matrix->a0 = 65536;		//  1.00000000		Y  -> Y
			matrix->a1 = 6508;		//  0.09931166		Cb -> Y
			matrix->a2 = 12563;		//  0.19169955		Cr -> Y
			matrix->b0 = 0;			//  0.00000000		Y  -> Cb
			matrix->b1 = 64871;		//  0.98985381		Cb -> Cb
			matrix->b2 = -7251;		// -0.11065251		Cr -> Cb
			matrix->c0 = 0;			//  0.00000000		Y  -> Cr
			matrix->c1 = -4748;		// -0.07245296		Cb -> Cr
			matrix->c2 = 64447;		//  0.98339782		Cr -> Cr

			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_gbr_smpte:
			matrix->a0 = 56064;		// 0.855469		 Gf -> Gs
			matrix->a1 = 0;			// 0.000000		 Bf -> Gs
			matrix->a2 = 0;			// 0.000000		 Rf -> Gs
			matrix->b0 = 0;			// 0.000000		 Gf -> Bs
			matrix->b1 = 56064;		// 0.855469		 Bf -> Bs
			matrix->b2 = 0;			// 0.000000		 Rf -> Bs
			matrix->c0 = 0;			// 0.000000		 Gf -> Rs
			matrix->c1 = 0;			// 0.000000		 Bf -> Rs
			matrix->c2 = 56064;		// 0.855469		 Rf -> Rs

			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x0800;
			matrix->post_offsetC = 0x0800;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_gbr_full:
			matrix->a0 = 76608;		// 1.168950		 Gs -> Gf
			matrix->a1 = 0;			// 0.000000		 Bs -> Gf
			matrix->a2 = 0;			// 0.000000		 Rs -> Gf
			matrix->b0 = 0;			// 0.000000		 Gs -> Bf
			matrix->b1 = 76608;		// 1.168950		 Bs -> Bf
			matrix->b2 = 0;			// 0.000000		 Rs -> Bf
			matrix->c0 = 0;			// 0.000000		 Gs -> Rf
			matrix->c1 = 0;			// 0.000000		 Bs -> Rf
			matrix->c2 = 76608;		// 1.168950		 Rs -> Rf

			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x0800;
			matrix->pre_offset2 = 0x0800;

			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec2020:
			matrix->a0 = 38048;		//  0.58057 G -> Y
			matrix->a1 = 3327;		//  0.05078 B -> Y
			matrix->a2 = 14742;		//  0.22495 R -> Y
			matrix->b0 = -20685;	// -0.31563 G -> Cb
			matrix->b1 = 28700;		//  0.43793 B -> Cb
			matrix->b2 = -8015;		// -0.12230 R -> Cb
			matrix->c0 = -26392;	// -0.40271 G -> Cr
			matrix->c1 = -2308;		// -0.03522 B -> Cr
			matrix->c2 = 28700;		//  0.43793 R -> Cr

			/* full-range RGB: no offset */
			matrix->pre_offset0 = 0x0000;
			matrix->pre_offset1 = 0x0000;
			matrix->pre_offset2 = 0x0000;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_gbr_smpte_to_ycbcr_rec2020:
			matrix->a0 = 44433;		//  0.67800		G -> Y
			matrix->a1 = 3886;		//  0.05930		B -> Y
			matrix->a2 = 17216;		//  0.26270		R -> Y
			matrix->b0 = -24156;	// -0.368594   	G -> Cb
			matrix->b1 = 33516;		//  0.511414   	B -> Cb
			matrix->b2 = -9359;		// -0.14282		R -> Cb
			matrix->c0 = -30820;	// -0.470284   	G -> Cr
			matrix->c1 = -2695;		// -0.04113		B -> Cr
			matrix->c2 = 33516;		//  0.511414   	R -> Cr

			/* RGB: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x0800;
			matrix->pre_offset2 = 0x0800;

			/* YCbCr: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x4000;
			matrix->post_offsetC = 0x4000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_full_rec2020:
			matrix->a0 = 76532;		//  1.167786	Y  -> G
			matrix->a1 = -12312;	// -0.187877	Cb -> G
			matrix->a2 = -42751;	// -0.652337	Cr -> G
			matrix->b0 = 76532;		//  1.167786	Y  -> B
			matrix->b1 = 140775;	//  2.148061	Cb -> B
			matrix->b2 = 0;			//  0.000000	Cr -> B
			matrix->c0 = 76532;		//  1.167786	Y  -> R
			matrix->c1 = 0;			//  0.000000	Cb -> R
			matrix->c2 = 110337;	//  1.683611	Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* full-range RGB: no offset */
			matrix->post_offsetA = 0x0000;
			matrix->post_offsetB = 0x0000;
			matrix->post_offsetC = 0x0000;
			break;
		case ntv2_kona_enhanced_csc_matrix_type_ycbcr_to_gbr_smpte_rec2020:
			matrix->a0 = 65536; 	//  1.000000	    Y  -> G
			matrix->a1 = -10543;	// -0.160880066		Cb -> G
			matrix->a2 = -36608;	// -0.5585997088   	Cr -> G
			matrix->b0 = 65536;		//  1.000000	    Y  -> B
			matrix->b1 = 120547;	//  1.839404464		Cb -> B
			matrix->b2 = 0;			//  0.000000	    Cr -> B
			matrix->c0 = 65536;		//  1.000000	    Y  -> R
			matrix->c1 = 0;			//  0.000000	    Cb -> R
			matrix->c2 = 94482;		//  1.441684821		Cr -> R

			/* YCbCr: "SMPTE" offsets */
			matrix->pre_offset0 = 0x0800;
			matrix->pre_offset1 = 0x4000;
			matrix->pre_offset2 = 0x4000;

			/* RGB: "SMPTE" offsets */
			matrix->post_offsetA = 0x0800;
			matrix->post_offsetB = 0x0800;
			matrix->post_offsetC = 0x0800;
			break;
	}

	matrix->preset = matrix_type;
}

void ntv2_csc_matrix_set_hue_rotate(struct ntv2_csc_matrix *matrix, ntv2_fp16 degrees)
{
	ntv2_fp16 rad;
	ntv2_fp16 one_eighty = ntv2_fp16_init(180, 0);

	if (matrix == NULL)
		return;

	rad = ntv2_fp16_div(ntv2_fp16_mul(ntv2_fp16_pi, degrees), one_eighty);

	matrix->b1 = ntv2_fp16_cos(rad);
	matrix->b2 = ntv2_fp16_sin(rad);
	matrix->c1 = -matrix->b2;	// -1 * ntv2_fp16_sin(rad);
	matrix->c2 = matrix->b1;	// ntv2_fp16_cos(rad);
	matrix->preset = ntv2_kona_enhanced_csc_matrix_type_custom;
}

void ntv2_csc_matrix_set_gain(struct ntv2_csc_matrix *matrix,
							  ntv2_fp16 gain0, ntv2_fp16 gain1, ntv2_fp16 gain2)
{
	if (matrix == NULL)
		return;

	matrix->a0 = gain0;
	matrix->b1 = gain1;
	matrix->c2 = gain2;
	matrix->preset = ntv2_kona_enhanced_csc_matrix_type_custom;
}

void ntv2_csc_matrix_pre_multiply(struct ntv2_csc_matrix *matrix,
								   const struct ntv2_csc_matrix *pre)
{
	struct ntv2_csc_matrix tmp;

	if (matrix == NULL || pre == NULL)
		return;

	tmp = *matrix;

	matrix->a0 = ntv2_fp16_mul(pre->a0, tmp.a0) + ntv2_fp16_mul(pre->b0, tmp.a1) + ntv2_fp16_mul(pre->c0, tmp.a2);
	matrix->a1 = ntv2_fp16_mul(pre->a1, tmp.a0) + ntv2_fp16_mul(pre->b1, tmp.a1) + ntv2_fp16_mul(pre->c1, tmp.a2);
	matrix->a2 = ntv2_fp16_mul(pre->a2, tmp.a0) + ntv2_fp16_mul(pre->b2, tmp.a1) + ntv2_fp16_mul(pre->c2, tmp.a2);

	matrix->b0 = ntv2_fp16_mul(pre->a0, tmp.b0) + ntv2_fp16_mul(pre->b0, tmp.b1) + ntv2_fp16_mul(pre->c0, tmp.b2);
	matrix->b1 = ntv2_fp16_mul(pre->a1, tmp.b0) + ntv2_fp16_mul(pre->b1, tmp.b1) + ntv2_fp16_mul(pre->c1, tmp.b2);
	matrix->b2 = ntv2_fp16_mul(pre->a2, tmp.b0) + ntv2_fp16_mul(pre->b2, tmp.b1) + ntv2_fp16_mul(pre->c2, tmp.b2);

	matrix->c0 = ntv2_fp16_mul(pre->a0, tmp.c0) + ntv2_fp16_mul(pre->b0, tmp.c1) + ntv2_fp16_mul(pre->c0, tmp.c2);
	matrix->c1 = ntv2_fp16_mul(pre->a1, tmp.c0) + ntv2_fp16_mul(pre->b1, tmp.c1) + ntv2_fp16_mul(pre->c1, tmp.c2);
	matrix->c2 = ntv2_fp16_mul(pre->a2, tmp.c0) + ntv2_fp16_mul(pre->b2, tmp.c1) + ntv2_fp16_mul(pre->c2, tmp.c2);

	matrix->preset = ntv2_kona_enhanced_csc_matrix_type_custom;
}

void ntv2_csc_matrix_post_multiply(struct ntv2_csc_matrix *matrix,
								   const struct ntv2_csc_matrix *post)
{
	struct ntv2_csc_matrix tmp;

	if (matrix == NULL || post == NULL)
		return;

	tmp = *matrix;

	matrix->a0 = ntv2_fp16_mul(tmp.a0, post->a0) + ntv2_fp16_mul(tmp.b0, post->a1) + ntv2_fp16_mul(tmp.c0, post->a2);
	matrix->a1 = ntv2_fp16_mul(tmp.a1, post->a0) + ntv2_fp16_mul(tmp.b1, post->a1) + ntv2_fp16_mul(tmp.c1, post->a2);
	matrix->a2 = ntv2_fp16_mul(tmp.a2, post->a0) + ntv2_fp16_mul(tmp.b2, post->a1) + ntv2_fp16_mul(tmp.c2, post->a2);

	matrix->b0 = ntv2_fp16_mul(tmp.a0, post->b0) + ntv2_fp16_mul(tmp.b0, post->b1) + ntv2_fp16_mul(tmp.c0, post->b2);
	matrix->b1 = ntv2_fp16_mul(tmp.a1, post->b0) + ntv2_fp16_mul(tmp.b1, post->b1) + ntv2_fp16_mul(tmp.c1, post->b2);
	matrix->b2 = ntv2_fp16_mul(tmp.a2, post->b0) + ntv2_fp16_mul(tmp.b2, post->b1) + ntv2_fp16_mul(tmp.c2, post->b2);

	matrix->c0 = ntv2_fp16_mul(tmp.a0, post->c0) + ntv2_fp16_mul(tmp.b0, post->c1) + ntv2_fp16_mul(tmp.c0, post->c2);
	matrix->c1 = ntv2_fp16_mul(tmp.a1, post->c0) + ntv2_fp16_mul(tmp.b1, post->c1) + ntv2_fp16_mul(tmp.c1, post->c2);
	matrix->c2 = ntv2_fp16_mul(tmp.a2, post->c0) + ntv2_fp16_mul(tmp.b2, post->c1) + ntv2_fp16_mul(tmp.c2, post->c2);

	matrix->preset = ntv2_kona_enhanced_csc_matrix_type_custom;
}