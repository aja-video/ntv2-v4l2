/*
 * NTV2 HDMI input control
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

#include "ntv2_hdmiin.h"
#include "ntv2_hinreg.h"
#include "ntv2_features.h"
#include "ntv2_konareg.h"
#include "ntv2_register.h"
#include "ntv2_konai2c.h"

/* 
   Bits to flag reporting of measurements. These are all set in mRelockReports whenever
   the TMDS clock has lost lock and it is necessary to re-sync everything. They get
   cleared by the individual cogs of the state machine as details of the format 
   are determined.
*/
#define NTV2_REPORT_CABLE	0x0001
#define NTV2_REPORT_SYNC	0x0002
#define NTV2_REPORT_FREQ	0x0004
#define NTV2_REPORT_INFO	0x0008
#define NTV2_REPORT_TIMING	0x0010
#define NTV2_REPORT_DVI		0x0020
#define NTV2_REPORT_HDMI	0x0040
#define NTV2_REPORT_FORMAT	0x0080

#define NTV2_REPORT_ANY		0xffff

/*
  At or below this clock frequency, the doubler will be turned on.
  This value would be 27.5MHz if not for deep color, which maintains pixel 
  doubling up to 40.5MHz in 576p. 41MHz is used because a tolerance of .5%
  above standard is required for certification (kHz).
*/
#define TMDS_DOUBLING_FREQ			41000
/* frame rate comparison tolerance (parts per thousand) */
#define FRAME_RATE_TOLERANCE		6
/* high frequency clock phase adjustment (degrees?)*/
#define CLOCK_PHASE_HF  			18

/* vic mapping */
struct ntv2_video_code_info {
	u32 video_standard;
	u32 frame_rate;
};

#define NTV2_AVI_VIC_INFO_SIZE			120
struct ntv2_video_code_info ntv2_avi_vic_info[NTV2_AVI_VIC_INFO_SIZE];

#define NTV2_VSI_VIC_INFO_SIZE			8
struct ntv2_video_code_info ntv2_vsi_vic_info[NTV2_VSI_VIC_INFO_SIZE];

static int ntv2_hdmiin_monitor(void* data);
static int ntv2_hdmiin_write_multi(struct ntv2_hdmiin *ntv2_hin,
								   u8 device,
								   struct ntv2_reg_value *reg_value,
								   int count);
static int ntv2_hdmiin_read_verify(struct ntv2_hdmiin *ntv2_hin,
								   u8 device,
								   struct ntv2_reg_value *reg_value,
								   int count);
static int ntv2_hdmiin_initialize(struct ntv2_hdmiin *ntv2_hin);
static int ntv2_hdmiin_set_color_mode(struct ntv2_hdmiin *ntv2_hin, bool yuv_input, bool yuv_output);
static int ntv2_hdmiin_set_uhd_mode(struct ntv2_hdmiin *ntv2_hin, bool enable);
static int ntv2_hdmi_set_derep_mode(struct ntv2_hdmiin *ntv2_hin, bool enable);
static void ntv2_hdmiin_update_tmds_freq(struct ntv2_hdmiin *ntv2_hin);
static void ntv2_hdmiin_config_pixel_clock(struct ntv2_hdmiin *ntv2_hin);
static u32 ntv2_hdmiin_read_paired_value(struct ntv2_hdmiin *ntv2_hin, u8 reg, u32 bits, u32 shift);
static void ntv2_hdmiin_update_timing(struct ntv2_hdmiin *ntv2_hin);
static void ntv2_hdmiin_find_dvi_format(struct ntv2_hdmiin *ntv2_hin,
										struct ntv2_hdmiin_format *format);
static void ntv2_hdmiin_find_hdmi_format(struct ntv2_hdmiin *ntv2_hin,
										 struct ntv2_hdmiin_format *format);
static u32 ntv2_hdmiin_pixel_double(struct ntv2_hdmiin *ntv2_hin, u32 pixels);
static int ntv2_hdmiin_set_video_format(struct ntv2_hdmiin *ntv2_hin,
										struct ntv2_hdmiin_format *format);
static void ntv2_hdmiin_set_no_video(struct ntv2_hdmiin *ntv2_hin);

