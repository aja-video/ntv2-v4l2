/*
 * NTV2 audio stream channel ops
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

#include "ntv2_audioops.h"
#include "ntv2_channel.h"
#include "ntv2_konareg.h"
#include "ntv2_register.h"


#include "ntv2_features.h"

int ntv2_audioops_setup_capture(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_features *features = ntv2_chn->features;
	struct ntv2_audio_config *audio_config;
	int index = ntv2_chn->index;
	u32 val;
	u32 mask;
	int i;

	audio_config = ntv2_features_get_audio_config(features, ntv2_chn->index);

	/* initialize audio input stream data */
	INIT_LIST_HEAD(&stream->data_ready_list);
	INIT_LIST_HEAD(&stream->data_done_list);
	stream->queue_run = false;
	stream->queue_last = false;
	stream->audio.sample_rate = audio_config->sample_rate;
	stream->audio.num_channels = audio_config->num_channels;
	stream->audio.sample_size = audio_config->sample_size;
	stream->audio.audio_offset = 0;
	stream->audio.ring_address = ntv2_features_get_audio_capture_address(features, ntv2_chn->index);
	stream->audio.ring_offset = audio_config->ring_size;
	stream->audio.ring_size = audio_config->ring_size;
	stream->audio.ring_init =
		audio_config->ring_offset_samples *
		audio_config->num_channels *
		audio_config->sample_size;
	stream->audio.sync_cadence = 0;
	stream->audio.sync_tolerance = audio_config->sync_tolerance;
	stream->audio.total_sample_count = 0;
	stream->audio.total_drop_count = 0;
	stream->audio.stat_sample_count = 0;
	stream->audio.stat_drop_count = 0;
	stream->audio.hardware_enable = false;

	/* initialize audio data buffers */
	for (i = 0; i < NTV2_MAX_CHANNEL_BUFFERS; i++) {
		stream->data_array[i].index = i;
		stream->data_array[i].type = stream->type;
		INIT_LIST_HEAD(&stream->data_array[i].list);
		stream->data_array[i].ntv2_str = stream;
		list_add_tail(&stream->data_array[i].list, &stream->data_done_list);
	}

	/* setup audio input (disable capture) */
	val = NTV2_FLD_SET(ntv2_kona_fld_audio_capture_enable, 0);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_capture_enable);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_reset, 1);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_reset);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_16_channel, 1);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_16_channel);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_big_buffer, 0);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_big_buffer);
	ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_control, index, val, mask);

	/* setup audio source */
	val = NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch12, ntv2_kona_audio_source_embedded);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch34, ntv2_kona_audio_source_embedded);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch56, ntv2_kona_audio_source_embedded);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch78, ntv2_kona_audio_source_embedded);
	ntv2_reg_write(ntv2_chn->vid_reg, ntv2_kona_reg_audio_source, index, val);

	stream->audio.embedded_clock = false;

	return 0;
}

int ntv2_audioops_update_mode(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	int index = ntv2_chn->index;
	u32 val;
	u32 mask;

	/* enable audio capture */
	if (stream->queue_enable) {
		val = NTV2_FLD_SET(ntv2_kona_fld_audio_capture_enable, 1);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_capture_enable);
		val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_reset, 0);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_reset);
		ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_control, index, val, mask);
		stream->audio.hardware_enable = true;
	} else {
		val = NTV2_FLD_SET(ntv2_kona_fld_audio_capture_enable, 0);
		mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_capture_enable);
		val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_reset, 1);
		mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_reset);
		ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_control, index, val, mask);
		stream->audio.hardware_enable = false;
	}

	return 0;
}

