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
#include "ntv2_mixops.h"
#include "ntv2_channel.h"


static int ntv2_mixops_info_source_control(struct snd_kcontrol *kcontrol,
										  struct snd_ctl_elem_info *info)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_features *features;
	struct ntv2_source_config *config;
	u32 num_sources;

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

static int ntv2_mixops_get_source_control(struct snd_kcontrol *kcontrol,
										 struct snd_ctl_elem_value *elem)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_source_config *config;

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

static int ntv2_mixops_put_source_control(struct snd_kcontrol *kcontrol,
										 struct snd_ctl_elem_value *elem)
{
	struct ntv2_audio *ntv2_aud = (struct ntv2_audio *)snd_kcontrol_chip(kcontrol);
	struct ntv2_source_config *config;

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

int ntv2_mixops_capture_configure(struct ntv2_audio *ntv2_aud)
{
	struct snd_kcontrol_new snd_control;
	char control_name[80];
	int ret;

	if (ntv2_aud == NULL)
		return -EPERM;

	memset(&snd_control, 0, sizeof(snd_control));

	snd_control.iface = SNDRV_CTL_ELEM_IFACE_MIXER;
	snd_control.name = control_name;
	snd_control.count = 1;
	snd_control.info = ntv2_mixops_info_source_control;
	snd_control.get = ntv2_mixops_get_source_control;
	snd_control.put = ntv2_mixops_put_source_control;
	
	snprintf(control_name, sizeof(control_name), "Channel %d Source Capture Route",
			 ntv2_aud->ntv2_chn->index + 1);

	ret = snd_ctl_add(ntv2_aud->ntv2_dev->snd_card, snd_ctl_new1(&snd_control, ntv2_aud));
	if (ret != 0) {
		NTV2_MSG_AUDIO_ERROR("%s: *error* adding audio control %08x\n",
							 ntv2_aud->ntv2_dev->name, ret);
	}

	return 0;
}