struct ntv2_hdmiin *ntv2_hdmiin_open(struct ntv2_object *ntv2_obj,
									 const char *name, int index)
{
	struct ntv2_hdmiin *ntv2_hin = NULL;

	ntv2_hin = kzalloc(sizeof(struct ntv2_hdmiin), GFP_KERNEL);
	if (ntv2_hin == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_hdmiin instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_hin->index = index;
	snprintf(ntv2_hin->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_hin->list);
	ntv2_hin->ntv2_dev = ntv2_obj->ntv2_dev;

	spin_lock_init(&ntv2_hin->state_lock);

	NTV2_MSG_HDMIIN_INFO("%s: open ntv2_hdmiin\n", ntv2_hin->name);

	return ntv2_hin;
}

void ntv2_hdmiin_close(struct ntv2_hdmiin *ntv2_hin)
{
	if (ntv2_hin == NULL) 
		return;

	NTV2_MSG_HDMIIN_INFO("%s: close ntv2_hdmiin\n", ntv2_hin->name);

	ntv2_hdmiin_disable(ntv2_hin);

	ntv2_konai2c_close(ntv2_hin->i2c_reg);

	memset(ntv2_hin, 0, sizeof(struct ntv2_hdmiin));
	kfree(ntv2_hin);
}

int ntv2_hdmiin_configure(struct ntv2_hdmiin *ntv2_hin,
						  struct ntv2_features *features,
						  struct ntv2_register *vid_reg)
{
	int result;

	if ((ntv2_hin == NULL) ||
		(features == NULL) ||
		(vid_reg == NULL))
		return -EPERM;

	NTV2_MSG_HDMIIN_INFO("%s: configure hdmi input device\n", ntv2_hin->name);

	ntv2_hin->features = features;
	ntv2_hin->vid_reg = vid_reg;

	ntv2_hin->i2c_reg = ntv2_konai2c_open((struct ntv2_object*)ntv2_hin, "i2c", ntv2_hin->index);
	if (ntv2_hin->i2c_reg == NULL)
		return -ENOMEM;

	result = ntv2_konai2c_configure(ntv2_hin->i2c_reg,
									ntv2_hin->vid_reg,
									NTV2_REG_NUM(ntv2_kona_reg_hdmiin_i2c_control, ntv2_hin->index),
									NTV2_REG_NUM(ntv2_kona_reg_hdmiin_i2c_data, ntv2_hin->index));
	if (result < 0)
		return result;

	return 0;
}

int ntv2_hdmiin_enable(struct ntv2_hdmiin *ntv2_hin)
{
	if (ntv2_hin == NULL)
		return -EPERM;

	if (ntv2_hin->monitor_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_HDMIIN_STATE("%s: enable hdmi input monitor\n", ntv2_hin->name);

	ntv2_hin->monitor_task = kthread_run(ntv2_hdmiin_monitor, (void*)ntv2_hin, ntv2_hin->name);
	if (IS_ERR(ntv2_hin->monitor_task)) {
		ntv2_hin->monitor_task = NULL;
		return -ENOMEM;
	}

	ntv2_hin->monitor_state = ntv2_task_state_enable;

	return 0;
}

int ntv2_hdmiin_disable(struct ntv2_hdmiin *ntv2_hin)
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

int ntv2_hdmiin_get_input_format(struct ntv2_hdmiin *ntv2_hin,
								 struct ntv2_hdmiin_format *format)
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

static int ntv2_hdmiin_monitor(void* data)
{
	struct ntv2_hdmiin *ntv2_hin = (struct ntv2_hdmiin *)data;
	int res;

	if (ntv2_hin == NULL)
		return -EPERM;

	NTV2_MSG_HDMIIN_STATE("%s: hdmi input monitor task start\n", ntv2_hin->name);

	res = ntv2_hdmiin_initialize(ntv2_hin);

	while(!kthread_should_stop()) {
		if (res == 0) {
			ntv2_hdmiin_periodic_update(ntv2_hin);
		}
		msleep_interruptible(100);
	}

	NTV2_MSG_HDMIIN_STATE("%s: hdmi input monitor task stop\n", ntv2_hin->name);

	return 0;
}

static int ntv2_hdmiin_initialize(struct ntv2_hdmiin *ntv2_hin)
{
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	int res;
	int i;

	/* initialize periodic update state */
	ntv2_hin->relock_reports = 0;
	ntv2_hin->hdmi_mode = false;
	ntv2_hin->hdcp_mode = false;
	ntv2_hin->derep_mode = false;;
	ntv2_hin->uhd_mode = false;
	ntv2_hin->cable_present = false;
	ntv2_hin->clock_present = false;
	ntv2_hin->input_locked = false;
	ntv2_hin->pixel_double_mode = false;
	ntv2_hin->avi_packet_present = false;
	ntv2_hin->vsi_packet_present = false;
	ntv2_hin->interlaced_mode = false;
	ntv2_hin->deep_color_10bit = false;
	ntv2_hin->deep_color_12bit = false;
	ntv2_hin->yuv_mode = false;
	ntv2_hin->prefer_yuv = false;
	ntv2_hin->prefer_rgb = false;
	ntv2_hin->relock_reports = NTV2_REPORT_ANY;

	/* initialize hdmi avi vic to ntv2 standard and rate table */
	for (i = 0; i < NTV2_AVI_VIC_INFO_SIZE; i++) {
		ntv2_avi_vic_info[i].video_standard = ntv2_kona_video_standard_none;
		ntv2_avi_vic_info[i].frame_rate = ntv2_kona_frame_rate_none;
	}

	ntv2_avi_vic_info[4].video_standard = ntv2_kona_video_standard_720p;
	ntv2_avi_vic_info[4].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[5].video_standard = ntv2_kona_video_standard_1080i;
	ntv2_avi_vic_info[5].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[6].video_standard = ntv2_kona_video_standard_525i;
	ntv2_avi_vic_info[6].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[7].video_standard = ntv2_kona_video_standard_525i;
	ntv2_avi_vic_info[7].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[16].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[16].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[19].video_standard = ntv2_kona_video_standard_720p;
	ntv2_avi_vic_info[19].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[20].video_standard = ntv2_kona_video_standard_1080i;
	ntv2_avi_vic_info[20].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[21].video_standard = ntv2_kona_video_standard_625i;
	ntv2_avi_vic_info[21].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[22].video_standard = ntv2_kona_video_standard_625i;
	ntv2_avi_vic_info[22].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[31].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[31].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[32].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[32].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_avi_vic_info[33].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[33].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[34].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[34].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[68].video_standard = ntv2_kona_video_standard_720p;
	ntv2_avi_vic_info[68].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[69].video_standard = ntv2_kona_video_standard_720p;
	ntv2_avi_vic_info[69].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[72].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[72].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_avi_vic_info[73].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[73].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[74].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[74].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[75].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[75].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[76].video_standard = ntv2_kona_video_standard_1080p;
	ntv2_avi_vic_info[76].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[93].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[93].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_avi_vic_info[94].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[94].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[95].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[95].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[96].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[96].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[97].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[97].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[98].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_avi_vic_info[98].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_avi_vic_info[99].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_avi_vic_info[99].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[100].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_avi_vic_info[100].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[101].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_avi_vic_info[101].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[102].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_avi_vic_info[102].frame_rate = ntv2_kona_frame_rate_6000;
	ntv2_avi_vic_info[103].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[103].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_avi_vic_info[104].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[104].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_avi_vic_info[105].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[105].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_avi_vic_info[106].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[106].frame_rate = ntv2_kona_frame_rate_5000;
	ntv2_avi_vic_info[107].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_avi_vic_info[107].frame_rate = ntv2_kona_frame_rate_6000;

	/* initialize hdmi vsi vic to ntv2 standard and rate table */
	for (i = 0; i < NTV2_VSI_VIC_INFO_SIZE; i++) {
		ntv2_vsi_vic_info[i].video_standard = ntv2_kona_video_standard_none;
		ntv2_vsi_vic_info[i].frame_rate = ntv2_kona_frame_rate_none;
	}

	ntv2_vsi_vic_info[1].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_vsi_vic_info[1].frame_rate = ntv2_kona_frame_rate_3000;
	ntv2_vsi_vic_info[2].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_vsi_vic_info[2].frame_rate = ntv2_kona_frame_rate_2500;
	ntv2_vsi_vic_info[3].video_standard = ntv2_kona_video_standard_3840x2160p;
	ntv2_vsi_vic_info[3].frame_rate = ntv2_kona_frame_rate_2400;
	ntv2_vsi_vic_info[4].video_standard = ntv2_kona_video_standard_4096x2160p;
	ntv2_vsi_vic_info[4].frame_rate = ntv2_kona_frame_rate_2400;

	/* reset the hdmi input chip */
	ntv2_konai2c_set_device(i2c_reg, device_io_bank);
	ntv2_konai2c_write(i2c_reg, 0xff, 0x80);
	usleep_range(10000, 10000);

	/* configure hdmi input chip default state */
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_io_bank, init_io0, init_io0_size);
	if (res < 0)
		goto bad_write;

	/* verify some written data */
	res = ntv2_hdmiin_read_verify(ntv2_hin, device_io_bank, init_io0, init_io0_size);
	if (res < 0)
		goto bad_write;

	/* continue config */
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_hdmi_bank, init_hdmi1, init_hdmi1_size);
	if (res < 0)
		goto bad_write;
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_io_bank, init_io2_non4k, init_io2_non4k_size);
	if (res < 0)
		goto bad_write;
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_cp_bank, init_cp3, init_cp3_size);
	if (res < 0)
		goto bad_write;
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_repeater_bank, init_rep4, init_rep4_size);
	if (res < 0)
		goto bad_write;
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_dpll_bank, init_dpll5_non4k, init_dpll5_non4k_size);
	if (res < 0)
		goto bad_write;
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_hdmi_bank, init_hdmi6, init_hdmi6_size);
	if (res < 0)
		goto bad_write;

	/* load edid */
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_edid_bank, init_edid_g, init_edid_g_size);
	if (res < 0)
		goto bad_write;

	/* final config */
	res = ntv2_hdmiin_write_multi(ntv2_hin, device_hdmi_bank, init_hdmi8, init_hdmi8_size);
	if (res < 0)
		goto bad_write;

	return 0;

bad_write:
	return -EINVAL;
}

static int ntv2_hdmiin_write_multi(struct ntv2_hdmiin *ntv2_hin,
								   u8 device,
								   struct ntv2_reg_value *reg_value,
								   int count)
{
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	int i;
	int res;

	ntv2_konai2c_set_device(i2c_reg, device);
	for (i = 0; i < count; i++) {
		res = ntv2_konai2c_write(i2c_reg, reg_value[i].address, reg_value[i].value);
		if (res < 0) {
			NTV2_MSG_HDMIIN_ERROR("%s: *error* write multi failed  device %02x  address %02x\n",
								  ntv2_hin->name, device, reg_value[i].address);
			return res;
		}
	}

	return 0;
}

static int ntv2_hdmiin_read_verify(struct ntv2_hdmiin *ntv2_hin,
								   u8 device,
								   struct ntv2_reg_value *reg_value,
								   int count)
{
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	u8 val;
	int i;
	int res;

	ntv2_konai2c_set_device(i2c_reg, device);
	res = ntv2_konai2c_cache_update(i2c_reg);
	if (res < 0) {
			NTV2_MSG_HDMIIN_ERROR("%s: *error* read verify cache update failed  device %02x\n",
								  ntv2_hin->name, device);
			return res;
	}

	for (i = 0; i < count; i++) {
		val = ntv2_konai2c_cache_read(i2c_reg, reg_value[i].address);
		if (val != reg_value[i].value) {
			NTV2_MSG_HDMIIN_ERROR("%s: *error* read verify failed  device %02x  address %02x  read %02x  expected %02x\n",
								  ntv2_hin->name, device, reg_value[i].address, val, reg_value[i].value);
		}
	}

	return 0;
}

