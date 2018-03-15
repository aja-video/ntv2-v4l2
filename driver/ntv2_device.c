/*
 * NTV2 device interface
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

#include "ntv2_device.h"
#include "ntv2_video.h"
#include "ntv2_audio.h"
#include "ntv2_mixops.h"
#include "ntv2_serial.h"
#include "ntv2_channel.h"
#include "ntv2_register.h"
#include "ntv2_nwldma.h"
#include "ntv2_nwlreg.h"
#include "ntv2_konareg.h"
#include "ntv2_features.h"
#include "ntv2_input.h"
#include "ntv2_chrdev.h"

static int ntv2_device_pci_configure(struct ntv2_device *ntv2_dev, struct pci_dev *pdev);
static void ntv2_device_pci_release(struct ntv2_device *ntv2_dev);

static int ntv2_device_dma_configure(struct ntv2_device *ntv2_dev);
static void ntv2_device_dma_release(struct ntv2_device *ntv2_dev);

static int ntv2_device_irq_configure(struct ntv2_device *ntv2_dev);
static void ntv2_device_irq_release(struct ntv2_device *ntv2_dev);

static void ntv2_device_irq_enable(struct ntv2_device *ntv2_dev);
static void ntv2_device_irq_disable(struct ntv2_device *ntv2_dev);

static irqreturn_t ntv2_device_interrupt(int irq, void* dev_id);
static void ntv2_device_init_hardware(struct ntv2_device *ntv2_dev);
static void ntv2_device_monitor(unsigned long data);

/*
 * Initialize device info
 */
struct ntv2_device *ntv2_device_open(struct ntv2_module *ntv2_mod,
									 const char *name, int index)
{
	struct ntv2_device *ntv2_dev = NULL;

	ntv2_dev = kzalloc(sizeof(struct ntv2_device), GFP_KERNEL);
	if (ntv2_dev == NULL) {
		NTV2_MSG_ERROR("%s: ntv2_device instance memory allocation failed\n", ntv2_mod->name);
		return NULL;
	}

	ntv2_dev->index = index;
	snprintf(ntv2_dev->name, NTV2_STRING_SIZE, "%s-%s%d", ntv2_mod->name, name, index);
	INIT_LIST_HEAD(&ntv2_dev->list);
	ntv2_dev->ntv2_dev = ntv2_dev;
	ntv2_dev->ntv2_mod = ntv2_mod;

	/* video list */
	INIT_LIST_HEAD(&ntv2_dev->video_list);
	spin_lock_init(&ntv2_dev->video_lock);
	atomic_set(&ntv2_dev->video_index, 0);

	/* audio list */
	INIT_LIST_HEAD(&ntv2_dev->audio_list);
	spin_lock_init(&ntv2_dev->audio_lock);
	atomic_set(&ntv2_dev->audio_index, 0);

	/* serial list */
	INIT_LIST_HEAD(&ntv2_dev->serial_list);
	spin_lock_init(&ntv2_dev->serial_lock);
	atomic_set(&ntv2_dev->serial_index, 0);

	/* channel list */
	INIT_LIST_HEAD(&ntv2_dev->channel_list);

	spin_lock_init(&ntv2_dev->channel_lock);
	atomic_set(&ntv2_dev->channel_index, 0);

	NTV2_MSG_DEVICE_INFO("%s: open ntv2_device\n", ntv2_dev->name);

	return ntv2_dev;
}

/*
 * Release device info
 */
