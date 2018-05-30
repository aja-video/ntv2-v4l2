/*
 * NTV2 timecode utility
 *
 * Copyright 2018 AJA Video Systems Inc. All rights reserved.
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

#ifndef NTV2_TIMECODE_H
#define NTV2_TIMECODE_H

#include "ntv2_common.h"

struct ntv2_timecode_packed
{
	u32 	timecode_low;
	u32 	timecode_high;
};

struct ntv2_timecode_data
{
	u32		hours;
	u32		minutes;
	u32		seconds;
	u32		frames;
	u32		user_bits;
	bool	drop_frame;
	bool	field;
};

bool ntv2_timecode_compare_packed(struct ntv2_timecode_packed *tca,
								  struct ntv2_timecode_packed *tcb,
								  bool time_bits, bool user_bits);
bool ntv2_timecode_compare_data(struct ntv2_timecode_data *tca,
								struct ntv2_timecode_data *tcb,
								bool time_bits, bool user_bits, bool field);

void ntv2_timecode_copy_data(struct ntv2_timecode_data *dst,
							 struct ntv2_timecode_data *src,
							 bool time_bits, bool user_bits, bool field);

void ntv2_timecode_unpack(struct ntv2_timecode_data *data,
						  struct ntv2_timecode_packed *pack, 
						  u32 fps, bool field);
void ntv2_timecode_pack(struct ntv2_timecode_data *data,
						struct ntv2_timecode_packed *pack, 
						u32 fps, bool field);

void ntv2_timecode_offset(struct ntv2_timecode_data *data, u32 fps, bool field, int count);
void ntv2_timecode_increment(struct ntv2_timecode_data *data, u32 fps, bool field);
void ntv2_timecode_decrement(struct ntv2_timecode_data *data, u32 fps, bool field);

#endif
