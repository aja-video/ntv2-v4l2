/*
 * NTV2 alsa pcm ops
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

#include "ntv2_audio.h"
#include "ntv2_pcmops.h"
#include "ntv2_features.h"
#include "ntv2_channel.h"


static int ntv2_allocate_dma_buffer(struct ntv2_pcm_stream *stream);
static void ntv2_free_dma_buffer(struct ntv2_pcm_stream *stream);
static void ntv2_copy_audio(void *dst_buffer, void *src_buffer,
							u32 dst_channels, u32 src_channels,
							u32 dst_sample_size, u32 src_sample_size,
							u32 samples);


static struct snd_pcm_hardware ntv2_pcm_hardware = {
	.info = (SNDRV_PCM_INFO_MMAP |
			 SNDRV_PCM_INFO_INTERLEAVED |
			 SNDRV_PCM_INFO_BLOCK_TRANSFER |
			 SNDRV_PCM_INFO_MMAP_VALID),
	.formats =          SNDRV_PCM_FMTBIT_S32_LE | SNDRV_PCM_FMTBIT_S16_LE,
	.rates =            SNDRV_PCM_RATE_48000,
	.rate_min =         48000,
	.rate_max =         48000,
	.channels_min =     1,
	.channels_max =     16,
	.buffer_bytes_max = 48000*16*4,
	.period_bytes_min = 800*2,
	.period_bytes_max = 4800*16*4,
	.periods_min =      10,
	.periods_max =      1920,
};

static int ntv2_pcmops_cap_open(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;
	struct snd_pcm_runtime *runtime = substream->runtime;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture open\n", ntv2_aud->name);

	runtime->hw = ntv2_pcm_hardware;
	stream->substream = substream;

	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);

	return 0;
}

static int ntv2_pcmops_cap_close(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture close\n", ntv2_aud->name);

	stream->substream = NULL;

	return 0;
}

static int ntv2_pcmops_cap_hw_params(struct snd_pcm_substream *substream,
									 struct snd_pcm_hw_params *hw_params)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct ntv2_source_config *config;
	int size;
	int ret;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture hardware params\n", ntv2_aud->name);

	/* allocate the pcm ring buffer */
	size = params_buffer_bytes(hw_params);
	if (runtime->dma_area != NULL) {
		if (runtime->dma_bytes > size)
			return 0;
		vfree(runtime->dma_area);
	}
	runtime->dma_area = vmalloc(size);
	if (runtime->dma_area == NULL)
		return -ENOMEM;
	runtime->dma_bytes = size;

	NTV2_MSG_AUDIO_STATE("%s: allocate pcm capture ring buffer size %d\n",
						 ntv2_aud->name, size);

	/* allocate the dma intermediate buffer */
	ret = ntv2_allocate_dma_buffer(stream);
	if (ret != 0)
		return ret;

	/* configure the audio source */
	config = ntv2_features_get_source_config(ntv2_aud->features,
											 ntv2_aud->ntv2_chn->index, 
											 ntv2_aud->source_index);
	ntv2_audio_set_source(ntv2_aud, config);

	return 0;
}

static int ntv2_pcmops_cap_hw_free(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;
	struct snd_pcm_runtime *runtime = substream->runtime;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture hardware free\n", ntv2_aud->name);

	ntv2_audio_disable(stream);

	ntv2_free_dma_buffer(stream);

	if (runtime->dma_area) {
		vfree(runtime->dma_area);
		runtime->dma_area = NULL;
	}

	return 0;
}

