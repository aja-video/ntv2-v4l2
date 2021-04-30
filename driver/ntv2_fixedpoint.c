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

/*
 * A bunch of this fixed point code was lifted from libfixmath[0], it is MIT licensed.
 *
 * The code that was used was changed so it would compile in the kernel.
 *
 * [0]: https://github.com/PetteriAimonen/libfixmath
 */

#include "ntv2_fixedpoint.h"

ntv2_fp16 ntv2_fp16_init(s32 val, s32 frac)
{
	ntv2_fp16 result;

	result = ((val&0xffff) << 16) + (frac&0xffff);
	return result;
}

s16 ntv2_fp16_round(ntv2_fp16 val)
{
	s16 result;

	if (val < 0) {
	  result = (s16)(-((-val+0x8000)>>16));
	} else {
	  result = (s16)((val + 0x8000)>>16);
	}
	return result;
}

s16 ntv2_fp16_trunc(ntv2_fp16 val)
{
	return (val>>16);
}

s16 ntv2_fp16_frac(ntv2_fp16 val)
{
  s16 result;

  if (val < 0) {
	result = -val&0xffff;
  } else {
	result = val&0xffff;
  }

  return result;
}

ntv2_fp16 ntv2_fp16_mul(ntv2_fp16 x, ntv2_fp16 y)
{
	s64 product = (s64)x * (s64)y;
	ntv2_fp16 result = (ntv2_fp16)(product >> 16);
	bool negative_x = (bool)(x & ntv2_fp16_overflow);
	bool negative_y = (bool)(y & ntv2_fp16_overflow);
	bool negative_result = negative_x != negative_y;

	if (!negative_result && (result == ntv2_fp16_max || result & ntv2_fp16_overflow))
	{
		/* over flow */
		result = ntv2_fp16_max_positive;
	}

	return result;
}

ntv2_fp16 ntv2_fp16_div(ntv2_fp16 x, ntv2_fp16 y)
{
	return ((s64)x * (1 << 16)) / y;
}

ntv2_fp16 ntv2_fp16_sqrt(ntv2_fp16 val)
{
	u8 neg = (val < 0);
	u32 num = (neg ? -val : val);
	u32 result = 0;
	u32 bit;
	u8  n;

	/* Many numbers will be less than 15, so
	 * this gives a good balance between time spent
	 * in if vs. time spent in the while loop
	 * when searching for the starting value
	 */
	if (num & 0xFFF00000)
		bit = (u32)1 << 30;
	else
		bit = (u32)1 << 18;

	while (bit > num) bit >>= 2;

	/* The main part is executed twice, in order to avoid
	 * using 64 bit values in computations.
	 */
	for (n = 0; n < 2; n++)
	{
		/* First we get the top 24 bits of the answer. */
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}

		if (n == 0)
		{
			/* Then process it again to get the lowest 8 bits. */
			if (num > 65535)
			{
				/* The remainder 'num' is too large to be shifted left
				 * by 16, so we have to add 1 to result manually and
				 * adjust 'num' accordingly.
				 * num = a - (result + 0.5)^2
				 *     = num + result^2 - (result + 0.5)^2
				 *     = num - result - 0.5
				 */
				num -= result;
				num = (num << 16) - 0x8000;
				result = (result << 16) + 0x8000;
			}
			else
			{
				num <<= 16;
				result <<= 16;
			}

			bit = 1 << 14;
		}
	}

	/* Finally, if next bit would have been 1, round the result upwards. */
	if (num > result)
	{
		result++;
	}

	return (neg ? -(ntv2_fp16)result : (ntv2_fp16)result);
}

ntv2_fp16 ntv2_fp16_exp(ntv2_fp16 val)
{
	bool neg;
	ntv2_fp16 result;
	s32 term;
	u8 i;

	if(val == 0      ) return ntv2_fp16_one;
	if(val == ntv2_fp16_one) return ntv2_fp16_e;
	if(val >= 681391 ) return ntv2_fp16_max;
	if(val <= -772243) return 0;

	/* The algorithm is based on the power series for exp(x):
	 * http://en.wikipedia.org/wiki/Exponential_function#Formal_definition
	 *
	 * From term n, we get term n+1 by multiplying with x/n.
	 * When the sum term drops to zero, we can stop summing.
	 */

	/* The power-series converges much faster on positive values
	 * and exp(-x) = 1/exp(x).
	 */
	neg = (val < 0);
	if (neg) val = -val;

	result = val + ntv2_fp16_one;
	term = val;

	for (i = 2; i < 30; i++)
	{
		term = ntv2_fp16_mul(term, ntv2_fp16_div(val, ntv2_fp16_init(i, 0)));
		result += term;

		if ((term < 500) && ((i > 15) || (term < 20)))
			break;
	}

	if (neg) result = ntv2_fp16_div(ntv2_fp16_one, result);

	return result;
}

ntv2_fp16 ntv2_fp16_ln(ntv2_fp16 val)
{
	const ntv2_fp16 e_to_fourth = 3578144;
	s32 guess = ntv2_fp16_init(2, 0);
	s32 delta;
	int scaling = 0;
	int count = 0;
	s32 e = 0;

	if (val <= 0)
		return ntv2_fp16_min;

	/* Bring the value to the most accurate range (1 < x < 100) */
	while (val > ntv2_fp16_init(100, 0))
	{
		val = ntv2_fp16_div(val, e_to_fourth);
		scaling += 4;
	}

	while (val < ntv2_fp16_one)
	{
		val = ntv2_fp16_mul(val, e_to_fourth);
		scaling -= 4;
	}

	do
	{
		/* Solving e(x) = y using Newton's method
		 * f(x) = e(x) - y
		 * f'(x) = e(x)
		 */
		e = ntv2_fp16_exp(guess);
		delta = ntv2_fp16_div(val - e, e);

		/* It's unlikely that logarithm is very large, so avoid overshooting. */
		if (delta > ntv2_fp16_init(3, 0))
			delta = ntv2_fp16_init(3, 0);

		guess += delta;
	} while ((count++ < 10)
		&& ((delta > 1) || (delta < -1)));

	return guess + ntv2_fp16_init(scaling, 0);
}

ntv2_fp16 ntv2_fp16_pow(ntv2_fp16 x, ntv2_fp16 y)
{
	ntv2_fp16 result;
	bool negative_x;

	if (x == 0) return 0;

	result = ntv2_fp16_exp(ntv2_fp16_mul(y, ntv2_fp16_ln(x)));
	negative_x = (bool)(x & ntv2_fp16_overflow);

	if (!negative_x && (result == ntv2_fp16_max))
	{
		//over flow, clamp to max positive number
		result = ntv2_fp16_max_positive;
	}

	return result;
}