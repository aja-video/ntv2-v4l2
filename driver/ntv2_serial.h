/*
 * NTV2 serial device interface
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

#ifndef NTV2_SERIAL_H
#define NTV2_SERIAL_H

#include "ntv2_common.h"

struct ntv2_features;

struct ntv2_serial {
	int								index;
	char							name[NTV2_STRING_SIZE];
	struct list_head				list;
	struct ntv2_device				*ntv2_dev;

	struct ntv2_features 			*features;
	struct ntv2_register			*vid_reg;

	bool							uart_enable;
	spinlock_t 						state_lock;
	spinlock_t 						int_lock;

	struct uart_driver 				ntv2_uart;
};

struct ntv2_serial *ntv2_serial_open(struct ntv2_object *ntv2_obj,
								   const char *name, int index);
void ntv2_serial_close(struct ntv2_serial *ntv2_ser);

int ntv2_serial_configure(struct ntv2_serial *ntv2_ser,
						  struct ntv2_features *features,
						  struct ntv2_register *vid_reg);

int ntv2_serial_enable(struct ntv2_serial *ntv2_ser);
int ntv2_serial_disable(struct ntv2_serial *ntv2_ser);

int ntv2_serial_interrupt(struct ntv2_serial *ntv2_ser,
						  struct ntv2_interrupt_status* irq_status);

#endif
