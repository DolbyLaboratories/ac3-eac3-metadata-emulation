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
 * @brief Compiler abstraction for GCC
 */

/* GCC Already uses the standard C99 type names int32_t etc, so we don't
 * need to define them.
 */

/* Linux/gcc does define M_PI and friends by default, unless you tell it not to.
 * It seems that we tell it not to by using -std=c99 -pedantic, because those
 * symbols are "BSD derived extensions".  We can turn them back on by adding
 * _BSD_SOURCE or _GNU_SOURCE to our command line, which we can do in the manifests or
 * in source files that use those symbols...
 *
 * Or we could put a #define _BSD_SOURCE here...
 */

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif /* _BSD_SOURCE */

#ifndef __STDC_LIMIT_MACROS 
#define __STDC_LIMIT_MACROS
#endif /* #ifndef __STDC_LIMIT_MACROS */

#include <stdint.h>
#include <math.h>
#include <float.h>
#include <limits.h> /* INT_MIN, INT_MAX */


/* gcc provides intrinsics for lrint and lrintf: use them */
#ifndef DLB_HAVE_LRINTF
#ifdef lrint
#undef lrint
#endif
#ifdef lrintf
#undef lrintf
#endif
#ifdef llrint
#undef llrint
#endif 
#ifdef llrintf
#undef llrintf
#endif 
#define lrint(x) __builtin_lrint(x)
#define lrintf(x) __builtin_lrintf(x)
#define llrint(x) __builtin_llrint(x)
#define llrintf(x) __builtin_llrintf(x)
#define DLB_HAVE_LRINTF
#endif /* DLB_HAVE_LRINTF */

/* gcc provides an intrinsic for clz
 * that is unfortunately undefined if the argument is zero */
#ifndef DLB_HAVE_UCLZ32
static DLB_PURE_FN inline unsigned int
DLB_Uclz32(uint32_t a) {
    return 0u == a ? 32u
                   : (unsigned int)__builtin_clz(a); }
#define DLB_HAVE_UCLZ32
#endif /* DLB_HAVE_UCLZ32 */

/* Helper to check that a float is not a NaN or a denorm */
static inline
void
DLB_checkF(double x)
{
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
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
#else
	(void)x;

	/* Check for NaN. */
	DLB_assert(x == x, "checkF+nan", "NaN detected.");

	/* Check for denorm */
	DLB_assert(
#if (DLB_SFRACT_BITS >= 64)
                  x == 0.0 || fabs(x) > DBL_MIN
#else
                  x == 0.0 || fabsf(x) > FLT_MIN
#endif
		,"checkF+denorm"
		,"Denorm detected.\n"
		);

	/* Check for infinity */
	DLB_assert(
#if (DLB_SFRACT_BITS >= 64)
                 fabs(x) <= DBL_MAX
#else
                 fabsf(x) <= FLT_MAX
#endif
		,"checkF+inf"
		,"Infinity detected.\n"
		);
#endif
}