int ntv2_hdmiin_periodic_update(struct ntv2_hdmiin *ntv2_hin)
{
	struct ntv2_register *vid_reg;
	struct ntv2_konai2c *i2c_reg;
	struct ntv2_hdmiin_format dvi_format;
	struct ntv2_hdmiin_format hdmi_format;
	struct ntv2_hdmiin_format input_format;
	bool tmds_lock_change = false;
	bool tmds_frequency_change = false;
	bool derep_on = false;
	bool yuv_input;
	bool yuv_output;
	u8 data = 0;
	u32 val = 0;
	int res = 0;

	if (ntv2_hin == NULL)
		return -EPERM;

	vid_reg = ntv2_hin->vid_reg;
	i2c_reg = ntv2_hin->i2c_reg;

	/* read io bank */
	ntv2_konai2c_set_device(i2c_reg, device_io_bank);
	res = ntv2_konai2c_cache_update(i2c_reg);
	if (res < 0) {
			NTV2_MSG_HDMIIN_ERROR("%s: *error* io bank read cache update failed\n",
								  ntv2_hin->name);
			return res;
	}

	/* check tmds lock transition */
	data = ntv2_konai2c_cache_read(i2c_reg, tmds_lock_detect_reg);
	tmds_lock_change = (data & tmds_lock_detect_mask) == tmds_lock_detect_mask;
	if (tmds_lock_change) {
		ntv2_konai2c_write(i2c_reg, tmds_lock_clear_reg, tmds_lock_clear_mask);
		NTV2_MSG_HDMIIN_STATE("%s: tmds lock transition detected\n",
							  ntv2_hin->name);
		ntv2_hin->tmds_frequency = 0;
		ntv2_hin->relock_reports = NTV2_REPORT_ANY;
	}

	/* tmds clock frequency transition */
	data = ntv2_konai2c_cache_read(i2c_reg, tmds_frequency_detect_reg);
	tmds_frequency_change = (data & tmds_frequency_detect_mask) == tmds_frequency_detect_mask;
	if (tmds_frequency_change) {
		ntv2_konai2c_write(i2c_reg, tmds_frequency_clear_reg, tmds_frequency_clear_mask);
		NTV2_MSG_HDMIIN_STATE("%s: tmds frequency transistion detected\n",
							  ntv2_hin->name);
		/* this happens on switch to uhd mode */
		ntv2_hin->relock_reports = NTV2_REPORT_ANY;
	}

	/* cable detect */
	data = ntv2_konai2c_cache_read(i2c_reg, cable_detect_reg);
	ntv2_hin->cable_present = (data & cable_detect_mask) == cable_detect_mask;
	if ((ntv2_hin->relock_reports & NTV2_REPORT_CABLE) != 0)	{
		NTV2_MSG_HDMIIN_STATE("%s: cable %s\n",
							  ntv2_hin->name, 
							  (ntv2_hin->cable_present) ? "present" : "absent")
		ntv2_hin->relock_reports &= ~NTV2_REPORT_CABLE;
	}

	/* check input clock */
	data = ntv2_konai2c_cache_read(i2c_reg, clock_detect_reg);
	ntv2_hin->clock_present = (data & clock_tmdsa_lock_mask) == clock_tmdsa_lock_mask;
	if ((ntv2_hin->relock_reports & NTV2_REPORT_SYNC) != 0)	{
		NTV2_MSG_HDMIIN_STATE("%s: tmds clock %s/%s  sync %s  regen %s\n",
							  ntv2_hin->name, 
							  (data & clock_tmdsa_present_mask) ? "present" : "absent",
							  (data & clock_tmdsa_lock_mask) ? "locked" : "unlocked",
							  (data & clock_sync_lock_mask) ? "locked" : "unlocked",
							  (data & clock_regen_lock_mask) ? "locked" : "unlocked");
		ntv2_hin->relock_reports &= ~NTV2_REPORT_SYNC;
	}

	/* avi/vsi packet detection */
	data = ntv2_konai2c_cache_read(i2c_reg, packet_detect_reg);
	ntv2_hin->avi_packet_present = (data & packet_detect_avi_mask) == packet_detect_avi_mask;
	ntv2_hin->vsi_packet_present = (data & packet_detect_vsi_mask) == packet_detect_vsi_mask;

	/* if pll lock was lost, recover to a state where we can take valid measurements */
	if (tmds_lock_change) {
		if (ntv2_hin->uhd_mode)
			ntv2_hdmiin_set_uhd_mode(ntv2_hin, false);

		if (ntv2_hin->derep_mode)
			ntv2_hdmi_set_derep_mode(ntv2_hin, false);

		ntv2_hdmiin_set_no_video(ntv2_hin);
		return 0;
	}

	if (!ntv2_hin->clock_present)
	{
		ntv2_hdmiin_set_no_video(ntv2_hin);
		return 0;
	}

	/* read hdmi bank */
	ntv2_konai2c_set_device(i2c_reg, device_hdmi_bank);
	res = ntv2_konai2c_cache_update(i2c_reg);
	if (res < 0) {
			NTV2_MSG_HDMIIN_ERROR("%s: *error* hdmi bank read cache update failed\n",
								  ntv2_hin->name);
			return res;
	}

	/* hdmi/dvi mode */
	data = ntv2_konai2c_cache_read(i2c_reg, hdmi_mode_reg);
	ntv2_hin->hdmi_mode = (data & hdmi_mode_mask) != 0;
	ntv2_hin->hdcp_mode = (data & hdmi_encrypted_mask) != 0;

	/* deep color mode */
	data = ntv2_konai2c_cache_read(i2c_reg, deep_color_mode_reg);
	ntv2_hin->deep_color_10bit = (data & deep_color_10bit_mask) != 0;
	ntv2_hin->deep_color_12bit = (data & deep_color_12bit_mask) != 0;

	/* dereplicator mode */
	data = ntv2_konai2c_cache_read(i2c_reg, derep_mode_reg);
	ntv2_hin->derep_mode = (data & derep_mode_mask) != 0;

	/* input locked */
	data = ntv2_konai2c_cache_read(i2c_reg, defilter_lock_detect_reg);
	ntv2_hin->input_locked = ((data & defilter_locked_mask) != 0) &&
		((data & vfilter_locked_mask) != 0);
	ntv2_hin->audio_multichannel = (data & audio_multichannel_mask) != 0;

	/* interlaced mode */
	data = ntv2_konai2c_cache_read(i2c_reg, interlaced_detect_reg);
	ntv2_hin->interlaced_mode = (data & interlaced_mask) != 0;

	/* audio detect */
	data = ntv2_konai2c_cache_read(i2c_reg, audio_detect_reg);
	ntv2_hin->audio_present = (data & audio_detect_mask) != 0;

	/* audio locked */
	data = ntv2_konai2c_cache_read(i2c_reg, audio_lock_reg);
	ntv2_hin->audio_locked = (data & audio_lock_mask) != 0;

	/* can not decrypt hdcp */
	if (ntv2_hin->hdcp_mode) {
		ntv2_hdmiin_set_no_video(ntv2_hin);
		return 0;
	}

	/* get current tmds frequency, does not work in uhd mode */
	if ((ntv2_hin->relock_reports & NTV2_REPORT_FREQ) && !ntv2_hin->uhd_mode) {
		/* read the new tmds fequency */
		ntv2_hdmiin_update_tmds_freq(ntv2_hin);
		/* update the pixel clock */
		ntv2_hdmiin_config_pixel_clock(ntv2_hin);

		NTV2_MSG_HDMIIN_STATE("%s: tmds frequency %d kHz\n",
							  ntv2_hin->name, ntv2_hin->tmds_frequency);
		ntv2_hin->relock_reports &= ~NTV2_REPORT_FREQ;
		return 0;
	}

	if (ntv2_hin->input_locked && !ntv2_hin->hdmi_mode)
	{
		/*
		  When in DVI mode and pixels get doubled, we must also turn on the
		  pixel dereplicator. This will halve the measurements and make
		  the video be the right size downstream. Note that in HDMI mode,
		  the dereplicator is automatically driven by the AVI info frame data.
		*/
		derep_on = (ntv2_hin->pixel_double_mode && ntv2_hin->interlaced_mode);
		/*
		  Turning on the pixel dereplicator is not reliable. Must check whether
		  the last such request really "took" and keep trying.
		*/
		if (derep_on && !ntv2_hin->derep_mode) {
			ntv2_hdmi_set_derep_mode(ntv2_hin, true);
			return 0;
		}
		if (!derep_on && ntv2_hin->derep_mode) {
			ntv2_hdmi_set_derep_mode(ntv2_hin, false);
			return 0;
		}
	}

	if (ntv2_hin->relock_reports & NTV2_REPORT_INFO) {
		val = 8;
		if (ntv2_hin->deep_color_10bit) val = 10;
		if (ntv2_hin->deep_color_12bit) val = 12;
		NTV2_MSG_HDMIIN_STATE("%s: input %s  mode %s  hdcp %s  derep %s  interlaced %d  depth %d\n",
							  ntv2_hin->name,
							  ntv2_hin->input_locked? "locked" : "unlocked",
							  ntv2_hin->hdmi_mode? "hdmi" : "dvi",
							  ntv2_hin->hdcp_mode? "on" : "off",
							  ntv2_hin->derep_mode? "on" : "off",
							  ntv2_hin->interlaced_mode,
							  val);
		ntv2_hin->relock_reports &= ~NTV2_REPORT_INFO;
	}

	if (!ntv2_hin->input_locked)
	{
		ntv2_hin->relock_reports |= NTV2_REPORT_INFO;
		return 0;
	}

	/* update timing values */
	if (ntv2_hin->relock_reports & NTV2_REPORT_TIMING) {
		ntv2_hdmiin_update_timing(ntv2_hin);
		NTV2_MSG_HDMIIN_STATE("%s: horizontal  active %d  total %d  fp %d  sync %d  bp %d\n",
							  ntv2_hin->name,
							  ntv2_hin->h_active_pixels,
							  ntv2_hin->h_total_pixels,
							  ntv2_hin->h_front_porch_pixels,
							  ntv2_hin->h_sync_pixels,
							  ntv2_hin->h_back_porch_pixels);
		NTV2_MSG_HDMIIN_STATE("%s: vertical  active %d/%d  total %d/%d  fp %d/%d  sync %d/%d  bp %d/%d  freq %d\n",
							  ntv2_hin->name,
							  ntv2_hin->v_active_lines0,
							  ntv2_hin->v_active_lines1,
							  ntv2_hin->v_total_lines0,
							  ntv2_hin->v_total_lines1,
							  ntv2_hin->v_front_porch_lines0,
							  ntv2_hin->v_front_porch_lines1,
							  ntv2_hin->v_sync_lines0,
							  ntv2_hin->v_sync_lines1,
							  ntv2_hin->v_back_porch_lines0,
							  ntv2_hin->v_back_porch_lines1,
							  ntv2_hin->v_frequency);
		ntv2_hin->relock_reports &= ~NTV2_REPORT_TIMING;
	}

	/* 
		sanity check - sometimes the hdmi chip provides bad values after a transition when 
		the pixel clock is too far off frequency (as in the certification test)
	*/
	if ((ntv2_hin->v_active_lines0 < 200) || (ntv2_hin->v_total_lines0 < 200) ||
		(ntv2_hin->h_sync_pixels < 2) || (ntv2_hin->h_active_pixels < 600) ||
		(ntv2_hin->v_sync_lines0 == 0) || (ntv2_hin->v_active_lines0 < 200) ||
		(ntv2_hin->v_total_lines0 < 200) || (ntv2_hin->v_frequency < 10) ||
		(ntv2_hin->interlaced_mode &&
		 ((ntv2_hin->v_active_lines1 < 200) || (ntv2_hin->v_total_lines1 < 200) ||
		  (ntv2_hin->v_sync_lines1 == 0)))) {
		ntv2_hin->relock_reports = NTV2_REPORT_ANY;
		return 0;
	}

	/* switch modes for uhd */
	if ((ntv2_hin->h_active_pixels > 2048) &&
		(!ntv2_hin->uhd_mode)) {
		ntv2_hdmiin_set_uhd_mode(ntv2_hin, true);
		return 0;
	}

	dvi_format.video_standard = ntv2_kona_video_standard_none;
	dvi_format.frame_rate = ntv2_kona_frame_rate_none;
	dvi_format.frame_flags = 0;
	dvi_format.pixel_flags = 0;
		
	/* determine input format from timing */
	ntv2_hdmiin_find_dvi_format(ntv2_hin, &dvi_format);
	if (ntv2_hin->relock_reports & NTV2_REPORT_DVI)
	{
		NTV2_MSG_HDMIIN_STATE("%s: dvi  standard %s  rate %s  frame %08x  pixel %08x\n",
							  ntv2_hin->name,
							  ntv2_video_standard_name(dvi_format.video_standard),
							  ntv2_frame_rate_name(dvi_format.frame_rate),
							  dvi_format.frame_flags,
							  dvi_format.pixel_flags);

		ntv2_hin->relock_reports &= ~NTV2_REPORT_DVI;
	}

	/* setup is done, probably an unsupported input format */
	if ((dvi_format.video_standard == ntv2_kona_video_standard_none) ||
		(dvi_format.frame_rate == ntv2_kona_frame_rate_none)) {
		return 0;
	}

	hdmi_format.video_standard = ntv2_kona_video_standard_none;
	hdmi_format.frame_rate = ntv2_kona_frame_rate_none;
	hdmi_format.frame_flags = 0;
	hdmi_format.pixel_flags = 0;

	if (ntv2_hin->hdmi_mode)
	{
		/* determine input format from hdmi info */
		if (ntv2_hin->relock_reports & NTV2_REPORT_HDMI)
		{
			/* read info bank */
			ntv2_konai2c_set_device(i2c_reg, device_info_bank);
			res = ntv2_konai2c_cache_update(i2c_reg);
			if (res < 0) {
				NTV2_MSG_HDMIIN_ERROR("%s: *error* info bank read cache update failed\n",
									  ntv2_hin->name);
				return res;
			}

			ntv2_hdmiin_find_hdmi_format(ntv2_hin, &hdmi_format);
			NTV2_MSG_HDMIIN_STATE("%s: hdmi standard %s  rate %s  frame %08x  pixel %08x\n",
								  ntv2_hin->name,
								  ntv2_video_standard_name(hdmi_format.video_standard),
								  ntv2_frame_rate_name(hdmi_format.frame_rate),
								  hdmi_format.frame_flags,
								  hdmi_format.pixel_flags);

			NTV2_MSG_HDMIIN_STATE("%s: hdmi audio present %s  locked %s  multichannel %s\n",
								  ntv2_hin->name,
								  ntv2_hin->audio_present? "true" : "false",
								  ntv2_hin->audio_locked? "true" : "false",
								  ntv2_hin->audio_multichannel? "true" : "false");

			ntv2_hin->relock_reports &= ~NTV2_REPORT_HDMI;
		}
	}

	/* initialize formats */
	input_format.video_standard = ntv2_kona_video_standard_none;
	input_format.frame_rate = ntv2_kona_frame_rate_none;
	input_format.frame_flags = 0;
	input_format.pixel_flags = 0;
	input_format.audio_detect = 0;

	if (ntv2_hin->relock_reports & NTV2_REPORT_FORMAT)
	{
		/* determine source format */
		if (hdmi_format.video_standard == dvi_format.video_standard) {
			input_format.video_standard = hdmi_format.video_standard;
			input_format.frame_rate = dvi_format.frame_rate;
			input_format.frame_flags = hdmi_format.frame_flags;
			input_format.pixel_flags = hdmi_format.pixel_flags;
			input_format.audio_detect = ntv2_hin->audio_present?
				(ntv2_hin->audio_multichannel? 0xf : 0x1) : 0x0;
		} else {
			input_format.video_standard = dvi_format.video_standard;
			input_format.frame_rate = dvi_format.frame_rate;
			input_format.frame_flags = dvi_format.frame_flags;
			input_format.pixel_flags = dvi_format.pixel_flags;
		}

		/* configure output color space */
		yuv_input = (input_format.pixel_flags & ntv2_kona_pixel_yuv) != 0;
		yuv_output = yuv_input;
		if (!ntv2_hin->uhd_mode) {
			if (ntv2_hin->prefer_yuv)
				yuv_output = true;
			if (ntv2_hin->prefer_rgb)
				yuv_output = false;
		}
		ntv2_hdmiin_set_color_mode(ntv2_hin, yuv_input, yuv_output);

		/* correct output pixel flags */
		if (yuv_output) {
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_rgb) | ntv2_kona_pixel_yuv;
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_444) | ntv2_kona_pixel_422;
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_full) | ntv2_kona_pixel_smpte;
			if (!yuv_input) {
				if ((input_format.pixel_flags & ntv2_kona_pixel_10bit) != 0) {
					input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_10bit) | ntv2_kona_pixel_12bit;
				}
				if ((input_format.pixel_flags & ntv2_kona_pixel_8bit) != 0) {
					input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_8bit) | ntv2_kona_pixel_10bit;
				}
			}
		} else {
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_yuv) | ntv2_kona_pixel_rgb;
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_422) | ntv2_kona_pixel_444;
			input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_smpte) | ntv2_kona_pixel_full;
			if (yuv_input) {
				if ((input_format.pixel_flags & ntv2_kona_pixel_10bit) != 0) {
					input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_10bit) | ntv2_kona_pixel_8bit;
				}
				if ((input_format.pixel_flags & ntv2_kona_pixel_12bit) != 0) {
					input_format.pixel_flags = (input_format.pixel_flags & ~ntv2_kona_pixel_8bit) | ntv2_kona_pixel_10bit;
				}
			}
		}

		ntv2_hdmiin_set_video_format(ntv2_hin, &input_format);

		NTV2_MSG_HDMIIN_STATE("%s: video standard %s  rate %s  frame %08x  pixel %08x  audio %08x\n",
							  ntv2_hin->name,
							  ntv2_video_standard_name(input_format.video_standard),
							  ntv2_frame_rate_name(input_format.frame_rate),
							  input_format.frame_flags,
							  input_format.pixel_flags,
							  input_format.audio_detect);

		ntv2_hin->relock_reports &= ~NTV2_REPORT_FORMAT;
	}

	return 0;
}

