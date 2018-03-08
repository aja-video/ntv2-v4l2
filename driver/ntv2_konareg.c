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

#define NTV2_REG_CONST
#include "ntv2_konareg.h"
#undef NTV2_REG_CONST
#include "ntv2_register.h"

static u32 audio_cadence_48[NTV2_MAX_FRAME_RATES][5] = {
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 },
	/* ntv2_kona_frame_rate_6000 */		{  800,  800,  800,  800,  800 },
	/* ntv2_kona_frame_rate_5994 */		{  800,  801,  801,  801,  801 },
	/* ntv2_kona_frame_rate_3000 */		{ 1600, 1600, 1600, 1600, 1600 },
	/* ntv2_kona_frame_rate_2997 */		{ 1602, 1601, 1602, 1601, 1602 },
	/* ntv2_kona_frame_rate_2500 */		{ 1920, 1920, 1920, 1920, 1920 },
	/* ntv2_kona_frame_rate_2400 */		{ 2000, 2000, 2000, 2000, 2000 },
	/* ntv2_kona_frame_rate_2398 */		{ 2002, 2002, 2002, 2002, 2002 },
	/* ntv2_kona_frame_rate_5000 */		{  960,  960,  960,  960,  960 },
	/* ntv2_kona_frame_rate_4800 */		{ 1000, 1000, 1000, 1000, 1000 },
	/* ntv2_kona_frame_rate_4795 */		{ 1001, 1001, 1001, 1001, 1001 },
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 },
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 },
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 },
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 },
	/* ntv2_kona_frame_rate_unknown */	{    0,    0,    0,    0,    0 }
};

u32 ntv2_audio_frame_samples(u32 frame_rate, u32 cadence)
{
	if (frame_rate >= NTV2_MAX_FRAME_RATES)
		return 0;

	return audio_cadence_48[frame_rate][cadence%5];
}

static u32 frame_fraction[NTV2_MAX_FRAME_RATES][2] = {
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 },
	/* ntv2_kona_frame_rate_6000 */		{    1,    60 },
	/* ntv2_kona_frame_rate_5994 */		{ 1001, 60000 },
	/* ntv2_kona_frame_rate_3000 */		{    1,    30 },
	/* ntv2_kona_frame_rate_2997 */		{ 1001, 30000 },
	/* ntv2_kona_frame_rate_2500 */		{    1,    25 },
	/* ntv2_kona_frame_rate_2400 */		{    1,    24 },
	/* ntv2_kona_frame_rate_2398 */		{ 1001, 24000 },
	/* ntv2_kona_frame_rate_5000 */		{    1,    50 },
	/* ntv2_kona_frame_rate_4800 */		{    1,    48 },
	/* ntv2_kona_frame_rate_4795 */		{ 1001, 48000 },
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 },
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 },
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 },
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 },
	/* ntv2_kona_frame_rate_unknown */	{    1,     1 }
};

u32 ntv2_frame_rate_duration(u32 frame_rate)
{
	if (frame_rate >= NTV2_MAX_FRAME_RATES)
		return 1;

	return frame_fraction[frame_rate][0];
}

u32 ntv2_frame_rate_scale(u32 frame_rate)
{
	if (frame_rate >= NTV2_MAX_FRAME_RATES)
		return 1;

	return frame_fraction[frame_rate][1];
}

static u32 frame_geometry_dimension[NTV2_MAX_FRAME_GEOMETRIES][2] = {
	/* ntv2_kona_frame_geometry_1920x1080 */	{ 1920, 1080 },
	/* ntv2_kona_frame_geometry_1280x720 */		{ 1280,  720 },
	/* ntv2_kona_frame_geometry_720x486 */		{  720,  486 },
	/* ntv2_kona_frame_geometry_720x576 */		{  720,  576 },
	/* ntv2_kona_frame_geometry_1920x1114 */	{ 1920, 1114 },
	/* ntv2_kona_frame_geometry_2048x1114 */	{ 2048, 1114 },
	/* ntv2_kona_frame_geometry_720x508 */		{  720,  508 },
	/* ntv2_kona_frame_geometry_720x598 */		{  720,  598 },
	/* ntv2_kona_frame_geometry_1920x1112 */	{ 1920, 1112 },
	/* ntv2_kona_frame_geometry_1280x740 */		{ 1280,  740 },
	/* ntv2_kona_frame_geometry_2048x1080 */	{ 2048, 1080 },
	/* ntv2_kona_frame_geometry_2048x1556 */	{ 2048, 1556 },
	/* ntv2_kona_frame_geometry_2048x1588 */	{ 2048, 1588 },
	/* ntv2_kona_frame_geometry_2048x1112 */	{ 2048, 1112 },
	/* ntv2_kona_frame_geometry_720x514 */		{  720,  514 },
	/* ntv2_kona_frame_geometry_720x612 */		{  720,  612 },
	/* ntv2_kona_frame_geometry_4x1920x1080 */	{ 3840, 2160 },
	/* ntv2_kona_frame_geometry_4x2048x1080 */	{ 4096, 2160 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 },
	/* ntv2_kona_frame_geometry_unknown */		{    0,    0 }
};

u32 ntv2_frame_geometry_width(u32 frame_geometry)
{
	if (frame_geometry >= NTV2_MAX_FRAME_GEOMETRIES)
		return 0;

	return frame_geometry_dimension[frame_geometry][0];
}

u32 ntv2_frame_geometry_height(u32 frame_geometry)
{
	if (frame_geometry >= NTV2_MAX_FRAME_GEOMETRIES)
		return 0;

	return frame_geometry_dimension[frame_geometry][1];
}

static u32 video_standard_dimension[NTV2_MAX_VIDEO_STANDARDS][3] = {
	/* ntv2_kona_video_standard_1080i */		{ 1920, 1080, 0 },
	/* ntv2_kona_video_standard_720p */			{ 1280,  720, 1 },
	/* ntv2_kona_video_standard_525i */			{ 720,   486, 0 },
	/* ntv2_kona_video_standard_625i */			{ 720,   576, 0 },
	/* ntv2_kona_video_standard_1080p */		{ 1920, 1080, 1 },
	/* ntv2_kona_video_standard_2048x1556 */	{ 2048, 1556, 0 },
	/* ntv2_kona_video_standard_2048x1080p */	{ 2048, 1080, 1 },
	/* ntv2_kona_video_standard_2048x1080i */	{ 2048, 1080, 0 },
	/* ntv2_kona_video_standard_3840x2160p */	{ 3840, 2160, 1 },
	/* ntv2_kona_video_standard_4096x2160p */	{ 4096, 2160, 1 },
	/* ntv2_kona_video_standard_3840_hfr */		{ 3840, 2160, 1 },
	/* ntv2_kona_video_standard_4096_hfr */		{ 4096, 2160, 1 },
	/* ntv2_kona_video_standard_undefined */	{    0,    0, 0 },
	/* ntv2_kona_video_standard_undefined */	{    0,    0, 0 },
	/* ntv2_kona_video_standard_undefined */	{    0,    0, 0 },
	/* ntv2_kona_video_standard_undefined */	{    0,    0, 0 }
};

u32 ntv2_video_standard_width(u32 video_standard)
{
	if (video_standard >= NTV2_MAX_VIDEO_STANDARDS)
		return 0;

	return video_standard_dimension[video_standard][0];
}

u32 ntv2_video_standard_height(u32 video_standard)
{
	if (video_standard >= NTV2_MAX_VIDEO_STANDARDS)
		return 0;

	return video_standard_dimension[video_standard][1];
}

bool ntv2_video_standard_progressive(u32 video_standard)
{
	if (video_standard >= NTV2_MAX_VIDEO_STANDARDS)
		return 0;

	return (video_standard_dimension[video_standard][2] == 1);
}

struct video_register_data {
	u32 int_enable_reg;
	u32 int_enable_fld;
	u32 int_clear_reg;
	u32 int_clear_fld;
	u32 int_active_reg;
	u32 int_active_fld;
	u32 vid_field_reg;
	u32 vid_field_fld;
	u32 vid_transmit_reg;
	u32 vid_transmit_fld;
};

struct sdi_input_status {
	u32 video_reg;
	u32 video_geometry_b012_fld;
	u32 video_geometry_b3_fld;
	u32 video_rate_b012_fld;
	u32 video_rate_b3_fld;
	u32 video_progressive_fld;
	u32 flag_reg;
	u32 flag_3g_fld;
	u32 flag_3gb_fld;
	u32 flag_3g_b2a_convert_fld;
	u32 flag_vpid_ds1_fld;
	u32 flag_vpid_ds2_fld;
	u32 audio_reg;
	u32 audio_detect_fld;
};

struct video_field {
	u32 reg;
	u32 fld;
};


