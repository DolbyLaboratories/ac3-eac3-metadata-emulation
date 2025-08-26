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
 * @brief Compiler abstraction for the Microsoft C compiler
 */

/* Disable paranoid warnings about potential buffer overruns */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Disable irrelevant and factually incorrect warnings about deprecation of
 * standard C functions.
 */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

/* Bring in useful maths constants */
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <float.h>
#include <limits.h> /* INT_MIN, INT_MAX */
 
typedef __int64           int64_t;
typedef unsigned __int64 uint64_t;
typedef __int32           int32_t;
typedef unsigned __int32 uint32_t;
typedef __int16           int16_t;
typedef unsigned __int16 uint16_t;
typedef int64_t          intmax_t;
typedef uint64_t        uintmax_t;

#define UINT16_MAX  (65535U)
#define INT16_MAX   (32767)
#define INT16_MIN   (-32768)
#define INT32_MAX   (0x7FFFFFFF)
#define INT32_MIN   (-0x7FFFFFFF - 1)
#define INTMAX_MAX  (9223372036854775807i64)
#define INTMAX_MIN  (-INTMAX_MAX - 1)

#define INTMAX_C(x) (x##i64)

#if defined(_MSC_VER) && ((_MSC_VER <= 1200) || ((_MSC_VER < 1400) && defined(_M_AMD64))) && !defined(__cplusplus)
static inline float
frexpf(float _X, int *_Y) { return (float)frexp((double)_X, _Y); }
#else
#define DLB_HAVE_C99_FLOAT_MATH_FN
#endif  /* old MSVC doesn't have frexpf in non-C++ code */

/* In VS2005 without SP1 there is an erroneous semicolon at the end of the macro definition for logf.
 * To avoid build problems using logf, we define it here correctly.
 * This is discussed at:
 * http://connect.microsoft.com/VisualStudio/feedback/details/98751/logf-macro-in-math-h-has-spurious-trailing-semicolon
 * Note: _MSC_VER is 1400 for VS2005 (regardless of service pack installed).
 */
#if defined(_MSC_VER) && (_MSC_VER == 1400) && !defined (_M_IA64) && !defined (_M_AMD64) && defined(logf)
#undef logf
#define logf(x)     ((float)log((double)(x)))
#endif /* Old MSVS has bad declaration for logf */

/* No version of MSVC seems to have lrint or lrintf or round or anything similar, which is very odd: */
static inline long
lrint(double _a) { return (long)(_a + (_a < 0.0 ? -0.5 : 0.5)); }

static inline int64_t 
llrint(double _a) { return (int64_t)(_a + (_a < 0.0 ? -0.5 : 0.5));}

static inline long
lrintf(float _a) { return (long)(_a + (_a < 0.0f ? -0.5f : 0.5f)); }

static inline int64_t 
llrintf(float _a) { return (int64_t)(_a + (_a < 0.0f ? -0.5f : 0.5f));}

/* Nor fmin, fminf, fmax, fmaxf... */
static inline double
fmin(double _a, double _b) { return _a < _b ? _a : _b; }

static inline float
fminf(float _a, float _b) { return _a < _b ? _a : _b; }

static inline double
fmax(double _a, double _b) { return _a < _b ? _b : _a; }

static inline float
fmaxf(float _a, float _b) { return _a < _b ? _b : _a; }

/* Helper to check that a float is not a NaN or a denorm */
static inline
void
DLB_checkF(double x)
{
    const int64_t* s = (const int64_t*) &x;
    int64_t exponent, fraction;

    exponent = *s & ((((int64_t)1<<12) - 1) << 52);
    fraction = *s & (((int64_t)1<<52) - 1);

    /* Check for denormal */
    DLB_assert(exponent || !fraction, "checkF+denorm", "Denorm detected.");

    /* Check for NaN/infinity */
    DLB_assert
        (exponent != ((int64_t)2047 << 52), "checkF+nan+inf"
        ,"NaN or infinity detected.\n"
        );
}

/* If operations with denormal floating point values are slow (and they are with x87 FPUs) then
 * the DLB_?flush?() operations must bother to flush, otherwise it is OK to do nothing.
 *
 * This is really not the correct test.  It is probably a safe default, but if the code
 * is able to manipulate the FPU flags to set denormal flushing in hardware, then the
 * software doesn't need to do it.  I can't think of a nice way to convey this state in the
 * source code.
 */
#if defined(_MSC_VER) && (_MSC_VER <= 1600)
#ifndef DLB_MUST_FLUSH_DENORMS
#define DLB_MUST_FLUSH_DENORMS
#endif /* DLB_MUST_FLUSH_DENORMS */
#endif
