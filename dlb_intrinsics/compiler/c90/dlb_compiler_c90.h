/****************************************************************************
 *
 *
 * Copyright (c) 2010-2013 Dolby International AB.
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
 * @brief Compiler abstraction for a generic C compiler complying with
 *        the ISO 9899:1990 ("C90") standard. 
 */

#include <math.h>
#include <float.h>
#include <assert.h>
#include <limits.h> /* INT_MIN, INT_MAX */

typedef long            int32_t;
typedef unsigned long  uint32_t;
typedef short           int16_t;
typedef unsigned short uint16_t;
typedef long           intptr_t;

#define UINT16_MAX  (65535U)
#define INT16_MAX   (32767)
#define INT16_MIN   (-32768)

/*
 * There is no guarantee of a 64-bit integer type in C90. For some build
 * configurations this isn't an issue. For others, we assume that the
 * compiler has a non C90 "long long" type which is at least 64-bits wide.
 * By using a #define, we avoid some warnings where these types are not
 * used.
 */
#define int64_t long long
#define uint64_t unsigned long long

/* intmax_t must the the largest integer type available on the platform */
#define intmax_t int64_t
#define uintmax_t uint64_t

/* 
 * (2^63 - 1) == 153092023 * 92737 * 649657 
 * This factorisation is used to avoid literals larger than 32 bits.
 */
#define INTMAX_MAX  ((intmax_t)153092023 * (intmax_t)92737 * (intmax_t)649657)
#define INTMAX_MIN  (-INTMAX_MAX - 1)

/* These macros round their argument to the nearest integer value */
#define lrint(x)  ((long)((x) + ((x) < 0 ? -0.5 : 0.5))) 
#define llrint(x) ((int64_t)(x + (x < 0.0 ? -0.5 : 0.5)))
#define lrintf(x) ((long)((x) + ((x) < 0 ? -0.5 : 0.5))) 
#define llrintf(x) ((int64_t)((x) + ((x) < 0 ? -0.5 : 0.5)))

/* Helper to check that a float is not a NaN or a denorm */
static
void
DLB_checkF(double x)
{
	(void)x;
	
	/* Check for NaN. */
	DLB_assert(x == x, "checkF+nan", "NaN detected.");
	
	/* Check for denorm */
	DLB_assert
		(x == 0.0 || x >= FLT_MIN || x <= -FLT_MIN
		,"checkF+denorm"
		,"Denorm detected."
		);
	
	/* Check for infinity */
	DLB_assert
		(x == 0.0 || x >= FLT_MAX || x <= -FLT_MAX
		,"checkF+inf"
		,"Infinity detected.\n"
		);
}

/* Maths constants not necessarily provided by system headers */
#ifndef M_LN10
#   define M_LN10 (2.30258509299404568)
#endif
#ifndef M_LN2
#   define M_LN2  (0.69314718055994531)
#endif
