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

#include "ntv2_timecode.h"


#define NTV2_TIMECODE_DROP_MASK_LOW 		0x00000400
#define NTV2_TIMECODE_COLOR_MASK_LOW 		0x00000800
#define NTV2_TIMECODE_FIELD_MASK_LOW 		0x08000000
#define NTV2_TIMECODE_BGF0_MASK_HIGH		0x00000800
#define NTV2_TIMECODE_BGF1_MASK_HIGH		0x04000000
#define NTV2_TIMECODE_BGF2_MASK_HIGH		0x08000000

#define NTV2_TIMECODE_25_COLOR_MASK_LOW 	0x00000800
#define NTV2_TIMECODE_25_FIELD_MASK_HIGH 	0x08000000
#define NTV2_TIMECODE_25_BGF0_MASK_LOW		0x08000000
#define NTV2_TIMECODE_25_BGF1_MASK_HIGH		0x04000000
#define NTV2_TIMECODE_25_BGF2_MASK_HIGH		0x00000800


bool ntv2_timecode_compare_packed(struct ntv2_timecode_packed *tca,
								  struct ntv2_timecode_packed *tcb,
								  bool time_bits, bool user_bits)
{
	u32 mask = 0;

	if ((tca == NULL) || (tcb == NULL))
		return false;
	
	if (time_bits) mask |= 0x0f0f0f0f;
	if (user_bits) mask |= 0xf0f0f0f0;

	if (((tca->timecode_low & mask) == (tcb->timecode_low & mask)) &&
		((tca->timecode_high & mask) == (tcb->timecode_high & mask)))
		return true;

	return false;
}
	
bool ntv2_timecode_compare_data(struct ntv2_timecode_data *tca,
								struct ntv2_timecode_data *tcb,
								bool time_bits, bool user_bits, bool field)
{
	if ((tca == NULL) || (tcb == NULL))
		return false;

	if (time_bits &&
		((tca->frames != tcb->frames) ||
		 (tca->seconds != tcb->seconds) ||
		 (tca->minutes != tcb->minutes) ||
		 (tca->hours != tcb->hours) ||
		 (tca->drop_frame != tcb->drop_frame)))
		return false;

	if (user_bits &&	(tca->user_bits != tcb->user_bits))
		return false;

	if (field && (tca->field != tcb->field))
		return false;

	return true;
}

void ntv2_timecode_copy_data(struct ntv2_timecode_data *dst,
							 struct ntv2_timecode_data *src,
							 bool time_bits, bool user_bits, bool field)
{
	if ((dst == NULL) || (src == NULL))
		return;

	if (time_bits) {
		dst->frames = src->frames;
		dst->seconds = src->seconds;
		dst->minutes = src->minutes;
		dst->hours = src->hours;
		dst->drop_frame = src->drop_frame;
	}
	
	if (user_bits) {
		dst->user_bits = src->user_bits;
	}
	
	if (field) {
		dst->field = src->field;
	}

	return;
}

void ntv2_timecode_unpack(struct ntv2_timecode_data *data,
						  struct ntv2_timecode_packed *pack, 
						  u32 fps, bool field)
{
	if ((data == NULL) || (pack == NULL))
		return;

	data->frames = (pack->timecode_low & 0x0000000f);				
	data->user_bits = (pack->timecode_low & 0x000000f0) >> 4;		/* >>4<<0 */
	data->frames += ((pack->timecode_low & 0x00000300) >> 8) * 10;
	data->user_bits |= (pack->timecode_low & 0x0000f000) >> 8; 		/* >>12<<4 */
	data->seconds = (pack->timecode_low & 0x000f0000) >> 16;
	data->user_bits |= (pack->timecode_low & 0x00f00000) >> 12;		/* >>20<<8 */
	data->seconds += ((pack->timecode_low & 0x07000000) >> 24) * 10;
	data->user_bits |= (pack->timecode_low & 0xf0000000) >> 16;		/* >>28<<12 */

	data->minutes = (pack->timecode_high & 0x0000000f) >> 0;
	data->user_bits |= (pack->timecode_high & 0x000000f0) << 12;	/* >>4<<16 */
	data->minutes += ((pack->timecode_high & 0x00000700) >> 8) * 10;
	data->user_bits |= (pack->timecode_high & 0x0000f000) << 8;		/* >>12<<20 */
	data->hours = (pack->timecode_high & 0x000f0000) >> 16;
	data->user_bits |= (pack->timecode_high & 0x00f00000) << 4;		/* >>20<<24 */
	data->hours += ((pack->timecode_high & 0x03000000) >> 24) * 10;
	data->user_bits |= (pack->timecode_high & 0xf0000000);			/* >>28<<28 */

	data->drop_frame = false;
	data->field = false;

	if (fps == 30) {
		data->drop_frame = (pack->timecode_low & NTV2_TIMECODE_DROP_MASK_LOW) != 0;
	}
	if (field) {
		if (fps == 25) {
			data->field = (pack->timecode_high & NTV2_TIMECODE_25_FIELD_MASK_HIGH) != 0;
		} else {
			data->field = (pack->timecode_low & NTV2_TIMECODE_FIELD_MASK_LOW) != 0;
		}
	}

	return;
}

