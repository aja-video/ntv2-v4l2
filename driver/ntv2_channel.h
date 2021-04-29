/*
 * NTV2 hardware channel interface
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

#ifndef NTV2_CHANNEL_H
#define NTV2_CHANNEL_H

#include "ntv2_common.h"

#define NTV2_MAX_CHANNEL_STREAMS		8
#define NTV2_MAX_CHANNEL_BUFFERS		64
#define NTV2_CHANNEL_STATISTIC_INTERVAL	5000000

enum ntv2_channel_state {
	ntv2_channel_state_unknown,
	ntv2_channel_state_idle,
	ntv2_channel_state_run,
	ntv2_channel_state_size
};

struct ntv2_features;

typedef void (*ntv2_channel_callback)(unsigned long);

struct ntv2_channel_status {
	bool							interrupt_input;
	bool							interrupt_output;
	v4l2_time_t						interrupt_time;
	u32								interrupt_rate;
	u32								audio_input_offset;
	u32								audio_output_offset;
	s64								stat_time;
};

struct ntv2_video_data {
	u32								frame_number;
	u32								address;
	u32								data_size;
	u32								timecode_low;
	u32								timecode_high;
	bool							timecode_present;
};

struct ntv2_audio_data {
	u32								offset;
	u32								address[2];
	u32								data_size[2];
	u32								num_channels;
	u32								sample_size;
};

struct ntv2_stream_data {
	int								index;
	struct list_head				list;
	struct ntv2_channel_stream		*ntv2_str;

	enum ntv2_stream_type			type;
	v4l2_time_t						timestamp;

	union {
		struct ntv2_video_data		video;
		struct ntv2_audio_data		audio;
	};
};

struct ntv2_video_stream {
	struct ntv2_video_format		video_format;
	struct ntv2_pixel_format		pixel_format;
	struct ntv2_input_format		input_format;

	u32								frame_first;
	u32								frame_last;
	u32								frame_size;
	bool							hardware_enable[NTV2_MAX_CHANNELS];

	int								csc_index;
	int								num_cscs;
	int								lut_index;
	int								num_luts;
	int								brightness;
	int								gamma;

	struct ntv2_stream_data			*frame_active;
	struct ntv2_stream_data			*frame_next;

	s64								total_frame_count;
	s64								total_drop_count;
	s64								stat_frame_count;
	s64								stat_drop_count;
    s64								last_display_time;

	u16								lut_red[1024];
	u16								lut_green[1024];
	u16								lut_blue[1024];
};

struct ntv2_audio_stream {
	struct ntv2_source_format		source_format;
	struct ntv2_source_format		auto_format;

	u32								sample_rate;
	u32								num_channels;
	u32								sample_size;
	u32								audio_offset;
	u32								ring_address;
	u32								ring_offset;
	u32								ring_size;
	u32								ring_init;
	u32								sync_cadence;
	u32								sync_tolerance;
	bool							hardware_enable;
	bool							embedded_clock;

	s64								total_sample_count;
	s64								total_transfer_count;
	s64								total_drop_count;
	s64								stat_sample_count;
	s64								stat_drop_count;
    s64								last_display_time;
};

struct ntv2_stream_ops {
	int (*setup)(struct ntv2_channel_stream *stream);
	int (*release)(struct ntv2_channel_stream *stream);
	int (*update_mode)(struct ntv2_channel_stream *stream);
	int (*update_timing)(struct ntv2_channel_stream *stream);
	int (*update_format)(struct ntv2_channel_stream *stream);
	int (*update_route)(struct ntv2_channel_stream *stream);
	int (*interrupt)(struct ntv2_channel_stream *stream);
};

struct ntv2_channel_stream {
	enum ntv2_stream_type			type;
	struct ntv2_channel 			*ntv2_chn;

	int								channel_index;
	int								num_channels;
	
	bool							capture;
	bool							queue_enable;
	bool							queue_run;
	bool							queue_last;
	ntv2_channel_callback			frame_callback_func;
	unsigned long					frame_callback_data;
	v4l2_time_t						timestamp;

	struct ntv2_stream_data			data_array[NTV2_MAX_CHANNEL_BUFFERS];
	struct list_head 				data_ready_list;
	struct list_head 				data_done_list;

	struct ntv2_stream_ops			ops;
	union {
		struct ntv2_video_stream 	video;
		struct ntv2_audio_stream 	audio;
	};
};

struct ntv2_channel {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;
	int								ref_count;

	struct ntv2_features 			*features;
	struct ntv2_register			*vid_reg;
	
	enum ntv2_channel_state			state;
	spinlock_t 						state_lock;

	struct tasklet_struct			int_dpc;
	spinlock_t 						int_lock;
	struct ntv2_channel_status		int_status;
	struct ntv2_channel_status		dpc_status;

	struct ntv2_channel_stream		*streams[ntv2_stream_type_size];
};

struct ntv2_channel *ntv2_channel_open(struct ntv2_object *ntv2_obj,
									   const char *name, int index);
void ntv2_channel_close(struct ntv2_channel *ntv2_chn);

int ntv2_channel_configure(struct ntv2_channel *ntv2_chn,
						   struct ntv2_features *features,
						   struct ntv2_register *vid_reg);

void ntv2_channel_disable_all(struct ntv2_channel *ntv2_chn);

struct ntv2_channel_stream *ntv2_channel_stream(struct ntv2_channel *ntv2_chn,
												enum ntv2_stream_type stype);

int ntv2_channel_set_video_format(struct ntv2_channel_stream *stream,
								  struct ntv2_video_format *vidf);

int ntv2_channel_get_video_format(struct ntv2_channel_stream *stream,
								  struct ntv2_video_format *vidf);

int ntv2_channel_set_pixel_format(struct ntv2_channel_stream *stream,
								  struct ntv2_pixel_format *pixf);

int ntv2_channel_get_pixel_format(struct ntv2_channel_stream *stream,
								  struct ntv2_pixel_format *pixf);

int ntv2_channel_set_input_format(struct ntv2_channel_stream *stream,
								  struct ntv2_input_format *inpf);

int ntv2_channel_get_input_format(struct ntv2_channel_stream *stream,
								  struct ntv2_input_format *inpf);

int ntv2_channel_set_source_format(struct ntv2_channel_stream *stream,
								   struct ntv2_source_format *souf);

int ntv2_channel_get_source_format(struct ntv2_channel_stream *stream,
								   struct ntv2_source_format *souf);

int ntv2_channel_set_frame_callback(struct ntv2_channel_stream *stream,
									ntv2_channel_callback func,
									unsigned long data);

int ntv2_channel_enable(struct ntv2_channel_stream *stream);
int ntv2_channel_disable(struct ntv2_channel_stream *stream);

int ntv2_channel_start(struct ntv2_channel_stream *stream);
int ntv2_channel_stop(struct ntv2_channel_stream *stream);
int ntv2_channel_flush(struct ntv2_channel_stream *stream);

struct ntv2_stream_data *ntv2_channel_data_ready(struct ntv2_channel_stream *stream);
void ntv2_channel_data_done(struct ntv2_stream_data *ntv2_data);

int ntv2_channel_interrupt(struct ntv2_channel *ntv2_chn,
						   struct ntv2_interrupt_status* irq_status);
#endif
