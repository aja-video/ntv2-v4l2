/*
 * NTV2 video stream channel ops
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

#include "ntv2_videoops.h"
#include "ntv2_channel.h"
#include "ntv2_konareg.h"
#include "ntv2_register.h"
#include "ntv2_features.h"

int ntv2_videoops_acquire_hardware(struct ntv2_channel_stream *stream);


int ntv2_videoops_setup_capture(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_features *features = ntv2_chn->features;
	int index = ntv2_chn->index;
	int buf_index;
	int result;
	int i;

	/* acquire video hardware resources */
	result = ntv2_videoops_acquire_hardware(stream);
	if (result != 0)
		return result;
	
	/* get the video frame buffer frame range and size */
	ntv2_features_get_frame_range(features,
								  &stream->video.video_format,
								  &stream->video.pixel_format,
								  index,
								  &stream->video.frame_first,
								  &stream->video.frame_last,
								  &stream->video.frame_size);

	/* initialize video input stream data */
	INIT_LIST_HEAD(&stream->data_ready_list);
	INIT_LIST_HEAD(&stream->data_done_list);
	stream->queue_run = false;
	stream->queue_last = false;
	stream->video.frame_active	= NULL;
	stream->video.frame_next	= NULL;
	stream->video.total_frame_count = 0;
	stream->video.total_drop_count = 0;
	stream->video.stat_frame_count = 0;
	stream->video.stat_drop_count = 0;
	stream->video.last_display_time = 0;
	for (i = 0; i < NTV2_MAX_CHANNELS; i++)
		stream->video.hardware_enable[i] = false;

	/* initialize video data buffers */
	buf_index = 0;
	for (i = stream->video.frame_first; i <= stream->video.frame_last; i++) {
		stream->data_array[buf_index].index = buf_index;
		stream->data_array[buf_index].type = stream->type;
		INIT_LIST_HEAD(&stream->data_array[buf_index].list);
		stream->data_array[buf_index].ntv2_str = stream;
		stream->data_array[buf_index].video.frame_number = i;
		stream->data_array[buf_index].video.address = i * stream->video.frame_size;
		stream->data_array[buf_index].video.data_size = 0;
		list_add_tail(&stream->data_array[buf_index].list, &stream->data_done_list);
		buf_index++;
	}

	/* initialize the frame store capture buffer */
	stream->video.frame_next = list_first_entry(&stream->data_done_list, struct ntv2_stream_data, list);
	list_del_init(&stream->video.frame_next->list);

	return 0;
}

int ntv2_videoops_release_capture(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_features *features = ntv2_chn->features;

	/* release hardware resources */
	ntv2_features_release_video_components(features, (unsigned long)stream);

	return 0;
}

int ntv2_videoops_update_mode(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	u32 val = stream->queue_enable? 1 : 0;
	u32 mask = NTV2_FLD_MASK(ntv2_kona_fld_frame_capture_enable);
	int i;

	/* enable/disable frame store capture mode */
	for (i = 0; i < NTV2_MAX_CHANNELS; i++) {
		if ((i >= stream->channel_index) && (i < (stream->channel_index + stream->num_channels))) {
			ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_frame_control, i, val, mask);
			stream->video.hardware_enable[i] = stream->queue_enable;
//			NTV2_MSG_INFO("%s: write frame control[%d]  enable\n", ntv2_chn->name, i);
		} else if (stream->video.hardware_enable[i]) {
			ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_frame_control, i, 0, mask);
			stream->video.hardware_enable[i] = false;
//			NTV2_MSG_INFO("%s: write frame control[%d]  disable\n", ntv2_chn->name, i);
		}
	}

	return 0;
}