void ntv2_timecode_pack(struct ntv2_timecode_data *data,
						struct ntv2_timecode_packed *pack, 
						u32 fps, bool field)
{
	if ((data == NULL) || (pack == NULL))
		return;

	pack->timecode_low = ((data->frames % 10) & 0xf) << 0;
	pack->timecode_low |= (data->user_bits & 0x0000000f) << 4;		/* >>0<<4 */
	pack->timecode_low |= ((data->frames/10) & 0x3) << 8;
	pack->timecode_low |= (data->user_bits & 0x000000f0) << 8;		/* >>4<<12 */
	pack->timecode_low |= ((data->seconds % 10) & 0xf) << 16;
	pack->timecode_low |= (data->user_bits & 0x00000f00) << 12;		/* >>8<<20 */
	pack->timecode_low |= ((data->seconds/10) & 7) << 24;
	pack->timecode_low |= (data->user_bits & 0x0000f000) << 16;		/* >>12<<28 */

	pack->timecode_high = ((data->minutes % 10) & 0xf) << 0;
	pack->timecode_high |= (data->user_bits & 0x000f0000)>>12;		/* >>16<<4 */
	pack->timecode_high |= ((data->minutes/10) & 0x7) << 8;
	pack->timecode_high |= (data->user_bits & 0x00f00000)>>8;		/* >>20<<12 */
	pack->timecode_high |= ((data->hours % 10) & 0xf) << 16;
	pack->timecode_high |= (data->user_bits & 0x0f000000)>>4;		/* >>24<<20 */
	pack->timecode_high |= ((data->hours/10) & 3) << 24;
	pack->timecode_high |= (data->user_bits & 0xf0000000);			/* >>28<<28 */

	if (fps == 30)	{
		if (data->drop_frame)
			pack->timecode_low |= NTV2_TIMECODE_DROP_MASK_LOW;
	}
	if (field && data->field) {
		if (fps == 25)	{
			pack->timecode_high |= NTV2_TIMECODE_25_FIELD_MASK_HIGH;
		} else {
			pack->timecode_low |= NTV2_TIMECODE_FIELD_MASK_LOW;
		}
	}

	return;
}

void ntv2_timecode_offset(struct ntv2_timecode_data *data, u32 fps, bool field, int count)
{
	int i;
	bool dec = false;

	if ((data == NULL) || (count == 0))
		return;

	if (count < 0) {
		dec = true;
		count = -count;
	}
	
	for (i = 0; i < count; i++) {
		if (dec) {
			ntv2_timecode_decrement(data, fps, field);
		} else {
			ntv2_timecode_increment(data, fps, field);
		}
	}

	return;
}

void ntv2_timecode_increment(struct ntv2_timecode_data *data, u32 fps, bool field)
{
	int hours;
	int minutes;
	int seconds;
	int frames;

	if (data == NULL)
		return;

	hours = (int)data->hours;
	minutes = (int)data->minutes;
	seconds = (int)data->seconds;
	frames = (int)data->frames;

	if (fps == 0)
		return;
	
	if (field) {
		if (!data->field) {
			data->field = true;
			return;
		}
		data->field = false;
	}		

	frames++;
	if (frames == (int)fps) { frames = 0; seconds++; }
	if (seconds == 60) { seconds = 0; minutes++; }
	if (minutes == 60) { minutes = 0; hours++; }
	if (data->drop_frame && (fps == 30) &&
		((minutes % 10) != 0) && 
		(seconds == 0) && 
		(frames == 0)) frames = 2;
	if (hours == 24) hours = 0;

	data->hours = (u32)hours;
	data->minutes = (u32)minutes;
	data->seconds = (u32)seconds;
	data->frames = (u32)frames;

	return;
}

void ntv2_timecode_decrement(struct ntv2_timecode_data *data, u32 fps, bool field)
{
	int hours;
	int minutes;
	int seconds;
	int frames;

	if (data == NULL)
		return;

	hours = (int)data->hours;
	minutes = (int)data->minutes;
	seconds = (int)data->seconds;
	frames = (int)data->frames;

	if (fps == 0)
		return;

	if (field) {
		if (data->field) {
			data->field = false;
			return;
		}
		data->field = true;
	}		

	if (data->drop_frame && (fps == 30) &&
		((minutes % 10) != 0) && 
		(seconds == 0) && 
		(frames == 2)) frames = 0;
	frames--;
	if (frames == -1) { frames = (int)fps - 1; seconds--; }
	if (seconds == -1) { seconds = 59; minutes--; }
	if (minutes == -1) { minutes = 59; hours--; }
	if (hours == -1) hours = 23;

	data->hours = (u32)hours;
	data->minutes = (u32)minutes;
	data->seconds = (u32)seconds;
	data->frames = (u32)frames;

	return;
}

