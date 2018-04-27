/*
 * NTV2 v4l2 driver common header
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

#ifndef NTV2_COMMON_H
#define NTV2_COMMON_H

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>

#define GSPCA_DEBUG

#include <media/v4l2-device.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-event.h>
#include <media/videobuf2-core.h>
#include <sound/core.h>
#include <sound/initval.h>
#include <sound/pcm.h>
#include <sound/control.h>

/* build pararameters (approximate ubuntu kernel versions) */
/* 3.10.0 does build */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))
#define NTV2_USE_VB2_VOID_FINISH			/* 3.15.0 required */
#define NTV2_USE_VB2_TIMESTAMP_FLAGS		/* 3.15.0 required */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0))
#define NTV2_USE_VB2_VOID_STREAMING			/* 3.16.0 required */
#define NTV2_USE_SND_CARD_NEW				/* 3.16.0 required */
#define NTV2_ZERO_ENUM_TIMINGS_PAD			/* 3.16.0 optional */
#define NTV2_USE_V4L2_EVENT					/* 3.16.0 optional */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0))
#define NTV2_USE_V4L2_FH					/* 3.17.0 required */
#define NTV2_USE_TTY_GROUP					/* 3.17.0 required */
#define NTV2_RGB_PIXEL_FORMATS				/* 3.17.0 required */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
#define NTV2_USE_VB2_V4L2_BUFFER			/* 4.4.0 required */
#define NTV2_USE_QUEUE_SETUP_PARG			/* 4.4.0 required */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0))
#define NTV2_USE_VB2_BUFFER_TIMESTAMP		/* 4.5.0 required */
#define NTV2_USE_QUEUE_SETUP_NO_FORMAT		/* 4.5.0 required */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0))
#define NTV2_USE_QUEUE_SETUP_DEVICE			/* 4.8.0 required */
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0))
#define NTV2_USE_TIMER_SETUP				/* 4.15.0 required */
#endif
/* 4.14.0 does build */

#ifdef NTV2_USE_VB2_V4L2_BUFFER
#include <media/videobuf2-v4l2.h>
#endif

/* 
   use dma sg for slight improvement in dma performance
   but does not work in all kernels
*/
//#define NTV2_USE_VB2_DMA_SG

#include "ntv2_params.h"

#endif
