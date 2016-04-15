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
static const u8 vfilter_locked_mask				= 0x80;         

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

	{ 0x6C, 0x14 },		/* Auto-assert HPD 100ms after (EDID active & cable detect) */
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
	{ 0x00, 0x02 },		/* ADI Recommended Write */
	{ 0x01, 0x06 },		/* ADI Recommended Write  */
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
	{ 0xdd, 0xA0 } 		/* Normal LLC frequency = 0x00 for non-4K modes */
						/* LLC Half frequence = 0xA0 for 4K modes */
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
	{ 0x6c, 0x04 }		/* HPA_MANUAL, Address 0x6C[0] */
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

static struct ntv2_reg_value init_edid_g[] =
{
	{ 0x00, 0x00 }, { 0x01, 0xFF }, { 0x02, 0xFF }, { 0x03, 0xFF }, 
	{ 0x04, 0xFF }, { 0x05, 0xFF }, { 0x06, 0xFF }, { 0x07, 0x00 }, 
	{ 0x08, 0x05 }, { 0x09, 0x41 }, { 0x0a, 0x22 }, { 0x0b, 0x00 }, 
	{ 0x0c, 0x01 }, { 0x0d, 0x01 }, { 0x0e, 0x01 }, { 0x0f, 0x01 }, 
	{ 0x10, 0x09 }, { 0x11, 0x16 }, { 0x12, 0x01 }, { 0x13, 0x03 }, 
	{ 0x14, 0x80 }, { 0x15, 0x46 }, { 0x16, 0x27 }, { 0x17, 0x78 }, 
	{ 0x18, 0x0A }, { 0x19, 0xE6 }, { 0x1a, 0x98 }, { 0x1b, 0xA3 }, 
	{ 0x1c, 0x54 }, { 0x1d, 0x4A }, { 0x1e, 0x99 }, { 0x1f, 0x26 }, 
	{ 0x20, 0x0F }, { 0x21, 0x4B }, { 0x22, 0x4E }, { 0x23, 0x00 }, 
	{ 0x24, 0x00 }, { 0x25, 0x00 }, { 0x26, 0x01 }, { 0x27, 0x01 }, 
	{ 0x28, 0x01 }, { 0x29, 0x01 }, { 0x2a, 0x01 }, { 0x2b, 0x01 }, 
	{ 0x2c, 0x01 }, { 0x2d, 0x01 }, { 0x2e, 0x01 }, { 0x2f, 0x01 }, 
	{ 0x30, 0x01 }, { 0x31, 0x01 }, { 0x32, 0x01 }, { 0x33, 0x01 }, 
	{ 0x34, 0x01 }, { 0x35, 0x01 }, { 0x36, 0x01 }, { 0x37, 0x1D }, 
	{ 0x38, 0x80 }, { 0x39, 0x3E }, { 0x3a, 0x73 }, { 0x3b, 0x38 }, 
	{ 0x3c, 0x2D }, { 0x3d, 0x40 }, { 0x3e, 0x7E }, { 0x3f, 0x2C }, 
	{ 0x40, 0x45 }, { 0x41, 0x80 }, { 0x42, 0x20 }, { 0x43, 0xC2 }, 
	{ 0x44, 0x31 }, { 0x45, 0x00 }, { 0x46, 0x00 }, { 0x47, 0x1E }, 
	{ 0x48, 0x00 }, { 0x49, 0x00 }, { 0x4a, 0x00 }, { 0x4b, 0xFD }, 
	{ 0x4c, 0x00 }, { 0x4d, 0x18 }, { 0x4e, 0x3C }, { 0x4f, 0x0F }, 
	{ 0x50, 0x44 }, { 0x51, 0x17 }, { 0x52, 0x00 }, { 0x53, 0x0A }, 
	{ 0x54, 0x20 }, { 0x55, 0x20 }, { 0x56, 0x20 }, { 0x57, 0x20 }, 
	{ 0x58, 0x20 }, { 0x59, 0x20 }, { 0x5a, 0x00 }, { 0x5b, 0x00 }, 
	{ 0x5c, 0x00 }, { 0x5d, 0xFF }, { 0x5e, 0x00 }, { 0x5f, 0x30 }, 
	{ 0x60, 0x30 }, { 0x61, 0x30 }, { 0x62, 0x30 }, { 0x63, 0x30 }, 
	{ 0x64, 0x30 }, { 0x65, 0x30 }, { 0x66, 0x30 }, { 0x67, 0x30 }, 
	{ 0x68, 0x30 }, { 0x69, 0x30 }, { 0x6a, 0x30 }, { 0x6b, 0x30 }, 
	{ 0x6c, 0x00 }, { 0x6d, 0x00 }, { 0x6e, 0x00 }, { 0x6f, 0xFC }, 
	{ 0x70, 0x00 }, { 0x71, 0x47 }, { 0x72, 0x34 }, { 0x73, 0x20 }, 
	{ 0x74, 0x49 }, { 0x75, 0x4D }, { 0x76, 0x42 }, { 0x77, 0x0A }, 
	{ 0x78, 0x20 }, { 0x79, 0x20 }, { 0x7a, 0x20 }, { 0x7b, 0x20 }, 
	{ 0x7c, 0x20 }, { 0x7d, 0x20 }, { 0x7e, 0x01 }, { 0x7f, 0x10 }, 
	{ 0x80, 0x02 }, { 0x81, 0x03 }, { 0x82, 0x1E }, { 0x83, 0x31 }, 
	{ 0x84, 0x4E }, { 0x85, 0x05 }, { 0x86, 0x14 }, { 0x87, 0x10 }, 
	{ 0x88, 0x1F }, { 0x89, 0x04 }, { 0x8a, 0x13 }, { 0x8b, 0x06 }, 
	{ 0x8c, 0x15 }, { 0x8d, 0x22 }, { 0x8e, 0x21 }, { 0x8f, 0x20 }, 
	{ 0x90, 0x02 }, { 0x91, 0x11 }, { 0x92, 0x01 }, { 0x93, 0x6A }, 
	{ 0x94, 0x03 }, { 0x95, 0x0C }, { 0x96, 0x00 }, { 0x97, 0x10 }, 
	{ 0x98, 0x00 }, { 0x99, 0x38 }, { 0x9a, 0x2E }, { 0x9b, 0x20 }, 
	{ 0x9c, 0x80 }, { 0x9d, 0x00 }, { 0x9e, 0x02 }, { 0x9f, 0x3A }, 
	{ 0xa0, 0x80 }, { 0xa1, 0x18 }, { 0xa2, 0x71 }, { 0xa3, 0x38 }, 
	{ 0xa4, 0x2D }, { 0xa5, 0x40 }, { 0xa6, 0x58 }, { 0xa7, 0x2C }, 
	{ 0xa8, 0x45 }, { 0xa9, 0x00 }, { 0xaa, 0xC4 }, { 0xab, 0x8E }, 
	{ 0xac, 0x21 }, { 0xad, 0x00 }, { 0xae, 0x00 }, { 0xaf, 0x1F }, 
	{ 0xb0, 0x00 }, { 0xb1, 0x00 }, { 0xb2, 0x00 }, { 0xb3, 0x00 }, 
	{ 0xb4, 0x00 }, { 0xb5, 0x00 }, { 0xb6, 0x00 }, { 0xb7, 0x00 }, 
	{ 0xb8, 0x00 }, { 0xb9, 0x00 }, { 0xba, 0x00 }, { 0xbb, 0x00 }, 
	{ 0xbc, 0x00 }, { 0xbd, 0x00 }, { 0xbe, 0x00 }, { 0xbf, 0x00 }, 
	{ 0xc0, 0x00 }, { 0xc1, 0x00 }, { 0xc2, 0x00 }, { 0xc3, 0x00 }, 
	{ 0xc4, 0x00 }, { 0xc5, 0x00 }, { 0xc6, 0x00 }, { 0xc7, 0x00 }, 
	{ 0xc8, 0x00 }, { 0xc9, 0x00 }, { 0xca, 0x00 }, { 0xcb, 0x00 }, 
	{ 0xcc, 0x00 }, { 0xcd, 0x00 }, { 0xce, 0x00 }, { 0xcf, 0x00 }, 
	{ 0xd0, 0x00 }, { 0xd1, 0x00 }, { 0xd2, 0x00 }, { 0xd3, 0x00 }, 
	{ 0xd4, 0x00 }, { 0xd5, 0x00 }, { 0xd6, 0x00 }, { 0xd7, 0x00 }, 
	{ 0xd8, 0x00 }, { 0xd9, 0x00 }, { 0xda, 0x00 }, { 0xdb, 0x00 }, 
	{ 0xdc, 0x00 }, { 0xdd, 0x00 }, { 0xde, 0x00 }, { 0xdf, 0x00 }, 
	{ 0xe0, 0x00 }, { 0xe1, 0x00 }, { 0xe2, 0x00 }, { 0xe3, 0x00 }, 
	{ 0xe4, 0x00 }, { 0xe5, 0x00 }, { 0xe6, 0x00 }, { 0xe7, 0x00 }, 
	{ 0xe8, 0x00 }, { 0xe9, 0x00 }, { 0xea, 0x00 }, { 0xeb, 0x00 }, 
	{ 0xec, 0x00 }, { 0xed, 0x00 }, { 0xee, 0x00 }, { 0xef, 0x00 }, 
	{ 0xf0, 0x00 }, { 0xf1, 0x00 }, { 0xf2, 0x00 }, { 0xf3, 0x00 }, 
	{ 0xf4, 0x00 }, { 0xf5, 0x00 }, { 0xf6, 0x00 }, { 0xf7, 0x00 }, 
	{ 0xf8, 0x00 }, { 0xf9, 0x00 }, { 0xfa, 0x00 }, { 0xfb, 0x00 }, 
	{ 0xfc, 0x00 }, { 0xfd, 0x00 }, { 0xfe, 0x00 }, { 0xff, 0x99 } 
};
static int init_edid_g_size = sizeof(init_edid_g) / sizeof(struct ntv2_reg_value);
#if 0
static struct ntv2_reg_value init_edid_n[] =
{
	{ 0x00, 0x00 },          // EDID Header Byte 01
	{ 0x01, 0xff },          // EDID Header Byte 02
	{ 0x02, 0xff },          // EDID Header Byte 03
	{ 0x03, 0xff },          // EDID Header Byte 04
	{ 0x04, 0xff },          // EDID Header Byte 05
	{ 0x05, 0xff },          // EDID Header Byte 06
	{ 0x06, 0xff },          // EDID Header Byte 07
	{ 0x07, 0x00 },          // EDID Header Byte 08

