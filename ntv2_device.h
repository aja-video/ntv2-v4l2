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

#ifndef NTV2_DEVICE_H
#define NTV2_DEVICE_H

#include "ntv2_common.h"

struct ntv2_device *ntv2_device_open(struct ntv2_module *ntv2_mod,
									 const char *name, int index);
void ntv2_device_close(struct ntv2_device *ntv2_dev);

int ntv2_device_configure(struct ntv2_device *ntv2_dev, struct pci_dev *pdev);

void ntv2_device_suspend(struct ntv2_device *ntv2_dev);
void ntv2_device_resume(struct ntv2_device *ntv2_dev);

#endif
