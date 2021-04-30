/*
 * NTV2 fixed point math
 *
 * Copyright 2021 AJA Video Systems Inc. All rights reserved.
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

#ifndef NTV2_FIXEDPOINT_H
#define NTV2_FIXEDPOINT_H

#include "ntv2_common.h"

typedef s32 ntv2_fp16;

static const ntv2_fp16 ntv2_fp16_max = 0x7FFFFFFF;
static const ntv2_fp16 ntv2_fp16_min = 0x80000000;
static const ntv2_fp16 ntv2_fp16_max_positive = 0x7FFeFFFF;
static const ntv2_fp16 ntv2_fp16_overflow = 0x80000000;
static const ntv2_fp16 ntv2_fp16_one = 0x00010000;
static const ntv2_fp16 ntv2_fp16_e   = 178145;

ntv2_fp16 ntv2_fp16_init(s32 val, s32 frac);
s16 ntv2_fp16_round(ntv2_fp16 val);
s16 ntv2_fp16_trunc(ntv2_fp16 val);
s16 ntv2_fp16_frac(ntv2_fp16 val);
ntv2_fp16 ntv2_fp16_mul(ntv2_fp16 x, ntv2_fp16 y);
ntv2_fp16 ntv2_fp16_div(ntv2_fp16 x, ntv2_fp16 y);
ntv2_fp16 ntv2_fp16_sqrt(ntv2_fp16 val);
ntv2_fp16 ntv2_fp16_exp(ntv2_fp16 val);
ntv2_fp16 ntv2_fp16_ln(ntv2_fp16 val);
ntv2_fp16 ntv2_fp16_pow(ntv2_fp16 x, ntv2_fp16 y);

#endif
