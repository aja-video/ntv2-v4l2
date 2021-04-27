/*
 * NTV2 video/audio register constants
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

#ifndef NTV2_KONAREG_H
#define NTV2_KONAREG_H

#include "ntv2_common.h"

/* video frame flags */
NTV2_CON(ntv2_kona_frame_none,								0x00000000);
NTV2_CON(ntv2_kona_frame_picture_progressive,				0x00000001);	/* picture progressive */
NTV2_CON(ntv2_kona_frame_picture_interlaced,				0x00000002);	/* picture interlaced */
NTV2_CON(ntv2_kona_frame_transport_progressive,				0x00000004);	/* transport progressive */
NTV2_CON(ntv2_kona_frame_transport_interlaced,				0x00000008);	/* transport interlaced */
NTV2_CON(ntv2_kona_frame_sd,								0x00000010);	/* data rate sd */
NTV2_CON(ntv2_kona_frame_hd,								0x00000020);	/* data rate hd */
NTV2_CON(ntv2_kona_frame_3g,								0x00000040);	/* data rate 3g */
NTV2_CON(ntv2_kona_frame_6g,								0x00000080);	/* data rate 6g */
NTV2_CON(ntv2_kona_frame_12g,								0x00000100);	/* data rate 12g */
NTV2_CON(ntv2_kona_frame_3ga,								0x00001000);	/* sdi transport 3ga */
NTV2_CON(ntv2_kona_frame_3gb,								0x00002000);	/* sdi transport 3gb */
NTV2_CON(ntv2_kona_frame_dual_link,							0x00004000);	/* sdi transport smpte 372 4444 */
NTV2_CON(ntv2_kona_frame_line_interleave,					0x00008000);	/* sdi transport smpte 372 >30 fps */
NTV2_CON(ntv2_kona_frame_square_division,					0x00010000);	/* transport square division */
NTV2_CON(ntv2_kona_frame_sample_interleave,					0x00020000);	/* transport sample interleaved */
NTV2_CON(ntv2_kona_frame_4x3,								0x01000000);	/* 4x3 aspect */
NTV2_CON(ntv2_kona_frame_16x9,								0x02000000);	/* 16x9 aspect */

/* video pixel flags */
NTV2_CON(ntv2_kona_pixel_none,								0x00000000);
NTV2_CON(ntv2_kona_pixel_yuv,								0x00000001);	/* yuv color space */
NTV2_CON(ntv2_kona_pixel_rgb,								0x00000002);	/* rgb color space */
NTV2_CON(ntv2_kona_pixel_full,								0x00000004);	/* full range black - white */
NTV2_CON(ntv2_kona_pixel_smpte,								0x00000008);	/* smpte range black - white */
NTV2_CON(ntv2_kona_pixel_rec601,							0x00000010);	/* rec 601 color standard */
NTV2_CON(ntv2_kona_pixel_rec709,							0x00000020);	/* rec 709 color standard */
NTV2_CON(ntv2_kona_pixel_rec2020,							0x00000040);	/* rec 2020 color standard */
NTV2_CON(ntv2_kona_pixel_adobe,								0x00000080);	/* adobe color standard */
NTV2_CON(ntv2_kona_pixel_420,								0x00000100);	/* 420 component format */
NTV2_CON(ntv2_kona_pixel_422,								0x00000200);	/* 422 component format */
NTV2_CON(ntv2_kona_pixel_444,								0x00000400);	/* 444 component format */
NTV2_CON(ntv2_kona_pixel_4444,								0x00000800);	/* 4444 component format */
NTV2_CON(ntv2_kona_pixel_4224,								0x00001000);	/* 4224 component format */
NTV2_CON(ntv2_kona_pixel_8bit,								0x00010000);	/* 8 bit component resolution */
NTV2_CON(ntv2_kona_pixel_10bit,								0x00020000);	/* 10 bit component resolution */
NTV2_CON(ntv2_kona_pixel_12bit,								0x00040000);	/* 12 bit component resolution */
NTV2_CON(ntv2_kona_pixel_16bit,								0x00080000);	/* 16 bit component resolution */

/* video frame rate */
NTV2_CON(ntv2_kona_frame_rate_none,							0);
NTV2_CON(ntv2_kona_frame_rate_6000,							1);
NTV2_CON(ntv2_kona_frame_rate_5994,							2);
NTV2_CON(ntv2_kona_frame_rate_3000,							3);
NTV2_CON(ntv2_kona_frame_rate_2997,							4);
NTV2_CON(ntv2_kona_frame_rate_2500,							5);
NTV2_CON(ntv2_kona_frame_rate_2400,							6);
NTV2_CON(ntv2_kona_frame_rate_2398,							7);
NTV2_CON(ntv2_kona_frame_rate_5000,							8);
NTV2_CON(ntv2_kona_frame_rate_4800,							9);
NTV2_CON(ntv2_kona_frame_rate_4795,							10);
NTV2_CON(ntv2_kona_frame_rate_12000,						11);
NTV2_CON(ntv2_kona_frame_rate_11988,						12);
NTV2_CON(ntv2_kona_frame_rate_1500,							13);
NTV2_CON(ntv2_kona_frame_rate_1400,							14);

/* video frame geometry */
NTV2_CON(ntv2_kona_frame_geometry_1920x1080,				0);
NTV2_CON(ntv2_kona_frame_geometry_1280x720,					1);
NTV2_CON(ntv2_kona_frame_geometry_720x486,					2);
NTV2_CON(ntv2_kona_frame_geometry_720x576,					3);
NTV2_CON(ntv2_kona_frame_geometry_1920x1114,				4);
NTV2_CON(ntv2_kona_frame_geometry_2048x1114,				5);
NTV2_CON(ntv2_kona_frame_geometry_720x508,					6);
NTV2_CON(ntv2_kona_frame_geometry_720x598,					7);
NTV2_CON(ntv2_kona_frame_geometry_1920x1112,				8);
NTV2_CON(ntv2_kona_frame_geometry_1280x740,					9);
NTV2_CON(ntv2_kona_frame_geometry_2048x1080,				10);
NTV2_CON(ntv2_kona_frame_geometry_2048x1556,				11);
NTV2_CON(ntv2_kona_frame_geometry_2048x1588,				12);
NTV2_CON(ntv2_kona_frame_geometry_2048x1112,				13);
NTV2_CON(ntv2_kona_frame_geometry_720x514,					14);
NTV2_CON(ntv2_kona_frame_geometry_720x612,					15);
NTV2_CON(ntv2_kona_frame_geometry_4x1920x1080,				16);
NTV2_CON(ntv2_kona_frame_geometry_4x2048x1080,				17);

/* video input geometry */
NTV2_CON(ntv2_kona_input_geometry_none,						0);
NTV2_CON(ntv2_kona_input_geometry_525,						1);
NTV2_CON(ntv2_kona_input_geometry_625,						2);
NTV2_CON(ntv2_kona_input_geometry_750,						3);
NTV2_CON(ntv2_kona_input_geometry_1125,						4);

/* video standard */
NTV2_CON(ntv2_kona_video_standard_1080i,					0);
NTV2_CON(ntv2_kona_video_standard_720p,						1);
NTV2_CON(ntv2_kona_video_standard_525i,						2);
NTV2_CON(ntv2_kona_video_standard_625i,						3);
NTV2_CON(ntv2_kona_video_standard_1080p,					4);
NTV2_CON(ntv2_kona_video_standard_2048x1556,				5);
NTV2_CON(ntv2_kona_video_standard_2048x1080p,				6);
NTV2_CON(ntv2_kona_video_standard_2048x1080i,				7);
NTV2_CON(ntv2_kona_video_standard_3840x2160p,				8);
NTV2_CON(ntv2_kona_video_standard_4096x2160p,				9);
NTV2_CON(ntv2_kona_video_standard_3840_hfr,					10);
NTV2_CON(ntv2_kona_video_standard_4096_hfr,					11);
NTV2_CON(ntv2_kona_video_standard_none,						12);

/* color space */
NTV2_CON(ntv2_kona_color_space_yuv422,						0);
NTV2_CON(ntv2_kona_color_space_rgb444,						1);
NTV2_CON(ntv2_kona_color_space_yuv444,						2);
NTV2_CON(ntv2_kona_color_space_yuv420,						3);
NTV2_CON(ntv2_kona_color_space_none,						4);

/* color depth */
NTV2_CON(ntv2_kona_color_depth_8bit,						0);
NTV2_CON(ntv2_kona_color_depth_10bit,						1);
NTV2_CON(ntv2_kona_color_depth_12bit,						2);
NTV2_CON(ntv2_kona_color_depth_none,						3);

/* sdi interface */
NTV2_CON(ntv2_kona_sdi_interface_none,						0);
NTV2_CON(ntv2_kona_sdi_interface_sd,						1);
NTV2_CON(ntv2_kona_sdi_interface_hd,						2);
NTV2_CON(ntv2_kona_sdi_interface_3ga,						3);
NTV2_CON(ntv2_kona_sdi_interface_3gb,						4);
NTV2_CON(ntv2_kona_sdi_interface_6gb,						5);
NTV2_CON(ntv2_kona_sdi_interface_12gb,						6);

/* register synchronization */
NTV2_CON(ntv2_kona_reg_sync_field,							0);
NTV2_CON(ntv2_kona_reg_sync_frame,							1);
NTV2_CON(ntv2_kona_reg_sync_immediate,						2);
NTV2_CON(ntv2_kona_reg_sync_field_10_lines,					3);

/* reference source */
NTV2_CON(ntv2_kona_ref_source_external,						0);
NTV2_CON(ntv2_kona_ref_source_sdiin1,						1);
NTV2_CON(ntv2_kona_ref_source_sdiin2,						2);
NTV2_CON(ntv2_kona_ref_source_freerun,						3);
NTV2_CON(ntv2_kona_ref_source_anain1,						4);
NTV2_CON(ntv2_kona_ref_source_hdmiin1,						5);
NTV2_CON(ntv2_kona_ref_source_sdiin3,						6);
NTV2_CON(ntv2_kona_ref_source_sdiin4,						7);
NTV2_CON(ntv2_kona_ref_source_sdiin5,						8);
NTV2_CON(ntv2_kona_ref_source_sdiin6,						9);
NTV2_CON(ntv2_kona_ref_source_sdiin7,						10);
NTV2_CON(ntv2_kona_ref_source_sdiin8,						11);

/* frame buffer format */
NTV2_CON(ntv2_kona_fbf_10bit_ycbcr,							0);
NTV2_CON(ntv2_kona_fbf_8bit_ycbcr,							1);
NTV2_CON(ntv2_kona_fbf_argb,								2);
NTV2_CON(ntv2_kona_fbf_rgba,								3);
NTV2_CON(ntv2_kona_fbf_10bit_rgb,							4);
NTV2_CON(ntv2_kona_fbf_8bit_yuy2,							5);
NTV2_CON(ntv2_kona_fbf_abgr,								6);
NTV2_CON(ntv2_kona_fbf_10bit_dpx,							7);
NTV2_CON(ntv2_kona_fbf_10bit_ycbcr_dpx,						8);
NTV2_CON(ntv2_kona_fbf_8bit_dvcpro,							9);
NTV2_CON(ntv2_kona_fbf_8bit_qrez,							10);
NTV2_CON(ntv2_kona_fbf_8bit_hdv,							11);
NTV2_CON(ntv2_kona_fbf_24bit_rgb,							12);
NTV2_CON(ntv2_kona_fbf_24bit_bgr,							13);
NTV2_CON(ntv2_kona_fbf_10bit_ycbcra,						14);
NTV2_CON(ntv2_kona_fbf_10bit_dpx_le,						15);
NTV2_CON(ntv2_kona_fbf_48bit_rgb,							16);
NTV2_CON(ntv2_kona_fbf_10bit_rgb_packed,					20);
NTV2_CON(ntv2_kona_fbf_10bit_argb,							21);
NTV2_CON(ntv2_kona_fbf_16bit_argb,							22);
NTV2_CON(ntv2_kona_fbf_10bit_raw_rgb,						24);
NTV2_CON(ntv2_kona_fbf_10bit_raw_ycbcr,						25);

/* frame buffer size */
NTV2_CON(ntv2_kona_frame_size_2mb,							0);
NTV2_CON(ntv2_kona_frame_size_4mb,							1);
NTV2_CON(ntv2_kona_frame_size_8mb,							2);
NTV2_CON(ntv2_kona_frame_size_16mb,							3);
NTV2_CON(ntv2_kona_frame_size_6mb,							4);
NTV2_CON(ntv2_kona_frame_size_10mb,							5);
NTV2_CON(ntv2_kona_frame_size_12mb,							6);
NTV2_CON(ntv2_kona_frame_size_14mb,							7);
NTV2_CON(ntv2_kona_frame_size_18mb,							8);
NTV2_CON(ntv2_kona_frame_size_20mb,							9);
NTV2_CON(ntv2_kona_frame_size_22mb,							10);
NTV2_CON(ntv2_kona_frame_size_24mb,							11);
NTV2_CON(ntv2_kona_frame_size_26mb,							12);
NTV2_CON(ntv2_kona_frame_size_28mb,							13);
NTV2_CON(ntv2_kona_frame_size_30mb,							14);
NTV2_CON(ntv2_kona_frame_size_32mb,							15);

