/*
 * NTV2 video stream channel ops
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

#ifndef NTV2_VIDEOOPS_H
#define NTV2_VIDEOOPS_H

#include "ntv2_common.h"

struct ntv2_channel_stream;

int ntv2_videoops_setup_capture(struct ntv2_channel_stream *stream);
int ntv2_videoops_release_capture(struct ntv2_channel_stream *stream);
int ntv2_videoops_update_mode(struct ntv2_channel_stream *stream);
int ntv2_videoops_update_format(struct ntv2_channel_stream *stream);
int ntv2_videoops_update_timing(struct ntv2_channel_stream *stream);
int ntv2_videoops_update_route(struct ntv2_channel_stream *stream);
int ntv2_videoops_update_frame(struct ntv2_channel_stream *stream);
int ntv2_videoops_interrupt_capture(struct ntv2_channel_stream *stream);

#endif
