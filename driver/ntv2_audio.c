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

#include "ntv2_audio.h"
#include "ntv2_features.h"
#include "ntv2_pcmops.h"
#include "ntv2_channel.h"
#include "ntv2_nwldma.h"
#include "ntv2_mixer.h"


#define NTV2_AUDIO_TRANSFER_TIMEOUT			(100000)

static void ntv2_audio_capture_task(unsigned long data);
static void ntv2_audio_playback_task(unsigned long data);
static void ntv2_audio_dma_callback(unsigned long data, int result);
static void ntv2_audio_channel_callback(unsigned long data);


struct ntv2_audio *ntv2_audio_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index)
{
	struct ntv2_audio *ntv2_aud = NULL;

	if (ntv2_obj == NULL)
		return NULL;

	ntv2_aud = kzalloc(sizeof(struct ntv2_audio), GFP_KERNEL);
	if (ntv2_aud == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_audio instance memory allocation failed\n", ntv2_obj->name);
		return NULL;
	}

	ntv2_aud->index = index;
	snprintf(ntv2_aud->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_obj->name, name, index);
	INIT_LIST_HEAD(&ntv2_aud->list);
	ntv2_aud->ntv2_dev = ntv2_obj->ntv2_dev;

	ntv2_aud->init = true;

	NTV2_MSG_AUDIO_INFO("%s: open ntv2_audio\n", ntv2_aud->name);

	return ntv2_aud;
}

void ntv2_audio_close(struct ntv2_audio *ntv2_aud)
{
	if (ntv2_aud == NULL) 
		return;

	NTV2_MSG_AUDIO_INFO("%s: close ntv2_audio\n", ntv2_aud->name);

	/* stop the queues */
	if (ntv2_aud->capture != NULL) {
		ntv2_audio_disable(ntv2_aud->capture);
		tasklet_kill(&ntv2_aud->capture->transfer_task);
		memset(ntv2_aud->capture, 0, sizeof(struct ntv2_pcm_stream));
		kfree(ntv2_aud->capture);
		ntv2_aud->capture = NULL;
	}
	if (ntv2_aud->playback != NULL) {
		ntv2_audio_disable(ntv2_aud->playback);
		tasklet_kill(&ntv2_aud->playback->transfer_task);
		memset(ntv2_aud->playback, 0, sizeof(struct ntv2_pcm_stream));
		kfree(ntv2_aud->playback);
		ntv2_aud->playback = NULL;
	}

	memset(ntv2_aud, 0, sizeof(struct ntv2_audio));
	kfree(ntv2_aud);
}

int ntv2_audio_configure(struct ntv2_audio *ntv2_aud,
						 struct ntv2_features *features,
						 struct snd_card *snd_card,
						 struct ntv2_channel *ntv2_chn,
						 struct ntv2_nwldma *ntv2_nwl)
{
	struct ntv2_pcm_stream *stream;
	bool capture;
	bool playback;
	int result;

	if ((ntv2_aud == NULL) ||
		(features == NULL) ||
		(snd_card == NULL) ||
		(ntv2_chn == NULL) ||
		(ntv2_nwl == NULL))
		return -EPERM;

	NTV2_MSG_AUDIO_INFO("%s: configure audio device\n", ntv2_aud->name);

	ntv2_aud->features = features;
	ntv2_aud->snd_card = snd_card;
	ntv2_aud->ntv2_chn = ntv2_chn;
	ntv2_aud->dma_engine = ntv2_nwl;

	capture = ntv2_aud->features->audio_config[ntv2_aud->index]->capture;
	playback = ntv2_aud->features->audio_config[ntv2_aud->index]->playback;

