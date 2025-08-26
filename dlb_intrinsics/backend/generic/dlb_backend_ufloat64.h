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
 * @section GENERIC_UFLOAT64
 * Abstraction for unscaled processing on double precision floating point architecture.
 * This is an unconstrained backend that uses double-precision floating-point data types,
 * which makes it the most similar to Matlab models. It allows for the creation of test tools
 * and infrastructure that use transforms provided by dlb_dsplib, such as FFT, but which
 * are not constrained to only operate on audio data. The backend can be compiled with 
 * different preprocessor directives which influence the backend behavior.
 * The list of defines can be found at @ref ufloat64_behavior_defines "backend description".
 * @}
 */

/****************************************************************************//**
 * @file
 * @brief  Abstraction for unscaled processing on double precision floating point architecture
 * @{
 * @section ufloat64_behavior_defines Behavior-Altering Preprocessor Directives
 * The following symbols may be defined (perhaps implicitly, by the compiler)
 * to alter behaviour in various ways:
 * 
 * @par DLB_MUST_FLUSH_DENORMS
 * Floating point values less than FLT_MIN can be set to 0.0f for performance reasons by
 * a call to the provided flush operation. If the define is not set these operations return
 * the input value.
 *
 * @}
 */
#include <math.h>   /* for floor(), rint() */
#include <float.h>  /* for DBL_MIN */

/******************************************************************************
sizes of the fractional types
******************************************************************************/
#define DLB_DOUBLE_MANTBITS 53

#define DLB_SFRACT_BITS     64                   /**< Total bits in DLB_SFRACT type (value is backend specific) */
#define DLB_SFRACT_MANTBITS DLB_DOUBLE_MANTBITS  /**< Precision of DLB_SFRACT type (value is backend specific) */

#define DLB_LFRACT_BITS     64                   /**< Total bits in DLB_LFRACT type (value is backend specific) */
#define DLB_LFRACT_MANTBITS DLB_DOUBLE_MANTBITS  /**< Precision of DLB_LFRACT type (value is backend specific) */

#define DLB_ACCU_BITS       64                   /**< Total bits in DLB_ACCU type (value is backend specific) */
#define DLB_ACCU_MANTBITS   DLB_DOUBLE_MANTBITS  /**< Precision of DLB_ACCU type (value is backend specific) */

#define DLB_XACCU_BITS      64                   /**< Total bits in DLB_XACCU type (value is backend specific) */
#define DLB_XACCU_MANTBITS  DLB_DOUBLE_MANTBITS  /**< Precision of DLB_XACCU type (value is backend specific) */

#ifndef DLB_NATIVE_FRAC_BITS
#define DLB_NATIVE_FRAC_BITS 15
#endif

#define DLB_MAX_SHIFT    60

/******************************************************************************
macros for legacy support
******************************************************************************/
#define DLB_L(x) x /**< Get the low part of a word */
#define DLB_H(x) x /**< Get the high part of a word */

/******************************************************************************
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

/**************************************************************************//**
@addtogroup dlb_intrinsics_core
@{
@name Linear Fractional Types
These are the types you use to write Dolby Intrinsics code.
@note The underlying type (double) used below is only an example. Dolby Intrinsics
provides a number of backends that can be used instead. Alternatively, you may create
your own custom underlying backend.
@{
******************************************************************************/
/** eXtended ACCUmulator.
 * Accumulator with extra bits of fractional precision. Use this only where
 * the standard DLB_ACCU type has been shown to have insufficient precision
 * for the given task as the intrinsics that use this type are expensive on
 * many targets.
 *
 * @note The underlying type (double) is only an example. Dolby Intrinsics
 * provides a number of backends that can be used instead. Alternatively, you may create
 * your own custom underlying backend.
 */
typedef double DLB_XACCU;

/** ACCUmulator.
 * Result of multiplying (or multiply-accumulating) a DLB_LFRACT by a
 * DLB_SFRACT. Use this type when calculating the result of IIR filters,
 * FIR filters and other multiply-accumulate dominated code.
 * The number of headroom integer bits depends on the backend configuration. To ensure
 * the correct range of DLB_ACCU for all available backends the data format should be
 * assumed as Q1.31.
 *
 *@note The underlying type (double) is only an example. Dolby Intrinsics
 * provides a number of backends that can be used instead. Alternatively, you may create
 * your own custom underlying backend.
 */
typedef double DLB_ACCU;

/** Long FRACTional.
 * The standard type for audio data.
 *
 * @note The underlying type (double) is only an example. Dolby Intrinsics
 * provides a number of backends that can be used instead. Alternatively, you may create
 * your own custom underlying backend.
 */
typedef double DLB_LFRACT;

/** Short FRACTional.
 * The standard type for coefficients and in special cases where only low
 * precision is required.
 *
 * @note The underlying type (double) is only an example. Dolby Intrinsics
 * provides a number of backends that can be used instead. Alternatively, you may create
 * your own custom underlying backend.
 */
typedef double DLB_SFRACT;

/* @} */