static int ntv2_hdmiin_set_color_mode(struct ntv2_hdmiin *ntv2_hin, bool yuv_input, bool yuv_output)
{
	ntv2_konai2c_set_device(ntv2_hin->i2c_reg, device_io_bank);

	if (ntv2_hin->uhd_mode) {
		if (yuv_input) {
			ntv2_konai2c_write(ntv2_hin->i2c_reg, 0x03, 0x96);
			ntv2_hin->yuv_mode = true;
		} else {
			ntv2_konai2c_write(ntv2_hin->i2c_reg, 0x03, 0x54);
			ntv2_hin->yuv_mode = false;
		}
	} else {
		if (yuv_output) {
			ntv2_konai2c_rmw(ntv2_hin->i2c_reg, 0x02, 0x04, 0x06);
			ntv2_konai2c_write(ntv2_hin->i2c_reg, 0x03, 0x82);
			ntv2_hin->yuv_mode = true;
		} else {
			ntv2_konai2c_rmw(ntv2_hin->i2c_reg, 0x02, 0x02, 0x06);
			ntv2_konai2c_write(ntv2_hin->i2c_reg, 0x03, 0x42);
			ntv2_hin->yuv_mode = false;
		}
	}

	return 0;
}

static int ntv2_hdmiin_set_uhd_mode(struct ntv2_hdmiin *ntv2_hin, bool enable)
{
	int res;

	if (enable)
	{
		NTV2_MSG_HDMIIN_STATE("%s: enable uhd mode\n", ntv2_hin->name);
		res = ntv2_hdmiin_write_multi(ntv2_hin, device_io_bank, init_io2_4k, init_io2_4k_size);
		if (res < 0)
			return res;;
		res = ntv2_hdmiin_write_multi(ntv2_hin, device_dpll_bank, init_dpll5_4k, init_dpll5_4k_size);
		if (res < 0)
			return res;;
	} else {
		NTV2_MSG_HDMIIN_STATE("%s: disable uhd mode\n", ntv2_hin->name);
		res = ntv2_hdmiin_write_multi(ntv2_hin, device_io_bank, init_io2_non4k, init_io2_non4k_size);
		if (res < 0)
			return res;;
		res = ntv2_hdmiin_write_multi(ntv2_hin, device_dpll_bank, init_dpll5_non4k, init_dpll5_non4k_size);
		if (res < 0)
			return res;
	}

	ntv2_hin->uhd_mode = enable;

	return 0;
}

