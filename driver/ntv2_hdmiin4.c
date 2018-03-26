/*
 * NTV2 HDMI4 input control
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

#include "ntv2_hdmiin4.h"
#include "ntv2_features.h"
#include "ntv2_konareg.h"
#include "ntv2_register.h"
#include "ntv2_konai2c.h"


enum ntv2_hdmi_clock_type
{
	ntv2_clock_type_unknown,
	ntv2_clock_type_sdd,
	ntv2_clock_type_sdn,
	ntv2_clock_type_hdd,
	ntv2_clock_type_hdn,
	ntv2_clock_type_3gd,
	ntv2_clock_type_3gn,
	ntv2_clock_type_4kd,
	ntv2_clock_type_4kn,
	ntv2_clock_type_h2d,
	ntv2_clock_type_h2n,
	ntv2_clock_type_size
};

struct ntv2_hdmi_format_data
{
	u32					video_standard;
	u32					frame_rate;
	u32					h_sync_start;
	u32					h_sync_end;
	u32					h_de_start;
	u32					h_total;
	u32					v_trans_f1;
	u32					v_trans_f2;
	u32					v_sync_start_f1;
	u32					v_sync_end_f1;
	u32					v_de_start_f1;
	u32					v_de_start_f2;
	u32					v_sync_start_f2;
	u32					v_sync_end_f2;
	u32					v_total_f1;
	u32					v_total_f2;
	u32					clock_ratio;
	u8					avi_byte_4;
	u8					hdmi_byte_5;
	enum ntv2_hdmi_clock_type	clock_type;
};

struct ntv2_hdmi_clock_data
{
	enum ntv2_hdmi_clock_type	clock_type;
	u32					bit_depth;
	u32					line_rate;
	u32					tmds_rate;
};


static struct ntv2_hdmi_format_data c_hdmi_format_data[] = {
	{ ntv2_kona_video_standard_525i,	    ntv2_kona_frame_rate_2997,    19,   81,  138,  858,   19,  448,    4,    7,   22,  285,  266,  269,  262,  525,    2,    0,    0, ntv2_clock_type_sdn },
	{ ntv2_kona_video_standard_625i,	    ntv2_kona_frame_rate_2500,    12,   75,  144,  864,   12,  444,    2,    5,   24,  337,  314,  317,  312,  625,    2,    0,    0, ntv2_clock_type_sdn },
	{ ntv2_kona_video_standard_720p,	    ntv2_kona_frame_rate_5000,   440,  480,  700, 1980,  440,    0,    5,   10,   30,    0,    0,    0,  750,    0,    2, 0x13,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_720p,	    ntv2_kona_frame_rate_5994,   110,  150,  370, 1650,  110,    0,    5,   10,   30,    0,    0,    0,  750,    0,    2, 0x04,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_720p,	    ntv2_kona_frame_rate_6000,   110,  150,  370, 1650,  110,    0,    5,   10,   30,    0,    0,    0,  750,    0,    2, 0x04,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080i,       ntv2_kona_frame_rate_2500,   528,  572,  720, 2640,  528, 1848,    2,    7,   22,  585,  564,  569,  562, 1125,    2, 0x14,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080i,       ntv2_kona_frame_rate_2997,    88,  132,  280, 2200,   88, 1188,    2,    7,   22,  585,  564,  569,  562, 1125,    2, 0x05,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_1080i,       ntv2_kona_frame_rate_3000,    88,  132,  280, 2200,   88, 1188,    2,    7,   22,  585,  564,  569,  562, 1125,    2, 0x05,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_2398,   638,  682,  830, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2, 0x20,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_2400,   638,  682,  830, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2, 0x20,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_2500,   528,  572,  720, 2640,  528,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2, 0x21,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_2997,    88,  132,  280, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2, 0x22,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_3000,    88,  132,  280, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2, 0x22,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_4795,   638,  682,  830, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1, 0x6f,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_4800,   638,  682,  830, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1, 0x6f,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_5000,   528,  572,  720, 2640,  528,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1, 0x1f,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_5994,    88,  132,  280, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1, 0x10,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_1080p,       ntv2_kona_frame_rate_6000,    88,  132,  280, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1, 0x10,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_2048x1080i,  ntv2_kona_frame_rate_2500,   528,  572,  702, 2740,  528,    0,    2,    7,   22,  585,  564,  569,  562, 1125,    2,    0,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_2048x1080i,  ntv2_kona_frame_rate_2997,    88,  132,  152, 2200,   88, 1188,    2,    7,   22,  585,  564,  569,  562, 1125,    2,    0,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_2048x1080i,  ntv2_kona_frame_rate_3000,    88,  132,  152, 2200,   88, 1188,    2,    7,   22,  585,  564,  569,  562, 1125,    2,    0,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_2398,   638,  682,  702, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2,    0,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_2400,   638,  682,  702, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2,    0,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_2500,   528,  572,  592, 2640,  528,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2,    0,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_2997,    88,  132,  152, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2,    0,    0, ntv2_clock_type_hdd },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_3000,    88,  132,  152, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    2,    0,    0, ntv2_clock_type_hdn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_4795,   638,  682,  702, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1,    0,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_4800,   638,  682,  702, 2750,  638,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1,    0,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_5000,   528,  572,  592, 2640,  528,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1,    0,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_5994,    88,  132,  152, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1,    0,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_2048x1080p,  ntv2_kona_frame_rate_6000,    88,  132,  152, 2200,   88,    0,    4,    9,   45,    0,    0,    0, 1125,    0,    1,    0,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2398,   638,  682,  830, 2750,  638,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5d,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2400,   638,  682,  830, 2750,  638,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5d,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2500,   528,  572,  720, 2640,  528,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x60,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2997,    88,  132,  280, 2200,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_3000,    88,  132,  280, 2200,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2398,  1276, 1364, 1660, 5500, 1276,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5d, 0x03, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2400,  1276, 1364, 1660, 5500, 1276,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5d, 0x03, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2500,  1056, 1144, 1440, 5280, 1056,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5e, 0x02, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_2997,   176,  264,  560, 4400,  176,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5f, 0x01, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_3000,   176,  264,  560, 4400,  176,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x5f, 0x01, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_4795,   638,  682,  830, 2750,  638,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x72,    0, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_4800,   638,  682,  830, 2750,  638,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x72,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_5000,   528,  572,  720, 2640,  528,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x60,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_5994,    88,  132,  280, 2200,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_6000,    88,  132,  280, 2200,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_4795,  1276, 1364, 1660, 5500, 1276,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x72,    0, ntv2_clock_type_h2d },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_4800,  1276, 1364, 1660, 5500, 1276,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x72,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_5000,  1056, 1144, 1440, 5280, 1056,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x60,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_5994,   176,  264,  560, 4400,  176,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_h2d },
	{ ntv2_kona_video_standard_3840x2160p,  ntv2_kona_frame_rate_6000,   176,  264,  560, 4400,  176,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x61,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2398,   510,  554,  702, 2750,  510,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x62,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2400,   510,  554,  702, 2750,  510,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x62,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2500,   484,  528,  592, 2640,  484,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x63,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2997,    44,   88,  152, 2200,   44,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x64,    0, ntv2_clock_type_3gd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_3000,    44,   88,  152, 2200,   44,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x64,    0, ntv2_clock_type_3gn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2398,  1020, 1108, 1404, 5500, 1020,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x62, 0x04, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2400,  1020, 1108, 1404, 5500, 1020,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x62, 0x04, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2500,   968, 1056, 1184, 5280,  968,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x63,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_2997,    88,  176,  304, 4400,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x64,    0, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_3000,    88,  176,  304, 4400,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x64,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_4795,   510,  554,  702, 2750,  510,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x73,    0, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_4800,   510,  554,  702, 2750,  510,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x73,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_5000,   484,  528,  592, 2640,  484,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x65,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_5994,    44,   88,  152, 2200,   44,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x66,    0, ntv2_clock_type_4kd },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_6000,    44,   88,  152, 2200,   44,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x66,    0, ntv2_clock_type_4kn },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_4795,  1020, 1108, 1404, 5500, 1020,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x73,    0, ntv2_clock_type_h2d },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_4800,  1020, 1108, 1404, 5500, 1020,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x73,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_5000,   968, 1056, 1184, 5280,  968,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x65,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_5994,    88,  176,  304, 4400,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x66,    0, ntv2_clock_type_h2d },
	{ ntv2_kona_video_standard_4096x2160p,  ntv2_kona_frame_rate_6000,    88,  176,  304, 4400,   88,    0,    8,   18,   90,    0,    0,    0, 2250,    0,    0, 0x66,    0, ntv2_clock_type_h2n },
	{ ntv2_kona_video_standard_none,        ntv2_kona_frame_rate_none,     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, ntv2_clock_type_unknown }
};


static struct ntv2_hdmi_clock_data c_hdmi_clock_data[] = {
	{ ntv2_clock_type_sdd,     8,      ntv2_kona_con_hdmiin4_linerate_270mhz,       26973027 },
	{ ntv2_clock_type_sdd,    10,      ntv2_kona_con_hdmiin4_linerate_337mhz,       33716284 },
	{ ntv2_clock_type_sdd,    12,      ntv2_kona_con_hdmiin4_linerate_405mhz,       40459540 },

	{ ntv2_clock_type_sdn,     8,      ntv2_kona_con_hdmiin4_linerate_270mhz,       27000000 },
	{ ntv2_clock_type_sdn,    10,      ntv2_kona_con_hdmiin4_linerate_337mhz,       33750000 },
	{ ntv2_clock_type_sdn,    12,      ntv2_kona_con_hdmiin4_linerate_405mhz,       40500000 },

	{ ntv2_clock_type_hdd,     8,      ntv2_kona_con_hdmiin4_linerate_742mhz,       74175824 },
	{ ntv2_clock_type_hdd,    10,      ntv2_kona_con_hdmiin4_linerate_928mhz,       92719780 },
	{ ntv2_clock_type_hdd,    12,      ntv2_kona_con_hdmiin4_linerate_1113mhz,     111263736 },

	{ ntv2_clock_type_hdn,     8,      ntv2_kona_con_hdmiin4_linerate_742mhz,       74250000 },
	{ ntv2_clock_type_hdn,    10,      ntv2_kona_con_hdmiin4_linerate_928mhz,       92812500 },
	{ ntv2_clock_type_hdn,    12,      ntv2_kona_con_hdmiin4_linerate_1113mhz,     111375000 },

	{ ntv2_clock_type_3gd,     8,      ntv2_kona_con_hdmiin4_linerate_1485mhz,     148351648 },
	{ ntv2_clock_type_3gd,    10,      ntv2_kona_con_hdmiin4_linerate_1856mhz,     185439560 },
	{ ntv2_clock_type_3gd,    12,      ntv2_kona_con_hdmiin4_linerate_2227mhz,     222527472 },

	{ ntv2_clock_type_3gn,     8,      ntv2_kona_con_hdmiin4_linerate_1485mhz,     148500000 },
	{ ntv2_clock_type_3gn,    10,      ntv2_kona_con_hdmiin4_linerate_1856mhz,     185625000 },
	{ ntv2_clock_type_3gn,    12,      ntv2_kona_con_hdmiin4_linerate_2227mhz,     222750000 },

	{ ntv2_clock_type_4kd,     8,      ntv2_kona_con_hdmiin4_linerate_2970mhz,     296703297 },
	{ ntv2_clock_type_4kn,     8,      ntv2_kona_con_hdmiin4_linerate_2970mhz,     297000000 },

	{ ntv2_clock_type_4kd,    10,      ntv2_kona_con_hdmiin4_linerate_3712mhz,      92719780 },
	{ ntv2_clock_type_4kd,    12,      ntv2_kona_con_hdmiin4_linerate_4455mhz,     111263736 },
	{ ntv2_clock_type_4kn,    10,      ntv2_kona_con_hdmiin4_linerate_3712mhz,      92812500 },
	{ ntv2_clock_type_4kn,    12,      ntv2_kona_con_hdmiin4_linerate_4455mhz,     111375000 },

	{ ntv2_clock_type_h2d,     8,      ntv2_kona_con_hdmiin4_linerate_5940mhz,     148351648 },
	{ ntv2_clock_type_h2n,     8,      ntv2_kona_con_hdmiin4_linerate_5940mhz,     148500000 },

	{ ntv2_clock_type_unknown, 0,       0,                                    0 }
};


static const u32 c_default_timeout		= 250;
static const u32 c_redriver_time		= 10;
static const u32 c_plug_time			= 10;
static const u32 c_lock_wait_max		= 2;
static const u32 c_unlock_wait_max		= 4;
static const u32 c_plug_wait_max		= 32;


static int ntv2_hdmiin4_monitor(void* data);
static void ntv2_hdmiin4_initialize(struct ntv2_hdmiin4 *ntv2_hin);

static bool is_input_locked(struct ntv2_hdmiin4 *ntv2_hin);
static bool is_deserializer_locked(struct ntv2_hdmiin4 *ntv2_hin);
static void reset_lock(struct ntv2_hdmiin4 *ntv2_hin);
static void hot_plug(struct ntv2_hdmiin4 *ntv2_hin);
static bool has_video_input_changed(struct ntv2_hdmiin4 *ntv2_hin);
static bool has_audio_input_changed(struct ntv2_hdmiin4 *ntv2_hin);
static bool update_input_state(struct ntv2_hdmiin4 *ntv2_hin);
static bool has_audio_control_changed(struct ntv2_hdmiin4 *ntv2_hin);
static bool config_audio_control(struct ntv2_hdmiin4 *ntv2_hin);
static void set_no_video(struct ntv2_hdmiin4 *ntv2_hin);

static struct ntv2_hdmi_format_data* find_format_data(u32 h_sync_start,
													 u32 h_sync_end,
													 u32 h_de_start,
													 u32 h_total,
													 u32 v_trans_f1,
													 u32 v_trans_f2,
													 u32 v_sync_start_f1,
													 u32 v_sync_end_f1,
													 u32 v_de_start_f1,
													 u32 v_de_start_f2,
													 u32 v_sync_start_f2,
													 u32 v_sync_end_f2,
													 u32 v_total_f1,
													 u32 v_total_f2,
													 enum ntv2_hdmi_clock_type	clockType);
static struct ntv2_hdmi_clock_data* find_clock_data(u32 lineRate, u32 tmdsRate);
static bool compare_tmds_rate(u32 tmdsRate, u32 tmdsRef);

struct ntv2_hdmiin4 *ntv2_hdmiin4_open(struct ntv2_object *ntv2_obj,
									   const char *name, int index)
{
	struct ntv2_hdmiin4 *ntv2_hin = NULL;

	ntv2_hin = kzalloc(sizeof(struct ntv2_hdmiin4), GFP_KERNEL);
	if (ntv2_hin == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_hdmiin4 instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_hin->index = index;
	snprintf(ntv2_hin->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_hin->list);
	ntv2_hin->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_hin->state_lock);

	NTV2_MSG_HDMIIN_INFO("%s: open ntv2_hdmiin4\n", ntv2_hin->name);

	return ntv2_hin;
}

void ntv2_hdmiin4_close(struct ntv2_hdmiin4 *ntv2_hin)
{
	if (ntv2_hin == NULL) 
		return;

	NTV2_MSG_HDMIIN_INFO("%s: close ntv2_hdmiin4\n", ntv2_hin->name);

	ntv2_hdmiin4_disable(ntv2_hin);

	memset(ntv2_hin, 0, sizeof(struct ntv2_hdmiin4));
	kfree(ntv2_hin);
}

int ntv2_hdmiin4_configure(struct ntv2_hdmiin4 *ntv2_hin,
						   struct ntv2_features *features,
						   struct ntv2_register *vid_reg)
{
	if ((ntv2_hin == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_HDMIIN_INFO("%s: configure hdmi input device\n", ntv2_hin->name);

	ntv2_hin->features = features;
	ntv2_hin->vid_reg = vid_reg;

	return 0;
}

int ntv2_hdmiin4_enable(struct ntv2_hdmiin4 *ntv2_hin)
{
	if (ntv2_hin == NULL)
		return -EPERM;

	if (ntv2_hin->monitor_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_HDMIIN_STATE("%s: enable hdmi input monitor\n", ntv2_hin->name);

	ntv2_hin->monitor_task = kthread_run(ntv2_hdmiin4_monitor, (void*)ntv2_hin, ntv2_hin->name);
	if (IS_ERR(ntv2_hin->monitor_task)) {
		ntv2_hin->monitor_task = NULL;
		return -ENOMEM;
	}

	ntv2_hin->monitor_state = ntv2_task_state_enable;

	return 0;
}

int ntv2_hdmiin4_disable(struct ntv2_hdmiin4 *ntv2_hin)
{
	if (ntv2_hin == NULL)
		return -EPERM;

	if (ntv2_hin->monitor_state != ntv2_task_state_enable)
		return 0;

	NTV2_MSG_HDMIIN_STATE("%s: disable hdmi input monitor\n", ntv2_hin->name);

	if (ntv2_hin->monitor_task != NULL) {
		kthread_stop(ntv2_hin->monitor_task);
		ntv2_hin->monitor_task = NULL;
	}

	ntv2_hin->monitor_state = ntv2_task_state_disable;

	return 0;
}

int ntv2_hdmiin4_get_input_format(struct ntv2_hdmiin4 *ntv2_hin,
								  struct ntv2_hdmiin4_format *format)
{
	unsigned long flags;

	if ((ntv2_hin == NULL) ||
		(format == NULL))
		return -EPERM;

	spin_lock_irqsave(&ntv2_hin->state_lock, flags);
	*format = ntv2_hin->input_format;
	spin_unlock_irqrestore(&ntv2_hin->state_lock, flags);

	return 0;
}

static int ntv2_hdmiin4_monitor(void* data)
{
	struct ntv2_hdmiin4 *ntv2_hin = (struct ntv2_hdmiin4 *)data;
	u32 lockWait = 0;
	u32 unlockWait = 0;
	u32 plugWait = 0;
	bool lock = false;
	bool reset = false;
	bool new_input = true;

	if (ntv2_hin == NULL)
		return 0;

	NTV2_MSG_HDMIIN_STATE("%s: hdmi input monitor task start\n", ntv2_hin->name);

	ntv2_hdmiin4_initialize(ntv2_hin);

	while(!kthread_should_stop())
	{
		if (is_input_locked(ntv2_hin)) {
			reset = false;
			unlockWait = 0;

			lockWait++;
			if (lockWait < c_lock_wait_max) {
				goto wait;
			}

			if (!lock) {
				NTV2_MSG_HDMIIN_STATE("%s: input is locked\n", ntv2_hin->name);
				lock = true;
				plugWait = 0;
			}

			if (has_video_input_changed(ntv2_hin) || has_audio_input_changed(ntv2_hin)) {
				NTV2_MSG_HDMIIN_STATE("%s: input change detected\n", ntv2_hin->name);
				new_input = true;
			}

			if (has_audio_control_changed(ntv2_hin)) {
				NTV2_MSG_HDMIIN_STATE("%s: audio control change detected\n", ntv2_hin->name);
				config_audio_control(ntv2_hin);
			}

			if (ntv2_hin->video_standard == ntv2_kona_video_standard_none) {
				new_input = true;
			}

			if (new_input) {
				if (!update_input_state(ntv2_hin)) {
					plugWait++;
					if (plugWait > c_plug_wait_max) {
						plugWait = 0;
						hot_plug(ntv2_hin);
					}
				}
				new_input = false;
			}
		} 
		else {
			lockWait = 0;

			unlockWait++;
			if (unlockWait < c_unlock_wait_max) {
				goto wait;
			}

			if (lock) {
				NTV2_MSG_HDMIIN_STATE("%s: input is unlocked\n", ntv2_hin->name);
				lock = false;
				plugWait = 0;
			}

			if (!reset) {
				set_no_video(ntv2_hin);
				reset_lock(ntv2_hin);
				reset = true;
			}

			plugWait++;
			if (plugWait > c_plug_wait_max) {
				plugWait = 0;
				hot_plug(ntv2_hin);
			}
		}

	wait:
		// sleep
		msleep_interruptible(c_default_timeout);
	}

	NTV2_MSG_HDMIIN_STATE("%s: hdmi input monitor task stop\n", ntv2_hin->name);

	return 0;
}

static void ntv2_hdmiin4_initialize(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 value;
	u32 mask;

	if (ntv2_hin == NULL)
		return;

	ntv2_hin->video_control		= 0;
	ntv2_hin->video_detect0		= 0;
	ntv2_hin->video_detect1		= 0;
	ntv2_hin->video_detect2		= 0;
	ntv2_hin->video_detect3		= 0;
	ntv2_hin->video_detect4		= 0;
	ntv2_hin->video_detect5		= 0;
	ntv2_hin->video_detect6		= 0;
	ntv2_hin->video_detect7		= 0;
	ntv2_hin->tmds_rate			= 0;

	ntv2_hin->input_locked		= false;
	ntv2_hin->hdmi_mode			= false;
	ntv2_hin->video_standard	= ntv2_kona_video_standard_none;
	ntv2_hin->frame_rate		= ntv2_kona_frame_rate_none;
	ntv2_hin->color_space		= 0;
	ntv2_hin->color_depth		= 0;

	ntv2_hin->audio_swap		= true;

	// configure hot plug and audio swap
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode, ntv2_kona_con_hdmiin4_hotplugmode_enable);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_audioswapmode, ntv2_kona_con_hdmiin4_audioswapmode_enable);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_audioswapmode);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

	// setup redriver
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_redrivercontrol_power, ntv2_kona_con_hdmiin4_power_disable);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_redrivercontrol_power);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_redrivercontrol_pinmode, ntv2_kona_con_hdmiin4_pinmode_enable);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_redrivercontrol_pinmode);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_redrivercontrol, ntv2_hin->index, value, mask);

	// wait for redriver reset
	msleep_interruptible(c_redriver_time);

	// enable redriver
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_redrivercontrol_power, ntv2_kona_con_hdmiin4_power_enable);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_redrivercontrol_power);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_redrivercontrol, ntv2_hin->index, value, mask);

	return;
}

static bool is_input_locked(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_inputlock);
	u32 value;

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index);
	if ((value & mask) == mask) return true;

	return false;
}

static bool is_deserializer_locked(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_deseriallock);
	u32 value;

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index);
	if ((value & mask) == mask) return true;

	return false;
}

static void reset_lock(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 reset = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_reset);

	// reset lock
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, reset, reset);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, 0, reset);
}

static void hot_plug(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 value = 0;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode);

	// disable hot plug
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode, ntv2_kona_con_hdmiin4_hotplugmode_disable);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

	// wait for input
	msleep_interruptible(c_plug_time);

	// enable hot plug
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_hotplugmode, ntv2_kona_con_hdmiin4_hotplugmode_enable);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);
}

static bool has_video_input_changed(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 value;
	bool changed = false;
	u32 control_mask = 
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_scrambledetect) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_descramblemode) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_scdcratedetect) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_scdcratemode) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_linerate) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_inputlock) |
		NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_hdmi5vdetect);

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index) & control_mask;
	if (value != ntv2_hin->video_control) {
		ntv2_hin->video_control = value;
		changed = true;
	}

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect0, ntv2_hin->index);
	if (value != ntv2_hin->video_detect0) {
		ntv2_hin->video_detect0 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect1, ntv2_hin->index);
	if (value != ntv2_hin->video_detect1) {
		ntv2_hin->video_detect1 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect2, ntv2_hin->index);
	if (value != ntv2_hin->video_detect2) {
		ntv2_hin->video_detect2 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect3, ntv2_hin->index);
	if (value != ntv2_hin->video_detect3) {
		ntv2_hin->video_detect3 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect4, ntv2_hin->index);
	if (value != ntv2_hin->video_detect4) {
		ntv2_hin->video_detect4 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect5, ntv2_hin->index);
	if (value != ntv2_hin->video_detect5) {
		ntv2_hin->video_detect5 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect6, ntv2_hin->index);
	if (value != ntv2_hin->video_detect6) {
		ntv2_hin->video_detect6 = value;
		changed = true;
	}
	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_videodetect7, ntv2_hin->index);
	if (value != ntv2_hin->video_detect7) {
		ntv2_hin->video_detect7 = value;
		changed = true;
	}

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin4_tmdsclockfrequency, 0);
	value = (value < 50000000)? (value & 0xfffff000) : (value & 0xffffc000);
	if (value != ntv2_hin->tmds_rate) {
		ntv2_hin->tmds_rate = value;
		changed = true;
	}

	return changed;
}

static bool has_audio_input_changed(struct ntv2_hdmiin4 *ntv2_hin)
{
#if defined (MSWindows) || defined (AJALinux)
	UNREFERENCED_PARAMETER(ntv2_hin);
#endif

	return false;
}

bool update_input_state(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	struct ntv2_hdmi_clock_data* clock_data;
	struct ntv2_hdmi_format_data* format_data;
	unsigned long flags;
	u32 value;
	u32 mask;
	u32 line_rate;
	u32 color_depth;
	u32 color_space;
	u32 interface;
	u32 h_sync_start;
	u32 h_sync_end;
	u32 h_de_start;
	u32 h_total;
	u32 v_trans_f1;
	u32 v_trans_f2;
	u32 v_sync_start_f1;
	u32 v_sync_end_f1;
	u32 v_de_start_f1;
	u32 v_de_start_f2;
	u32 v_sync_start_f2;
	u32 v_sync_end_f2;
	u32 v_total_f1;
	u32 v_total_f2;

	bool input_locked = false;
	bool hdmi_mode = false;
	u32 video_rgb = 0;
	u32 video_deep = 0;
	u32 video_standard = 0;
	u32 video_prog = 0;
	u32 video_sd = 0;
	u32 frame_rate = 0;
	u32 f_flags = 0;
	u32 p_flags = 0;
	u32 a_detect = 0;

	// read hardware input state
	line_rate = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videocontrol_linerate, ntv2_hin->video_control);

	NTV2_MSG_HDMIIN_DETECT("%s: clock  line %d  tmds %d\n", 
						   ntv2_hin->name, line_rate, ntv2_hin->tmds_rate);

	// find clock rate type base on hardware data
	clock_data = find_clock_data(line_rate, ntv2_hin->tmds_rate);
	if (clock_data == NULL)	{
		if (ntv2_hin->format_clock_count < 1) {
			NTV2_MSG_HDMIIN_STATE("%s: unrecognized hardware clock data\n", ntv2_hin->name);
		}
		ntv2_hin->format_clock_count++;
		set_no_video(ntv2_hin);
		return false;
	}
	ntv2_hin->format_clock_count = 0;

	color_space = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect0_colorspace, ntv2_hin->video_detect0);
	color_depth = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect0_colordepth, ntv2_hin->video_detect0);
	interface = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect0_interfacemode, ntv2_hin->video_detect0);

	h_sync_start = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect1_hsyncstart, ntv2_hin->video_detect1);
	h_sync_end = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect1_hsyncend, ntv2_hin->video_detect1);
	
	h_de_start = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect2_hdestart, ntv2_hin->video_detect2);
	h_total = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect2_htotal, ntv2_hin->video_detect2);

	v_trans_f1 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect3_vtransf1, ntv2_hin->video_detect3);
	v_trans_f2 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect3_vtransf2, ntv2_hin->video_detect3);

	v_sync_start_f1 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect4_vsyncstartf1, ntv2_hin->video_detect4);
	v_sync_end_f1 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect4_vsyncendf1, ntv2_hin->video_detect4);

	v_de_start_f1 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect5_vdestartf1, ntv2_hin->video_detect5);
	v_de_start_f2 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect5_vdestartf2, ntv2_hin->video_detect5);

	v_sync_start_f2 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect6_vsyncstartf2, ntv2_hin->video_detect6);
	v_sync_end_f2 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect6_vsyncendf2, ntv2_hin->video_detect6);

	v_total_f1 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect7_vtotalf1, ntv2_hin->video_detect7);
	v_total_f2 = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_videodetect7_vtotalf2, ntv2_hin->video_detect7);

	NTV2_MSG_HDMIIN_DETECT("%s: detect  cs %d  cd %d  dvi %d\n", 
						   ntv2_hin->name, color_space, color_depth, interface);
	NTV2_MSG_HDMIIN_DETECT("%s: detect  hss %d  hse %d  hds %d  ht %d\n", 
						   ntv2_hin->name, h_sync_start, h_sync_end, h_de_start, h_total);
	NTV2_MSG_HDMIIN_DETECT("%s: detect  vtr1 %d  vtr2 %d  vss1 %d  vse1 %d\n", 
						   ntv2_hin->name, v_trans_f1, v_trans_f2, v_sync_start_f1, v_sync_end_f1);
	NTV2_MSG_HDMIIN_DETECT("%s: detect  vds1 %d  vds2 %d  vss2 %d  vse2 %d\n", 
						   ntv2_hin->name, v_de_start_f1, v_de_start_f2, v_sync_start_f2, v_sync_end_f2);
	NTV2_MSG_HDMIIN_DETECT("%s: detect  vtot1 %d  vtot2 %d\n", 
						   ntv2_hin->name, v_total_f1, v_total_f2);

	// find the format based on the hardware registers
	format_data = find_format_data(h_sync_start,
								   h_sync_end,
								   h_de_start,
								   h_total,
								   v_trans_f1,
								   v_trans_f2,
								   v_sync_start_f1,
								   v_sync_end_f1,
								   v_de_start_f1,
								   v_de_start_f2,
								   v_sync_start_f2,
								   v_sync_end_f2,
								   v_total_f1,
								   v_total_f2,
								   clock_data->clock_type);
	if (format_data == NULL) {
		if (ntv2_hin->format_raster_count < 1) {
			NTV2_MSG_HDMIIN_STATE("%s: unrecognized hardware raster data\n", ntv2_hin->name);
		}
		ntv2_hin->format_raster_count++;
		set_no_video(ntv2_hin);
		return false;
	}
	ntv2_hin->format_raster_count = 0;

	// get video data
	input_locked = true;
	hdmi_mode = (interface == ntv2_kona_con_hdmiin4_interfacemode_hdmi);
	video_standard = format_data->video_standard;
	frame_rate = format_data->frame_rate;
	video_prog = ntv2_video_standard_progressive(video_standard);
	video_rgb = (color_space == ntv2_kona_color_space_rgb444)? 1 : 0;
	video_sd = ((video_standard == ntv2_kona_video_standard_525i) || (video_standard == ntv2_kona_video_standard_625i))? 1 : 0;
	video_deep = ((video_rgb == 1) && (color_depth != ntv2_kona_color_depth_8bit))? 1 : 0;

	// check to do 420 conversions
	if (color_space == ntv2_kona_color_space_yuv420)
	{
		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_420mode, ntv2_kona_con_hdmiin4_420mode_enable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_420mode);
		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_420convert, ntv2_kona_con_hdmiin4_420convert_enable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_420convert);
		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_pixelcontrol, ntv2_hin->index, value, mask);
	}
	else
	{
		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_420mode, ntv2_kona_con_hdmiin4_420mode_disable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_420mode);
		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_420convert, ntv2_kona_con_hdmiin4_420convert_disable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_420convert);
		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_pixelcontrol, ntv2_hin->index, value, mask);
	}

	// check to do 4K conversions
	if (ntv2_video_standard_width(video_standard) > 2048)
	{
		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_hsyncdivide, ntv2_kona_con_hdmiin4_hsyncdivide_none);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_hsyncdivide);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_pixelsperclock, 4);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_pixelsperclock);

		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_hlinefilter, ntv2_kona_con_hdmiin4_hlinefilter_disable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_hlinefilter);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_clockratio, format_data->clock_ratio);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_clockratio);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_lineinterleave, ntv2_kona_con_hdmiin4_lineinterleave_enable);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_lineinterleave);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_pixelinterleave, ntv2_kona_con_hdmiin4_pixelinterleave_enable);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_pixelinterleave);

		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_pixelcontrol, ntv2_hin->index, value, mask);
	}
	else
	{
		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_hsyncdivide, ntv2_kona_con_hdmiin4_hsyncdivide_none);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_hsyncdivide);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_pixelsperclock, 1);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_pixelsperclock);

		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

		value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_hlinefilter, ntv2_kona_con_hdmiin4_hlinefilter_enable);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_hlinefilter);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_clockratio, format_data->clock_ratio);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_clockratio);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_lineinterleave, ntv2_kona_con_hdmiin4_lineinterleave_disable);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_lineinterleave);

		value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_pixelinterleave, ntv2_kona_con_hdmiin4_pixelinterleave_disable);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_pixelinterleave);

		ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_pixelcontrol, ntv2_hin->index, value, mask);
	}

	// disable crop
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_croplocation_start, 0x040);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_croplocation_end, 0x7bf);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin4_croplocation, ntv2_hin->index, value);

	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_pixelcontrol_cropmode, ntv2_kona_con_hdmiin4_cropmode_disable);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_pixelcontrol_cropmode);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_pixelcontrol, ntv2_hin->index, value, mask);

	// write input format
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_locked, input_locked? 1 : 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_stable, input_locked? 1 : 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_rgb, video_rgb);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_deep_color, video_deep);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_code, video_standard);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_8ch, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_progressive, video_prog);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_sd, video_sd);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_74_25, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_rate, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_word_length, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_format, video_standard);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_dvi, (hdmi_mode? 0 : 1));
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_rate, frame_rate);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin4_input_status, ntv2_hin->index, value);

	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_color_space, color_space);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_color_space);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_color_depth, color_depth);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_color_depth);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmi4_control, ntv2_hin->index, value, mask);

//	if ((ntv2_hin->input_locked != input_locked) ||
//		(ntv2_hin->hdmi_mode != hdmi_mode) ||
//		(ntv2_hin->video_standard != video_standard) ||
//		(ntv2_hin->frame_rate != frame_rate) ||
//		(ntv2_hin->color_space != color_space) ||
//		(ntv2_hin->color_depth != color_depth)) 
	{
		NTV2_MSG_HDMIIN_STATE("%s: new format  mode %s  std %s  rate %s  clr %s  dpth %s\n",
							  ntv2_hin->name,
							  hdmi_mode? "hdmi" : "dvi",
							  ntv2_video_standard_name(video_standard),
							  ntv2_frame_rate_name(frame_rate),
							  ntv2_color_space_name(color_space),
							  ntv2_color_depth_name(color_depth));

		ntv2_hin->input_locked = input_locked;
		ntv2_hin->hdmi_mode = hdmi_mode;
		ntv2_hin->video_standard = video_standard;
		ntv2_hin->frame_rate = frame_rate;
		ntv2_hin->color_space = color_space;
		ntv2_hin->color_depth = color_depth;
	}

	/* set frame progressive/interlace flags */
	if (ntv2_video_standard_progressive(video_standard)) {
		f_flags = ntv2_kona_frame_picture_progressive | ntv2_kona_frame_transport_progressive;
	} else {
		f_flags = ntv2_kona_frame_picture_interlaced | ntv2_kona_frame_transport_interlaced;
	}

	/* use line rate to set frame rate class */
	switch (line_rate) {
	case ntv2_kona_con_hdmiin4_linerate_5940mhz:
	case ntv2_kona_con_hdmiin4_linerate_4455mhz:
	case ntv2_kona_con_hdmiin4_linerate_3712mhz:
		f_flags |= ntv2_kona_frame_12g;
		break;
	case ntv2_kona_con_hdmiin4_linerate_2970mhz:
	case ntv2_kona_con_hdmiin4_linerate_2227mhz:
	case ntv2_kona_con_hdmiin4_linerate_1856mhz:
		f_flags |= ntv2_kona_frame_6g;
		break;
	case ntv2_kona_con_hdmiin4_linerate_1485mhz:
	case ntv2_kona_con_hdmiin4_linerate_1113mhz:
	case ntv2_kona_con_hdmiin4_linerate_928mhz:
		f_flags |= ntv2_kona_frame_3g;
		break;
	case ntv2_kona_con_hdmiin4_linerate_742mhz:
	case ntv2_kona_con_hdmiin4_linerate_556mhz:
	case ntv2_kona_con_hdmiin4_linerate_540mhz:
	case ntv2_kona_con_hdmiin4_linerate_405mhz:
	case ntv2_kona_con_hdmiin4_linerate_337mhz:
		f_flags |= ntv2_kona_frame_hd;
		break;
	case ntv2_kona_con_hdmiin4_linerate_270mhz:
	case ntv2_kona_con_hdmiin4_linerate_250mhz:
		f_flags |= ntv2_kona_frame_sd;
		break;
	default:
		break;
	}

	/* color component */
	switch (color_space) {
	case ntv2_kona_color_space_yuv422:
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_422;
		break;
	case ntv2_kona_color_space_rgb444:
		p_flags |= ntv2_kona_pixel_rgb | ntv2_kona_pixel_444;
		break;
	case ntv2_kona_color_space_yuv444:
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_444;
		break;
	case ntv2_kona_color_space_yuv420:
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_420;
		break;
	default:
		break;
	}

	/* scan for colorimetry (simple for now) */
	if ((f_flags & ntv2_kona_frame_sd) != 0) {
		p_flags |= ntv2_kona_pixel_rec601;
	} else {
		p_flags |= ntv2_kona_pixel_rec709;
	}

	/* scan for black/white range (simple for now) */
	if ((p_flags & ntv2_kona_pixel_rgb) != 0) {
		p_flags |= ntv2_kona_pixel_full;
	} else {
		p_flags |= ntv2_kona_pixel_smpte;
	}
	
	/* scan for aspect ratio ( simple for now) */
	if ((f_flags & ntv2_kona_frame_sd) != 0) {
		f_flags |= ntv2_kona_frame_4x3;
	} else {
		f_flags |= ntv2_kona_frame_16x9;
	}

	/* color depth */
	switch (color_depth) {
	case ntv2_kona_color_depth_8bit:
		p_flags |= ntv2_kona_pixel_8bit;
		break;
	case ntv2_kona_color_depth_10bit:
		p_flags |= ntv2_kona_pixel_10bit;
		break;
	case ntv2_kona_color_depth_12bit:
		p_flags |= ntv2_kona_pixel_12bit;
		break;
	default:
		break;
	}

	/* audio 8 channels */
	a_detect = hdmi_mode? 0xf : 0x0;
	
	spin_lock_irqsave(&ntv2_hin->state_lock, flags);
	ntv2_hin->input_format.video_standard = video_standard;
	ntv2_hin->input_format.frame_rate = frame_rate;
	ntv2_hin->input_format.frame_flags = f_flags;
	ntv2_hin->input_format.pixel_flags = p_flags;
	ntv2_hin->input_format.audio_detect = a_detect;
	spin_unlock_irqrestore(&ntv2_hin->state_lock, flags);

	return true;
}

