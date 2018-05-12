/*
 * NTV2 HDMI input hardware register constants
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

#ifndef NTV2_HINREG_H
#define NTV2_HINREG_H

/* i2c register / value data */
struct ntv2_reg_value {
	u8 address;
	u8 value;
};

/* hdmi i2c device addresses */
static const u8 device_io_bank					= 0x4c;		/* hdmi chip io register bank */
static const u8 device_hdmi_bank				= 0x34;		/* hdmi chip hdmi register bank */
static const u8 device_cec_bank					= 0x40;		/* hdmi chip cec register bank */
static const u8 device_cp_bank					= 0x22;		/* hdmi chip cp register bank */
static const u8 device_repeater_bank			= 0x32;		/* hdmi chip repeater register bank */
static const u8 device_edid_bank				= 0x36;		/* hdmi chip edid register bank */
static const u8 device_dpll_bank				= 0x26;		/* hdmi chip dpll register bank */
static const u8 device_info_bank				= 0x3e;		/* hdmi chip info frame register bank */

/* hdmi i2c data registers and bit masks */
static const u8 packet_detect_reg				= 0x60;
static const u8 packet_detect_avi_mask			= 0x01;
static const u8 packet_detect_vsi_mask			= 0x10;

static const u8 clock_detect_reg				= 0x6a;
static const u8 clock_tmdsa_present_mask		= 0x10;
static const u8 clock_tmdsa_lock_mask			= 0x40;
static const u8 clock_sync_lock_mask			= 0x02;
static const u8 clock_regen_lock_mask			= 0x01;

static const u8 tmds_lock_detect_reg			= 0x6b;
static const u8 tmds_lock_detect_mask			= 0x40;

static const u8 tmds_lock_clear_reg				= 0x6c;
static const u8 tmds_lock_clear_mask			= 0x40;

static const u8 cable_detect_reg				= 0x6f;
static const u8 cable_detect_mask				= 0x01;

static const u8 tmds_frequency_detect_reg		= 0x83;
static const u8 tmds_frequency_detect_mask		= 0x02;

static const u8 tmds_frequency_clear_reg		= 0x85;
static const u8 tmds_frequency_clear_mask		= 0x02;

static const u8 audio_lock_reg					= 0x04;
static const u8 audio_lock_mask					= 0x01;

static const u8 io_color_reg					= 0x02;
static const u8 io_color_space_mask				= 0x06;

static const u8 hdmi_hpa_reg					= 0x6c;
static const u8 hdmi_hpa_manual_mask			= 0x01;

static const u8 hdmi_mode_reg					= 0x05;
static const u8 hdmi_mode_mask					= 0x80;
static const u8 hdmi_encrypted_mask				= 0x40;

static const u8 deep_color_mode_reg				= 0x0b;
static const u8 deep_color_10bit_mask			= 0x40;
static const u8 deep_color_12bit_mask			= 0x80;

static const u8 derep_mode_reg					= 0x41;
static const u8 derep_mode_mask					= 0x1f;

static const u8 defilter_lock_detect_reg		= 0x07;
static const u8 defilter_locked_mask			= 0x20;         
static const u8 audio_multichannel_mask			= 0x40;
static const u8 vfilter_locked_mask				= 0x80;         

static const u8 audio_detect_reg				= 0x18;
static const u8 audio_detect_mask				= 0x01;

static const u8 interlaced_detect_reg			= 0x0b;
static const u8 interlaced_mask					= 0x20;

static const u8 tristate_reg					= 0x15;
static const u8 tristate_disable_outputs		= 0x9e;
static const u8 tristate_enable_outputs			= 0x80;         

static const u8 vsi_infoframe_packet_id			= 0xec;
static const u8 vsi_infoframe_version			= 0xed;
static const u8 vsi_infoframe_length			= 0xee;
static const u8 vsi_infoframe_checksum			= 0x54;
static const u8 vsi_infoframe_byte1				= 0x55;

static const u8 vsi_video_format_mask4			= 0xe0;
static const u8 vsi_video_format_shift4			= 0x05;
static const u8 vsi_3d_structure_mask5			= 0xf0;
static const u8 vsi_3d_structure_shift5			= 0x04;