int ntv2_audioops_update_route(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_channel_stream *video_stream = NULL;
	struct ntv2_source_config *source_config = NULL;
	struct ntv2_source_format source_format;
	struct ntv2_features *features = ntv2_chn->features;
	int index = ntv2_chn->index;
	u32 org_source = ntv2_kona_audio_source_aes;
	u32 org_bit0 = 0;
	u32 org_bit1 = 0;
	u32 new_source = ntv2_kona_audio_source_aes;
	u32 new_bit0 = 0;
	u32 new_bit1 = 0;
	u32 val;
	u32 mask;

	/* get original audio source */
	val = ntv2_reg_read(ntv2_chn->vid_reg, ntv2_kona_reg_audio_source, index);
	org_source = NTV2_FLD_GET(ntv2_kona_fld_audio_input_ch12, val);
	org_bit0 = NTV2_FLD_GET(ntv2_kona_fld_audio_embedded_input_b0, val);
	org_bit1 = NTV2_FLD_GET(ntv2_kona_fld_audio_embedded_input_b1, val);

	/* handle auto source */
	if (stream->audio.source_format.type == ntv2_input_type_auto) {
		/* if video source use for audio */
		video_stream = ntv2_chn->streams[ntv2_stream_type_vidin];
		if (video_stream != NULL) {
			source_config = ntv2_features_find_source_config(
				features,
				index,
				video_stream->video.input_format.type,
				video_stream->video.input_format.input_index);
			if (source_config != NULL) {
				ntv2_features_gen_source_format(source_config, &source_format);
				NTV2_MSG_CHANNEL_STATE("%s: audio auto route to video %s\n",
									   ntv2_chn->name, source_config->name);
			} else {
				/* if no audio source use default */
				source_format = stream->audio.auto_format;
				NTV2_MSG_CHANNEL_STATE("%s: audio auto route default\n",
									   ntv2_chn->name);
			}
		} else {
			/* if no video source use default */
			source_format = stream->audio.auto_format;
			NTV2_MSG_CHANNEL_STATE("%s: audio auto route default\n",
								   ntv2_chn->name);
		}
	} else {
		source_format = stream->audio.source_format;
	}

	new_source = source_format.audio_source;
	new_bit0 = source_format.input_index & 0x1;
	new_bit1 = (source_format.input_index & 0x2) >> 1;

	/* set audio source */
	val = NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch12, new_source);
	mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_input_ch12);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch34, new_source);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_ch34);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch56, new_source);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_ch56);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_input_ch78, new_source);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_input_ch78);

	/* set sdi embedded / hdmi channel */
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_embedded_input_b0, new_bit0);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_embedded_input_b0);
	val |= NTV2_FLD_SET(ntv2_kona_fld_audio_embedded_input_b1, new_bit1);
	mask |= NTV2_FLD_MASK(ntv2_kona_fld_audio_embedded_input_b1);

	/* update source */
	ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_source, index, val, mask);

	/* if audio source changes then resync with video */
	if ((new_source != org_source) ||
		(new_bit0 != org_bit0) ||
		(new_bit1 != org_bit1)) {
		stream->audio.ring_offset = 0x40000000;
	}

	return 0;
}

int ntv2_audioops_interrupt_capture(struct ntv2_channel_stream *stream)
{
	struct ntv2_channel *ntv2_chn = stream->ntv2_chn;
	struct ntv2_channel_stream *video_stream = ntv2_chn->streams[ntv2_stream_type_vidin];
	struct ntv2_stream_data *data_ready;
	int index = ntv2_chn->index;
	s64 stat_time = ntv2_chn->dpc_status.stat_time;
	s64 time_us;
	u32 prev_audio_offset;
	u32 audio_stride;
	u32 audio_offset;
	u32 ring_offset;
	u32 audio_delta_ring;
	u32 audio_delta_offset;
	u32 audio_delta = 0;
	u32 audio_samples;
	u32 audio_size;
	u32 ring_size = stream->audio.ring_size;
	u32 val;
	u32 mask;
	bool input_clock = false;
	bool embedded_clock = false;

	/* idle if not enabled */
	if (!stream->queue_enable)
		return 0;

	/* if video input is enabled, run on the video input interrupt */
	if ((video_stream != NULL) && video_stream->queue_enable)
		input_clock = true;

	/* for sdi and hdmi inputs use the generated embedded audio clock */
	if ((video_stream->video.input_format.type == ntv2_input_type_sdi) ||
		(video_stream->video.input_format.type == ntv2_input_type_hdmi_adv) ||
		(video_stream->video.input_format.type == ntv2_input_type_hdmi_aja) ||
		(video_stream->video.input_format.type == ntv2_input_type_hdmi4k_aja))
		embedded_clock = true;

	/* if there are no reference inputs use the input interrupt */
	if (stream->ntv2_chn->features->num_reference_inputs == 0)
		input_clock = true;
			
	/* only run on the correct interrupt */
	if (input_clock) {
		if(!ntv2_chn->dpc_status.interrupt_input)
			return 0;
	} else {
		if(!ntv2_chn->dpc_status.interrupt_output)
			return 0;
	}

	/* set the embedded audio clock source for sdi sources */
	if (embedded_clock) {
		if (!stream->audio.embedded_clock) {
			val = NTV2_FLD_SET(ntv2_kona_fld_audio_embedded_clock, 1);
			mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_embedded_clock);
			ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_source, index, val, mask);
			stream->audio.embedded_clock = true;
		}
	} else {
		if (stream->audio.embedded_clock) {
			val = NTV2_FLD_SET(ntv2_kona_fld_audio_embedded_clock, 0);
			mask = NTV2_FLD_MASK(ntv2_kona_fld_audio_embedded_clock);
			ntv2_reg_rmw(ntv2_chn->vid_reg, ntv2_kona_reg_audio_source, index, val, mask);
			stream->audio.embedded_clock = false;
		}
	}

	/* get dynamic stream time and audio position */
	stream->timestamp = ntv2_chn->dpc_status.interrupt_time;
	audio_offset = ntv2_chn->dpc_status.audio_input_offset;

	/* align the current hardware audio offset */
	audio_stride = stream->audio.num_channels * stream->audio.sample_size;
	audio_offset = audio_offset / audio_stride * audio_stride;

	/* offset the current hardware audio offset for frame buffer latency */
	audio_offset = (audio_offset + ring_size - stream->audio.ring_init)%ring_size;

	/* compute expected sample bytes from last interrupt */
	audio_samples = ntv2_audio_frame_samples(ntv2_chn->dpc_status.interrupt_rate, stream->audio.sync_cadence++);
	audio_size = audio_samples * audio_stride;

	if (stream->queue_last) {
		prev_audio_offset = stream->audio.audio_offset;
		/* update computed ring offset */
		ring_offset = (stream->audio.ring_offset + audio_size)%ring_size;
		/* check audio sync with hardware */
		audio_delta_ring = (ring_offset + ring_size - audio_offset)%ring_size;
		audio_delta_offset = (audio_offset + ring_size - ring_offset)%ring_size;
		audio_delta = min(audio_delta_ring, audio_delta_offset);
		audio_delta = (audio_delta / audio_stride) * 10000 / stream->audio.sample_rate;
		if (audio_delta > (stream->audio.sync_tolerance/100)) {
			NTV2_MSG_CHANNEL_STATE("%s: %s correcting audio sync  exp %08x  act %08x  error %d us\n",
								   ntv2_chn->name,
								   ntv2_stream_name(ntv2_stream_type_audin),
								   ring_offset,
								   audio_offset,
								   audio_delta * 100);
			/* correct number of samples transfered */
			audio_size += (s32)(stream->audio.total_sample_count - stream->audio.total_transfer_count) * (s32)audio_stride;
			/* limit in case of unexpected results */
			if (audio_size > ring_size/4)
				audio_size = audio_samples * audio_stride;
			/* move the ring position to the hardware audio offset */
			prev_audio_offset = (audio_offset + ring_size - audio_size)%ring_size;
			ring_offset = audio_offset;
		}
		/* save for stats */
		stream->audio.total_sample_count += audio_samples;
		stream->audio.stat_sample_count += audio_samples;
	} else {
		/* set offset on start */
		prev_audio_offset = audio_offset;
		ring_offset = audio_offset;
		/* initialize stats */
		stream->audio.total_sample_count = 0;
		stream->audio.total_transfer_count = 0;
		stream->audio.total_drop_count = 0;
		stream->audio.stat_sample_count = 0;
		stream->audio.stat_drop_count = 0;
		stream->audio.last_display_time = stat_time;
	}

	/* save current audio offset */
	stream->audio.audio_offset = audio_offset;
	stream->audio.ring_offset = ring_offset;