int ntv2_videoops_update_format(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	u32 val;
	u32 mask;
	int i;

	val = NTV2_FLD_SET(ntv2_kona_fld_frame_buffer_format_b0123, stream->video.pixel_format.ntv2_pixel_format);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_frame_buffer_format_b0123);

	/* set frame store pixel format */
	for (i = stream->channel_index; i < (stream->channel_index + stream->num_channels); i++) {
		if (ntv2_chn->state == ntv2_channel_state_run) {
			ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_frame_control, i, val, mask);
		} else {
			ntv2_reg_write(ntv2_chn->vid_reg, ntv2_kona_reg_frame_control, i, val);
		}
//		NTV2_MSG_INFO("%s: write frame control[%d]  %08x\n", ntv2_chn->name, index + i, val);
	}

	return 0;
}

int ntv2_videoops_update_timing(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_input_format *input_format = &stream->video.input_format;
	struct ntv2_video_format *video_format = &stream->video.video_format;
	int index = stream->channel_index;
	int mode_372 = 0;
	int mode_sync = ntv2_kona_reg_sync_field;
	bool mode_tsi = false;
	bool mode_quad = false;
	u32 standard;
	u32 rate;
	u32 val;
	u32 msk;

	/* sync to frame for interlaced video */
	if ((video_format->frame_flags & ntv2_kona_frame_picture_interlaced) != 0)
		mode_sync = ntv2_kona_reg_sync_frame;
	
	/* look for two sample interleave video */
	if ((input_format->frame_flags & ntv2_kona_frame_sample_interleave) != 0)
		mode_tsi = 1;
	
	/* look for square division video */
	if ((input_format->frame_flags & ntv2_kona_frame_square_division) != 0)
		mode_quad = 1;
	
	/* kluge 372 mode */
	if ((input_format->num_inputs == 2) &&
		((input_format->frame_flags & ntv2_kona_frame_hd) != 0) &&
		((input_format->frame_flags & ntv2_kona_frame_line_interleave) != 0))
		mode_372 = 1;

	/* determine video standard and rate */
	standard = video_format->video_standard;
	rate = video_format->frame_rate;
	if (mode_372 == 1) {
		if (standard == ntv2_kona_video_standard_1080p)
			standard = ntv2_kona_video_standard_1080i;
		if (rate == ntv2_kona_frame_rate_5000) {
			rate = ntv2_kona_frame_rate_2500;
		} else if (rate == ntv2_kona_frame_rate_5994) {
			rate = ntv2_kona_frame_rate_2997;
		} else if (rate == ntv2_kona_frame_rate_6000) {
			rate = ntv2_kona_frame_rate_3000;
		}
	}

	NTV2_MSG_INFO("%s: input - std = %d, rate = %d, reg_idx = %d, input_idx = %d, num_inputs = %d, num_streams = %d",
				  ntv2_chn->name, input_format->video_standard, input_format->frame_rate, input_format->reg_index,
				  input_format->input_index, input_format->num_inputs, input_format->num_streams);

	NTV2_MSG_INFO("%s: fmt - name = %s, std = %d, geom = %d, rate = %d",
				  ntv2_chn->name,video_format->name,video_format->video_standard,video_format->frame_geometry,
				  video_format->frame_rate)

	NTV2_MSG_INFO("%s: mode_sync = %d, mode_tsi = %d, mode_quad = %d, mode_372 = %d, std = %d, rate = %d",
				  ntv2_chn->name, mode_sync, mode_tsi, mode_quad, mode_372, standard, rate);

	/* setup video timing, device sync source and sync mode */
	val = NTV2_FLD_SET(ntv2_kona_fld_global_frame_rate_b012, rate & 0x7);
	val |= NTV2_FLD_SET(ntv2_kona_fld_global_frame_rate_b3, rate >> 3);
	val |= NTV2_FLD_SET(ntv2_kona_fld_global_frame_geometry, video_format->frame_geometry);
	val |= NTV2_FLD_SET(ntv2_kona_fld_global_video_standard, standard);
	val |= NTV2_FLD_SET(ntv2_kona_fld_reference_source_b012, ntv2_kona_ref_source_sdiin1 & 0x7);
	val |= NTV2_FLD_SET(ntv2_kona_fld_linkb_p60_mode_ch2, mode_372);
	val |= NTV2_FLD_SET(ntv2_kona_fld_global_reg_sync, mode_sync);
	val |= NTV2_FLD_SET(ntv2_kona_fld_global_quad_tsi_enable, mode_tsi);
	ntv2_reg_write(ntv2_chn->vid_reg, ntv2_kona_reg_global_control, index, val);
	NTV2_MSG_INFO("%s: write global control  %08x\n", ntv2_chn->name, val);

	val = 0;
	msk = 0;
	if (mode_quad) {
		/* set quad bit for square division video */
		if ((stream->channel_index / 4) == 0) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs1234_quad_mode, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs1234_quad_mode);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb12_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb12_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb34_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb34_425mode_enable);
		} else if ((stream->channel_index / 4) == 1) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs5678_quad_mode, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs5678_quad_mode);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb56_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb56_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb78_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb78_425mode_enable);
		}
	} else if (mode_tsi) {
		/* set 425 bit for two sample interleave video */
		if ((stream->channel_index / 2) == 0) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb12_425mode_enable, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb12_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs1234_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs1234_quad_mode);
		} else if ((stream->channel_index / 2) == 1) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb34_425mode_enable, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb34_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs1234_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs1234_quad_mode);
		} else if ((stream->channel_index / 2) == 2) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb56_425mode_enable, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb56_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs5678_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs5678_quad_mode);
		} else if ((stream->channel_index / 2) == 3) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb78_425mode_enable, 1);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb78_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs5678_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs5678_quad_mode);
		}
	} else {
		if ((stream->channel_index / 2) == 0) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb12_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb12_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs1234_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs1234_quad_mode);
		} else if ((stream->channel_index / 2) == 1) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb34_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb34_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs1234_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs1234_quad_mode);
		} else if ((stream->channel_index / 2) == 2) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb56_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb56_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs5678_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs5678_quad_mode);
		} else if ((stream->channel_index / 2) == 3) {
			val |= NTV2_FLD_SET(ntv2_kona_fld_fb78_425mode_enable, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fb78_425mode_enable);
			val |= NTV2_FLD_SET(ntv2_kona_fld_fs5678_quad_mode, 0);
			msk |= NTV2_FLD_MASK(ntv2_kona_fld_fs5678_quad_mode);
		}
	}
	
	/* channels independent */
	val |= NTV2_FLD_SET(ntv2_kona_fld_independent_channel_enable, 1);
	msk |= NTV2_FLD_MASK(ntv2_kona_fld_independent_channel_enable);

	/* need to figure out how to handle reference source */
	val |= NTV2_FLD_SET(ntv2_kona_fld_reference_source_b3, ntv2_kona_ref_source_sdiin1 >> 3);
	msk |= NTV2_FLD_MASK(ntv2_kona_fld_reference_source_b3);
	
	ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_global_control2, 0, val, msk);
	NTV2_MSG_INFO("%s: write global control2 %08x/%08x\n", ntv2_chn->name, val, msk);

	return 0;
}