static int ntv2_hdmi_set_derep_mode(struct ntv2_hdmiin *ntv2_hin, bool enable)
{
	int res;

	NTV2_MSG_HDMIIN_STATE("%s: %s derep mode\n", ntv2_hin->name, enable? "enable" : "disable");

	ntv2_konai2c_set_device(ntv2_hin->i2c_reg, device_hdmi_bank);
	res = ntv2_konai2c_write(ntv2_hin->i2c_reg, 0x41, enable? 0x11 : 0x00);
	if (res < 0)
		return res;

	ntv2_hin->derep_mode = enable;

	return 0;
}

static void ntv2_hdmiin_update_tmds_freq(struct ntv2_hdmiin *ntv2_hin)
{
	u32 hival;
	u32 loval;

	hival = (u32)ntv2_konai2c_cache_read(ntv2_hin->i2c_reg, 0x51);
	loval = (u32)ntv2_konai2c_cache_read(ntv2_hin->i2c_reg, 0x52);
	ntv2_hin->tmds_frequency = ((hival << 1) | (loval >> 7))*1000 +
		(loval & 0x7f)*1000/128;
}

static void ntv2_hdmiin_config_pixel_clock(struct ntv2_hdmiin *ntv2_hin)
{
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	u8 phase = 0;
	bool invert = false;

	/* high frequency clock phase */
	if (ntv2_hin->tmds_frequency >= 279000)
		phase = CLOCK_PHASE_HF;

	ntv2_konai2c_set_device(i2c_reg, device_hdmi_bank);
	if (ntv2_hin->tmds_frequency <= TMDS_DOUBLING_FREQ)
	{
		/* adi required for TMDS frequency 27Mhz and below */
		ntv2_konai2c_write(i2c_reg, 0x85, 0x11);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0x80);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0xC0);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0x00);
		ntv2_konai2c_write(i2c_reg, 0x85, 0x11);
		ntv2_konai2c_write(i2c_reg, 0x86, 0x9B);
		ntv2_konai2c_write(i2c_reg, 0x9B, 0x03);

		ntv2_konai2c_set_device(i2c_reg, device_io_bank);
		ntv2_konai2c_write(i2c_reg, 0x19, 0xC0 | phase);
		ntv2_hin->pixel_double_mode = true;
	}
	else
	{
		/* adi required for TMDS frequency above 27Mhz */
		ntv2_konai2c_write(i2c_reg, 0x85, 0x10);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0x80);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0xC0);
		ntv2_konai2c_write(i2c_reg, 0x9C, 0x00);
		ntv2_konai2c_write(i2c_reg, 0x85, 0x10);
		ntv2_konai2c_write(i2c_reg, 0x86, 0x9B);
		ntv2_konai2c_write(i2c_reg, 0x9B, 0x03);

		ntv2_konai2c_set_device(i2c_reg, device_io_bank);
		ntv2_konai2c_write(i2c_reg, 0x19, 0x80 | phase);
		ntv2_hin->pixel_double_mode = false;
	}

	if (invert)
		ntv2_konai2c_write(i2c_reg, 0x06, 0xa7);
}