#if 0
	NTV2_MSG_CHANNEL_STATE("%s: %s  offset %d  ring %d  samples %d  delta %d\n",
						   ntv2_chn->name,
						   ntv2_stream_name(stream->type),
						   audio_offset,
						   ring_offset,
						   audio_samples,
						   audio_delta);
#endif								   
								   
	/* add frame to queue */
	if (stream->queue_run && (stream->audio.total_sample_count != 0)) {
		audio_size = (audio_offset + ring_size - prev_audio_offset)%ring_size;
		audio_samples = audio_size / audio_stride;

		if (!list_empty(&stream->data_done_list)) {
			/* get next data object */
			data_ready = list_first_entry(&stream->data_done_list,
										  struct ntv2_stream_data, list);
			list_del_init(&data_ready->list);
			/* add audio data to queue */
			data_ready->audio.offset = prev_audio_offset;
			data_ready->audio.address[0] = stream->audio.ring_address + prev_audio_offset;
			data_ready->audio.address[1] = stream->audio.ring_address;
			if ((prev_audio_offset + audio_size) > ring_size) {
				data_ready->audio.data_size[0] = ring_size - prev_audio_offset;
				data_ready->audio.data_size[1] = audio_size - data_ready->audio.data_size[0];
			} else {
				data_ready->audio.data_size[0] = audio_size;
				data_ready->audio.data_size[1] = 0;
			}
			data_ready->audio.num_channels = stream->audio.num_channels;
			data_ready->audio.sample_size = stream->audio.sample_size;

			list_add_tail(&data_ready->list, &stream->data_ready_list);
			NTV2_MSG_CHANNEL_STREAM("%s: audio capture data queue %d  size %d\n",
									ntv2_chn->name,
									data_ready->index,
									audio_size);

			stream->audio.total_transfer_count += audio_samples;
		} else {
			stream->video.total_drop_count += audio_samples;
			stream->video.stat_drop_count += audio_samples;
		}
	}

	/* cache last enable state */
	stream->queue_last = stream->queue_run;

	/* print statistics */
	if (stream->audio.stat_sample_count != 0) {
		time_us = stat_time - stream->audio.last_display_time;
		if (time_us > NTV2_CHANNEL_STATISTIC_INTERVAL)
		{
			NTV2_MSG_CHANNEL_STATISTICS("%s: audio samples %4d  drops %4d  time %6d (us)   total samples %lld  transfers %lld  drops %lld\n",
										ntv2_chn->name,
										(u32)(stream->audio.stat_sample_count),
										(u32)(stream->audio.stat_drop_count),
										(u32)(time_us / stream->audio.stat_sample_count),
										stream->audio.total_sample_count,
										stream->audio.total_transfer_count,
										stream->audio.total_drop_count);
				
			stream->audio.stat_sample_count = 0;
			stream->audio.stat_drop_count = 0;
			stream->audio.last_display_time = stat_time;
		}
	}

	return 0;
}