/**************************************************************************//**
@name Constant Generation Macros
These macros are suitable for use in definitions of constant (ROM) tables.
If you're not sure which one to use, prefer the DLB_ScqF macro.
For literal values in functions, use the corresponding intrinsic function,
which will have the same name as the macro, minus the "c" (which stands for
constant).
@{
******************************************************************************/
/** Wrap shift counts for the purpose of scaling into the range [-1,+1) in
 * this macro so that they can be elided on floating point targets.
 * @attention This is experimental and usage may change in future releases.
 */
#define DLB_UscaleU(i)      (0)   /* no scaling (i UNUSED) */

/** Wrap scaling down of floating point constants into the range [-1,+1] in
 * this macro so that they can be elided on floating point targets.
 * @attention This is experimental and usage may change in future releases.
 */
#define DLB_FscaleFU(x,i)   (x) /* no scaling (i UNUSED) */

/** Convert a floating point literal to long fractional.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_L_F.
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_LcF(x)          ((double) (x))

/** Convert a floating point literal to fractional DLB_LFRACT data type with headroom.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_L_FU.
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_LcFU(x, s)      ((double) (x))

/** Convert a integer literal to long fractional with scaling.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x floating point literal in range [INT_MIN,INT_MAX]
 */
#define DLB_LcIU(x, s)      ((double) (x)/((intmax_t)1<<(s)))

/** Quantize a floating point literal to 16 bit (Q1.15) precision and convert
 * to long fractional.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_LcqF(x)         DLB_FcqF(x)

/** Quantize a floating point literal to 16 bit (Q1.15) precision and convert
 * to long fractional with headroom.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_LcqFU(x, s)     DLB_FcqF(x)

/** Convert a floating point literal to short fractional.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_S_F.
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_ScF(x)          ((double) (x))

/** Convert a floating point literal to fractional DLB_SFRACT data type with headroom.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_S_FU.
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_ScFU(x, s)      ((double) (x))

/** Convert a interger literal to short fractional with scaling.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x integer literal in range [INT_MIN,INT_MAX]
 */
#define DLB_ScIU(x, s)      ((double) (x)/((intmax_t)1<<(s)))

/** Quantize a floating point literal to 16 bit (Q1.15) precision and convert
 * to short fractional.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_ScqF(x)         DLB_FcqF(x)

/** Quantize a floating point literal to 16 bit (Q1.15) precision and convert
 * to short fractional with headroom.
 * Use this macro outside of a function (eg. when defining a const table).
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_ScqFU(x, s)     DLB_FcqF(x)

/* @}*/
/* @}*/
/* @}*/

#ifdef DLB_MUST_FLUSH_DENORMS
#define _dlb_flush_eps (DBL_MIN*512)
#define _flush(x) (_dlb_flush_eps < fabs(x) ? x : 0.0)
#else
#define _flush(x) (x)
#endif /* DLB_MUST_FLUSH_DENORMS */

/******************************************************************************
processor-specific implementations of intrinsics
******************************************************************************/
/** @cond  DOXYGEN_EXCLUDED_SECTION */
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
DLB_SFRACT /** @return resulting short fractional value */
DLB_S_F
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_SFRACT /** @return resulting short fractional value */
DLB_SqF
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return DLB_ScqF(f);
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_L_F
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_LqF
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return DLB_LcqF(f);
}

static inline
DLB_ACCU
DLB_A_F
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_XACCU
DLB_X_F
    (double f       /**< floating point literal */
     DLB_XARGS
    )
{
    return f;
}

static inline
DLB_SFRACT /** @return resulting short fractional value */
DLB_S_16
    (int16_t x      /**< 16 bit fixed point value */
     DLB_XARGS
    )
{
    return _dlb_Q15_recip * x;
}

