/****************************************************************************
 *
 *
 * Copyright (c) 2004-2013 Dolby International AB.
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
 * @addtogroup dlb_intrinsics_core_generic_backends
 * @{
 * @section GENERIC_FLOAT64
 * Abstraction for generic double precision floating point architecture.
 * This is the "golden reference" backend that uses double precision floating-point for
 * all types. It also uses some non-optimal behaviour to more closely model the fixed-point
 * backends, which would not be desirable in an actual implementation. Useful for testing,
 * but not intended as a deployment target. It will always give saturated results for all
 * saturating operations (e.g. L*s*addLS()). The backend can be compiled with different
 * preprocessor directives which influence the backend behavior.
 * The list of defines can be found at @ref float64_behavior_defines "backend description".
 * @}
 */

/****************************************************************************//**
 * @file
 * @brief Abstraction for generic double precision floating point architecture
 * @{
 * @section float64_behavior_defines Behavior-Altering Preprocessor Directives
 * The following symbols may be defined (perhaps implicitly, by the compiler)
 * to alter behaviour in various ways:
 * 
 * @par DLB_MUST_FLUSH_DENORMS
 * Floating point values less than FLT_MIN can be set to 0.0f for performance reasons by
 * a call to the provided flush operation. If the define is not set these operations return
 * the input value.
 *
 * @}
 * @internal
 * @{
 */
 
#include <math.h>   /* for floor() */
#include <float.h>  /* for DBL_MIN */

/*
sizes of the fractional types
******************************************************************************/
#define DLB_DOUBLE_MANTBITS 53

#define DLB_SFRACT_BITS     64  /* Total bits in DLB_SFRACT type */
#define DLB_SFRACT_MANTBITS DLB_DOUBLE_MANTBITS

#define DLB_LFRACT_BITS     64  /* Total bits in DLB_LFRACT type */
#define DLB_LFRACT_MANTBITS DLB_DOUBLE_MANTBITS

#define DLB_ACCU_BITS       64  /* Total bits in DLB_ACCU type */
#define DLB_ACCU_MANTBITS   DLB_DOUBLE_MANTBITS

#define DLB_XACCU_BITS      64  /* Total bits in DLB_XACCU type */
#define DLB_XACCU_MANTBITS  DLB_DOUBLE_MANTBITS

#define DLB_NACCU_BITS      64  /* Total bits in DLB_NACCU type */
#define DLB_NACCU_MANTBITS  DLB_DOUBLE_MANTBITS


#ifndef DLB_NATIVE_FRAC_BITS
#define DLB_NATIVE_FRAC_BITS 15
#endif

#define DLB_MAX_SHIFT    60

/*
macros for legacy support
******************************************************************************/
#define DLB_L(x) x
#define DLB_H(x) x

/*
support macros
******************************************************************************/	
#define _dlb_lround(f)        (intmax_t) (((f) >= 0.0) ? ((f)+0.5):((f)-0.5))

#define _isatq15(a) \
		( (a) <= -1.0f				 ? -32768 \
		: (32767.0f/32768.0f) <= (a)   ?  32767 \
		: _dlb_lround((double)(a)*32768.0))

/* Conceptually assert(0 <= n && n <= 31) here, but can't really do that in a compile-time
 * context, unfortunately.  Be careful! */
#define _two_to(n) (0 < (unsigned)(n) ? (2.0f)*(1 << (unsigned)((n)-1)) : 1.0f)

/* compile time 0.5^^n for constant n (requires a division, so only good for compile time) */
#define _half_to(n) ((1.0f)/_two_to(n))

#define _iqfu(a, sigbits) \
    ((intmax_t)(_two_to(sigbits) * (a) + ((a) < 0.0f ? -0.5f : 0.5f)))

#define _isatq(a, q) \
    ( (a) <= -1.0f               ? -1 - ((1 << (q)) - 1) \
    : 1.0f - _half_to(q) <= (a)  ?      ((1 << (q)) - 1) \
    : _iqfu((a), (q)) )


#define DLB_FcqF(x)         (( 1.0 / 32768.0) * _isatq15(x))
#define DLB_FcqFU(x, q)     (_half_to(q) * _isatq((x), (q)))

/*
fractional types
******************************************************************************/
typedef double DLB_XACCU;
typedef double DLB_ACCU;
typedef double DLB_LFRACT;
typedef double DLB_SFRACT;
typedef double DLB_NACCU;

/*
constant generation macros
******************************************************************************/
#define DLB_UscaleU(i)      (i)
#define DLB_FscaleFU(x,i)   ((double) (x) / ((uintmax_t)1 << (i)))

#define DLB_LcF(x)          ((double) (x))
#define DLB_LcFU(x, s)      ((double) (x))
#define DLB_LcIU(x, s)      ((double) (x)/((intmax_t)1<<(s)))
#define DLB_LcqF(x)         DLB_FcqF(x)
#define DLB_LcqFU(x, s)     DLB_FcqF(x)

#define DLB_ScF(x)          ((double) (x))
#define DLB_ScFU(x, s)      ((double) (x))
#define DLB_ScIU(x, s)      ((double) (x)/((intmax_t)1<<(s)))
#define DLB_ScqF(x)         DLB_FcqF(x)
#define DLB_ScqFU(x, s)     DLB_FcqF(x)


