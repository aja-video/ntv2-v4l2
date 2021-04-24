/*
 * NTV2 device parameters
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

#ifndef NTV2_PARAMS_H
#define NTV2_PARAMS_H

#define NTV2_MODULE_NAME 			KBUILD_MODNAME
#define NTV2_DRV_VERSION			"1.2.2"

#define NTV2_PCI_VENID				0xf1d0
#define NTV2_PCI_DEVID_KONA4		0xeb0b
#define NTV2_PCI_DEVID_CORVID88		0xeb0d
#define NTV2_PCI_DEVID_CORVID44		0xeb0e
#define NTV2_PCI_DEVID_CORVIDHBR	0xeb18
#define NTV2_PCI_DEVID_KONAHDMI		0xeb24
#define NTV2_PCI_DEVID_KONA1		0xeb23

#define NTV2_DEVICE_ID_KONA4		0x10518400
#define NTV2_DEVICE_ID_CORVID88		0x10538200
#define NTV2_DEVICE_ID_CORVID44		0x10565400
#define NTV2_DEVICE_ID_CORVIDHBR	0x10668200
#define NTV2_DEVICE_ID_KONAHDMI		0x10767400
#define NTV2_DEVICE_ID_KONAHDMI2RX	0x10767401
#define NTV2_DEVICE_ID_KONA1		0x10756600

#define NTV2_PCI_BAR_NWL			0
#define NTV2_PCI_BAR_VIDEO			1

#define NTV2_STRING_SIZE			80

#define NTV2_MAX_CHANNELS			8
#define NTV2_MAX_STREAMS			4
#define NTV2_MAX_SDI_INPUTS			8
#define NTV2_MAX_HDMI_INPUTS		4
#define NTV2_MAX_VIDEO_FORMATS		32
#define NTV2_MAX_PIXEL_FORMATS		8
#define NTV2_MAX_INPUT_CONFIGS		8
#define NTV2_MAX_CSC_CONFIGS		4
#define NTV2_MAX_SOURCE_CONFIGS		8
#define NTV2_MAX_FRAME_RATES		16
#define NTV2_MAX_VIDEO_STANDARDS	16
#define NTV2_MAX_FRAME_GEOMETRIES	24
#define NTV2_MAX_INPUT_GEOMETRIES	8
#define NTV2_MAX_COLOR_SPACES		8
#define NTV2_MAX_COLOR_DEPTHS		8
#define NTV2_MAX_DMA_ENGINES		4

#define NTV2_MAX_UARTS				16
#define NTV2_TTY_NAME				"ttyNTV"

#define NTV2_MAX_CDEVS				16
#define NTV2_CDEV_NAME				"ntv2dev"

#ifdef NTV2_REG_CONST
#define NTV2_REG_ARGS(a1, a2, a3, a4, a5, a6, a7, a8, aN, ...) aN
#define NTV2_REG(reg, ...) \
	const u32 reg[] = { NTV2_REG_ARGS(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1), __VA_ARGS__ }
#define NTV2_FLD(field, size, shift) const u32 field = (((size) << 16) | (shift))
#else
#define NTV2_REG(reg, ...) extern const u32 reg[]
#define NTV2_FLD(field, size, shift) extern const u32 field
#endif

#define NTV2_CON(con, value) enum { con = (value) }

#define NTV2_REG_COUNT(reg) (reg[0])
#define NTV2_REG_NUM(reg, index) (((index) < NTV2_REG_COUNT(reg))? reg[(index) + 1] : 0)

#define NTV2_FLD_SIZE(field) ((field) >> 16)
#define NTV2_FLD_SHIFT(field) ((field) & 0xffff)
#define NTV2_FLD_MASK(field) ((((u32)0x1 << NTV2_FLD_SIZE(field)) - 1) << NTV2_FLD_SHIFT(field))
#define NTV2_FLD_GET(field, value) ((((u32)value) & NTV2_FLD_MASK(field)) >> NTV2_FLD_SHIFT(field))
#define NTV2_FLD_SET(field, value) ((((u32)value) << NTV2_FLD_SHIFT(field)) & NTV2_FLD_MASK(field))

#define NTV2_U64_HIGH(value) ((u32)((value) >> 32))
#define NTV2_U64_LOW(value) ((u32)(value))
#define NTV2_U64_BUILD(high, low) ((((u64)(high)) << 32) | ((u64)(low)))

#define NTV2_FOURCC_CHARS(fourcc) \
	*((char*)fourcc), *(((char*)fourcc)+1), *(((char*)fourcc)+2), *(((char*)fourcc)+3)

#define NTV2_DEBUG_REGISTER				32

/* debug print macros */
#define NTV2_DEBUG_INFO					0x00000001
#define NTV2_DEBUG_ERROR				0x00000002
#define NTV2_DEBUG_PCI_STATE			0x00000010
#define NTV2_DEBUG_CHANNEL_STATISTICS	0x00000020
#define NTV2_DEBUG_INPUT_STATE			0x00000100
#define NTV2_DEBUG_VIDEO_STATE			0x00000200
#define NTV2_DEBUG_AUDIO_STATE			0x00000400
#define NTV2_DEBUG_CHANNEL_STATE		0x00000800
#define NTV2_DEBUG_HDMIIN_STATE			0x00001000
#define NTV2_DEBUG_HDMIIN_DETECT		0x00002000
#define NTV2_DEBUG_SERIAL_STATE			0x00004000
#define NTV2_DEBUG_CHRDEV_STATE			0x00008000
#define NTV2_DEBUG_VIDEO_STREAM			0x00100000
#define NTV2_DEBUG_AUDIO_STREAM			0x00200000
#define NTV2_DEBUG_CHANNEL_STREAM		0x00400000
#define NTV2_DEBUG_SERIAL_STREAM		0x00800000
#define NTV2_DEBUG_REGISTER_READ		0x01000000
#define NTV2_DEBUG_REGISTER_WRITE		0x02000000
#define NTV2_DEBUG_KONAI2C_READ			0x04000000
#define NTV2_DEBUG_KONAI2C_WRITE		0x08000000
#define NTV2_DEBUG_DMA_STATE			0x10000000
#define NTV2_DEBUG_DMA_STREAM			0x20000000
#define NTV2_DEBUG_DMA_DESCRIPTOR		0x40000000
#define NTV2_DEBUG_DMA_STATISTICS		0x80000000

