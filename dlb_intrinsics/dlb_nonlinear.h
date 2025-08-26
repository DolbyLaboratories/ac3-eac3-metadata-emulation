/****************************************************************************
 *
 *
 * Copyright (c) 2012 Dolby International AB.
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
 * @brief Dolby Intrinsics Non-Linear Maths Extension
 */

#ifndef dlb_nonlinear_H
#define dlb_nonlinear_H

#include "dlb_intrinsics.h"

/**************************************************************************//**
@addtogroup dlb_nonlinear
This extension to the Dolby Intrinsics model allows the multiplication
of a DLB_LFRACT with another DLB_LFRACT. The resulting type is a DLB_NACCU.
@{
******************************************************************************/

#ifndef DLB_SKIP_INTRINSIC_PROTOTYPES
/**************************************************************************//**
@name Conversion to DLB Types
@{
******************************************************************************/

/** Convert a floating point literal to a nonlinear accumulator.
 * This should be used for literals within functions and exists for the
 * purpose of specifying a clipping limit where DLB_L_F does not provide
 * enough precision.
 */
DLB_NACCU /** @return Resulting nonlinear accumulator value. */
DLB_N_F
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Convert from a right-aligned fixed point value to a DLB_NACCU.
 * The radix point must be DLB_NACCU_MANTBITS up from the bottom of m.
 */