/* video crosspoint source */
NTV2_CON(ntv2_kona_xpt_black,								0x00);
NTV2_CON(ntv2_kona_xpt_sdiin1_ds1,							0x01);
NTV2_CON(ntv2_kona_xpt_sdiin1_ds2,							0x1e);
NTV2_CON(ntv2_kona_xpt_sdiin2_ds1,							0x02);
NTV2_CON(ntv2_kona_xpt_sdiin2_ds2,							0x1f);
NTV2_CON(ntv2_kona_xpt_lut1_yuv,							0x04);
NTV2_CON(ntv2_kona_xpt_csc1_vid_yuv,						0x05);
NTV2_CON(ntv2_kona_xpt_conv1,								0x06);
NTV2_CON(ntv2_kona_xpt_comp1,								0x07);
NTV2_CON(ntv2_kona_xpt_fb1_ds1_yuv,							0x08);
NTV2_CON(ntv2_kona_xpt_fs1_yuv,								0x09);
NTV2_CON(ntv2_kona_xpt_fs2_yuv,								0x0a);
NTV2_CON(ntv2_kona_xpt_dlout1_ds1,							0x0b);
NTV2_CON(ntv2_kona_xpt_dlout1_ds2,							0x26);
NTV2_CON(ntv2_kona_xpt_dlout2_ds1,							0x1c);
NTV2_CON(ntv2_kona_xpt_dlout2_ds2,							0x27);
NTV2_CON(ntv2_kona_xpt_dlout3_ds1,							0x36);
NTV2_CON(ntv2_kona_xpt_dlout3_ds2,							0x37);
NTV2_CON(ntv2_kona_xpt_dlout4_ds1,							0x38);
NTV2_CON(ntv2_kona_xpt_dlout4_ds2,							0x39);
NTV2_CON(ntv2_kona_xpt_alpha_out,							0x0c);
NTV2_CON(ntv2_kona_xpt_alpha_in,							0x16);
NTV2_CON(ntv2_kona_xpt_hdmiin1_yuv_q1,						0x17);
NTV2_CON(ntv2_kona_xpt_hdmiin1_yuv_q2,						0x41);
NTV2_CON(ntv2_kona_xpt_hdmiin1_yuv_q3,						0x42);
NTV2_CON(ntv2_kona_xpt_hdmiin1_yuv_q4,						0x43);
NTV2_CON(ntv2_kona_xpt_hdmiin1_rgb_q1,						0x97);
NTV2_CON(ntv2_kona_xpt_hdmiin1_rgb_q2,						0xc1);
NTV2_CON(ntv2_kona_xpt_hdmiin1_rgb_q3,						0xc2);
NTV2_CON(ntv2_kona_xpt_hdmiin1_rgb_q4,						0xc3);
NTV2_CON(ntv2_kona_xpt_dlin1,								0x83);
NTV2_CON(ntv2_kona_xpt_dlin2,								0xa8);
NTV2_CON(ntv2_kona_xpt_dlin3,								0xb4);
NTV2_CON(ntv2_kona_xpt_dlin4,								0xb5);
NTV2_CON(ntv2_kona_xpt_lut1_rgb,							0x84);
NTV2_CON(ntv2_kona_xpt_csc1_vid_rgb,						0x85);
NTV2_CON(ntv2_kona_xpt_fb1_ds1_rgb,							0x88);
NTV2_CON(ntv2_kona_xpt_fs1_rgb,								0x89);
NTV2_CON(ntv2_kona_xpt_fs2_rgb,								0x8a);
NTV2_CON(ntv2_kona_xpt_lut2_rgb,							0x8d);
NTV2_CON(ntv2_kona_xpt_csc1_key_yuv,						0x0e);
NTV2_CON(ntv2_kona_xpt_fb2_ds1_yuv,							0x0f);
NTV2_CON(ntv2_kona_xpt_fb2_ds1_rgb,							0x8f);
NTV2_CON(ntv2_kona_xpt_csc2_vid_yuv,						0x10);
NTV2_CON(ntv2_kona_xpt_csc2_vid_rgb,						0x90);
NTV2_CON(ntv2_kona_xpt_csc2_key_yuv,						0x11);
NTV2_CON(ntv2_kona_xpt_mix1_vid_yuv,						0x12);
NTV2_CON(ntv2_kona_xpt_mix1_key_yuv,						0x13);
NTV2_CON(ntv2_kona_xpt_wm1_rgb,								0x94);
NTV2_CON(ntv2_kona_xpt_wm1_yuv,								0x14);
NTV2_CON(ntv2_kona_xpt_wm2_rgb,								0x9a);
NTV2_CON(ntv2_kona_xpt_wm2_yuv,								0x1a);
NTV2_CON(ntv2_kona_xpt_iict1_rgb,							0x95);
NTV2_CON(ntv2_kona_xpt_iict2_rgb,							0x9b);
NTV2_CON(ntv2_kona_xpt_testpat_yuv,							0x1d);
NTV2_CON(ntv2_kona_xpt_dcimix1_vid_yuv,						0x22);
NTV2_CON(ntv2_kona_xpt_dcimix_vid_rgb,						0xa2);
NTV2_CON(ntv2_kona_xpt_mix2_vid_yuv,						0x20);
NTV2_CON(ntv2_kona_xpt_mix2_key_yuv,						0x21);
NTV2_CON(ntv2_kona_xpt_stereo_comp1,						0x23);
NTV2_CON(ntv2_kona_xpt_lut3,								0xa9);
NTV2_CON(ntv2_kona_xpt_lut4,								0xaa);
NTV2_CON(ntv2_kona_xpt_fb3_ds1_yuv,							0x24);
NTV2_CON(ntv2_kona_xpt_fb3_ds1_rgb,							0xa4);
NTV2_CON(ntv2_kona_xpt_fb4_ds1_yuv,							0x25);
NTV2_CON(ntv2_kona_xpt_fb4_ds1_rgb,							0xa5);
NTV2_CON(ntv2_kona_xpt_sdiin3_ds1,							0x30);
NTV2_CON(ntv2_kona_xpt_sdiin3_ds2,							0x32);
NTV2_CON(ntv2_kona_xpt_sdiin4_ds1,							0x31);
NTV2_CON(ntv2_kona_xpt_sdiin4_ds2,							0x33);
NTV2_CON(ntv2_kona_xpt_csc3_vid_yuv,						0x3a);
NTV2_CON(ntv2_kona_xpt_csc3_vid_rgb,						0xba);
NTV2_CON(ntv2_kona_xpt_csc3_key_yuv,						0x3b);
NTV2_CON(ntv2_kona_xpt_csc4_vid_yuv,						0x3c);
NTV2_CON(ntv2_kona_xpt_csc4_vid_rgb,						0xbc);
NTV2_CON(ntv2_kona_xpt_csc4_key_yuv,						0x3d);
NTV2_CON(ntv2_kona_xpt_csc5_vid_yuv,						0x2c);
NTV2_CON(ntv2_kona_xpt_csc5_vid_rgb,						0xac);
NTV2_CON(ntv2_kona_xpt_csc5_key_yuv,						0x2d);
NTV2_CON(ntv2_kona_xpt_lut5,								0xab);
NTV2_CON(ntv2_kona_xpt_dlout5_ds1,							0x3e);
NTV2_CON(ntv2_kona_xpt_dlout5_ds2,							0x3f);
NTV2_CON(ntv2_kona_xpt_4kdc1_yuv,							0x44);
NTV2_CON(ntv2_kona_xpt_4kdc1_rgb,							0xc4);
NTV2_CON(ntv2_kona_xpt_fb5_yuv,								0x51);
NTV2_CON(ntv2_kona_xpt_fb5_ds1_rgb,							0xd1);
NTV2_CON(ntv2_kona_xpt_fb6_ds1_yuv,							0x52);
NTV2_CON(ntv2_kona_xpt_fb6_ds1_rgb,							0xd2);
NTV2_CON(ntv2_kona_xpt_fb7_ds1_yuv,							0x53);
NTV2_CON(ntv2_kona_xpt_fb7_ds1_rgb,							0xd3);
NTV2_CON(ntv2_kona_xpt_fb8_ds1_yuv,							0x54);
NTV2_CON(ntv2_kona_xpt_fb8_ds1_rgb,							0xd4);
NTV2_CON(ntv2_kona_xpt_sdiin5_ds1,							0x45);
NTV2_CON(ntv2_kona_xpt_sdiin5_ds2,							0x47);
NTV2_CON(ntv2_kona_xpt_sdiin6_ds1,							0x46);
NTV2_CON(ntv2_kona_xpt_sdiin6_ds2,							0x48);
NTV2_CON(ntv2_kona_xpt_sdiin7_ds1,							0x49);
NTV2_CON(ntv2_kona_xpt_sdiin7_ds2,							0x4b);
NTV2_CON(ntv2_kona_xpt_sdiin8_ds1,							0x4a);
NTV2_CON(ntv2_kona_xpt_sdiin8_ds2,							0x4c);
NTV2_CON(ntv2_kona_xpt_csc6_vid_yuv,						0x59);
NTV2_CON(ntv2_kona_xpt_csc6_vid_rgb,						0xd9);
NTV2_CON(ntv2_kona_xpt_csc6_key_yuv,						0x5a);
NTV2_CON(ntv2_kona_xpt_csc7_vid_yuv,						0x5b);
NTV2_CON(ntv2_kona_xpt_csc7_vid_rgb,						0xdb);
NTV2_CON(ntv2_kona_xpt_csc7_key_yuv,						0x5c);
NTV2_CON(ntv2_kona_xpt_csc8_vid_yuv,						0x5d);
NTV2_CON(ntv2_kona_xpt_csc8_vid_rgb,						0xdd);
NTV2_CON(ntv2_kona_xpt_csc8_key_yuv,						0x5e);
NTV2_CON(ntv2_kona_xpt_lut6,								0xdf);
NTV2_CON(ntv2_kona_xpt_lut7,								0xe0);
NTV2_CON(ntv2_kona_xpt_lut8,								0xe1);
NTV2_CON(ntv2_kona_xpt_dlout6_ds1,							0x62);
NTV2_CON(ntv2_kona_xpt_dlout6_ds2,							0x63);
NTV2_CON(ntv2_kona_xpt_dlout7_ds1,							0x64);
NTV2_CON(ntv2_kona_xpt_dlout7_ds2,							0x65);
NTV2_CON(ntv2_kona_xpt_dlout8_ds1,							0x66);
NTV2_CON(ntv2_kona_xpt_dlout8_ds2,							0x67);
NTV2_CON(ntv2_kona_xpt_mix3_vid_yuv,						0x55);
NTV2_CON(ntv2_kona_xpt_mix3_key_yuv,						0x56);
NTV2_CON(ntv2_kona_xpt_mix4_vid_yuv,						0x57);
NTV2_CON(ntv2_kona_xpt_mix4_key_yuv,						0x58);
NTV2_CON(ntv2_kona_xpt_dlin5,								0xcd);
NTV2_CON(ntv2_kona_xpt_dlin6,								0xce);
NTV2_CON(ntv2_kona_xpt_dlin7,								0xcf);
NTV2_CON(ntv2_kona_xpt_dlin8,								0xd0);
NTV2_CON(ntv2_kona_xpt_425mux1_ds1_yuv,						0x68);
NTV2_CON(ntv2_kona_xpt_425mux1_ds1_rgb,						0xe8);
NTV2_CON(ntv2_kona_xpt_425mux1_ds2_yuv,						0x69);
NTV2_CON(ntv2_kona_xpt_425mux1_ds2_rgb,						0xe9);
NTV2_CON(ntv2_kona_xpt_425mux2_ds1_yuv,						0x6a);
NTV2_CON(ntv2_kona_xpt_425mux2_ds1_rgb,						0xea);
NTV2_CON(ntv2_kona_xpt_425mux2_ds2_yuv,						0x6b);
NTV2_CON(ntv2_kona_xpt_425mux2_ds2_rgb,						0xeb);
NTV2_CON(ntv2_kona_xpt_425mux3_ds1_yuv,						0x6c);
NTV2_CON(ntv2_kona_xpt_425mux3_ds1_rgb,						0xec);
NTV2_CON(ntv2_kona_xpt_425mux3_ds2_yuv,						0x6d);
NTV2_CON(ntv2_kona_xpt_425mux3_ds2_rgb,						0xed);
NTV2_CON(ntv2_kona_xpt_425mux4_ds1_yuv,						0x6e);
NTV2_CON(ntv2_kona_xpt_425mux4_ds1_rgb,						0xee);
NTV2_CON(ntv2_kona_xpt_425mux4_ds2_yuv,						0x6f);
NTV2_CON(ntv2_kona_xpt_425mux4_ds2_rgb,						0xef);
NTV2_CON(ntv2_kona_xpt_fb1_ds2_yuv,							0x70);
NTV2_CON(ntv2_kona_xpt_fb1_ds2_rgb,							0xf0);
NTV2_CON(ntv2_kona_xpt_fb2_ds2_yuv,							0x71);
NTV2_CON(ntv2_kona_xpt_fb2_ds2_rgb,							0xf1);
NTV2_CON(ntv2_kona_xpt_fb3_ds2_yuv,							0x72);
NTV2_CON(ntv2_kona_xpt_fb3_ds2_rgb,							0xf2);
NTV2_CON(ntv2_kona_xpt_fb4_ds2_yuv,							0x73);
NTV2_CON(ntv2_kona_xpt_fb4_ds2_rgb,							0xf3);
NTV2_CON(ntv2_kona_xpt_fb5_ds2_yuv,							0x74);
NTV2_CON(ntv2_kona_xpt_fb5_ds2_rgb,							0xf4);
NTV2_CON(ntv2_kona_xpt_fb6_ds2_yuv,							0x75);
NTV2_CON(ntv2_kona_xpt_fb6_ds2_rgb,							0xf5);
NTV2_CON(ntv2_kona_xpt_fb7_ds2_yuv,							0x76);
NTV2_CON(ntv2_kona_xpt_fb7_ds2_rgb,							0xf6);
NTV2_CON(ntv2_kona_xpt_fb8_ds2_yuv,							0x77);
NTV2_CON(ntv2_kona_xpt_fb8_ds2_rgb,							0xf7);
NTV2_CON(ntv2_kona_xpt_hdmiin2_yuv_q1,						0x78);
NTV2_CON(ntv2_kona_xpt_hdmiin2_yuv_q2,						0x79);
NTV2_CON(ntv2_kona_xpt_hdmiin2_yuv_q3,						0x7a);
NTV2_CON(ntv2_kona_xpt_hdmiin2_yuv_q4,						0x7b);
NTV2_CON(ntv2_kona_xpt_hdmiin2_rgb_q1,						0xf8);
NTV2_CON(ntv2_kona_xpt_hdmiin2_rgb_q2,						0xf9);
NTV2_CON(ntv2_kona_xpt_hdmiin2_rgb_q3,						0xfa);
NTV2_CON(ntv2_kona_xpt_hdmiin2_rgb_q4,						0xfb);
NTV2_CON(ntv2_kona_xpt_hdmiin3_yuv_q1,						0x7c);
NTV2_CON(ntv2_kona_xpt_hdmiin3_rgb_q1,						0xfc);
NTV2_CON(ntv2_kona_xpt_hdmiin4_yuv_q1,						0x7d);
NTV2_CON(ntv2_kona_xpt_hdmiin4_rgb_q1,						0xfd);

/* audio source */
NTV2_CON(ntv2_kona_audio_source_aes,						0x0);
NTV2_CON(ntv2_kona_audio_source_embedded,					0x1);
NTV2_CON(ntv2_kona_audio_source_analog,						0x9);
NTV2_CON(ntv2_kona_audio_source_hdmi,						0xa);

/* audio rate */
NTV2_CON(ntv2_kona_audio_rate_48Khz,						0);
NTV2_CON(ntv2_kona_audio_rate_96Khz,						1);

/* board id register */
NTV2_REG(ntv2_kona_reg_device_id,							50);
NTV2_REG(ntv2_kona_reg_frame_buffer_offset,					194);

/* interrupt control registers */
NTV2_REG(ntv2_kona_reg_interrupt_control,					20);
NTV2_FLD(ntv2_kona_fld_out1_vertical_enable,				1,	0);
NTV2_FLD(ntv2_kona_fld_in1_vertical_enable,					1,	1);
NTV2_FLD(ntv2_kona_fld_in2_vertical_enable,					1,	2);
NTV2_FLD(ntv2_kona_fld_uart1_tx_enable,						1,	7);
NTV2_FLD(ntv2_kona_fld_uart1_rx_enable,						1,	8);
NTV2_FLD(ntv2_kona_fld_uart1_rx_clear,						1,	15);
NTV2_FLD(ntv2_kona_fld_out2_vertical_enable,				1,	18);
NTV2_FLD(ntv2_kona_fld_out3_vertical_enable,				1,	19);
NTV2_FLD(ntv2_kona_fld_out4_vertical_enable,				1,	20);
NTV2_FLD(ntv2_kona_fld_out4_vertical_clear,					1,	21);
NTV2_FLD(ntv2_kona_fld_out3_vertical_clear,					1,	22);
NTV2_FLD(ntv2_kona_fld_out2_vertical_clear,					1,	23);
NTV2_FLD(ntv2_kona_fld_uart1_tx_clear,						1,	24);
NTV2_FLD(ntv2_kona_fld_in2_vertical_clear,					1,	29);
NTV2_FLD(ntv2_kona_fld_in1_vertical_clear,					1,	30);
NTV2_FLD(ntv2_kona_fld_out1_vertical_clear,					1,	31);