void ntv2_device_close(struct ntv2_device *ntv2_dev)
{
	struct list_head *ptr;
	struct list_head *next;
	struct ntv2_serial *ser;
	struct ntv2_audio *aud;
	struct ntv2_video *vid;
	struct ntv2_channel *chn;
	unsigned long flags;

	if (ntv2_dev == NULL) 
		return;

	NTV2_MSG_DEVICE_INFO("%s: close ntv2_device\n", ntv2_dev->name);

	ntv2_device_irq_disable(ntv2_dev);

	/* delete all serial objects */
	list_for_each_safe(ptr, next, &ntv2_dev->serial_list) {
		ser = list_entry(ptr, struct ntv2_serial, list);
		spin_lock_irqsave(&ntv2_dev->serial_lock, flags);
		list_del_init(&ser->list);
		spin_unlock_irqrestore(&ntv2_dev->serial_lock, flags);
		ntv2_serial_close(ser);
	}

	/* delete all audio objects */
	list_for_each_safe(ptr, next, &ntv2_dev->audio_list) {
		aud = list_entry(ptr, struct ntv2_audio, list);
		spin_lock_irqsave(&ntv2_dev->audio_lock, flags);
		list_del_init(&aud->list);
		spin_unlock_irqrestore(&ntv2_dev->audio_lock, flags);
		ntv2_audio_close(aud);
	}

	/* delete all video objects */
	list_for_each_safe(ptr, next, &ntv2_dev->video_list) {
		vid = list_entry(ptr, struct ntv2_video, list);
		spin_lock_irqsave(&ntv2_dev->video_lock, flags);
		list_del_init(&vid->list);
		spin_unlock_irqrestore(&ntv2_dev->video_lock, flags);
		ntv2_video_close(vid);
	}

	/* delete all channel objects */
	list_for_each_safe(ptr, next, &ntv2_dev->channel_list) {
		chn = list_entry(ptr, struct ntv2_channel, list);
		spin_lock_irqsave(&ntv2_dev->channel_lock, flags);
		list_del_init(&chn->list);
		spin_unlock_irqrestore(&ntv2_dev->channel_lock, flags);
		ntv2_channel_close(chn);
	}

	/* free the audio system */
	if (ntv2_dev->snd_card != NULL)
		snd_card_free(ntv2_dev->snd_card);

	/* free the input monitor */
	ntv2_input_close(ntv2_dev->inp_mon);

	/* close the character device */
	ntv2_chrdev_close(ntv2_dev->chr_dev);

	/* release the resources */
	ntv2_device_irq_release(ntv2_dev);
	ntv2_nwldma_close(ntv2_dev->dma_engine);
	ntv2_device_dma_release(ntv2_dev);
	ntv2_register_close(ntv2_dev->pci_reg);
	ntv2_register_close(ntv2_dev->vid_reg);
	ntv2_device_pci_release(ntv2_dev);
	ntv2_features_close(ntv2_dev->features);
	memset(ntv2_dev, 0, sizeof(struct ntv2_device));
	kfree(ntv2_dev);
}

/*
 * Configure device
 */
int ntv2_device_configure(struct ntv2_device *ntv2_dev,
						  struct pci_dev *pdev)
{
	struct ntv2_channel *ntv2_chn;
	struct ntv2_video *ntv2_vid;
	struct ntv2_audio *ntv2_aud;
	struct ntv2_serial *ntv2_ser;
	unsigned long flags;
	u32 device_id;
	int num_channels;
	int num_video;
	int num_audio;
	int num_serial;
	int index;
	int result;
#ifndef TEST_HDMI	
	int i;
#endif
	if ((ntv2_dev == NULL) || (pdev == NULL))
		return -EPERM;

	/* initialize register data */
	ntv2_kona_register_initialize();

	/* configure ntv2 pci resources */
	result = ntv2_device_pci_configure(ntv2_dev, pdev);
	if (result != 0) 
		return result;

	/* initialize register access */
	if (ntv2_dev->pci_region) {
		ntv2_dev->pci_reg = ntv2_register_open((struct ntv2_object*)ntv2_dev, ntv2_pci_name(ntv2_dev->pci_type), 0);
		if (ntv2_dev->pci_reg == NULL)
			return -ENOMEM;

		result = ntv2_register_configure(ntv2_dev->pci_reg,
										 ntv2_dev->pci_base,
										 ntv2_dev->pci_size);
		if (result != 0)
			return result;

		result = ntv2_register_enable(ntv2_dev->pci_reg);
		if (result != 0)
			return result;
	}

	if (ntv2_dev->vid_region) {
		ntv2_dev->vid_reg = ntv2_register_open((struct ntv2_object*)ntv2_dev, "avr", 0);
		if (ntv2_dev->vid_reg == NULL)
			return -ENOMEM;

		result = ntv2_register_configure(ntv2_dev->vid_reg, 
										 ntv2_dev->vid_base,
										 ntv2_dev->vid_size);
		if (result != 0)
			return result;

		result = ntv2_register_enable(ntv2_dev->vid_reg);
		if (result != 0)
			return result;
	}