#ifdef DLB_MUST_FLUSH_DENORMS
#define _dlb_flush_eps (DBL_MIN*512)
#define _flush(x) (_dlb_flush_eps < fabs(x) ? x : 0.0)
#else
#define _flush(x) (x)
#endif /* DLB_MUST_FLUSH_DENORMS */

/*
processor-specific implementations of intrinsics
******************************************************************************/
static const double _dlb_Q15 = 32768.0;
static const double _dlb_Q31 = -(double)(INT32_MIN);
static const double _dlb_Qnative = -(double)((long)(~0L << DLB_NATIVE_FRAC_BITS));
static const double _dlb_Q15_recip = 1.0/32768.0;
static const double _dlb_Q31_recip = -1.0/(double)(INT32_MIN);
static const double _dlb_Qnative_recip = -1.0/((long)(~0L << DLB_NATIVE_FRAC_BITS));
static const long INATIVE_MAX = ~(~0L << DLB_NATIVE_FRAC_BITS);

static DLB_PURE_FN inline double _dlb_fclip(double a, double lo, double hi) { return fmin( fmax(a, lo), hi); }

static DLB_PURE_FN inline intmax_t _dlb_lmax(intmax_t a, intmax_t b) { return a < b ? b : a; }
static DLB_PURE_FN inline intmax_t _dlb_lmin(intmax_t a, intmax_t b) { return a < b ? a : b; }
static DLB_PURE_FN inline intmax_t _dlb_lclip(intmax_t a, intmax_t lo, intmax_t hi) { return _dlb_lmin( _dlb_lmax(a, lo), hi); }

static inline
double
_dlb_saturate(double f)
{
    return _dlb_fclip(f, -1.0, 1.0);

}

static inline
DLB_SFRACT
DLB_S_M(intmax_t m DLB_XARGS)
{
    return (DLB_SFRACT) m / ((uintmax_t)1 << DLB_SFRACT_MANTBITS);
}

static inline
DLB_LFRACT
DLB_L_M(intmax_t m DLB_XARGS)
{
    return (DLB_LFRACT) m / ((uintmax_t)1 << DLB_LFRACT_MANTBITS);
}

static inline
DLB_ACCU
DLB_A_M(intmax_t m DLB_XARGS)
{
    return (DLB_ACCU) m / ((uintmax_t)1 << DLB_ACCU_MANTBITS);
}

static inline
DLB_XACCU
DLB_X_M(intmax_t m DLB_XARGS)
{
    return (DLB_XACCU) m / ((uintmax_t)1 << DLB_XACCU_MANTBITS);
}

static inline
intmax_t
DLB_M_S(DLB_SFRACT x DLB_XARGS)
{
    DLB_SFRACT r = x * ((uintmax_t)1 << DLB_SFRACT_MANTBITS);
	/* C specifies that the float->int cast truncates towards zero, but we
	 * want truncation towards -infinity to emulate fixed point behaviour.
	 */
	return (intmax_t) floor(r);
}

static inline
intmax_t
DLB_M_L(DLB_LFRACT x DLB_XARGS)
{
    DLB_LFRACT r = x * ((uintmax_t)1 << DLB_LFRACT_MANTBITS);
	/* C specifies that the float->int cast truncates towards zero, but we
	 * want truncation towards -infinity to emulate fixed point behaviour.
	 */
	return (intmax_t) floor(r);
}

static inline
intmax_t
DLB_M_A(DLB_ACCU x DLB_XARGS)
{
    DLB_ACCU r = x * ((uintmax_t)1 << DLB_ACCU_MANTBITS);
	/* C specifies that the float->int cast truncates towards zero, but we
	 * want truncation towards -infinity to emulate fixed point behaviour.
	 */
	return (intmax_t) floor(r);
}

static inline
intmax_t
DLB_M_X(DLB_XACCU x DLB_XARGS)
{
    DLB_XACCU r = x * ((uintmax_t)1 << DLB_XACCU_MANTBITS);
	/* C specifies that the float->int cast truncates towards zero, but we
	 * want truncation towards -infinity to emulate fixed point behaviour.
	 */
	return (intmax_t) floor(r);
}