NTV2_REG(ntv2_kona_reg_interrupt_control2,					266);
NTV2_FLD(ntv2_kona_fld_in3_vertical_enable,					1,	1);
NTV2_FLD(ntv2_kona_fld_in4_vertical_enable,					1,	2);
NTV2_FLD(ntv2_kona_fld_hdmi1_rx_plug_enable,				1,	4);
NTV2_FLD(ntv2_kona_fld_hdmi1_rx_plug_clear,					1,	5);
NTV2_FLD(ntv2_kona_fld_hdmi1_rx_chip_enable,				1,	6);
NTV2_FLD(ntv2_kona_fld_hdmi1_rx_chip_clear,					1,	7);
NTV2_FLD(ntv2_kona_fld_in5_vertical_enable,					1,	8);
NTV2_FLD(ntv2_kona_fld_in6_vertical_enable,					1,	9);
NTV2_FLD(ntv2_kona_fld_in7_vertical_enable,					1,	10);
NTV2_FLD(ntv2_kona_fld_in8_vertical_enable,					1,	11);
NTV2_FLD(ntv2_kona_fld_out5_vertical_enable,				1,	12);
NTV2_FLD(ntv2_kona_fld_out6_vertical_enable,				1,	13);
NTV2_FLD(ntv2_kona_fld_out7_vertical_enable,				1,	14);
NTV2_FLD(ntv2_kona_fld_out8_vertical_enable,				1,	15);
NTV2_FLD(ntv2_kona_fld_out8_vertical_clear,					1,	16);
NTV2_FLD(ntv2_kona_fld_out7_vertical_clear,					1,	17);
NTV2_FLD(ntv2_kona_fld_out6_vertical_clear,					1,	18);
NTV2_FLD(ntv2_kona_fld_out5_vertical_clear,					1,	19);
NTV2_FLD(ntv2_kona_fld_in8_vertical_clear,					1,	25);
NTV2_FLD(ntv2_kona_fld_in7_vertical_clear,					1,	26);
NTV2_FLD(ntv2_kona_fld_in6_vertical_clear,					1,	27);
NTV2_FLD(ntv2_kona_fld_in5_vertical_clear,					1,	28);
NTV2_FLD(ntv2_kona_fld_in4_vertical_clear,					1,	29);
NTV2_FLD(ntv2_kona_fld_in3_vertical_clear,					1,	30);

NTV2_REG(ntv2_kona_reg_interrupt_control3,					201);
NTV2_FLD(ntv2_kona_fld_p2p_msg1_enable,						1,	0);
NTV2_FLD(ntv2_kona_fld_p2p_msg2_enable,						1,	1);
NTV2_FLD(ntv2_kona_fld_p2p_msg3_enable,						1,	2);
NTV2_FLD(ntv2_kona_fld_p2p_msg4_enable,						1,	3);
NTV2_FLD(ntv2_kona_fld_p2p_msg5_enable,						1,	4);
NTV2_FLD(ntv2_kona_fld_p2p_msg6_enable,						1,	5);
NTV2_FLD(ntv2_kona_fld_p2p_msg7_enable,						1,	6);
NTV2_FLD(ntv2_kona_fld_p2p_msg8_enable,						1,	7);
NTV2_FLD(ntv2_kona_fld_p2p_msg1_clear,						1,	16);
NTV2_FLD(ntv2_kona_fld_p2p_msg2_clear,						1,	17);
NTV2_FLD(ntv2_kona_fld_p2p_msg3_clear,						1,	18);
NTV2_FLD(ntv2_kona_fld_p2p_msg4_clear,						1,	19);
NTV2_FLD(ntv2_kona_fld_p2p_msg5_clear,						1,	20);
NTV2_FLD(ntv2_kona_fld_p2p_msg6_clear,						1,	21);
NTV2_FLD(ntv2_kona_fld_p2p_msg7_clear,						1,	22);
NTV2_FLD(ntv2_kona_fld_p2p_msg8_clear,						1,	23);

/* interrupt status registers */
NTV2_REG(ntv2_kona_reg_interrupt_status,					21);
NTV2_FLD(ntv2_kona_fld_out4_vertical_blank,					1,	0);
NTV2_FLD(ntv2_kona_fld_out4_field_id,						1,	1);
NTV2_FLD(ntv2_kona_fld_out3_vertical_blank,					1,	2);
NTV2_FLD(ntv2_kona_fld_out3_field_id,						1,	3);
NTV2_FLD(ntv2_kona_fld_out2_vertical_blank,					1,	4);
NTV2_FLD(ntv2_kona_fld_out2_field_id,						1,	5);
NTV2_FLD(ntv2_kona_fld_out4_vertical_active,				1,	6);
NTV2_FLD(ntv2_kona_fld_out3_vertical_active,				1,	7);
NTV2_FLD(ntv2_kona_fld_out2_vertical_active,				1,	8);
NTV2_FLD(ntv2_kona_fld_uart1_rx_active,						1,	15);
NTV2_FLD(ntv2_kona_fld_in2_vertical_blank,					1,	18);
NTV2_FLD(ntv2_kona_fld_in2_field_id,						1,	19);
NTV2_FLD(ntv2_kona_fld_in1_vertical_blank,					1,	20);
NTV2_FLD(ntv2_kona_fld_in1_field_id,						1,	21);
NTV2_FLD(ntv2_kona_fld_out1_vertical_blank,					1,	22);
NTV2_FLD(ntv2_kona_fld_out1_field_id,						1,	23);
NTV2_FLD(ntv2_kona_fld_uart1_tx_active,						1,	24);
NTV2_FLD(ntv2_kona_fld_in2_vertical_active,					1,	29);
NTV2_FLD(ntv2_kona_fld_in1_vertical_active,					1,	30);
NTV2_FLD(ntv2_kona_fld_out1_vertical_active,				1,	31);

NTV2_REG(ntv2_kona_reg_interrupt_status2,					265);
NTV2_FLD(ntv2_kona_fld_hdmi1_plug_active,					1,	0);
NTV2_FLD(ntv2_kona_fld_hdmi1_chip_active,					1,	1);
NTV2_FLD(ntv2_kona_fld_out8_vertical_blank,					1,	2);
NTV2_FLD(ntv2_kona_fld_out8_field_id,						1,	3);
NTV2_FLD(ntv2_kona_fld_out7_vertical_blank,					1,	4);
NTV2_FLD(ntv2_kona_fld_out7_field_id,						1,	5);
NTV2_FLD(ntv2_kona_fld_out6_vertical_blank,					1,	6);
NTV2_FLD(ntv2_kona_fld_out6_field_id,						1,	7);
NTV2_FLD(ntv2_kona_fld_out5_vertical_blank,					1,	8);
NTV2_FLD(ntv2_kona_fld_out5_field_id,						1,	9);
NTV2_FLD(ntv2_kona_fld_in8_vertical_blank,					1,	10);
NTV2_FLD(ntv2_kona_fld_in8_field_id,						1,	11);
NTV2_FLD(ntv2_kona_fld_in7_vertical_blank,					1,	12);
NTV2_FLD(ntv2_kona_fld_in7_field_id,						1,	13);
NTV2_FLD(ntv2_kona_fld_in6_vertical_blank,					1,	14);
NTV2_FLD(ntv2_kona_fld_in6_field_id,						1,	15);
NTV2_FLD(ntv2_kona_fld_in5_vertical_blank,					1,	16);
NTV2_FLD(ntv2_kona_fld_in5_field_id,						1,	17);
NTV2_FLD(ntv2_kona_fld_in4_vertical_blank,					1,	18);
NTV2_FLD(ntv2_kona_fld_in4_field_id,						1,	19);
NTV2_FLD(ntv2_kona_fld_in3_vertical_blank,					1,	20);
NTV2_FLD(ntv2_kona_fld_in3_field_id,						1,	21);
NTV2_FLD(ntv2_kona_fld_out8_vertical_active,				1,	22);
NTV2_FLD(ntv2_kona_fld_out7_vertical_active,				1,	23);
NTV2_FLD(ntv2_kona_fld_out6_vertical_active,				1,	24);
NTV2_FLD(ntv2_kona_fld_in8_vertical_active,					1,	25);
NTV2_FLD(ntv2_kona_fld_in7_vertical_active,					1,	26);
NTV2_FLD(ntv2_kona_fld_in6_vertical_active,					1,	27);
NTV2_FLD(ntv2_kona_fld_in5_vertical_active,					1,	28);
NTV2_FLD(ntv2_kona_fld_in4_vertical_active,					1,	29);
NTV2_FLD(ntv2_kona_fld_in3_vertical_active,					1,	30);
NTV2_FLD(ntv2_kona_fld_out5_vertical_active,				1,	31);

NTV2_REG(ntv2_kona_reg_interrupt_status3,					200);
NTV2_FLD(ntv2_kona_fld_p2p_msg1_active,						1,	0);
NTV2_FLD(ntv2_kona_fld_p2p_msg2_active,						1,	1);
NTV2_FLD(ntv2_kona_fld_p2p_msg3_active,						1,	2);
NTV2_FLD(ntv2_kona_fld_p2p_msg4_active,						1,	3);
NTV2_FLD(ntv2_kona_fld_p2p_msg5_active,						1,	4);
NTV2_FLD(ntv2_kona_fld_p2p_msg6_active,						1,	5);
NTV2_FLD(ntv2_kona_fld_p2p_msg7_active,						1,	6);
NTV2_FLD(ntv2_kona_fld_p2p_msg8_active,						1,	7);

/* dma control status register */
NTV2_REG(ntv2_kona_reg_dma_control_status,					48);
NTV2_FLD(ntv2_kona_fld_dma1_go_busy,						1,	0);
NTV2_FLD(ntv2_kona_fld_dma2_go_busy,						1,	1);
NTV2_FLD(ntv2_kona_fld_dma3_go_busy,						1,	2);
NTV2_FLD(ntv2_kona_fld_dma4_go_busy,						1,	3);
NTV2_FLD(ntv2_kona_fld_dma_force_64_bit,					1,	4);
NTV2_FLD(ntv2_kona_fld_dma_64_bit_auto_detect,				1,	5);
NTV2_FLD(ntv2_kona_fld_dma_66MHz_detect,					1,	6);
NTV2_FLD(ntv2_kona_fld_dma_strap,							1,	7);
NTV2_FLD(ntv2_kona_fld_fpga_firmware_version,				8,	8);
NTV2_FLD(ntv2_kona_fld_nwl_pcie_lane_count,					4,	16);
NTV2_FLD(ntv2_kona_fld_nwl_pcie_generation,					4,	20);
NTV2_FLD(ntv2_kona_fld_sdram_phy_init_done,					1,	24);
NTV2_FLD(ntv2_kona_fld_atx_power_connected,					1,	25);
NTV2_FLD(ntv2_kona_fld_fpga_video_interrupt_valid,			1,	26);
NTV2_FLD(ntv2_kona_fld_dma1_active,							1,	27);
NTV2_FLD(ntv2_kona_fld_dma2_active,							1,	28);
NTV2_FLD(ntv2_kona_fld_dma3_active,							1,	29);
NTV2_FLD(ntv2_kona_fld_dma4_active,							1,	30);
NTV2_FLD(ntv2_kona_fld_dma_bus_error,						1,	31);

/* dma interrupt control register */
NTV2_REG(ntv2_kona_reg_dma_interrupt_control,				49);
NTV2_FLD(ntv2_kona_fld_dma1_enable,							1,	0);
NTV2_FLD(ntv2_kona_fld_dma2_enable,							1,	1);
NTV2_FLD(ntv2_kona_fld_dma3_enable,							1,	2);
NTV2_FLD(ntv2_kona_fld_dma4_enable,							1,	3);
NTV2_FLD(ntv2_kona_fld_dma_bus_error_enable,				1,	4);
NTV2_FLD(ntv2_kona_fld_dma1_clear,							1,	27);
NTV2_FLD(ntv2_kona_fld_dma2_clear,							1,	28);
NTV2_FLD(ntv2_kona_fld_dma3_clear,							1,	29);
NTV2_FLD(ntv2_kona_fld_dma4_clear,							1,	30);
NTV2_FLD(ntv2_kona_fld_dma_bus_error_clear,					1,	31);

/* global control registers */
NTV2_REG(ntv2_kona_reg_global_control,						0, 377, 378, 379, 380, 381, 382, 383);
NTV2_FLD(ntv2_kona_fld_global_frame_rate_b012,				3,	0);
NTV2_FLD(ntv2_kona_fld_global_frame_geometry,				4,	3);
NTV2_FLD(ntv2_kona_fld_global_video_standard,				3,	7);
NTV2_FLD(ntv2_kona_fld_reference_source_b012,				3,	10);
NTV2_FLD(ntv2_kona_fld_linkb_p60_mode_ch2,					1,	15);
NTV2_FLD(ntv2_kona_fld_global_led,							4,	16);
NTV2_FLD(ntv2_kona_fld_global_reg_sync,						2,	20);
NTV2_FLD(ntv2_kona_fld_global_frame_rate_b3,			   	1,	22);
NTV2_FLD(ntv2_kona_fld_global_quad_tsi_enable,				1,  24);
NTV2_FLD(ntv2_kona_fld_sdiout1_rp188_enable,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiout2_rp188_enable,				1,	29);

/* global control 2 register */
NTV2_REG(ntv2_kona_reg_global_control2,						267);
NTV2_FLD(ntv2_kona_fld_reference_source_b3,					1,	0);
NTV2_FLD(ntv2_kona_fld_fs1234_quad_mode,					1,	3);
NTV2_FLD(ntv2_kona_fld_as1_auto_play,						1,	4);
NTV2_FLD(ntv2_kona_fld_as2_auto_play,						1,	5);
NTV2_FLD(ntv2_kona_fld_as3_auto_play,						1,	6);
NTV2_FLD(ntv2_kona_fld_as4_auto_play,						1,	7);
NTV2_FLD(ntv2_kona_fld_as5_auto_play,						1,	8);
NTV2_FLD(ntv2_kona_fld_as6_auto_play,						1,	9);
NTV2_FLD(ntv2_kona_fld_as7_auto_play,						1,	10);
NTV2_FLD(ntv2_kona_fld_as8_auto_play,						1,	11);
NTV2_FLD(ntv2_kona_fld_fs5678_quad_mode,					1,	12);
NTV2_FLD(ntv2_kona_fld_sdiout4_linkb_p60_mode,				1,	13);
NTV2_FLD(ntv2_kona_fld_sdiout6_linkb_p60_mode,				1,	14);
NTV2_FLD(ntv2_kona_fld_sdiout8_linkb_p60_mode,				1,	15);
NTV2_FLD(ntv2_kona_fld_independent_channel_enable,			1,	16);
NTV2_FLD(ntv2_kona_fld_fb_2mb_supported,					1,	17);
NTV2_FLD(ntv2_kona_fld_has_audio_mixer,						1,	18);
NTV2_FLD(ntv2_kona_fld_fb12_425mode_enable,					1,	20);
NTV2_FLD(ntv2_kona_fld_fb34_425mode_enable,					1,	21);
NTV2_FLD(ntv2_kona_fld_fb56_425mode_enable,					1,	22);
NTV2_FLD(ntv2_kona_fld_fb78_425mode_enable,					1,	23);
NTV2_FLD(ntv2_kona_fld_425mux1234_delay_enable,				1,	24);
NTV2_FLD(ntv2_kona_fld_425mux5678_delay_enable,				1,	25);
NTV2_FLD(ntv2_kona_fld_sdiout7_rp188_enable,				1,	26);
NTV2_FLD(ntv2_kona_fld_sdiout8_rp188_enable,				1,	27);
NTV2_FLD(ntv2_kona_fld_sdiout3_rp188_enable,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiout4_rp188_enable,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiout5_rp188_enable,				1,	30);
NTV2_FLD(ntv2_kona_fld_sdiout6_rp188_enable,				1,	31);