int ntv2_videoops_update_route(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_register *vid_reg = ntv2_chn->vid_reg;
	struct ntv2_input_format *input_format = &stream->video.input_format;
	struct ntv2_pixel_format *pixel_format = &stream->video.pixel_format;
	int chn_index = 0;
	int inp_index = 0;
	int csc_index = 0;
	int lut_index = 0;
	int csc2_index = 0;
	bool do_csc = false;
	bool convert3gb = false;
	bool in_rgb;
	bool fs_rgb;
	int i;

	/* get hardware components */
	chn_index = ntv2_chn->index;
	inp_index = input_format->input_index;
	csc_index = stream->video.csc_index;
	lut_index = stream->video.lut_index;
	
	/* determine input and pixel rgbness */
	in_rgb = (input_format->pixel_flags & ntv2_kona_pixel_rgb) != 0;
	fs_rgb = (pixel_format->pixel_flags & ntv2_kona_pixel_rgb) != 0;
	do_csc = (in_rgb && !fs_rgb) || (!in_rgb && fs_rgb);

	convert3gb =
		((input_format->frame_flags &
		  ntv2_kona_frame_3gb) != 0) &&
		((input_format->frame_flags &
		  ntv2_kona_frame_line_interleave) != 0);

	for (i = 0; i < input_format->num_inputs; i++) {
		/* set sdi to input mode */
		ntv2_sdi_output_transmit_enable(vid_reg, inp_index + i, false);
		/* convert 3gb to 3ga for line interleaved input */
		ntv2_sdi_input_convert_3g_enable(vid_reg, inp_index + i, convert3gb);
	}

	if (input_format->type == ntv2_input_type_sdi) {
		if ((input_format->frame_flags & ntv2_kona_frame_sample_interleave) != 0) {
			if (input_format->num_inputs == 2) {
				/* route 3gb tsi inputs */
				for (i = 0; i < 4; i++) {
					if (do_csc) {
						ntv2_route_sdi_to_csc(vid_reg,
											  inp_index + (i/2), i%2, in_rgb,
											  csc_index + i, 0);
						ntv2_route_csc_to_mux(vid_reg,
											  csc_index + i, 0, fs_rgb,
											  chn_index + (i/2), i%2);
						ntv2_route_mux_to_fs(vid_reg,
											 chn_index + (i/2), i%2, fs_rgb,
											 chn_index + (i/2), i%2);
					} else {
						ntv2_route_sdi_to_mux(vid_reg,
											  inp_index + (i/2), i%2, in_rgb,
											  chn_index + (i/2), i%2);
						ntv2_route_mux_to_fs(vid_reg,
											 chn_index + (i/2), i%2, fs_rgb,
											 chn_index + (i/2), i%2);
					}
				}
			} else {
				/* route 3ga tsi inputs */
				for (i = 0; i < 4; i++) {
					if (do_csc) {
						ntv2_route_sdi_to_csc(vid_reg,
											  inp_index + i, 0, in_rgb,
											  csc_index + i, 0);
						ntv2_route_csc_to_mux(vid_reg,
											  csc_index + i, 0, fs_rgb,
											  chn_index + (i/2), i%2);
						ntv2_route_mux_to_fs(vid_reg,
											 chn_index + (i/2), i%2, fs_rgb,
											 chn_index + (i/2), i%2);
					} else {
						ntv2_route_sdi_to_mux(vid_reg,
											  inp_index + i, 0, in_rgb,
											  chn_index + (i/2), i%2);
						ntv2_route_mux_to_fs(vid_reg,
											 chn_index + (i/2), i%2, fs_rgb,
											 chn_index + (i/2), i%2);
					}
				}
			}
		} else if (((input_format->frame_flags & ntv2_kona_frame_square_division) != 0) &&
				   (input_format->num_inputs == 2)) {
			/* route 3gb sqd inputs */
			for (i = 0; i < input_format->num_inputs; i++) {
				if (do_csc) {
					ntv2_route_sdi_to_csc(vid_reg,
										  inp_index + (i/2), i%2, in_rgb,
										  csc_index + i, 0);
					ntv2_route_csc_to_fs(vid_reg,
										 csc_index + i, 0, !in_rgb,
										 chn_index + i, 0);
				} else {
					ntv2_route_sdi_to_fs(vid_reg,
										 inp_index + (i/2), i%2, in_rgb,
										 chn_index + i, 0);
				}
			}
		} else {
			/* route 3ga and hd inputs */
			for (i = 0; i < input_format->num_inputs; i++) {
				if (do_csc) {
					ntv2_route_sdi_to_csc(vid_reg,
										  inp_index + i, 0, in_rgb,
										  csc_index + i, 0);
					ntv2_route_csc_to_fs(vid_reg,
										 csc_index + i, 0, !in_rgb,
										 chn_index + i, 0);
				} else {
					ntv2_route_sdi_to_fs(vid_reg,
										 inp_index + i, 0, in_rgb,
										 chn_index + i, 0);
				}
			}
		}
	}

	if ((input_format->type == ntv2_input_type_hdmi_adv) ||
		(input_format->type == ntv2_input_type_hdmi_aja)) {
		if ((input_format->frame_flags & ntv2_kona_frame_sample_interleave) != 0) {
			/* route hdmi tsi input */
			for (i = 0; i < 4; i++) {
				if (do_csc) {
					ntv2_route_hdmi_to_csc(vid_reg,
										   inp_index, i, in_rgb,
										   csc_index + i, 0);
					ntv2_route_csc_to_mux(vid_reg,
										  csc_index + i, 0, fs_rgb,
										  chn_index + (i/2), i%2);
					ntv2_route_mux_to_fs(vid_reg,
										 chn_index + (i/2), i%2, fs_rgb,
										 chn_index + (i/2), i%2);
				} else {
					ntv2_route_hdmi_to_mux(vid_reg,
										   inp_index, i, in_rgb,
										   chn_index + (i/2), i%2);
					ntv2_route_mux_to_fs(vid_reg,
										 chn_index + (i/2), i%2, fs_rgb,
										 chn_index + (i/2), i%2);
				}
			}
		} else if ((input_format->frame_flags & ntv2_kona_frame_square_division) != 0) {
			/* configure qrc for sqd input */
			ntv2_qrc_4k_enable(vid_reg, true, false);

			/* route hdmi sqd input */
			for (i = 0; i < input_format->num_streams; i++) {
				if (do_csc) {
					ntv2_route_hdmi_to_csc(vid_reg,
										   inp_index, i, in_rgb,
										   csc_index + i, 0);
					ntv2_route_csc_to_fs(vid_reg,
										 csc_index + i, 0, !in_rgb,
										 chn_index + i, 0);
				} else {
					ntv2_route_hdmi_to_fs(vid_reg,
										  inp_index, i, in_rgb,
										  chn_index + i, 0);
				}
			}
		} else {
			/* configure qrc for 3g and hd */
			ntv2_qrc_4k_enable(vid_reg, false, false);

			/* route 3g and hd input */
			if (do_csc) {
				ntv2_route_hdmi_to_csc(vid_reg,
									   inp_index, 0, in_rgb,
									   csc_index, 0);
				ntv2_route_csc_to_fs(vid_reg,
									 csc_index, 0, !in_rgb,
									 chn_index, 0);
			} else {
				ntv2_route_hdmi_to_fs(vid_reg,
									  inp_index, 0, in_rgb,
									  chn_index, 0);
			}
		}
	}

	if (input_format->type == ntv2_input_type_hdmi4k_aja) {
		/* configure qrc for 3g and hd */
		ntv2_qrc_4k_enable(vid_reg, false, false);

		if (in_rgb) {
			/* rgb input
				Input1 -> LUT1 -> CSC3 -> FrameBuffer1(yuv)
				Input2 -> LUT2 -> CSC4 -> FrameBuffer2(yuv)
			*/
			lut_index = inp_index;
			csc2_index = csc_index+2;

			ntv2_route_hdmi_to_lut(vid_reg,
								   inp_index, 0, true,
								   lut_index, 0);

			ntv2_route_lut_to_csc(vid_reg,
								  lut_index, 0,
								  csc2_index, 0);

			ntv2_route_csc_to_fs(vid_reg,
								 csc2_index, 0, false,
								 chn_index, 0);

		} else {
			/* yuv input
				Input1 -> CSC1 -> LUT1 -> CSC3 -> FrameBuffer1(yuv)
				Input2 -> CSC2 -> LUT2 -> CSC4 -> FrameBuffer2(yuv)
			*/
			lut_index = inp_index;
			csc2_index = csc_index+2;

			ntv2_route_hdmi_to_csc(vid_reg,
								   inp_index, 0, false,
								   csc_index, 0);

			ntv2_route_csc_to_lut(vid_reg,
								  csc_index, 0, true,
								  lut_index, 0);

			ntv2_route_lut_to_csc(vid_reg,
								  lut_index, 0,
								  csc2_index, 0);

			ntv2_route_csc_to_fs(vid_reg,
								 csc2_index, 0, false,
								 chn_index, 0);
		}
	}

	return 0;
}