	/* read device id */
	device_id = ntv2_reg_read(ntv2_dev->vid_reg, ntv2_kona_reg_device_id, 0);

	/* configure ntv2 features */
	ntv2_dev->features = ntv2_features_open((struct ntv2_object*)ntv2_dev, "ftr", 0);
	if (ntv2_dev->features == NULL)
		return  -ENOMEM;

	result = ntv2_features_configure(ntv2_dev->features, device_id);
	if (result != 0) {
		NTV2_MSG_DEVICE_ERROR("%s: *error* no features for device id %08x\n",
							  ntv2_dev->name, device_id);
		return result;
	}
	NTV2_MSG_DEVICE_INFO("%s: device name %s  device id %08x\n",
						 ntv2_dev->name,
						 ntv2_dev->features->device_name,
						 ntv2_dev->features->device_id);

	/* initialize ntv2 hardware */
	ntv2_device_init_hardware(ntv2_dev);
	
	/* configure ntv2 dma resources */
	result = ntv2_device_dma_configure(ntv2_dev);
	if (result != 0) 
		return result;

	/* configure ntv2 irq resources */
	result = ntv2_device_irq_configure(ntv2_dev);
	if (result != 0)
		return result;
#ifndef TEST_HDMI
	/* initialize ntv2 dma engines */
	ntv2_dev->dma_engine = ntv2_nwldma_open((struct ntv2_object*)ntv2_dev, "nwd", 4);
	if (ntv2_dev->dma_engine == NULL)
		return  -ENOMEM;

	result = ntv2_nwldma_configure(ntv2_dev->dma_engine, ntv2_dev->pci_reg);
	if (result != 0)
		return result;

	result = ntv2_nwldma_enable(ntv2_dev->dma_engine);
	if (result != 0)
		return result;
#endif
	/* initialize ntv2 input monitor */
	ntv2_dev->inp_mon = ntv2_input_open((struct ntv2_object*)ntv2_dev, "inp", 0);
	if (ntv2_dev->inp_mon == NULL)
		return  -ENOMEM;

	result = ntv2_input_configure(ntv2_dev->inp_mon,
								  ntv2_dev->features,
								  ntv2_dev->vid_reg);
	if (result != 0)
		return result;

	/* enable input detection */
	ntv2_input_enable(ntv2_dev->inp_mon);
#ifndef TEST_HDMI
	/* initialize character device */
	ntv2_dev->chr_dev = ntv2_chrdev_open((struct ntv2_object*)ntv2_dev, "chr", 0);
	if (ntv2_dev->chr_dev == NULL)
		return  -ENOMEM;

	result = ntv2_chrdev_configure(ntv2_dev->chr_dev,
								   ntv2_dev->features,
								   ntv2_dev->vid_reg);
	if (result != 0)
		return result;

	ntv2_chrdev_enable(ntv2_dev->chr_dev);
	
	/* create the audio system */
#ifdef NTV2_USE_SND_CARD_NEW
	result = snd_card_new(&ntv2_dev->pci_dev->dev,
						  SNDRV_DEFAULT_IDX1,
						  SNDRV_DEFAULT_STR1,
						  THIS_MODULE,
						  0,
						  &ntv2_dev->snd_card);
#else
	result = snd_card_create(SNDRV_DEFAULT_IDX1,
							 SNDRV_DEFAULT_STR1,
							 THIS_MODULE,
							 0,
							 &ntv2_dev->snd_card);
#endif
	if (result < 0) {
		NTV2_MSG_DEVICE_ERROR("%s: *error* snd_card_new/create failed code %d\n",
							  ntv2_dev->name, result);
		return result;
	}

	strcpy(ntv2_dev->snd_card->driver, ntv2_dev->ntv2_mod->name);
	strcpy(ntv2_dev->snd_card->shortname, ntv2_dev->features->device_name);
	snprintf(ntv2_dev->snd_card->longname,
			 sizeof(ntv2_dev->snd_card->longname),
			 "AJA %s IO Board on %s  irq %d",
			 ntv2_dev->snd_card->shortname,
			 pci_name(ntv2_dev->pci_dev),
			 ntv2_dev->pci_dev->irq);