									// Block 0 Vendor/Product Information
	{ 0x08, 0x05 },          // ID Manufacturer = AJA
	{ 0x09, 0x41 },          // ID Manufacturer = AJA
	{ 0x0a, 0x07 },          // ID Product Code = 2200
	{ 0x0b, 0xeb },          // ID Product Code = 2200
	{ 0x0c, 0x01 },          // ID Serial number = 01010101
	{ 0x0d, 0x01 },          // ID Serial number = 01010101
	{ 0x0e, 0x01 },          // ID Serial number = 01010101
	{ 0x0f, 0x01 },          // ID Serial number = 01010101
	{ 0x10, 0xff },          // Year only
	{ 0x11, 0x17 },          // Year of Manufacture = 2012

									// Block 0 EDID Version/Revision
	{ 0x12, 0x01 },          // EDID Version = 1.3
	{ 0x13, 0x03 },          // EDID Version = 1.3

									// Block 0 Basic Display Parameters Features
									// Video Input Definition
	{ 0x14, 0x80 },          // Digital
									// Horizontal and Vertical Screen Size
	{ 0x15, 0x46 },          // Horizontal screen = 70 centimeters
	{ 0x16, 0x27 },          // Vertical screen = 39 centimeters
									// Display Transfer Characteristic
	{ 0x17, 0x78 },          // Gamma = 2.20
									// Feature Support
	{ 0x18, 0x0A },          // RGB 4:4:4 and YCrCb 4:4:4
									// Preferred timing mode is not native