static u32 ntv2_hdmiin_read_paired_value(struct ntv2_hdmiin *ntv2_hin, u8 reg, u32 bits, u32 shift)
{
	u8 msb;
	u8 lsb;
	u32 val;

	msb = ntv2_konai2c_cache_read(ntv2_hin->i2c_reg, reg);
	lsb = ntv2_konai2c_cache_read(ntv2_hin->i2c_reg, reg + 1);

	val = (msb << 8) | lsb;
	val &= ~(0xffffffff << bits);
	val >>= shift;

	return val;
}

static void ntv2_hdmiin_update_timing(struct ntv2_hdmiin *ntv2_hin)
{
	u32 total_lines = 0;
	u32 frame_rate = 0;

	ntv2_hin->h_active_pixels = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x07, 13, 0);
	ntv2_hin->h_total_pixels = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x1e, 13, 0);
	ntv2_hin->h_front_porch_pixels = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x20, 13, 0);
	ntv2_hin->h_sync_pixels = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x22, 13, 0);
	ntv2_hin->h_back_porch_pixels = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x24, 13, 0);

	ntv2_hin->v_active_lines0 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x09, 13, 0);
	ntv2_hin->v_total_lines0 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x26, 14, 1);
	ntv2_hin->v_front_porch_lines0 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x2a, 14, 1);
	ntv2_hin->v_sync_lines0 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x2e, 14, 1);
	ntv2_hin->v_back_porch_lines0 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x32, 14, 1);

	if (ntv2_hin->interlaced_mode) {
		ntv2_hin->v_active_lines1 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x0b, 13, 0);
		ntv2_hin->v_total_lines1 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x28, 14, 1);
		ntv2_hin->v_front_porch_lines1 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x2c, 14, 1);
		ntv2_hin->v_sync_lines1 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x30, 14, 1);
		ntv2_hin->v_back_porch_lines1 = ntv2_hdmiin_read_paired_value(ntv2_hin, 0x34, 14, 1);
	} else {
		ntv2_hin->v_active_lines1 = 0;
		ntv2_hin->v_total_lines1 = 0;
		ntv2_hin->v_front_porch_lines1 = 0;
		ntv2_hin->v_sync_lines1 = 0;
		ntv2_hin->v_back_porch_lines1 = 0;
	}

	/* compute frame rate (fps * 1000) */
	total_lines = ntv2_hin->v_total_lines0 + ntv2_hin->v_total_lines1;
	if ((total_lines != 0) && (ntv2_hin->h_total_pixels != 0)) {
		frame_rate = ntv2_hin->tmds_frequency * 1000 / total_lines;
		frame_rate = frame_rate * 1000 / ntv2_hin->h_total_pixels;
	}

	if (ntv2_hin->deep_color_10bit)
		frame_rate = frame_rate * 8 / 10;
	if (ntv2_hin->deep_color_12bit)
		frame_rate = frame_rate * 8 / 12;
	if (ntv2_hin->pixel_double_mode)
		frame_rate /= 2;
	ntv2_hin->v_frequency = frame_rate;
}

static void ntv2_hdmiin_find_dvi_format(struct ntv2_hdmiin *ntv2_hin,
										struct ntv2_hdmiin_format *format)
{
	u32 standard = ntv2_kona_video_standard_none;
	u32 rate = ntv2_kona_frame_rate_none;
	u32 f_flags = 0;
	u32 p_flags = 0;
	u32 width;
	u32 height;
	bool progressive;
	int input_rate = (int)ntv2_hin->v_frequency;
	int ntv2_rate;
	int min_diff = 1000000;
	int diff;
	u32 i;

	/* find ntv2 video standard */
	for (i = 0; i < NTV2_MAX_VIDEO_STANDARDS; i++) {
		width = ntv2_video_standard_width(i);
		height = ntv2_video_standard_height(i);
		progressive = ntv2_video_standard_progressive(i);
		if (height == 486)
			height = 480;
		if ((width == ntv2_hin->h_active_pixels) &&
			(height == (ntv2_hin->v_active_lines0 + ntv2_hin->v_active_lines1)) &&
			(progressive == !ntv2_hin->interlaced_mode)) {
			standard = i;
			break;
		}
	}

	/* find ntv2 frame rate */
	for (i = 0; i < NTV2_MAX_FRAME_RATES; i++) {
		ntv2_rate = ntv2_frame_rate_scale(i) * 1000 / ntv2_frame_rate_duration(i);
		diff = abs(ntv2_rate - input_rate);
		if (diff < (ntv2_rate * FRAME_RATE_TOLERANCE / 1000)) {
			if (diff < min_diff) {
				min_diff = diff;
				rate = i;
			}
		}
	}

	/* set ntv2 frame progressive/interlaced flags */
	if (ntv2_hin->interlaced_mode) {
		f_flags = ntv2_kona_frame_picture_interlaced | ntv2_kona_frame_transport_interlaced;
	} else {
		f_flags = ntv2_kona_frame_picture_progressive | ntv2_kona_frame_transport_progressive;
	}

	/* use tmds frequency to set frame rate class */
	if (ntv2_hin->tmds_frequency > 145000) {
		f_flags |= ntv2_kona_frame_3g;
	} else if (ntv2_hin->tmds_frequency > 74000) {
		f_flags |= ntv2_kona_frame_hd;
	} else {
		f_flags |= ntv2_kona_frame_sd;
	}

	/* set ntv2 pixel flags */
	p_flags = ntv2_kona_pixel_rgb |
		ntv2_kona_pixel_full |
		ntv2_kona_pixel_444 |
		ntv2_kona_pixel_8bit;
	if ((standard == ntv2_kona_video_standard_525i) ||
		(standard == ntv2_kona_video_standard_625i)) {
		f_flags |= ntv2_kona_frame_4x3;
		p_flags |= ntv2_kona_pixel_rec601;
	} else {
		f_flags |= ntv2_kona_frame_16x9;
		p_flags |= ntv2_kona_pixel_rec709;
	}		

	format->video_standard = standard;
	format->frame_rate = rate;
	format->frame_flags = f_flags;
	format->pixel_flags = p_flags;
}		

static void ntv2_hdmiin_find_hdmi_format(struct ntv2_hdmiin *ntv2_hin,
										 struct ntv2_hdmiin_format *format)
{
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	u32 standard = ntv2_kona_video_standard_none;
	u32 rate = ntv2_kona_frame_rate_none;
	u32 f_flags = 0;
	u32 p_flags = 0;
	u8 a_packet;
	u8 a_version;
	u8 a_byte[6];
	u8 v_packet;
	u8 v_version;
	u8 v_byte[6];
	u8 val;
	bool a_good = false;
	bool v_good = false;
	int i;
	
	/* read avi info data */
	if (ntv2_hin->avi_packet_present) {
		a_packet = ntv2_konai2c_cache_read(i2c_reg, avi_infoframe_packet_id);
		a_version = ntv2_konai2c_cache_read(i2c_reg, avi_infoframe_version);
		a_good = (a_packet == avi_packet_id) && (a_version == avi_version);
	}
	if (a_good) {
		for (i = 0; i < 5; i++) {
			a_byte[i + 1] = ntv2_konai2c_cache_read(i2c_reg, avi_infoframe_byte1 + i);
		}
	}
	else {
		goto done;
	}

	/* read vsi info data */
	if (ntv2_hin->vsi_packet_present) {
		v_packet = ntv2_konai2c_cache_read(i2c_reg, vsi_infoframe_packet_id);
		v_version = ntv2_konai2c_cache_read(i2c_reg, vsi_infoframe_version);
		v_good = (v_packet == vsi_packet_id) && (v_version == vsi_version);
	}
	if (v_good) {
		for (i = 0; i < 5; i++) {
			v_byte[i + 1] = ntv2_konai2c_cache_read(i2c_reg, vsi_infoframe_byte1 + i);
		}
	}

	/* scan for standard and rate */
	val = (a_byte[4] & avi_vic_mask4) >> avi_vic_shift4;
	if (val != 0) {
		if (val < NTV2_AVI_VIC_INFO_SIZE) {
			standard = ntv2_avi_vic_info[val].video_standard;
			rate = ntv2_avi_vic_info[val].frame_rate;
		}
	} else {
		standard = ntv2_kona_video_standard_none;
		if (v_good) {
			val = (v_byte[4] & vsi_video_format_mask4) >> vsi_video_format_shift4;
			if (val == vsi_format_extended) {
				val = v_byte[5];
				if (val < NTV2_VSI_VIC_INFO_SIZE) {
					standard = ntv2_vsi_vic_info[val].video_standard;
					rate = ntv2_vsi_vic_info[val].frame_rate;
				}
			}
		}
	}