static bool has_audio_control_changed(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 value = 0;
	bool changed = false;
	bool audio_swap;

	value =  ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmi_control, ntv2_hin->index);
	audio_swap = NTV2_FLD_GET(ntv2_kona_fld_hdmiin4_chn34_swap_disable, value) == 0;

	if (audio_swap != ntv2_hin->audio_swap) {
		ntv2_hin->audio_swap = audio_swap;
		changed = true;
	}

	return changed;
}

static bool config_audio_control(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	u32 swap = ntv2_hin->audio_swap? ntv2_kona_con_hdmiin4_audioswapmode_enable : ntv2_kona_con_hdmiin4_audioswapmode_disable;
	u32 value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_videocontrol_audioswapmode, swap);
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_videocontrol_audioswapmode);

	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmiin4_videocontrol, ntv2_hin->index, value, mask);

	NTV2_MSG_HDMIIN_STATE("%s: new control  audio swap %s\n", ntv2_hin->name, ntv2_hin->audio_swap? "enable":"disable");

	return true;
}
	
static void set_no_video(struct ntv2_hdmiin4 *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	unsigned long flags;
	u32 value;
	u32 mask;

	/* clear fpga hdmi status */
	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_locked, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_stable, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_rgb, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_deep_color, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_code, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_8ch, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_progressive, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_sd, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_74_25, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_rate, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_audio_word_length, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_format, 0);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_dvi, 1);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_video_rate, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin4_input_status, ntv2_hin->index, value);

	value = NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_color_space, 0);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_color_space);
	value |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin4_color_depth, 0);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_hdmiin4_color_depth);
	ntv2_reg_rmw(vid_reg, ntv2_kona_reg_hdmi4_control, ntv2_hin->index, value, mask);

	ntv2_hin->input_locked		= false;
	ntv2_hin->hdmi_mode			= false;
	ntv2_hin->video_standard	= ntv2_kona_video_standard_none;
	ntv2_hin->frame_rate		= ntv2_kona_frame_rate_none;
	ntv2_hin->color_space		= ntv2_kona_color_space_none;
	ntv2_hin->color_depth		= ntv2_kona_color_depth_none;

	spin_lock_irqsave(&ntv2_hin->state_lock, flags);
	ntv2_hin->input_format.video_standard = ntv2_kona_video_standard_none;
	ntv2_hin->input_format.frame_rate = ntv2_kona_frame_rate_none;
	ntv2_hin->input_format.frame_flags = 0;
	ntv2_hin->input_format.pixel_flags = 0;
	ntv2_hin->input_format.audio_detect = 0;
	spin_unlock_irqrestore(&ntv2_hin->state_lock, flags);
}