int ntv2_videoops_update_frame(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	int index = ntv2_chn->index;

	/* set the frame store next frame buffer number */
	ntv2_reg_write(ntv2_chn->vid_reg,
				   ntv2_kona_reg_frame_input, index,
				   stream->video.frame_next->video.frame_number);

	return 0;
}

int ntv2_videoops_interrupt_capture(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_stream_data *data_ready;
	s64 stat_time = ntv2_chn->dpc_status.stat_time;
	s64 time_us;
	u32 val;
	int chn_index = ntv2_chn->index;
	int reg_index = 0;
	
	if (!stream->queue_enable)
		return 0;

	/* need an input interrupt */
	if(!ntv2_chn->dpc_status.interrupt_input)
		return 0;

	/* update time stamp */
	stream->timestamp = ntv2_chn->dpc_status.interrupt_time;

	/* this frame can be ready */
	data_ready = stream->video.frame_active;

	/* this is now the active frame */
	stream->video.frame_active = stream->video.frame_next;
	if (stream->video.frame_active != NULL)
		stream->video.frame_active->timestamp = stream->timestamp;

	if (stream->queue_run) {
		if (stream->queue_last) {
			stream->video.total_frame_count++;
			stream->video.stat_frame_count++;
		} else {
			stream->video.total_frame_count = 0;
			stream->video.total_drop_count = 0;
			stream->video.stat_frame_count = 0;
			stream->video.stat_drop_count = 0;
			stream->video.last_display_time = stat_time;
		}

		/* get the next data object */
		if (!list_empty(&stream->data_done_list)) {
			stream->video.frame_next = list_first_entry(&stream->data_done_list,
														struct ntv2_stream_data, list);
			list_del_init(&stream->video.frame_next->list);
		} else {
			stream->video.total_drop_count++;
			stream->video.stat_drop_count++;
		}

		/* add ready frame to queue */
		if ((stream->video.total_frame_count != 0) &&
			(data_ready != NULL) &&
			(data_ready->video.frame_number != stream->video.frame_active->video.frame_number)) {
			
			/* get frame address and size */
			data_ready->video.address = data_ready->video.frame_number * stream->video.frame_size;
			data_ready->video.data_size = stream->video.frame_size;

			/* get frame timecode */
			data_ready->video.timecode_present = false;
			data_ready->video.timecode_low = 0;
			data_ready->video.timecode_high = 0;
			if (stream->video.input_format.type == ntv2_input_type_sdi) {
				reg_index = stream->video.input_format.reg_index;
				val = ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_sdiin_timecode_rp188_dbb, reg_index);
				val = NTV2_FLD_GET(ntv2_kona_fld_sdiin_rp188_select_present, val);
				if (val == 1) {
					data_ready->video.timecode_present = true;
					data_ready->video.timecode_low =
						ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_sdiin_timecode_rp188_low, reg_index);
					data_ready->video.timecode_high =
						ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_sdiin_timecode_rp188_high, reg_index);
				}
			}

			/* add frame to ready list */
			list_add_tail(&data_ready->list, &stream->data_ready_list);
			NTV2_MSG_CHANNEL_STREAM("%s: video capture data queue %d  buffer %d\n",
									ntv2_chn->name,
									data_ready->index,
									data_ready->video.frame_number);
		}
	}
			
	/* frame store fills frame next */
	ntv2_reg_write(ntv2_chn->vid_reg,
				   ntv2_kona_reg_frame_input, chn_index,
				   stream->video.frame_next->video.frame_number);

	/* cache last enable state */
	stream->queue_last = stream->queue_run;

	/* print statistics */
	if (stream->queue_run && (stream->video.stat_frame_count != 0)) {
		time_us = stat_time - stream->video.last_display_time;
		if (time_us > NTV2_CHANNEL_STATISTIC_INTERVAL)
		{
			NTV2_MSG_CHANNEL_STATISTICS("%s: video frames %4d  drops %4d  time %6d (us)   total frames %lld  drops %lld\n",
										ntv2_chn->name,
										(u32)(stream->video.stat_frame_count),
										(u32)(stream->video.stat_drop_count),
										(u32)(time_us / stream->video.stat_frame_count),
										stream->video.total_frame_count,
										stream->video.total_drop_count);
			
			stream->video.stat_frame_count = 0;
			stream->video.stat_drop_count = 0;
			stream->video.last_display_time = stat_time;
		}
	}

	return 0;
}