static inline
DLB_SFRACT /** @return resulting short fractional value */
DLB_S_32
    (int32_t x      /**< 32 bit fixed point value */
     DLB_XARGS
    )
{
    return _dlb_Q31_recip * x;
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_L_16
    (int16_t x      /**< 16 bit fixed point value */
     DLB_XARGS
    )
{
    return _dlb_Q15_recip * x;
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_L_32
    (int32_t x     /**< 32 bit fixed point value */
     DLB_XARGS
    )
{
    return _dlb_Q31_recip * x;
}

static inline
double /** @return resulting floating point value */
DLB_F_S
    (DLB_SFRACT x   /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return x;
}

static inline
double /** @return resulting floating point value */
DLB_F_L
    (DLB_LFRACT x   /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return x;
}

static inline
double /** @return resulting floating point value */
DLB_F_A
    (DLB_ACCU x     /**< accumulator to convert */
     DLB_XARGS
    )
{
    return x;
}

static inline
double /** @return resulting floating point value */
DLB_F_X
    (DLB_XACCU x  /**< extended accumulator to convert */
     DLB_XARGS
    )
{
    return x;
}

static inline
int16_t /** @return resulting fixed point 16 bit value */
DLB_16srndS
    (DLB_SFRACT x  /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip(lrint(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int16_t /** @return resulting fixed point 16 bit value */
DLB_16sfloorS
    (DLB_SFRACT x  /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip((long)floor(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int32_t /** @return resulting fixed point 32 bit value */
DLB_32srndS
    (DLB_SFRACT x    /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int32_t)lrint(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
}

static inline
int32_t /** @return resulting fixed point 32 bit value */
DLB_32sfloorS
    (DLB_SFRACT x    /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int32_t)floor(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
}

static inline
int16_t /** @return resulting fixed point 16 bit value */
DLB_16srndL
    (DLB_LFRACT x   /**< long fractional value to convert */
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip(lrint(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int16_t /** @return resulting fixed point 16 bit value */
DLB_16sfloorL
    (DLB_LFRACT x  /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int16_t)_dlb_lclip((long)floor(_dlb_Q15 * x), INT16_MIN, INT16_MAX);
}

static inline
int32_t /** @return resulting fixed point 32 bit value */
DLB_32srndL
    (DLB_LFRACT x   /**< long fractional value to convert */
     DLB_XARGS
    )
{
    return (int32_t)lrint(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
}

static inline
int32_t /** @return resulting fixed point 32 bit value */
DLB_32sfloorL
    (DLB_LFRACT x    /**< short fractional value to convert */
     DLB_XARGS
    )
{
    return (int32_t)floor(_dlb_fclip(_dlb_Q31 * x, (double)INT32_MIN, (double)INT32_MAX));
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
    return (DLB_SFRACT)(i * _dlb_Qnative_recip);
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
    , unsigned fp
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
    return (DLB_LFRACT)(i * _dlb_Qnative_recip);
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
DLB_SFRACT /** @return The closer to -infinity of a and b. */
DLB_SminSS
    (DLB_SFRACT  a    /**< first value to compare */
    ,DLB_SFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_SFRACT /** @return The closer to +infinity of a and b. */
DLB_SmaxSS
    (DLB_SFRACT  a    /**< first value to compare */
    ,DLB_SFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_LFRACT /** @return The closer to -infinity of a and b. */
DLB_LminLL
    (DLB_LFRACT  a    /**< first value to compare */
    ,DLB_LFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_LFRACT /** @return The closer to +infinity of a and b. */
DLB_LmaxLL
    (DLB_LFRACT  a    /**< first value to compare */
    ,DLB_LFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_ACCU /** @return The closer to -infinity of a and b. */
DLB_AminAA
    (DLB_ACCU  a    /**< first value to compare */
    ,DLB_ACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_ACCU /** @return The closer to +infinity of a and b. */
DLB_AmaxAA
    (DLB_ACCU  a    /**< first value to compare */
    ,DLB_ACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
DLB_XACCU /** @return The closer to -infinity of a and b. */
DLB_XminXX
    (DLB_XACCU  a    /**< first value to compare */
    ,DLB_XACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_XACCU /** @return The closer to +infinity of a and b. */
DLB_XmaxXX
    (DLB_XACCU  a    /**< first value to compare */
    ,DLB_XACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
int /** @return non-zero iff a < b */
DLB_IltSS
    (DLB_SFRACT  a    /**< first value to compare */
    ,DLB_SFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int /** @return non-zero iff a <= b */
DLB_IleqSS
    (DLB_SFRACT  a    /**< first value to compare */
    ,DLB_SFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int /** @return non-zero iff a == b */
DLB_IeqSS
    (DLB_SFRACT  a    /**< first value to compare */
    ,DLB_SFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int /** @return non-zero iff a < b */
DLB_IltLL
    (DLB_LFRACT  a    /**< first value to compare */
    ,DLB_LFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int /** @return non-zero iff a <= b */
DLB_IleqLL
    (DLB_LFRACT  a    /**< first value to compare */
    ,DLB_LFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int /** @return non-zero iff a == b */
DLB_IeqLL
    (DLB_LFRACT  a    /**< first value to compare */
    ,DLB_LFRACT  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int /** @return non-zero iff a < b */
DLB_IltAA
    (DLB_ACCU  a    /**< first value to compare */
    ,DLB_ACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int /** @return non-zero iff a <= b */
DLB_IleqAA
    (DLB_ACCU  a    /**< first value to compare */
    ,DLB_ACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int /** @return non-zero iff a == b */
DLB_IeqAA
    (DLB_ACCU  a    /**< first value to compare */
    ,DLB_ACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a == b;
}

static inline
int /** @return non-zero iff a < b */
DLB_IltXX
    (DLB_XACCU  a    /**< first value to compare */
    ,DLB_XACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int /** @return non-zero iff a <= b */
DLB_IleqXX
    (DLB_XACCU  a    /**< first value to compare */
    ,DLB_XACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int /** @return non-zero iff a == b */
DLB_IeqXX
    (DLB_XACCU  a    /**< first value to compare */
    ,DLB_XACCU  b    /**< second value to compare */
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
DLB_SFRACT /** @return shifted output value */
DLB_SshlSU
    (DLB_SFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SsshlSU
    (DLB_SFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SshrSU
    (DLB_SFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SshlSI
    (DLB_SFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SshrSI
    (DLB_SFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SsshlSI
    (DLB_SFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_SFRACT /** @return shifted output value */
DLB_SsshrSI
    (DLB_SFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LshlLU
    (DLB_LFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LsshlLU
    (DLB_LFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LshrLU
    (DLB_LFRACT a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LshlLI
    (DLB_LFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LshrLI
    (DLB_LFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LsshlLI
    (DLB_LFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return shifted output value */
DLB_LsshrLI
    (DLB_LFRACT a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU /** @return shifted output value */
DLB_AshlAU
    (DLB_ACCU   a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU /** @return shifted output value */
DLB_AshrAU
    (DLB_ACCU   a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU /** @return shifted output value */
DLB_AshlAI
    (DLB_ACCU   a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_ACCU /** @return shifted output value */
DLB_AshrAI
    (DLB_ACCU   a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU /** @return shifted output value */
DLB_XshlXU
    (DLB_XACCU  a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU /** @return shifted output value */
DLB_XshrXU
    (DLB_XACCU  a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}


static inline
DLB_XACCU /** @return shifted output value */
DLB_XshlXI
    (DLB_XACCU  a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_XACCU /** @return shifted output value */
DLB_XshrXI
    (DLB_XACCU  a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

#define _dlb_iclipiii(x, lo, hi) \
        (x <= lo ? lo :  hi <= x ? hi : x)

static inline
int /** @return a bit shift count that will work safely with any of the
     *  DLB_S*shlSI and DLB_S*shrSI family of intrinsics.
     */
DLB_IlimsI
    (int        i   /**< bit shift count */
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static inline
int /** @return a bit shift count that will work safely with any of the
     *  DLB_L*shlLI and DLB_L*shrLI family of intrinsics.
     */
DLB_IlimlI
    (int        i   /**< bit shift count */
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static inline
int /** @return a bit shift count that will work safely with any of the
     *  DLB_A*shlAI and DLB_A*shrAI family of intrinsics.
     */
DLB_IlimaI
    (int        i   /**< bit shift count */
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static inline
int /** @return a bit shift count that will work safely with any of the
     *  DLB_N*shlNI and DLB_N*shrNI family of intrinsics.
     */
DLB_IlimnI
    (int        i   /**< bit shift count */
     DLB_XARGS
    )
{
    return _dlb_iclipiii(i, -DLB_MAX_SHIFT, DLB_MAX_SHIFT);
}

static inline
int /** @return a bit shift count that will work safely with any of the
     *  DLB_X*shlXI and DLB_X*shrXI family of intrinsics.
     */
DLB_IlimxI
    (int        i   /**< bit shift count */
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
unsigned /** @return leading sign bits */
DLB_UnormS
    (DLB_SFRACT a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned /** @return leading sign bits */
DLB_UnormL
    (DLB_LFRACT a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned /** @return leading sign bits */
DLB_UnormA
    (DLB_ACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned /** @return leading sign bits */
DLB_UnormX
    (DLB_XACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned /** @return leading zero bits */
DLB_UpnormS
    (DLB_SFRACT a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned /** @return leading zero bits */
DLB_UpnormL
    (DLB_LFRACT a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned /** @return leading zero bits */
DLB_UpnormA
    (DLB_ACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
unsigned /** @return leading zero bits */
DLB_UpnormX
    (DLB_XACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
DLB_SFRACT /** @return a + b */
DLB_SaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_SFRACT /** @return a - b */
DLB_SsubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_SFRACT /** @return a + b */
DLB_SsaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_SFRACT /** @return a - b */
DLB_SssubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_LFRACT /** @return a + b */
DLB_LaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_LFRACT /** @return a - b */
DLB_LsubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_LFRACT /** @return a + b */
DLB_LsaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_LFRACT
DLB_LssubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_LFRACT /** @return a + b */
DLB_LsaddLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_LFRACT /** @return a - b */
DLB_LssubLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_ACCU /** @return a + b */
DLB_AaddAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_ACCU /** @return a - b */
DLB_AsubAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_XACCU /** @return a + b */
DLB_XaddXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_XACCU /** @return a - b */
DLB_XsubXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_SFRACT /** @return -a */
DLB_SnegS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return -a ;
}

static inline
DLB_SFRACT /** @return -a */
DLB_SsnegS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_LFRACT /** @return -a */
DLB_LnegL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_LFRACT /** @return -a */
DLB_LsnegL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_ACCU /** @return -a */
DLB_AnegA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_XACCU /** @return -a */
DLB_XnegX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_SFRACT /** @return |a| */
DLB_SabsS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_SFRACT /** @return |a| */
DLB_SsabsS
    (DLB_SFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT /** @return |a| */
DLB_LabsL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT /** @return |a| */
DLB_LsabsL
    (DLB_LFRACT a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_ACCU /** @return |a| */
DLB_AabsA
    (DLB_ACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_XACCU /** @return |a| */
DLB_XabsX
    (DLB_XACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LmpyLS
    (DLB_LFRACT a   /**< typically audio data */
    ,DLB_SFRACT b   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LsmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LsmpyLS
    (DLB_LFRACT a   /**< typically audio data */
    ,DLB_SFRACT b   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_LFRACT /** @return the product of a and b */
DLB_LsmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_SFRACT /** @return the product of a and b */
DLB_SrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_SFRACT /** @return the product of a and b */
DLB_SsrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_ACCU /** @return the product of a and b */
DLB_AmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_ACCU /** @return the product of l and s */
DLB_AmpyLS
    (DLB_LFRACT l   /**< typically audio data */
    ,DLB_SFRACT s   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return l * s;
}

static inline
DLB_ACCU /** @return the product of a and b */
DLB_AmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    )
{
    return a * b;
}

static inline
DLB_XACCU /** @return the product of l and s */
DLB_XmpyLS
    (DLB_LFRACT l   /**< typically audio data */
    ,DLB_SFRACT s   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return l * s;
}

static inline
DLB_ACCU /** @return the product of x and y */
DLB_AmpyAS
    (DLB_ACCU   x   /**< accumulator */
    ,DLB_SFRACT y   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return x * y;
}

static inline
DLB_XACCU /** @return the product of x and y */
DLB_XmpyXS
    (DLB_XACCU x    /**< accumulator */
    ,DLB_SFRACT y   /**< typically a coefficient */
     DLB_XARGS
    )
{
    return x * y;
}

/* Mixed integer / fractional multiplications */
static inline
DLB_LFRACT /** @return the product of a and i */
DLB_LmpyLI
    (DLB_LFRACT  a    /**< long fractional */
    ,int         i    /**< integer */
     DLB_XARGS
    )
{
    return a * i;
}

static inline
int /** @return the product of i and a */
DLB_ImpyIL
    (int         i    /**< integer */
    ,DLB_LFRACT  a    /**< long fractional */
     DLB_XARGS
    )
{
    return (int)floor(i * a);
}

static inline
int /** @return the product of i and a */
DLB_IrmpyIL
    (int         i    /**< integer */
    ,DLB_LFRACT  a    /**< long fractional */
     DLB_XARGS
    )
{
    return (int)lrint(i * a);
}

static inline
unsigned /** @return the product of i and a */
DLB_UrmpyUL
    (unsigned    i    /**< unsigned integer */
    ,DLB_LFRACT  a    /**< long fractional */
     DLB_XARGS
    )
{
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    return (unsigned)lrint(i * a);
}

static inline
int /** @return the product of i and a */
DLB_ImpyIS
    (int         i    /**< integer */
    ,DLB_SFRACT  a    /**< short fractional */
     DLB_XARGS
    )
{
    return (int)floor(i * a);
}

static inline
int /** @return the product of i and a */
DLB_IrmpyIS
    (int         i    /**< integer */
    ,DLB_SFRACT  a    /**< short fractional */
     DLB_XARGS
    )
{
    return (int)lrint(i * a);
}

static inline
unsigned /** @return the product of i and a */
DLB_UrmpyUS
    (unsigned    i    /**< unsigned integer */
    ,DLB_SFRACT  a    /**< short fractional */
     DLB_XARGS
    )
{
	DLB_assert(0 <= a, "input check", "Negative input for unsigned multiplication.");
    return (unsigned)lrint(i * a);
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LsmacLSS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LsmacLLS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LsmacLLL
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LmacLSS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LmacLLS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a + x*y */
DLB_LmacLLL
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU /** @return a + x*y */
DLB_AmacASS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU /** @return a + x*y */
DLB_AmacALS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU /** @return a + x*y */
DLB_AmacALL
    (DLB_ACCU   a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_ACCU /** @return a + x*y */
DLB_AmacAAS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_ACCU   x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_XACCU /** @return a + x*y */
DLB_XmacXLS
    (DLB_XACCU  a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_XACCU /** @return a + x*y */
DLB_XmacXXS
    (DLB_XACCU  a /**< accumulator */
    ,DLB_XACCU  x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a + x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LsmsuLSS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LsmsuLLS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LsmsuLLL
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LmsuLSS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LmsuLLS
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_LFRACT /** @return a - x*y */
DLB_LmsuLLL
    (DLB_LFRACT a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU /** @return a - x*y */
DLB_AmsuASS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_SFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU /** @return a - x*y */
DLB_AmsuALS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU /** @return a - x*y */
DLB_AmsuALL
    (DLB_ACCU   a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_LFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_ACCU /** @return a - x*y */
DLB_AmsuAAS
    (DLB_ACCU   a /**< accumulator */
    ,DLB_ACCU   x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_XACCU /** @return a - x*y */
DLB_XmsuXLS
    (DLB_XACCU  a /**< accumulator */
    ,DLB_LFRACT x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
     DLB_XARGS
    )
{
    return a - x * y;
}

static inline
DLB_XACCU /** @return a - x*y */
DLB_XmsuXXS
    (DLB_XACCU  a /**< accumulator */
    ,DLB_XACCU  x /**< multiplicand */
    ,DLB_SFRACT y /**< multiplicand */
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
    return a;
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
   return DLB_FshlFU(a, i DLB_PASS_XARGS);
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
    return a;
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
   return DLB_FshlFU(a, i DLB_PASS_XARGS);
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
    return a;
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
    return a;
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
    return a;
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
/** @endcond  DOXYGEN_EXCLUDED_SECTION */


/**************************************************************************//**
@addtogroup dlb_nonlinear
@{
@name Non-Linear Fractional Types
These are the types you use to write non-linear Dolby Intrinsics code.
@note The underlying type (double) used below is only an example. Dolby Intrinsics
provides a number of backends that can be used instead. Alternatively, you may create
your own custom underlying backend.
@{
******************************************************************************/
/** Nonlinear ACCUmulator.
 * Accumulator suitable for storing the result of an LFRACT*LFRACT multiply
 * in high precision.
 * Use this only where the standard DLB_ACCU type has been shown to have
 * insufficient precision for the given task as the intrinsics that use this
 * type are expensive on many targets.
 *
 * @note The underlying type (double) is only an example. Dolby Intrinsics
 * provides a number of backends that can be used instead. Alternatively, you may create
 * your own custom underlying backend.
 */
typedef double DLB_NACCU;

/* @} */
/* @} */

#define DLB_NACCU_BITS      64  /**< Total bits in DLB_NACCU type */
#define DLB_NACCU_MANTBITS  DLB_DOUBLE_MANTBITS /**< Number of mantissa (fractional) bits */


/******************************************************************************
processor-specific implementations of nonlinear extension intrinsics
******************************************************************************/
/** @cond  DOXYGEN_EXCLUDED_SECTION */

static inline
DLB_NACCU
DLB_N_F
    (double f       /**< floating point literal */
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
    (void) hr;
    return a;
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
double /** @return resulting floating point value */
DLB_F_N
    (DLB_NACCU a  /**< nonlinear accumulator to convert */
     DLB_XARGS
    )
{
    return a;
}

static inline
DLB_NACCU /** @return The closer to -infinity of a and b. */
DLB_NminNN
    (DLB_NACCU  a    /**< first value to compare */
    ,DLB_NACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a < b) ? a:b;
}

static inline
DLB_NACCU /** @return The closer to +infinity of a and b. */
DLB_NmaxNN
    (DLB_NACCU  a    /**< first value to compare */
    ,DLB_NACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return (a > b) ? a:b;
}

static inline
int /** @return Non-zero iff a < b */
DLB_IltNN
    (DLB_NACCU  a    /**< first value to compare */
    ,DLB_NACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a < b;
}

static inline
int /** @return Non-zero iff a <= b */
DLB_IleqNN
    (DLB_NACCU  a    /**< first value to compare */
    ,DLB_NACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a <= b;
}

static inline
int /** @return Non-zero iff a == b */
DLB_IeqNN
    (DLB_NACCU  a    /**< first value to compare */
    ,DLB_NACCU  b    /**< second value to compare */
     DLB_XARGS
    )
{
    return a == b;
}

static inline
DLB_NACCU /** @return shifted output value */
DLB_NshlNU
    (DLB_NACCU  a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshlFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU /** @return shifted output value */
DLB_NshrNU
    (DLB_NACCU  a   /**< value to be shifted */
    ,unsigned   i   /**< number of bits to shift by */
     DLB_XARGS
    )
{
    return DLB_FshrFU(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU /** @return shifted output value */
DLB_NshlNI
    (DLB_NACCU  a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshlFI(a, i DLB_PASS_XARGS);
}

static inline
DLB_NACCU /** @return shifted output value */
DLB_NshrNI
    (DLB_NACCU  a   /**< value to be shifted */
    ,int        i   /**< number of bits to shift by (can be < 0) */
     DLB_XARGS
    )
{
    return DLB_FshrFI(a, i DLB_PASS_XARGS);
}

static inline
unsigned /** @return leading sign bits */
DLB_UnormN
    (DLB_NACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UnormF(a);
}

static inline
unsigned /** @return leading zero bits */
DLB_UpnormN
    (DLB_NACCU a /**< value to count */
     DLB_XARGS
    )
{
    return DLB_UpnormF(a);
}

static inline
DLB_NACCU /** @return a + b */
DLB_NaddNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    )
{
    return a + b;
}

static inline
DLB_NACCU /** @return a - b */
DLB_NsubNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    )
{
    return a - b;
}

static inline
DLB_NACCU /** @return -a */
DLB_NnegN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return -a;
}

static inline
DLB_NACCU /** @return abs(a) */
DLB_NabsN
    (DLB_NACCU a
     DLB_XARGS
    )
{
    return (a < 0) ? -a:a;
}

static inline
DLB_NACCU /** @return the product of l1 and l2 */
DLB_NmpyLL
    (DLB_LFRACT l1  /**< typically audio data */
    ,DLB_LFRACT l2  /**< typically more audio data */
     DLB_XARGS
    )
{
    return l1 * l2;
}

static inline
DLB_NACCU /** @return a + x*y */
DLB_NmacNLL
    (DLB_NACCU  a  /**< accumulator */
    ,DLB_LFRACT x  /**< typically audio data */
    ,DLB_LFRACT y  /**< typically more audio data */
     DLB_XARGS
    )
{
    return a + x*y;
}

static inline
DLB_NACCU /** @return a - x*y */
DLB_NmsuNLL
    (DLB_NACCU  a  /**< accumulator */
    ,DLB_LFRACT x  /**< typically audio data */
    ,DLB_LFRACT y  /**< typically more audio data */
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
    return a;
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

/******************************************************************************
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
    (void) hr;
    return a;
}

static inline
DLB_LFRACT
DLB_LqFU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return DLB_FcqF(a);
}

static inline
DLB_ACCU
DLB_A_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_XACCU
DLB_X_FU
	(double		a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
double
DLB_F_LU
	(DLB_LFRACT a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
double
DLB_F_AU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
int
DLB_I_LU
	(DLB_LFRACT a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
	a *= ((uintmax_t)1 << DLB_NATIVE_FRAC_BITS);
	a += (a >= 0) ? +0.5:-0.5;
	if (a >= ((uintmax_t)1 << DLB_NATIVE_FRAC_BITS))
		a = ((uintmax_t)1 << DLB_NATIVE_FRAC_BITS)-1;
	else if (a < -((intmax_t)1 << DLB_NATIVE_FRAC_BITS))
		a = -((intmax_t)1 << DLB_NATIVE_FRAC_BITS);
	return (int) a;
}

static inline
DLB_LFRACT
DLB_L_IU
	(int		a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return (DLB_LFRACT)a / ((uintmax_t)1 << DLB_NATIVE_FRAC_BITS);
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
    (void) hr;
    return (int) floor(x * ((intmax_t)1 << fp));
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
    (void) hr;
    x *= ((intmax_t)1 << fp);
    return x - (DLB_LFRACT) floor(x);
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
    (void) hr;
    return a;
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
    (void) hr;
    return a;
}

static inline
DLB_SFRACT
DLB_SrndAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_SFRACT
DLB_SsrndAU
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
DLB_LsatAU
	(DLB_ACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_SFRACT
DLB_SsatAU
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
    (void) hr;
    return a;
}

static inline
DLB_LFRACT
DLB_L_SU
	(DLB_SFRACT a
	,unsigned   hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_LFRACT
DLB_L_32U
	(int32_t	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return (double) x / (double) ((intmax_t)1 << 31);
}

static inline
DLB_LFRACT
DLB_L_16U
	(int16_t	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return (double) x / (double) ((intmax_t)1 << 15);
}

static inline
int32_t
DLB_32srndLU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    x *= ((intmax_t)1 << 31);
    x += (x >= 0) ? +0.5:-0.5;
    if (x > (((intmax_t)1 << 31)-1))
        return (int32_t) (((intmax_t)1 << 31)-1);
    else if (x < -((intmax_t)1 << 31))
        return (int32_t) -((intmax_t)1 << 31);
    else
        return (int32_t) x;
}

static inline
int16_t
DLB_16srndLU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    x *= ((intmax_t)1 << 15);
    x += (x >= 0) ? +0.5:-0.5;
    if (x > (((intmax_t)1 << 15)-1))
        return (int16_t) (((intmax_t)1 << 15)-1);
    else if (x < -((intmax_t)1 << 15))
        return (int16_t) -((intmax_t)1 << 15);
    else
        return (int16_t) x;
}

static inline
double
DLB_F_XU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
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
    (void) hr;
    return a;
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
    (void) hr;
    return a;
}

static inline
DLB_SFRACT
DLB_SrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_SFRACT
DLB_SsrndXU
	(DLB_XACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
double
DLB_F_NU
	(DLB_NACCU	a
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return a;
}

static inline
DLB_ACCU
DLB_AtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_XACCU
DLB_XtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_LFRACT
DLB_LtruncNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_LFRACT
DLB_LsatNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_LFRACT
DLB_LrndNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_LFRACT
DLB_LsrndNU
	(DLB_NACCU	x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
}

static inline
DLB_NACCU
DLB_N_LU
	(DLB_LFRACT x
	,unsigned	hr
	 DLB_XARGS
	)
{
    (void) hr;
    return x;
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
     DLB_XARGS
    )
{
    return DLB_SminSS(DLB_SmaxSS(x, lo DLB_PASS_XARGS), hi DLB_PASS_XARGS);
}

static inline
DLB_LFRACT
DLB_LclipLLL
    (DLB_LFRACT x
    ,DLB_LFRACT lo
    ,DLB_LFRACT hi
     DLB_XARGS
    )
{
    return DLB_LminLL(DLB_LmaxLL(x, lo DLB_PASS_XARGS), hi DLB_PASS_XARGS);
}

static inline
DLB_ACCU
DLB_AclipAAA
    (DLB_ACCU x
    ,DLB_ACCU lo
    ,DLB_ACCU hi
     DLB_XARGS
    )
{
    return DLB_AminAA(DLB_AmaxAA(x, lo DLB_PASS_XARGS), hi DLB_PASS_XARGS);
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_LsrndLU
    (DLB_LFRACT a   /**< long fractional value to quantise */
    ,unsigned s     /**< the bits to be kept */
     DLB_XARGS
	)
{
    double scale = (double)((intmax_t)1 << s);
    double recip = 1.0/scale;
    DLB_assert(s < DLB_LFRACT_MANTBITS && s < 32
              , "quantize_round"
              , "The number of bits to be kept should be less than min(DLB_LFRACT_MANTBITS, 32).");
    return fmin(1.0 - recip, floor(a * scale + 0.5) * recip);
}

static inline
DLB_LFRACT /** @return resulting long fractional value */
DLB_LtruncLU
    (DLB_LFRACT a   /**< long fractional value to quantise */
    ,unsigned s     /**< the bits to be kept */
     DLB_XARGS
    )
{
    double scale = (double)((intmax_t)1 << s);
    double recip = 1.0/scale;
	DLB_assert(s < DLB_LFRACT_MANTBITS && s < 32
              , "quantize"
              , "The number of bits to be kept should be less than min(DLB_LFRACT_MANTBITS,32).");
	return floor(a * scale) * recip;
}

/** @endcond  DOXYGEN_EXCLUDED_SECTION */


/**************************************************************************//**
@name Constant Generation Macros
@{
******************************************************************************/
#ifdef DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS
/* Should do pre- and post-scaling by using LheadLU and LleftLU. */
static inline
DLB_ACCU
DLB_AleftAU
    (DLB_ACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void)hr;
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
    (void)hr;
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
    (void)hr;
    return x;
}

static inline
DLB_ACCU
DLB_AheadAU
    (DLB_ACCU x
    ,unsigned hr
     DLB_XARGS
    )
{
    (void)hr;
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
    (void)hr;
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
    (void)hr;
    return x;
}

/** DLB_AcF*, DLB_XcF* and DLB_NcF* are deprecated ops and will not be supported in future releases
 *  A sample alternative construction would be as follows:
 *        const DLB_LFRACT Lfoo = DLB_LcF(bar);
 *        DLB_NACCU Nfoo = DLB_N_L(Lfoo);
 */

/** Convert a floating point literal to fractional DLB_ACCU data type.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_A_F. 
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_AcF(x)          ((double) (x))

/** Convert a floating point literal to fractional DLB_ACCU data type with headroom.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_A_FU. 
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1)
 */
#define DLB_AcFU(x, s)      ((double) (x))


/** Convert a floating point literal to fractional DLB_NACCU data type.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_N_F.
 * A sample alternative construction would be as follows:
 *        const DLB_LFRACT Lfoo = DLB_LcF(bar);
 *        DLB_NACCU Nfoo = DLB_N_L(Lfoo);
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1]
 */
#define DLB_NcF(x)          ((double) (x))

/** Convert a floating point literal to fractional DLB_NACCU data type with scaling.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_N_FU. 
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1]
 */
#define DLB_NcFU(x, s)          ((double) (x))

/** Convert a floating point literal to fractional DLB_XACCU data type.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_X_F. 
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1]
 */
#define DLB_XcF(x)          ((double) (x))

/** Convert a floating point literal to fractional DLB_XACCU data type with scaling.
 * Use this macro outside of a function (eg. when defining a const table).
 * The corresponding intrinsic to use inside a function is DLB_X_FU. 
 * @deprecated This is a deprecated operation and will not be supported in future releases. 
 * @param x floating point literal in range [-1,+1]
 */
#define DLB_XcFU(x, s)          ((double) (x))

/**
  @addtogroup dlb_intrinsics_core
  @{
  @name Deprecated Operator Macros
  @{ 
*/
#define DLB_IrndS           DLB_IsrndS              /**< This macro supports calls to the deprecated DLB_IrndS() operator for 
                                                         backward compatibility.
                                                         @deprecated This is a deprecated function name and will not be supported
                                                         in future release. The operator name changed to DLB_IsrndS() to reflect 
                                                         the saturation that was actually happening.*/
#define DLB_IrndL           DLB_IsrndL              /**< This macro supports calls to the deprecated DLB_IrndL() operator for
                                                         backward compatibility.
                                                         @deprecated This is a deprecated function name and will not be supported
                                                         in future release. The operator name changed to DLB_IsrndL() to reflect 
                                                         the saturation that was actually happening.*/                                    
/**
  @}
  @}  
*/
#endif /* DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS */

/* @}*/

/** Indicates that the backend uses floating point arithmetic.
 * This define can be checked in code invoking Dolby Intrinsics to distinguish
 * between floating point and fixed point arithmetic. This is useful e.g. when
 * no unified implementation is available that is suitable for both fixed point
 * and floating point arithmetic.
 */
#define DLB_METHOD_IS_FLOAT