static int ntv2_pcmops_cap_prepare(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;
	struct snd_pcm_runtime *runtime = substream->runtime;
	u32 sample_size;
	u32 num_channels;
	int ret;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture prepare\n", ntv2_aud->name);

	if (runtime->rate != 48000) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* capture bad runtime sample rate %d\n",
							 ntv2_aud->name, runtime->rate);
		return -EINVAL;
	}

	sample_size = runtime->sample_bits / 8;
	if ((sample_size != 2) && (sample_size != 4)) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* capture bad runtime sample size %d\n",
							 ntv2_aud->name, sample_size);
		return -EINVAL;
	}
	
	num_channels = runtime->frame_bits / 8 / sample_size;
	if ((num_channels < 1) || (num_channels > 16)) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* capture bad runtime number of channels %d\n",
							 ntv2_aud->name, num_channels);
		return -EINVAL;
	}

	/* enable streaming */
	ret = ntv2_audio_enable(stream);
	if (ret != 0) {
		return ret;
	}

	NTV2_MSG_AUDIO_STATE("%s: capture buffer  sample size %d  channels %d  rate %d\n",
						 ntv2_aud->name,
						 sample_size,
						 num_channels,
						 runtime->rate);
	NTV2_MSG_AUDIO_STATE("%s: capture buffer  period frames %d  periods %d  buffer frames %d\n",
						 ntv2_aud->name,
						 (int)runtime->period_size,
						 (int)runtime->periods,
						 (int)runtime->buffer_size);

	stream->sample_ptr = 0;
	stream->period_ptr = 0;
	stream->sample_cycle = 0;

	return 0;
}

static int ntv2_pcmops_cap_trigger(struct snd_pcm_substream *substream,
								  int cmd)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->capture;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		NTV2_MSG_AUDIO_STATE("%s: pcm capture trigger start\n", ntv2_aud->name);
		ntv2_audio_start(stream);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		NTV2_MSG_AUDIO_STATE("%s: pcm capture trigger stop\n", ntv2_aud->name);
		ntv2_audio_stop(stream);
		ntv2_audio_flush(stream);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_uframes_t ntv2_pcmops_cap_pointer(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	snd_pcm_uframes_t current_ptr = ntv2_aud->capture->sample_ptr;

	NTV2_MSG_AUDIO_STREAM("%s: pcm capture pointer %d\n",
						  ntv2_aud->name, (int)current_ptr);

	return current_ptr;
}

static struct page *ntv2_pcmops_cap_page(struct snd_pcm_substream *substream,
										 unsigned long offset)
{
	void *page_ptr = substream->runtime->dma_area + offset;
	return vmalloc_to_page(page_ptr);
}

static struct snd_pcm_ops ntv2_pcmops_capture_ops = {
	.open =			ntv2_pcmops_cap_open,
	.close =		ntv2_pcmops_cap_close,
	.ioctl =		snd_pcm_lib_ioctl,
	.hw_params =	ntv2_pcmops_cap_hw_params,
	.hw_free =		ntv2_pcmops_cap_hw_free,
	.prepare =		ntv2_pcmops_cap_prepare,
	.trigger =		ntv2_pcmops_cap_trigger,
	.pointer =		ntv2_pcmops_cap_pointer,
	.page =			ntv2_pcmops_cap_page,
};

static int ntv2_pcmops_play_open(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->playback;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int result;

	NTV2_MSG_AUDIO_STATE("%s: pcm playback open\n", ntv2_aud->name);

	runtime->hw = ntv2_pcm_hardware;
	stream->substream = substream;

	snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);

	result = ntv2_audio_enable(stream);
	if (result != 0) {
		return result;
	}

	return 0;
}

static int ntv2_pcmops_play_close(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->playback;

	NTV2_MSG_AUDIO_STATE("%s: pcm playback close\n", ntv2_aud->name);

	ntv2_audio_disable(stream);

	stream->substream = NULL;

	return 0;
}