	result = snd_pcm_new(ntv2_aud->snd_card,
						 features->pcm_name,
						 ntv2_aud->index,
						 playback? 1 : 0, 
						 capture? 1 : 0, 
						 &ntv2_aud->pcm);
	if (result < 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* snd_pcm_new() failed code %d\n",
							 ntv2_aud->name, result);
		return result;
	}
	ntv2_aud->pcm->private_data = ntv2_aud;

	NTV2_MSG_AUDIO_INFO("%s: register audio pcm device: %s\n",
						ntv2_aud->name, features->pcm_name);

	if (capture) {
		stream = kzalloc(sizeof(struct ntv2_pcm_stream), GFP_KERNEL);
		if (stream == NULL)
			return -ENOMEM;

		stream->type = ntv2_stream_type_audin;
		stream->ntv2_aud = ntv2_aud;
		stream->chn_str = ntv2_channel_stream(ntv2_chn, ntv2_stream_type_audin);
		spin_lock_init(&stream->state_lock);
		tasklet_init(&stream->transfer_task,
					 ntv2_audio_capture_task,
					 (unsigned long)stream);

		result = ntv2_pcmops_configure(stream);
		if (result < 0)
			return result;

		ntv2_aud->capture = stream;
	}
	if (playback) {
		stream = kzalloc(sizeof(struct ntv2_pcm_stream), GFP_KERNEL);
		if (stream == NULL)
			return -ENOMEM;

		stream->type = ntv2_stream_type_audout;
		stream->ntv2_aud = ntv2_aud;
		stream->chn_str = ntv2_channel_stream(ntv2_chn, ntv2_stream_type_audout);
		spin_lock_init(&stream->state_lock);
		tasklet_init(&stream->transfer_task,
					 ntv2_audio_playback_task,
					 (unsigned long)stream);

		result = ntv2_pcmops_configure(stream);
		if (result < 0)
			return result;

		ntv2_aud->playback = stream;
	}


	result = ntv2_mixer_configure(ntv2_aud);
	if (result < 0)
		return result;

	return 0;
}

struct ntv2_pcm_stream *ntv2_audio_capture_stream(struct ntv2_audio *ntv2_aud)
{
	if (ntv2_aud == NULL)
		return NULL;

	return ntv2_aud->capture;
}

struct ntv2_pcm_stream *ntv2_audio_playback_stream(struct ntv2_audio *ntv2_aud)
{
	if (ntv2_aud == NULL)
		return NULL;

	return ntv2_aud->playback;
}

int ntv2_audio_enable(struct ntv2_pcm_stream *stream)
{
	struct ntv2_audio *ntv2_aud;
	unsigned long flags;
	int result;

	if (stream == NULL)
		return -EPERM;

	ntv2_aud = stream->ntv2_aud;

	if (stream->transfer_state == ntv2_task_state_enable)
		return 0;

	NTV2_MSG_AUDIO_STATE("%s: %s transfer task enable\n",
						 ntv2_aud->name,
						 ntv2_stream_name(stream->type));

	/* initialize the audio state */
	spin_lock_irqsave(&stream->state_lock, flags);
	stream->dma_audbuf = NULL;
	stream->dma_start = false;
	stream->dma_done = false;
	stream->dma_result = 0;
	stream->transfer_state = ntv2_task_state_enable;
	spin_unlock_irqrestore(&stream->state_lock, flags);

	/* enable the channel */
	ntv2_channel_set_frame_callback(stream->chn_str,
									ntv2_audio_channel_callback,
									(unsigned long)stream);

	result = ntv2_channel_enable(stream->chn_str);
	if (result != 0) {
		return result;
	}

	/* schedule the transfer task */
	tasklet_schedule(&stream->transfer_task);

	return 0;
}

int ntv2_audio_disable(struct ntv2_pcm_stream *stream)
{
	struct ntv2_audio *ntv2_aud;
	unsigned long flags;
	int result;

	if (stream == NULL)
		return -EINVAL;

	ntv2_aud = stream->ntv2_aud;

	if (stream->transfer_state != ntv2_task_state_enable)
		return 0;

	NTV2_MSG_AUDIO_STATE("%s: %s transfer task disable\n",
						 ntv2_aud->name,
						 ntv2_stream_name(stream->type));

	/* disable channel */
	ntv2_channel_set_frame_callback(stream->chn_str, NULL, 0);

	spin_lock_irqsave(&stream->state_lock, flags);
	stream->transfer_state = ntv2_task_state_disable;
	spin_unlock_irqrestore(&stream->state_lock, flags);

	/* schedule the transfer task */
	tasklet_schedule(&stream->transfer_task);

	/* wait for transfer task stop */
	result = ntv2_wait((int*)&stream->task_state,
					   (int)ntv2_task_state_disable,
					   NTV2_AUDIO_TRANSFER_TIMEOUT);
	if (result != 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* %s timeout waiting for transfer task stop\n",
							 ntv2_aud->name,
							 ntv2_stream_name(stream->type));
		return result;
	}

	ntv2_channel_flush(stream->chn_str);
	ntv2_channel_disable(stream->chn_str);

	return 0;
}