#define NTV2_DEBUG_ACTIVE(msg_mask) \
	((ntv2_module_info()->debug_mask & msg_mask) != 0)

#define NTV2_MSG_PRINT(msg_mask, string, ...) \
	if(NTV2_DEBUG_ACTIVE(msg_mask)) printk(string, __VA_ARGS__);

#define NTV2_MSG_INFO(string, ...)					NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_ERROR(string, ...)					NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_PCI_INFO(string, ...)				NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_PCI_ERROR(string, ...)				NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_PCI_STATE(string, ...)				NTV2_MSG_PRINT(NTV2_DEBUG_PCI_STATE, string, __VA_ARGS__)
#define NTV2_MSG_DEVICE_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_DEVICE_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_REGISTER_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_REGISTER_ERROR(string, ...)		NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_REGISTER_READ(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_REGISTER_READ, string, __VA_ARGS__)
#define NTV2_MSG_REGISTER_WRITE(string, ...)		NTV2_MSG_PRINT(NTV2_DEBUG_REGISTER_WRITE, string, __VA_ARGS__)
#define NTV2_MSG_DMA_INFO(string, ...)				NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_DMA_ERROR(string, ...)	   			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_DMA_STATE(string, ...)				NTV2_MSG_PRINT(NTV2_DEBUG_DMA_STATE, string, __VA_ARGS__)
#define NTV2_MSG_DMA_STREAM(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_DMA_STREAM, string, __VA_ARGS__)
#define NTV2_MSG_DMA_DESCRIPTOR(string, ...)		NTV2_MSG_PRINT(NTV2_DEBUG_DMA_DESCRIPTOR, string, __VA_ARGS__)
#define NTV2_MSG_DMA_STATISTICS(string, ...)		NTV2_MSG_PRINT(NTV2_DEBUG_DMA_STATISTICS, string, __VA_ARGS__)
#define NTV2_MSG_INPUT_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_INPUT_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_INPUT_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INPUT_STATE, string, __VA_ARGS__)
#define NTV2_MSG_VIDEO_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_VIDEO_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_VIDEO_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_VIDEO_STATE, string, __VA_ARGS__)
#define NTV2_MSG_VIDEO_STREAM(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_VIDEO_STREAM, string, __VA_ARGS__)
#define NTV2_MSG_AUDIO_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_AUDIO_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_AUDIO_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_AUDIO_STATE, string, __VA_ARGS__)
#define NTV2_MSG_AUDIO_STREAM(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_AUDIO_STREAM, string, __VA_ARGS__)
#define NTV2_MSG_CHANNEL_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_CHANNEL_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_CHANNEL_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_CHANNEL_STATE, string, __VA_ARGS__)
#define NTV2_MSG_CHANNEL_STREAM(string, ...)		NTV2_MSG_PRINT(NTV2_DEBUG_CHANNEL_STREAM, string, __VA_ARGS__)
#define NTV2_MSG_CHANNEL_STATISTICS(string, ...)	NTV2_MSG_PRINT(NTV2_DEBUG_CHANNEL_STATISTICS, string, __VA_ARGS__)
#define NTV2_MSG_KONAI2C_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_KONAI2C_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_KONAI2C_READ(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_KONAI2C_READ, string, __VA_ARGS__)
#define NTV2_MSG_KONAI2C_WRITE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_KONAI2C_WRITE, string, __VA_ARGS__)
#define NTV2_MSG_HDMIIN_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_HDMIIN_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_HDMIIN_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_HDMIIN_STATE, string, __VA_ARGS__)
#define NTV2_MSG_HDMIIN_DETECT(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_HDMIIN_DETECT, string, __VA_ARGS__)
#define NTV2_MSG_SERIAL_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_SERIAL_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_SERIAL_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_SERIAL_STATE, string, __VA_ARGS__)
#define NTV2_MSG_SERIAL_STREAM(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_SERIAL_STREAM, string, __VA_ARGS__)
#define NTV2_MSG_CHRDEV_INFO(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_INFO, string, __VA_ARGS__)
#define NTV2_MSG_CHRDEV_ERROR(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_ERROR, string, __VA_ARGS__)
#define NTV2_MSG_CHRDEV_STATE(string, ...)			NTV2_MSG_PRINT(NTV2_DEBUG_CHRDEV_STATE, string, __VA_ARGS__)