DLB_NACCU
DLB_N_M /** @return Resulting nonlinear accumulator value. */
    (intmax_t m /**< Right-aligned with DLB_NACCU_MANTBITS fractional bits. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion from DLB Types
@{
******************************************************************************/

/** Convert a nonlinear accumulator to floating point. */
double /** @return Resulting floating point value. */
DLB_F_N
    (DLB_NACCU a  /**< Nonlinear accumulator to convert. */
     DLB_XARGS
    );

/** Convert a nonlinear accumulator into a right-aligned fixed point value.
 * The lowest DLB_NACCU_MANTBITS of the return value are fractional bits.
 */
intmax_t  /** @return Right-aligned fixed point with DLB_NACCU_MANTBITS. */
DLB_M_N
    (DLB_NACCU x    /**< Value to convert. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Minimum/Maximum
@{
******************************************************************************/

/** Find the minimum of two nonlinear accumulator values. */
DLB_NACCU /** @return The closer to -infinity of a and b. */
DLB_NminNN
    (DLB_NACCU  a    /**< First value to compare. */
    ,DLB_NACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the maximum of two nonlinear accumulator values. */
DLB_NACCU /** @return The closer to +infinity of a and b. */
DLB_NmaxNN
    (DLB_NACCU  a    /**< First value to compare. */
    ,DLB_NACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Comparison
Since the Dolby Intrinsics types cannot be assumed to be C numeric types, we
provide operators to compare values. Only comparisons for less than and equal 
are provided. To do a comparison for greater than, reverse the arguments.
@{
******************************************************************************/

/** Check if one nonlinear accumulator value is less than another. */
int /** @return Non-zero iff a < b. */
DLB_IltNN
    (DLB_NACCU  a    /**< First value to compare. */
    ,DLB_NACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Check if one nonlinear accumulator value is less than or equal to another. */
int /** @return Non-zero iff a <= b. */
DLB_IleqNN
    (DLB_NACCU  a    /**< First value to compare. */
    ,DLB_NACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Check if one nonlinear accumulator value is equal to another. */
int /** @return Non-zero iff a == b. */
DLB_IeqNN
    (DLB_NACCU  a    /**< First value to compare. */
    ,DLB_NACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Bit Shifting
@{
******************************************************************************/

/** Arithmetic shift left of a nonlinear accumulator. */
DLB_NACCU /** @return Shifted output value. */
DLB_NshlNU
    (DLB_NACCU  a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift right of a nonlinear accumulator. */
DLB_NACCU /** @return Shifted output value. */
DLB_NshrNU
    (DLB_NACCU  a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of a nonlinear accumulator. */
DLB_NACCU /** @return Shifted output value. */
DLB_NshlNI
    (DLB_NACCU  a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of a nonlinear accumulator. */
DLB_NACCU /** @return Shifted output value. */
DLB_NshrNI
    (DLB_NACCU  a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Bit Shift Limiting
These functions can be wrapped around shift counts to ensure they produce an
actual shift on a given target. For example, on x86, DLB_NshrNI(x, 4) is safe
because x86 can do a shift right by 4 natively. However, DLB_NshrNI(x, 64) will
return x unchanged on an x86. The solution is to wrap the shift count of 64 in the
DLB_IlimnI() operation which will limit it to 62 (risc16x32 back-end): 
DLB_NshrNI(x, DLB_IlimnI(64)). Doing this causes a performance penalty on many 
targets when the shift count is not a constant so the limiting functions should 
be reserved for use on "dangerous" shift-counts only.
@{
******************************************************************************/

/** Limit a shift count for a non-linear accumulator shift to safe values. */
int /** @return A bit shift count that will work safely with any of the
     *  DLB_N*shlNI and DLB_N*shrNI family of intrinsics.
     */
DLB_IlimnI
    (int        i   /**< Bit shift count. */
     DLB_XARGS
    );

/**************************************************************************//**
@name Normalisation
@{
******************************************************************************/

/** Count the number of leading sign bits in the mantissa.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count.
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT. 
 */
unsigned /** @return Number of leading sign bits in the mantissa. */
DLB_UnormN
    (DLB_NACCU a /**< Value to count. */
     DLB_XARGS
    );

/** Count the number of leading zero bits in the mantissa in a known positive value.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. Behaviour for negative 
 * arguments is undefined.
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Number of leading zero bits. */
DLB_UpnormN
    (DLB_NACCU a /**< Value to count. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Addition/Subtraction
@{
******************************************************************************/

/** Nonlinear accumulator addition (backend-dependent saturation
 * behaviour). */
DLB_NACCU /** @return a + b. */
DLB_NaddNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    );

/** Nonlinear accumulator subtraction (backend-dependent saturation
 * behaviour).
 */
DLB_NACCU /** @return a - b. */
DLB_NsubNN
    (DLB_NACCU a
    ,DLB_NACCU b
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Negation
@{
******************************************************************************/

/** Nonlinear accumulator negation (backend dependent saturation behaviour). */
DLB_NACCU /** @return -a. */
DLB_NnegN
    (DLB_NACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Absolute Value
@{
******************************************************************************/

/** Nonlinear accumulator absolute value (saturation behaviour undefined). */
DLB_NACCU /** @return abs(a). */
DLB_NabsN
    (DLB_NACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Multiplication
@{
******************************************************************************/

/** Multiply a long fractional by another long fractional producing a
 * nonlinear accumulator.
 */
DLB_NACCU /** @return The product of l1 and l2. */
DLB_NmpyLL
    (DLB_LFRACT l1  /**< Typically audio data. */
    ,DLB_LFRACT l2  /**< Typically more audio data. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Multiply-Accumulation/Subtraction
@{
******************************************************************************/

/** Multiply-accumulate a long fractional by another long fractional
 * into a nonlinear accumulator.
 */
DLB_NACCU /** @return a + x*y. */
DLB_NmacNLL
    (DLB_NACCU  a  /**< Accumulator. */
    ,DLB_LFRACT x  /**< Typically audio data. */
    ,DLB_LFRACT y  /**< Typically more audio data. */
     DLB_XARGS
    );

/** Multiply-subtract a long fractional by another long fractional
 * into a nonlinear accumulator.
 */
DLB_NACCU /** @return a - x*y. */
DLB_NmsuNLL
    (DLB_NACCU  a  /**< Accumulator. */
    ,DLB_LFRACT x  /**< Typically audio data. */
    ,DLB_LFRACT y  /**< Typically more audio data. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion to Higher Precision
@{
******************************************************************************/

/** LFRACT to NACCU conversion. */
DLB_NACCU
DLB_N_L
    (DLB_LFRACT a
     DLB_XARGS
    );

/** ACCU to NACCU conversion. */
DLB_NACCU
DLB_N_A
    (DLB_ACCU a
     DLB_XARGS
    );

/** XACCU to NACCU conversion. */
DLB_NACCU
DLB_N_X
    (DLB_XACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion to Lower Precision
@{
******************************************************************************/

/** Convert nonlinear accumulator to long fractional using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LtruncN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear accumulator to long fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LsatN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear accumulator to long fractional with rounding.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LrndN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear accumulator to long fractional using the best method
 * available.
 * Saturation and rounding must be performed.
 */
DLB_LFRACT
DLB_LsrndN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear accumulator to long fractional using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_LFRACT
DLB_LrnddN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear to standard accumulator.
 * Typically no saturation will be done here and the accumulator is assumed to
 * have enough headroom bits to store the value.
 * Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_ACCU
DLB_AtruncN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear to standard accumulator using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_ACCU
DLB_ArnddN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear to extended accumulator.
 * Typically no saturation will be done here and the extended accumulator
 * is assumed to have enough headroom bits to store the value.
 * Saturation and rounding may optionally be performed if there is no
 * performance penalty.
 */
DLB_XACCU
DLB_XtruncN
    (DLB_NACCU a
     DLB_XARGS
    );

/** Convert nonlinear to extended accumulator using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_XACCU
DLB_XrnddN
    (DLB_NACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Accumulator Storage
@{
******************************************************************************/

/** Ready a nonlinear accumulator for storage to memory.
 * This optionally saturates to [-1,+1) on processors where extra work would
 * need to be done to store the headroom (guard) bits (such as 56k).
 */
DLB_NACCU
DLB_NstoreN
    (DLB_NACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Denormal Handling
@details
Processors implementing the IEEE 754 floating point standard (such as x87)
must handle very small values using a "denormal" representation. Denormal
values are typically processed very slowly, often using software
assistance rather than pure hardware. We also don't care about values this
small for audio DSP code. We therefore want to flush denormal values to 0
when they occur (typically in an IIR filter that is decaying away to 0).
These intrinsics do this flush on affected floating point targets and are
no-ops elsewhere.
@{
******************************************************************************/

/** Flush denormal values to 0 on floating point processors. */
DLB_NACCU
DLB_NflushN
    (DLB_NACCU a
     DLB_XARGS
    );

/* @} */
/* @} */
#endif /* DLB_SKIP_INTRINSIC_PROTOTYPES */

/******************************************************************************
intrinsics debug macros (to collect file and line information in debug builds).
******************************************************************************/
#if defined DLB_DEBUG_XARGS && !defined DLB_SHOW_XARGS
/* We can define macros with the same names as the functions by virtue of
 * $6.8.3.4 of ISO 9899:1990 ("C90"), which reads in part:
 * "If the name of the macro being replaced is found during this scan of the
 *  replacement list (not including the rest of the source file's preprocessing
 *  tokens), it is not replaced. Further, if any nested replacements encounter
 *  the name of the macro being replaced, it is not replaced. These
 *  nonreplaced macro name preprocessing tokens are no longer available for
 *  further replacement even if they are later (re)examined in contexts in
 *  which that macro name preprocessing token would otherwise have been
 *  replaced."
 */
#   define DLB_N_F(f)              DLB_N_F(f DLB_VALUE_XARGS)
#   define DLB_F_N(n)              DLB_F_N(n DLB_VALUE_XARGS)
#   define DLB_N_M(m)              DLB_N_M(m DLB_VALUE_XARGS)
#   define DLB_M_N(n)              DLB_M_N(n DLB_VALUE_XARGS)
#   define DLB_NminNN(a, b)        DLB_NminNN(a, b DLB_VALUE_XARGS)
#   define DLB_NmaxNN(a, b)        DLB_NmaxNN(a, b DLB_VALUE_XARGS)
#   define DLB_IltNN(a, b)         DLB_IltNN(a, b DLB_VALUE_XARGS)
#   define DLB_IleqNN(a, b)        DLB_IleqNN(a, b DLB_VALUE_XARGS)
#   define DLB_IeqNN(a, b)         DLB_IeqNN(a, b DLB_VALUE_XARGS)
#   define DLB_NshlNU(a, i)        DLB_NshlNU(a, i DLB_VALUE_XARGS)
#   define DLB_NshrNU(a, i)        DLB_NshrNU(a, i DLB_VALUE_XARGS)
#   define DLB_NshlNI(a, i)        DLB_NshlNI(a, i DLB_VALUE_XARGS)
#   define DLB_NshrNI(a, i)        DLB_NshrNI(a, i DLB_VALUE_XARGS)
#   define DLB_IlimnI(i)           DLB_IlimnI(i DLB_VALUE_XARGS)
#   define DLB_UnormN(a)           DLB_UnormN(a DLB_VALUE_XARGS)
#   define DLB_UpnormN(a)          DLB_UpnormN(a DLB_VALUE_XARGS)
#   define DLB_NaddNN(a, b)        DLB_NaddNN(a, b DLB_VALUE_XARGS)
#   define DLB_NsubNN(a, b)        DLB_NsubNN(a, b DLB_VALUE_XARGS)
#   define DLB_NnegN(a)            DLB_NnegN(a DLB_VALUE_XARGS)
#   define DLB_NabsN(a)            DLB_NabsN(a DLB_VALUE_XARGS)
#   define DLB_NmpyLL(x, y)        DLB_NmpyLL(x, y DLB_VALUE_XARGS)
#   define DLB_NmacNLL(a, x, y)    DLB_NmacNLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_NmsuNLL(a, x, y)    DLB_NmsuNLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_N_L(a)              DLB_N_L(a DLB_VALUE_XARGS)
#   define DLB_N_A(a)              DLB_N_A(a DLB_VALUE_XARGS)
#   define DLB_N_X(a)              DLB_N_X(a DLB_VALUE_XARGS)
#   define DLB_LtruncN(a)          DLB_LtruncN(a DLB_VALUE_XARGS)
#   define DLB_LsatN(a)            DLB_LsatN(a DLB_VALUE_XARGS)
#   define DLB_LrndN(a)            DLB_LrndN(a DLB_VALUE_XARGS)
#   define DLB_LsrndN(a)           DLB_LsrndN(a DLB_VALUE_XARGS)
#   define DLB_LrnddN(a)           DLB_LrnddN(a DLB_VALUE_XARGS)
#   define DLB_AtruncN(a)          DLB_AtruncN(a DLB_VALUE_XARGS)
#   define DLB_ArnddN(a)           DLB_ArnddN(a DLB_VALUE_XARGS)
#   define DLB_XtruncN(a)          DLB_XtruncN(a DLB_VALUE_XARGS)
#   define DLB_XrnddN(a)           DLB_XrnddN(a DLB_VALUE_XARGS)
#   define DLB_NstoreN(a)          DLB_NstoreN(a DLB_VALUE_XARGS)
#   define DLB_NflushN(a)          DLB_NflushN(a DLB_VALUE_XARGS)
#endif

#endif