static struct video_register_data video_input_data[NTV2_MAX_CHANNELS];
static struct video_register_data video_output_data[NTV2_MAX_CHANNELS];
static struct sdi_input_status sdi_input_status[NTV2_MAX_CHANNELS];
static struct video_field video_fs_route[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static struct video_field video_csc_route[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static struct video_field video_mux_route[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_sdi_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_dl_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_csc_yuv_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_csc_rgb_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_hdmi_yuv_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_hdmi_rgb_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_mux_yuv_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_mux_rgb_source[NTV2_MAX_CHANNELS][NTV2_MAX_STREAMS];
static u32 video_standard_to_hdmi[NTV2_MAX_VIDEO_STANDARDS];
static u32 frame_rate_to_hdmi[NTV2_MAX_FRAME_RATES];
static const char *video_standard_name[NTV2_MAX_VIDEO_STANDARDS];
static const char *frame_geometry_name[NTV2_MAX_FRAME_GEOMETRIES];
static const char *input_geometry_name[NTV2_MAX_INPUT_GEOMETRIES];
static const char *frame_rate_name[NTV2_MAX_FRAME_RATES];
static const char *color_space_name[NTV2_MAX_COLOR_SPACES];
static const char *color_depth_name[NTV2_MAX_COLOR_DEPTHS];
static bool register_data_init = false;

void ntv2_kona_register_initialize(void)
{
	int i;

	if (register_data_init)
		return;

	/* organize hardware register functions by channel index */
	memset(video_input_data, 0, sizeof(video_input_data));
	video_input_data[0].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_input_data[0].int_enable_fld = ntv2_kona_fld_in1_vertical_enable;
	video_input_data[0].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_input_data[0].int_clear_fld = ntv2_kona_fld_in1_vertical_clear;
	video_input_data[0].int_active_reg = 0;
	video_input_data[0].int_active_fld = ntv2_kona_fld_in1_vertical_active;
	video_input_data[0].vid_field_reg = 0;
	video_input_data[0].vid_field_fld = ntv2_kona_fld_in1_field_id;
	video_input_data[0].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[0].vid_transmit_fld = ntv2_kona_fld_sdi1_transmit;

	video_input_data[1].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_input_data[1].int_enable_fld = ntv2_kona_fld_in2_vertical_enable;
	video_input_data[1].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_input_data[1].int_clear_fld = ntv2_kona_fld_in2_vertical_clear;
	video_input_data[1].int_active_reg = 0;
	video_input_data[1].int_active_fld = ntv2_kona_fld_in2_vertical_active;
	video_input_data[1].vid_field_reg = 0;
	video_input_data[1].vid_field_fld = ntv2_kona_fld_in2_field_id;
	video_input_data[1].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[1].vid_transmit_fld = ntv2_kona_fld_sdi2_transmit;

	video_input_data[2].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[2].int_enable_fld = ntv2_kona_fld_in3_vertical_enable;
	video_input_data[2].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[2].int_clear_fld = ntv2_kona_fld_in3_vertical_clear;
	video_input_data[2].int_active_reg = 1;
	video_input_data[2].int_active_fld = ntv2_kona_fld_in3_vertical_active;
	video_input_data[2].vid_field_reg = 1;
	video_input_data[2].vid_field_fld = ntv2_kona_fld_in3_field_id;
	video_input_data[2].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[2].vid_transmit_fld = ntv2_kona_fld_sdi3_transmit;

	video_input_data[3].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[3].int_enable_fld = ntv2_kona_fld_in4_vertical_enable;
	video_input_data[3].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[3].int_clear_fld = ntv2_kona_fld_in4_vertical_clear;
	video_input_data[3].int_active_reg = 1;
	video_input_data[3].int_active_fld = ntv2_kona_fld_in4_vertical_active;
	video_input_data[3].vid_field_reg = 1;
	video_input_data[3].vid_field_fld = ntv2_kona_fld_in4_field_id;
	video_input_data[3].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[3].vid_transmit_fld = ntv2_kona_fld_sdi4_transmit;

	video_input_data[4].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[4].int_enable_fld = ntv2_kona_fld_in5_vertical_enable;
	video_input_data[4].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[4].int_clear_fld = ntv2_kona_fld_in5_vertical_clear;
	video_input_data[4].int_active_reg = 1;
	video_input_data[4].int_active_fld = ntv2_kona_fld_in5_vertical_active;
	video_input_data[4].vid_field_reg = 1;
	video_input_data[4].vid_field_fld = ntv2_kona_fld_in5_field_id;
	video_input_data[4].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[4].vid_transmit_fld = ntv2_kona_fld_sdi5_transmit;

	video_input_data[5].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[5].int_enable_fld = ntv2_kona_fld_in6_vertical_enable;
	video_input_data[5].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[5].int_clear_fld = ntv2_kona_fld_in6_vertical_clear;
	video_input_data[5].int_active_reg = 1;
	video_input_data[5].int_active_fld = ntv2_kona_fld_in6_vertical_active;
	video_input_data[5].vid_field_reg = 1;
	video_input_data[5].vid_field_fld = ntv2_kona_fld_in6_field_id;
	video_input_data[5].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[5].vid_transmit_fld = ntv2_kona_fld_sdi6_transmit;

	video_input_data[6].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[6].int_enable_fld = ntv2_kona_fld_in7_vertical_enable;
	video_input_data[6].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[6].int_clear_fld = ntv2_kona_fld_in7_vertical_clear;
	video_input_data[6].int_active_reg = 1;
	video_input_data[6].int_active_fld = ntv2_kona_fld_in7_vertical_active;
	video_input_data[6].vid_field_reg = 1;
	video_input_data[6].vid_field_fld = ntv2_kona_fld_in7_field_id;
	video_input_data[6].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[6].vid_transmit_fld = ntv2_kona_fld_sdi7_transmit;

	video_input_data[7].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[7].int_enable_fld = ntv2_kona_fld_in8_vertical_enable;
	video_input_data[7].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_input_data[7].int_clear_fld = ntv2_kona_fld_in8_vertical_clear;
	video_input_data[7].int_active_reg = 1;
	video_input_data[7].int_active_fld = ntv2_kona_fld_in8_vertical_active;
	video_input_data[7].vid_field_reg = 1;
	video_input_data[7].vid_field_fld = ntv2_kona_fld_in8_field_id;
	video_input_data[7].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_input_data[7].vid_transmit_fld = ntv2_kona_fld_sdi8_transmit;

	memset(video_output_data, 0, sizeof(video_output_data));
	video_output_data[0].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[0].int_enable_fld = ntv2_kona_fld_out1_vertical_enable;
	video_output_data[0].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[0].int_clear_fld = ntv2_kona_fld_out1_vertical_clear;
	video_output_data[0].int_active_reg = 0;
	video_output_data[0].int_active_fld = ntv2_kona_fld_out1_vertical_active;
	video_output_data[0].vid_field_reg = 0;
	video_output_data[0].vid_field_fld = ntv2_kona_fld_out1_field_id;
	video_output_data[0].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[0].vid_transmit_fld = ntv2_kona_fld_sdi1_transmit;

	video_output_data[1].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[1].int_enable_fld = ntv2_kona_fld_out2_vertical_enable;
	video_output_data[1].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[1].int_clear_fld = ntv2_kona_fld_out2_vertical_clear;
	video_output_data[1].int_active_reg = 0;
	video_output_data[1].int_active_fld = ntv2_kona_fld_out2_vertical_active;
	video_output_data[1].vid_field_reg = 0;
	video_output_data[1].vid_field_fld = ntv2_kona_fld_out2_field_id;
	video_output_data[1].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[1].vid_transmit_fld = ntv2_kona_fld_sdi2_transmit;

	video_output_data[2].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[2].int_enable_fld = ntv2_kona_fld_out3_vertical_enable;
	video_output_data[2].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[2].int_clear_fld = ntv2_kona_fld_out3_vertical_clear;
	video_output_data[2].int_active_reg = 0;
	video_output_data[2].int_active_fld = ntv2_kona_fld_out3_vertical_active;
	video_output_data[2].vid_field_reg = 0;
	video_output_data[2].vid_field_fld = ntv2_kona_fld_out3_field_id;
	video_output_data[2].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[2].vid_transmit_fld = ntv2_kona_fld_sdi3_transmit;

	video_output_data[3].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[3].int_enable_fld = ntv2_kona_fld_out4_vertical_enable;
	video_output_data[3].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control, 0);
	video_output_data[3].int_clear_fld = ntv2_kona_fld_out4_vertical_clear;
	video_output_data[3].int_active_reg = 0;
	video_output_data[3].int_active_fld = ntv2_kona_fld_out4_vertical_active;
	video_output_data[3].vid_field_reg = 0;
	video_output_data[3].vid_field_fld = ntv2_kona_fld_out4_field_id;
	video_output_data[3].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[3].vid_transmit_fld = ntv2_kona_fld_sdi4_transmit;

	video_output_data[4].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[4].int_enable_fld = ntv2_kona_fld_out5_vertical_enable;
	video_output_data[4].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[4].int_clear_fld = ntv2_kona_fld_out5_vertical_clear;
	video_output_data[4].int_active_reg = 1;
	video_output_data[4].int_active_fld = ntv2_kona_fld_out5_vertical_active;
	video_output_data[4].vid_field_reg = 1;
	video_output_data[4].vid_field_fld = ntv2_kona_fld_out5_field_id;
	video_output_data[4].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[4].vid_transmit_fld = ntv2_kona_fld_sdi5_transmit;

	video_output_data[5].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[5].int_enable_fld = ntv2_kona_fld_out6_vertical_enable;
	video_output_data[5].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[5].int_clear_fld = ntv2_kona_fld_out6_vertical_clear;
	video_output_data[5].int_active_reg = 1;
	video_output_data[5].int_active_fld = ntv2_kona_fld_out6_vertical_active;
	video_output_data[5].vid_field_reg = 1;
	video_output_data[5].vid_field_fld = ntv2_kona_fld_out6_field_id;
	video_output_data[5].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[5].vid_transmit_fld = ntv2_kona_fld_sdi6_transmit;

	video_output_data[6].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[6].int_enable_fld = ntv2_kona_fld_out7_vertical_enable;
	video_output_data[6].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[6].int_clear_fld = ntv2_kona_fld_out7_vertical_clear;
	video_output_data[6].int_active_reg = 1;
	video_output_data[6].int_active_fld = ntv2_kona_fld_out7_vertical_active;
	video_output_data[6].vid_field_reg = 1;
	video_output_data[6].vid_field_fld = ntv2_kona_fld_out7_field_id;
	video_output_data[6].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[6].vid_transmit_fld = ntv2_kona_fld_sdi7_transmit;

	video_output_data[7].int_enable_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[7].int_enable_fld = ntv2_kona_fld_out8_vertical_enable;
	video_output_data[7].int_clear_reg = NTV2_REG_NUM(ntv2_kona_reg_interrupt_control2, 0);
	video_output_data[7].int_clear_fld = ntv2_kona_fld_out8_vertical_clear;
	video_output_data[7].int_active_reg = 1;
	video_output_data[7].int_active_fld = ntv2_kona_fld_out8_vertical_active;
	video_output_data[7].vid_field_reg = 1;
	video_output_data[7].vid_field_fld = ntv2_kona_fld_out8_field_id;
	video_output_data[7].vid_transmit_reg = NTV2_REG_NUM(ntv2_kona_reg_sdi_transmit_control, 0);
	video_output_data[7].vid_transmit_fld = ntv2_kona_fld_sdi8_transmit;

	/* organize sdi input status by input index */
	memset(sdi_input_status, 0, sizeof(sdi_input_status));
	sdi_input_status[0].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status, 0);
	sdi_input_status[0].video_geometry_b012_fld = ntv2_kona_fld_sdiin1_geometry_b012;
	sdi_input_status[0].video_geometry_b3_fld = ntv2_kona_fld_sdiin1_geometry_b3;
	sdi_input_status[0].video_rate_b012_fld = ntv2_kona_fld_sdiin1_frame_rate_b012;
	sdi_input_status[0].video_rate_b3_fld = ntv2_kona_fld_sdiin1_frame_rate_b3;
	sdi_input_status[0].video_progressive_fld = ntv2_kona_fld_sdiin1_progressive;
	sdi_input_status[0].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status, 0);
	sdi_input_status[0].flag_3g_fld = ntv2_kona_fld_sdiin1_3g_mode;
	sdi_input_status[0].flag_3gb_fld = ntv2_kona_fld_sdiin1_3gb_mode;
	sdi_input_status[0].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin1_3g_b2a_convert;
	sdi_input_status[0].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin1_vpid_linka_valid;
	sdi_input_status[0].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin1_vpid_linkb_valid;
	sdi_input_status[0].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect, 0);
	sdi_input_status[0].audio_detect_fld = ntv2_kona_fld_sdiin1_audio_detect;

	sdi_input_status[1].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status, 0);
	sdi_input_status[1].video_geometry_b012_fld = ntv2_kona_fld_sdiin2_geometry_b012;
	sdi_input_status[1].video_geometry_b3_fld = ntv2_kona_fld_sdiin2_geometry_b3;
	sdi_input_status[1].video_rate_b012_fld = ntv2_kona_fld_sdiin2_frame_rate_b012;
	sdi_input_status[1].video_rate_b3_fld = ntv2_kona_fld_sdiin2_frame_rate_b3;
	sdi_input_status[1].video_progressive_fld = ntv2_kona_fld_sdiin2_progressive;
	sdi_input_status[1].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status, 0);
	sdi_input_status[1].flag_3g_fld = ntv2_kona_fld_sdiin2_3g_mode;
	sdi_input_status[1].flag_3gb_fld = ntv2_kona_fld_sdiin2_3gb_mode;
	sdi_input_status[1].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin2_3g_b2a_convert;
	sdi_input_status[1].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin2_vpid_linka_valid;
	sdi_input_status[1].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin2_vpid_linkb_valid;
	sdi_input_status[1].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect, 0);
	sdi_input_status[1].audio_detect_fld = ntv2_kona_fld_sdiin2_audio_detect;

	sdi_input_status[2].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status2, 0);
	sdi_input_status[2].video_geometry_b012_fld = ntv2_kona_fld_sdiin3_geometry_b012;
	sdi_input_status[2].video_geometry_b3_fld = ntv2_kona_fld_sdiin3_geometry_b3;
	sdi_input_status[2].video_rate_b012_fld = ntv2_kona_fld_sdiin3_frame_rate_b012;
	sdi_input_status[2].video_rate_b3_fld = ntv2_kona_fld_sdiin3_frame_rate_b3;
	sdi_input_status[2].video_progressive_fld = ntv2_kona_fld_sdiin3_progressive;
	sdi_input_status[2].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status2, 0);
	sdi_input_status[2].flag_3g_fld = ntv2_kona_fld_sdiin3_3g_mode;
	sdi_input_status[2].flag_3gb_fld = ntv2_kona_fld_sdiin3_3gb_mode;
	sdi_input_status[2].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin3_3g_b2a_convert;
	sdi_input_status[2].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin3_vpid_linka_valid;
	sdi_input_status[2].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin3_vpid_linkb_valid;
	sdi_input_status[2].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect2, 0);
	sdi_input_status[2].audio_detect_fld = ntv2_kona_fld_sdiin3_audio_detect;

	sdi_input_status[3].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status2, 0);
	sdi_input_status[3].video_geometry_b012_fld = ntv2_kona_fld_sdiin4_geometry_b012;
	sdi_input_status[3].video_geometry_b3_fld = ntv2_kona_fld_sdiin4_geometry_b3;
	sdi_input_status[3].video_rate_b012_fld = ntv2_kona_fld_sdiin4_frame_rate_b012;
	sdi_input_status[3].video_rate_b3_fld = ntv2_kona_fld_sdiin4_frame_rate_b3;
	sdi_input_status[3].video_progressive_fld = ntv2_kona_fld_sdiin4_progressive;
	sdi_input_status[3].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status2, 0);
	sdi_input_status[3].flag_3g_fld = ntv2_kona_fld_sdiin4_3g_mode;
	sdi_input_status[3].flag_3gb_fld = ntv2_kona_fld_sdiin4_3gb_mode;
	sdi_input_status[3].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin4_3g_b2a_convert;
	sdi_input_status[3].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin4_vpid_linka_valid;
	sdi_input_status[3].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin4_vpid_linkb_valid;
	sdi_input_status[3].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect2, 0);
	sdi_input_status[3].audio_detect_fld = ntv2_kona_fld_sdiin4_audio_detect;

	sdi_input_status[4].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status3, 0);
	sdi_input_status[4].video_geometry_b012_fld = ntv2_kona_fld_sdiin5_geometry_b012;
	sdi_input_status[4].video_geometry_b3_fld = ntv2_kona_fld_sdiin5_geometry_b3;
	sdi_input_status[4].video_rate_b012_fld = ntv2_kona_fld_sdiin5_frame_rate_b012;
	sdi_input_status[4].video_rate_b3_fld = ntv2_kona_fld_sdiin5_frame_rate_b3;
	sdi_input_status[4].video_progressive_fld = ntv2_kona_fld_sdiin5_progressive;
	sdi_input_status[4].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status3, 0);
	sdi_input_status[4].flag_3g_fld = ntv2_kona_fld_sdiin5_3g_mode;
	sdi_input_status[4].flag_3gb_fld = ntv2_kona_fld_sdiin5_3gb_mode;
	sdi_input_status[4].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin5_3g_b2a_convert;
	sdi_input_status[4].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin5_vpid_linka_valid;
	sdi_input_status[4].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin5_vpid_linkb_valid;
	sdi_input_status[4].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect3, 0);
	sdi_input_status[4].audio_detect_fld = ntv2_kona_fld_sdiin5_audio_detect;

	sdi_input_status[5].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status3, 0);
	sdi_input_status[5].video_geometry_b012_fld = ntv2_kona_fld_sdiin6_geometry_b012;
	sdi_input_status[5].video_geometry_b3_fld = ntv2_kona_fld_sdiin6_geometry_b3;
	sdi_input_status[5].video_rate_b012_fld = ntv2_kona_fld_sdiin6_frame_rate_b012;
	sdi_input_status[5].video_rate_b3_fld = ntv2_kona_fld_sdiin6_frame_rate_b3;
	sdi_input_status[5].video_progressive_fld = ntv2_kona_fld_sdiin6_progressive;
	sdi_input_status[5].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status3, 0);
	sdi_input_status[5].flag_3g_fld = ntv2_kona_fld_sdiin6_3g_mode;
	sdi_input_status[5].flag_3gb_fld = ntv2_kona_fld_sdiin6_3gb_mode;
	sdi_input_status[5].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin6_3g_b2a_convert;
	sdi_input_status[5].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin6_vpid_linka_valid;
	sdi_input_status[5].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin6_vpid_linkb_valid;
	sdi_input_status[5].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect3, 0);
	sdi_input_status[5].audio_detect_fld = ntv2_kona_fld_sdiin6_audio_detect;

	sdi_input_status[6].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status4, 0);
	sdi_input_status[6].video_geometry_b012_fld = ntv2_kona_fld_sdiin7_geometry_b012;
	sdi_input_status[6].video_geometry_b3_fld = ntv2_kona_fld_sdiin7_geometry_b3;
	sdi_input_status[6].video_rate_b012_fld = ntv2_kona_fld_sdiin7_frame_rate_b012;
	sdi_input_status[6].video_rate_b3_fld = ntv2_kona_fld_sdiin7_frame_rate_b3;
	sdi_input_status[6].video_progressive_fld = ntv2_kona_fld_sdiin7_progressive;
	sdi_input_status[6].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status3, 0);
	sdi_input_status[6].flag_3g_fld = ntv2_kona_fld_sdiin7_3g_mode;
	sdi_input_status[6].flag_3gb_fld = ntv2_kona_fld_sdiin7_3gb_mode;
	sdi_input_status[6].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin7_3g_b2a_convert;
	sdi_input_status[6].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin7_vpid_linka_valid;
	sdi_input_status[6].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin7_vpid_linkb_valid;
	sdi_input_status[6].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect3, 0);
	sdi_input_status[6].audio_detect_fld = ntv2_kona_fld_sdiin7_audio_detect;

	sdi_input_status[7].video_reg = NTV2_REG_NUM(ntv2_kona_reg_input_status4, 0);
	sdi_input_status[7].video_geometry_b012_fld = ntv2_kona_fld_sdiin8_geometry_b012;
	sdi_input_status[7].video_geometry_b3_fld = ntv2_kona_fld_sdiin8_geometry_b3;
	sdi_input_status[7].video_rate_b012_fld = ntv2_kona_fld_sdiin8_frame_rate_b012;
	sdi_input_status[7].video_rate_b3_fld = ntv2_kona_fld_sdiin8_frame_rate_b3;
	sdi_input_status[7].video_progressive_fld = ntv2_kona_fld_sdiin8_progressive;
	sdi_input_status[7].flag_reg = NTV2_REG_NUM(ntv2_kona_reg_input_3g_status3, 0);
	sdi_input_status[7].flag_3g_fld = ntv2_kona_fld_sdiin8_3g_mode;
	sdi_input_status[7].flag_3gb_fld = ntv2_kona_fld_sdiin8_3gb_mode;
	sdi_input_status[7].flag_3g_b2a_convert_fld = ntv2_kona_fld_sdiin1_3g_b2a_convert;
	sdi_input_status[7].flag_vpid_ds1_fld = ntv2_kona_fld_sdiin8_vpid_linka_valid;
	sdi_input_status[7].flag_vpid_ds2_fld = ntv2_kona_fld_sdiin8_vpid_linkb_valid;
	sdi_input_status[7].audio_reg = NTV2_REG_NUM(ntv2_kona_reg_sdiin_audio_detect3, 0);
	sdi_input_status[7].audio_detect_fld = ntv2_kona_fld_sdiin8_audio_detect;

	/* organize video routing fields by channel index */
	memset(video_fs_route, 0, sizeof(video_fs_route));
	video_fs_route[0][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select2, 0);
	video_fs_route[0][0].fld = ntv2_kona_fld_fb1_ds1_source;
	video_fs_route[0][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select34, 0);
	video_fs_route[0][1].fld = ntv2_kona_fld_fb1_ds2_source;
	video_fs_route[1][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select34, 0);
	video_fs_route[1][1].fld = ntv2_kona_fld_fb2_ds2_source;
	video_fs_route[1][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select5, 0);
	video_fs_route[1][0].fld = ntv2_kona_fld_fb2_ds1_source;
	video_fs_route[2][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select13, 0);
	video_fs_route[2][0].fld = ntv2_kona_fld_fb3_ds1_source;
	video_fs_route[2][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select34, 0);
	video_fs_route[2][1].fld = ntv2_kona_fld_fb3_ds2_source;
	video_fs_route[3][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select13, 0);
	video_fs_route[3][0].fld = ntv2_kona_fld_fb4_ds1_source;
	video_fs_route[3][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select34, 0);
	video_fs_route[3][1].fld = ntv2_kona_fld_fb4_ds2_source;
	video_fs_route[4][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select21, 0);
	video_fs_route[4][0].fld = ntv2_kona_fld_fb5_ds1_source;
	video_fs_route[4][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select35, 0);
	video_fs_route[4][1].fld = ntv2_kona_fld_fb5_ds2_source;
	video_fs_route[5][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select21, 0);
	video_fs_route[5][0].fld = ntv2_kona_fld_fb6_ds1_source;
	video_fs_route[5][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select35, 0);
	video_fs_route[5][1].fld = ntv2_kona_fld_fb6_ds2_source;
	video_fs_route[6][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select21, 0);
	video_fs_route[6][0].fld = ntv2_kona_fld_fb7_ds1_source;
	video_fs_route[6][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select35, 0);
	video_fs_route[6][1].fld = ntv2_kona_fld_fb7_ds2_source;
	video_fs_route[7][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select21, 0);
	video_fs_route[7][0].fld = ntv2_kona_fld_fb8_ds1_source;
	video_fs_route[7][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select35, 0);
	video_fs_route[7][1].fld = ntv2_kona_fld_fb8_ds2_source;

	memset(video_csc_route, 0, sizeof(video_csc_route));
	video_csc_route[0][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select1, 0);
	video_csc_route[0][0].fld = ntv2_kona_fld_csc1_vid_source;
	video_csc_route[0][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select3, 0);
	video_csc_route[0][1].fld = ntv2_kona_fld_csc1_key_source;
	video_csc_route[1][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select5, 0);
	video_csc_route[1][1].fld = ntv2_kona_fld_csc2_vid_source;
	video_csc_route[1][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select5, 0);
	video_csc_route[1][0].fld = ntv2_kona_fld_csc2_key_source;
	video_csc_route[2][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select17, 0);
	video_csc_route[2][0].fld = ntv2_kona_fld_csc3_vid_source;
	video_csc_route[2][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select17, 0);
	video_csc_route[2][1].fld = ntv2_kona_fld_csc3_key_source;
	video_csc_route[3][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select17, 0);
	video_csc_route[3][0].fld = ntv2_kona_fld_csc4_vid_source;
	video_csc_route[3][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select18, 0);
	video_csc_route[3][1].fld = ntv2_kona_fld_csc4_key_source;
	video_csc_route[4][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select18, 0);
	video_csc_route[4][0].fld = ntv2_kona_fld_csc5_vid_source;
	video_csc_route[4][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select35, 0);
	video_csc_route[4][1].fld = ntv2_kona_fld_csc5_key_source;
	video_csc_route[5][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select30, 0);
	video_csc_route[5][0].fld = ntv2_kona_fld_csc6_vid_source;
	video_csc_route[5][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select30, 0);
	video_csc_route[5][1].fld = ntv2_kona_fld_csc6_key_source;
	video_csc_route[6][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select23, 0);
	video_csc_route[6][0].fld = ntv2_kona_fld_csc7_vid_source;
	video_csc_route[6][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select23, 0);
	video_csc_route[6][1].fld = ntv2_kona_fld_csc7_key_source;
	video_csc_route[7][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select23, 0);
	video_csc_route[7][0].fld = ntv2_kona_fld_csc8_vid_source;
	video_csc_route[7][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select23, 0);
	video_csc_route[7][1].fld = ntv2_kona_fld_csc8_key_source;

	memset(video_mux_route, 0, sizeof(video_mux_route));
	video_mux_route[0][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select32, 0);
	video_mux_route[0][0].fld = ntv2_kona_fld_425mux1_ds1_source;
	video_mux_route[0][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select32, 0);
	video_mux_route[0][1].fld = ntv2_kona_fld_425mux1_ds2_source;
	video_mux_route[1][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select32, 0);
	video_mux_route[1][0].fld = ntv2_kona_fld_425mux2_ds1_source;
	video_mux_route[1][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select32, 0);
	video_mux_route[1][1].fld = ntv2_kona_fld_425mux2_ds2_source;
	video_mux_route[2][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select33, 0);
	video_mux_route[2][0].fld = ntv2_kona_fld_425mux3_ds1_source;
	video_mux_route[2][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select33, 0);
	video_mux_route[2][1].fld = ntv2_kona_fld_425mux3_ds2_source;
	video_mux_route[3][0].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select33, 0);
	video_mux_route[3][0].fld = ntv2_kona_fld_425mux4_ds1_source;
	video_mux_route[3][1].reg = NTV2_REG_NUM(ntv2_kona_reg_xpt_select33, 0);
	video_mux_route[3][1].fld = ntv2_kona_fld_425mux4_ds2_source;

	/* organize video routing sources by channel index */
	memset(video_sdi_source, 0, sizeof(video_sdi_source));
	video_sdi_source[0][0] = ntv2_kona_xpt_sdiin1_ds1;
	video_sdi_source[0][1] = ntv2_kona_xpt_sdiin1_ds2;
	video_sdi_source[1][0] = ntv2_kona_xpt_sdiin2_ds1;
	video_sdi_source[1][1] = ntv2_kona_xpt_sdiin2_ds2;
	video_sdi_source[2][0] = ntv2_kona_xpt_sdiin3_ds1;
	video_sdi_source[2][1] = ntv2_kona_xpt_sdiin3_ds2;
	video_sdi_source[3][0] = ntv2_kona_xpt_sdiin4_ds1;
	video_sdi_source[3][1] = ntv2_kona_xpt_sdiin4_ds2;
	video_sdi_source[4][0] = ntv2_kona_xpt_sdiin5_ds1;
	video_sdi_source[4][1] = ntv2_kona_xpt_sdiin5_ds2;
	video_sdi_source[5][0] = ntv2_kona_xpt_sdiin6_ds1;
	video_sdi_source[5][1] = ntv2_kona_xpt_sdiin6_ds2;
	video_sdi_source[6][0] = ntv2_kona_xpt_sdiin7_ds1;
	video_sdi_source[6][1] = ntv2_kona_xpt_sdiin7_ds2;
	video_sdi_source[7][0] = ntv2_kona_xpt_sdiin8_ds1;
	video_sdi_source[7][1] = ntv2_kona_xpt_sdiin8_ds2;

	memset(video_dl_source, 0, sizeof(video_sdi_source));
	video_dl_source[0][0] = ntv2_kona_xpt_dlin1;
	video_dl_source[1][0] = ntv2_kona_xpt_dlin1;
	video_dl_source[2][0] = ntv2_kona_xpt_dlin2;
	video_dl_source[3][0] = ntv2_kona_xpt_dlin2;
	video_dl_source[4][0] = ntv2_kona_xpt_dlin3;
	video_dl_source[5][0] = ntv2_kona_xpt_dlin3;
	video_dl_source[6][0] = ntv2_kona_xpt_dlin4;
	video_dl_source[7][0] = ntv2_kona_xpt_dlin4;

	memset(video_csc_yuv_source, 0, sizeof(video_csc_yuv_source));
	video_csc_yuv_source[0][0] = ntv2_kona_xpt_csc1_vid_yuv;
	video_csc_yuv_source[0][1] = ntv2_kona_xpt_csc1_key_yuv;
	video_csc_yuv_source[1][0] = ntv2_kona_xpt_csc2_vid_yuv;
	video_csc_yuv_source[1][1] = ntv2_kona_xpt_csc2_key_yuv;
	video_csc_yuv_source[2][0] = ntv2_kona_xpt_csc3_vid_yuv;
	video_csc_yuv_source[2][1] = ntv2_kona_xpt_csc3_key_yuv;
	video_csc_yuv_source[3][0] = ntv2_kona_xpt_csc4_vid_yuv;
	video_csc_yuv_source[3][1] = ntv2_kona_xpt_csc4_key_yuv;
	video_csc_yuv_source[4][0] = ntv2_kona_xpt_csc5_vid_yuv;
	video_csc_yuv_source[4][1] = ntv2_kona_xpt_csc5_key_yuv;
	video_csc_yuv_source[5][0] = ntv2_kona_xpt_csc6_vid_yuv;
	video_csc_yuv_source[5][1] = ntv2_kona_xpt_csc6_key_yuv;
	video_csc_yuv_source[6][0] = ntv2_kona_xpt_csc7_vid_yuv;
	video_csc_yuv_source[6][1] = ntv2_kona_xpt_csc7_key_yuv;
	video_csc_yuv_source[7][0] = ntv2_kona_xpt_csc8_vid_yuv;
	video_csc_yuv_source[7][1] = ntv2_kona_xpt_csc8_key_yuv;

	memset(video_csc_rgb_source, 0, sizeof(video_csc_rgb_source));
	video_csc_rgb_source[0][0] = ntv2_kona_xpt_csc1_vid_rgb;
	video_csc_rgb_source[1][0] = ntv2_kona_xpt_csc2_vid_rgb;
	video_csc_rgb_source[2][0] = ntv2_kona_xpt_csc3_vid_rgb;
	video_csc_rgb_source[3][0] = ntv2_kona_xpt_csc4_vid_rgb;
	video_csc_rgb_source[4][0] = ntv2_kona_xpt_csc5_vid_rgb;
	video_csc_rgb_source[5][0] = ntv2_kona_xpt_csc6_vid_rgb;
	video_csc_rgb_source[6][0] = ntv2_kona_xpt_csc7_vid_rgb;
	video_csc_rgb_source[7][0] = ntv2_kona_xpt_csc8_vid_rgb;

	memset(video_hdmi_yuv_source, 0, sizeof(video_hdmi_yuv_source));
	video_hdmi_yuv_source[0][0] = ntv2_kona_xpt_hdmiin1_yuv_q1;
	video_hdmi_yuv_source[0][1] = ntv2_kona_xpt_hdmiin1_yuv_q2;
	video_hdmi_yuv_source[0][2] = ntv2_kona_xpt_hdmiin1_yuv_q3;
	video_hdmi_yuv_source[0][3] = ntv2_kona_xpt_hdmiin1_yuv_q4;
	video_hdmi_yuv_source[1][0] = ntv2_kona_xpt_hdmiin2_yuv_q1;
	video_hdmi_yuv_source[1][1] = ntv2_kona_xpt_hdmiin2_yuv_q2;
	video_hdmi_yuv_source[1][2] = ntv2_kona_xpt_hdmiin2_yuv_q3;
	video_hdmi_yuv_source[1][3] = ntv2_kona_xpt_hdmiin2_yuv_q4;
	video_hdmi_yuv_source[2][0] = ntv2_kona_xpt_hdmiin3_yuv_q1;
	video_hdmi_yuv_source[3][0] = ntv2_kona_xpt_hdmiin4_yuv_q1;

	memset(video_hdmi_rgb_source, 0, sizeof(video_hdmi_rgb_source));
	video_hdmi_rgb_source[0][0] = ntv2_kona_xpt_hdmiin1_rgb_q1;
	video_hdmi_rgb_source[0][1] = ntv2_kona_xpt_hdmiin1_rgb_q2;
	video_hdmi_rgb_source[0][2] = ntv2_kona_xpt_hdmiin1_rgb_q3;
	video_hdmi_rgb_source[0][3] = ntv2_kona_xpt_hdmiin1_rgb_q4;
	video_hdmi_rgb_source[1][0] = ntv2_kona_xpt_hdmiin2_rgb_q1;
	video_hdmi_rgb_source[1][1] = ntv2_kona_xpt_hdmiin2_rgb_q2;
	video_hdmi_rgb_source[1][2] = ntv2_kona_xpt_hdmiin2_rgb_q3;
	video_hdmi_rgb_source[1][3] = ntv2_kona_xpt_hdmiin2_rgb_q4;
	video_hdmi_rgb_source[2][0] = ntv2_kona_xpt_hdmiin3_rgb_q1;
	video_hdmi_rgb_source[3][0] = ntv2_kona_xpt_hdmiin4_rgb_q1;

	memset(video_mux_yuv_source, 0, sizeof(video_mux_yuv_source));
	video_mux_yuv_source[0][0] = ntv2_kona_xpt_425mux1_ds1_yuv;
	video_mux_yuv_source[0][1] = ntv2_kona_xpt_425mux1_ds2_yuv;
	video_mux_yuv_source[1][0] = ntv2_kona_xpt_425mux2_ds1_yuv;
	video_mux_yuv_source[1][1] = ntv2_kona_xpt_425mux2_ds2_yuv;
	video_mux_yuv_source[2][0] = ntv2_kona_xpt_425mux3_ds1_yuv;
	video_mux_yuv_source[2][1] = ntv2_kona_xpt_425mux3_ds2_yuv;
	video_mux_yuv_source[3][0] = ntv2_kona_xpt_425mux4_ds1_yuv;
	video_mux_yuv_source[3][1] = ntv2_kona_xpt_425mux4_ds2_yuv;

	memset(video_mux_rgb_source, 0, sizeof(video_mux_rgb_source));
	video_mux_rgb_source[0][0] = ntv2_kona_xpt_425mux1_ds1_rgb;
	video_mux_rgb_source[0][1] = ntv2_kona_xpt_425mux1_ds2_rgb;
	video_mux_rgb_source[1][0] = ntv2_kona_xpt_425mux2_ds1_rgb;
	video_mux_rgb_source[1][1] = ntv2_kona_xpt_425mux2_ds2_rgb;
	video_mux_rgb_source[2][0] = ntv2_kona_xpt_425mux3_ds1_rgb;
	video_mux_rgb_source[2][1] = ntv2_kona_xpt_425mux3_ds2_rgb;
	video_mux_rgb_source[3][0] = ntv2_kona_xpt_425mux4_ds1_rgb;
	video_mux_rgb_source[3][1] = ntv2_kona_xpt_425mux4_ds2_rgb;

	/* ntv2 video standard to hdmi video standard */
	for (i = 0; i < NTV2_MAX_VIDEO_STANDARDS; i++) {
		video_standard_to_hdmi[i] = ntv2_kona_hdmiin_video_standard_none;
	}
	video_standard_to_hdmi[ntv2_kona_video_standard_1080i] = ntv2_kona_hdmiin_video_standard_1080i;
	video_standard_to_hdmi[ntv2_kona_video_standard_720p] = ntv2_kona_hdmiin_video_standard_720p;
	video_standard_to_hdmi[ntv2_kona_video_standard_525i] = ntv2_kona_hdmiin_video_standard_525i;
	video_standard_to_hdmi[ntv2_kona_video_standard_625i] = ntv2_kona_hdmiin_video_standard_625i;
	video_standard_to_hdmi[ntv2_kona_video_standard_1080p] = ntv2_kona_hdmiin_video_standard_1080p;
	video_standard_to_hdmi[ntv2_kona_video_standard_3840x2160p] = ntv2_kona_hdmiin_video_standard_4k;
	video_standard_to_hdmi[ntv2_kona_video_standard_4096x2160p] = ntv2_kona_hdmiin_video_standard_4k;

	/* ntv2 frame rate to hdmi frame rate */
	for (i = 0; i < NTV2_MAX_FRAME_RATES; i++) {
		frame_rate_to_hdmi[i] = ntv2_kona_hdmiin_frame_rate_none;
	}
	frame_rate_to_hdmi[ntv2_kona_frame_rate_6000] = ntv2_kona_hdmiin_frame_rate_6000;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_5994] = ntv2_kona_hdmiin_frame_rate_5994;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_3000] = ntv2_kona_hdmiin_frame_rate_3000;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_2997] = ntv2_kona_hdmiin_frame_rate_2997;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_2500] = ntv2_kona_hdmiin_frame_rate_2500;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_2400] = ntv2_kona_hdmiin_frame_rate_2400;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_2398] = ntv2_kona_hdmiin_frame_rate_2398;
	frame_rate_to_hdmi[ntv2_kona_frame_rate_5000] = ntv2_kona_hdmiin_frame_rate_5000;

	/* ntv2 strings */
	for (i = 0; i < NTV2_MAX_VIDEO_STANDARDS; i++) {
		video_standard_name[i] = "undefined";
	}
	video_standard_name[ntv2_kona_video_standard_1080i] = "1920x1080i";
	video_standard_name[ntv2_kona_video_standard_720p] = "1280x720p";
	video_standard_name[ntv2_kona_video_standard_525i] = "525i";
	video_standard_name[ntv2_kona_video_standard_625i] = "625i";
	video_standard_name[ntv2_kona_video_standard_1080p] = "1920x1080p";
	video_standard_name[ntv2_kona_video_standard_2048x1556] = "2048x1556";
	video_standard_name[ntv2_kona_video_standard_2048x1080p] = "2048x1080p";
	video_standard_name[ntv2_kona_video_standard_2048x1080i] = "2048x1080i";
	video_standard_name[ntv2_kona_video_standard_3840x2160p] = "3840x2160p";
	video_standard_name[ntv2_kona_video_standard_4096x2160p] = "4096x2160p";

	for (i = 0; i < NTV2_MAX_FRAME_GEOMETRIES; i++) {
		frame_geometry_name[i] = "undefined";
	}
	frame_geometry_name[ntv2_kona_frame_geometry_1920x1080] = "1920x1080";
	frame_geometry_name[ntv2_kona_frame_geometry_1280x720] = "1280x720";
	frame_geometry_name[ntv2_kona_frame_geometry_720x486] = "720x486";
	frame_geometry_name[ntv2_kona_frame_geometry_720x576] = "720x576";
	frame_geometry_name[ntv2_kona_frame_geometry_1920x1114] = "1920x1114";
	frame_geometry_name[ntv2_kona_frame_geometry_2048x1114] = "2048x1114";
	frame_geometry_name[ntv2_kona_frame_geometry_720x508] = "720x508";
	frame_geometry_name[ntv2_kona_frame_geometry_720x598] = "720x598";
	frame_geometry_name[ntv2_kona_frame_geometry_1920x1112] = "1920x1112";
	frame_geometry_name[ntv2_kona_frame_geometry_1280x740] = "1280x740";
	frame_geometry_name[ntv2_kona_frame_geometry_2048x1080] = "2048x1080";
	frame_geometry_name[ntv2_kona_frame_geometry_2048x1556] = "2048x1556";
	frame_geometry_name[ntv2_kona_frame_geometry_2048x1588] = "2048x1588";
	frame_geometry_name[ntv2_kona_frame_geometry_2048x1112] = "2048x1112";
	frame_geometry_name[ntv2_kona_frame_geometry_720x514]= "720x514";
	frame_geometry_name[ntv2_kona_frame_geometry_720x612] = "720x612";
	frame_geometry_name[ntv2_kona_frame_geometry_4x1920x1080] = "4x1920x1080";
	frame_geometry_name[ntv2_kona_frame_geometry_4x2048x1080] = "4x2048x1080";

	for (i = 0; i < NTV2_MAX_INPUT_GEOMETRIES; i++) {
		input_geometry_name[i] = "undefined";
	}
	input_geometry_name[ntv2_kona_input_geometry_525] = "525";
	input_geometry_name[ntv2_kona_input_geometry_625] = "625";
	input_geometry_name[ntv2_kona_input_geometry_750] = "720";
	input_geometry_name[ntv2_kona_input_geometry_1125] = "1080";

	for (i = 0; i < NTV2_MAX_FRAME_RATES; i++) {
		frame_rate_name[i] = "undefined";
	}
	frame_rate_name[ntv2_kona_frame_rate_6000] = "60";
	frame_rate_name[ntv2_kona_frame_rate_5994] = "59.94";
	frame_rate_name[ntv2_kona_frame_rate_3000] = "30";
	frame_rate_name[ntv2_kona_frame_rate_2997] = "29.97";
	frame_rate_name[ntv2_kona_frame_rate_2500] = "25";
	frame_rate_name[ntv2_kona_frame_rate_2400] = "24";
	frame_rate_name[ntv2_kona_frame_rate_2398] = "23.98";
	frame_rate_name[ntv2_kona_frame_rate_5000] = "50";
	frame_rate_name[ntv2_kona_frame_rate_4800] = "48";
	frame_rate_name[ntv2_kona_frame_rate_4795] = "47.95";
	frame_rate_name[ntv2_kona_frame_rate_12000] = "120";
	frame_rate_name[ntv2_kona_frame_rate_11988] = "119.88";
	frame_rate_name[ntv2_kona_frame_rate_1500] = "15";
	frame_rate_name[ntv2_kona_frame_rate_1400] = "14";

	for (i = 0; i < NTV2_MAX_COLOR_SPACES; i++) {
		color_space_name[i] = "undefined";
	}
	color_space_name[ntv2_kona_color_space_yuv422] = "yuv 422";
	color_space_name[ntv2_kona_color_space_rgb444] = "rgb 444";
	color_space_name[ntv2_kona_color_space_yuv444] = "yuv 444";
	color_space_name[ntv2_kona_color_space_yuv420] = "yuv 420";
	color_space_name[ntv2_kona_color_space_none] = "none";

	for (i = 0; i < NTV2_MAX_COLOR_DEPTHS; i++) {
		color_depth_name[i] = "undefined";
	}
	color_depth_name[ntv2_kona_color_depth_8bit] = "8 bit";
	color_depth_name[ntv2_kona_color_depth_10bit] = "10 bit";
	color_depth_name[ntv2_kona_color_depth_12bit] = "12 bit";
	color_depth_name[ntv2_kona_color_depth_none] = "none";
	
	register_data_init = true;
}