/* frame store control registers */
NTV2_REG(ntv2_kona_reg_frame_control,						1, 5, 257, 260, 384, 388, 392, 396);
NTV2_FLD(ntv2_kona_fld_frame_capture_enable,				1,	0);
NTV2_FLD(ntv2_kona_fld_frame_buffer_format_b0123,			4,	1);
NTV2_FLD(ntv2_kona_fld_frame_alpha_input2,					1,	5);
NTV2_FLD(ntv2_kona_fld_frame_buffer_format_b4,				1,	6);
NTV2_FLD(ntv2_kona_fld_frame_disable,						1,	7);
NTV2_FLD(ntv2_kona_fld_frame_size,							2,	20);
NTV2_FLD(ntv2_kona_fld_frame_8to10b_convert_mode,			1,	23);
NTV2_FLD(ntv2_kona_fld_frame_vblank_rgb_range,				1,	24);
NTV2_FLD(ntv2_kona_fld_frame_size_by_sw,					1,	29);

/* frame store size control */
NTV2_REG(ntv2_kona_reg_frame_2m_control,					498, 499, 500, 501, 502, 503, 504, 505);
NTV2_FLD(ntv2_kona_fld_frame_2m_size,						5,	0);
NTV2_FLD(ntv2_kona_fld_channel_bar,							9,	16);

/* frame store input/output frame buffer registers */
NTV2_REG(ntv2_kona_reg_frame_output,						3, 7, 258, 261, 385, 389, 393, 397);
NTV2_REG(ntv2_kona_reg_frame_input,							4, 8, 259, 262, 386, 390, 394, 398);

/* sdi/ref/aes input status registers */
NTV2_REG(ntv2_kona_reg_input_status,						22);
NTV2_FLD(ntv2_kona_fld_sdiin1_frame_rate_b012,				3,	0);
NTV2_FLD(ntv2_kona_fld_sdiin1_geometry_b012,				3,	4);
NTV2_FLD(ntv2_kona_fld_sdiin1_progressive,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin2_frame_rate_b012,				3,	8);
NTV2_FLD(ntv2_kona_fld_sdiin2_geometry_b012,				3,	12);
NTV2_FLD(ntv2_kona_fld_sdiin2_progressive,					1,	15);
NTV2_FLD(ntv2_kona_fld_refin_frame_rate,					4,	16);
NTV2_FLD(ntv2_kona_fld_refin_frame_lines,					3,	20);
NTV2_FLD(ntv2_kona_fld_refin_progressive,					1,	23);
NTV2_FLD(ntv2_kona_fld_aesin12_invalid,						1,	24);
NTV2_FLD(ntv2_kona_fld_aesin34_invalid,						1,	25);
NTV2_FLD(ntv2_kona_fld_aesin56_invalid,						1,	26);
NTV2_FLD(ntv2_kona_fld_aesin78_invalid,						1,	27);
NTV2_FLD(ntv2_kona_fld_sdiin1_frame_rate_b3,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiin2_frame_rate_b3,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiin1_geometry_b3,					1,	30);
NTV2_FLD(ntv2_kona_fld_sdiin2_geometry_b3,					1,	31);

NTV2_REG(ntv2_kona_reg_input_status2,						288);
NTV2_FLD(ntv2_kona_fld_sdiin3_frame_rate_b012,				3,	0);
NTV2_FLD(ntv2_kona_fld_sdiin3_geometry_b012,				3,	4);
NTV2_FLD(ntv2_kona_fld_sdiin3_progressive,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin4_frame_rate_b012,				3,	8);
NTV2_FLD(ntv2_kona_fld_sdiin4_geometry_b012,				3,	12);
NTV2_FLD(ntv2_kona_fld_sdiin4_progressive,					1,	15);
NTV2_FLD(ntv2_kona_fld_sdiin3_frame_rate_b3,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiin4_frame_rate_b3,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiin3_geometry_b3,					1,	30);
NTV2_FLD(ntv2_kona_fld_sdiin4_geometry_b3,					1,	31);

NTV2_REG(ntv2_kona_reg_input_status3,						458);
NTV2_FLD(ntv2_kona_fld_sdiin5_frame_rate_b012,				3,	0);
NTV2_FLD(ntv2_kona_fld_sdiin5_geometry_b012,				3,	4);
NTV2_FLD(ntv2_kona_fld_sdiin5_progressive,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin6_frame_rate_b012,				3,	8);
NTV2_FLD(ntv2_kona_fld_sdiin6_geometry_b012,				3,	12);
NTV2_FLD(ntv2_kona_fld_sdiin6_progressive,					1,	15);
NTV2_FLD(ntv2_kona_fld_sdiin5_frame_rate_b3,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiin6_frame_rate_b3,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiin5_geometry_b3,					1,	30);
NTV2_FLD(ntv2_kona_fld_sdiin6_geometry_b3,					1,	31);

NTV2_REG(ntv2_kona_reg_input_status4,						459);
NTV2_FLD(ntv2_kona_fld_sdiin7_frame_rate_b012,				3,	0);
NTV2_FLD(ntv2_kona_fld_sdiin7_geometry_b012,				3,	4);
NTV2_FLD(ntv2_kona_fld_sdiin7_progressive,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin8_frame_rate_b012,				3,	8);
NTV2_FLD(ntv2_kona_fld_sdiin8_geometry_b012,				3,	12);
NTV2_FLD(ntv2_kona_fld_sdiin8_progressive,					1,	15);
NTV2_FLD(ntv2_kona_fld_sdiin7_frame_rate_b3,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiin8_frame_rate_b3,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiin7_geometry_b3,					1,	30);
NTV2_FLD(ntv2_kona_fld_sdiin8_geometry_b3,					1,	31);

/* sdi input 3g/vpid status registers */
NTV2_REG(ntv2_kona_reg_input_3g_status,						232);
NTV2_FLD(ntv2_kona_fld_sdiin1_3g_mode,						1,	0);
NTV2_FLD(ntv2_kona_fld_sdiin1_3gb_mode,						1,	1);
NTV2_FLD(ntv2_kona_fld_sdiin1_3g_b2a_convert,				1,	2);
NTV2_FLD(ntv2_kona_fld_sdiin1_vpid_linka_valid,				1,	4);
NTV2_FLD(ntv2_kona_fld_sdiin1_vpid_linkb_valid,				1,	5);
NTV2_FLD(ntv2_kona_fld_sdiin1_6gb_mode,						1,	6);
NTV2_FLD(ntv2_kona_fld_sdiin1_12gb_mode,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin2_3g_mode,						1,	8);
NTV2_FLD(ntv2_kona_fld_sdiin2_3gb_mode,						1,	9);
NTV2_FLD(ntv2_kona_fld_sdiin2_3g_b2a_convert,				1,	10);
NTV2_FLD(ntv2_kona_fld_sdiin2_vpid_linka_valid,				1,	12);
NTV2_FLD(ntv2_kona_fld_sdiin2_vpid_linkb_valid,				1,	13);
NTV2_FLD(ntv2_kona_fld_sdiin2_6gb_mode,						1,	14);
NTV2_FLD(ntv2_kona_fld_sdiin2_12gb_mode,					1,	15);

NTV2_REG(ntv2_kona_reg_input_3g_status2,					287);
NTV2_FLD(ntv2_kona_fld_sdiin3_3g_mode,						1,	0);
NTV2_FLD(ntv2_kona_fld_sdiin3_3gb_mode,						1,	1);
NTV2_FLD(ntv2_kona_fld_sdiin3_3g_b2a_convert,				1,	2);
NTV2_FLD(ntv2_kona_fld_sdiin3_vpid_linka_valid,				1,	4);
NTV2_FLD(ntv2_kona_fld_sdiin3_vpid_linkb_valid,				1,	5);
NTV2_FLD(ntv2_kona_fld_sdiin3_6gb_mode,						1,	6);
NTV2_FLD(ntv2_kona_fld_sdiin3_12gb_mode,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin4_3g_mode,						1,	8);
NTV2_FLD(ntv2_kona_fld_sdiin4_3gb_mode,						1,	9);
NTV2_FLD(ntv2_kona_fld_sdiin4_3g_b2a_convert,				1,	10);
NTV2_FLD(ntv2_kona_fld_sdiin4_vpid_linka_valid,				1,	12);
NTV2_FLD(ntv2_kona_fld_sdiin4_vpid_linkb_valid,				1,	13);
NTV2_FLD(ntv2_kona_fld_sdiin4_6gb_mode,						1,	14);
NTV2_FLD(ntv2_kona_fld_sdiin4_12gb_mode,					1,	15);

NTV2_REG(ntv2_kona_reg_input_3g_status3,					457);
NTV2_FLD(ntv2_kona_fld_sdiin5_3g_mode,						1,	0);
NTV2_FLD(ntv2_kona_fld_sdiin5_3gb_mode,						1,	1);
NTV2_FLD(ntv2_kona_fld_sdiin5_3g_b2a_convert,				1,	2);
NTV2_FLD(ntv2_kona_fld_sdiin5_vpid_linka_valid,				1,	4);
NTV2_FLD(ntv2_kona_fld_sdiin5_vpid_linkb_valid,				1,	5);
NTV2_FLD(ntv2_kona_fld_sdiin5_6gb_mode,						1,	6);
NTV2_FLD(ntv2_kona_fld_sdiin5_12gb_mode,					1,	7);
NTV2_FLD(ntv2_kona_fld_sdiin6_3g_mode,						1,	8);
NTV2_FLD(ntv2_kona_fld_sdiin6_3gb_mode,						1,	9);
NTV2_FLD(ntv2_kona_fld_sdiin6_3g_b2a_convert,				1,	10);
NTV2_FLD(ntv2_kona_fld_sdiin6_vpid_linka_valid,				1,	12);
NTV2_FLD(ntv2_kona_fld_sdiin6_vpid_linkb_valid,				1,	13);
NTV2_FLD(ntv2_kona_fld_sdiin6_6gb_mode,						1,	14);
NTV2_FLD(ntv2_kona_fld_sdiin6_12gb_mode,					1,	15);
NTV2_FLD(ntv2_kona_fld_sdiin7_3g_mode,						1,	16);
NTV2_FLD(ntv2_kona_fld_sdiin7_3gb_mode,						1,	17);
NTV2_FLD(ntv2_kona_fld_sdiin7_3g_b2a_convert,				1,	18);
NTV2_FLD(ntv2_kona_fld_sdiin7_vpid_linka_valid,				1,	20);
NTV2_FLD(ntv2_kona_fld_sdiin7_vpid_linkb_valid,				1,	21);
NTV2_FLD(ntv2_kona_fld_sdiin7_6gb_mode,						1,	22);
NTV2_FLD(ntv2_kona_fld_sdiin7_12gb_mode,					1,	23);
NTV2_FLD(ntv2_kona_fld_sdiin8_3g_mode,						1,	24);
NTV2_FLD(ntv2_kona_fld_sdiin8_3gb_mode,						1,	25);
NTV2_FLD(ntv2_kona_fld_sdiin8_3g_b2a_convert,				1,	26);
NTV2_FLD(ntv2_kona_fld_sdiin8_vpid_linka_valid,				1,	28);
NTV2_FLD(ntv2_kona_fld_sdiin8_vpid_linkb_valid,				1,	29);
NTV2_FLD(ntv2_kona_fld_sdiin8_6gb_mode,						1,	30);
NTV2_FLD(ntv2_kona_fld_sdiin8_12gb_mode,					1,	31);

/* sdi input vpid registers */
NTV2_REG(ntv2_kona_reg_sdiin_vpid_linka,					188, 238, 306, 308, 410, 412, 421, 430);
NTV2_REG(ntv2_kona_reg_sdiin_vpid_linkb,					189, 239, 307, 309, 411, 413, 422, 431);
/* vpid byte 1 defines interface standard */
NTV2_FLD(ntv2_kona_fld_vpid_byte1_standard,					8,	0);
NTV2_CON(ntv2_kona_con_vpid_standard_sd,		  			0x81);
NTV2_CON(ntv2_kona_con_vpid_standard_720_hd,		   		0x84);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_hd,  				0x85);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_hd_dual,	   		0x87);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_3ga,				0x89);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_3gb,				0x8a);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_3gb_x2,			0x8c);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_3ga_dual,			0x94);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_3gb_dual,			0x95);
NTV2_CON(ntv2_kona_con_vpid_standard_2160_3gb_dual,			0x96);
NTV2_CON(ntv2_kona_con_vpid_standard_2160_3ga_quad,			0x97);
NTV2_CON(ntv2_kona_con_vpid_standard_2160_3gb_quad,			0x98);
NTV2_CON(ntv2_kona_con_vpid_standard_2160_6gb,				0xc0);
NTV2_CON(ntv2_kona_con_vpid_standard_1080_6gb_x4,			0xc1);
NTV2_CON(ntv2_kona_con_vpid_standard_2160_12gb,				0xce);
/* vpid byte 2 defines frame rate */
NTV2_FLD(ntv2_kona_fld_vpid_byte2_frame_rate,				4,	8);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_2398,				0x2);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_2400,				0x3);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_4795,				0x4);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_2500,				0x5);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_2997,				0x6);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_3000,				0x7);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_4800,				0x8);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_5000,				0x9);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_5994,				0xa);
NTV2_CON(ntv2_kona_con_vpid_frame_rate_6000,				0xb);
NTV2_FLD(ntv2_kona_fld_vpid_frame_scan_picture,				1,	14);
NTV2_FLD(ntv2_kona_fld_vpid_frame_scan_transport,			1,	15);
NTV2_CON(ntv2_kona_con_vpid_frame_scan_progressive,			0x1);
NTV2_CON(ntv2_kona_con_vpid_frame_scan_interlaced,			0x0);
/* vpid byte 3 pixel sampling structure */
NTV2_FLD(ntv2_kona_fld_vpid_sampling,						4,	16);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuv_422,				0x0);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuv_444,				0x1);
NTV2_CON(ntv2_kona_con_vpid_sampling_gbr_444,				0x2);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuv_420,				0x3);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuva_4224,				0x4);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuva_4444,				0x5);
NTV2_CON(ntv2_kona_con_vpid_sampling_gbra_4444,				0x6);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuvd_4224,				0x8);
NTV2_CON(ntv2_kona_con_vpid_sampling_yuvd_4444,				0x9);
NTV2_CON(ntv2_kona_con_vpid_sampling_gbrd_4444,				0xa);
NTV2_CON(ntv2_kona_con_vpid_sampling_xyz_444,				0xe);
NTV2_FLD(ntv2_kona_fld_vpid_colorimetry,					2,	21);
NTV2_CON(ntv2_kona_con_vpid_colorimetry_rec709,				0x0);
NTV2_CON(ntv2_kona_con_vpid_colorimetry_vanc,				0x1);
NTV2_CON(ntv2_kona_con_vpid_colorimetry_rec2020,			0x2);
NTV2_FLD(ntv2_kona_fld_vpid_aspect_ratio_b5,				1,	21);
NTV2_CON(ntv2_kona_con_vpid_aspect_ratio_4x3,				0x0);
NTV2_CON(ntv2_kona_con_vpid_aspect_ratio_16x9,				0x1);
NTV2_FLD(ntv2_kona_fld_vpid_horizontal_pixels,				1,	22);
NTV2_CON(ntv2_kona_con_vpid_horizontal_1920,				0x0);
NTV2_CON(ntv2_kona_con_vpid_horizontal_2048,				0x1);
NTV2_FLD(ntv2_kona_fld_vpid_aspect_ratio_b7,				1,	23);
/* vpid byte 4 video bit depth and channel */
NTV2_FLD(ntv2_kona_fld_vpid_bit_depth,						2,	24);
NTV2_CON(ntv2_kona_con_vpid_bit_depth_8,					0x0);
NTV2_CON(ntv2_kona_con_vpid_bit_depth_10,					0x1);
NTV2_CON(ntv2_kona_con_vpid_bit_depth_12,					0x2);
NTV2_FLD(ntv2_kona_fld_vpid_audio_status,					1,	26);
NTV2_CON(ntv2_kona_con_vpid_audio_status_additional,		0x0);
NTV2_CON(ntv2_kona_con_vpid_audio_status_copy,				0x1);
NTV2_FLD(ntv2_kona_fld_vpid_dynamic_range,					2,	27);
NTV2_FLD(ntv2_kona_fld_vpid_channel_octa,					3,	29);
NTV2_FLD(ntv2_kona_fld_vpid_channel_quad,					2,	30);
NTV2_FLD(ntv2_kona_fld_vpid_channel_dual,					1,	30);
NTV2_CON(ntv2_kona_con_vpid_channel_1,						0x0);
NTV2_CON(ntv2_kona_con_vpid_channel_2,						0x1);
NTV2_CON(ntv2_kona_con_vpid_channel_3,						0x2);
NTV2_CON(ntv2_kona_con_vpid_channel_4,						0x3);
NTV2_CON(ntv2_kona_con_vpid_channel_5,						0x4);
NTV2_CON(ntv2_kona_con_vpid_channel_6,						0x5);
NTV2_CON(ntv2_kona_con_vpid_channel_7,						0x6);
NTV2_CON(ntv2_kona_con_vpid_channel_8,						0x7);

