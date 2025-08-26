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
 * @brief Compiler abstraction for Code Composer Studio cl6x
 */

#include <assert.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <limits.h>
#include <math.h>

#ifndef M_PI
/* I haven't found a way to get math.h or other to expose these.
 * Odd. */
#define M_E         2.71828182845904523536028747135266250   /* e */
#define M_LOG2E     1.44269504088896340735992468100189214   /* log 2e */
#define M_LOG10E    0.434294481903251827651128918916605082  /* log 10e */
#define M_LN2       0.693147180559945309417232121458176568  /* log e2 */
#define M_LN10      2.30258509299404568401799145468436421   /* log e10 */
#define M_PI        3.14159265358979323846264338327950288   /* pi */
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2 */
#define M_PI_4      0.785398163397448309615660845819875721  /* pi/4 */
#define M_1_PI      0.318309886183790671537767526745028724  /* 1/pi */
#define M_2_PI      0.636619772367581343075535053490057448  /* 2/pi */
#define M_2_SQRTPI  1.12837916709551257389615890312154517   /* 2/sqrt(pi) */
#define M_SQRT2     1.41421356237309504880168872420969808   /* sqrt(2) */
#define M_SQRT1_2   0.707106781186547524400844362104849039  /* 1/sqrt(2) */
#endif

/* Force generic_risc16x32 backend to use the shift version of _dlb_sgn().
 * The default (safe) version of _dlb_sgn() doesn't work well on TI's platform, 
 * compiled by cl6x.
 * There might be a compiler bug.
 */
#ifndef DLB_SGN_SHIFT31
#define DLB_SGN_SHIFT31
#endif

/* TI runtime libs provide some floating point functions, but not these ones.
   They are needed for building a monolithic low level operator test harness
   with a built-in floating point reference. */
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
    (void)x; /* UNUSED */
}

/* The TI compiler and library support the float-argument math functions */
#define DLB_HAVE_C99_FLOAT_MATH_FN

#ifdef C64PLUSEVM
#include <evm6424.h>
/*XXX*/
#include <stdio.h>
/*XXX*/

static void do_cache_init(void)
{
    printf ("was: L1PCFG:%08X, L1DCFG:%08X, CACHE_L2CFG=%08X\n",CACHE_L1PCFG, CACHE_L1DCFG, CACHE_L2CFG);

    CACHE_L1PCFG = 0x4;     /* 32KB cache */
    CACHE_L1DCFG = 0x4;     /* 32KB cache */
    CACHE_L2CFG = 0x3;      /* 128KB cache */

    printf ("now: L1PCFG:%08X, L1DCFG:%08X, CACHE_L2CFG=%08X\n",CACHE_L1PCFG, CACHE_L1DCFG, CACHE_L2CFG);

    *( volatile Uint32* )( CACHE_MAR_BASE + 0x200 ) = 0x1; /* MAR 128 for 0x80000000 - 0x80FFFFFF cacheable */
    *( volatile Uint32* )( CACHE_MAR_BASE + 0x204 ) = 0x1; /* MAR 128 for 0x81000000 - 0x81FFFFFF cacheable */
    *( volatile Uint32* )( CACHE_MAR_BASE + 0x208 ) = 0x1; /* MAR 128 for 0x82000000 - 0x82FFFFFF cacheable */
    *( volatile Uint32* )( CACHE_MAR_BASE + 0x20C ) = 0x1; /* MAR 128 for 0x83000000 - 0x83FFFFFF cacheable */
    *( volatile Uint32* )( CACHE_MAR_BASE + 0x210 ) = 0x1; /* MAR 128 for 0x84000000 - 0x84FFFFFF cacheable */

    TIMER0_TGCR = 0;
    TIMER0_TIM12=0;
    TIMER0_PRD12 = 0x30000000;
    TIMER0_EMUMGT = 0x0;
    TIMER0_TRC = 0x80;
    TIMER0_TGCR = 3;

}

#define main \
/*int*/ hook_main(int, char**); \
int main(int argc, char** argv) { do_cache_init(); return hook_main(argc, argv); } \
int hook_main

#endif /* C64PLUSEVM */

