/*
 * NTV2 alsa device interface
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

#ifndef NTV2_AUDIO_H
#define NTV2_AUDIO_H

#include "ntv2_common.h"

struct ntv2_audio;
struct ntv2_features;
struct ntv2_source_config;

struct ntv2_pcm_stream {
	enum ntv2_stream_type		type;
	struct ntv2_audio			*ntv2_aud;
	spinlock_t 					state_lock;
	struct tasklet_struct		transfer_task;
	enum ntv2_task_state		transfer_state;
	enum ntv2_task_state		task_state;

	struct ntv2_channel_stream	*chn_str;
	struct snd_pcm_substream	*substream;
	u32							sample_ptr;
	u32							period_ptr;
	u32							sample_cycle;
	bool						trigger;

	struct ntv2_stream_data		*dma_audbuf;
	bool						dma_start;
	bool						dma_done;
	int							dma_result;
	u32							dma_size;

	u8							*dma_buffer;
	u32							dma_buffer_size;
	struct sg_table 			dma_sgtable;
	u32							dma_buffer_pages;
};

struct ntv2_audio {
	int							index;
	char						name[NTV2_STRING_SIZE];
	struct list_head			list;
	struct ntv2_device			*ntv2_dev;

	struct ntv2_features 		*features;
	struct snd_card 			*snd_card;
	struct ntv2_channel 		*ntv2_chn;
	struct ntv2_input			*ntv2_inp;
	struct ntv2_nwldma			*dma_engine;
	bool						init;
	int							source_index;

	struct snd_pcm 				*pcm;
	struct ntv2_pcm_stream		*capture;
	struct ntv2_pcm_stream		*playback;
};

struct ntv2_audio *ntv2_audio_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index);
void ntv2_audio_close(struct ntv2_audio *ntv2_aud);

int ntv2_audio_configure(struct ntv2_audio *ntv2_aud,
						 struct ntv2_features *features,
						 struct snd_card *snd_card,
						 struct ntv2_channel *ntv2_chn,
						 struct ntv2_input *ntv2_inp,
						 struct ntv2_nwldma *ntv2_nwl);

int ntv2_audio_set_source(struct ntv2_audio *ntv2_aud,
						  struct ntv2_source_config *config);

struct ntv2_pcm_stream *ntv2_audio_capture_stream(struct ntv2_audio *ntv2_aud);
struct ntv2_pcm_stream *ntv2_audio_playback_stream(struct ntv2_audio *ntv2_aud);

int ntv2_audio_enable(struct ntv2_pcm_stream *stream);
int ntv2_audio_disable(struct ntv2_pcm_stream *stream);

int ntv2_audio_start(struct ntv2_pcm_stream *stream);
int ntv2_audio_stop(struct ntv2_pcm_stream *stream);
int ntv2_audio_flush(struct ntv2_pcm_stream *stream);

#endif
