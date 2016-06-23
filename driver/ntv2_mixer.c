/*
 * NTV2 alsa mixer
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
#include "ntv2_mixer.h"
#include "ntv2_channel.h"

static int ntv2_mixer_num_audio(struct ntv2_device *ntv2_dev);
static struct ntv2_audio *ntv2_mixer_get_audio(struct ntv2_device *ntv2_dev, int index);


static int ntv2_mixer_info_source_control(struct snd_kcontrol *kcontrol,
										  struct snd_ctl_elem_info *info)
{
	struct ntv2_device *ntv2_dev = (struct ntv2_device *)snd_kcontrol_chip(kcontrol);
	struct ntv2_audio *ntv2_aud;
	struct ntv2_features *features;
	struct ntv2_source_config *config;
	u32 num_sources;

	ntv2_aud = ntv2_mixer_get_audio(ntv2_dev, info->id.index);
	if (ntv2_aud == NULL)
		return -EINVAL;

	features = ntv2_aud->features;
	num_sources = ntv2_features_num_source_configs(ntv2_aud->features, ntv2_aud->ntv2_chn->index);

	info->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	info->count = 1;
	info->value.enumerated.items = num_sources;

	if (info->value.enumerated.item >= num_sources)
		info->value.enumerated.item = num_sources - 1;

	config = ntv2_features_get_source_config(features,
											 ntv2_aud->ntv2_chn->index, 
											 info->value.enumerated.item);

	if (config != NULL) {
		strlcpy(info->value.enumerated.name, config->name, sizeof(info->value.enumerated.name));
		NTV2_MSG_AUDIO_STATE("%s: enumerate audio item %d - %s\n",
							 ntv2_aud->name, (int)info->value.enumerated.item, config->name);
	}
	else {
		strlcpy(info->value.enumerated.name, "bad source", sizeof(info->value.enumerated.name));
		NTV2_MSG_AUDIO_ERROR("%s: *error* enumerate bad source\n",
							 ntv2_aud->name);
	}

	return 0;
}

static int ntv2_mixer_get_source_control(struct snd_kcontrol *kcontrol,
										 struct snd_ctl_elem_value *elem)
{
	struct ntv2_device *ntv2_dev = (struct ntv2_device *)snd_kcontrol_chip(kcontrol);
	struct ntv2_audio *ntv2_aud;
	struct ntv2_source_config *config;

	ntv2_aud = ntv2_mixer_get_audio(ntv2_dev, elem->id.index);
	if (ntv2_aud == NULL)
		return -EINVAL;

	config = ntv2_features_get_source_config(ntv2_aud->features,
											 ntv2_aud->ntv2_chn->index,
											 ntv2_aud->source_index);
	if (config == NULL) {
		ntv2_aud->source_index = 0;
		config = ntv2_features_get_source_config(ntv2_aud->features,
												 ntv2_aud->ntv2_chn->index,
												 ntv2_aud->source_index);
	}

	elem->value.enumerated.item[0] = ntv2_aud->source_index;

	NTV2_MSG_AUDIO_STATE("%s: get audio item %d - %s\n",
						 ntv2_aud->name, elem->value.enumerated.item[0], config->name);

	return 0;
}

static int ntv2_mixer_put_source_control(struct snd_kcontrol *kcontrol,
										 struct snd_ctl_elem_value *elem)
{
	struct ntv2_device *ntv2_dev = (struct ntv2_device *)snd_kcontrol_chip(kcontrol);
	struct ntv2_audio *ntv2_aud;
	struct ntv2_source_config *config;

	ntv2_aud = ntv2_mixer_get_audio(ntv2_dev, elem->id.index);
	if (ntv2_aud == NULL)
		return -EINVAL;

	ntv2_aud->source_index = elem->value.enumerated.item[0];

	config = ntv2_features_get_source_config(ntv2_aud->features,
											 ntv2_aud->ntv2_chn->index, 
											 ntv2_aud->source_index);
	if (config == NULL) {
		ntv2_aud->source_index = 0;
		config = ntv2_features_get_source_config(ntv2_aud->features,
												 ntv2_aud->ntv2_chn->index,
												 ntv2_aud->source_index);
	}

	NTV2_MSG_AUDIO_STATE("%s: put audio item %d - %s\n",
						 ntv2_aud->name, elem->value.enumerated.item[0], config->name);

	return ntv2_audio_set_source(ntv2_aud, config);
}

static struct snd_kcontrol_new ntv2_snd_controls[] = {
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name =	"Source Capture Route",
		.count = 1,
		.info =	ntv2_mixer_info_source_control,
		.get =	ntv2_mixer_get_source_control,
		.put =	ntv2_mixer_put_source_control
	}
};

int ntv2_mixer_configure(struct ntv2_device *ntv2_dev)
{
	int i;
	int ret;
	int count;

	if (ntv2_dev == NULL)
		return -EPERM;

	NTV2_MSG_AUDIO_INFO("%s: configure audio mixer\n", ntv2_dev->name);

	count = ntv2_mixer_num_audio(ntv2_dev);
	if (count == 0)
		return 0;

	for (i = 0; i < (int)ARRAY_SIZE(ntv2_snd_controls); i++) {
		ntv2_snd_controls[i].count = count;
		ret = snd_ctl_add(ntv2_dev->snd_card, snd_ctl_new1(&ntv2_snd_controls[i], ntv2_dev));
		if (ret != 0) {
			NTV2_MSG_AUDIO_ERROR("%s: *error* adding audio control %08x\n",
								 ntv2_dev->name, ret);
		}
	}

	return 0;
}

static int ntv2_mixer_num_audio(struct ntv2_device *ntv2_dev)
{
	struct list_head *ptr;
	struct list_head *next;
	int count = 0;

	list_for_each_safe(ptr, next, &ntv2_dev->audio_list) {
		count++;
	}

	return count;
}

static struct ntv2_audio
*ntv2_mixer_get_audio(struct ntv2_device *ntv2_dev, int index)
{
	struct list_head *ptr;
	struct list_head *next;
	struct ntv2_audio *aud = NULL;
	int count = 0;

	list_for_each_safe(ptr, next, &ntv2_dev->audio_list) {
		if (count == index) {
			aud = list_entry(ptr, struct ntv2_audio, list);
			break;
		}
		count++;
	}

	return aud;
}