	num_video = ntv2_dev->features->num_video_channels;
	num_audio = ntv2_dev->features->num_audio_channels;
	num_channels = max(num_video, num_audio);
	num_serial = ntv2_dev->features->num_serial_ports;

	for (i = 0; i < num_channels; i++) {
		/* allocate and initialize channel instance */
		index = atomic_inc_return(&ntv2_dev->channel_index) - 1;
		ntv2_chn = ntv2_channel_open((struct ntv2_object*)ntv2_dev, "chn", index);

		/* configure channel */
		result = ntv2_channel_configure(ntv2_chn,
										ntv2_dev->features,
										ntv2_dev->vid_reg);
		if (result != 0) {
			ntv2_channel_close(ntv2_chn);
			return result;
		}

		/* add to the channel list */
		spin_lock_irqsave(&ntv2_dev->channel_lock, flags);
		list_add_tail(&ntv2_chn->list, &ntv2_dev->channel_list);
		spin_unlock_irqrestore(&ntv2_dev->channel_lock, flags);

		if (i < num_video) {
			/* allocate and initialize video device instance */
			index = atomic_inc_return(&ntv2_dev->video_index) - 1;
			ntv2_vid = ntv2_video_open((struct ntv2_object*)ntv2_dev, "vid", index);

			/* configure video device */
			result = ntv2_video_configure(ntv2_vid,
										  ntv2_dev->features,
										  ntv2_chn,
										  ntv2_dev->inp_mon,
										  ntv2_dev->dma_engine);
			if (result != 0) {
				ntv2_video_close(ntv2_vid);
				return result;
			}

			/* add to the video list */
			spin_lock_irqsave(&ntv2_dev->video_lock, flags);
			list_add_tail(&ntv2_vid->list, &ntv2_dev->video_list);
			spin_unlock_irqrestore(&ntv2_dev->video_lock, flags);
		}

		if (i < num_audio) {
			/* allocate and initialize audio device instance */
			index = atomic_inc_return(&ntv2_dev->audio_index) - 1;
			ntv2_aud = ntv2_audio_open((struct ntv2_object*)ntv2_dev, "aud", index);

			/* configure audio device */
			result = ntv2_audio_configure(ntv2_aud,
										  ntv2_dev->features,
										  ntv2_dev->snd_card,
										  ntv2_chn,
										  ntv2_dev->inp_mon,
										  ntv2_dev->dma_engine);
			if (result != 0) {
				ntv2_audio_close(ntv2_aud);
				return result;
			}

			/* add to the audio list */
			spin_lock_irqsave(&ntv2_dev->audio_lock, flags);
			list_add_tail(&ntv2_aud->list, &ntv2_dev->audio_list);
			spin_unlock_irqrestore(&ntv2_dev->audio_lock, flags);
		}
	}

	/* configure the audio mixer */
	ntv2_mixops_configure(ntv2_dev);

	/* register the pcm devices */
	if (num_audio > 0) {
		result = snd_card_register(ntv2_dev->snd_card);
		if (result < 0) {
			NTV2_MSG_DEVICE_ERROR("%s: *error* snd_card_register failed code %d\n",
								  ntv2_dev->name, result);
		}
	}

	for (i = 0; i < num_serial; i++) {
		/* allocate and initialize serial device instance */
		index = atomic_inc_return(&ntv2_dev->serial_index) - 1;
		ntv2_ser = ntv2_serial_open((struct ntv2_object*)ntv2_dev, "ser", index);

		/* configure serial device */
		result = ntv2_serial_configure(ntv2_ser,
									   ntv2_dev->features,
									   ntv2_dev->vid_reg);
		if (result != 0) {
			ntv2_serial_close(ntv2_ser);
			return result;
		}

		/* add to the serial list */
		spin_lock_irqsave(&ntv2_dev->serial_lock, flags);
		list_add_tail(&ntv2_ser->list, &ntv2_dev->serial_list);
		spin_unlock_irqrestore(&ntv2_dev->serial_lock, flags);
	}

	/* enable interrupts */
	ntv2_device_irq_enable(ntv2_dev);
#endif
	return 0;
}