const char* ntv2_video_standard_name(u32 standard)
{
	if (standard >= NTV2_MAX_VIDEO_STANDARDS)
		return "*bad video standard*";
	return video_standard_name[standard];
}

const char* ntv2_frame_geometry_name(u32 geometry)
{
	if (geometry >= NTV2_MAX_FRAME_GEOMETRIES)
		return "*bad video geometry*";
	return frame_geometry_name[geometry];
}

const char* ntv2_input_geometry_name(u32 geometry)
{
	if (geometry >= NTV2_MAX_INPUT_GEOMETRIES)
		return "*bad input geometry*";
	return input_geometry_name[geometry];
}

const char* ntv2_frame_rate_name(u32 rate)
{
	if (rate >= NTV2_MAX_FRAME_RATES)
		return "*bad frame rate*";
	return frame_rate_name[rate];
}

const char* ntv2_color_space_name(u32 color)
{
	if (color >= NTV2_MAX_COLOR_SPACES)
		return "*bad color space*";
	return color_space_name[color];
}

const char* ntv2_color_depth_name(u32 color)
{
	if (color >= NTV2_MAX_COLOR_DEPTHS)
		return "*bad color depth*";
	return color_depth_name[color];
}

u32 ntv2_video_standard_to_hdmiin(u32 video_standard)
{
	if (video_standard >= NTV2_MAX_VIDEO_STANDARDS)
		return 0;

	return video_standard_to_hdmi[video_standard];
}