/* sdi input timecode registers */
NTV2_REG(ntv2_kona_reg_sdiin_timecode_rp188_dbb,			29, 64, 268, 273, 342, 418, 427, 436);
NTV2_FLD(ntv2_kona_fld_sdiin_rp188_dbb,						8,	0);
NTV2_FLD(ntv2_kona_fld_sdiin_rp188_select_present,			1,	17);
NTV2_FLD(ntv2_kona_fld_sdiin_rp188_ltc_present,				1,	18);
NTV2_FLD(ntv2_kona_fld_sdiin_rp188_vitc1_present,			1,	19);
NTV2_FLD(ntv2_kona_fld_sdiin_rp188_select,					8,	24);

NTV2_REG(ntv2_kona_reg_sdiin_timecode_rp188_low,			30, 65, 269, 274, 340, 416, 425, 434);
NTV2_REG(ntv2_kona_reg_sdiin_timecode_rp188_high,			31, 66, 270, 275, 341, 417, 426, 435);

NTV2_REG(ntv2_kona_reg_sdiin_timecode_ltc_low,				110, 252, 316, 318, 344, 419, 428, 437);
NTV2_REG(ntv2_kona_reg_sdiin_timecode_ltc_high,				111, 253, 317, 319, 345, 420, 429, 438);

NTV2_REG(ntv2_kona_reg_sdiin_timecode_vitc1_low,			202, 204, 206, 208, 210, 212, 214, 216);
NTV2_REG(ntv2_kona_reg_sdiin_timecode_vitc1_high,			203, 205, 207, 209, 211, 213, 215, 217);

/* audio detection bits */
NTV2_FLD(ntv2_kona_fld_audio_detect_gr1ch12,				1,	0);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr1ch34,				1,	1);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr2ch12,				1,	2);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr2ch34,				1,	3);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr3ch12,				1,	4);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr3ch34,				1,	5);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr4ch12,				1,	6);
NTV2_FLD(ntv2_kona_fld_audio_detect_gr4ch34,				1,	7);

/* sdi input embedded audio detect registers */
NTV2_REG(ntv2_kona_reg_sdiin_audio_detect,					23);
NTV2_FLD(ntv2_kona_fld_sdiin1_audio_detect,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiin2_audio_detect,					8,	8);

NTV2_REG(ntv2_kona_reg_sdiin_audio_detect2,					282);
NTV2_FLD(ntv2_kona_fld_sdiin3_audio_detect,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiin4_audio_detect,					8,	8);

NTV2_REG(ntv2_kona_reg_sdiin_audio_detect3,					456);
NTV2_FLD(ntv2_kona_fld_sdiin5_audio_detect,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiin6_audio_detect,					8,	8);
NTV2_FLD(ntv2_kona_fld_sdiin7_audio_detect,					8,	16);
NTV2_FLD(ntv2_kona_fld_sdiin8_audio_detect,					8,	24);

/* sdi output control registers */
NTV2_REG(ntv2_kona_reg_sdiout_control,						129, 130, 169, 170, 337, 475, 476, 477);
NTV2_FLD(ntv2_kona_fld_sdiout_video_standard,				3,	0);
NTV2_FLD(ntv2_kona_fld_sdiout_2Kx1080_mode,					1,	3);
NTV2_FLD(ntv2_kona_fld_sdiout_mux_control,					2,	4);
NTV2_FLD(ntv2_kona_fld_sdiout_hblank_rgb_range,				1,	7);
NTV2_FLD(ntv2_kona_fld_sdiout_timing_adjust,				8,	8);
NTV2_FLD(ntv2_kona_fld_sdiout_ds1_audio_source_b2,			1,	18);
NTV2_FLD(ntv2_kona_fld_sdiout_ds2_audio_source_b2,			1,	19);
NTV2_FLD(ntv2_kona_fld_sdiout_passthrough,					1,	20);
NTV2_FLD(ntv2_kona_fld_sdiout_rgb_level_a,					1,	22);
NTV2_FLD(ntv2_kona_fld_sdiout_level_a2b,					1,	23);
NTV2_FLD(ntv2_kona_fld_sdiout_3g_mode,						1,	24);
NTV2_FLD(ntv2_kona_fld_sdiout_3gb_mode,						1,	25);
NTV2_FLD(ntv2_kona_fld_sdiout_vpid_insert,					1,	26);
NTV2_FLD(ntv2_kona_fld_sdiout_vpid_overwrite,				1,	27);
NTV2_FLD(ntv2_kona_fld_sdiout_ds1_audio_source_b1,			1,	28);
NTV2_FLD(ntv2_kona_fld_sdiout_ds2_audio_source_b1,			1,	29);
NTV2_FLD(ntv2_kona_fld_sdiout_ds1_audio_source_b0,			1,	30);
NTV2_FLD(ntv2_kona_fld_sdiout_ds2_audio_source_b0,			1,	31);

/* sdi output vpid registers */
NTV2_REG(ntv2_kona_reg_sdiout_vpid_linka,					234, 236, 271, 276, 338, 414, 423, 432);
NTV2_REG(ntv2_kona_reg_sdiout_vpid_linkb,					235, 237, 272, 277, 339, 415, 424, 433);

/* sdi transmit control register */
NTV2_REG(ntv2_kona_reg_sdi_transmit_control,				256);
NTV2_FLD(ntv2_kona_fld_sdi5_transmit,						1,	24);
NTV2_FLD(ntv2_kona_fld_sdi6_transmit,						1,	25);
NTV2_FLD(ntv2_kona_fld_sdi7_transmit,						1,	26);
NTV2_FLD(ntv2_kona_fld_sdi8_transmit,						1,	27);
NTV2_FLD(ntv2_kona_fld_sdi1_transmit,						1,	28);
NTV2_FLD(ntv2_kona_fld_sdi2_transmit,						1,	29);
NTV2_FLD(ntv2_kona_fld_sdi3_transmit,						1,	30);
NTV2_FLD(ntv2_kona_fld_sdi4_transmit,						1,	31);

/* hdmi output configuration */
NTV2_REG(ntv2_kona_reg_hdmiout_output_config,				125, 0x2c12, 0x3012);
NTV2_FLD(ntv2_kona_fld_hdmiout_video_standard,				4,	0);		/* video standard */
NTV2_FLD(ntv2_kona_fld_hdmiout_audio_group_select,			1,	5);		/* audio upper group select */
NTV2_FLD(ntv2_kona_fld_hdmiout_rgb,							1,	8);		/* rgb color space (not yuv) */
NTV2_FLD(ntv2_kona_fld_hdmiout_frame_rate,					4,	9);		/* frame rate */
NTV2_FLD(ntv2_kona_fld_hdmiout_deep_color,					1,	14);	/* 10 bit deep color (not 8 bit) */
NTV2_FLD(ntv2_kona_fld_hdmiout_yuv_444,						1,	15);	/* yuv 444 mode */
NTV2_FLD(ntv2_kona_fld_hdmiout_full_range,					1,	28);	/* full range rgb (not smpte) */
NTV2_FLD(ntv2_kona_fld_hdmiout_audio_8ch,					1,	29);	/* 8 audio channels (not 2) */
NTV2_FLD(ntv2_kona_fld_hdmiout_dvi,							1,	30);	/* dvi mode (vs hdmi) */

/* hdmi input status */
NTV2_REG(ntv2_kona_reg_hdmiin_input_status,					126, 0x2c13, 0x3013);
NTV2_FLD(ntv2_kona_fld_hdmiin_locked,						1,	0);		
NTV2_FLD(ntv2_kona_fld_hdmiin_stable,						1,	1);		
NTV2_FLD(ntv2_kona_fld_hdmiin_rgb,							1,	2);		
NTV2_FLD(ntv2_kona_fld_hdmiin_deep_color,					1,	3);		
NTV2_FLD(ntv2_kona_fld_hdmiin_video_code,					6,	4);		/* ntv2 video standard v2 */
NTV2_FLD(ntv2_kona_fld_hdmiin_audio_8ch,					1,	12);	/* 8 audio channels (vs 2) */
NTV2_FLD(ntv2_kona_fld_hdmiin_progressive,					1,	13);	
NTV2_FLD(ntv2_kona_fld_hdmiin_video_sd,						1,	14);	/* video pixel clock sd (not hd or 3g) */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_74_25,					1,	15);	/* not used */
NTV2_FLD(ntv2_kona_fld_hdmiin_audio_rate,					4,	16);	
NTV2_FLD(ntv2_kona_fld_hdmiin_audio_word_length,			4,	20);	
NTV2_FLD(ntv2_kona_fld_hdmiin_video_format,					3,	24);	/* really ntv2 standard */
NTV2_FLD(ntv2_kona_fld_hdmiin_dvi,							1,	27);	/* input dvi (vs hdmi) */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_rate,					4,	28);	/* ntv2 video rate */

/* hdmi control */
NTV2_REG(ntv2_kona_reg_hdmi_control,						127, 0x2c14, 0x3014);
NTV2_FLD(ntv2_kona_fld_hdmiout_force_config,				1,	1);		/* force output config (ignore edid) */	
NTV2_FLD(ntv2_kona_fld_hdmiin_chan34_swap_disable,			1,	5);		/* hdmi input audio channel 3/4 swap disable */	
NTV2_FLD(ntv2_kona_fld_hdmiout_chan34_swap_disable,			1,	6);		/* hdmi output audio channel 3/4 swap disable */	
NTV2_FLD(ntv2_kona_fld_hdmiout_fallback_enable,				1,	7);		/* enable 4k/uhd to 2k/hd fallback */	
NTV2_FLD(ntv2_kona_fld_hdmiin_color_depth,					2,	12);	/* hdmi input bit depth */	
NTV2_FLD(ntv2_kona_fld_hdmiin_color_space,					2,	14);	/* hdmi input color space */
NTV2_FLD(ntv2_kona_fld_hdmiout_source_select,				4,	20);	/* output audio source select */	
NTV2_FLD(ntv2_kona_fld_hdmiout_crop_enable,					1,	24);	/* crop 2k -> hd  4k -> uhd */
NTV2_FLD(ntv2_kona_fld_hdmiout_channel_select,				2,	29);	/* output audio channel select */	

/* hdmi rasterizer mode */
NTV2_CON(ntv2_kona_hdmi_raster_mode_hdsd_bidirect,			0x0);
NTV2_CON(ntv2_kona_hdmi_raster_mode_4k_input,				0x1);
NTV2_CON(ntv2_kona_hdmi_raster_mode_4k_output,				0x2);
NTV2_CON(ntv2_kona_hdmi_raster_mode_disable,				0x3);

/* hdmi rasterizer control register */
NTV2_REG(ntv2_kona_reg_hdmi_rasterizer_control,				358);
NTV2_FLD(ntv2_kona_fld_hdmi_raster_mode,					2,	0);		
NTV2_FLD(ntv2_kona_fld_hdmi_tsi_io,							1,	2);		
NTV2_FLD(ntv2_kona_fld_hdmi_raster_level_b,					1,	4);		
NTV2_FLD(ntv2_kona_fld_hdmi_raster_decimate,				1,	8);		

/* hdmi input video mode */
NTV2_CON(ntv2_kona_hdmiin_video_mode_hdsdi,					0x0);		/* hd-sdi */
NTV2_CON(ntv2_kona_hdmiin_video_mode_sdsdi,					0x1);		/* sd_sdi */
NTV2_CON(ntv2_kona_hdmiin_video_mode_3gsdi,					0x2);		/* 3g-sdi */

/* hdmi input video map */
NTV2_CON(ntv2_kona_hdmiin_video_map_422_10bit,				0x0);		/* yuv 422 10 bit */
NTV2_CON(ntv2_kona_hdmiin_video_map_444_10bit,				0x1);		/* yuv/rgb 444 10 bit */

/* hdmi input video standard */
NTV2_CON(ntv2_kona_hdmiin_video_standard_1080i,				0x0);		/* 1080i */
NTV2_CON(ntv2_kona_hdmiin_video_standard_720p,				0x1);		/* 720p */
NTV2_CON(ntv2_kona_hdmiin_video_standard_525i,				0x2);		/* 525i */
NTV2_CON(ntv2_kona_hdmiin_video_standard_625i,				0x3);		/* 625i */
NTV2_CON(ntv2_kona_hdmiin_video_standard_1080p,				0x4);		/* 1080p */
NTV2_CON(ntv2_kona_hdmiin_video_standard_4k,				0x5);		/* 4K */
NTV2_CON(ntv2_kona_hdmiin_video_standard_2205p,				0x6);		/* 3D frame packed mode */
NTV2_CON(ntv2_kona_hdmiin_video_standard_none,				0x7);		/* undefined */