static const u8 avi_infoframe_packet_id			= 0xe0;
static const u8 avi_infoframe_version			= 0xe1;
static const u8 avi_infoframe_length			= 0xe2;
static const u8 avi_infoframe_checksum			= 0x00;
static const u8 avi_infoframe_byte1				= 0x01;

static const u8 avi_scan_data_mask1				= 0x03;
static const u8 avi_scan_data_shift1			= 0x00;
static const u8 avi_bar_data_mask1				= 0x0c;
static const u8 avi_bar_data_shift1				= 0x02;
static const u8 avi_active_format_mask1			= 0x10;
static const u8 avi_active_format_shift1		= 0x04;
static const u8 avi_color_component_mask1		= 0x60;
static const u8 avi_color_component_shift1		= 0x05;
static const u8 avi_active_aspect_mask2			= 0x0f;
static const u8 avi_active_aspect_shift2		= 0x00;
static const u8 avi_frame_aspect_ratio_mask2	= 0x30;
static const u8 avi_frame_aspect_ratio_shift2	= 0x04;
static const u8 avi_colorimetry_mask2			= 0xc0;
static const u8 avi_colorimetry_shift2			= 0x06;
static const u8 avi_nonuniform_scaling_mask3	= 0x03;
static const u8 avi_nonuniform_scaling_shift3	= 0x00;
static const u8 avi_quantization_range_mask3	= 0x0c;
static const u8 avi_quantization_range_shift3	= 0x02;
static const u8 avi_extended_colorimetry_mask3	= 0x70;
static const u8 avi_extended_colorimetry_shift3	= 0x04;
static const u8 avi_it_content_mask3			= 0x80;
static const u8 avi_it_content_shift3			= 0x07;
static const u8 avi_vic_mask4					= 0x7f;
static const u8 avi_vic_shift4					= 0x00;
static const u8 avi_pixel_repetition_mask5		= 0x0f;
static const u8 avi_pixel_repetition_shift5		= 0x00;
static const u8 avi_it_content_type_mask5		= 0x30;
static const u8 avi_it_content_type_shift5		= 0x04;
static const u8 avi_ycc_quant_range_mask5		= 0xc0;
static const u8 avi_ycc_quant_range_shift5		= 0x06;

/* info frame data values */
static const u8 vsi_packet_id					= 0x81;
static const u8 vsi_version						= 0x01;

static const u8 vsi_format_none					= 0x00;
static const u8 vsi_format_extended				= 0x01;
static const u8 vsi_format_3d					= 0x02;

static const u8 vsi_vic_reserved				= 0x00;
static const u8 vsi_vic_3840x2160_30			= 0x01;
static const u8 vsi_vic_3840x2160_25			= 0x02;
static const u8 vsi_vic_3840x2160_24			= 0x03;
static const u8 vsi_vic_4096x2160_24			= 0x04;

static const u8 avi_packet_id					= 0x82;
static const u8 avi_version						= 0x02;

static const u8 avi_scan_nodata					= 0x00;
static const u8 avi_scan_ovderscanned			= 0x01;
static const u8 avi_scan_underscanned			= 0x02;
static const u8 avi_scan_future					= 0x03;

static const u8 avi_bar_nodata					= 0x00;
static const u8 avi_bar_vertical				= 0x01;
static const u8 avi_bar_horizontal				= 0x02;
static const u8 avi_bar_both					= 0x03;

static const u8 avi_color_comp_rgb				= 0x00;
static const u8 avi_color_comp_422				= 0x01;
static const u8 avi_color_comp_444				= 0x02;
static const u8 avi_color_comp_420				= 0x03;

static const u8 avi_frame_aspect_nodata			= 0x00;
static const u8 avi_frame_aspect_4x3			= 0x01;
static const u8 avi_frame_aspect_16x9			= 0x02;
static const u8 avi_frame_aspect_future			= 0x03;

static const u8 avi_colorimetry_nodata			= 0x00;
static const u8 avi_colorimetry_smpte170m		= 0x01;
static const u8 avi_colorimetry_itu_r709		= 0x02;
static const u8 avi_colorimetry_extended		= 0x03;