static int ntv2_pcmops_play_hw_params(struct snd_pcm_substream *substream,
									  struct snd_pcm_hw_params *hw_params)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->playback;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int size;
	int ret;

	NTV2_MSG_AUDIO_STATE("%s: pcm play hardware params\n", ntv2_aud->name);

	/* allocate the pcm ring buffer */
	size = params_buffer_bytes(hw_params);
	if (runtime->dma_area != NULL) {
		if (runtime->dma_bytes > size)
			return 0;
		vfree(runtime->dma_area);
	}
	runtime->dma_area = vmalloc(size);
	if (runtime->dma_area == NULL)
		return -ENOMEM;
	runtime->dma_bytes = size;

	NTV2_MSG_AUDIO_STATE("%s: allocate pcm play ring buffer size %d\n",
						 ntv2_aud->name, size);

	/* allocate the dma intermediate buffer */
	ret = ntv2_allocate_dma_buffer(stream);
	if (ret != 0)
		return ret;

	return 0;
}

static int ntv2_pcmops_play_hw_free(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->playback;
	struct snd_pcm_runtime *runtime = substream->runtime;

	NTV2_MSG_AUDIO_STATE("%s: pcm capture hardware free\n", ntv2_aud->name);

	ntv2_free_dma_buffer(stream);

	if (runtime->dma_area) {
		vfree(runtime->dma_area);
		runtime->dma_area = NULL;
	}

	return 0;
}

static int ntv2_pcmops_play_prepare(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);

	NTV2_MSG_AUDIO_STATE("%s: pcm playback prepare\n", ntv2_aud->name);

	return 0;
}

static int ntv2_pcmops_play_trigger(struct snd_pcm_substream *substream,
									int cmd)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	struct ntv2_pcm_stream *stream = ntv2_aud->playback;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		NTV2_MSG_AUDIO_STATE("%s: pcm playback trigger start\n", ntv2_aud->name);
		ntv2_audio_start(stream);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		NTV2_MSG_AUDIO_STATE("%s: pcm playback trigger stop\n", ntv2_aud->name);
		ntv2_audio_stop(stream);
		ntv2_audio_flush(stream);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_uframes_t ntv2_pcmops_play_pointer(struct snd_pcm_substream *substream)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_pcm_substream_chip(substream);
	snd_pcm_uframes_t current_ptr = ntv2_aud->playback->sample_ptr;

	NTV2_MSG_AUDIO_STREAM("%s: pcm playback pointer %d\n",
						  ntv2_aud->name, (int)current_ptr);

	return current_ptr;
}

static struct page *ntv2_pcmops_play_page(struct snd_pcm_substream *substream,
										  unsigned long offset)
{
	void *page_ptr = substream->runtime->dma_area + offset;
	return vmalloc_to_page(page_ptr);
}

static struct snd_pcm_ops ntv2_pcmops_playback_ops = {
	.open =			ntv2_pcmops_play_open,
	.close =		ntv2_pcmops_play_close,
	.ioctl =		snd_pcm_lib_ioctl,
	.hw_params =	ntv2_pcmops_play_hw_params,
	.hw_free =		ntv2_pcmops_play_hw_free,
	.prepare =		ntv2_pcmops_play_prepare,
	.trigger =		ntv2_pcmops_play_trigger,
	.pointer =		ntv2_pcmops_play_pointer,
	.page =			ntv2_pcmops_play_page,
};

int ntv2_pcmops_configure(struct ntv2_pcm_stream *stream)
{
	if (stream == NULL)
		return -EPERM;

	/* set operators */
	if (stream->type == ntv2_stream_type_audin) {
		snd_pcm_set_ops(stream->ntv2_aud->pcm,
						SNDRV_PCM_STREAM_CAPTURE,
						&ntv2_pcmops_capture_ops);
	}
	if (stream->type == ntv2_stream_type_audout) {
		snd_pcm_set_ops(stream->ntv2_aud->pcm,
						SNDRV_PCM_STREAM_PLAYBACK,
						&ntv2_pcmops_playback_ops);
	}

	return 0;
}

void ntv2_pcmops_tstamp(struct ntv2_pcm_stream *stream)
{
	struct snd_pcm_runtime *runtime;

	if ((stream == NULL) ||
		(stream->substream == NULL))
		return;

	runtime = stream->substream->runtime;
	if (runtime == NULL)
		return;

	snd_pcm_gettime(runtime, &runtime->trigger_tstamp);
//	runtime->trigger_tstamp_latched = true;
}