static inline
DLB_SFRACT
DLB_S_F
    (double f
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_SFRACT
DLB_SqF
    (double f
     DLB_XARGS
    )
{
    return DLB_ScqF(f);
}

static inline
DLB_LFRACT
DLB_L_F
    (double f
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_LFRACT
DLB_LqF
    (double f
     DLB_XARGS
    )
{
    return DLB_LcqF(f);
}

static inline
DLB_ACCU
DLB_A_F
    (double f
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_XACCU
DLB_X_F
    (double f
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_SFRACT
DLB_S_16
    (int16_t x
     DLB_XARGS
    )
{
    return _dlb_Q15_recip * x;
}

static inline
DLB_SFRACT
DLB_S_32
    (int32_t x
     DLB_XARGS
    )
{
    return _dlb_Q31_recip * x;
}

static inline
DLB_LFRACT
DLB_L_16
    (int16_t x
     DLB_XARGS
    )
{
    return _dlb_Q15_recip * x;
}

static inline
DLB_LFRACT
DLB_L_32
    (int32_t x
     DLB_XARGS
    )
{
    return _dlb_Q31_recip * x;
}

static inline
double
DLB_F_S
    (DLB_SFRACT x
     DLB_XARGS
    )
{
    return x;
}

static inline
double
DLB_F_L
    (DLB_LFRACT x
     DLB_XARGS
    )
{
    return x;
}

static inline
double
DLB_F_A
    (DLB_ACCU x
     DLB_XARGS
    )
{
    return x;
}

static inline
double
DLB_F_X
    (DLB_XACCU x
     DLB_XARGS
    )
{
    return x;
}

static inline
int16_t
DLB_16srndS
    (DLB_SFRACT x
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip(lrint(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int16_t
DLB_16sfloorS
    (DLB_SFRACT x
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip((long)floor(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int32_t
DLB_32srndS
    (DLB_SFRACT x
     DLB_XARGS
    )
{
    return (int32_t)lrint(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
}

static inline
int32_t
DLB_32sfloorS
    (DLB_SFRACT x
     DLB_XARGS
    )
{
    return (int32_t)_dlb_lclip((long)floor(_dlb_Q31 * x), INT32_MIN, INT32_MAX);
}

static inline
int16_t
DLB_16srndL
    (DLB_LFRACT x
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip(lrint(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int16_t
DLB_16sfloorL
    (DLB_LFRACT x
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip((long)floor(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int32_t
DLB_32srndL
    (DLB_LFRACT x
     DLB_XARGS
    )
{
    return (int32_t)lrint(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
}

static inline
int32_t
DLB_32sfloorL
    (DLB_LFRACT x
     DLB_XARGS
    )
{
    return (int32_t)_dlb_lclip((long)floor(_dlb_Q31 * x), INT32_MIN, INT32_MAX);
}

static inline
int
DLB_I_S
    (DLB_SFRACT s
     DLB_XARGS
    )
{
    return (int)_dlb_lmin(INATIVE_MAX, (long)floor(s * _dlb_Qnative));
}

static inline
int
DLB_IsrndS
    (DLB_SFRACT s
     DLB_XARGS
    )
{
    return (int)_dlb_lmin(INATIVE_MAX, lrint(s * _dlb_Qnative));
}

static inline
DLB_SFRACT
DLB_S_I
    (int        i
     DLB_XARGS
    )
{
    return (DLB_SFRACT)(_dlb_Qnative_recip * i);
}

static inline
int
DLB_I_L
    (DLB_LFRACT l
     DLB_XARGS
    )
{
    return (int)_dlb_lmin(INATIVE_MAX, (long)floor(l * _dlb_Qnative));
}

static inline
int
DLB_IsrndL
    (DLB_LFRACT l
     DLB_XARGS
    )
{
    return (int)_dlb_lmin(INATIVE_MAX, lrint(l * _dlb_Qnative));
}

static inline
int
DLB_IsrndLU
    (DLB_LFRACT l
    ,unsigned fp
     DLB_XARGS
    )
{
    return (int)_dlb_lclip(llrint(l * ((intmax_t)1 << fp)), INT_MIN, INT_MAX);
}

static inline
DLB_LFRACT
DLB_L_I
    (int        i
     DLB_XARGS
    )
{
    return (DLB_LFRACT)(_dlb_Qnative_recip * i);
}

static inline
int
DLB_IintLU
    (DLB_LFRACT x
    ,unsigned   fp
     DLB_XARGS
    )
{
    return (int)_dlb_lmin(INATIVE_MAX, (intmax_t)floor(x * ((intmax_t)1 << fp)));
}

static inline
DLB_LFRACT
DLB_LmantLU
    (DLB_LFRACT x
    ,unsigned   fp
     DLB_XARGS
    )
{
    x *= ((intmax_t)1 << fp);
    return x - (DLB_LFRACT) floor(x);
}

static inline
DLB_SFRACT
DLB_SminSS
    (DLB_SFRACT  a
    ,DLB_SFRACT  b
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_SFRACT
DLB_SmaxSS
    (DLB_SFRACT  a
    ,DLB_SFRACT  b
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_LFRACT
DLB_LminLL
    (DLB_LFRACT  a
    ,DLB_LFRACT  b
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_LFRACT
DLB_LmaxLL
    (DLB_LFRACT  a
    ,DLB_LFRACT  b
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_ACCU
DLB_AminAA
    (DLB_ACCU  a
    ,DLB_ACCU  b
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_ACCU
DLB_AmaxAA
    (DLB_ACCU  a
    ,DLB_ACCU  b
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_XACCU
DLB_XminXX
    (DLB_XACCU  a
    ,DLB_XACCU  b
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_XACCU
DLB_XmaxXX
    (DLB_XACCU  a
    ,DLB_XACCU  b
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
int
DLB_IltSS
    (DLB_SFRACT  a
    ,DLB_SFRACT  b
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int
DLB_IleqSS
    (DLB_SFRACT  a
    ,DLB_SFRACT  b
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int
DLB_IeqSS
    (DLB_SFRACT  a
    ,DLB_SFRACT  b
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int
DLB_IltLL
    (DLB_LFRACT  a
    ,DLB_LFRACT  b
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int
DLB_IleqLL
    (DLB_LFRACT  a
    ,DLB_LFRACT  b
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int
DLB_IeqLL
    (DLB_LFRACT  a
    ,DLB_LFRACT  b
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int
DLB_IltAA
    (DLB_ACCU  a
    ,DLB_ACCU  b
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int
DLB_IleqAA
    (DLB_ACCU  a
    ,DLB_ACCU  b
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int
DLB_IeqAA
    (DLB_ACCU  a
    ,DLB_ACCU  b
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int
DLB_IltXX
    (DLB_XACCU  a
    ,DLB_XACCU  b
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int
DLB_IleqXX
    (DLB_XACCU  a
    ,DLB_XACCU  b
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int
DLB_IeqXX
    (DLB_XACCU  a
    ,DLB_XACCU  b
     DLB_XARGS
    )
{
    return a == b;
}

static inline
double
DLB_FshlFU
    (double a
    ,unsigned i
     DLB_XARGS
    )
{
    return a * ((intmax_t)1 << i);
}

static inline
double
DLB_FshrFU
    (double a
    ,unsigned i
     DLB_XARGS
    )
{
    return a * (1.0 / ((intmax_t)1 << i));
}

static inline
double
DLB_FshlFI
    (double a
    ,int    i
     DLB_XARGS
    )
{
    return (0 <= i) ? DLB_FshlFU(a, (unsigned)i DLB_PASS_XARGS)
                    : DLB_FshrFU(a, (unsigned)(-i) DLB_PASS_XARGS);
}

static inline
double
DLB_FshrFI
    (double a
    ,int    i
     DLB_XARGS
    )
{
    return (0 <= i) ? DLB_FshrFU(a, (unsigned)i DLB_PASS_XARGS)
                    : DLB_FshlFU(a, (unsigned)(-i) DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SshlSU
    (DLB_SFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SsshlSU
    (DLB_SFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFU(a, i DLB_PASS_XARGS));
}

static inline
DLB_SFRACT
DLB_SshrSU
    (DLB_SFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SshlSI
    (DLB_SFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SshrSI
    (DLB_SFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SsshlSI
    (DLB_SFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFI(a, i DLB_PASS_XARGS));
}

static inline
DLB_SFRACT
DLB_SsshrSI
    (DLB_SFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshrFI(a, i DLB_PASS_XARGS));
}

static inline
DLB_LFRACT
DLB_LshlLU
    (DLB_LFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LsshlLU
    (DLB_LFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFU(a, i DLB_PASS_XARGS));
}

static inline
DLB_LFRACT
DLB_LshrLU
    (DLB_LFRACT a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LshlLI
    (DLB_LFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LshrLI
    (DLB_LFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LsshlLI
    (DLB_LFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFI(a, i DLB_PASS_XARGS));
}

static inline
DLB_LFRACT
DLB_LsshrLI
    (DLB_LFRACT a
    ,int        i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshrFI(a, i DLB_PASS_XARGS));
}

static inline
DLB_ACCU
DLB_AshlAU
    (DLB_ACCU   a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU
DLB_AshrAU
    (DLB_ACCU   a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU
DLB_AshlAI
    (DLB_ACCU   a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU
DLB_AshrAI
    (DLB_ACCU   a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU
DLB_XshlXU
    (DLB_XACCU  a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU
DLB_XshrXU
    (DLB_XACCU  a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}


static inline
DLB_XACCU
DLB_XshlXI
    (DLB_XACCU  a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU
DLB_XshrXI
    (DLB_XACCU  a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

#define _dlb_iclipiii(x, lo, hi) \
        (x <= lo ? lo :  hi <= x ? hi : x)

static DLB_PURE_FN inline
int
DLB_IlimsI
    (int        i
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static DLB_PURE_FN inline
int
DLB_IlimlI
    (int        i
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static DLB_PURE_FN inline
int
DLB_IlimaI
    (int        i
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static DLB_PURE_FN inline
int
DLB_IlimxI
    (int        i
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static DLB_PURE_FN inline
int
DLB_IlimnI
    (int        i
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static inline
DLB_LFRACT
DLB_LleftLU
    (DLB_LFRACT x
    ,unsigned   hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
unsigned
DLB_UnormF
    (double a)
{    
    static const double norm_min = 1.0f/((intmax_t)1<<DLB_MAX_SHIFT);
    /* it is possible that norm_min needs to be FLT_MIN for bug-compatability with previous versions */
    int exp;
    double m = frexp(a, &exp);
    return fabs(a) <= norm_min ? DLB_MAX_SHIFT
         : 0 < exp ? 0u
         : -0.5f == m ? (unsigned int)(1-exp)    /* XXX: ugly, but de tests fail otherwise */
                      : (unsigned int)(-exp); 
}

static inline
unsigned
DLB_UpnormF
    (double a)
{
    static const double norm_min = 1.0f/((intmax_t)1<<DLB_MAX_SHIFT);
    /* it is possible that norm_min needs to be FLT_MIN for bug-compatability with previous versions */
    int exp;
    (void)frexp(a, &exp);
    return a <= norm_min ? DLB_MAX_SHIFT
         : 0 < exp ? 0u
         : (unsigned int)(-exp);
}

static inline
unsigned
DLB_UnormS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned
DLB_UnormL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned
DLB_UnormA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned
DLB_UnormX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned
DLB_UpnormS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned
DLB_UpnormL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned
DLB_UpnormA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned
DLB_UpnormX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
DLB_SFRACT
DLB_SaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_SFRACT
DLB_SsubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_SFRACT
DLB_SsaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a + b);
}

static inline
DLB_SFRACT
DLB_SssubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a - b);
}

static inline
DLB_LFRACT
DLB_LaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_LFRACT
DLB_LsubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_LFRACT
DLB_LsaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a + b);
}

static inline
DLB_LFRACT
DLB_LssubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a - b);
}

static inline
DLB_LFRACT
DLB_LsaddLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a + b);
}

static inline
DLB_LFRACT
DLB_LssubLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a - b);
}

static inline
DLB_ACCU
DLB_AaddAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_ACCU
DLB_AsubAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_XACCU
DLB_XaddXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_XACCU
DLB_XsubXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_SFRACT
DLB_SnegS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return -a ;
}

static inline
DLB_SFRACT
DLB_SsnegS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_LFRACT
DLB_LnegL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_LFRACT
DLB_LsnegL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_ACCU
DLB_AnegA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_XACCU
DLB_XnegX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_SFRACT
DLB_SabsS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_SFRACT
DLB_SsabsS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT
DLB_LabsL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT
DLB_LsabsL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_ACCU
DLB_AabsA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_XACCU
DLB_XabsX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT
DLB_LmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT
DLB_LmpyLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT
DLB_LmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT
DLB_LsmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a * b);
}

static inline
DLB_LFRACT
DLB_LsmpyLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a * b);
}

static inline
DLB_LFRACT
DLB_LsmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a * b);
}

static inline
DLB_SFRACT
DLB_SrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_SFRACT
DLB_SsrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return _dlb_saturate(a * b);
}

static inline
DLB_ACCU
DLB_AmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_ACCU
DLB_AmpyLS
    (DLB_LFRACT l
    ,DLB_SFRACT s
     DLB_XARGS
    )
{
    return l * s;
}

static inline
DLB_ACCU
DLB_AmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_XACCU
DLB_XmpyLS
    (DLB_LFRACT l
    ,DLB_SFRACT s
     DLB_XARGS
    )
{
    return l * s;
}

static inline
DLB_ACCU
DLB_AmpyAS
    (DLB_ACCU   x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return x * y;
}

static inline
DLB_XACCU
DLB_XmpyXS
    (DLB_XACCU x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return x * y;
}

/* Mixed integer / fractional multiplications */
static inline
DLB_LFRACT
DLB_LmpyLI
    (DLB_LFRACT  a
    ,int         i
     DLB_XARGS
    )
{
    return a * i;
}

static inline
int
DLB_ImpyIL
    (int         i
    ,DLB_LFRACT  a
     DLB_XARGS
    )
{
    return (int)floor(i * a);
}

static inline
int
DLB_IrmpyIL
    (int         i
    ,DLB_LFRACT  a
     DLB_XARGS
    )
{
    return (int)lrint(i * a);
}

static inline
unsigned
DLB_UrmpyUL
    (unsigned    i
    ,DLB_LFRACT  a
     DLB_XARGS
    )
{
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    return (unsigned)lrint(i * a);
}

static inline
int
DLB_ImpyIS
    (int         i
    ,DLB_SFRACT  a
     DLB_XARGS
    )
{
    return (int)floor(i * a);
}

static inline
int
DLB_IrmpyIS
    (int         i
    ,DLB_SFRACT  a
     DLB_XARGS
    )
{
    return (int)lrint(i * a);
}

static inline
unsigned
DLB_UrmpyUS
    (unsigned    i
    ,DLB_SFRACT  a
     DLB_XARGS
    )
{
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    return (unsigned)lrint(i * a);
}

static inline
DLB_LFRACT
DLB_LsmacLSS
    (DLB_LFRACT a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a + _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LsmacLLS
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a + _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LsmacLLL
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a + _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LmacLSS
    (DLB_LFRACT a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT
DLB_LmacLLS
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT
DLB_LmacLLL
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU
DLB_AmacASS
    (DLB_ACCU   a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU
DLB_AmacALS
    (DLB_ACCU   a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU
DLB_AmacALL
    (DLB_ACCU   a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU
DLB_AmacAAS
    (DLB_ACCU   a
    ,DLB_ACCU   x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_XACCU
DLB_XmacXLS
    (DLB_XACCU  a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_XACCU
DLB_XmacXXS
    (DLB_XACCU  a
    ,DLB_XACCU  x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT
DLB_LsmsuLSS
    (DLB_LFRACT a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a - _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LsmsuLLS
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a - _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LsmsuLLL
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return _dlb_saturate(a - _dlb_saturate(x * y));
}

static inline
DLB_LFRACT
DLB_LmsuLSS
    (DLB_LFRACT a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT
DLB_LmsuLLS
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT
DLB_LmsuLLL
    (DLB_LFRACT a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU
DLB_AmsuASS
    (DLB_ACCU   a
    ,DLB_SFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU
DLB_AmsuALS
    (DLB_ACCU   a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU
DLB_AmsuALL
    (DLB_ACCU   a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU
DLB_AmsuAAS
    (DLB_ACCU   a
    ,DLB_ACCU   x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_XACCU
DLB_XmsuXLS
    (DLB_XACCU  a
    ,DLB_LFRACT x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_XACCU
DLB_XmsuXXS
    (DLB_XACCU  a
    ,DLB_XACCU  x
    ,DLB_SFRACT y
     DLB_XARGS
    )
{
    return a - x * y;
}


static inline
DLB_XACCU
DLB_X_S
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_ACCU
DLB_A_S
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_L_S
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_XACCU
DLB_X_L
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_ACCU
DLB_A_L
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_XACCU
DLB_X_A
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_StruncL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsatL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SrndL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsrndL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_SFRACT
DLB_StruncA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsatA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SrndA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsrndA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_LFRACT
DLB_LtruncA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LshrAU
    (DLB_ACCU a
     ,unsigned i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LshlAU
    (DLB_ACCU a
     ,unsigned i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LsshlAU
    (DLB_ACCU a
     ,unsigned i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFU(a, i DLB_PASS_XARGS));
}

static inline
DLB_LFRACT
DLB_LsatA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LrndA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LsrndA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_SFRACT
DLB_StruncX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsatX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SrndX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_SFRACT
DLB_SsrndX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_LFRACT
DLB_LtruncX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LsatX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LrndX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LsrndX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_ACCU
DLB_AtruncX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_ACCU
DLB_ArndX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline 
DLB_SFRACT
DLB_SrnddL(DLB_LFRACT a
           DLB_XARGS
           )
{
    return (DLB_SFRACT)(a); 
}

static inline 
DLB_SFRACT
DLB_SrnddA(DLB_ACCU a
           DLB_XARGS
           )
{
    return (DLB_SFRACT)(a); 
}

static inline 
DLB_SFRACT
DLB_SrnddX(DLB_XACCU a
           DLB_XARGS
           )
{
    return (DLB_SFRACT)(a); 
}

static inline 
DLB_LFRACT
DLB_LrnddA(DLB_ACCU a
           DLB_XARGS
           )
{
    return (DLB_LFRACT)(a); 
}

static inline 
DLB_LFRACT
DLB_LrnddX(DLB_XACCU a
           DLB_XARGS
           )
{
    return (DLB_LFRACT)(a); 
}

static inline 
DLB_LFRACT
DLB_LrnddN(DLB_NACCU a
           DLB_XARGS
           )
{
    return (DLB_LFRACT)(a); 
}

static inline 
DLB_ACCU
DLB_ArnddX(DLB_XACCU a
           DLB_XARGS
           )
{
    return (DLB_ACCU)(a); 
}

static inline 
DLB_ACCU
DLB_ArnddN(DLB_NACCU a
           DLB_XARGS
           )
{
    return (DLB_ACCU)(a); 
}

static inline 
DLB_ACCU
DLB_XrnddN(DLB_NACCU a
           DLB_XARGS
           )
{
    return (DLB_XACCU)(a); 
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddLU(DLB_LFRACT a, unsigned int bits)
{
    double scale = (double)((intmax_t)1 << bits);
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddLU"
              , "The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.\n"
              );
    return floor(a * scale) / scale;
}

static DLB_PURE_FN inline DLB_LFRACT
DLB_LrnddXU(DLB_XACCU a, unsigned int bits)
{
    double scale = (double)((intmax_t)1 << bits);
    DLB_assert(bits <= DLB_LFRACT_MANTBITS
              , "DLB_LrnddXU"
              , "The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.\n"
              );
    return floor(a * scale) / scale;
}

static inline
DLB_ACCU
DLB_AstoreA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_XACCU
DLB_XstoreX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_SFRACT
DLB_SflushS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_LFRACT
DLB_LflushL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_ACCU
DLB_AflushA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_XACCU
DLB_XflushX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}


/*
processor-specific implementations of nonlinear extension intrinsics
******************************************************************************/
static inline
DLB_NACCU
DLB_N_F
    (double f
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_NACCU
DLB_N_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a / (1 << (2*hr));
}

static inline
DLB_NACCU
DLB_N_M
    (intmax_t m
     DLB_XARGS
    )
{
    return (DLB_NACCU) m / ((uintmax_t)1 << DLB_NACCU_MANTBITS);
}

static inline
intmax_t
DLB_M_N
    (DLB_NACCU x
     DLB_XARGS
    )
{
    DLB_NACCU r = x * ((uintmax_t)1 << DLB_NACCU_MANTBITS);
	/* C specifies that the float->int cast truncates towards zero, but we
	 * want truncation towards -infinity to emulate fixed point behaviour.
	 */
	return (intmax_t) floor(r);
}

static inline
double
DLB_F_N
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_NACCU
DLB_NminNN
    (DLB_NACCU  a
    ,DLB_NACCU  b
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_NACCU
DLB_NmaxNN
    (DLB_NACCU  a
    ,DLB_NACCU  b
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
int
DLB_IltNN
    (DLB_NACCU  a
    ,DLB_NACCU  b
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int
DLB_IleqNN
    (DLB_NACCU  a
    ,DLB_NACCU  b
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int
DLB_IeqNN
    (DLB_NACCU  a
    ,DLB_NACCU  b
     DLB_XARGS
    )
{
    return a == b;
}

static inline
DLB_NACCU
DLB_NshlNU
    (DLB_NACCU  a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU
DLB_NshrNU
    (DLB_NACCU  a
    ,unsigned   i
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU
DLB_NshlNI
    (DLB_NACCU  a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU
DLB_NshrNI
    (DLB_NACCU  a
    ,int        i
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
unsigned
DLB_UnormN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned
DLB_UpnormN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
DLB_NACCU
DLB_NaddNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_NACCU
DLB_NsubNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_NACCU
DLB_NnegN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_NACCU
DLB_NabsN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_NACCU
DLB_NmpyLL
    (DLB_LFRACT l1
    ,DLB_LFRACT l2
     DLB_XARGS
    )
{
	return l1 * l2;
}

static inline
DLB_NACCU
DLB_NmacNLL
    (DLB_NACCU  a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a + x*y;
}

static inline
DLB_NACCU
DLB_NmsuNLL
    (DLB_NACCU  a
    ,DLB_LFRACT x
    ,DLB_LFRACT y
     DLB_XARGS
    )
{
    return a - x*y;
}

static inline
DLB_NACCU
DLB_N_L
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_NACCU
DLB_N_A
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_NACCU
DLB_N_X
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LtruncN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LsatN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LrndN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_LFRACT
DLB_LsrndN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return _dlb_saturate(a);
}

static inline
DLB_ACCU
DLB_AtruncN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_XACCU
DLB_XtruncN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_NACCU
DLB_NstoreN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}

static inline
DLB_NACCU
DLB_NflushN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return _flush(a);
}

/*
processor-specific implementations of headroom extension intrinsics
******************************************************************************/
static inline
DLB_LFRACT
DLB_LheadLU
    (DLB_LFRACT x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_LFRACT
DLB_L_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a / (1 << hr);
}

static inline
DLB_LFRACT
DLB_LqFU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return DLB_FcqF(a) / (1 << hr);
}

static inline
DLB_ACCU
DLB_A_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a / (1 << hr);
}

static inline
DLB_XACCU
DLB_X_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a / (1 << hr);
}

static inline
double
DLB_F_LU
	(DLB_LFRACT a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1 << hr);
}

static inline
double
DLB_F_AU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1 << hr);
}

static inline
int
DLB_I_LU
	(DLB_LFRACT a
	,unsigned	hr
	 DLB_XARGS
	)
{
	a *= ((uintmax_t)1 << (DLB_NATIVE_FRAC_BITS+hr));
    return (int)_dlb_lclip(llrint(a), -((intmax_t)1 << DLB_NATIVE_FRAC_BITS), ((uintmax_t)1 << DLB_NATIVE_FRAC_BITS)-1);
}

static inline
DLB_LFRACT
DLB_L_IU
	(int		a
	,unsigned	hr
	 DLB_XARGS
	)
{
    return (DLB_LFRACT)a / ((uintmax_t)1 << (DLB_NATIVE_FRAC_BITS+hr));
}

static inline
int
DLB_IintLUU
    (DLB_LFRACT x
    ,unsigned   fp
	,unsigned	hr
     DLB_XARGS
    )
{
    return (int) floor(x * ((intmax_t)1 << (fp+hr)));
}

static inline
DLB_LFRACT
DLB_LmantLUU
	(DLB_LFRACT x
	,unsigned	fp
	,unsigned	hr
	 DLB_XARGS
	)
{
    x *= ((intmax_t)1 << (fp+hr));
    return (x - (DLB_LFRACT) floor(x)) / (1 << hr);
}

static inline
DLB_LFRACT
DLB_LtruncAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_LFRACT
DLB_StruncAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1 << hr);
}

static inline
DLB_SFRACT
DLB_SshlAU
    (DLB_ACCU a
     ,unsigned i
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT
DLB_SsshlAU
    (DLB_ACCU a
     ,unsigned i
     DLB_XARGS
    )
{
    return _dlb_saturate(DLB_FshlFU(a, i DLB_PASS_XARGS));
}

static inline
DLB_LFRACT
DLB_LrndAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_LFRACT
DLB_LsrndAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1 << hr)) / (1 << hr);
}

static inline
DLB_SFRACT
DLB_SrndAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1 << hr);
}

static inline
DLB_SFRACT
DLB_SsrndAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1 << hr));
}

static inline
DLB_LFRACT
DLB_LsatAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1 << hr)) / (1 << hr);
}

static inline
DLB_SFRACT
DLB_SsatAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1 << hr));
}

static inline
DLB_ACCU
DLB_A_LU
	(DLB_LFRACT	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_ACCU
DLB_A_SU
	(DLB_SFRACT a
	,unsigned   hr
	 DLB_XARGS
	)
{
	return a / (1 << hr);
}

static inline
DLB_LFRACT
DLB_L_SU
	(DLB_SFRACT a
	,unsigned   hr
	 DLB_XARGS
	)
{
	return a / (1 << hr);
}

static inline
DLB_LFRACT
DLB_L_32U
	(int32_t	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    return (double) x / (double) ((intmax_t)1 << (31+hr));
}

static inline
DLB_LFRACT
DLB_L_16U
	(int16_t	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    return (double) x / (double) ((intmax_t)1 << (15+hr));
}

static inline
int32_t
DLB_32srndLU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
    x *= ((intmax_t)1 << (31+hr));
    return (int32_t)_dlb_lclip(llrint(x), INT32_MIN, INT32_MAX);
}

static inline
int16_t
DLB_16srndLU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
    x *= ((intmax_t)1 << (15+hr));
    return (int16_t)_dlb_lclip(llrint(x), INT16_MIN, INT16_MAX);
}

static inline
double
DLB_F_XU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1<<hr);
}

static inline
DLB_XACCU
DLB_X_LU
	(DLB_LFRACT a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_XACCU
DLB_X_AU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_ACCU
DLB_ArndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_LFRACT
DLB_LtruncXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_LFRACT
DLB_LsatXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1 << hr)) / (1 << hr);
}

static inline
DLB_LFRACT
DLB_LrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	(void) hr;
	return a;
}

static inline
DLB_LFRACT
DLB_LsrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1<<hr)) / (1<<hr);
}

static inline
DLB_SFRACT
DLB_SrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1<<hr);
}

static inline
DLB_SFRACT
DLB_SsrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(a * (1<<hr));
}

static inline
double
DLB_F_NU
	(DLB_NACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
	return a * (1<<(hr*2));
}

static inline
DLB_ACCU
DLB_AtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return x * (1<<hr);
}

static inline
DLB_XACCU
DLB_XtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return x * (1<<hr);
}

static inline
DLB_LFRACT
DLB_LtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return x * (1<<hr);
}

static inline
DLB_LFRACT
DLB_LsatNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(x * (1<<(2*hr))) / (1<<hr);
}

static inline
DLB_LFRACT
DLB_LrndNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return x * (1<<hr);
}

static inline
DLB_LFRACT
DLB_LsrndNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return _dlb_saturate(x * (1<<(2*hr))) / (1<<hr);
}

static inline
DLB_NACCU
DLB_N_LU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
	return x / (1<<hr);
}

static inline
DLB_LFRACT
DLB_LabsdiffLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a <= b ? b - a : a - b;
}

static inline
DLB_LFRACT
DLB_LclipALL
    (DLB_ACCU   a
    ,DLB_LFRACT lo
    ,DLB_LFRACT hi
     DLB_XARGS
    )
{
    return a < lo ? lo
         : hi < a ? hi
         : a;
}

static inline
DLB_SFRACT
DLB_SclipSSS
    (DLB_SFRACT x
    ,DLB_SFRACT lo
    ,DLB_SFRACT hi
    )
{
    return DLB_SminSS(DLB_SmaxSS(x, lo), hi);
}

static inline
DLB_LFRACT
DLB_LclipLLL
    (DLB_LFRACT x
    ,DLB_LFRACT lo
    ,DLB_LFRACT hi
    )
{
    return DLB_LminLL(DLB_LmaxLL(x, lo), hi);
}

static inline
DLB_ACCU
DLB_AclipAAA
    (DLB_ACCU x
    ,DLB_ACCU lo
    ,DLB_ACCU hi
    )
{
    return DLB_AminAA(DLB_AmaxAA(x, lo), hi);
}

static inline
DLB_LFRACT
DLB_LsrndLU
    (DLB_LFRACT a
    ,unsigned s
     DLB_XARGS
	)
{
    double scale = (double)((intmax_t)1 << s);
    double recip = 1.0/scale;
    DLB_assert(s < DLB_LFRACT_MANTBITS && s < 32
              , "quantize_round"
              , "The number of bits to be kept should be less than min(DLB_LFRACT_MANTBITS,32).");
    /* the min() ensures we clip at one s-scaled LSB below 1.0 */
    return fmin(1.0 - recip, floor(a * scale + 0.5) * recip);
}

static inline
DLB_LFRACT
DLB_LtruncLU
    (DLB_LFRACT a
    ,unsigned s
     DLB_XARGS
    )
{
    double scale = (double)((intmax_t)1 << s);
    DLB_assert(s < DLB_LFRACT_MANTBITS && s < 32
               , "quantize"
               , "The number of bits to be kept should be less than min(DLB_LFRACT_MANTBITS,32).");
    return floor(a * scale) / scale;
}


#ifdef DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS

/* Should do pre- and post-scaling by using LheadLU and LleftLU. */
static inline
DLB_ACCU
DLB_AheadAU
    (DLB_ACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_XACCU
DLB_XheadXU
    (DLB_XACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_NACCU
DLB_NheadNU
    (DLB_NACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_ACCU
DLB_AleftAU
    (DLB_ACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_XACCU
DLB_XleftXU
    (DLB_XACCU  x
    ,unsigned   hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

static inline
DLB_NACCU
DLB_NleftNU
    (DLB_NACCU  x
    ,unsigned   hr
     DLB_XARGS
    )
{
    (void) hr;
    return x;
}

/* DLB_AcF*, DLB_XcF and DLB_NcF* are deprecated ops and will not be supported in future releases */
/* A sample alternative construction would be as follows:
         const DLB_LFRACT Lfoo = DLB_LcF(bar);
         DLB_NACCU Nfoo = DLB_N_L(Lfoo);
*/
#define DLB_AcF(x)          ((double) (x))
#define DLB_XcF(x)          ((double) (x))
#define DLB_NcF(x)          ((double) (x))
#define DLB_AcFU(x, s)      ((double) (x))
#define DLB_XcFU(x,hr)      ((double) (x))
#define DLB_NcFU(x,hr)      ((double) (x))

/* in v1.5 the "op" name changed to reflect the saturation that was actually happneing */
#define DLB_IrndS DLB_IsrndS
#define DLB_IrndL DLB_IsrndL
#endif /* DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS */

/** @} */

#define DLB_METHOD_IS_FLOAT