									// Block 0 Color Characteristics
	{ 0x19, 0xE6 },          // Red/Green Low Bits Rx1 Rx0 Ry1 Ry0 Gx1 Gx0 Gy1Gy0
	{ 0x1a, 0x98 },          // Blue/White Low Bits Bx1 Bx0 By1 By0 Wx1 Wx0 Wy1 Wy0
	{ 0x1b, 0xA3 },          // Red-x Bits 9 - 2             0.640 (full 10 bits)
	{ 0x1c, 0x54 },          // Red-y Bits 9 - 2             0.330 (full 10 bits)
	{ 0x1d, 0x4A },          // Green-x Bits 9 - 2           0.290 (full 10 bits)
	{ 0x1e, 0x99 },          // Green-y Bits 9 - 2           0.600 (full 10 bits)
	{ 0x1f, 0x26 },          // Blue-x Bits 9 - 2            0.150 (full 10 bits)
	{ 0x20, 0x0F },          // Blue-y Bits 9 - 2            0.060 (full 10 bits)
	{ 0x21, 0x4B },          // White-x Bits 9 - 2           0.295 (full 10 bits)
	{ 0x22, 0x4E },          // White-y Bits 9 - 2           0.305 (full 10 bits)

									// Block 0 Established Timings
	{ 0x23, 0x00 },          // Established Timings 1, none selected
	{ 0x24, 0x00 },          // Established Timings 2, none selected
	{ 0x25, 0x00 },          // Manufacturer's Timings, none selected