struct ntv2_register;
struct ntv2_pci;
struct ntv2_channel;
struct ntv2_video;
struct ntv2_audio;
struct ntv2_input;
struct ntv2_chrdev;

enum ntv2_pci_type {
	ntv2_pci_type_unknown,
	ntv2_pci_type_nwl,
	ntv2_pci_type_xlx,
	ntv2_pci_type_size
};

enum ntv2_stream_type {
	ntv2_stream_type_unknown,
	ntv2_stream_type_vidin,
	ntv2_stream_type_vidout,
	ntv2_stream_type_audin,
	ntv2_stream_type_audout,
	ntv2_stream_type_size
};

enum ntv2_input_type {
	ntv2_input_type_unknown,
	ntv2_input_type_auto,
	ntv2_input_type_sdi,
	ntv2_input_type_hdmi_adv,
	ntv2_input_type_hdmi_aja,
	ntv2_input_type_hdmi4k_aja,
	ntv2_input_type_aes,
	ntv2_input_type_analog,
	ntv2_input_type_size
};

enum ntv2_task_state {
	ntv2_task_state_unknown,
	ntv2_task_state_enable,
	ntv2_task_state_disable,
	ntv2_task_state_size
};

enum ntv2_transfer_mode {
	ntv2_transfer_mode_unknown,
	ntv2_transfer_mode_s2c,
	ntv2_transfer_mode_c2s,
	ntv2_transfer_mode_size
};

enum ntv2_edid_type {
	ntv2_edid_type_unknown,
	ntv2_edid_type_konahdmi_20,
	ntv2_edid_type_konahdmi_13,
	ntv2_edid_type_corvidhbr,
	ntv2_edid_type_io4k,
	ntv2_edid_type_io4kplus,
	ntv2_edid_type_size
};

#ifdef NTV2_USE_VB2_BUFFER_TIMESTAMP
typedef u64					v4l2_time_t;
#else
typedef struct timeval		v4l2_time_t;
#endif

struct ntv2_object {
	int						index;
	char					name[NTV2_STRING_SIZE];
	struct list_head		list;
	struct ntv2_device		*ntv2_dev;
};