int ntv2_videoops_acquire_hardware(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_features *features = ntv2_chn->features;
	struct ntv2_register *vid_reg = ntv2_chn->vid_reg;
	struct ntv2_input_format *input_format = &stream->video.input_format;
	struct ntv2_pixel_format *pixel_format = &stream->video.pixel_format;
	struct ntv2_widget_config *csc_config = NULL;
	struct ntv2_widget_config *lut_config = NULL;
	int index = ntv2_chn->index;
	int num_channels = 1;
	int num_cscs = 1;
	int lut_bank = 0;
	int result = 0;
	bool in_rgb = false;
	bool fs_rgb = false;
	bool do_csc = false;
	int i = 0;

	/* acquire input(s) */
	if (input_format->type == ntv2_input_type_sdi) {
		result = ntv2_features_acquire_components(features,
												  ntv2_component_sdi,
												  input_format->input_index,
												  input_format->num_inputs,
												  (unsigned long)stream);
	} else 	if ((input_format->type == ntv2_input_type_hdmi_adv) ||
				(input_format->type == ntv2_input_type_hdmi_aja) ||
				(input_format->type == ntv2_input_type_hdmi4k_aja)) {
		result = ntv2_features_acquire_components(features,
												  ntv2_component_hdmi,
												  input_format->input_index,
												  input_format->num_inputs,
												  (unsigned long)stream);
	}
	if (result != 0)
		goto release;

	/* acquire frame store(s) */
	/* look for smpte 372 high rate on 2 wires */
	if ((input_format->num_inputs == 2) &&
		((input_format->frame_flags & ntv2_kona_frame_3g) != 0) &&
		((input_format->frame_flags & ntv2_kona_frame_line_interleave) != 0)) {
		num_channels = ntv2_features_req_line_interleave_channels(features);
		num_cscs = num_channels;
	}
	/* look for two sample interleave */
	else if ((input_format->frame_flags & ntv2_kona_frame_sample_interleave) != 0) {
		num_channels = ntv2_features_req_sample_interleave_channels(features);
		num_cscs = num_channels * 2;
	}
	/* look for square division */
	else if ((input_format->frame_flags & ntv2_kona_frame_square_division) != 0) {
		num_channels = ntv2_features_req_square_division_channels(features);
		num_cscs = num_channels;
	}
	if (num_channels < 1) {
		result = -EPERM;
		goto release;
	}

	result = ntv2_features_acquire_components(features,
											  ntv2_component_video, 
											  index,
											  num_channels,
											  (unsigned long)stream);
	if (result != 0)
		goto release;

	stream->channel_index = index;
	stream->num_channels = num_channels;

	/* acquire csc(s) if needed */
	in_rgb = (input_format->pixel_flags & ntv2_kona_pixel_rgb) != 0;
	fs_rgb = (pixel_format->pixel_flags & ntv2_kona_pixel_rgb) != 0;
	do_csc = (in_rgb && !fs_rgb) || (!in_rgb && fs_rgb);

	if (do_csc) {
		csc_config = ntv2_features_find_csc_config(features, index, num_cscs);
		if (csc_config == NULL) {
			result = -EPERM;
			goto release;
		}

		result = ntv2_features_acquire_components(features,
												  ntv2_component_csc, 
												  csc_config->widget_index,
												  csc_config->num_widgets,
												  (unsigned long)stream);
		if (result != 0)
			goto release;

		stream->video.csc_index = csc_config->widget_index;
		stream->video.num_cscs = csc_config->num_widgets;

		NTV2_MSG_INFO("%s: acquire  csc %d  num %d\n", ntv2_chn->name,
					  stream->video.csc_index, stream->video.num_cscs);
	}

	if (features->device_id == NTV2_DEVICE_ID_KONAHDMI2RX) {
		/* luts */
		lut_config = ntv2_features_find_lut_config(features, index, 1);
		if (lut_config == NULL) {
			result = -EPERM;
			goto release;
		}

		result = ntv2_features_acquire_components(features,
												  ntv2_component_lut,
												  lut_config->widget_index,
												  lut_config->num_widgets,
												  (unsigned long)stream);

		if (result != 0)
			goto release;

		stream->video.lut_index = lut_config->widget_index;
		stream->video.num_luts = lut_config->num_widgets;

		/* sml: should the lut setup go here or in ntv2_videoops_setup_capture ? */
		for (i = 0; i < 1024; i++) {
			stream->video.lut_red[i] = i;
			stream->video.lut_green[i] = i;
			stream->video.lut_blue[i] = i;
		}

		lut_bank = ntv2_chn->index;
		ntv2_lut_set_output_bank(vid_reg, stream->video.lut_index, lut_bank);
		ntv2_lut_set_enable(vid_reg, stream->video.lut_index, true);
		ntv2_lut_set_color_correction_host_access_bank_v2(vid_reg, ntv2_chn->index, lut_bank);
		ntv2_lut_write_10bit_tables(vid_reg, true, stream->video.lut_red, stream->video.lut_green, stream->video.lut_blue);
		ntv2_lut_set_enable(vid_reg, stream->video.lut_index, false);

		NTV2_MSG_INFO("%s: acquire  lut %d  num %d\n", ntv2_chn->name,
					  stream->video.lut_index, stream->video.num_luts);

		/* setup enhanced csc for hue and saturation conversion */
		ntv2_csc_set_method(vid_reg, stream->video.csc_index+2, ntv2_kona_color_space_method_enhanced);
		ntv2_csc_use_custom_coefficient(vid_reg, stream->video.csc_index+2, false);

		stream->video.enhanced_csc.input_pixel_format = ntv2_kona_enhanced_csc_pixel_format_rgb444;
		stream->video.enhanced_csc.output_pixel_format = ntv2_kona_enhanced_csc_pixel_format_ycbcr422;
		stream->video.enhanced_csc.chroma_filter_select = ntv2_kona_enhanced_csc_chroma_filter_select_full;
		stream->video.enhanced_csc.chroma_edge_control = ntv2_kona_enhanced_csc_chroma_edge_control_black;
		ntv2_csc_matrix_initialize(&stream->video.enhanced_csc.matrix, ntv2_kona_enhanced_csc_matrix_type_gbr_full_to_ycbcr_rec709);
	}

	return 0;

release:
	ntv2_features_release_video_components(features, (unsigned long)stream);
	return result;
}