static const u8 avi_active_aspect_nodata		= 0x00;
static const u8 avi_active_aspect_reserved		= 0x01;
static const u8 avi_active_aspect_box_16x9_top	= 0x02;
static const u8 avi_active_aspect_box_14x9_top	= 0x03;
static const u8 avi_active_aspect_box_16x9_cen	= 0x04;
static const u8 avi_active_aspect_coded_frame	= 0x08;
static const u8 avi_active_aspect_4x3_cen		= 0x09;
static const u8 avi_active_aspect_16x9_cen		= 0x0a;
static const u8 avi_active_aspect_14x9_cen		= 0x0b;
static const u8 avi_active_aspect_4x3_cen_14x9	= 0x0d;
static const u8 avi_active_aspect_16x9_cen_14x9	= 0x0e;
static const u8 avi_active_aspect_16x9_cen_4x3	= 0x0f;

static const u8 avi_nonuniform_scaling_nodata	= 0x00;
static const u8 avi_nonuniform_scaling_horiz	= 0x01;
static const u8 avi_nonuniform_scaling_vert		= 0x02;
static const u8 avi_nonuniform_scaling_both		= 0x03;

static const u8 avi_rgb_quant_range_default		= 0x00;
static const u8 avi_rgb_quant_range_limited		= 0x01;
static const u8 avi_rgb_quant_range_full		= 0x02;
static const u8 avi_rgb_quant_range_reserved	= 0x03;

static const u8 avi_ext_colorimetry_xv_ycc601	= 0x00;
static const u8 avi_ext_colorimetry_xv_ycc709	= 0x01;
static const u8 avi_ext_colorimetry_s_ycc601	= 0x02;
static const u8 avi_ext_colorimetry_adobe_601	= 0x03;
static const u8 avi_ext_colorimetry_adobe_rgb	= 0x04;
static const u8 avi_ext_colorimetry_ycc2020		= 0x05;
static const u8 avi_ext_colorimetry_rgb2020		= 0x06;
static const u8 avi_ext_colorimetry_reserved	= 0x07;

static const u8 avi_it_type_graphics			= 0x00;
static const u8 avi_it_type_photo				= 0x01;
static const u8 avi_it_type_cinema				= 0x02;
static const u8 avi_it_type_game				= 0x03;

static const u8 avi_ycc_quant_range_limited		= 0x00;
static const u8 avi_ycc_quant_range_full		= 0x01;
static const u8 avi_ycc_quant_range_reserved	= 0x02;
static const u8 avi_ycc_quant_range_reserved1	= 0x03;

# if 0
/* strings for infoframe data */
static const char *threeDFormatName[] = {
	"Not present", "Frame-packed", "Side-by-side", "Top-bottom", "Undefined"
};
static const char *const scanStr[] = { "nodata", "overscan", "underscan", "future" };
static const char *const barStr[] = { "nodata", "vertical", "horizontal", "both" };
static const char *const colorCompStr[] = { "RGB", "422", "444", "future" };
static const char *const aspectStr[] = { "nodata", "reserved", "box_16x9_top", "box_14x9_top",
										 "box_16x9_center", "invalid", "invalid",  "invalid", 
										 "codedframe", "4x3_center", "16x9_center", "14x9_center", 
										 "invalid", "4x3center14x9", "16x9center14x9", "16x9center4x3" };
static const char *const frameAspectStr[] = { "nodata", "4x3", "16x9", "future" };
static const char *const colorimetryStr[] = { "nodata", "SMPTE170M", "ITU_R709", "Extended" };
static const char *const scaleStr[] = { "nodata", "horizontal", "vertical", "both" };
static const char *const quantStr[] = { "limited", "full", "reserved", "reserved1" };
static const char *const extColorimetryStr[] = { "YCC601", "YCC709", "sYCC701", "AdobeYCC601", 
												 "AdobeRGB", "reserved1", "reserved1", "reserved3" };
static const char *const cTypeStr[] = { "graphics", "photo", "cinema", "game" };
static const char *const yccQuantStr[] = { "limited", "full", "reserved", "reserved1" };
#endif