									// Block 0 Standard Timing Identification
	{ 0x26, 0x01 },          // Standard Timing Identification 1 - Unused
	{ 0x27, 0x01 },          // Standard Timing Identification 1 - Unused
	{ 0x28, 0x01 },          // Standard Timing Identification 2 - Unused
	{ 0x29, 0x01 },          // Standard Timing Identification 2 - Unused
	{ 0x2a, 0x01 },          // Standard Timing Identification 3 - Unused
	{ 0x2b, 0x01 },          // Standard Timing Identification 3 - Unused
	{ 0x2c, 0x01 },          // Standard Timing Identification 4 - Unused
	{ 0x2d, 0x01 },          // Standard Timing Identification 4 - Unused
	{ 0x2e, 0x01 },          // Standard Timing Identification 5 - Unused
	{ 0x2f, 0x01 },          // Standard Timing Identification 5 - Unused
	{ 0x30, 0x01 },          // Standard Timing Identification 6 - Unused
	{ 0x31, 0x01 },          // Standard Timing Identification 6 - Unused
	{ 0x32, 0x01 },          // Standard Timing Identification 7 - Unused
	{ 0x33, 0x01 },          // Standard Timing Identification 7 - Unused
	{ 0x34, 0x01 },          // Standard Timing Identification 8 - Unused
	{ 0x35, 0x01 },          // Standard Timing Identification 8 - Unused

									// Block 0 Detailed Timing Descriptions
									// Descriptor 1 - Detailed Timing Definition
	{ 0x36, 0x04 },          // Pixel clock = 297 Mhz = 0x7404, LSB = 04
	{ 0x37, 0x74 },          // Pixel clock MSB = 74
	{ 0x38, 0x00 },          // Horizontal active pixels = 3840 = 0x0F00, 00 = lower 8 bits
	{ 0x39, 0x7C },          // Horizontal blanking pixels = 1660 = 0x067C, 7C lower 8 bits
	{ 0x3a, 0xF6 },          // Upper nibble : upper 4 bits of horizontal active pixels
									// Lower nibble : upper 4 bits of horizontal blanking  pixels
	{ 0x3b, 0x70 },          // Vertical Active Lines = 2160 = 0x870, lower 8 bits = 70
	{ 0x3c, 0x5A },          // Vertical Blanking Lines = 90 = 0x5A, lower 8 bits = 5A
	{ 0x3d, 0x80 },          // Upper nibble : upper 4 bits of Vertical Active
									// Lower nibble : upper 4 bits of Vertical Blanking
	{ 0x3e, 0xFC },          // Horizontal front porch = 252 = 0xFC = lower 8 bits = FC
	{ 0x3f, 0x58 },          // Horizontal sync width = 88 = 0x58 = lower 8 bits = 58
	{ 0x40, 0x8A },          // Upper nibble : lines, lower 4 bits of Vertical Sync Offset = 8 lines
									// Lower nibble : lines, lower 4 bits of Vertical Sync Pulse Width = 10 = 0A lines
	{ 0x41, 0x00 },          // bits 7,6 : upper 2 bits of Horizontal Sync Offset
									// bits 5,4 : upper 2 bits of Horizontal Sync Pulse Width
									// bits 3,2 : upper 2 bits of Vertical Sync Offset
									// bits 1,0 : upper 2 bits of Vertical Sync Pulse Width
	{ 0x42, 0x78 },          // Horizontal Image Size = 376 = 0x178, lower 8 bits = 78
	{ 0x43, 0x2D },          // Vertical Image Size = 301 = 0x12D, lower 8 bits = 2D
	{ 0x44, 0x11 },          // Upper nibble : upper 4 bits of Horizontal Image Size
									// Lower nibble : upper 4 bits of Vertical Image Size
	{ 0x45, 0x00 },          // Horizontal Border
	{ 0x46, 0x00 },          // Verticle Border
	{ 0x47, 0x1E },          // Interlace, Stereo, Horizontal polarity, Vertical polarity, Sync Configuration
									// Bit 7 Function, 0 Non-interlaced
									// Bit 6 Bit 5 Function 0 0 Normal display, no stereo
									// Bit 4 Bit 3 Function 1 1 Digital separate
									// Bit 2 Bit 1 Positive sync polarity
									// Bit 0, no stereo, default to 0