struct ntv2_video_format {
	const char					*name;
	struct v4l2_dv_timings		v4l2_timings;
	u32							video_standard;
	u32							frame_geometry;
	u32							frame_rate;
	u32							frame_flags;
};

struct ntv2_pixel_format {
	const char					*name;
	u32							v4l2_pixel_format;
	u32							ntv2_pixel_format;
	u32							pixel_flags;
	u32							cadence_pixels;
	u32							cadence_bytes;
	u32							pitch_alignment;
};

struct ntv2_input_format {
	enum ntv2_input_type		type;
	u32							video_standard;
	u32							frame_rate;
	u32							frame_flags;
	u32							pixel_flags;
	int							reg_index;
	int							input_index;
	int							num_inputs;
	int							num_streams;
};

struct ntv2_source_format {
	enum ntv2_input_type		type;
	u32							audio_source;
	u32							audio_detect;
	int							reg_index;
	int							input_index;
	int							num_inputs;
};

struct ntv2_sdi_input_status {
	int							input_index;
	u32							input_geometry;
	u32							frame_rate;
	bool						progressive;
	u32							interface;
	u32							vpid_ds1;
	u32							vpid_ds2;
	u32							audio_detect;
};

struct ntv2_aes_input_status {
	int							input_index;
	u32							audio_detect;
};

struct ntv2_interrupt_status {
	u32							interrupt_status[2];
	v4l2_time_t					v4l2_time;
};

typedef void (*ntv2_transfer_callback)(unsigned long, int);

struct ntv2_transfer {
	enum ntv2_transfer_mode		mode;
	struct scatterlist 			*sg_list;
	u32 						sg_pages;
	u32 						sg_offset;
	u32 						card_address[2];
	u32 						card_size[2];
	ntv2_transfer_callback 		callback_func;
	unsigned long 				callback_data;
};

struct ntv2_device {
	int							index;
	char						name[NTV2_STRING_SIZE];
	struct list_head			list;
	struct ntv2_device			*ntv2_dev;

	struct ntv2_module			*ntv2_mod;
	bool						init;

	struct pci_dev				*pci_dev;
	enum ntv2_pci_type			pci_type;
	bool						pci_region;
	bool						vid_region;
	int							pci_bar;
	int							vid_bar;
	void __iomem 				*pci_base;
	void __iomem 				*vid_base;
	u32							pci_size;
	u32							vid_size;
	
	bool						irq_msi;
	irq_handler_t				irq_handler;
	struct ntv2_register		*pci_reg;
	struct ntv2_register		*vid_reg;
	struct ntv2_pci				*pci_dma;
	struct ntv2_features		*features;
	struct ntv2_input			*inp_mon;
	struct ntv2_chrdev			*chr_dev;
	struct snd_card 			*snd_card;

	struct list_head 			video_list;
	spinlock_t 					video_lock;
	atomic_t					video_index;

	struct list_head 			audio_list;
	spinlock_t 					audio_lock;
	atomic_t					audio_index;

	struct list_head 			channel_list;
	spinlock_t 					channel_lock;
	atomic_t					channel_index;

	struct list_head 			serial_list;
	spinlock_t 					serial_lock;
	atomic_t					serial_index;
};

struct ntv2_module {
	const char					*name;
	u32							debug_mask;
	const char					*version;
	bool						init;

	struct list_head			device_list;
	spinlock_t 					device_lock;
	atomic_t					device_index;

	struct uart_driver 			*uart_driver;
	u32							uart_max;
	atomic_t					uart_index;

	dev_t 						cdev_number;
	u32							cdev_max;
	const char					*cdev_name;
	atomic_t					cdev_index;
};

void ntv2_module_initialize(void);
void ntv2_module_release(void);

struct ntv2_module *ntv2_module_info(void);

s64 ntv2_system_time(void);

int ntv2_wait(int *event, int state, int timeout);

const char* ntv2_stream_name(enum ntv2_stream_type type);
const char* ntv2_pci_name(enum ntv2_pci_type type);

int ntv2_alloc_scatterlist(struct sg_table *sgt, u8* vm_buffer, u32 vm_size);
void ntv2_free_scatterlist(struct sg_table *sgt);

#endif