	/* set frame progressive/interlace flags */
	if (ntv2_video_standard_progressive(standard)) {
		f_flags = ntv2_kona_frame_picture_progressive | ntv2_kona_frame_transport_progressive;
	} else {
		f_flags = ntv2_kona_frame_picture_interlaced | ntv2_kona_frame_transport_interlaced;
	}

	/* use tmds frequency to set frame rate class */
	if (ntv2_hin->tmds_frequency > 145000) {
		f_flags |= ntv2_kona_frame_3g;
	} else if (ntv2_hin->tmds_frequency > 74000) {
		f_flags |= ntv2_kona_frame_hd;
	} else {
		f_flags |= ntv2_kona_frame_sd;
	}

	/* scan for color component */
	val = (a_byte[1] & avi_color_component_mask1) >> avi_color_component_shift1;
	if (val == avi_color_comp_422) {
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_422;
	} else if (val == avi_color_comp_444) {
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_444;
	} else if (val == avi_color_comp_420) {
		p_flags |= ntv2_kona_pixel_yuv | ntv2_kona_pixel_420;
	} else {
		p_flags |= ntv2_kona_pixel_rgb | ntv2_kona_pixel_444;
	}

	/* scan for colorimetry */
	val = (a_byte[2] & avi_colorimetry_mask2) >> avi_colorimetry_shift2;
	if (val == avi_colorimetry_extended) {
		val = (a_byte[3] & avi_extended_colorimetry_mask3) >> avi_extended_colorimetry_shift3;
		if ((val == avi_ext_colorimetry_adobe_601) ||
			(val == avi_ext_colorimetry_adobe_rgb)) {
			p_flags |= ntv2_kona_pixel_adobe;
		} else if ((val == avi_ext_colorimetry_xv_ycc601) ||
				   (val == avi_ext_colorimetry_s_ycc601)) {
			p_flags |= ntv2_kona_pixel_rec601;
		} else if ((val == avi_ext_colorimetry_ycc2020) ||
				   (val == avi_ext_colorimetry_rgb2020)) {
			p_flags |= ntv2_kona_pixel_rec2020;
		} else {
			p_flags |= ntv2_kona_pixel_rec709;
		}
	} else if (val == avi_colorimetry_smpte170m) {
		p_flags |= ntv2_kona_pixel_rec601;
	} else {
		p_flags |= ntv2_kona_pixel_rec709;
	}

	/* scan for black/white range */
	if ((p_flags & ntv2_kona_pixel_rgb) != 0) {
		val = (a_byte[3] & avi_quantization_range_mask3) >> avi_quantization_range_shift3;
		if (val == avi_rgb_quant_range_limited) {
			p_flags |= ntv2_kona_pixel_smpte;
		} else {
			p_flags |= ntv2_kona_pixel_full;
		}
	} else {
		p_flags |= ntv2_kona_pixel_smpte;
	}

	/* scan for aspect ratio */
	val = (a_byte[2] & avi_frame_aspect_ratio_mask2) >> avi_frame_aspect_ratio_shift2;
	if (val == avi_frame_aspect_4x3) {
		f_flags |= ntv2_kona_frame_4x3;
	} else {
		f_flags |= ntv2_kona_frame_16x9;
	}

	/* use detected deep color for bit depth */
	if ((p_flags & ntv2_kona_pixel_rgb) != 0) {
		if (ntv2_hin->deep_color_12bit) {
			p_flags |= ntv2_kona_pixel_12bit;
		} else if (ntv2_hin->deep_color_10bit) {
			p_flags |= ntv2_kona_pixel_10bit;
		} else {
			p_flags |= ntv2_kona_pixel_8bit;
		}
	} else {
		p_flags |= ntv2_kona_pixel_10bit;
	}

done:
	format->video_standard = standard;
	format->frame_rate = rate;
	format->frame_flags = f_flags;
	format->pixel_flags = p_flags;
}

static u32 ntv2_hdmiin_pixel_double(struct ntv2_hdmiin *ntv2_hin, u32 pixels)
{
	return ntv2_hin->pixel_double_mode ? pixels * 2 : pixels;
}