									// Descriptor 2 - Range Limits
	{ 0x48, 0x00 },          // Flag 00
	{ 0x49, 0x00 },          // Flag 00
	{ 0x4a, 0x00 },          // Flag 00
	{ 0x4b, 0xfd },          // Monitor range limits, binary coded
	{ 0x4c, 0x00 },          // Flag 00
	{ 0x4d, 0x18 },          // Min vertical rate = 24 = 0x18
	{ 0x4e, 0x3C },          // Max vertical rate = 60 = 0x3C
	{ 0x4f, 0x0F },          // Min horizontal rate = 15 = 0x0F
	{ 0x50, 0x44 },          // Max horizontal rate = 68 = 44
	{ 0x51, 0x1E },          // Max Pixel Clock = 300 = 0x1E
	{ 0x52, 0x00 },          // No secondary timing formula supported
	{ 0x53, 0x0a },          // Terminate with 0x0A
	{ 0x54, 0x20 },          // space
	{ 0x55, 0x20 },          // space
	{ 0x56, 0x20 },          // space
	{ 0x57, 0x20 },          // space
	{ 0x58, 0x20 },          // space
	{ 0x59, 0x20 },          // space

									// Descriptor 3: Product Serial Number
	{ 0x5a, 0x00 },          // Flag 00
	{ 0x5b, 0x00 },          // Flag 00
	{ 0x5c, 0x00 },          // Flag 00
	{ 0x5d, 0xff },          // Product Serial Number
	{ 0x5e, 0x00 },          // Flag 00
	{ 0x5f, 0x30 },          // ASCII 0
	{ 0x60, 0x30 },          // ASCII 0
	{ 0x61, 0x30 },          // ASCII 0
	{ 0x62, 0x30 },          // ASCII 0
	{ 0x63, 0x30 },          // ASCII 0
	{ 0x64, 0x30 },          // ASCII 0
	{ 0x65, 0x30 },          // ASCII 0
	{ 0x66, 0x30 },          // ASCII 0
	{ 0x67, 0x30 },          // ASCII 0
	{ 0x68, 0x30 },          // ASCII 0
	{ 0x69, 0x30 },          // ASCII 0
	{ 0x6a, 0x30 },          // ASCII 0
	{ 0x6b, 0x30 },          // ASCII 0

									// Descriptor 4: Monitor name
	{ 0x6c, 0x00 },          // Flag 00
	{ 0x6d, 0x00 },          // Flag 00
	{ 0x6e, 0x00 },          // Flag 00
	{ 0x6f, 0xfc },          // Monitor name
	{ 0x70, 0x00 },          // Flag 00
	{ 0x71, 0x41 },          // ASCII A
	{ 0x72, 0x4a },          // ASCII J
	{ 0x73, 0x41 },          // ASCII A
	{ 0x74, 0x20 },          // ASCII Space
	{ 0x75, 0x49 },          // ASCII I
	{ 0x76, 0x4f },          // ASCII O
	{ 0x77, 0x34 },          // ASCII 4
	{ 0x78, 0x4b },          // ASCII K
	{ 0x79, 0x0a },          // Terminate with 0x0A
	{ 0x7a, 0x20 },          // ASCII space
	{ 0x7b, 0x20 },          // ASCII space
	{ 0x7c, 0x20 },          // ASCII space
	{ 0x7d, 0x20 },          // ASCII space
	{ 0x7e, 0x01 },          // Extension flag = 01, not a Block map
	{ 0x7f, 0xa0 },          // Checksum

