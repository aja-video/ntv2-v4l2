/*
 * NTV2 device features
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

#ifndef NTV2_FEATURES_H
#define NTV2_FEATURES_H

#include "ntv2_common.h"

enum ntv2_component {
	ntv2_component_unknown,
	ntv2_component_sdi,
	ntv2_component_hdmi,
	ntv2_component_csc,
	ntv2_component_video,
	ntv2_component_audio,
	ntv2_component_size,
};

struct ntv2_video_config {
	bool						capture;
	bool						playback;
};

struct ntv2_audio_config {
	bool						capture;
	bool						playback;
	u32							sample_rate;
	u32							num_channels;
	u32							sample_size;
	u32							ring_size;
	u32							ring_offset_samples;
	u32							sync_tolerance;
};

struct ntv2_serial_config {
	u32							type;
	u32							fifo_size;
};

struct ntv2_input_config {
	const char*					name;
	enum ntv2_input_type		type;
	u32							version;
	struct v4l2_dv_timings_cap	v4l2_timings_cap;
	u32							frame_flags;
	int							reg_index;
	int							input_index;
	int							num_inputs;
};

struct ntv2_source_config {
	const char*					name;
	enum ntv2_input_type		type;
	u32							version;
	u32							audio_source;
	u32							num_channels;
	int							input_index;
	int							num_inputs;
};

struct ntv2_widget_config {
	const char*					name;
	int							widget_index;
	int							num_widgets;
};

struct ntv2_features {
	int							index;
	char						name[NTV2_STRING_SIZE];
	struct list_head			list;
	struct ntv2_device			*ntv2_dev;
	spinlock_t 					state_lock;

	u32							device_id;
	const char					*device_name;
	const char					*pcm_name;

	int							num_video_channels;
	int							num_audio_channels;
	int							num_csc_channels;
	int							num_sdi_inputs;
	int							num_hdmi_inputs;
	int							num_aes_inputs;
	int							num_analog_inputs;
	int							num_reference_inputs;
	int							num_serial_ports;
	u32							frame_buffer_size;
	int							req_line_interleave_channels;
	int							req_sample_interleave_channels;
	int							req_square_division_channels;

	struct ntv2_video_config	*video_config[NTV2_MAX_CHANNELS];
	struct ntv2_input_config	*input_config[NTV2_MAX_CHANNELS][NTV2_MAX_INPUT_CONFIGS];
	struct ntv2_widget_config	*csc_config[NTV2_MAX_CHANNELS][NTV2_MAX_CSC_CONFIGS];

	struct ntv2_audio_config	*audio_config[NTV2_MAX_CHANNELS];
	struct ntv2_source_config	*source_config[NTV2_MAX_CHANNELS][NTV2_MAX_SOURCE_CONFIGS];

	struct ntv2_video_format	*video_formats[NTV2_MAX_VIDEO_FORMATS];
	struct ntv2_pixel_format	*pixel_formats[NTV2_MAX_PIXEL_FORMATS];
	struct v4l2_dv_timings		*v4l2_timings[NTV2_MAX_VIDEO_FORMATS];

	unsigned long				component_owner[ntv2_component_size][NTV2_MAX_CHANNELS];

	struct ntv2_serial_config	*serial_config[NTV2_MAX_CHANNELS];
};


struct ntv2_features *ntv2_features_open(struct ntv2_object *ntv2_obj,
										 const char *name, int index);
void ntv2_features_close(struct ntv2_features *features);

int ntv2_features_configure(struct ntv2_features *features, u32 id);

struct ntv2_video_config
*ntv2_features_get_video_config(struct ntv2_features *features,
								int channel_index);

struct ntv2_audio_config
*ntv2_features_get_audio_config(struct ntv2_features *features,
								int channel_index);

struct ntv2_input_config
*ntv2_features_get_input_config(struct ntv2_features *features,
								int channel_index,
								int input_index);
int ntv2_features_num_input_configs(struct ntv2_features *features,
									int channel_index);
struct ntv2_input_config
*ntv2_features_get_default_input_config(struct ntv2_features *features,
										int channel_index);

struct ntv2_source_config
*ntv2_features_get_source_config(struct ntv2_features *features,
								 int channel_index,
								 int source_index);
int ntv2_features_num_source_configs(struct ntv2_features *features,
									 int channel_index);
struct ntv2_source_config
*ntv2_features_get_default_source_config(struct ntv2_features *features,
										 int channel_index);

struct ntv2_pixel_format
*ntv2_features_get_pixel_format(struct ntv2_features *features,
								int channel_index,
								int format_index);
int ntv2_features_num_pixel_formats(struct ntv2_features *features,
									int channel_index);
struct ntv2_pixel_format
*ntv2_features_get_default_pixel_format(struct ntv2_features *features,
										int channel_index);

struct ntv2_video_format
*ntv2_features_get_video_format(struct ntv2_features *features,
								int channel_index,
								int format_index);
int ntv2_features_num_video_formats(struct ntv2_features *features,
									int channel_index);
struct ntv2_video_format
*ntv2_features_get_default_video_format(struct ntv2_features *features,
										int channel_index);

struct ntv2_source_config
*ntv2_features_find_source_config(struct ntv2_features *features,
								  int channel_index,
								  enum ntv2_input_type input_type,
								  int input_index);

struct ntv2_widget_config
*ntv2_features_find_csc_config(struct ntv2_features *features,
							   int channel_index, int num_cscs);

void ntv2_features_gen_input_format(struct ntv2_input_config *config,
									struct ntv2_video_format *vidf,
									struct ntv2_pixel_format *pixf,
									struct ntv2_input_format *inpf);

void ntv2_features_gen_source_format(struct ntv2_source_config *config,
									 struct ntv2_source_format *format);

u32 ntv2_features_line_pitch(struct ntv2_pixel_format *format, u32 pixels);

u32 ntv2_features_ntv2_frame_size(struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf);

u32 ntv2_features_v4l2_frame_size(struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf);

int ntv2_features_get_frame_range(struct ntv2_features *features,
								  struct ntv2_video_format *vidf,
								  struct ntv2_pixel_format *pixf,
								  int index,
								  u32 *first,
								  u32 *last,
								  u32 *size);

u32 ntv2_features_get_audio_capture_address(struct ntv2_features *features, u32 index);
u32 ntv2_features_get_audio_play_address(struct ntv2_features *features, u32 index);

int ntv2_features_acquire_components(struct ntv2_features *features, enum ntv2_component com,
									 int index, int num, unsigned long owner);
int ntv2_features_release_components(struct ntv2_features *features, enum ntv2_component com,
									 int index, int num, unsigned long owner);
void ntv2_features_release_video_components(struct ntv2_features *features, unsigned long owner);
void ntv2_features_release_audio_components(struct ntv2_features *features, unsigned long owner);

bool ntv2_features_valid_dv_timings(struct ntv2_features *features,
									const struct v4l2_dv_timings *t,
									const struct v4l2_dv_timings_cap *cap);

int ntv2_features_enum_dv_timings_cap(struct ntv2_features *features,
									  struct v4l2_enum_dv_timings *t,
									  const struct v4l2_dv_timings_cap *cap);

bool ntv2_features_find_dv_timings_cap(struct ntv2_features *features,
									   struct v4l2_dv_timings *t,
									   const struct v4l2_dv_timings_cap *cap,
									   unsigned pclock_delta);

bool ntv2_features_match_dv_timings(const struct v4l2_dv_timings *measured,
									const struct v4l2_dv_timings *standard,
									unsigned pclock_delta);

int ntv2_features_req_line_interleave_channels(struct ntv2_features *features);
int ntv2_features_req_sample_interleave_channels(struct ntv2_features *features);
int ntv2_features_req_square_division_channels(struct ntv2_features *features);

#endif