static int ntv2_hdmiin_set_video_format(struct ntv2_hdmiin *ntv2_hin,
										struct ntv2_hdmiin_format *format)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	u32 hdmiin_standard = ntv2_video_standard_to_hdmiin(format->video_standard);
	u32 hdmiin_rate = ntv2_frame_rate_to_hdmiin(format->frame_rate);
	unsigned long flags;
	u32 video_deep;
	u32 video_sd;
	u32 video_mode;
	u32 video_rgb;
	u32 video_map;
	u32 video_420;
	u32 video_uhd;
	u32 video_prog;
	u32 video_status;
	u32 h_sync_bp;
	u32 h_active;
	u32 h_blank;
	u32 v_sync_bp_fld1;
	u32 v_sync_bp_fld2;
	u32 v_active_fld1;
	u32 v_active_fld2;
	u32 video_setup;
	u32 horizontal_data;
	u32 hblank_data0;
	u32 hblank_data1;
	u32 vertical_data_fld1;
	u32 vertical_data_fld2;
	u32 input_status;

	/* good format ??? */
	if ((hdmiin_standard == ntv2_kona_hdmiin_video_standard_none) ||
		(hdmiin_rate == ntv2_kona_hdmiin_frame_rate_none))
		return -EINVAL;

	/* gather register field data */
	video_deep = ((format->pixel_flags & ntv2_kona_pixel_8bit) == 0) &&
		((format->pixel_flags & ntv2_kona_pixel_444) != 0);

	video_sd = (format->video_standard == ntv2_kona_video_standard_525i) ||
		(format->video_standard == ntv2_kona_video_standard_625i)? 1 : 0;

	if ((format->frame_flags & ntv2_kona_frame_3g) != 0) {
		video_mode = ntv2_kona_hdmiin_video_mode_3gsdi;
	} else if ((format->frame_flags & ntv2_kona_frame_hd) != 0) {
		video_mode = ntv2_kona_hdmiin_video_mode_hdsdi;
	} else {
		video_mode = ntv2_kona_hdmiin_video_mode_sdsdi;
	}

	video_rgb = ((format->pixel_flags & ntv2_kona_pixel_rgb) != 0)? 1 : 0;

	video_map = ntv2_kona_hdmiin_video_map_444_10bit;
	if ((format->pixel_flags & ntv2_kona_pixel_444) == 0)
		video_map = ntv2_kona_hdmiin_video_map_422_10bit;

	video_420 = ((format->pixel_flags & ntv2_kona_pixel_420) != 0)? 1 : 0;

	video_uhd = (hdmiin_standard == ntv2_kona_hdmiin_video_standard_4k)? 1 : 0;

	video_prog = ((format->frame_flags & ntv2_kona_frame_transport_progressive) != 0)? 1 : 0;

	h_blank = ntv2_hin->h_front_porch_pixels +
		ntv2_hin->h_sync_pixels +
		ntv2_hin->h_back_porch_pixels;

	h_sync_bp = 0;
	h_active = 0;
	v_sync_bp_fld1 = 0;
	v_sync_bp_fld2 = 0;
	v_active_fld1 = 0;
	v_active_fld2 = 0;

	if (!ntv2_hin->uhd_mode) {
		h_sync_bp = ntv2_hdmiin_pixel_double(ntv2_hin,
											 ntv2_hin->h_sync_pixels +
											 ntv2_hin->h_back_porch_pixels);
										 
		h_active = ntv2_hdmiin_pixel_double(ntv2_hin, ntv2_hin->h_active_pixels);

		v_sync_bp_fld1 = ntv2_hdmiin_pixel_double(ntv2_hin,
												  ntv2_hin->h_total_pixels *
												  (ntv2_hin->v_sync_lines0 +
												   ntv2_hin->v_back_porch_lines0) -
												  ntv2_hin->h_front_porch_pixels);

		v_sync_bp_fld2 = ntv2_hdmiin_pixel_double(ntv2_hin,
												  ntv2_hin->h_total_pixels *
												  (ntv2_hin->v_sync_lines1 +
												   ntv2_hin->v_back_porch_lines1) -
												  ntv2_hin->h_front_porch_pixels +
												  ntv2_hin->h_total_pixels/2);

		v_active_fld1 = ntv2_hdmiin_pixel_double(ntv2_hin,
												 ntv2_hin->v_active_lines0 *
												 ntv2_hin->h_total_pixels);

		v_active_fld2 = ntv2_hdmiin_pixel_double(ntv2_hin,
												 ntv2_hin->v_active_lines1 *
												 ntv2_hin->h_total_pixels);
	}										

	/* enable hdmi to fpga link */
	ntv2_konai2c_set_device(i2c_reg, device_io_bank);
	ntv2_konai2c_write(i2c_reg, tristate_reg, tristate_enable_outputs);

	/* setup fpga hdmi input data */
	video_setup = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_mode, video_mode);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_map, video_map);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_420, video_420);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_standard, hdmiin_standard);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_frame_rate, hdmiin_rate);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_3d_structure, ntv2_kona_hdmiin_3d_frame_packing);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_4k, video_uhd);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_progressive, video_prog);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_3d, 0);
	video_setup |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_3d_frame_pack_enable, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_video_setup, ntv2_hin->index, video_setup);

	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hsync_duration, ntv2_hin->index, h_sync_bp);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_h_active, ntv2_hin->index, h_active);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vsync_duration_fld1, ntv2_hin->index, v_sync_bp_fld1);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vsync_duration_fld2, ntv2_hin->index, v_sync_bp_fld2);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_v_active_fld1, ntv2_hin->index, v_active_fld1);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_v_active_fld2, ntv2_hin->index, v_active_fld2);

	video_status = ntv2_reg_read(vid_reg, ntv2_kona_reg_hdmiin_video_status, ntv2_hin->index);

	horizontal_data = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_h_total_pixels, ntv2_hin->h_total_pixels);
	horizontal_data |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_h_active_pixels, ntv2_hin->h_active_pixels);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_horizontal_data, ntv2_hin->index, horizontal_data);

	hblank_data0 = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_h_front_porch_pixels, ntv2_hin->h_front_porch_pixels);
	hblank_data0 |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_h_back_porch_pixels, ntv2_hin->h_back_porch_pixels);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hblank_data0, ntv2_hin->index, hblank_data0);

	hblank_data1 = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_hsync_pixels, ntv2_hin->h_sync_pixels);
	hblank_data1 |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_hblank_pixels, h_blank);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hblank_data1, ntv2_hin->index, hblank_data1);

	vertical_data_fld1 = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_v_total_lines, ntv2_hin->v_total_lines0);
	vertical_data_fld1 |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_v_active_lines, ntv2_hin->v_active_lines0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vertical_data_fld1, ntv2_hin->index, vertical_data_fld1);

	vertical_data_fld2 = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_v_total_lines, ntv2_hin->v_total_lines1);
	vertical_data_fld2 |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_v_active_lines, ntv2_hin->v_active_lines1);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vertical_data_fld2, ntv2_hin->index, vertical_data_fld2);

	/* set fpga hdmi status */
	input_status = NTV2_FLD_SET(ntv2_kona_fld_hdmiin_locked, (ntv2_hin->input_locked? 1 : 0));
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_stable, (ntv2_hin->input_locked? 1 : 0));
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_rgb, video_rgb);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_deep_color, video_deep);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_code, format->video_standard);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_8ch, 0);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_progressive, video_prog);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_sd, video_sd);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_74_25, 0);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_rate, 0);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_word_length, 0);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_format, format->video_standard);
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_dvi, (ntv2_hin->hdmi_mode? 0 : 1));
	input_status |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_rate, format->frame_rate);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_input_status, ntv2_hin->index, input_status);

	spin_lock_irqsave(&ntv2_hin->state_lock, flags);
	ntv2_hin->input_format = *format;
	spin_unlock_irqrestore(&ntv2_hin->state_lock, flags);
#if 0
	NTV2_MSG_HDMIIN_STATE("%s: video setup            %08x\n", ntv2_hin->name, video_setup);
	NTV2_MSG_HDMIIN_STATE("%s: h sync                 %08x\n", ntv2_hin->name, h_sync_bp);
	NTV2_MSG_HDMIIN_STATE("%s: h active               %08x\n", ntv2_hin->name, h_active);
	NTV2_MSG_HDMIIN_STATE("%s: v sync fld 1           %08x\n", ntv2_hin->name, v_sync_bp_fld1);
	NTV2_MSG_HDMIIN_STATE("%s: v sync fld 2           %08x\n", ntv2_hin->name, v_sync_bp_fld2);
	NTV2_MSG_HDMIIN_STATE("%s: v active fld 1         %08x\n", ntv2_hin->name, v_active_fld1);
	NTV2_MSG_HDMIIN_STATE("%s: v active fld 2         %08x\n", ntv2_hin->name, v_active_fld2);
	NTV2_MSG_HDMIIN_STATE("%s: video status           %08x\n", ntv2_hin->name, video_status);
	NTV2_MSG_HDMIIN_STATE("%s: h active:total         %08x\n", ntv2_hin->name, horizontal_data);
	NTV2_MSG_HDMIIN_STATE("%s: h back:front           %08x\n", ntv2_hin->name, hblank_data0);
	NTV2_MSG_HDMIIN_STATE("%s: h blank:sync           %08x\n", ntv2_hin->name, hblank_data1);
	NTV2_MSG_HDMIIN_STATE("%s: v active:total fld 1   %08x\n", ntv2_hin->name, vertical_data_fld1);
	NTV2_MSG_HDMIIN_STATE("%s: v active:total fld 2   %08x\n", ntv2_hin->name, vertical_data_fld2);
	NTV2_MSG_HDMIIN_STATE("%s: input status           %08x\n", ntv2_hin->name, input_status);
#endif
	return 0;
}

static void ntv2_hdmiin_set_no_video(struct ntv2_hdmiin *ntv2_hin)
{
	struct ntv2_register *vid_reg = ntv2_hin->vid_reg;
	struct ntv2_konai2c *i2c_reg = ntv2_hin->i2c_reg;
	unsigned long flags;
	u32 val = 0;

	/* disable hdmi to fpga link */
	ntv2_konai2c_set_device(i2c_reg, device_io_bank);
	ntv2_konai2c_write(i2c_reg, tristate_reg, tristate_disable_outputs);

	/* clear fpga hdmi input data */
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_video_setup, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hsync_duration, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_h_active, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vsync_duration_fld1, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vsync_duration_fld2, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_v_active_fld1, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_v_active_fld2, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_horizontal_data, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hblank_data0, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_hblank_data1, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vertical_data_fld1, ntv2_hin->index, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_vertical_data_fld2, ntv2_hin->index, 0);

	/* clear fpga status */
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_locked, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_stable, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_rgb, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_deep_color, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_code, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_8ch,	0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_progressive, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_sd, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_74_25, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_rate, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_audio_word_length,	0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_format, 0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_dvi, 1);
	val |= NTV2_FLD_SET(ntv2_kona_fld_hdmiin_video_rate, 0);
	ntv2_reg_write(vid_reg, ntv2_kona_reg_hdmiin_input_status, ntv2_hin->index, val);

	spin_lock_irqsave(&ntv2_hin->state_lock, flags);
	ntv2_hin->input_format.video_standard = ntv2_kona_video_standard_none;
	ntv2_hin->input_format.frame_rate = ntv2_kona_frame_rate_none;
	ntv2_hin->input_format.frame_flags = 0;
	ntv2_hin->input_format.pixel_flags = 0;
	ntv2_hin->input_format.audio_detect = 0;
	spin_unlock_irqrestore(&ntv2_hin->state_lock, flags);
}