/* Establish register bank mappings. Note that the actual I2C bus addresses end up */
/* being right shifted by 1 from the addresses used here and in the chip docs. */
static struct ntv2_reg_value init_io0[] = 
{
	{ 0xf4, 0x80 },		/* CEC Map Registers, I2C Address = 80 */
	{ 0xf5, 0x7C },		/* Info Frame Map Registers, I2C Address = 7C */
	{ 0xf8, 0x4c },		/* DPLL Map Registers, I2C Address = 4C */
	{ 0xf9, 0x64 },		/* Repeater Map Registers, I2C Address = 64 */
	{ 0xfa, 0x6c },		/* EDID Map Registers, I2C Address = 6C */
	{ 0xfb, 0x68 },		/* HDMI Map Registers, I2C Address = 68 */
	{ 0xfd, 0x44 }		/* CP Map Registers, I2C Address = 44 */
};
static int init_io0_size = sizeof(init_io0) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_hdmi1[] = 
/* HDMI Register - I2C address = 0x68 */
/* ADI Recommended write */
{
	{ 0xC0, 0x03 },		/* Recommended ADI write, documentation from script */
	{ 0x4C, 0x44 },		/* %%%%% Set NEW_VS_PARAM (improves vertical filter locking) */

	/* %%%%% "Recommended writes" added 7/14/14 */
	{ 0x03, 0x98 },
	{ 0x10, 0xA5 },
	{ 0x45, 0x04 },
	{ 0x3D, 0x10 },
	{ 0x3e, 0x69 },
	{ 0x3F, 0x46 },
	{ 0x4E, 0xFE },
	{ 0x4f, 0x08 },
	{ 0x50, 0x00 },
	{ 0x57, 0xa3 },
	{ 0x58, 0x07 },
	{ 0x93, 0x03 },
	{ 0x5A, 0x80 },

//	{ 0x6C, 0x14 },		/* Auto-assert HPD 100ms after (EDID active & cable detect) */
	{ 0x6C, 0x54 },		/* Auto-assert HPD 100ms after (EDID active & cable detect) */
	{ 0x0d, 0x02 }		/* Set TMDS frequency change tolerance to 2MHz */
};
static int init_hdmi1_size = sizeof(init_hdmi1) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_io2_non4k[] =
/* IO registers - I2C address = 0x98 */
{
	{ 0x00, 0x02 },		/* ADI Recommended Write */
	{ 0x01, 0x06 },		/* ADI Recommended Write  */
	{ 0x02, 0xf2 },		/* %%%%% INP_COLOR_SPACE[3:0], Address 0x02[7:4] = 1111 */
						/* 1111: Input color space depends on color space reported by HDMI block */
						/* ALT_GAMMA, Address 0x02[3] */
						/* 0 (default) No conversion */
						/* 1 YUV601 to YUV709 conversion if input is YUV601, YUV709 to YUV601 conversion if input is YUV709 */
						/* OP_656_RANGE, IO, Address 0x02[2] */
						/* 0 (default) Enables full output range (0 to 255) */
						/* 1 Enables limited output range (16 to 235)    */
						/* RGB_OUT, IO, Address 0x02[1]  */
						/* 0 (default) YPbPr color space output */
						/* 1 RGB color space output */
						/* ALT_DATA_SAT, IO, Address 0x02[0] */
						/* 0 (default) Data saturator enabled or disabled according to OP_656_RANGE setting */
						/* 1 Reverses OP_656_RANGE decision to enable or disable the data saturator */
	{ 0x03, 0x42 },		/* 36 Bit SDR Mode, RGB, Non-4K mode */
						/* Register changes to 0x54 for 4K mode */
	{ 0x04, 0x00 },		/* OP_CH_SEL[2:0], Address 0x04[7:5] = 000 P[35:24] Y/G, P[23:12] U/CrCb/B, P[11:0] V/R */
						/* XTAL_FREQ_SEL[1:0], Address 0x04[2:1] = 00, 27 Mhz */
						/* 4K mode requires 0x62 */
	{ 0x05, 0x38 },		/* F_OUT_SEL, IO, Address 0x05[4], Select DE or FIELD signal to be output on the DE pin */
						/* 0 (default) Selects DE output on DE pin */
						/* 1 Selects FIELD output on DE pin */
						/* DATA_BLANK_EN, IO, Address 0x05[3], A control to blank data during video blanking sections */
						/* 0 Do not blank data during horizontal and vertical blanking periods */
						/* 1 (default) Blank data during horizontal and vertical blanking periods */
						/* AVCODE_INSERT_EN, IO, Address 0x05[2], Select AV code insertion into the data stream */
						/* 0 Does not insert AV codes into data stream */
						/* 1 (default) Inserts AV codes into data stream */
						/* REPL_AV_CODE, IO, Address 0x05[1], duplicate AV codes and insertion on all output stream data channels */
						/* 0 (default) Outputs complete SAV/EAV codes on all channels, Channel A, Channel B, and Channel C */
						/* 1 Spreads AV code across three channels, Channel B and C contain the first two ten bit words, 0x3FF and 0x000 */
						/* Channel A contains the final two 10-bit words 0x00 and 0xXYZ */
						/* OP_SWAP_CB_CR, IO, Address 0x05[0], Controls the swapping of Cr and Cb data on the pixel buses */
						/* 0 (default) Outputs Cr and Cb as per OP_FORMAT_SEL */
						/* 1 Inverts the order of Cb and Cr in the interleaved data stream */
	{ 0x06, 0xa6 },		/* VS_OUT_SEL, Address 0x06[7], Select the VSync or FIELD signal to be output on the VS/FIELD/ALSB pin */
						/* 0 Selects FIELD output on VS/FIELD/ALSB pin */
						/* 1 (default) Selects VSync output on VS/FIELD/ALSB pin */
						/* INV_F_POL, Address 0x06[3], controls polarity of the DE signal */
						/* 0 (default) Negative FIELD/DE polarity */
						/* 1 Positive FIELD/DE polarity */
						/* INV_VS_POL, IO, Address 0x06[2] Controls polarity of the VS/FIELD/ALSB signal */
						/* 0 (default) Negative polarity VS/FIELD/ALSB */
						/* 1 Positive polarity VS/FIELD/ALSB */
						/* INV_HS_POL, Address 0x06[1], Controls polarity of the HS signal */
						/* 0 (default) Negative polarity HS */
						/* 1 Positive polarity HS */
						/* INV_LLC_POL, Address 0x06[0], Controls the polarity of the LLC */
						/* 0 (default) Does not invert LLC */
						/* 1 Inverts LLC */
	{ 0x0c, 0x42 },		/* Power up part */
	{ 0x14, 0x3F },		/* DR_STR[1:0], IO, Address 0x14[5:4] */
						/* 00 Reserved */
						/* 01 Medium low (2× */
						/* 10 (default) Medium high (3× */
						/* 11 High (4× */
						/* DR_STR_CLK[1:0], IO, Address 0x14[3:2] */
						/* 00 Reserved */
						/* 01 Medium low (2× for LLC up to 60 MHz */
						/* 10 (default) Medium high (3× for LLC from 44 MHz to 105 MHz */
						/* 11 High (4× for LLC greater than 100 MHz */
						/* DR_STR_SYNC[1:0], IO, Address 0x14[1:0] */
						/* 00 Reserved */
						/* 01 Medium low (2× */
						/* 10 (default) Medium high (3× */
						/* 11 High (4× */
	{ 0x15, 0x80 },		/* Disable Tristate of Pins */
/*!!        { 0x19, 0xC0 },	%%%%%	LLC DLL phase */
	{ 0x20, 0x04 },		/* HPA_MAN_VALUE_A, IO, Address 0x20[7] */
						/* A manual control for the value of HPA on Port A, Valid only if HPA_MANUAL is set to 1 */
						/* 0 - 0 V applied to HPA_A pin */
						/* 1 (default) High level applied to HPA_A pin */
						/* HPA_MAN_VALUE_B, IO, Address 0x20[6] */
						/* A manual control for the value of HPB on Port A, Valid only if HPA_MANUAL is set to 1 */
						/* 0 - 0 V applied to HPA_B pin */
						/* 1 (default) High level applied to HPA_B pin */
						/* HPA_TRISTATE_A, IO, Address 0x20[3] Tristates HPA output pin for Port A */
						/* 0 (default) HPA_A pin active */
						/* 1 Tristates HPA_A pin */
						/* HPA_TRISTATE_B, IO, Address 0x20[2] Tristates HPA output pin for Port B */
						/* 0 (default) HPA_B pin active */
						/* 1 Tristates HPA_B pin */
	{ 0x33, 0x40 },		/* LLC DLL MUX enable */
	{ 0xdd, 0x00 },		/* Normal LLC frequency = 0x00 for non-4K modes */
						/* LLC Half frequence = 0xA0 for 4K modes */
	{ 0x6e, 0x40 },		/* %%%%% TMDSPLL_LCK_A_MB1 enable to catch PLL loss of lock (enables INT1) */
	{ 0x86, 0x02 } 		/* %%%%% NEW_TMDS_FREQ_MB1 enable to catch frequency changes */
};
static int init_io2_non4k_size = sizeof(init_io2_non4k) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_io2_4k[] = 
/* IO registers - I2C address = 0x98 */
{
//	{ 0x00, 0x02 },		/* ADI Recommended Write */
//	{ 0x01, 0x06 },		/* ADI Recommended Write  */

	{ 0x00, 0x19 },		/* ADI Recommended Write per PCN 15_0178 */
	{ 0x01, 0x05 },		/* ADI Recommended Write per PCN 15_0178 */

	{ 0x02, 0xf2 },		/* INP_COLOR_SPACE[3:0], Address 0x02[7:4] = 1111 */
						/* 1111: Input color space depends on color space reported by HDMI block */
						/* ALT_GAMMA, Address 0x02[3] */
						/* 0 (default) No conversion */
						/* 1 YUV601 to YUV709 conversion if input is YUV601, YUV709 to YUV601 conversion if input is YUV709 */
						/* OP_656_RANGE, IO, Address 0x02[2] */
						/* 0 (default) Enables full output range (0 to 255) */
						/* 1 Enables limited output range (16 to 235)    */
						/* RGB_OUT, IO, Address 0x02[1]  */
						/* 0 (default) YPbPr color space output */
						/* 1 RGB color space output */
						/* ALT_DATA_SAT, IO, Address 0x02[0] */
						/* 0 (default) Data saturator enabled or disabled according to OP_656_RANGE setting */
						/* 1 Reverses OP_656_RANGE decision to enable or disable the data saturator */
	{ 0x03, 0x54 },		/* 36 Bit SDR Mode, RGB, Non-4K mode */
						/* Register changes to 0x54 for 4K mode */
	{ 0x04, 0x62 },		/* OP_CH_SEL[2:0], Address 0x04[7:5] = 000 P[35:24] Y/G, P[23:12] U/CrCb/B, P[11:0] V/R */
						/* XTAL_FREQ_SEL[1:0], Address 0x04[2:1] = 00, 27 Mhz */
						/* 4K mode requires 0x62 */
	{ 0x05, 0x38 },		/* F_OUT_SEL, IO, Address 0x05[4], Select DE or FIELD signal to be output on the DE pin */
						/* 0 (default) Selects DE output on DE pin */
						/* 1 Selects FIELD output on DE pin */
						/* DATA_BLANK_EN, IO, Address 0x05[3], A control to blank data during video blanking sections */
						/* 0 Do not blank data during horizontal and vertical blanking periods */
						/* 1 (default) Blank data during horizontal and vertical blanking periods */
						/* AVCODE_INSERT_EN, IO, Address 0x05[2], Select AV code insertion into the data stream */
						/* 0 Does not insert AV codes into data stream */
						/* 1 (default) Inserts AV codes into data stream */
						/* REPL_AV_CODE, IO, Address 0x05[1], duplicate AV codes and insertion on all output stream data channels */
						/* 0 (default) Outputs complete SAV/EAV codes on all channels, Channel A, Channel B, and Channel C */
						/* 1 Spreads AV code across three channels, Channel B and C contain the first two ten bit words, 0x3FF and 0x000 */
						/* Channel A contains the final two 10-bit words 0x00 and 0xXYZ */
						/* OP_SWAP_CB_CR, IO, Address 0x05[0], Controls the swapping of Cr and Cb data on the pixel buses */
						/* 0 (default) Outputs Cr and Cb as per OP_FORMAT_SEL */
						/* 1 Inverts the order of Cb and Cr in the interleaved data stream */
	{ 0x06, 0xa6 },		/* VS_OUT_SEL, Address 0x06[7], Select the VSync or FIELD signal to be output on the VS/FIELD/ALSB pin */
						/* 0 Selects FIELD output on VS/FIELD/ALSB pin */
						/* 1 (default) Selects VSync output on VS/FIELD/ALSB pin */
						/* INV_F_POL, Address 0x06[3], controls polarity of the DE signal */
						/* 0 (default) Negative FIELD/DE polarity */
						/* 1 Positive FIELD/DE polarity */
						/* INV_VS_POL, IO, Address 0x06[2] Controls polarity of the VS/FIELD/ALSB signal */
						/* 0 (default) Negative polarity VS/FIELD/ALSB */
						/* 1 Positive polarity VS/FIELD/ALSB */
						/* INV_HS_POL, Address 0x06[1], Controls polarity of the HS signal */
						/* 0 (default) Negative polarity HS */
						/* 1 Positive polarity HS */
						/* INV_LLC_POL, Address 0x06[0], Controls the polarity of the LLC */
						/* 0 (default) Does not invert LLC */
						/* 1 Inverts LLC */
	{ 0x0c, 0x42 },		/* Power up part */
	{ 0x14, 0x3F },		/* DR_STR[1:0], IO, Address 0x14[5:4] */
						/* 00 Reserved */
						/* 01 Medium low (2× */
						/* 10 (default) Medium high (3× */
						/* 11 High (4× */
						/* DR_STR_CLK[1:0], IO, Address 0x14[3:2] */
						/* 00 Reserved */
						/* 01 Medium low (2× for LLC up to 60 MHz */
						/* 10 (default) Medium high (3× for LLC from 44 MHz to 105 MHz */
						/* 11 High (4× for LLC greater than 100 MHz */
						/* DR_STR_SYNC[1:0], IO, Address 0x14[1:0] */
						/* 00 Reserved */
						/* 01 Medium low (2× */
						/* 10 (default) Medium high (3× */
						/* 11 High (4× */
	{ 0x15, 0x80 },		/* Disable Tristate of Pins */
/*!!        { 0x19, 0x80 },	%%%%%	LLC DLL phase */
	{ 0x33, 0x40 },		/* LLC DLL MUX enable */
//	{ 0xdd, 0xA0 } 		/* Normal LLC frequency = 0x00 for non-4K modes */
						/* LLC Half frequence = 0xA0 for 4K modes */

	{ 0xdd, 0x00 },		/* ADI Recommended Write per PCN 15_0178 */
	{ 0xE7, 0x04 }		/* ADI Recommended Write per PCN 15_0178 */
};
static int init_io2_4k_size = sizeof(init_io2_4k) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_cp3[] = 
/* %%%%% CP Register - I2C address = 0x44 */
{
	{ 0xba, 0x00 },		/* No HDMI FreeRun */
	{ 0x6c, 0x00 }, 	/* CP clamp disable */
	{ 0x69, 0x10 },
	{ 0x68, 0x00 }
};
static int init_cp3_size = sizeof(init_cp3) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_rep4[] = 
/* Repeater Map Registers - I2C address = 0x64 */
{
	{ 0x40, 0x81 },		/* BCAPS  */
	{ 0x74, 0x03 } 		/* Enable EDID */
};
static int init_rep4_size = sizeof(init_rep4) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_dpll5_non4k[] = 
/* DPLL Registers - I2C address = 0x4C */
{
	{ 0xb5, 0x01 },		/* Setting MCLK to 256Fs */
	{ 0xc3, 0x00 },		/* ADI Recommended Settings (NormFreq) */
	{ 0xcf, 0x00 } 		/* ADI Recommended Settings (NormFreq) */
};
static int init_dpll5_non4k_size = sizeof(init_dpll5_non4k) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_dpll5_4k[] = 
/* DPLL Registers - I2C address = 0x4C */
{
	{ 0xb5, 0x01 },		/* Setting MCLK to 256Fs */
	{ 0xc3, 0x80 },		/* ADI Recommended Settings (NormFreq) */
	{ 0xcf, 0x03 } 		/* ADI Recommended Settings (NormFreq) */
};
static int init_dpll5_4k_size = sizeof(init_dpll5_4k) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_hdmi6[] = 
/* HDMI Registers - I2C address = 0x68 */
{
	{ 0x00, 0x00 },		/* BG_MEAS_PORT_SEL[2:0], Addr 68 (HDMI), Address 0x00[5:3] */
						/* 000 (default) Port A */
						/* 001 Port B */
	{ 0x01, 0x01 },		/* TERM_AUTO, Address 0x01[0] */
						/* This bit allows the user to select automatic or manual control of clock termination */
						/* If automatic mode termination is enabled, then termination on the port HDMI_PORT_SELECT[1:0] is enabled  */
						/* 0 (default) Disable termination automatic control */
						/* 1 Enable termination automatic control */
	{ 0x02, 0x01 },		/* EN_BG_PORT_A, Address 0x02[0] */
						/* 0 (default) Port disabled, unless selected with HDMI_PORT_SELECT[2:0] */
						/* 1 Port enabled in background mode */
						/* EN_BG_PORT_B, Address 0x02[1] */
						/* 0 (default) Port disabled, unless selected with HDMI_PORT_SELECT[2:0] */
						/* 1 Port enabled in background mode */
	{ 0x03, 0x58 },		/* I2SOUTMODE[1:0],  Address 0x03[6:5] */
						/* 00 (default) I2S mode */
						/* 01 Right justified */
						/* 10 Left justified */
						/* 11 Raw SPDIF (IEC60958) mode */
						/* I2SBITWIDTH[4:0], Address 0x03[4:0] */
						/* 11000 24 bits */
	{ 0x6c, 0x01 },		/* HPA_MANUAL, Address 0x6C[0] */
						/* Manual control enable for the HPA output pins */
						/* Manual control is determined by the HPA_MAN_VALUE_A */
						/* 1 HPA takes its value from HPA_MAN_VALUE_A */
	{ 0x3e, 0x69 },
	{ 0x3f, 0x46 },
	{ 0x4e, 0x7e },
	{ 0x4f, 0x42 },
	{ 0x57, 0xa3 },
	{ 0x58, 0x07 },
	{ 0x83, 0xfc },		/* CLOCK_TERMB_DISABLE, Address 0x83[1] */
						/* Disable clock termination on Port B, Can be used when TERM_AUTO set to 0 */
						/* 0 Enable Termination Port B */
						/* 1 (default) Disable Termination Port B */
						/* CLOCK_TERMA_DISABLE, Address 0x83[0] */
						/* Disable clock termination on Port A, Can be used when TERM_AUTO set to 0 */
						/* 0 Enable Termination Port A */
						/* 1 (default) Disable Termination Port A */
						/* Note - TERM_AUTO, Address 0x01[0] set to 1 which overrides this bit */

						/* Required for TMDS frequency 27Mhz and below */

	{ 0x89, 0x03 },
	{ 0x84, 0x03 },

	{ 0x85, 0x11 },		/* ADI Recommended Write */
	{ 0x9C, 0x80 },		/* ADI Recommended Write */
	{ 0x9C, 0xC0 },		/* ADI Recommended Write */
	{ 0x9C, 0x00 },		/* ADI Recommended Write */
	{ 0x85, 0x11 },		/* ADI Recommended Write */
	{ 0x86, 0x9B },		/* ADI Recommended Write */
	{ 0x9b, 0x03 }
};
static int init_hdmi6_size = sizeof(init_hdmi6) / sizeof(struct ntv2_reg_value);

static struct ntv2_reg_value init_hdmi8[] = 
/* HDMI Registers - I2C address = 0x68 */
{
//	{ 0x6c, 0x04 }		/* HPA_MANUAL, Address 0x6C[0] */
	{ 0x6c, 0x54 }		/* HPA_MANUAL, Address 0x6C[0] */
						/* 0 (default)HPA takes its value based on HPA_AUTO_INT_EDID */
						/* HPA_AUTO_INT_EDID[1:0],Address 0x6C[2:1] */
						/* HPA_AUTO_INT_EDID[1:0] */
						/* 10 */
						/* HPA of an HDMI port asserted high after two conditions met */
						/* 1. Internal EDID is active for that port */
						/* 2. Delayed version of cable detect signal CABLE_DET_X_RAW for that port is high */
						/* HPA of an HDMI port immediately deasserted after either of these two conditions are met: */
						/* 1. Internal EDID is de-activated for that port */
						/* 2. Cable detect signal CABLE_DET_X_RAW for that port is low  */
						/* HPA of a specific HDMI port deasserted low immediately after internal E-EDID is de-activated */
};
static int init_hdmi8_size = sizeof(init_hdmi8) / sizeof(struct ntv2_reg_value);

#endif