u32 ntv2_frame_rate_to_hdmiin(u32 frame_rate)
{
	if (frame_rate >= NTV2_MAX_FRAME_RATES)
		return 0;

	return frame_rate_to_hdmi[frame_rate];
}

u32 ntv2_read_frame_size(struct ntv2_register *ntv2_reg, int index)
{
	u32 val;
	u32 mult = 1;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return 0;

	val = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_global_control2, 0);
	if (index < 4) {
		val = NTV2_FLD_GET(ntv2_kona_fld_fs1234_quad_mode, val);
	} else {
		val = NTV2_FLD_GET(ntv2_kona_fld_fs5678_quad_mode, val);
	}
	if (val != 0)
		mult = 4;

	val = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_frame_control, index);
	val = NTV2_FLD_GET(ntv2_kona_fld_frame_size, val);

	if (val == ntv2_kona_frame_size_2mb)
		val = 0x200000;
	else if (val == ntv2_kona_frame_size_4mb)
		val = 0x400000;
	else if (val == ntv2_kona_frame_size_8mb)
		val = 0x800000;
	else
		val = 0x1000000;

	return val * mult;
}

void ntv2_video_read_interrupt_status(struct ntv2_register *ntv2_reg,
									  struct ntv2_interrupt_status* irq_status)
{
	if ((ntv2_reg == NULL) ||
		(irq_status == NULL))
		return;