int ntv2_audio_start(struct ntv2_pcm_stream *stream)
{
	int result;

	if (stream == NULL)
		return -EINVAL;

	stream->trigger = true;
	result = ntv2_channel_start(stream->chn_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

int ntv2_audio_stop(struct ntv2_pcm_stream *stream)
{
	int result;

	if (stream == NULL)
		return -EINVAL;

	result = ntv2_channel_stop(stream->chn_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

int ntv2_audio_flush(struct ntv2_pcm_stream *stream)
{
	int result;

	if (stream == NULL)
		return -EINVAL;

	result = ntv2_channel_flush(stream->chn_str);
	if (result != 0) {
		return result;
	}

	return 0;
}

static void ntv2_audio_capture_task(unsigned long data)
{
	struct ntv2_pcm_stream *stream = (struct ntv2_pcm_stream*)data;
	struct ntv2_audio *ntv2_aud = stream->ntv2_aud;
	unsigned long flags;
	bool dodma = false;
	int result;

	spin_lock_irqsave(&stream->state_lock, flags);
	if (!stream->dma_start)
		stream->task_state = stream->transfer_state;
	if (stream->task_state != ntv2_task_state_enable) {
		spin_unlock_irqrestore(&stream->state_lock, flags);
		return;
	}
	
	if (stream->dma_done) {
		ntv2_pcmops_copy_audio(stream, 
							   ((stream->dma_result == 0)? stream->dma_buffer : NULL),
							   stream->dma_size,
							   stream->dma_audbuf->audio.num_channels,
							   stream->dma_audbuf->audio.sample_size);
		ntv2_channel_data_done(stream->dma_audbuf);
		stream->dma_audbuf = NULL;
		stream->dma_start = false;
		stream->dma_done = false;
		stream->dma_result = 0;
		stream->dma_size = 0;
	}

	if (!stream->dma_start) {
		stream->dma_audbuf = ntv2_channel_data_ready(stream->chn_str);
		if (stream->dma_audbuf != NULL) {
			stream->dma_start = true;
			dodma = true;
		}
	}

	spin_unlock_irqrestore(&stream->state_lock, flags);

	if (dodma) {
		stream->dma_size =
			stream->dma_audbuf->audio.data_size[0] +
			stream->dma_audbuf->audio.data_size[1];
		result = ntv2_nwldma_transfer(ntv2_aud->dma_engine,
									  ntv2_nwldma_mode_c2s,
									  stream->dma_sgtable.sgl,
									  stream->dma_buffer_pages,
									  0,
									  stream->dma_audbuf->audio.address,
									  stream->dma_audbuf->audio.data_size,
									  ntv2_audio_dma_callback,
									  (unsigned long)stream);
		if (result != 0) {
			stream->dma_done = true;
			stream->dma_result = result;
		}
	}
}

static void ntv2_audio_playback_task(unsigned long data)
{
}

static void ntv2_audio_dma_callback(unsigned long data, int result)
{
	struct ntv2_pcm_stream *stream = (struct ntv2_pcm_stream *)data;
	unsigned long flags;

	if (stream == NULL)
		return;

	/* record dma complete */
	spin_lock_irqsave(&stream->state_lock, flags);
	stream->dma_done = true;
	stream->dma_result = result;
	spin_unlock_irqrestore(&stream->state_lock, flags);

	/* schedule the dma task */
	tasklet_schedule(&stream->transfer_task);
}

static void ntv2_audio_channel_callback(unsigned long data)
{
	struct ntv2_pcm_stream *stream = (struct ntv2_pcm_stream *)data;

	if (stream == NULL)
		return;

	/* timestamp the audio capture start */
	if (stream->trigger) {
		ntv2_pcmops_tstamp(stream);
		stream->trigger = false;
	}

	/* schedule the dma task */
	tasklet_schedule(&stream->transfer_task);
}