									// Block 1
	{ 0x80, 0x02 },          // CEA-EXT: CEA 861 Series Extension, Refer to the latest revision of the CEA 861 Standard
	{ 0x81, 0x03 },          // Revision number
	{ 0x82, 0x2D },          // Detailed Timing Descriptors start at 0xB2
	{ 0x83, 0x51 },          // bit 7 (underscan) = 1 if sink underscans IT video formats by default.
									// bit 6 (audio) = 1 if sink supports basic audio.
									// bit 5 (YCBCR 4:4:4) = 1 if sink supports YCBCR 4:4:4 in addition to RGB.
									// bit 4 (YCBCR 4:2:2) = 1 if sink supports YCBCR 4:2:2 in addition to RGB.
									// lower 4 bits = total number of native DTDs

									// Video data
	{ 0x84, 0x4b },          // bits 7-5 Video tag code = 010 = 2 = Video Data Block
									// bits 4-0 total number of bytes following this byte = 0xD = 13
	{ 0x85, 0x85 },          // Video Descriptor 1:   Native, VIC = 05 (05H), 1920x1080i 29.97/30            ,16:9
	{ 0x86, 0x94 },          // Video Descriptor 2:   Native, VIC = 20 (14H), 1920x1080i 25                  ,16:9
	{ 0x87, 0x90 },          // Video Descriptor 3:   Native, VIC = 16 (10H), 1920x1080p 59.94Hz/60Hz        ,16:9
	{ 0x88, 0x9f },          // Video Descriptor 4:   Native, VIC = 31 (1FH), 1920x1080p 50Hz                ,16:9
	{ 0x89, 0x84 },          // Video Descriptor 5:   Native, VIC = 04 (04H), 1280x720p 59.94Hz/60Hz         ,16:9
	{ 0x8a, 0x93 },          // Video Descriptor 6:   Native, VIC = 19 (13H), 1280x720p 50Hz                 ,16:9
	{ 0x8b, 0x86 },          // Video Descriptor 7:   Native, VIC = 06 (06H), 720(1440)x480i 59.94Hz/60Hz    ,4:3
	{ 0x8c, 0x95 },          // Video Descriptor 8:   Native, VIC = 21 (15H), 720(1440)x576i 50Hz            ,4:3
	{ 0x8d, 0xA2 },          // Video Descriptor 9:   Native, VIC = 34 (22H), 1280x720p 29.97Hz/30Hz         ,16:9
	{ 0x8e, 0xA1 },          // Video Descriptor 10:  Native, VIC = 33 (21H), 1920x1080p 50Hz                ,16:9
	{ 0x8f, 0xA0 },          // Video Descriptor 11:  Native, VIC = 32 (20H), 1920x1080p 23.97Hz/24Hz        ,16:9

	{ 0x90, 0x23 },			// Audio data
	{ 0x91, 0x0F },          // bits 7-5 Audio tag code = 001 = 1 = Audio Data Block
	{ 0x92, 0x04 },			// bits 4-0 total number of bytes following this byte = 3
	{ 0x93, 0x07 },			// bit 7 = 0
									// bits 6-3 Audio Format code = 0001
									// bits 2-0 = 8 channel, = 111
									// 48 kHz
									// 16,20,and 24 bit