	irq_status->interrupt_status[0] = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_interrupt_status, 0);
	irq_status->interrupt_status[1] = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_interrupt_status2, 0);
}

void ntv2_video_input_interrupt_enable(struct ntv2_register *ntv2_reg, int index, bool enable)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(video_input_data[index].int_enable_fld, enable? 1 : 0);
	mask = NTV2_FLD_MASK(video_input_data[index].int_enable_fld);
	ntv2_register_rmw(ntv2_reg, video_input_data[index].int_enable_reg, val, mask);
}

void ntv2_video_input_interrupt_clear(struct ntv2_register *ntv2_reg, int index)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(video_input_data[index].int_clear_fld, 1);
	mask = NTV2_FLD_MASK(video_input_data[index].int_clear_fld);
	ntv2_register_rmw(ntv2_reg, video_input_data[index].int_clear_reg, val, mask);
}

bool ntv2_video_input_interrupt_active(struct ntv2_interrupt_status* irq_status, int index)
{
	if ((irq_status == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return false;

	if ((irq_status->interrupt_status[video_input_data[index].int_active_reg] &
		 NTV2_FLD_MASK(video_input_data[index].int_active_fld)) != 0)
		return true;

	return false;
}

u32 ntv2_video_input_field_id(struct ntv2_interrupt_status* irq_status, int index)
{
	if ((irq_status == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return 0;

	if ((irq_status->interrupt_status[video_input_data[index].vid_field_reg] &
		 NTV2_FLD_MASK(video_input_data[index].vid_field_fld)) != 0)
		return 1;

	return 0;
}

void ntv2_video_output_interrupt_enable(struct ntv2_register* ntv2_reg, int index, bool enable)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(video_output_data[index].int_enable_fld, enable? 1 : 0);
	mask = NTV2_FLD_MASK(video_output_data[index].int_enable_fld);
	ntv2_register_rmw(ntv2_reg, video_output_data[index].int_enable_reg, val, mask);
}

void ntv2_video_output_interrupt_clear(struct ntv2_register* ntv2_reg, int index)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(video_output_data[index].int_clear_fld, 1);
	mask = NTV2_FLD_MASK(video_output_data[index].int_clear_fld);
	ntv2_register_rmw(ntv2_reg, video_output_data[index].int_clear_reg, val, mask);
}

u32 ntv2_video_output_interrupt_rate(struct ntv2_register *ntv2_reg, int index)
{
	u32 val;
	u32 std;
	u32 rate;
	u32 sync;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return ntv2_kona_frame_rate_none;

	val = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_global_control, index);
	std = NTV2_FLD_GET(ntv2_kona_fld_global_video_standard, val);
	sync = NTV2_FLD_GET(ntv2_kona_fld_global_reg_sync, val);
	rate = NTV2_FLD_GET(ntv2_kona_fld_global_frame_rate_b012, val);
	rate |= NTV2_FLD_GET(ntv2_kona_fld_global_frame_rate_b3, val) << 3;

	if (rate == ntv2_kona_frame_rate_none)
		return ntv2_kona_frame_rate_none;

	if (ntv2_video_standard_progressive(std))
		return rate;

	if (sync == ntv2_kona_reg_sync_frame)
		return rate;

	if (rate == ntv2_kona_frame_rate_3000)
		return ntv2_kona_frame_rate_6000;
	if (rate == ntv2_kona_frame_rate_2997)
		return ntv2_kona_frame_rate_5994;
	if (rate == ntv2_kona_frame_rate_2500)
		return ntv2_kona_frame_rate_5000;

	return ntv2_kona_frame_rate_none;
}

bool ntv2_video_output_interrupt_active(struct ntv2_interrupt_status* irq_status, int index)
{
	if ((irq_status == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return false;

	if ((irq_status->interrupt_status[video_output_data[index].int_active_reg] &
		 NTV2_FLD_MASK(video_output_data[index].int_active_fld)) != 0)
		return true;

	return false;
}

u32 ntv2_video_output_field_id(struct ntv2_interrupt_status* irq_status, int index)
{
	if ((irq_status == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return 0;

	if ((irq_status->interrupt_status[video_output_data[index].vid_field_reg] &
		 NTV2_FLD_MASK(video_output_data[index].vid_field_fld)) != 0)
		return 1;

	return 0;
}

void ntv2_sdi_output_transmit_enable(struct ntv2_register* ntv2_reg, int index, bool enable)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(video_output_data[index].vid_transmit_fld, enable? 1 : 0);
	mask = NTV2_FLD_MASK(video_output_data[index].vid_transmit_fld);
	ntv2_register_rmw(ntv2_reg, video_output_data[index].vid_transmit_reg, val, mask);
}

void ntv2_sdi_input_convert_3g_enable(struct ntv2_register *ntv2_reg, int index, bool enable)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = NTV2_FLD_SET(sdi_input_status[index].flag_3g_b2a_convert_fld, enable? 1 : 0);
	mask = NTV2_FLD_MASK(sdi_input_status[index].flag_3g_b2a_convert_fld);
	ntv2_register_rmw(ntv2_reg, sdi_input_status[index].flag_reg, val, mask);
}

void ntv2_read_sdi_input_status(struct ntv2_register* ntv2_reg, int index,
								struct ntv2_sdi_input_status *input_status)
{
	u32 val = 0;
	u32 rate = 0;
	u32 geom = 0;
	bool prog = false;
	bool tg = false;
	bool tgb = false;
	u32 vds1 = 0;
	u32 vds2 = 0;
	u32 detect = 0;

	if ((ntv2_reg == NULL) ||
		(input_status == NULL) ||
		(index < 0) || (index > NTV2_MAX_CHANNELS))
		return;

	val = ntv2_register_read(ntv2_reg, sdi_input_status[index].video_reg);
//	printk("sdi input status %d %08x\n", sdi_input_status[index].video_reg, val);

	rate = NTV2_FLD_GET(sdi_input_status[index].video_rate_b012_fld, val);
	rate |= NTV2_FLD_GET(sdi_input_status[index].video_rate_b3_fld, val) << 3;
	if (rate != 0) {
		geom = NTV2_FLD_GET(sdi_input_status[index].video_geometry_b012_fld, val);
		geom |= NTV2_FLD_GET(sdi_input_status[index].video_geometry_b3_fld, val) << 3;
		prog = NTV2_FLD_GET(sdi_input_status[index].video_progressive_fld, val) != 0;
		
		val = ntv2_register_read(ntv2_reg, sdi_input_status[index].flag_reg);

		tg = NTV2_FLD_GET(sdi_input_status[index].flag_3g_fld, val) != 0;
		tgb = NTV2_FLD_GET(sdi_input_status[index].flag_3gb_fld, val) != 0;
		
		if (NTV2_FLD_GET(sdi_input_status[index].flag_vpid_ds1_fld, val) != 0)
			vds1 = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_sdiin_vpid_linka, index);
		if (NTV2_FLD_GET(sdi_input_status[index].flag_vpid_ds2_fld, val) != 0)
			vds2 = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_sdiin_vpid_linka, index);

		val = ntv2_register_read(ntv2_reg, sdi_input_status[index].audio_reg);
		detect = NTV2_FLD_GET(sdi_input_status[index].audio_detect_fld, val);
	}

	input_status->input_index = index;
	input_status->frame_rate = rate;
	input_status->input_geometry = geom;
	input_status->progressive = prog;
	input_status->is3g = tg;
	input_status->is3gb = tgb;
	input_status->vpid_ds1 = vds1;
	input_status->vpid_ds2 = vds2;
	input_status->audio_detect = detect;
}

void ntv2_read_aes_input_status(struct ntv2_register* ntv2_reg, int index,
								struct ntv2_aes_input_status *input_status)
{
	u32 val = 0;
	u32 invalid = 0;

	if ((ntv2_reg == NULL) ||
		(input_status == NULL) ||
		(index != 0))
		return;

	val = ntv2_reg_read(ntv2_reg, ntv2_kona_reg_input_status, 0);
	invalid = NTV2_FLD_GET(ntv2_kona_fld_aesin12_invalid, val) << 0;
	invalid |= NTV2_FLD_GET(ntv2_kona_fld_aesin12_invalid, val) << 1;
	invalid |= NTV2_FLD_GET(ntv2_kona_fld_aesin12_invalid, val) << 2;
	invalid |= NTV2_FLD_GET(ntv2_kona_fld_aesin12_invalid, val) << 3;

	input_status->input_index = index;
	input_status->audio_detect = (~invalid) & 0xf;
}

void ntv2_route_sdi_to_fs(struct ntv2_register* ntv2_reg,
						  int sdi_index, int sdi_stream, bool sdi_rgb,
						  int fs_index, int fs_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(sdi_index < 0) || (sdi_index >= NTV2_MAX_CHANNELS) ||
		(sdi_stream < 0) || (sdi_stream >= NTV2_MAX_STREAMS) ||
		(fs_index < 0) || (fs_index >= NTV2_MAX_CHANNELS) ||
		(fs_stream < 0) || (fs_stream >= NTV2_MAX_STREAMS) ||
		(video_fs_route[fs_index][fs_stream].reg == 0))
		return;

	if (sdi_rgb) {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_dl_source[sdi_index][sdi_stream]);
	} else {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_sdi_source[sdi_index][sdi_stream]);
	}
	mask = NTV2_FLD_MASK(video_fs_route[fs_index][fs_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_fs_route[fs_index][fs_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_fs_route[fs_index][fs_stream].reg, val, mask);
}

void ntv2_route_sdi_to_csc(struct ntv2_register* ntv2_reg,
						   int sdi_index, int sdi_stream, bool sdi_rgb,
						   int csc_index, int csc_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(sdi_index < 0) || (sdi_index >= NTV2_MAX_CHANNELS) ||
		(sdi_stream < 0) || (sdi_stream >= NTV2_MAX_STREAMS) ||
		(csc_index < 0) || (csc_index >= NTV2_MAX_CHANNELS) ||
		(csc_stream < 0) || (csc_stream >= NTV2_MAX_STREAMS) ||
		(video_csc_route[csc_index][csc_stream].reg == 0))
		return;

	if (sdi_rgb) {
		val = NTV2_FLD_SET(video_csc_route[csc_index][csc_stream].fld, video_dl_source[sdi_index][sdi_stream]);
	} else {
		val = NTV2_FLD_SET(video_csc_route[csc_index][csc_stream].fld, video_sdi_source[sdi_index][sdi_stream]);
	}
	mask = NTV2_FLD_MASK(video_csc_route[csc_index][csc_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_csc_route[csc_index][csc_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_csc_route[csc_index][csc_stream].reg, val, mask);
}

void ntv2_route_sdi_to_mux(struct ntv2_register* ntv2_reg,
						   int sdi_index, int sdi_stream, bool sdi_rgb,
						   int mux_index, int mux_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(sdi_index < 0) || (sdi_index >= NTV2_MAX_CHANNELS) ||
		(sdi_stream < 0) || (sdi_stream >= NTV2_MAX_STREAMS) ||
		(mux_index < 0) || (mux_index >= NTV2_MAX_CHANNELS) ||
		(mux_stream < 0) || (mux_stream >= NTV2_MAX_STREAMS) ||
		(video_mux_route[mux_index][mux_stream].reg == 0))
		return;

	if (sdi_rgb) {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_dl_source[sdi_index][sdi_stream]);
	} else {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_sdi_source[sdi_index][sdi_stream]);
	}
	mask = NTV2_FLD_MASK(video_mux_route[mux_index][mux_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_mux_route[mux_index][mux_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_mux_route[mux_index][mux_stream].reg, val, mask);
}

void ntv2_route_hdmi_to_fs(struct ntv2_register* ntv2_reg,
						   int hdmi_index, int hdmi_stream, bool hdmi_rgb,
						   int fs_index, int fs_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(hdmi_index < 0) || (hdmi_index >= NTV2_MAX_CHANNELS) ||
		(hdmi_stream < 0) || (hdmi_stream >= NTV2_MAX_STREAMS) ||
		(fs_index < 0) || (fs_index >= NTV2_MAX_CHANNELS) ||
		(fs_stream < 0) || (fs_stream >= NTV2_MAX_STREAMS) ||
		(video_fs_route[fs_index][fs_stream].reg == 0))
		return;

	if (hdmi_rgb) {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_hdmi_rgb_source[hdmi_index][hdmi_stream]);
	} else {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_hdmi_yuv_source[hdmi_index][hdmi_stream]);
	}
	mask = NTV2_FLD_MASK(video_fs_route[fs_index][fs_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_fs_route[fs_index][fs_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_fs_route[fs_index][fs_stream].reg, val, mask);
}

void ntv2_route_hdmi_to_csc(struct ntv2_register* ntv2_reg,
							int hdmi_index, int hdmi_stream, bool hdmi_rgb,
							int csc_index, int csc_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(hdmi_index < 0) || (hdmi_index >= NTV2_MAX_CHANNELS) ||
		(hdmi_stream < 0) || (hdmi_stream >= NTV2_MAX_STREAMS) ||
		(csc_index < 0) || (csc_index >= NTV2_MAX_CHANNELS) ||
		(csc_stream < 0) || (csc_stream >= NTV2_MAX_STREAMS) ||
		(video_csc_route[csc_index][csc_stream].reg == 0))
		return;

	if (hdmi_rgb) {
		val = NTV2_FLD_SET(video_csc_route[csc_index][csc_stream].fld, video_hdmi_rgb_source[hdmi_index][hdmi_stream]);
	} else {
		val = NTV2_FLD_SET(video_csc_route[csc_index][csc_stream].fld, video_hdmi_yuv_source[hdmi_index][hdmi_stream]);
	}
	mask = NTV2_FLD_MASK(video_csc_route[csc_index][csc_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_csc_route[csc_index][csc_stream].reg, val, mask);
}

void ntv2_route_hdmi_to_mux(struct ntv2_register* ntv2_reg,
							int hdmi_index, int hdmi_stream, bool hdmi_rgb,
							int mux_index, int mux_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(hdmi_index < 0) || (hdmi_index >= NTV2_MAX_CHANNELS) ||
		(hdmi_stream < 0) || (hdmi_stream >= NTV2_MAX_STREAMS) ||
		(mux_index < 0) || (mux_index >= NTV2_MAX_CHANNELS) ||
		(mux_stream < 0) || (mux_stream >= NTV2_MAX_STREAMS) ||
		(video_mux_route[mux_index][mux_stream].reg == 0))
		return;

	if (hdmi_rgb) {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_hdmi_rgb_source[hdmi_index][hdmi_stream]);
	} else {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_hdmi_yuv_source[hdmi_index][hdmi_stream]);
	}
	mask = NTV2_FLD_MASK(video_mux_route[mux_index][mux_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_mux_route[mux_index][mux_stream].reg, val, mask);
}