void ntv2_device_suspend(struct ntv2_device *ntv2_dev)
{
	struct list_head *ptr;
	struct list_head *next;
	struct ntv2_audio *aud;
	struct ntv2_video *vid;
	struct ntv2_channel *chn;

	if (ntv2_dev == NULL)
		return;

	/* suspend audio */
	if (ntv2_dev->snd_card != NULL)
		snd_power_change_state(ntv2_dev->snd_card, SNDRV_CTL_POWER_D3hot);

	/* disable all streaming */
	list_for_each_safe(ptr, next, &ntv2_dev->audio_list) {
		aud = list_entry(ptr, struct ntv2_audio, list);
		if ((aud != NULL) && (aud->pcm != NULL)) {
			snd_pcm_suspend_all(aud->pcm);
		}
		ntv2_audio_disable_all(aud);
	}

	list_for_each_safe(ptr, next, &ntv2_dev->video_list) {
		vid = list_entry(ptr, struct ntv2_video, list);
		ntv2_video_disable(vid);
	}

	list_for_each_safe(ptr, next, &ntv2_dev->channel_list) {
		chn = list_entry(ptr, struct ntv2_channel, list);
		ntv2_channel_disable_all(chn);
	}

	/* disable hardware stuff */
	ntv2_chrdev_disable(ntv2_dev->chr_dev);
	ntv2_input_disable(ntv2_dev->inp_mon);
	ntv2_nwldma_disable(ntv2_dev->dma_engine);
	ntv2_device_irq_disable(ntv2_dev);
	ntv2_register_disable(ntv2_dev->vid_reg);
	ntv2_register_disable(ntv2_dev->pci_reg);
}

void ntv2_device_resume(struct ntv2_device *ntv2_dev)
{
	if (ntv2_dev == NULL)
		return;

	/* enable hardware */
	ntv2_register_enable(ntv2_dev->pci_reg);
	ntv2_register_enable(ntv2_dev->vid_reg);
	ntv2_device_irq_enable(ntv2_dev);
	ntv2_nwldma_enable(ntv2_dev->dma_engine);
	ntv2_input_enable(ntv2_dev->inp_mon);
	ntv2_chrdev_enable(ntv2_dev->chr_dev);

	/* resume audio */
	if (ntv2_dev->snd_card != NULL)
		snd_power_change_state(ntv2_dev->snd_card, SNDRV_CTL_POWER_D0);
}

/*
 * Configure pci resources
 */