									// Speaker data
	{ 0x94, 0x83 },          // bits 7-5 Speaker tag code = 100 = 4 = Speaker Allocation Block
									// bits 4-0 total number of bytes following this byte = 3
	{ 0x95, 0x2f },          // bit 7 = FLW/FRW = 0
									// bit 6 = RLC/RRC = 0
									// bit 5 = FLC/FRC = 1
									// bit 4 = RC = 0
									// bit 3 = RL/RR = 1
									// bit 2 = FC = 1
									// bit 1 = LFE = 1
									// bit 0 = FL/FR = 1
	{ 0x96, 0x00 },          // bit 2 = FCH = 0
									// bit 1 = TC = 0
									// bit 0 = FLH/FRH = 0
	{ 0x97, 0x00 },          // set to 00

									// HDMI Vendor Specific data
	{ 0x98, 0x70 },          // bits 7-5 Vendor Specific Data = 011 = 3 = Vendor Specific data block
									// bits 4-0 total number of bytes following this byte = 10000 = 16
	{ 0x99, 0x03 },          // IEEE = 000C03 = LSB = 03
	{ 0x9a, 0x0c },          // IEEE = 000C03 = LSB = 0c
	{ 0x9b, 0x00 },          // IEEE = 000C03 = LSB = 28
	{ 0x9c, 0x10 },          // Source Physical Address = 1000
	{ 0x9d, 0x00 },          // Source Physical Address = 1000
	{ 0x9e, 0x38 },          // bit 7 = _AI DC_
									// bit 6 = support 48bit (16 bits/color)
									// bit 5 = support 36bit (12 bits/color)
									// bit 4 = support 30bit (10 bits/color)
									// bit 3 = support YCbCR in deep color
									// bit 2 = reserved
									// bit 1 = reserved
									// bit 0 = support dual DVI
	{ 0x9f, 0x3C },          // Max clock = 300 Mhz, Max rate = Max_TMDS_Clock * 5MHz, 0x3C = 60, 60 * 5 = 300
	{ 0xa0, 0x20 },          // Latency fields not present
	{ 0xa1, 0xA0 },          // bit 7 = 3D present
	{ 0xa2, 0x82 },          // bit 7-5 = HDMI VIC length = 4
									// HDMI_3D length = 02
	{ 0xa3, 0x04 },          // HDMI VIC 04 4k x 2k, 24       Hz, 4096 x 2160, pixel clock = 297
	{ 0xa4, 0x03 },          // HDMI VIC 03 4k x 2k, 23.98,24 Hz, 3840 x 2160, pixel clock = 297
	{ 0xa5, 0x02 },          // HDMI VIC 02 4k x 2k, 25       Hz, 3840 x 2160, pixel clock = 297
	{ 0xa6, 0x01 },          // HDMI VIC 01 4k x 2k, 29.97,30 Hz, 3840 x 2160, pixel clock = 297
	{ 0xa7, 0x00 },
	{ 0xa8, 0x49 },          // 3d Enabled: Modes Side by Side, Top/Bottom, FramePacked
	{ 0xa9, 0x00 },
	{ 0xaa, 0x00 },
	{ 0xab, 0x00 },
	{ 0xac, 0x00 },

