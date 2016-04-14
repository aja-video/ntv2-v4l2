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

#ifndef NTV2_PCMOPS_H
#define NTV2_PCMOPS_H

#include "ntv2_common.h"

int ntv2_pcmops_configure(struct ntv2_pcm_stream *stream);

void ntv2_pcmops_tstamp(struct ntv2_pcm_stream *stream);

void ntv2_pcmops_copy_audio(struct ntv2_pcm_stream *stream,
							u8 *address,
							u32 size,
							u32 num_channels,
							u32 sample_size);

#endif