/* hdmi input frame rate */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_none,					0x0);		/* undefined */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_6000,					0x1);		/* 60.00 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_5994,					0x2);		/* 59.94 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_3000,					0x3);		/* 30.00 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_2997,					0x4);		/* 29.97 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_2500,					0x5);		/* 25.00 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_2400,					0x6);		/* 24.00 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_2398,					0x7);		/* 23.98 */
NTV2_CON(ntv2_kona_hdmiin_frame_rate_5000,					0x8);		/* 50.00 */

/* hdmi 3d structure */
NTV2_CON(ntv2_kona_hdmiin_3d_frame_packing,			    	0x0);		/* 0000 frame packing */
NTV2_CON(ntv2_kona_hdmiin_3d_field_alternative,				0x1);		/* 0001 field alternative */
NTV2_CON(ntv2_kona_hdmiin_3d_line_alternative,				0x2);		/* 0010 line alternative */
NTV2_CON(ntv2_kona_hdmiin_3d_side_by_side_full,				0x3);		/* 0011 side by side full */
NTV2_CON(ntv2_kona_hdmiin_3d_l_depth,						0x4);		/* 0100 L + depth */
NTV2_CON(ntv2_kona_hdmiin_3d_l_d_g,							0x5);		/* 0101 L + depth + graphics -depth */
NTV2_CON(ntv2_kona_hdmiin_3d_top_bottom,					0x6);		/* 0110 top bottom */
NTV2_CON(ntv2_kona_hdmiin_3d_side_by_side_half,				0x8);		/* 1000 side by side half */

/* hdmi input i2c control register */
NTV2_REG(ntv2_kona_reg_hdmiin_i2c_control,					360, 0x2c00, 0x3000);
NTV2_FLD(ntv2_kona_fld_hdmiin_subaddress,					8,	0);		/* i2c subaddress (8-bit register on device) */
NTV2_FLD(ntv2_kona_fld_hdmiin_device_address,				7,	8);		/* i2c device address (hdmiin_addr) */
NTV2_FLD(ntv2_kona_fld_hdmiin_read_disable,					1,	16);	/* i2c read disable bit */
NTV2_FLD(ntv2_kona_fld_hdmiin_write_busy,					1,	20);	/* i2c write busy bit */
NTV2_FLD(ntv2_kona_fld_hdmiin_i2c_error,					1,	21);	/* i2c error bit */
NTV2_FLD(ntv2_kona_fld_hdmiin_i2c_busy,						1,	22);	/* i2c busy bit */
NTV2_FLD(ntv2_kona_fld_hdmiin_i2c_reset,					1,	24);	/* i2c reset bit */
NTV2_FLD(ntv2_kona_fld_hdmiin_ram_data_ready,				1,			28);	/* i2c ram data ready bit */

/* hdmi input data register */
NTV2_REG(ntv2_kona_reg_hdmiin_i2c_data,						361, 0x2c01, 0x3001);
NTV2_FLD(ntv2_kona_fld_hdmiin_data_out,						8,	0);		/* i2c data to write to selected subaddress */
NTV2_FLD(ntv2_kona_fld_hdmiin_data_in,						8,	8);		/* i2c data read from selected subaddress */

/* hdmi input video setup regiser */
NTV2_REG(ntv2_kona_reg_hdmiin_video_setup,					362, 0x2c02, 0x3002);
NTV2_FLD(ntv2_kona_fld_hdmiin_video_mode,					2,	0);		/* video mode (hdmiin_video_mode) */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_map,					2,	2);		/* video map (hdmiin_video_map) */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_420,					1,	4);		/* 420 video input */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_standard,				3,	8);		/* video standard select (hdmiin_video_standard) */
NTV2_FLD(ntv2_kona_fld_hdmiin_frame_rate,					4,	16);	/* frame rate select (Hz) (hdmiin_frame_rate) */
NTV2_FLD(ntv2_kona_fld_hdmiin_3d_structure,					4,	20);	/* 3D frame structure (hdmi_3d) */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_4k,						1,	28);	/* 4K video input */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_progressive,			1,	29);	/* progressive video input */
NTV2_FLD(ntv2_kona_fld_hdmiin_video_3d,						1,	30);	/* 3D video input */
NTV2_FLD(ntv2_kona_fld_hdmiin_3d_frame_pack_enable,			1,	31);	/* enable special 3D frame-packed mode */

/* hdmi input horizontal sync and back porch regiser */
NTV2_REG(ntv2_kona_reg_hdmiin_hsync_duration,				363, 0x2c03, 0x3003);

/* hdmi input horizontal active regiser */
NTV2_REG(ntv2_kona_reg_hdmiin_h_active,						364, 0x2c04, 0x3004);

/* hdmi input vertical sync and back porch regiser, field 1 */
NTV2_REG(ntv2_kona_reg_hdmiin_vsync_duration_fld1,			365, 0x2c05, 0x3005);

/* hdmi input vertical sync and back porch regiser, field 2 */
NTV2_REG(ntv2_kona_reg_hdmiin_vsync_duration_fld2,			366, 0x2c06, 0x3006);

/* hdmi input vertical active regiser, field 1 */
NTV2_REG(ntv2_kona_reg_hdmiin_v_active_fld1,				367, 0x2c07, 0x3007);

/* hdmi input vertical active regiser, field 2 */
NTV2_REG(ntv2_kona_reg_hdmiin_v_active_fld2,				368, 0x2c08, 0x3008);

/* hdmi input video status regiser 1 */
NTV2_REG(ntv2_kona_reg_hdmiin_video_status,					369, 0x2c09, 0x3009);
NTV2_FLD(ntv2_kona_fld_hdmiin_det_frame_rate,				4,	0);		/* detected frame rate (hdmiin_frame_rate) */
NTV2_FLD(ntv2_kona_fld_hdmiin_det_video_standard,			3,	8);		/* detected video standard (hdmiin_video_standard) */
NTV2_FLD(ntv2_kona_fld_hdmiin_ident_valid,					1,	16);	/* identification valid */
NTV2_FLD(ntv2_kona_fld_hdmiin_hv_locked,					1,	17);	/* HV Locked */
NTV2_FLD(ntv2_kona_fld_hdmiin_hd_74mhz,						1,	18);	/* HD 74.xx vs 27 Mhz clock */
NTV2_FLD(ntv2_kona_fld_hdmiin_det_progressive,				1,	19);	/* detected progressive */

/* hdmi input H pixel data */
NTV2_REG(ntv2_kona_reg_hdmiin_horizontal_data,				370, 0x2c0a, 0x300a);
NTV2_FLD(ntv2_kona_fld_hdmiin_h_total_pixels,				16,	0);		/* H total pixels per line */
NTV2_FLD(ntv2_kona_fld_hdmiin_h_active_pixels,				16,	16);	/* H active pixels per line */

/* hdmi input H blanking data */
NTV2_REG(ntv2_kona_reg_hdmiin_hblank_data0,					371, 0x2c0b, 0x300b);
NTV2_FLD(ntv2_kona_fld_hdmiin_h_front_porch_pixels,			16,	0);		/* H front porch pixels */
NTV2_FLD(ntv2_kona_fld_hdmiin_h_back_porch_pixels,			16,	16);	/* H back porch pixels */

/* hdmi input H Blanking data */
NTV2_REG(ntv2_kona_reg_hdmiin_hblank_data1,					372, 0x2c0c, 0x300c);
NTV2_FLD(ntv2_kona_fld_hdmiin_hsync_pixels,					16,	0);		/* H sync pixels */
NTV2_FLD(ntv2_kona_fld_hdmiin_hblank_pixels,				16,	16);	/* H blank pixels */

/* hdmi input field 1 V data */
NTV2_REG(ntv2_kona_reg_hdmiin_vertical_data_fld1,			373, 0x2c0d, 0x300d);

/* hdmi input field 2 V data */
NTV2_REG(ntv2_kona_reg_hdmiin_vertical_data_fld2,			374, 0x2c0e, 0x300e);
NTV2_FLD(ntv2_kona_fld_hdmiin_v_total_lines,				16,	0);		/* V total lines field 1,2 */
NTV2_FLD(ntv2_kona_fld_hdmiin_v_active_lines,				16,	16);	/* V active lines field 1,2 */

/* hdmi input color depth */
NTV2_REG(ntv2_kona_reg_hdmiin_color_depth,					375, 0x2c0f, 0x300f);
NTV2_FLD(ntv2_kona_fld_hdmiin_deep_color_detect,			1,	6);		/* detected deep color */

/* hdmi4 output configuration */
NTV2_REG(ntv2_kona_reg_hdmiout4_output_config,				125, 0x1d14, 0x2514);
NTV2_FLD(ntv2_kona_fld_hdmiout4_video_standard,				4,	0);		/* video standard */
NTV2_FLD(ntv2_kona_fld_hdmiout4_audio_group_select,			1,	5);		/* audio upper group select */
NTV2_FLD(ntv2_kona_fld_hdmiout4_rgb,						1,	8);		/* rgb color space (not yuv) */
NTV2_FLD(ntv2_kona_fld_hdmiout4_frame_rate,					4,	9);		/* frame rate */
NTV2_FLD(ntv2_kona_fld_hdmiout4_deep_color,					1,	14);	/* 10 bit deep color (not 8 bit) */
NTV2_FLD(ntv2_kona_fld_hdmiout4_yuv_444,					1,	15);	/* yuv 444 mode */
NTV2_FLD(ntv2_kona_fld_hdmiout4_full_range,					1,	28);	/* full range rgb (not smpte) */
NTV2_FLD(ntv2_kona_fld_hdmiout4_audio_8ch,					1,	29);	/* 8 audio channels (not 2) */
NTV2_FLD(ntv2_kona_fld_hdmiout4_dvi,						1,	30);	/* dvi mode (vs hdmi) */

/* hdmi4 input status */
NTV2_REG(ntv2_kona_reg_hdmiin4_input_status,				126, 0x1d15, 0x2515);
NTV2_FLD(ntv2_kona_fld_hdmiin4_locked,						1,	0);		
NTV2_FLD(ntv2_kona_fld_hdmiin4_stable,						1,	1);		
NTV2_FLD(ntv2_kona_fld_hdmiin4_rgb,							1,	2);		
NTV2_FLD(ntv2_kona_fld_hdmiin4_deep_color,					1,	3);		
NTV2_FLD(ntv2_kona_fld_hdmiin4_video_code,					6,	4);		/* ntv2 video standard v2 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_audio_8ch,					1,	12);	/* 8 audio channels (vs 2) */
NTV2_FLD(ntv2_kona_fld_hdmiin4_progressive,					1,	13);	
NTV2_FLD(ntv2_kona_fld_hdmiin4_video_sd,					1,	14);	/* video pixel clock sd (not hd or 3g) */
NTV2_FLD(ntv2_kona_fld_hdmiin4_video_74_25,					1,	15);	/* not used */
NTV2_FLD(ntv2_kona_fld_hdmiin4_audio_rate,					4,	16);	
NTV2_FLD(ntv2_kona_fld_hdmiin4_audio_word_length,			4,	20);	
NTV2_FLD(ntv2_kona_fld_hdmiin4_video_format,				3,	24);	/* really ntv2 standard */
NTV2_FLD(ntv2_kona_fld_hdmiin4_dvi,							1,	27);	/* input dvi (vs hdmi) */
NTV2_FLD(ntv2_kona_fld_hdmiin4_video_rate,					4,	28);	/* ntv2 video rate */

/* hdmi4 control */
NTV2_REG(ntv2_kona_reg_hdmi4_control,						127, 0x1d16, 0x2516);
NTV2_FLD(ntv2_kona_fld_hdmiout4_force_config,				1,	1);		/* force output config (ignore edid) */	
NTV2_FLD(ntv2_kona_fld_hdmiin4_chn34_swap_disable,			1,	5);		/* hdmi input audio channel 3/4 swap disable */	
NTV2_FLD(ntv2_kona_fld_hdmiout4_chn34_swap_disable,			1,	6);		/* hdmi output audio channel 3/4 swap disable */	
NTV2_FLD(ntv2_kona_fld_hdmiout4_fallback_enable,			1,	7);		/* enable 4k/uhd to 2k/hd fallback */	
NTV2_FLD(ntv2_kona_fld_hdmiin4_color_depth,					2,	12);	/* hdmi input bit depth */	
NTV2_FLD(ntv2_kona_fld_hdmiin4_color_space,					2,	14);	/* hdmi input color space */
NTV2_FLD(ntv2_kona_fld_hdmiout4_source_select,				4,	20);	/* output audio source select */	
NTV2_FLD(ntv2_kona_fld_hdmiout4_crop_enable,				1,	24);	/* crop 2k -> hd  4k -> uhd */
NTV2_FLD(ntv2_kona_fld_hdmiout4_channel_select,				2,	29);	/* output audio channel select */	

/* hdmi4 input video control */
NTV2_REG(ntv2_kona_reg_hdmiin4_videocontrol,				0x1d00, 0x1d00, 0x2500);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_scrambledetect,	1,	 0);	/* scdc 2.0 scramble detect */
NTV2_CON(ntv2_kona_con_hdmiin4_scrambledetect_false,		0x0);		/* scramble not detected */
NTV2_CON(ntv2_kona_con_hdmiin4_scrambledetect_true,			0x1);		/* scramble detected */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_descramblemode,	1,	 1);	/* scdc 2.0 descamble mode */
NTV2_CON(ntv2_kona_con_hdmiin4_descramblemode_disable,		0x0);		/* descramble disable */
NTV2_CON(ntv2_kona_con_hdmiin4_descramblemode_enable,		0x1);		/* descramble enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_scdcratedetect,	1,	 2);	/* scdc hdmi receive > 3.4 gbps */
NTV2_CON(ntv2_kona_con_hdmiin4_scdcratedetect_low,			0x0);		/* scdc hdmi receive rate < 3.4 gbps */
NTV2_CON(ntv2_kona_con_hdmiin4_scdcratedetect_high,			0x1);		/* scdc hdmi receive rate > 3.4 gbps */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_scdcratemode,	1,	 3);	/* scdc hdmi mode > 3.4 gbps */
NTV2_CON(ntv2_kona_con_hdmiin4_scdcratemode_low,			0x0);		/* scdc hdmi mode rate < 3.4 gbps */
NTV2_CON(ntv2_kona_con_hdmiin4_scdcratemode_high,			0x1);		/* scdc hdmi mode rate > 3.4 gbps */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_420mode,		1,	5);		/* 420 mode */
NTV2_CON(ntv2_kona_con_hdmiin4_420mode_disable,				0x0);		/* 420 disable */
NTV2_CON(ntv2_kona_con_hdmiin4_420mode_enable,				0x1);		/* 420 enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_pixelsperclock,	3,	 8);	/* pixels per clock */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_hsyncdivide,	1,	 12);	/* hsync divide mode */
NTV2_CON(ntv2_kona_con_hdmiin4_hsyncdivide_none,			0x0);		/* no hsync divide */
NTV2_CON(ntv2_kona_con_hdmiin4_hsyncdivide_2,				0x1);		/* divide hsync by 2 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_audioswapmode,	1,	 13);	/* audio channel 34 swap */
NTV2_CON(ntv2_kona_con_hdmiin4_audioswapmode_enable,		0x0);		/* swap */
NTV2_CON(ntv2_kona_con_hdmiin4_audioswapmode_disable,		0x1);		/* no swap */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_linerate,		5,	 16);	/* line rate */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_none,				0x0);		/* undetected */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_5940mhz,			0x1);		/* 5940 mhz  8 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_2970mhz,			0x2);		/* 2970 mhz  8 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_1485mhz,			0x3);		/* 1485 mhz  8 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_742mhz,				0x4);		/*  742 mhz  8 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_270mhz,				0x5);		/*  270 mhz  8 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_3712mhz,			0x6);		/* 3712 mhz 10 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_1856mhz,			0x7);		/* 1856 mhz 10 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_928mhz,				0x8);		/*  928 mhz 10 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_337mhz,				0x9);		/*  337 mhz 10 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_4455mhz,			0xa);		/* 4455 mhz 12 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_2227mhz,			0xb);		/* 2227 mhz 12 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_1113mhz,			0xc);		/* 1113 mhz 12 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_405mhz,				0xd);		/*  405 mhz 12 bit */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_556mhz,				0xe);		/*  556 mhz */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_540mhz,				0xf);		/*  540 mhz */
NTV2_CON(ntv2_kona_con_hdmiin4_linerate_250mhz,				0x10);		/*  250 mhz */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_deseriallock,	3,	 24);	/* deserializers lock state */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_inputlock,		1,	 27);	/* input lock state */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_hdmi5vdetect,	1,	 28);	/* hdmi detect state */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode,	1,	 29);	/* hot plug mode */
NTV2_CON(ntv2_kona_con_hdmiin4_hotplugmode_disable,			0x0);		/* disable edid */
NTV2_CON(ntv2_kona_con_hdmiin4_hotplugmode_enable,			0x1);		/* enable edid */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_resetdone,		1,	 30);	/* rx reset done */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videocontrol_reset,	   		1,	 31);	/* rx reset */