									// CEA Detailed Timing Descriptor
	{ 0xad, 0x02 },          // Pixel clock = 148.5 Mhz = 0x3a02, LSB = 02
	{ 0xae, 0x3A },          // Pixel clock MSB = 3A
	{ 0xaf, 0x80 },          // Horizontal active pixels = 1920 = 0x780, 80 = lower 8 bits
	{ 0xb0, 0x18 },          // Horizontal active pixels = 280 = 0x118, 18 lower 8 bits
	{ 0xb1, 0x71 },          // Upper nibble : upper 4 bits of horizontal active pixels
									// Lower nibble : upper 4 bits of horizontal blanking  pixels
	{ 0xb2, 0x38 },          // Vertical Active Lines = 1080 = 0x438, lower 8 bits = 38
	{ 0xb3, 0x2D },          // Vertical Blanking Lines = 45 = 0x2D, lower 8 bits = 2D
	{ 0xb4, 0x40 },          // Upper nibble : upper 4 bits of Vertical Active
									// Lower nibble : upper 4 bits of Vertical Blanking
	{ 0xb5, 0x58 },          // Horizontal front porch = 88 = 0x58 = lower 8 bits = 58
	{ 0xb6, 0x2C },          // Horizontal sync width = 44 = 0x2C = lower 8 bits = 2C
	{ 0xb7, 0x45 },          // Upper nibble : lines, lower 4 bits of Vertical Sync Offset = 4 lines
															// Lower nibble : lines, lower 4 bits of Vertical Sync Pulse Width = 5 lines
	{ 0xb8, 0x00 },          // bits 7,6 : upper 2 bits of Horizontal Sync Offset
									// bits 5,4 : upper 2 bits of Horizontal Sync Pulse Width
									// bits 3,2 : upper 2 bits of Vertical Sync Offset
									// bits 1,0 : upper 2 bits of Vertical Sync Pulse Width
	{ 0xb9, 0xC4 },          // Horizontal Image Size = 708 = 0x2C4, lower 8 bits = C4
	{ 0xba, 0x8E },          // Vertical Image Size = 398 = 0x18E, lower 8 bits = 8E
	{ 0xbb, 0x21 },          // Upper nibble : upper 4 bits of Horizontal Image Size
									// Lower nibble : upper 4 bits of Vertical Image Size
	{ 0xbc, 0x00 },          // Horizontal Border
	{ 0xbd, 0x00 },          // Verticle Border
	{ 0xbe, 0x1E },          // Interlace, Stereo, Horizontal polarity, Vertical polarity, Sync Configuration
									// Bit 7 Function, 0 Non-interlaced
									// Bit 6 Bit 5 Function 0 0 Normal display, no stereo
									// Bit 4 Bit 3 Function 1 1 Digital separate
									// Bit 2 Bit 1 Positive sync polarity
									// Bit 0, stereo

	{ 0xbf, 0x00 },
	{ 0xc0, 0x00 },
	{ 0xc1, 0x00 },
	{ 0xc2, 0x00 },
	{ 0xc3, 0x00 },
	{ 0xc4, 0x00 },
	{ 0xc5, 0x00 },
	{ 0xc6, 0x00 },
	{ 0xc7, 0x00 },
	{ 0xc8, 0x00 },
	{ 0xc9, 0x00 },
	{ 0xca, 0x00 },
	{ 0xcb, 0x00 },
	{ 0xcc, 0x00 },
	{ 0xcd, 0x00 },
	{ 0xce, 0x00 },
	{ 0xcf, 0x00 },

	{ 0xd0, 0x00 },
	{ 0xd1, 0x00 },
	{ 0xd2, 0x00 },
	{ 0xd3, 0x00 },
	{ 0xd4, 0x00 },
	{ 0xd5, 0x00 },
	{ 0xd6, 0x00 },
	{ 0xd7, 0x00 },
	{ 0xd8, 0x00 },
	{ 0xd9, 0x00 },
	{ 0xda, 0x00 },
	{ 0xdb, 0x00 },
	{ 0xdc, 0x00 },
	{ 0xdd, 0x00 },
	{ 0xde, 0x00 },
	{ 0xdf, 0x00 },

	{ 0xe0, 0x00 },
	{ 0xe1, 0x00 },
	{ 0xe2, 0x00 },
	{ 0xe3, 0x00 },
	{ 0xe4, 0x00 },
	{ 0xe5, 0x00 },
	{ 0xe6, 0x00 },
	{ 0xe7, 0x00 },
	{ 0xe8, 0x00 },
	{ 0xe9, 0x00 },
	{ 0xea, 0x00 },
	{ 0xeb, 0x00 },
	{ 0xec, 0x00 },
	{ 0xed, 0x00 },
	{ 0xee, 0x00 },
	{ 0xef, 0x00 },

	{ 0xf0, 0x00 },
	{ 0xf1, 0x00 },
	{ 0xf2, 0x00 },
	{ 0xf3, 0x00 },
	{ 0xf4, 0x00 },
	{ 0xf5, 0x00 },
	{ 0xf6, 0x00 },
	{ 0xf7, 0x00 },
	{ 0xf8, 0x00 },
	{ 0xf9, 0x00 },
	{ 0xfa, 0x00 },
	{ 0xfb, 0x00 },
	{ 0xfc, 0x00 },
	{ 0xfd, 0x00 },
	{ 0xfe, 0x00 },
	{ 0xff, 0x0A }            // Checksum
};
static int init_edid_n_size = sizeof(init_edid_n) / sizeof(struct ntv2_reg_value);
#endif

#endif