void ntv2_pcmops_copy_audio(struct ntv2_pcm_stream *stream,
							u8 *address,
							u32 size,
							u32 num_channels,
							u32 sample_size)
{
	struct ntv2_audio *ntv2_aud = stream->ntv2_aud;
	struct snd_pcm_substream *substream;
	struct snd_pcm_runtime *runtime;
	u32 old_ptr;
	u32 buf_frames;
	u32 buf_stride;
	u32 buf_channels;
	u32 buf_sample_size;
	u32 ring_stride;
	u32 ring_channels;
	u32 ring_sample_size;
	bool new_period = false;

	if ((size == 0) ||
		(num_channels == 0) ||
		(sample_size == 0))
		return;

	substream = stream->substream;
	if (substream == NULL) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* pcm_data NULL substream\n", ntv2_aud->name);
		return;
	}

	runtime = substream->runtime;
	if (runtime == NULL) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* pcm_data NULL runtime\n", ntv2_aud->name);
		return;
	}

	if (runtime->dma_area == NULL) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* pcm_data NULL dma_area\n", ntv2_aud->name);
		return;
	}

	ring_sample_size = runtime->sample_bits / 8;
	if (ring_sample_size == 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* pcm_data bad sample size %d\n",
							 ntv2_aud->name, ring_sample_size);
		return;
	}

	ring_stride = runtime->frame_bits / 8;
	if (ring_stride == 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* pcm_data bad stride %d\n",
							 ntv2_aud->name, ring_stride);
		return;
	}

	ring_channels = ring_stride / ring_sample_size;

	buf_sample_size = sample_size;
	buf_channels = num_channels;
	buf_stride = buf_channels * buf_sample_size;
	buf_frames = size / buf_stride;

	old_ptr = stream->sample_ptr;
	if ((old_ptr + buf_frames) > runtime->buffer_size) {
		u32 cnt = runtime->buffer_size - old_ptr;
		if (address != NULL) {
			ntv2_copy_audio(runtime->dma_area + (old_ptr * ring_stride), address,
							ring_channels, buf_channels,
							ring_sample_size, buf_sample_size,
							cnt);
			ntv2_copy_audio(runtime->dma_area, address + (cnt * num_channels * 4),
							ring_channels, buf_channels,
							ring_sample_size, buf_sample_size,
							buf_frames - cnt);
		} else {
			memset(runtime->dma_area + (old_ptr * ring_stride), 0, cnt * ring_channels * ring_sample_size);
			memset(runtime->dma_area, 0, (buf_frames - cnt) * ring_channels * ring_sample_size);
		}
	} else {
		if (address != NULL) {
			ntv2_copy_audio(runtime->dma_area + (old_ptr * ring_stride), address,
							ring_channels, buf_channels,
							ring_sample_size, buf_sample_size,
							buf_frames);
		} else {
			memset(runtime->dma_area + (old_ptr * ring_stride), 0, buf_frames * ring_channels * ring_sample_size);
		}
	}

	snd_pcm_stream_lock(substream);
	stream->sample_ptr = (stream->sample_ptr + buf_frames)%runtime->buffer_size;
	stream->period_ptr += buf_frames;
	if (stream->period_ptr > runtime->period_size) {
		stream->period_ptr %= runtime->period_size;
		new_period = true;
	}
	snd_pcm_stream_unlock(substream);

	NTV2_MSG_AUDIO_STREAM("%s: pcm_data transfer done  ptr %d\n",
						  ntv2_aud->name,
						  stream->sample_ptr);

	if (new_period)
		snd_pcm_period_elapsed(substream);
}