void ntv2_route_csc_to_fs(struct ntv2_register* ntv2_reg,
						  int csc_index, int csc_stream, bool csc_rgb,
						  int fs_index, int fs_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(csc_index < 0) || (csc_index >= NTV2_MAX_CHANNELS) ||
		(csc_stream < 0) || (csc_stream >= NTV2_MAX_STREAMS) ||
		(fs_index < 0) || (fs_index >= NTV2_MAX_CHANNELS) ||
		(fs_stream < 0) || (fs_stream >= NTV2_MAX_STREAMS) ||
		(video_fs_route[fs_index][fs_stream].reg == 0))
		return;

	if (csc_rgb) {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_csc_rgb_source[csc_index][csc_stream]);
	} else {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_csc_yuv_source[csc_index][csc_stream]);
	}
	mask = NTV2_FLD_MASK(video_fs_route[fs_index][fs_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_fs_route[fs_index][fs_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_fs_route[fs_index][fs_stream].reg, val, mask);
}

void ntv2_route_csc_to_mux(struct ntv2_register* ntv2_reg,
						   int csc_index, int csc_stream, bool csc_rgb,
						   int mux_index, int mux_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(csc_index < 0) || (csc_index >= NTV2_MAX_CHANNELS) ||
		(csc_stream < 0) || (csc_stream >= NTV2_MAX_STREAMS) ||
		(mux_index < 0) || (mux_index >= NTV2_MAX_CHANNELS) ||
		(mux_stream < 0) || (mux_stream >= NTV2_MAX_STREAMS) ||
		(video_mux_route[mux_index][mux_stream].reg == 0))
		return;

	if (csc_rgb) {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_csc_rgb_source[csc_index][csc_stream]);
	} else {
		val = NTV2_FLD_SET(video_mux_route[mux_index][mux_stream].fld, video_csc_yuv_source[csc_index][csc_stream]);
	}
	mask = NTV2_FLD_MASK(video_mux_route[mux_index][mux_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_mux_route[mux_index][mux_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_mux_route[mux_index][mux_stream].reg, val, mask);
}

void ntv2_route_mux_to_fs(struct ntv2_register* ntv2_reg,
						  int mux_index, int mux_stream, bool mux_rgb,
						  int fs_index, int fs_stream)
{
	u32 val;
	u32 mask;

	if ((ntv2_reg == NULL) ||
		(mux_index < 0) || (mux_index >= NTV2_MAX_CHANNELS) ||
		(mux_stream < 0) || (mux_stream >= NTV2_MAX_STREAMS) ||
		(fs_index < 0) || (fs_index >= NTV2_MAX_CHANNELS) ||
		(fs_stream < 0) || (fs_stream >= NTV2_MAX_STREAMS) ||
		(video_fs_route[fs_index][fs_stream].reg == 0))
		return;

	if (mux_rgb) {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_mux_rgb_source[mux_index][mux_stream]);
	} else {
		val = NTV2_FLD_SET(video_fs_route[fs_index][fs_stream].fld, video_mux_yuv_source[mux_index][mux_stream]);
	}
	mask = NTV2_FLD_MASK(video_fs_route[fs_index][fs_stream].fld);
	ntv2_register_rmw(ntv2_reg, video_fs_route[fs_index][fs_stream].reg, val, mask);
//	NTV2_MSG_INFO("write reg %d  val %08x  mask %08x\n",
//				  video_fs_route[fs_index][fs_stream].reg, val, mask);
}