/* hdmi4 input video detect 0 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect0,				0x1d01, 0x1d01, 0x2501);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect0_colordepth,		2,	 0);	/* color depth */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect0_colorspace,		2,	 2);	/* color space */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect0_scanmode,		1,	 4);	/* video scan mode */
NTV2_CON(ntv2_kona_con_hdmiin4_scanmode_interlaced,			0x0);		/* interlaced */
NTV2_CON(ntv2_kona_con_hdmiin4_scanmode_progressive,		0x1);		/* progressive */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect0_interfacemode,	1,	 5);	/* interface mode */
NTV2_CON(ntv2_kona_con_hdmiin4_interfacemode_hdmi,			0x0);		/* hdmi */
NTV2_CON(ntv2_kona_con_hdmiin4_interfacemode_dvi,			0x1);		/* dvi */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect0_syncpolarity,	1,	 6);	/* sync polarity */
NTV2_CON(ntv2_kona_con_hdmiin4_syncpolarity_activelow,		0x0);		/* active low */
NTV2_CON(ntv2_kona_con_hdmiin4_syncpolarity_activehigh,		0x1);		/* active high */

/* hdmi4 input video detect 1 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect1,				0x1d02, 0x1d02, 0x2502);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect1_hsyncstart,		16,	 0);	/* horizontal sync start */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect1_hsyncend,		16,	 16);	/* horizontal sync end */

/* hdmi4 input video detect 2 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect2,				0x1d03, 0x1d03, 0x2503);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect2_hdestart,		16,	 0);	/* horizontal de start */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect2_htotal,			16,	 16);	/* horizontal total */

/* hdmi4 input video detect 3 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect3,				0x1d04, 0x1d04, 0x2504);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect3_vtransf1,		16,	 0);	/* vertical transistion field 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect3_vtransf2,		16,	 16);	/* vertical transistion field 2 */

/* hdmi4 input video detect 4 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect4,				0x1d05, 0x1d05, 0x2505);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect4_vsyncstartf1,	16,	 0);	/* vertical sync start field 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect4_vsyncendf1,		16,	 16);	/* virtical sync end field 1 */

/* hdmi4 input video detect 5 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect5,				0x1d06, 0x1d06, 0x2506);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect5_vdestartf1,		16,	 0);	/* vertical de start field 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect5_vdestartf2,		16,	 16);	/* vertical de start field 2 */

/* hdmi4 input video detect 6 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect6,				0x1d07, 0x1d07, 0x2507);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect6_vsyncstartf2,	16,	 0);	/* vertical sync start field 2 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect6_vsyncendf2,		16,	 16);	/* virtical sync end field 2 */

/* hdmi4 input video detect 7 register */
NTV2_REG(ntv2_kona_reg_hdmiin4_videodetect7,				0x1d08, 0x1d08, 0x2508);
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect7_vtotalf1,		16,	 0);	/* vertical total field 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_videodetect7_vtotalf2,		16,	 16);	/* vertical total field 2 */

/* hdmi4 input aux control */
NTV2_REG(ntv2_kona_reg_hdmiin4_auxcontrol,					0x1d09, 0x1d09, 0x2509);
NTV2_FLD(ntv2_kona_fld_hdmiin4_auxcontrol_auxactive,		1,	 0);	/* aux data active bank */
NTV2_CON(ntv2_kona_con_hdmiin4_auxactive_bank0,				0x0);		/* bank 0 */
NTV2_CON(ntv2_kona_con_hdmiin4_auxactive_bank1,				0x1);		/* bank 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_auxcontrol_auxread,			1,	 1);	/* aux data read bank */
NTV2_CON(ntv2_kona_con_hdmiin4_auxread_bank0,				0x0);		/* bank 0 */
NTV2_CON(ntv2_kona_con_hdmiin4_auxread_bank1,				0x1);		/* bank 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_auxcontrol_auxwrite,			1,	 2);	/* aux data write bank */
NTV2_CON(ntv2_kona_con_hdmiin4_auxwrite_bank0,				0x0);		/* bank 0 */
NTV2_CON(ntv2_kona_con_hdmiin4_auxwrite_bank1,				0x1);		/* bank 1 */
NTV2_FLD(ntv2_kona_fld_hdmiin4_auxcontrol_bank0count,		8,	 8);	/* aux bank 0 packet count */
NTV2_FLD(ntv2_kona_fld_hdmiin4_auxcontrol_bank1count,		8,	 16);	/* aux bank 1 packet count */

/* hdmi4 input tx status */
NTV2_REG(ntv2_kona_reg_hdmiin4_receiverstatus,				0x1d0a, 0x1d0a, 0x250a);

/* hdmi4 input rx error count */
NTV2_FLD(ntv2_kona_fld_hdmiin4_receiverstatus_errorcount,	24,	 0);

/* hdmi4 input aux packet ignore 0-3 */
NTV2_REG(ntv2_kona_reg_hdmiin4_auxpacketignore0,			0x1d0b, 0x1d0b, 0x250b);
NTV2_REG(ntv2_kona_reg_hdmiin4_auxpacketignore1,			0x1d0c, 0x1d0c, 0x250c);
NTV2_REG(ntv2_kona_reg_hdmiin4_auxpacketignore2,			0x1d0d, 0x1d0d, 0x250d);
NTV2_REG(ntv2_kona_reg_hdmiin4_auxpacketignore3,			0x1d0e, 0x1d0e, 0x250e);

/* hdmi4 input redriver control */
NTV2_REG(ntv2_kona_reg_hdmiin4_redrivercontrol,				0x1d0f, 0x1d0f, 0x250f);
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_power,		1,	 0);	/* power */
NTV2_CON(ntv2_kona_con_hdmiin4_power_disable,				0x0);		/* power disable */
NTV2_CON(ntv2_kona_con_hdmiin4_power_enable,				0x1);		/* power enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_pinmode,		1,	 1);	/* pin mode */
NTV2_CON(ntv2_kona_con_hdmiin4_pinmode_disable,				0x0);		/* pin disable */
NTV2_CON(ntv2_kona_con_hdmiin4_pinmode_enable,				0x1);		/* pin enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_vodrange,	1,	 2);	/* differential voltage range */
NTV2_CON(ntv2_kona_con_hdmiin4_vodrange_low,				0x0);		/* voltage swing low */
NTV2_CON(ntv2_kona_con_hdmiin4_vodrange_high,				0x1);		/* voltage swing high */
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_deemphasis,	2,	 4);	/* deemphasis */
NTV2_CON(ntv2_kona_con_hdmiin4_deemphasis_0d0db,			0x0);		/* 0 db */
NTV2_CON(ntv2_kona_con_hdmiin4_deemphasis_3d5db,			0x1);		/* 3.5 db */
NTV2_CON(ntv2_kona_con_hdmiin4_deemphasis_6d0db,			0x2);		/* 6 db */
NTV2_CON(ntv2_kona_con_hdmiin4_deemphasis_9d5db,			0x3);		/* 9.5 db */
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_preemphasis,	2,	 8);	/* preemphasis */
NTV2_CON(ntv2_kona_con_hdmiin4_preemphasis_0d0db,			0x0);		/* 0 db */
NTV2_CON(ntv2_kona_con_hdmiin4_preemphasis_1d6db,			0x1);		/* 1.6 db */
NTV2_CON(ntv2_kona_con_hdmiin4_preemphasis_3d5db,			0x2);		/* 3.5 db */
NTV2_CON(ntv2_kona_con_hdmiin4_preemphasis_6d0db,			0x3);		/* 6 db */
NTV2_FLD(ntv2_kona_fld_hdmiin4_redrivercontrol_boost,		4,	 12);	/* boost */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_00d25db,				0x0);		/* 0.25 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_00d80db,				0x1);		/* 0.80 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_01d10db,				0x2);		/* 1.1 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_02d20db,				0x3);		/* 2.2 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_04d10db,				0x4);		/* 4.1 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_07d10db,				0x5);		/* 7.1 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_09d00db,				0x6);		/* 9.0 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_10d30db,				0x7);		/* 10.3 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_11d80db,				0x8);		/* 11.8 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_13d90db,				0x9);		/* 13.9 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_15d30db,				0xa);		/* 15.3 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_16d90db,				0xb);		/* 16.9 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_17d90db,				0xc);		/* 17.9 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_19d20db,				0xd);		/* 19.2 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_20d50db,				0xe);		/* 20.5 db */
NTV2_CON(ntv2_kona_con_hdmiin4_boost_22d20db,				0xf);		/* 22.2 db */

/* hdmi4 input reference clock frequency */
NTV2_REG(ntv2_kona_reg_hdmiin4_refclockfrequency,			0x1d10, 0x1d10, 0x2510);

/* hdmi4 input tmds clock frequency */
NTV2_REG(ntv2_kona_reg_hdmiin4_tmdsclockfrequency,			0x1d11, 0x1d11, 0x2511);

/* hdmi4 input rx clock frequency */
NTV2_REG(ntv2_kona_reg_hdmiin4_rxclockfrequency,			0x1d12, 0x1d12, 0x2512);

/* hdmi4 input rx oversampling */
NTV2_REG(ntv2_kona_reg_hdmiin4_rxoversampling,				0x1d13, 0x1d13, 0x2513);
NTV2_FLD(ntv2_kona_fld_hdmiin4_rxoversampling_ratiofraction,10,	 0);	/* oversampling ratio fraction */
NTV2_FLD(ntv2_kona_fld_hdmiin4_rxoversampling_ratiointeger,	4,	 10);	/* oversampling ratio integer */
NTV2_FLD(ntv2_kona_fld_hdmiin4_rxoversampling_mode,			2,	 16);	/* oversampling mode */
NTV2_CON(ntv2_kona_con_hdmiin4_mode_none,					0x0);		/* no oversampling */
NTV2_CON(ntv2_kona_con_hdmiin4_mode_asynchronous,			0x1);		/* asynchronous oversampling */
NTV2_CON(ntv2_kona_con_hdmiin4_mode_synchronous,			0x2);		/* synchronous oversampling */

/* hdmi4 edid ram port */
NTV2_REG(ntv2_kona_reg_hdmiin4_edid,						0x1d1d, 0x1d1d, 0x251d);
NTV2_FLD(ntv2_kona_fld_hdmiin4_edid_write_data,				8,	 0);
NTV2_FLD(ntv2_kona_fld_hdmiin4_edid_read_data,				8,	 8);
NTV2_FLD(ntv2_kona_fld_hdmiin4_edid_address,				8,	 16);
NTV2_FLD(ntv2_kona_fld_hdmiin4_edid_write_enable,			1,	 24);
NTV2_FLD(ntv2_kona_fld_hdmiin4_edid_busy,					1,	 25);

/* hdmi4 input crop location */
NTV2_REG(ntv2_kona_reg_hdmiin4_croplocation,				0x1d1e, 0x1d1e, 0x251e);
NTV2_FLD(ntv2_kona_fld_hdmiin4_croplocation_start,			16,	 0);	/* crop start location */
NTV2_FLD(ntv2_kona_fld_hdmiin4_croplocation_end,			16,	 16);	/* crop end location */

/* hdmi4 input pixel control */
NTV2_REG(ntv2_kona_reg_hdmiin4_pixelcontrol,				0x1d1f, 0x1d1f, 0x251f);
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_lineinterleave,	1,	0);		/* line interleave */
NTV2_CON(ntv2_kona_con_hdmiin4_lineinterleave_disable,		0x0);		/* disable */
NTV2_CON(ntv2_kona_con_hdmiin4_lineinterleave_enable,		0x1);		/* enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_pixelinterleave,1,	1);		/* pixel interleave */
NTV2_CON(ntv2_kona_con_hdmiin4_pixelinterleave_disable,		0x0);		/* disable */
NTV2_CON(ntv2_kona_con_hdmiin4_pixelinterleave_enable,		0x1);		/* enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_420convert,		1,	2);		/* 420 to 422 conversion */
NTV2_CON(ntv2_kona_con_hdmiin4_420convert_disable,			0x0);		/* disable */
NTV2_CON(ntv2_kona_con_hdmiin4_420convert_enable,			0x1);		/* enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_cropmode,		1,	 3);	/* crop mode */
NTV2_CON(ntv2_kona_con_hdmiin4_cropmode_disable,			0x0);		/* disable */
NTV2_CON(ntv2_kona_con_hdmiin4_cropmode_enable,				0x1);		/* enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_hlinefilter,	1,	 4);	/* horizontal line filter mode */
NTV2_CON(ntv2_kona_con_hdmiin4_hlinefilter_disable,			0x0);		/* disable */
NTV2_CON(ntv2_kona_con_hdmiin4_hlinefilter_enable,			0x1);		/* enable */
NTV2_FLD(ntv2_kona_fld_hdmiin4_pixelcontrol_clockratio,		4,	 8);	/* core clock to data clock ratio */

/* free running audio sample counter register */
NTV2_REG(ntv2_kona_reg_audio_counter,						28);

/* audio system control registers */
NTV2_REG(ntv2_kona_reg_audio_control,						24, 240, 278, 279, 440, 444, 448, 452);
NTV2_FLD(ntv2_kona_fld_audio_capture_enable,				1,	0);
NTV2_FLD(ntv2_kona_fld_audio_loopback_mode,					1,	3);
NTV2_FLD(ntv2_kona_fld_audio_input_reset,					1,	8);
NTV2_FLD(ntv2_kona_fld_audio_output_reset,					1,	9);
NTV2_FLD(ntv2_kona_fld_audio_pause,							1,	11);
NTV2_FLD(ntv2_kona_fld_audio_8_channel,						1,	16);
NTV2_FLD(ntv2_kona_fld_audio_rate,							1,	18);
NTV2_FLD(ntv2_kona_fld_audio_16_channel,					1,	20);
NTV2_FLD(ntv2_kona_fld_audio_big_buffer,					1,	31);

