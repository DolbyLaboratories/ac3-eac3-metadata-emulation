/****************************************************************************
 *
 *
 * Copyright (c) 2010 Dolby International AB.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED
 * BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @file
 * @brief Compiler abstraction for a generic C compiler complying with the
 *        ISO 9899:1999 ("C99") standard. 
 */

#include <math.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <assert.h>
#include <limits.h> /* INT_MIN, INT_MAX */

/* Helper to check that a float is not a NaN or a denorm */
static inline
void
DLB_checkF(double x)
{
	switch(fpclassify(x))
	{
	case FP_NAN:
		DLB_print(DLB_ERROR("checkF+nan") "NaN detected.\n");
		break;
	case FP_INFINITE:
		DLB_print(DLB_ERROR("checkF+inf") "Infinity detected.\n");
		break;
	case FP_ZERO:
		break;
	case FP_SUBNORMAL:
		DLB_print(DLB_ERROR("checkF+denorm") "Denorm detected.\n");
		break;
	case FP_NORMAL:
		break;
	default:
		DLB_print(DLB_ERROR("checkF") "Unknown float type.\n");
	}
}

/* Maths constants not necessarily provided by system headers */
#ifndef M_LN10
#   define M_LN10 (2.30258509299404568)
#endif
#ifndef M_LN2
#   define M_LN2  (0.69314718055994531)
#endif

/* Enable DLB_HAVE_C99_MATH_FN switch by default for C99 compiler. 
 * This gets used in math_inlines.h, for mapping of double precision
 * math functions to single precision 'f' flavor functions provided
 * by C99 compiler tool chain.
 */
#define DLB_HAVE_C99_FLOAT_MATH_FN