static int ntv2_device_pci_configure(struct ntv2_device *ntv2_dev, struct pci_dev *pdev)
{
	enum ntv2_pci_type pci_type = ntv2_pci_type_unknown;
	bool pci_region = false;
	bool vid_region = false;
	int pci_bar = 0;
	int vid_bar = 0;
	int result = -EPERM;

	NTV2_MSG_DEVICE_INFO("%s: configure pci resources\n", ntv2_dev->name);

	ntv2_dev->pci_dev = pdev;

	/* determine barness */
	switch (pdev->device)
	{
	case NTV2_PCI_DEVID_KONA4:
	case NTV2_PCI_DEVID_CORVID88:
	case NTV2_PCI_DEVID_CORVID44:
	case NTV2_PCI_DEVID_CORVIDHDBT:
		pci_type = ntv2_pci_type_nwl;
		pci_region = true;
		vid_region = true;
		pci_bar = 0;
		vid_bar = 1;
		break;
	case NTV2_PCI_DEVID_KONAHDMI:
		pci_type = ntv2_pci_type_xlx;
		pci_region = true;
		vid_region = true;
		pci_bar = 1;
		vid_bar = 0;
		break;
	default:
		NTV2_MSG_ERROR("%s: *error* unrecognized pci device %04x\n", ntv2_dev->name, pdev->device);
		return -ENOMEM;
	}

	/* request nwl pci bar */
	if (pci_type == ntv2_pci_type_nwl) {
		result = pci_request_region(pdev, pci_bar, ntv2_dev->name);
		if (result < 0)	{
			NTV2_MSG_DEVICE_ERROR("%s: request nwl bar failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		ntv2_dev->pci_type = pci_type;
		ntv2_dev->pci_region = true;
		ntv2_dev->pci_bar = pci_bar;
	}

	/* request video pci bar */
	if (vid_region) {
		result = pci_request_region(pdev, vid_bar, ntv2_dev->name);
		if (result < 0)	{
			NTV2_MSG_DEVICE_ERROR("%s: request video bar failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		ntv2_dev->vid_region = true;
		ntv2_dev->vid_bar = vid_bar;
	}
	
	/* request xlx pci bar */
	if (pci_type == ntv2_pci_type_xlx) {
		result = pci_request_region(pdev, pci_bar, ntv2_dev->name);
		if (result < 0)	{
			NTV2_MSG_DEVICE_ERROR("%s: request xlx bar failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		ntv2_dev->pci_type = pci_type;
		ntv2_dev->pci_region = true;
		ntv2_dev->pci_bar = pci_bar;
	}

	/* map video bar */
	if (vid_region) {
		ntv2_dev->vid_base = ioremap_nocache(pci_resource_start(pdev, vid_bar),
											 pci_resource_len(pdev, vid_bar));
		if (ntv2_dev->vid_base == 0) {
			NTV2_MSG_ERROR("%s: *error* video ioremap_nocache failed\n", ntv2_dev->name);
			return -ENOMEM;
		}
		ntv2_dev->vid_size = pci_resource_len(pdev, vid_bar);

		NTV2_MSG_DEVICE_INFO("%s: map vid pci bar %d  phys 0x%08x  address 0x%px  size 0x%08x\n",
						 ntv2_dev->name,
						 vid_bar,
						 (u32)pci_resource_start(pdev, vid_bar),
						 ntv2_dev->vid_base, 
						 ntv2_dev->vid_size);
	}
	
	/* map dma bar */
	if (pci_region) {
		ntv2_dev->pci_base = ioremap_nocache(pci_resource_start(pdev, pci_bar),
											 pci_resource_len(pdev, pci_bar));
		if (ntv2_dev->pci_base == 0) {
			NTV2_MSG_ERROR("%s: *error* %s ioremap_nocache failed\n", ntv2_dev->name, ntv2_pci_name(pci_type));
			return -ENOMEM;
		}
		ntv2_dev->pci_size = (u32)pci_resource_len(pdev, pci_bar);

		NTV2_MSG_DEVICE_INFO("%s: map %s pci bar %d  phys 0x%08x  address 0x%px  size 0x%08x\n",
							 ntv2_dev->name,
							 ntv2_pci_name(pci_type),
							 pci_bar,
							 (u32)pci_resource_start(pdev, pci_bar),
							 ntv2_dev->pci_base, 
							 ntv2_dev->pci_size);
	}

	return 0;
}

/*
 * Release pci resources
 */
static void ntv2_device_pci_release(struct ntv2_device *ntv2_dev)
{
	struct pci_dev* pdev = ntv2_dev->pci_dev;

	NTV2_MSG_DEVICE_INFO("%s: release pci resources\n", ntv2_dev->name);

	if (ntv2_dev->pci_base != NULL) {
		NTV2_MSG_DEVICE_INFO("%s: unmap %s bar %d address 0x%px\n",
							 ntv2_dev->name, ntv2_pci_name(ntv2_dev->pci_type), 
							 ntv2_dev->pci_bar, ntv2_dev->pci_base);
		iounmap(ntv2_dev->pci_base);
		ntv2_dev->pci_base = NULL;
		ntv2_dev->pci_size = 0;
	}

	if (ntv2_dev->vid_base != NULL) {
		NTV2_MSG_DEVICE_INFO("%s: unmap vid bar %d address 0x%px\n",
							 ntv2_dev->name, ntv2_dev->vid_bar, ntv2_dev->vid_base);
		iounmap(ntv2_dev->vid_base);
		ntv2_dev->vid_base = NULL;
		ntv2_dev->vid_size = 0;
	}

	if (ntv2_dev->pci_type == ntv2_pci_type_xlx) {
		release_mem_region(pci_resource_start(pdev, ntv2_dev->pci_bar),
						   pci_resource_len(pdev, ntv2_dev->pci_bar));
		ntv2_dev->pci_type = ntv2_pci_type_unknown;
	}

	if (ntv2_dev->vid_region) {
		release_mem_region(pci_resource_start(pdev, ntv2_dev->vid_bar),
						   pci_resource_len(pdev, ntv2_dev->vid_bar));
		ntv2_dev->vid_region = false;
	}

	if (ntv2_dev->pci_type == ntv2_pci_type_nwl) {
		release_mem_region(pci_resource_start(pdev, ntv2_dev->pci_bar),
						   pci_resource_len(pdev, ntv2_dev->pci_bar));
		ntv2_dev->pci_type = ntv2_pci_type_unknown;
	}

	return;
}

/*
 * Configure dma resources
 */
static int ntv2_device_dma_configure(struct ntv2_device *ntv2_dev)
{
	struct pci_dev* pdev = ntv2_dev->pci_dev;
	int result = 0;

	NTV2_MSG_DEVICE_INFO("%s: configure dma resources\n", ntv2_dev->name);

	/* we are a pci dma master */
	pci_set_master(pdev);

	/* set dma mask to 64 bits with fallback to 32 bits*/
	result = pci_set_dma_mask(pdev, DMA_BIT_MASK(64));
	if (result == 0) {
		result = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64));
		if (result < 0) {
			NTV2_MSG_DEVICE_ERROR("%s: set consistent dma mask to 64 bit failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		NTV2_MSG_DEVICE_INFO("%s: pci dma mask = 64 bit\n", ntv2_dev->name);
	}
	else {
		result = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (result < 0) {
			NTV2_MSG_DEVICE_ERROR("%s: set dma mask to 32 bit failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		result = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
		if (result < 0) {
			NTV2_MSG_DEVICE_ERROR("%s: set consistent dma mask to 32 bit failed code %d\n",
								  ntv2_dev->name, result);
			return result;
		}
		NTV2_MSG_DEVICE_INFO("%s: pci dma mask = 32 bit\n", ntv2_dev->name);
	}

	return 0;
}

/*
 * Release dma resources
 */
static void ntv2_device_dma_release(struct ntv2_device *ntv2_dev)
{
	NTV2_MSG_DEVICE_INFO("%s: release dma resources\n", ntv2_dev->name);

	return;
}

/*
 * Configure irq resources
 */
static int ntv2_device_irq_configure(struct ntv2_device *ntv2_dev)
{
	struct pci_dev* pdev = ntv2_dev->pci_dev;
	int result = 0;

	NTV2_MSG_DEVICE_INFO("%s: configure irq resources\n", ntv2_dev->name);

	/* we are msi */
	result = pci_enable_msi(pdev);
	if (result != 0)	{
		NTV2_MSG_DEVICE_ERROR("%s: *error* can not enable msi irq\n", ntv2_dev->name);
		return -EPERM;
	}
	ntv2_dev->irq_msi = true;

	/* connect interrupt routine to irq */
	result = request_irq(pdev->irq, ntv2_device_interrupt, 0, ntv2_dev->name, (void*)ntv2_dev);
	if (result != 0)	{
		NTV2_MSG_DEVICE_ERROR("%s: *error* request irq %d failed\n",
							  ntv2_dev->name, pdev->irq);
		return -EPERM;
	}
	ntv2_dev->irq_handler = ntv2_device_interrupt;

	NTV2_MSG_DEVICE_INFO("%s: pci msi irq %d\n", ntv2_dev->name, pdev->irq);

	return 0;
}

/*
 * Release irq resources
 */
static void ntv2_device_irq_release(struct ntv2_device *ntv2_dev)
{
	struct pci_dev* pdev = ntv2_dev->pci_dev;

	NTV2_MSG_DEVICE_INFO("%s: release irq (%d) resources\n",
						 ntv2_dev->name, pdev->irq);

	if (ntv2_dev->irq_handler != NULL) {
		free_irq(pdev->irq, (void*)ntv2_dev);
		ntv2_dev->irq_handler = NULL;
	}

	if (ntv2_dev->irq_msi) {
		pci_disable_msi(pdev);
		ntv2_dev->irq_msi = false;
	}

	return;
}

static void ntv2_device_irq_enable(struct ntv2_device *ntv2_dev)
{
	if ((ntv2_dev == NULL) || (ntv2_dev->pci_reg == NULL))
		return;

	NTV2_MSG_DEVICE_INFO("%s: enable interrupts\n", ntv2_dev->name);

	ntv2_reg_write(ntv2_dev->pci_reg,
				   ntv2_nwldma_reg_common_control_status, 0,
				   NTV2_FLD_MASK(ntv2_nwldma_fld_dma_interrupt_enable) |
				   NTV2_FLD_MASK(ntv2_nwldma_fld_user_interrupt_enable));

	return;
}

static void ntv2_device_irq_disable(struct ntv2_device *ntv2_dev)
{
	if ((ntv2_dev == NULL) || (ntv2_dev->pci_reg == NULL))
		return;

	NTV2_MSG_DEVICE_INFO("%s: disable interrupts\n", ntv2_dev->name);

	ntv2_reg_write(ntv2_dev->pci_reg,
				   ntv2_nwldma_reg_common_control_status, 0,
				   0);

	return;
}

static irqreturn_t ntv2_device_interrupt(int irq, void* dev_id)
{
	struct ntv2_device *ntv2_dev = (struct ntv2_device*)dev_id;
	struct ntv2_interrupt_status irq_status;
	struct list_head *ptr;
	struct ntv2_channel *chn;
	struct ntv2_serial *ser;
	int result = IRQ_NONE;
	int res;

	if (ntv2_dev == NULL) 
		return IRQ_NONE;

	/* timestamp the interrupt */
#ifdef NTV2_USE_VB2_BUFFER_TIMESTAMP
	irq_status.v4l2_time = ktime_get_ns();
#else
	v4l2_get_timestamp(&irq_status.v4l2_time);
#endif
	/* read the video interrupt status registers */
	ntv2_video_read_interrupt_status(ntv2_dev->vid_reg, &irq_status);

	/* process dma interrupt */
	res = ntv2_nwldma_interrupt(ntv2_dev->dma_engine);
	if (res == IRQ_HANDLED)
		result = IRQ_HANDLED;

	/* process video interrupts */
	list_for_each(ptr, &ntv2_dev->channel_list) {
		chn = list_entry(ptr, struct ntv2_channel, list);
		res = ntv2_channel_interrupt(chn, &irq_status);
		if (res == IRQ_HANDLED)
			result = IRQ_HANDLED;
	}

	/* process uart interrupts */
	list_for_each(ptr, &ntv2_dev->serial_list) {
		ser = list_entry(ptr, struct ntv2_serial, list);
		res = ntv2_serial_interrupt(ser, &irq_status);
		if (res == IRQ_HANDLED)
			result = IRQ_HANDLED;
	}

	return result;
}

static void ntv2_device_init_hardware(struct ntv2_device *ntv2_dev)
{
	u32 val;
	int num;
	int i;

	NTV2_MSG_DEVICE_INFO("%s: initialize ntv2 hardware\n", ntv2_dev->name);

	/* disable pci interrupts */
	ntv2_reg_write(ntv2_dev->pci_reg, ntv2_nwldma_reg_common_control_status, 0, 0);

	/* disable nwl interrupts */
	num = NTV2_REG_COUNT(ntv2_nwldma_reg_capabilities);
	for (i = 0; i < num; i++) {
		val = ntv2_reg_read(ntv2_dev->pci_reg, ntv2_nwldma_reg_capabilities, i);
		if ((val & NTV2_FLD_MASK(ntv2_nwldma_fld_present)) != 0) {
			ntv2_reg_write(ntv2_dev->pci_reg, ntv2_nwldma_reg_engine_control_status, i, 0);
		}
	}

	/* disable fpga interrupts */
	ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_interrupt_control, 0, 0);
	ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_interrupt_control2, 0, 0);
	ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_interrupt_control3, 0, 0);

	/* disable fpga frame writes */
	num = NTV2_REG_COUNT(ntv2_kona_reg_frame_control);
	for (i = 0; i < num; i++) {
		ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_frame_control, i, 0);
	}

	/* initialize frame buffer dma offset */
	ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_frame_buffer_offset, 0, 0);

	/* disable sdi outputs */
	ntv2_reg_write(ntv2_dev->vid_reg, ntv2_kona_reg_sdi_transmit_control, 0, 0);
}