/* audio source select registers */
NTV2_REG(ntv2_kona_reg_audio_source,						25, 241, 280, 281, 441, 445, 449, 453);
NTV2_FLD(ntv2_kona_fld_audio_input_ch12,					4,	0);
NTV2_FLD(ntv2_kona_fld_audio_input_ch34,					4,	4);
NTV2_FLD(ntv2_kona_fld_audio_input_ch56,					4,	8);
NTV2_FLD(ntv2_kona_fld_audio_input_ch78,					4,	12);
NTV2_FLD(ntv2_kona_fld_audio_embedded_input_b0,				1,	16);
NTV2_FLD(ntv2_kona_fld_audio_auto_erase,					1,	19);
NTV2_FLD(ntv2_kona_fld_audio_embedded_3gb,					1,	21);
NTV2_FLD(ntv2_kona_fld_audio_embedded_clock,				1,	22);
NTV2_FLD(ntv2_kona_fld_audio_embedded_input_b1,				1,	23);

/* audio ring io pointer registers */
NTV2_REG(ntv2_kona_reg_audio_output_address,				26, 242, 283, 285, 442, 446, 450, 454);
NTV2_REG(ntv2_kona_reg_audio_input_address,					27, 243, 284, 286, 443, 447, 451, 455);

/* serial status register */
NTV2_REG(ntv2_kona_reg_serial_status,						0x2200);
NTV2_FLD(ntv2_kona_fld_serial_rx_valid,						1,	0);
NTV2_FLD(ntv2_kona_fld_serial_rx_full,						1,	1);
NTV2_FLD(ntv2_kona_fld_serial_tx_empty,						1,	2);
NTV2_FLD(ntv2_kona_fld_serial_tx_full,						1,	3);
NTV2_FLD(ntv2_kona_fld_serial_interrupt_state,				1,	4);
NTV2_FLD(ntv2_kona_fld_serial_error_overrun,				1,	5);
NTV2_FLD(ntv2_kona_fld_serial_error_frame,					1,	6);
NTV2_FLD(ntv2_kona_fld_serial_error_parity,					1,	7);
NTV2_FLD(ntv2_kona_fld_serial_int_active,					1,	8);
NTV2_FLD(ntv2_kona_fld_serial_loopback_state,				1,	30);

/* serial control register */
NTV2_REG(ntv2_kona_reg_serial_control,						0x2204);
NTV2_FLD(ntv2_kona_fld_serial_reset_tx,						1,	0);
NTV2_FLD(ntv2_kona_fld_serial_reset_rx,						1,	1);
NTV2_FLD(ntv2_kona_fld_serial_interrupt_enable,				1,	4);
NTV2_FLD(ntv2_kona_fld_serial_interrupt_clear,				1,	8);
NTV2_FLD(ntv2_kona_fld_serial_loopback_enable,				1,	30);
NTV2_FLD(ntv2_kona_fld_serial_rx_trigger,					1,	31);

/* serial register */
NTV2_REG(ntv2_kona_reg_serial_rx,							0x2208);
NTV2_FLD(ntv2_kona_fld_serial_rx_data,						8,	0);
NTV2_FLD(ntv2_kona_fld_serial_rx_active,					1,	31);

NTV2_REG(ntv2_kona_reg_serial_tx,							0x220c);
NTV2_FLD(ntv2_kona_fld_serial_tx_data,						8,	0);

/* video crosspoint registers */
NTV2_REG(ntv2_kona_reg_xpt_select1,							136);
NTV2_FLD(ntv2_kona_fld_lut1_source,							8,	0);
NTV2_FLD(ntv2_kona_fld_csc1_vid_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_conv1_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_comp1_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select2,							137);
NTV2_FLD(ntv2_kona_fld_fb1_ds1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_fs1_source,							8,	8);
NTV2_FLD(ntv2_kona_fld_fs2_source,							8,	16);
NTV2_FLD(ntv2_kona_fld_dlout1_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select3,							138);
NTV2_FLD(ntv2_kona_fld_anaout1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout1_ds1_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_sdiout2_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_csc1_key_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select4,							139);
NTV2_FLD(ntv2_kona_fld_mix1_fgv_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_mix1_fgk_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_mix1_bgv_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_mix1_bgk_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select5,							140);
NTV2_FLD(ntv2_kona_fld_fb2_ds1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_lut2_source,							8,	8);
NTV2_FLD(ntv2_kona_fld_csc2_vid_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_csc2_key_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select6,							141);
NTV2_FLD(ntv2_kona_fld_fwm1_source,							8,	0);
NTV2_FLD(ntv2_kona_fld_iict1_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_hdmiout1_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_conv2_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select7,							163);
NTV2_FLD(ntv2_kona_fld_fwm2_source,							8,	0);
NTV2_FLD(ntv2_kona_fld_iict2_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_dlout2_source,						8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select8,							164);
NTV2_FLD(ntv2_kona_fld_sdiout3_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout4_ds1_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_sdiout5_ds1_source,					8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select9,							250);
NTV2_FLD(ntv2_kona_fld_mix2_fgv_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_mix2_fgk_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_mix2_bgv_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_mix2_bgk_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select10,						251);
NTV2_FLD(ntv2_kona_fld_sdiout1_ds2_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout2_ds2_source,					8,	8);

NTV2_REG(ntv2_kona_reg_xpt_select11,						191);
NTV2_FLD(ntv2_kona_fld_dlin1_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_dlin1_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_dlin2_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_dlin2_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select12,						193);
NTV2_FLD(ntv2_kona_fld_lut3_source,							8,	0);
NTV2_FLD(ntv2_kona_fld_lut4_source,							8,	8);
NTV2_FLD(ntv2_kona_fld_lut5_source,							8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select13,						263);
NTV2_FLD(ntv2_kona_fld_fb3_ds1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_fb4_ds1_source,						8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select14,						264);
NTV2_FLD(ntv2_kona_fld_sdiout3_ds2_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout5_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_sdiout4_ds2_source,					8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select15,						302);
NTV2_FLD(ntv2_kona_fld_dlin3_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_dlin3_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_dlin4_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_dlin4_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select16,						303);
NTV2_FLD(ntv2_kona_fld_dlout3_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_dlout4_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_dlout5_source,						8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select17,						301);
NTV2_FLD(ntv2_kona_fld_csc3_vid_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_csc3_key_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_csc4_vid_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_csc4_key_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select18,						352);
NTV2_FLD(ntv2_kona_fld_csc5_vid_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_csc5_key_source,						8,	8);

NTV2_REG(ntv2_kona_reg_xpt_select19,						356);
NTV2_FLD(ntv2_kona_fld_4kdc_q1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_4kdc_q2_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_4kdc_q3_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_4kdc_q4_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select20,						357);
NTV2_FLD(ntv2_kona_fld_hdmiout1_q1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_hdmiout1_q2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_hdmiout1_q3_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_hdmiout1_q4_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select21,						400);
NTV2_FLD(ntv2_kona_fld_fb5_ds1_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_fb6_ds1_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_fb7_ds1_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_fb8_ds1_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select22,						401);
NTV2_FLD(ntv2_kona_fld_sdiout6_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout6_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_sdiout7_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_sdiout7_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select23,						403);
NTV2_FLD(ntv2_kona_fld_csc7_vid_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_csc7_key_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_csc8_vid_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_csc8_key_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select24,						404);
NTV2_FLD(ntv2_kona_fld_lut6_source,							8,	0);
NTV2_FLD(ntv2_kona_fld_lut7_source,							8,	8);
NTV2_FLD(ntv2_kona_fld_lut8_source,							8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select25,						405);
NTV2_FLD(ntv2_kona_fld_dlin5_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_dlin5_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_dlin6_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_dlin6_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select26,						406);
NTV2_FLD(ntv2_kona_fld_dlin7_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_dlin7_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_dlin8_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_dlin8_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select27,						407);
NTV2_FLD(ntv2_kona_fld_dlout6_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_dlout7_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_dlout8_source,						8,	16);

NTV2_REG(ntv2_kona_reg_xpt_select28,						408);
NTV2_FLD(ntv2_kona_fld_mix3_fgv_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_mix3_fgk_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_mix3_bgv_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_mix3bgk_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select29,						409);
NTV2_FLD(ntv2_kona_fld_mix4_fgv_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_mix4_fgk_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_mix4_bgv_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_mix4_bgk_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select30,						402);
NTV2_FLD(ntv2_kona_fld_sdiout8_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_sdiout8_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_csc6_vid_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_csc6_key_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select31,						439);

NTV2_REG(ntv2_kona_reg_xpt_select32,						506);
NTV2_FLD(ntv2_kona_fld_425mux1_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_425mux1_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_425mux2_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_425mux2_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select33,						507);
NTV2_FLD(ntv2_kona_fld_425mux3_ds1_source,					8,	0);
NTV2_FLD(ntv2_kona_fld_425mux3_ds2_source,					8,	8);
NTV2_FLD(ntv2_kona_fld_425mux4_ds1_source,					8,	16);
NTV2_FLD(ntv2_kona_fld_425mux4_ds2_source,					8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select34,						508);
NTV2_FLD(ntv2_kona_fld_fb1_ds2_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_fb2_ds2_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_fb3_ds2_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_fb4_ds2_source,						8,	24);

NTV2_REG(ntv2_kona_reg_xpt_select35,						509);
NTV2_FLD(ntv2_kona_fld_fb5_ds2_source,						8,	0);
NTV2_FLD(ntv2_kona_fld_fb6_ds2_source,						8,	8);
NTV2_FLD(ntv2_kona_fld_fb7_ds2_source,						8,	16);
NTV2_FLD(ntv2_kona_fld_fb8_ds2_source,						8,	24);

void ntv2_kona_register_initialize(void);

const char* ntv2_video_standard_name(u32 standard);
const char* ntv2_video_geometry_name(u32 geometry);
const char* ntv2_input_geometry_name(u32 geometry);
const char* ntv2_frame_rate_name(u32 rate);
const char* ntv2_color_space_name(u32 rate);
const char* ntv2_color_depth_name(u32 rate);

u32 ntv2_audio_frame_samples(u32 frame_rate, u32 cadence);

u32 ntv2_frame_rate_duration(u32 frame_rate);
u32 ntv2_frame_rate_scale(u32 frame_rate);
bool ntv2_frame_rate_drop(u32 frame_rate);

u32 ntv2_timecode_rate(u32 frame_rate);
bool ntv2_timecode_drop(u32 frame_rate);

u32 ntv2_frame_geometry_width(u32 frame_geometry);
u32 ntv2_frame_geometry_height(u32 frame_geometry);

u32 ntv2_video_standard_width(u32 video_standard);
u32 ntv2_video_standard_height(u32 video_standard);
bool ntv2_video_standard_progressive(u32 video_standard);

u32 ntv2_video_standard_to_hdmiin(u32 video_standard);
u32 ntv2_frame_rate_to_hdmiin(u32 frame_rate);

u32 ntv2_read_frame_size(struct ntv2_register *ntv2_reg, int index);

void ntv2_video_read_interrupt_status(struct ntv2_register *ntv2_reg,
									  struct ntv2_interrupt_status* irq_status);

void ntv2_video_input_interrupt_enable(struct ntv2_register *ntv2_reg, int index, bool enable);
void ntv2_video_input_interrupt_clear(struct ntv2_register *ntv2_reg, int index);
bool ntv2_video_input_interrupt_active(struct ntv2_interrupt_status* irq_status, int index);
u32 ntv2_video_input_field_id(struct ntv2_interrupt_status* irq_status, int index);

void ntv2_video_output_interrupt_enable(struct ntv2_register *ntv2_reg, int index, bool enable);
void ntv2_video_output_interrupt_clear(struct ntv2_register *ntv2_reg, int index);
bool ntv2_video_output_interrupt_active(struct ntv2_interrupt_status* irq_status, int index);
u32 ntv2_video_output_field_id(struct ntv2_interrupt_status* irq_status, int index);

u32 ntv2_video_output_interrupt_rate(struct ntv2_register *ntv2_reg, int index);

void ntv2_sdi_output_transmit_enable(struct ntv2_register *ntv2_reg, int index, bool enable);
void ntv2_sdi_input_convert_3g_enable(struct ntv2_register *ntv2_reg, int index, bool enable);
void ntv2_qrc_4k_enable(struct ntv2_register *ntv2_reg, bool input, bool output);

void ntv2_read_sdi_input_status(struct ntv2_register *ntv2_reg, int index,
								struct ntv2_sdi_input_status *input_status);
void ntv2_read_aes_input_status(struct ntv2_register* ntv2_reg, int index,
								struct ntv2_aes_input_status *input_status);

void ntv2_route_sdi_to_fs(struct ntv2_register* ntv2_reg,
						  int sdi_index, int sdi_stream, bool sdi_rgb,
						  int fs_index, int fs_stream);
void ntv2_route_sdi_to_csc(struct ntv2_register* ntv2_reg,
						   int sdi_index, int sdi_stream, bool sdi_rgb,
						   int csc_index, int csc_stream);
void ntv2_route_sdi_to_mux(struct ntv2_register* ntv2_reg,
						   int sdi_index, int sdi_stream, bool sdi_rgb,
						   int mux_index, int mux_stream);
void ntv2_route_hdmi_to_fs(struct ntv2_register* ntv2_reg,
						   int hdmi_index, int hdmi_stream, bool hdmi_rgb,
						   int fs_index, int fs_stream);
void ntv2_route_hdmi_to_csc(struct ntv2_register* ntv2_reg,
							int hdmi_index, int hdmi_stream, bool hdmi_rgb,
							int csc_index, int csc_stream);
void ntv2_route_hdmi_to_mux(struct ntv2_register* ntv2_reg,
							int hdmi_index, int hdmi_stream, bool hdmi_rgb,
							int mux_index, int mux_stream);
void ntv2_route_hdmi_to_lut(struct ntv2_register* ntv2_reg,
							int hdmi_index, int hdmi_stream, bool hdmi_rgb,
							int lut_index, int lut_stream);
void ntv2_route_csc_to_fs(struct ntv2_register* ntv2_reg,
						  int csc_index, int csc_stream, bool csc_rgb,
						  int fs_index, int fs_stream);
void ntv2_route_csc_to_mux(struct ntv2_register* ntv2_reg,
						   int csc_index, int csc_stream, bool csc_rgb,
						   int mux_index, int mux_stream);
void ntv2_route_csc_to_lut(struct ntv2_register* ntv2_reg,
						   int csc_index, int csc_stream, bool csc_rgb,
						   int lut_index, int lut_stream);
void ntv2_route_mux_to_fs(struct ntv2_register* ntv2_reg,
						  int mux_index, int mux_stream, bool mux_rgb,
						  int fs_index, int fs_stream);
void ntv2_route_lut_to_csc(struct ntv2_register* ntv2_reg,
						   int lut_index, int lut_stream,
						   int csc_index, int csc_stream);

#endif