static struct ntv2_hdmi_format_data* find_format_data(u32 h_sync_start,
													  u32 h_sync_end,
													  u32 h_de_start,
													  u32 h_total,
													  u32 v_trans_f1,
													  u32 v_trans_f2,
													  u32 v_sync_start_f1,
													  u32 v_sync_end_f1,
													  u32 v_de_start_f1,
													  u32 v_de_start_f2,
													  u32 v_sync_start_f2,
													  u32 v_sync_end_f2,
													  u32 v_total_f1,
													  u32 v_total_f2,
													  enum ntv2_hdmi_clock_type clock_type)
{
	int i = 0;
	while (c_hdmi_format_data[i].video_standard != ntv2_kona_video_standard_none)
	{
		if (
			(h_sync_start == c_hdmi_format_data[i].h_sync_start) &&
			(h_sync_end == c_hdmi_format_data[i].h_sync_end) &&
			(h_de_start == c_hdmi_format_data[i].h_de_start) &&
			(h_total == c_hdmi_format_data[i].h_total) &&
			(v_trans_f1 == c_hdmi_format_data[i].v_trans_f1) &&
			(v_trans_f2 == c_hdmi_format_data[i].v_trans_f2) &&
			(v_sync_start_f1 == c_hdmi_format_data[i].v_sync_start_f1) &&
			(v_sync_end_f1 == c_hdmi_format_data[i].v_sync_end_f1) &&
			(v_de_start_f1 == c_hdmi_format_data[i].v_de_start_f1) &&
			(v_de_start_f2 == c_hdmi_format_data[i].v_de_start_f2) &&
			(v_sync_start_f2 == c_hdmi_format_data[i].v_sync_start_f2) &&
			(v_sync_end_f2 == c_hdmi_format_data[i].v_sync_end_f2) &&
			(v_total_f1 == c_hdmi_format_data[i].v_total_f1) &&
			(v_total_f2 == c_hdmi_format_data[i].v_total_f2) &&
			(clock_type == c_hdmi_format_data[i].clock_type))
		{
			return &c_hdmi_format_data[i];
		}
		i++;
	}

	return NULL;
}

static struct ntv2_hdmi_clock_data* find_clock_data(u32 lineRate, u32 tmdsRate)
{
	int i = 0;
	while (c_hdmi_clock_data[i].clock_type != ntv2_clock_type_unknown)
	{
		if ((lineRate == c_hdmi_clock_data[i].line_rate) &&
			compare_tmds_rate(tmdsRate, c_hdmi_clock_data[i].tmds_rate))
		{
			return &c_hdmi_clock_data[i];
		}
		i++;
	}

	return NULL;
}

static bool compare_tmds_rate(u32 tmdsRate, u32 tmdsRef)
{
	u32 tol = 15000;
	if (tmdsRef < 50000) tol = 10000;

	if ((tmdsRate > (tmdsRef - tol)) &&
		(tmdsRate < (tmdsRef + tol)))
		return true;

	return false;
}

