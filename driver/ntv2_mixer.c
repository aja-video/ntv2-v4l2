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

static int ntv2_mixer_info_source_control(struct snd_kcontrol *kcontrol,
										  struct snd_ctl_elem_info *info)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_features *features = ntv2_aud->features;
	struct ntv2_source_config *config;
	u32 num_sources = ntv2_features_num_source_configs(ntv2_aud->features, ntv2_aud->ntv2_chn->index);

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
		NTV2_MSG_AUDIO_STATE("%s: enumerate audio item %d of %d  source %s\n",
							 ntv2_aud->name, (int)info->value.enumerated.item, num_sources, config->name);
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
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_features *features = ntv2_aud->features;
	struct ntv2_source_format format;
	struct ntv2_source_config *config;
	u32 num_sources = ntv2_features_num_source_configs(ntv2_aud->features, ntv2_aud->ntv2_chn->index);
	u32 i;

	ntv2_channel_get_source_format(ntv2_aud->capture->chn_str, &format);

	for (i = 0; i < num_sources; i++) {
		config = ntv2_features_get_source_config(features, ntv2_aud->ntv2_chn->index, i);
		if ((config != NULL) && (config->type == format.type))
			break;
	}

	if (i >= num_sources) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* bad audio source format type %d\n",
							 ntv2_aud->name, format.type);
		i = 0;
		config = ntv2_features_get_source_config(features, ntv2_aud->ntv2_chn->index, i);
		ntv2_features_gen_source_format(config, &format);
		ntv2_channel_set_source_format(ntv2_aud->capture->chn_str, &format);
	}

	elem->value.enumerated.item[0] = i;

	NTV2_MSG_AUDIO_STATE("%s: get audio source: %s\n",
						 ntv2_aud->name, config->name);

	return 0;
}

static int ntv2_mixer_put_source_control(struct snd_kcontrol *kcontrol,
										 struct snd_ctl_elem_value *elem)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_features *features = ntv2_aud->features;
	struct ntv2_source_format format;
	struct ntv2_source_config *config;

	ntv2_channel_get_source_format(ntv2_aud->capture->chn_str, &format);

	config = ntv2_features_get_source_config(features,
											 ntv2_aud->ntv2_chn->index, 
											 elem->value.enumerated.item[0]);

	if (config == NULL) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* bad audio source item %d\n",
							 ntv2_aud->name, (int)elem->value.enumerated.item[0]);
		config = ntv2_features_get_source_config(features, ntv2_aud->ntv2_chn->index, 0);
	}

	if (config->type != format.type) {
		ntv2_features_gen_source_format(config, &format);
		ntv2_channel_set_source_format(ntv2_aud->capture->chn_str, &format);
		return 1;
	}

	NTV2_MSG_AUDIO_STATE("%s: put audio source: %s\n",
						 ntv2_aud->name, config->name);

	return 0;
}

static struct snd_kcontrol_new ntv2_snd_controls[] = {
	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name =	"Audio Source",
		.info =	ntv2_mixer_info_source_control,
		.get =	ntv2_mixer_get_source_control,
		.put =	ntv2_mixer_put_source_control
	}
};

int ntv2_mixer_configure(struct ntv2_audio *ntv2_aud)
{
	int i;
	int ret;

	if (ntv2_aud == NULL)
		return -EPERM;

	NTV2_MSG_AUDIO_INFO("%s: close ntv2_audio\n", ntv2_aud->name);

	for (i = 0; i < (int)ARRAY_SIZE(ntv2_snd_controls); i++) {
		ret = snd_ctl_add(ntv2_aud->snd_card, snd_ctl_new1(&ntv2_snd_controls[i], ntv2_aud));
		if (ret != 0) {
			NTV2_MSG_AUDIO_ERROR("%s: *error* adding audio control %08x\n",
								 ntv2_aud->name, ret);
		}
	}

	return 0;
}