static int ntv2_allocate_dma_buffer(struct ntv2_pcm_stream *stream)
{
	struct ntv2_audio *ntv2_aud = stream->ntv2_aud;
	unsigned long num_bytes = 0;
	unsigned long num_pages = 0;
	int res;

    if (stream->dma_buffer != NULL) {
		return -EPERM;
	}

	/* allocate the dma buffer */
	num_bytes = PAGE_ALIGN(NTV2_PCM_DMA_BUFFER_SIZE);
	stream->dma_buffer = vmalloc(num_bytes);
	if (stream->dma_buffer == NULL) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* dma buffer allocation failed\n",
							 ntv2_aud->name);
		ntv2_free_dma_buffer(stream);
		return -ENOMEM;
	}
	stream->dma_buffer_size = num_bytes;

	/* allocate the scatter buffer */
	res = ntv2_alloc_scatterlist(&stream->dma_sgtable,
								 stream->dma_buffer,
								 stream->dma_buffer_size);
	if (res < 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* dma scatter list allocation failed\n",
							 ntv2_aud->name);
		ntv2_free_dma_buffer(stream);
		return -ENOMEM;
	}

	/* map the scatter list */
	num_pages = dma_map_sg(&ntv2_aud->ntv2_dev->pci_dev->dev,
						   stream->dma_sgtable.sgl,
						   stream->dma_sgtable.nents,
						   DMA_FROM_DEVICE);
	if (num_pages <= 0) {
		NTV2_MSG_VIDEO_ERROR("%s: *error* dma scatter list map failed\n",
							 ntv2_aud->name);
		ntv2_free_dma_buffer(stream);
		return -ENOMEM;
	}
	stream->dma_buffer_pages = num_pages;

	return 0;
}

static void ntv2_free_dma_buffer(struct ntv2_pcm_stream *stream)
{
	struct ntv2_audio *ntv2_aud = stream->ntv2_aud;

	if (stream->dma_buffer_pages > 0)
		dma_unmap_sg(&ntv2_aud->ntv2_dev->pci_dev->dev,
					 stream->dma_sgtable.sgl,
					 stream->dma_sgtable.nents,
					 DMA_FROM_DEVICE);
	stream->dma_buffer_pages = 0;

	ntv2_free_scatterlist(&stream->dma_sgtable);

	if (stream->dma_buffer != NULL)
		vfree(stream->dma_buffer);

	stream->dma_buffer = NULL;
	stream->dma_buffer_size = 0;
	stream->dma_buffer_pages = 0;
}

static void ntv2_copy_audio(void *dst_buffer, void *src_buffer,
							u32 dst_channels, u32 src_channels,
							u32 dst_sample_size, u32 src_sample_size,
							u32 samples)
{
	u32 dst_copy = 0;
	u32 dst_skip = 0;
	u32 src_skip = 0;
	u32 i, j;

	if (dst_buffer == NULL)
		return;

	if (src_buffer == NULL) {
		memset(dst_buffer, 0, samples * dst_channels * dst_sample_size);
		return;
	}

	if (src_channels > dst_channels) {
		dst_copy = dst_channels;
		src_skip = src_channels - dst_channels;
	} else {
		dst_copy = src_channels;
		dst_skip = dst_channels - src_channels;
	}

	if ((src_sample_size == 4) && (dst_sample_size == 4)) {
		s32 *dst = (s32*)dst_buffer;
		s32 *src = (s32*)src_buffer;
		for (i = 0; i < samples; i++) {
			for (j = 0; j < dst_copy; j++)
				*dst++ = *src++;
			for (j = 0; j < dst_skip; j++)
				*dst++ = 0;
			dst += dst_skip;
			src += src_skip;
		}
		return;
	}

	if ((src_sample_size == 4) && (dst_sample_size == 2)) {
		s32 *src = (s32*)src_buffer;
		s16 *dst = (s16*)dst_buffer;
		for (i = 0; i < samples; i++) {
			for (j = 0; j < dst_copy; j++)
				*dst++ = (*src++) >> 16;
			for (j = 0; j < dst_skip; j++)
				*dst++ = 0;
			dst += dst_skip;
			src += src_skip;
		}
		return;
	}

	/* copy sample sizes not supported */
	memset(dst_buffer, 0, samples * dst_channels * dst_sample_size);
}
