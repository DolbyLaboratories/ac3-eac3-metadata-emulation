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
 *
 * Dolby Intrinsics Doxygen Module Hierarchy
 *
 * @defgroup dlb_intrinsics_lib Dolby Intrinsics Library
 * @{
 *      @defgroup dlb_intrinsics Dolby Intrinsics Low Level Operations Library
 *      Dolby intrinsics is a DSP abstraction library. It abstracts away differences
 *      between processors and allows the same codebase to target fixed and floating
 *      point processors with various bit depths.
 *      @{
 *          @defgroup dlb_intrinsics_core Intrinsics Core
 *          @{
 *          @}
 *          @defgroup dlb_nonlinear  Non-Linear Extension
 *          @{
 *          @}
 *          @defgroup dlb_headroom   Headroom Extension
 *          @{
 *          @}
 *          @defgroup dlb_util_intops  Integer operations
 *          @{
 *          @}
 *      @}
 *      @defgroup dlb_dsplib Digital Signal Processing Library
 *      The DSP library contains a collection of typical signal-processing routines
 *      that can be often found in DSP processor libraries.
 *      When porting to a specific platform, some portions (esp. FFTs) can be
 *      substituted by a corresponding implementation from the vendor's library.
 *      Alternatively, the generic backend can be used as a starting point for
 *      further Assembly optimizations.
 *      @{
 *          @defgroup dlb_dsplib_math Mathlib API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_blk Blocklib API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_vec Vectorlib API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_compr Dynamic Range Compressor
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_qmf QMF API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_fft FFT API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_dct DCT API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_mdct MDCT API
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_complexvec Complex Vectors
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_complexvec_functions Complex Vector Functions
 *          @{
 *          @}
 *          @defgroup dlb_dsplib_hilbert Hilbert Transform
 *          @{
 *          @} 
 *          @defgroup dlb_dsplib_alignment Memory block alignment API
 *          @{
 *          @}
 *      @}
 *      @defgroup dlb_intrinsics_core_backends Supported Backends
 *      @{
 *          @defgroup dlb_intrinsics_core_generic_backends Generic Backends
 *
 *          These backends provide predefined mappings of data types and arithmetic
 *          operators for different classes of processor (for example, 32- or 64-bit
 *          floating point DSPs). You can choose the generic definition that best matches
 *          your target process and further optimize both data type and arithmetic operator
 *          definitions.
 * 
 *          @defgroup dlb_intrinsics_core_model_backends Model Backends
 *
 *          These backends support the mapping of the Dolby intrinsics data types to the
 *          appropriate data types and data widths on the target platform. Each model back
 *          end calls a set of definitions for arithmetic operators for either floating- or
 *          fixed-point arithmetic. Model back ends allow you to parametrically model a specific
 *          processor so that you can then assess the numerical accuracy with which algorithms 
 *          perform on it. Model back ends are not designed to perform efficiently on a target platform.
 *          @{
 *              @defgroup dlb_method_fixed Fixed Point Arithmetic Model
 *              @{
 *              @}
 *              @defgroup dlb_method_float Floating Point Arithmetic Model
 *              @{
 *              @}
 *			@}	  
 *          @defgroup dlb_intrinsics_core_processor_backends Processor Backends
 *
 *          Each of these backends provides an implementation of the low-level intrinsics optimized
 *          for a specific target processor.
 *      @}
 *      @defgroup dlb_debug Debugging Module
 *      @{
 *      @}
 *      @defgroup dlb_instrument Instrumentation Module
 *      @{
 *          @defgroup dlb_instrument_count Stat Counter Backend
 *          @{
 *          @}
 *          @defgroup dlb_instrument_profiler Profiler Backend
 *          @{
 *          @}
 *      @}
 *      @defgroup dlb_deprecated Dolby Intrinsics Deprecated Operators & Modules
 *      The Dolby Intrinsics Library includes a list of deprecated operators and modules which can be
 *      found as @link deprecated Deprecated List @endlink in the Related Pages. The individual
 *      operators and modules are also marked as deprecated in their detailed description.
 *      @{
 *          @section dlb_deprecation_phases The Deprecation Phases
 *          The deprecation process of operators and modules can be described in three phases:
 *          @par Phase One - Marked as deprecated 
 *          In this phase the regarding operator or module is only marked as deprecated and is @b still @b available @b unless
 *          the preprocessor directive @b DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS is set explicitly.
 *          Regarding code looks like this:
 *          @code
 *          #ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS
 *          <deprecated code>
 *          #endif
 *          @endcode
 *          Customers can set the DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS symbol in this phase to check their client code 
 *          campatibility for upcoming phase two.
 *          @par Phase Two - Being deprecated
 *          In this phase the regarding operator or module is fully deprecated and is @b unavailable @b unless the preprocessor
 *          directive @b DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS is set. This activates all deprecated operators and modules again.
 *          Regarding code looks like this:
 *          @code
 *          #ifdef DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS
 *          <deprecated code>
 *          #endif
 *          @endcode
 *          Customers can set the DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS symbol in this phase to still use deprecated ops in their
 *          client code althought these ones should not be used anymore. This may be for transition use cases. 
 *          Be careful to prepare your client code for the upcoming phase three.
 *          @par Phase Three - Erasement
 *          In this phase the operator or module is not available anymore.
 *          @section dlb_deprecation_process The Deprecation Process
 *          In the Dolby Intrinsics deprecation process every operator or module which should be deprecated passes the three phases 
 *          until it is erased from the code base. The transitions between the individual phases are bound to the official Dolby 
 *          Intrinsics releases (not including maintenance releases).
 *          For example for a release v1.2.x all phase one operators of v1.1.x move to the next phase two. Due to this transition
 *          all preprocessor directives around these operators are changed.
 *          In addition, new operators or modules can enter the phase one and all the ones from former phase two might be fully erased
 *          (phase three). However, in order to reduce the risk of backward compatibility issue, the deprecated functions in phase two
 *          might exist in more than one official Dolby Intrinsics release (e.g. both in v1.2.x and v1.3.x).
 *          @section dlb_deprecation_macro_combinations Valid combinations of those macros
 *          Notes of this table:
 *          - "!exclude" means DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS is not defined.
 *          - "include" means DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS is defined.
 *          - Result column shows which type of deprecated functions is available. 
 *
 *          @htmlonly
 *          <pre>
 *          +------------------------------------------------------+ 
 *          |     combination    | Result          | Valid?        | 
 *          |------------------------------------------------------| 
 *          |!exclude & !include | phase1 (Default)| Yes           | 
 *          |!exclude &  include | phase1 & phase2 | Yes           | 
 *          | exclude & !include | None            | Yes           | 
 *          | exclude &  include | phase2          | No            | 
 *          +------------------------------------------------------+ 
 *          </pre>
 *          @endhtmlonly
 *      @}
 *      @defgroup dlb_legacy Legacy Interfaces
 *      Includes legacy name compatability layer.
 *      @{
 *          @defgroup dlb_legacy_compat Legacy Name Compatibility Layer
 *          @{
 *          @}
 *      @}
 *      @defgroup dlb_test_harness Test Harness
 *      Optional tests to verify intrinsics backend sanity. These test harnesses are
 *      useful when backend modifications are done or a new backend is created and to ensure
 *      nothing is broken. These tests are not mandatory for customers to pass certification.
 *      The complete set of tests can be devided into two fields.
 *
 *      - @b Low @b Level @b Test @b Harness
 *
 *      This test verifies the low level operations of the Dolby Intrinsics Library like basic arithmetic
 *      operations.
 *
 *      - @b High @b Level @b Test @b Harness (Signal Processing Library)
 *
 *      This test verifies the signal processing functions of the Dolby Intrinsics Library like FFT implementations.
 *      For each sub library of the Signal Processing Library exists one dedicated test which can be run as single test or
 *      in the whole set of all high level tests like it is described on the start page of the High Level Test Harness.
 *
 *      @b Run @b the @b Low @b Level @b Test @b Harness
 *
 *      The low level test harness can test each of the supported target backends against the reference implementation of the
 *      low level operations, which can be found in the GENERIC_FLOAT64 (generic floating point double precision) backend.
 *      To compile and run the testharness for all or one specific target backend use the following description:  
 *      @{ 
 *          @defgroup dlb_dsplib_test_harness Signal Processing Library Test Harness
 *          @b Run @b the @b High @b Level @b Test @bHarness
 *
 *          The high level test harness can test each of the supported target backends against the reference implementation of each
 *          module of the signal processing library, which can be found in the GENERIC_FLOAT64 (generic floating point double precision)
 *          backend.
 *          To compile the testharness for all or one specific target backend use the following starting point: 
 *          @code
 *          cd dlb_dsplib/test
 *          make help
 *          @endcode
 *          To run all tests of the test harness use the Python script provided in the test folder
 *          @code
 *          cd dlb_dsplib/test
 *          ./all_tests.py 
 *          @endcode
 *          @{
 *          @}
 *          @defgroup dlb_hexfract The Hexfract Data Format
 *          @{
 *          @}
 *      @}     
 * @}
 */

#ifndef DLB_INTRINSICS_H
#define DLB_INTRINSICS_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* Include all the abstraction details. These are in a separate file so that
 * dlb_intrinsics.h is a clean list of intrinsics declarations.
 */
#include "dlb_abstraction.h"

/* Include all the static constants here. Separate file is maintained so as to
 * keep dlb_intrinsics.h clean and not crash test engine.
 */
#include "dlb_const.h"

/* The utility integer operators feel like intrinsics, and often use "compiler intrinsics"
 * for their implementation, so they should come along for anyone who includes
 * dlb_intrinsics.h, even if they are also useful in code that does not use
 * the other dlb_intriniscs.h operators.
 */
#include "dlb_util_intops.h"

#ifdef DLB_INSTRUMENT_COVERAGE
#include "instrument/coverage/dlb_instrument_coverage.h"
#endif

#ifndef DLB_NO_CHECK_DEPREACATION_COMBINATION
#if defined(DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS) && defined(DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS)
#error "Invalid combination of macros DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS and DLB_INTRINSICS_INCLUDE_DEPRECATED_OPS"
#endif
#endif

#ifndef DLB_SKIP_INTRINSIC_PROTOTYPES
/**************************************************************************//**
@addtogroup dlb_intrinsics_core
The core intrinsics interface.
@{
@section saturation A Note on Saturation
@{ In the Dolby Intrinsics context, saturation refers to a clipping operation, which checks for
 * situations where a signal exceeds the maximum allowable amplitude (which might be caused by
 * rounding errors, for example), and mitigates unwanted effects such as signal wrap-around 
 * by clipping the signal instead. The result is still an error but the effect is likely
 * to be subjectively less severe than wrap-around. Whether or not saturation is implemented depends
 * on the specific backend requirements. Floating-point backends usually omit saturation, while fixed-point backends do not.
 */
/**
@}
@name Conversion into DLB types
@{
******************************************************************************/

/** Convert a floating point literal to a short fractional representation.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope,
 * use DLB_ScF().
 * Some compilers are unable to const-propagate DLB_S_F() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_ScF().
 */
DLB_SFRACT /** @return Resulting short fractional value. */
DLB_S_F
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Quantize a floating point literal to 16 bits and convert to a short
 * fractional representation.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope,
 * use DLB_ScqF().
 * Some compilers are unable to const-propagate DLB_SqF() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_ScqF().
 */
DLB_SFRACT /** @return Resulting short fractional value. */
DLB_SqF
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Convert a floating point literal to a long fractional representation.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope,
 * use DLB_LcF().
 * Some compilers are unable to const-propagate DLB_L_F() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_LcF().
 */
DLB_LFRACT /** @return Resulting long fractional value. */
DLB_L_F
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Quantise a floating point literal to 16 bits and convert to a long
 * fractional representation.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope,
 * use DLB_LcqF().
 * Some compilers are unable to const-propagate DLB_LqF() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_LcqF().
 */
DLB_LFRACT /** @return Resulting long fractional value. */
DLB_LqF
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Quantise a long fractional representation to n bits.
 * This is round-to-nearest form, with saturation (for the rounding operation).
 * The number of bits to be kept should be in [0, DLB_LFRACT_MANTBITS) for all backends except model_dsp24 backends.
 * For model_dsp24 backend, DLB_LFRACT_MANTBITS is 23. This value is needed to support Dolby TrueHD project. 
 * For all the other backends, we don't support DLB_LFRACT_MANTBITS to get better performance for the specific implementations.
 * For s>=DLB_LFRACT_MANTBITS, the behaviour is backend specific, may be overflow(e.g, for backends such as generic_risc16x32, C64 backend).
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation).
 */
DLB_LFRACT /** @return Resulting long fractional value. */
DLB_LsrndLU
    (DLB_LFRACT a       /**< Long fractional value. */
    ,unsigned s         /**< Number of bits to be kept. Range is (0, DLB_LFRACT_MANTBITS). */
     DLB_XARGS
    );

/** Quantise a long fractional representation to n bits.
 * This is round down, towards negative-infinity (floor) form. (Cut off low-n-bits, on arithmetic)
 * The number of bits to be kept should be in [0, DLB_LFRACT_MANTBITS) for all backends except model_dsp24 backends.
 * For model_dsp24 backend, DLB_LFRACT_MANTBITS is 23. This value is needed to support Dolby TrueHD project. 
 * For all the other backends, we don't support DLB_LFRACT_MANTBITS to get better performance for the specific implementations.
 * For s>=DLB_LFRACT_MANTBITS, the behaviour is backend specific, may be overflow(e.g, for backends such as generic_risc16x32, C64 backend).
 */
DLB_LFRACT /** @return Resulting long fractional value. */
DLB_LtruncLU
    (DLB_LFRACT a       /**< Long fractional value. */
    ,unsigned s         /**< Number of bits to be kept. Range is (0, DLB_LFRACT_MANTBITS). */
     DLB_XARGS
    );

/** Quantise a long fractional representation to n bits.
 * This is round down, towards negative-infinity (floor) form. (Cut off low-n-bits, on arithmetic)
 * The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.
 */
DLB_LFRACT /** @return Resuling long fractional value. */
DLB_LrnddLU
    (DLB_LFRACT a       /**< Long fractional value. */
    ,unsigned s         /**< Number of bits to be kept. */
     DLB_XARGS
    );

/** Quantise an extended accumulator representation to n bits of long fractional representation.
 * This is round down, towards negative-infinity (floor) form. (Cut off low-n-bits, on arithmetic)
 * The number of bits to be kept should be no more than DLB_LFRACT_MANTBITS.
 */
DLB_LFRACT /** @return Resuling long fractional value. */
DLB_LrnddXU
    (DLB_XACCU a       /**< Extended accumulator value. */
    ,unsigned s         /**< Number of bits to be kept. */
     DLB_XARGS
    );

/** Convert a floating point literal to an accumulator.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope, use DLB_AcF().
 * DLB_A_F() exists for the purpose of specifying a clipping limit where DLB_L_F() does not provide
 * enough precision.
 * Some compilers are unable to const-propagate DLB_A_F() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_AcF().
 */
DLB_ACCU /** @return Resulting accumulator value. */
DLB_A_F
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Convert a floating point literal to an extended accumulator.
 * This should be used for literals within an expression context inside functions.
 * When declaring a constant expression, both inside and outside of function scope, use DLB_XcF().
 * DLB_X_F() exists for the purpose of specifying a clipping limit where DLB_L_F() does not provide
 * enough precision. Some compilers are unable to const-propagate DLB_X_F() expressions
 * to the extent required, and will result in floating-point arithmetic at run-time.
 * Prefer static const values that are initialised with DLB_XcF().
 */
DLB_XACCU /** @return Resulting extended accumulator value. */
DLB_X_F
    (double f       /**< Floating point literal. */
     DLB_XARGS
    );

/** Convert a 16 bit left justified audio sample to short fractional. */
DLB_SFRACT /** @return Resulting short fractional value. */
DLB_S_16
    (int16_t        /**< 16 bit fixed point value. */
     DLB_XARGS
    );

/** Convert a 32 bit left justified audio sample to short fractional. */
DLB_SFRACT /** @return Resulting short fractional value. */
DLB_S_32
    (int32_t        /**< 32 bit fixed point value. */
     DLB_XARGS
    );

/** Interpret a 16 bit left justified audio sample as Q.15 and convert to long fractional. */
DLB_LFRACT /** @return Resulting long fractional value. */
DLB_L_16
    (int16_t        /**< 16 bit fixed point value. */
     DLB_XARGS
    );

/** Interpret a 32 bit left justified audio sample as Q.31 and convert to long fractional.
 * At most, DLB_LFRACT_MANTBITS of the argument will be used to produce the result.
 * The LSBs are rounded or truncated according to the target platform.*/
DLB_LFRACT /** @return resulting long fractional value. */
DLB_L_32
    (int32_t        /**< 32 bit fixed point value. */
     DLB_XARGS
    );

/** Convert from a right-aligned fixed point value to a DLB_SFRACT.
 * The radix point is must be DLB_SFRACT_MANTBITS up from the bottom of m.
 * Warning: _M converters are intended for internal use by test harnesses,
 * and not intended for use in client code.
 */
DLB_SFRACT /** @return Resulting short fractional value. */
DLB_S_M
    (intmax_t m /**< Right-aligned with DLB_SFRACT_MANTBITS fractional bits. */
     DLB_XARGS
    );

/** Convert from a right-aligned fixed point value to a DLB_LFRACT.
 * The radix point is must be DLB_LFRACT_MANTBITS up from the bottom of m.
 * Warning: _M converters are intended for internal use by test harnesses,
 * and not intended for use in client code.
 */
DLB_LFRACT
DLB_L_M
    (intmax_t m /**< Right-aligned with DLB_LFRACT_MANTBITS fractional bits. */
     DLB_XARGS
    );

/** Convert from a right-aligned fixed point value to a DLB_ACCU.
 * The radix point is must be DLB_ACCU_MANTBITS up from the bottom of m.
 * Warning: _M converters are intended for internal use by test harnesses,
 * and not intended for use in client code.
 */
DLB_ACCU
DLB_A_M
    (intmax_t m /**< Right-aligned with DLB_ACCU_MANTBITS fractional bits. */
     DLB_XARGS
    );

/** Convert from a right-aligned fixed point value to a DLB_XACCU.
 * The radix point is must be DLB_XACCU_MANTBITS up from the bottom of m.
 * Warning: _M converters are intended for internal use by test harnesses,
 * and not intended for use in client code.
 */
DLB_XACCU
DLB_X_M
    (intmax_t m /**< Right-aligned with DLB_XACCU_MANTBITS fractional bits. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion from DLB types
@{
******************************************************************************/

/** Convert a short fractional to floating point.
 * Its primary use is for debugging logs, as floating point operations
 * are extremely costly on fixed-point DSPs. */
double /** @return Resulting floating point value. */
DLB_F_S
    (DLB_SFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert a long fractional to floating point.
 * Its primary use is for debugging logs, as floating point operations
 * are extremely costly on fixed-point DSPs. */
double /** @return Resulting floating point value */
DLB_F_L
    (DLB_LFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert an accumulator to floating point.
 * Its primary use is for debugging logs of the output, as floating point operations
 * are extremely costly on fixed-point DSPs. */
double /** @return Resulting floating point value */
DLB_F_A
    (DLB_ACCU     /**< Accumulator to convert. */
     DLB_XARGS
    );

/** Convert an extended accumulator to floating point.
 * Its primary use is for debugging logs, as floating point operations
 * are extremely costly on fixed-point DSPs. */
double /** @return Resulting floating point value. */
DLB_F_X
    (DLB_XACCU    /**< Extended accumulator to convert. */
     DLB_XARGS
    );

/** Convert a short fractional to a closest 16 bit left justified audio sample after saturation. */
int16_t /** @return Resulting fixed point 16 bit value. */
DLB_16srndS
    (DLB_SFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert a short fractional to the largest 16 bit left justified audio sample less than the
 * argument, after saturation. */
int16_t /** @return Resulting fixed point 16 bit value. */
DLB_16sfloorS
    (DLB_SFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert a short fractional to a closest 32 bit left justified audio sample after saturation. */
int32_t /** @return Resulting fixed point 32 bit value. */
DLB_32srndS
    (DLB_SFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert a short fractional to the largest 32 bit left justified audio sample less than the
 * argument, after saturation. */
int32_t /** @return Resulting fixed point 32 bit value. */
DLB_32sfloorS
    (DLB_SFRACT     /**< Short fractional value to convert. */
     DLB_XARGS
    );

/** Convert a long fractional to a closest 16 bit left justified audio sample after saturation. */
int16_t /** @return Resulting fixed point 16 bit value. */
DLB_16srndL
    (DLB_LFRACT     /**< Long fractional value to convert. */
     DLB_XARGS
    );

/** Convert a long fractional to the largest 16 bit left justified audio sample less than the
 * argument, after saturation. */
int16_t /** @return Resulting fixed point 16 bit value. */
DLB_16sfloorL
    (DLB_LFRACT     /**< Long fractional value to convert. */
     DLB_XARGS
    );

/** Convert a long fractional to a closest 32 bit left justified audio sample after saturation. */
int32_t /** @return Resulting fixed point 32 bit value. */
DLB_32srndL
    (DLB_LFRACT     /**< Long fractional value to convert. */
     DLB_XARGS
    );

/** Convert a long fractional to the largest 32 bit left justified audio sample less than the
 * argument, after saturation. */
int32_t /** @return Resulting fixed point 32 bit value. */
DLB_32sfloorL
    (DLB_LFRACT     /**< Long fractional value to convert. */
     DLB_XARGS
    );

/** Convert a DLB_SFRACT into a right-aligned fixed point value.
 * The lowest DLB_SFRACT_MANTBITS of the return value are fractional bits.
 */
intmax_t /** @return Right-aligned fixed point with DLB_SFRACT_MANTBITS. */
DLB_M_S
    (DLB_SFRACT x   /**< Value to convert. */
     DLB_XARGS
    );

/** Convert a DLB_LFRACT into a right-aligned fixed point value.
 * The lowest DLB_LFRACT_MANTBITS of the return value are fractional bits.
 */
intmax_t /** @return Right-aligned fixed point with DLB_LFRACT_MANTBITS. */
DLB_M_L
    (DLB_LFRACT x   /**< Value to convert. */
     DLB_XARGS
    );

/** Convert a DLB_ACCU into a right-aligned fixed point value.
 * The lowest DLB_ACCU_MANTBITS of the return value are fractional bits.
 */
intmax_t
DLB_M_A /** @return Right-aligned fixed point with DLB_ACCU_MANTBITS. */
    (DLB_ACCU x     /**< Value to convert. */
     DLB_XARGS
    );

/** Convert a DLB_XACCU into a right-aligned fixed point value.
 * The lowest DLB_XACCU_MANTBITS of the return value are fractional bits.
 */
intmax_t
DLB_M_X /** @return Right-aligned fixed point with DLB_XACCU_MANTBITS. */
    (DLB_XACCU x    /**< Value to convert. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Minimum/Maximum
@{
******************************************************************************/

/** Find the minimum of two short fractional values. */
DLB_SFRACT /** @return The closer to -infinity of a and b. */
DLB_SminSS
    (DLB_SFRACT  a    /**< First value to compare. */
    ,DLB_SFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the maximum of two short fractional values. */
DLB_SFRACT /** @return The closer to +infinity of a and b. */
DLB_SmaxSS
    (DLB_SFRACT  a    /**< First value to compare. */
    ,DLB_SFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the minimum of two long fractional values. */
DLB_LFRACT /** @return The closer to -infinity of a and b. */
DLB_LminLL
    (DLB_LFRACT  a    /**< First value to compare. */
    ,DLB_LFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the maximum of two long fractional values. */
DLB_LFRACT /** @return The closer to +infinity of a and b. */
DLB_LmaxLL
    (DLB_LFRACT  a    /**< First value to compare. */
    ,DLB_LFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the minimum of two accumulator values. */
DLB_ACCU /** @return The closer to -infinity of a and b. */
DLB_AminAA
    (DLB_ACCU  a    /**< First value to compare. */
    ,DLB_ACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the maximum of two accumulator values. */
DLB_ACCU /** @return The closer to +infinity of a and b. */
DLB_AmaxAA
    (DLB_ACCU  a    /**< First value to compare. */
    ,DLB_ACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the minimum of two extended accumulator values. */
DLB_XACCU /** @return The closer to -infinity of a and b. */
DLB_XminXX
    (DLB_XACCU  a    /**< First value to compare. */
    ,DLB_XACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Find the maximum of two extended accumulator values. */
DLB_XACCU /** @return The closer to +infinity of a and b. */
DLB_XmaxXX
    (DLB_XACCU  a    /**< First value to compare. */
    ,DLB_XACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Comparison
Since the Dolby Intrinsics types cannot be assumed to be C numeric types, we
provide intrinsics to compare values.
Only comparison for less than and equal are provided. To do comparison for
greater than, reverse the arguments.
@{
******************************************************************************/

/** Is one short fractional value less than another? */
int /** @return Non-zero iff a < b. */
DLB_IltSS
    (DLB_SFRACT  a    /**< First value to compare. */
    ,DLB_SFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one short fractional value less than or equal to another? */
int /** @return Non-zero iff a <= b. */
DLB_IleqSS
    (DLB_SFRACT  a    /**< First value to compare. */
    ,DLB_SFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one short fractional value equal to another? */
int /** @return Non-zero iff a == b. */
DLB_IeqSS
    (DLB_SFRACT  a    /**< First value to compare. */
    ,DLB_SFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one long fractional value less than another? */
int /** @return Non-zero iff a < b. */
DLB_IltLL
    (DLB_LFRACT  a    /**< First value to compare. */
    ,DLB_LFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one long fractional value less than or equal to another? */
int /** @return Non-zero iff a <= b. */
DLB_IleqLL
    (DLB_LFRACT  a    /**< First value to compare. */
    ,DLB_LFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one long fractional value equal to another? */
int /** @return Non-zero iff a == b. */
DLB_IeqLL
    (DLB_LFRACT  a    /**< First value to compare. */
    ,DLB_LFRACT  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one accumulator value less than another? */
int /** @return Non-zero iff a < b. */
DLB_IltAA
    (DLB_ACCU  a    /**< First value to compare. */
    ,DLB_ACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one accumulator value less than or equal to another? */
int /** @return Non-zero iff a <= b. */
DLB_IleqAA
    (DLB_ACCU  a    /**< First value to compare. */
    ,DLB_ACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one accumulator value equal to another? */
int /** @return Non-zero iff a == b. */
DLB_IeqAA
    (DLB_ACCU  a    /**< First value to compare. */
    ,DLB_ACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one extended accumulator value less than another? */
int /** @return Non-zero iff a < b. */
DLB_IltXX
    (DLB_XACCU  a    /**< First value to compare. */
    ,DLB_XACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one extended accumulator value less than or equal to another? */
int /** @return Non-zero iff a <= b. */
DLB_IleqXX
    (DLB_XACCU  a    /**< First value to compare. */
    ,DLB_XACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/** Is one extended accumulator value equal to another? */
int /** @return Non-zero iff a == b. */
DLB_IeqXX
    (DLB_XACCU  a    /**< First value to compare. */
    ,DLB_XACCU  b    /**< Second value to compare. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Bit Shifting
Arithmetic shifts of the fractional data types.
The behaviour of these shift functions is undefined if a shift count is
specified that is larger than the data type in question. This is not a problem
for small shift counts that are guaranteed to be within the number of bits in
the data type. However, to write code that works safely with large variable
shift counts, wrap the shift count in one of the bit shift limiting intrinsics
below. Do this only when necessary as it causes a performance penalty.
@{
******************************************************************************/
/** Arithmetic shift left of a short fractional value. */
DLB_SFRACT /** @return Shifted output value. */
DLB_SshlSU
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift left of a short fractional value with saturation. */
DLB_SFRACT /** @return Shifted output value. */
DLB_SsshlSU
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift right of a short fractional value. */
DLB_SFRACT /** @return Shifted output value. */
DLB_SshrSU
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of a short fractional value. */
DLB_SFRACT /** @return Shifted output value. */
DLB_SshlSI
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of a short fractional value. */
DLB_SFRACT /** @return Shifted output value. */
DLB_SshrSI
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of a short fractional value
 * with saturation.
 */
DLB_SFRACT /** @return Shifted output value. */
DLB_SsshlSI
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of a short fractional value
 * with saturation.
 */
DLB_SFRACT /** @return Shifted output value. */
DLB_SsshrSI
    (DLB_SFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Arithmetic shift left of a long fractional value. */
DLB_LFRACT /** @return Shifted output value. */
DLB_LshlLU
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift left of a long fractional value with saturation. */
DLB_LFRACT /** @return Shifted output value. */
DLB_LsshlLU
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift right of a long fractional value. */
DLB_LFRACT /** @return Shifted output value. */
DLB_LshrLU
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of a long fractional value. */
DLB_LFRACT /** @return Shifted output value. */
DLB_LshlLI
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of a long fractional value. */
DLB_LFRACT /** @return Shifted output value. */
DLB_LshrLI
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of a long fractional value
 * with saturation.
 */
DLB_LFRACT /** @return Shifted output value. */
DLB_LsshlLI
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of a long fractional value
 * with saturation.
 */
DLB_LFRACT /** @return Shifted output value. */
DLB_LsshrLI
    (DLB_LFRACT a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Arithmetic shift left of an accumulator. */
DLB_ACCU /** @return Shifted output value. */
DLB_AshlAU
    (DLB_ACCU   a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift right of an accumulator. */
DLB_ACCU /** @return Shifted output value. */
DLB_AshrAU
    (DLB_ACCU   a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of an accumulator. */
DLB_ACCU /** @return Shifted output value. */
DLB_AshlAI
    (DLB_ACCU   a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of an accumulator. */
DLB_ACCU /** @return Shifted output value. */
DLB_AshrAI
    (DLB_ACCU   a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Arithmetic shift left of an extended accumulator. */
DLB_XACCU /** @return Shifted output value. */
DLB_XshlXU
    (DLB_XACCU  a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Arithmetic shift right of an extended accumulator. */
DLB_XACCU /** @return Shifted output value. */
DLB_XshrXU
    (DLB_XACCU  a   /**< Value to be shifted. */
    ,unsigned   i   /**< Number of bits to shift by. */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift left of an extended accumulator. */
DLB_XACCU /** @return Shifted output value. */
DLB_XshlXI
    (DLB_XACCU  a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/** Bidirectional arithmetic shift right of an extended accumulator. */
DLB_XACCU /** @return Shifted output value. */
DLB_XshrXI
    (DLB_XACCU  a   /**< Value to be shifted. */
    ,int        i   /**< Number of bits to shift by (can be < 0). */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Bit Shift Limiting
These functions can be wrapped around shift counts to ensure they produce an
actual shift on a given target. For example, on x86, DLB_LshrLI(x, 4) is safe
because x86 can do a shift right by 4 natively. However, DLB_LshrLI(x, 32) will
return x unchanged on an x86. The solution is to wrap the shift count 32 in the
DLB_IlimlI() instrinsic which will limit it to 31:
DLB_LshrLI(x, DLB_IlimlI(32)). Doing this causes a performance penalty on many
targets when the shift count is not a constant so the limiting functions
should be reserved for use on "dangerous" shift counts only.
@{
******************************************************************************/

/** Limit a shift count for a short fractional shift to safe values. */
int /** @return A bit shift count that will work safely with any of the
     *  DLB_S*shlSI and DLB_S*shrSI family of intrinsics.
     */
DLB_IlimsI
    (int        i   /**< Bit shift count. */
     DLB_XARGS
    );

/** Limit a shift count for a long fractional shift to safe values. */
int /** @return A bit shift count that will work safely with any of the
     *  DLB_L*shlLI and DLB_L*shrLI family of intrinsics.
     */
DLB_IlimlI
    (int        i   /**< Bit shift count. */
     DLB_XARGS
    );

/** Limit a shift count for an accumulator shift to safe values. */
int /** @return A bit shift count that will work safely with any of the
     *  DLB_A*shlAI and DLB_A*shrAI family of intrinsics.
     */
DLB_IlimaI
    (int        i   /**< Bit shift count. */
     DLB_XARGS
    );

/** Limit a shift count for an extended accumulator shift to safe values. */
int /** @return A bit shift count that will work safely with any of the
     *  DLB_X*shlXI and DLB_X*shrXI family of intrinsics.
     */
DLB_IlimxI
    (int        i   /**< Bit shift count. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Normalisation
@{
******************************************************************************/

/** Count number of leading sign bits.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. 
 * Note that for an equal or close to zero input value the behavior is backend 
 * specific. On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT. On C64/C64plus/Modle_C64/Modle_C64plus/C66_fixed 
 * backends for a = 0 it would return DLB_LFRACT_MANTBITS.
 */
unsigned /** @return Leading sign bits. */
DLB_UnormS
    (DLB_SFRACT a /**< Value to count. */
     DLB_XARGS
    );

/** Count number of leading sign bits.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. 
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading sign bits. */
DLB_UnormL
    (DLB_LFRACT a /**< Value to count. */
     DLB_XARGS
    );

/** Count number of leading sign bits.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. 
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading sign bits. */
DLB_UnormA
    (DLB_ACCU a /**< Value to count. */
     DLB_XARGS
    );

/** Count number of leading sign bits.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. 
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading sign bits. */
DLB_UnormX
    (DLB_XACCU a /**< Value to count. */
     DLB_XARGS
    );

/** Count leading zero bits in a known positive value.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. Behaviour for negative 
 * arguments is undefined.
 * Note that for an equal or close to zero input value the behavior is backend 
 * specific. On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT. On C64/C64plus/Modle_C64/Modle_C64plus/C66_fixed 
 * backends for a = 0 it would return DLB_LFRACT_MANTBITS.
 */
unsigned /** @return Leading zero bits. */
DLB_UpnormS
    (DLB_SFRACT a /**< Value to count. */
     DLB_XARGS
    );

/** Count leading zero bits in a known positive value.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. Behaviour for negative 
 * arguments is undefined.
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading zero bits. */
DLB_UpnormL
    (DLB_LFRACT a /**< Value to count. */
     DLB_XARGS
    );

/** Count leading zero bits in a known positive value.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. Behaviour for negative 
 * arguments is undefined.
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading zero bits. */
DLB_UpnormA
    (DLB_ACCU a /**< Value to count. */
     DLB_XARGS
    );

/** Count leading zero bits in a known positive value.
 * The result is a number which can be passed to a shift left operation to bring 
 * the absolute value into the range [0.5, 1]. If no such number exists, then the 
 * result will be the largest possible shift count. Behaviour for negative 
 * arguments is undefined.
 * Note that for a close to zero input value the behavior is backend specific. 
 * On floating-point backends for a < 1.0 / ((intmax_t)1<<DLB_MAX_SHIFT) 
 * it would return DLB_MAX_SHIFT.
 */
unsigned /** @return Leading zero bits. */
DLB_UpnormX
    (DLB_XACCU a /**< Value to count. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Addition/Subtraction
@{
******************************************************************************/

/** Short addition (saturation behaviour undefined). */
DLB_SFRACT /** @return a + b. */
DLB_SaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Short subtraction (saturation behaviour undefined). */
DLB_SFRACT /** @return a - b. */
DLB_SsubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Short saturating addition. */
DLB_SFRACT /** @return a + b. */
DLB_SsaddSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Short saturating subtraction. */
DLB_SFRACT /** @return a - b */
DLB_SssubSS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Long addition (saturation behaviour undefined). */
DLB_LFRACT /** @return a + b. */
DLB_LaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Long subtraction (saturation behaviour undefined). */
DLB_LFRACT /** @return a - b. */
DLB_LsubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Find the absolute value of the difference between a and b. */
DLB_LFRACT /** @return abs(a - b). */
DLB_LabsdiffLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Long saturating addition.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a + b. */
DLB_LsaddLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Long saturating subtraction.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT
DLB_LssubLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Mixed saturating addition.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a + b. */
DLB_LsaddLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Mixed saturating subtraction.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a - b. */
DLB_LssubLS
    (DLB_LFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Accumulator addition (backend-dependent saturation behaviour). */
DLB_ACCU /** @return a + b. */
DLB_AaddAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    );

/** Accumulator subtraction (backend-dependent saturation behaviour). */
DLB_ACCU /** @return a - b. */
DLB_AsubAA
    (DLB_ACCU a
    ,DLB_ACCU b
     DLB_XARGS
    );

/** Extended accumulator addition (backend-dependent saturation behaviour). */
DLB_XACCU /** @return a + b. */
DLB_XaddXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    );

/** Extended accumulator subtraction (backend-dependent saturation
 * behaviour).
 */
DLB_XACCU /** @return a - b. */
DLB_XsubXX
    (DLB_XACCU a
    ,DLB_XACCU b
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Negation
@{
******************************************************************************/

/** Short negation (saturation behaviour undefined) */
DLB_SFRACT /** @return -a. */
DLB_SnegS
    (DLB_SFRACT a
     DLB_XARGS
    );

/** Short saturating negation. */
DLB_SFRACT /** @return -a. */
DLB_SsnegS
    (DLB_SFRACT a
     DLB_XARGS
    );

/** Long negation (saturation behaviour undefined). */
DLB_LFRACT /** @return -a. */
DLB_LnegL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Long saturating negation. */
DLB_LFRACT /** @return -a. */
DLB_LsnegL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Accumulator negation (backend dependent saturation behaviour). */
DLB_ACCU /** @return -a. */
DLB_AnegA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Extended accumulator negation (backend dependent saturation behaviour). */
DLB_XACCU /** @return -a. */
DLB_XnegX
    (DLB_XACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Absolute Value
@{
******************************************************************************/

/** Short absolute value (saturation behaviour undefined). */
DLB_SFRACT /** @return |a|. */
DLB_SabsS
    (DLB_SFRACT a
     DLB_XARGS
    );

/** Short saturating absolute value. */
DLB_SFRACT /** @return |a|. */
DLB_SsabsS
    (DLB_SFRACT a
     DLB_XARGS
    );

/** Long absolute value (saturation behaviour undefined). */
DLB_LFRACT /** @return |a|. */
DLB_LabsL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Long saturating absolute value. */
DLB_LFRACT /** @return |a|. */
DLB_LsabsL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Accumulator absolute value (saturation behaviour undefined). */
DLB_ACCU /** @return |a|. */
DLB_AabsA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Extended accumulator absolute value (saturation behaviour undefined). */
DLB_XACCU /** @return |a|. */
DLB_XabsX
    (DLB_XACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Multiplication
@{
******************************************************************************/

/** Multiply a short fractional by a short fractional and produce a long
 * fractional.
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Multiply a long fractional by a short fractional producing a long fract.
 * Typically the long fractional value is audio data and the short fractional
 * value is a coefficient, but other uses are possible.
 * Use this form for single multiplies that are not part of a sequence of
 * multiply-accumulates.
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LmpyLS
    (DLB_LFRACT a   /**< Typically audio data. */
    ,DLB_SFRACT b   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply a long fractional by a long fractional producing a long
 * fractional.
 * Please note that -1 x -1 is not the only combination of arguments that can 
 * result in overflow on some backends.
 * Client code should use DLB_LsmpyLL if wrap-around may occur.
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Multiply a short fractional by a short fractional and produce a long
 * fractional with saturation for the -1*-1 case.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation).
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LsmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Multiply a long fractional by a short fractional producing a long
 * fractional with saturation for the -1*-1 case.
 * Typically the long fractional value is audio data and the short fractional
 * value is a coefficient, but other uses are possible.
 * Use this form for single multiplies that are not part of a sequence of
 * multiply-accumulates.
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LsmpyLS
    (DLB_LFRACT a   /**< Typically audio data. */
    ,DLB_SFRACT b   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply a long fractional by a long fractional producing a long
 * fractional with saturation for the -1*-1 case.
 */
DLB_LFRACT /** @return The product of @a a and @a b. */
DLB_LsmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Multiply two short fractionals without saturation for the -1*-1 case and
 * round the result to a short fractional.
 */
DLB_SFRACT /** @return The product of @a a and @a b. */
DLB_SrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Multiply two short fractionals with saturation for the -1*-1 case and
 * round the result to a short fractional.
 */
DLB_SFRACT /** @return The product of @a a and @a b. */
DLB_SsrmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Multiply a long fractional by a short fractional producing an accumulator.
 * Typically the long fractional value is audio data and the short fractional
 * value is a coefficient, but other uses are possible.
 * This is the standard DSP multiply that should be used whenever you plan to
 * subsequently multiply-accumulate values with the result.
 */
DLB_ACCU /** @return The product of @a l and @a s. */
DLB_AmpyLS
    (DLB_LFRACT l   /**< Typically audio data. */
    ,DLB_SFRACT s   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply a short fractional by a short fractional producing an accumulator.
 *  Since short fractional values are typically too short for audio data, this
 *  operation is mostly useful for control-path calculations.  It may be faster
 *  than DLB_AmpyLS on some target platforms, but that is rare.
 */
DLB_ACCU /** @return The product of @a a and @a b. */
DLB_AmpySS
    (DLB_SFRACT a
    ,DLB_SFRACT b
     DLB_XARGS
    );

/** Multiply a long fractional by a long fractional producing an accumulator.
 * Typically one long fractional value is audio data and the second long fractional
 * value is a coefficient, but other uses are possible.
 * This is not the standard DSP multiply and should be avoided whenever you can use
 * short-fractional coefficient precision.
 */
DLB_ACCU /** @return The product of @a a and @a b. */
DLB_AmpyLL
    (DLB_LFRACT a
    ,DLB_LFRACT b
     DLB_XARGS
    );

/** Multiply a long fractional by a short fractional producing an extended
 * accumulator.
 * Typically the long fractional value is audio data and the short fractional
 * value is a coefficient, but other uses are possible.
 */
DLB_XACCU /** @return The product of @a l and @a s. */
DLB_XmpyLS
    (DLB_LFRACT l   /**< Typically audio data. */
    ,DLB_SFRACT s   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply an accumulator by a short fractional and produce an accumulator.
 * This is typically implemented as a double precision multiply on a DSP.
 */
DLB_ACCU /** @return the product of @a x and @a y. */
DLB_AmpyAS
    (DLB_ACCU   x   /**< Accumulator. */
    ,DLB_SFRACT y   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply an extended accumulator by a short fractional and produce an
 * extended accumulator.
 * This is typically implemented as a double precision multiply on a DSP.
 */
DLB_XACCU /** @return the product of @a x and @a y. */
DLB_XmpyXS
    (DLB_XACCU x    /**< Accumulator. */
    ,DLB_SFRACT y   /**< Typically a coefficient. */
     DLB_XARGS
    );

/** Multiply a long fractional by an integer and produce a
 * long fractional. Don't care for clipping or rounding.
 * This is a mixed integer / fractional multiplication.
 */
DLB_LFRACT /** @return The product of @a x and @a i. */
DLB_LmpyLI
    (DLB_LFRACT x    /**< Long fractional. */
    ,int i           /**< Integer. */
     DLB_XARGS
    );

/** Multiply an integer by a long fractional and produce an
 * integer. The output integer is rounded to negative infinity (floor).
 * This is a mixed integer / fractional multiplication.
 */
int /** @return The product of @a i and @a x. */
DLB_ImpyIL
    (int i           /**< Integer. */
    ,DLB_LFRACT x    /**< Long fractional. */
     DLB_XARGS
    );

/** Multiply an integer by a long fractional and produce an
 * integer. The output integer is rounded to nearest (or nearest-even if that is faster).
 * This is a mixed integer / fractional multiplication.
 */
int /** @return The product of @a i and @a x. */
DLB_IrmpyIL
    (int i           /**< Integer. */
    ,DLB_LFRACT x    /**< Long fractional. */
     DLB_XARGS
    );

/** Multiply an unsigned integer by a long fractional and produce an
 * unsigned integer with rounding. The output integer is rounded to nearest (or nearest-even).
 * This is a mixed integer / fractional unsigned multiplication.
 */
unsigned /** @return The product of @a i and @a x. */
DLB_UrmpyUL
    (unsigned i      /**< Integer. */
    ,DLB_LFRACT x    /**< Long fractional. */
     DLB_XARGS
    );

/** Multiply an integer by a short fractional and produce an
 * integer. The output integer is rounded to negative infinity (floor).
 * This is a mixed integer / fractional multiplication.
 */
int /** @return The product of @a i and @a x. */
DLB_ImpyIS
    (int i           /**< Integer. */
    ,DLB_SFRACT x    /**< Short fractional. */
     DLB_XARGS
    );

/** Multiply an integer by a short fractional and produce an
 * integer. The output integer is rounded to nearest (or nearest-even if faster).
 * This is a mixed integer / fractional multiplication.
 */
int /** @return The product of @a i and @a x. */
DLB_IrmpyIS
    (int i           /**< Integer. */
    ,DLB_SFRACT x    /**< Short fractional. */
     DLB_XARGS
    );

/** Multiply an unsigned integer by a short fractional and produce an
 * unsigned integer with rounding. The output integer is rounded to nearest (or nearest-even).
 * This is a mixed integer / fractional unsigned multiplication.
 */
unsigned /** @return The product of @a x and @a y. */
DLB_UrmpyUS
    (unsigned i      /**< Integer. */
    ,DLB_SFRACT x    /**< Short fractional. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Multiply-Accumulation/Subtraction
@{
******************************************************************************/

/** Multiply accumulate LFRACT += SFRACT*SFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a + x*y */
DLB_LsmacLSS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate LFRACT += LFRACT*SFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a + x*y. */
DLB_LsmacLLS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate LFRACT += LFRACT*LFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a + x*y. */
DLB_LsmacLLL
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate LFRACT += SFRACT*SFRACT. */
DLB_LFRACT /** @return a + x*y. */
DLB_LmacLSS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate LFRACT += LFRACT*SFRACT. */
DLB_LFRACT /** @return a + x*y. */
DLB_LmacLLS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate LFRACT += LFRACT*LFRACT. */
DLB_LFRACT /** @return a + x*y. */
DLB_LmacLLL
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate ACCU += SFRACT*SFRACT.
 */
DLB_ACCU /** @return a + x*y. */
DLB_AmacASS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate ACCU += LFRACT*SFRACT.
 * This is the standard DSP multiply accumulate that should be used in new
 * code. Using this instead of LsmacLS saves extra bit shifts on backends
 * that lack a dedicated fractional multiply unit.
 */
DLB_ACCU /** @return a + x*y. */
DLB_AmacALS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate ACCU += LFRACT*LFRACT.
 */
DLB_ACCU /** @return a + x*y. */
DLB_AmacALL
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate ACCU += ACCU*SFRACT.
 */
DLB_ACCU /** @return a + x*y. */
DLB_AmacAAS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_ACCU   x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Extended precision multiply accumulate XACCU += LFRACT*SFRACT. */
DLB_XACCU /** @return a + x*y. */
DLB_XmacXLS
    (DLB_XACCU  a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply accumulate XACCU += XACCU*SFRACT.
 * This is typically implemented as a double-precision multiply.
 */
DLB_XACCU /** @return a + x*y. */
DLB_XmacXXS
    (DLB_XACCU  a /**< Accumulator. */
    ,DLB_XACCU  x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= SFRACT*SFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a - x*y. */
DLB_LsmsuLSS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= LFRACT*SFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a - x*y. */
DLB_LsmsuLLS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= LFRACT*LFRACT with saturation.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation). */
DLB_LFRACT /** @return a - x*y. */
DLB_LsmsuLLL
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= SFRACT*SFRACT. */
DLB_LFRACT /** @return a - x*y. */
DLB_LmsuLSS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= LFRACT*SFRACT. */
DLB_LFRACT /** @return a - x*y. */
DLB_LmsuLLS
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand */
     DLB_XARGS
    );

/** Multiply subtract LFRACT -= LFRACT*LFRACT. */
DLB_LFRACT /** @return a - x*y. */
DLB_LmsuLLL
    (DLB_LFRACT a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract ACCU -= SFRACT*SFRACT.
 */
DLB_ACCU /** @return a - x*y. */
DLB_AmsuASS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_SFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract ACCU -= LFRACT*SFRACT.
 * This is the standard DSP multiply subtract that should be used in new
 * code. Using this instead of LsmsuLS saves extra bit shifts on backends
 * that lack a dedicated fractional multiply unit.
 */
DLB_ACCU /** @return a - x*y. */
DLB_AmsuALS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract ACCU -= LFRACT*LFRACT.
 */
DLB_ACCU /** @return a - x*y. */
DLB_AmsuALL
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_LFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract ACCU -= ACCU*SFRACT.
 * This is typically implemented as a double-precision multiply.
 */
DLB_ACCU /** @return a - x*y. */
DLB_AmsuAAS
    (DLB_ACCU   a /**< Accumulator. */
    ,DLB_ACCU   x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Extended precision multiply subtract XACCU -= LFRACT*SFRACT. */
DLB_XACCU /** @return a - x*y. */
DLB_XmsuXLS
    (DLB_XACCU  a /**< Accumulator. */
    ,DLB_LFRACT x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/** Multiply subtract XACCU -= XACCU*SFRACT.
 * This is typically implemented as a double-precision multiply.
 */
DLB_XACCU /** @return a - x*y. */
DLB_XmsuXXS
    (DLB_XACCU  a /**< Accumulator. */
    ,DLB_XACCU  x /**< Multiplicand. */
    ,DLB_SFRACT y /**< Multiplicand. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion to Higher Precision
@{
******************************************************************************/

/** SFRACT to XACCU conversion. */
DLB_XACCU
DLB_X_S
    (DLB_SFRACT a
     DLB_XARGS
    );

/** SFRACT to ACCU conversion. */
DLB_ACCU
DLB_A_S
    (DLB_SFRACT a
     DLB_XARGS
    );

/** SFRACT to LFRACT conversion. */
DLB_LFRACT
DLB_L_S
    (DLB_SFRACT a
     DLB_XARGS
    );

/** LFRACT to XACCU conversion. */
DLB_XACCU
DLB_X_L
    (DLB_LFRACT a
     DLB_XARGS
    );

/** LFRACT to ACCU conversion. */
DLB_ACCU
DLB_A_L
    (DLB_LFRACT a
     DLB_XARGS
    );

/** ACCU to XACCU conversion. */
DLB_XACCU
DLB_X_A
    (DLB_ACCU a
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Fixed Point Control Interface
These functions support a standardized Qx.15 integer control interface across
any backends.
@{
******************************************************************************/

/** Convert a short fractional into a Qx.15 int.
 *  Round down toward -ve infinity (or truncate, whichever is fastest). */
int
DLB_I_S
    (DLB_SFRACT s
     DLB_XARGS
    );

/** Convert a short fractional into a Qx.15 int.
 *  Round to nearest (or nearest-even). */
int
DLB_IsrndS
    (DLB_SFRACT s
     DLB_XARGS
    );

/** Convert a Qx.15 int into a short fractional. */
DLB_SFRACT
DLB_S_I
    (int        i
     DLB_XARGS
    );

/** Convert a long fractional into a Qx.15 int.
 *  Round down to -ve infinity (or truncate, whichever is fastest). */
int
DLB_I_L
    (DLB_LFRACT l
     DLB_XARGS
    );

/** Convert a long fractional into a Qx.15 int.
 *  Round to nearest (or nearest-even). */
int
DLB_IsrndL
    (DLB_LFRACT s
     DLB_XARGS
    );

/** Convert a Qx.15 int into a long fractional. */
DLB_LFRACT
DLB_L_I
    (int        i
     DLB_XARGS
    );

/** Return the integer part of what you would get if you could scale the
 * long fractional argument up by fp bits. This operation quantises to fp bits by rounding down.
 * This is useful for treating a fractional value as having a fractional
 * point further down the word than usual and getting hold of the bits
 * above this fractional point.
 */
int /** @return Upper fp bits of a as an integer. */
DLB_IintLU
    (DLB_LFRACT a
    ,unsigned   fp /**< Location of fractional point from top. */
     DLB_XARGS
    );

/** Quantise a long fractional representation to (s+1) bits, returning the
 * result as the (s+1) LSBs of an int.
 * This is round-to-nearest form, with saturation (for the rounding operation).
 * The number of bits to be kept should be in (0, DLB_LFRACT_MANTBITS).
 * For s>31 or s=0, the behaviour is backend specific, maybe overflow.
 *
 * This can be thought of as returning the integer part of what you would get
 * if you could scale the long fractional argument up by s bits and round
 * the result.  This is useful for quantisation, and is the integer result
 * equivalent of DLB_LsrndLU (which quantises within an LFRACT).
 * Alternatively, think of DLB_IsrndL as a special case of DLB_IsrndLU where
 * the fp argument is always 15.
 *
 * Note that this function does not restrict the range of the resulting
 * integer to 16 bits, so care must be taken on short-int targets.
 */
int /** @return Upper (s+1) bits of a (considered as a 2's compliment,
     *          left-aligned fraction) as an integer. */
DLB_IsrndLU
    (DLB_LFRACT a
    ,unsigned   s /**< log_2 of scale factor applied to a before rounding to int. */
     DLB_XARGS
    );

/** Return the fractional part of what you would get if your could scale the
 * long fractional argument up by fp bits.
 * This is useful for treating a fractional value as having a fractional
 * point further down the word than usual and getting hold of the bits
 * below this fractional point as a left-aligned fractional value.
 */
DLB_LFRACT /** @return Bits below fp in x. */
DLB_LmantLU
    (DLB_LFRACT x
    ,unsigned   fp /**< Location of fractional point from top. */
     DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Conversion to Lower Precision
@{
******************************************************************************/

/** Convert long to short fractional using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the long is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_StruncL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Convert long to short fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SsatL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Convert long to short fractional with rounding.
 * Typically no saturation will be done here and the long is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SrndL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Convert long to short fractional using the best method available.
 * Saturation and rounding must be performed.
 */
DLB_SFRACT
DLB_SsrndL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Convert long to short fractional using method of round down(i.e. 
 * round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_SFRACT
DLB_SrnddL
    (DLB_LFRACT a
    DLB_XARGS
    );

/** Convert an accumulator to short fractional using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_StruncA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to short fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SsatA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to short fractional with rounding.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SrndA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to short fractional using the best method available.
 * Saturation and rounding must be performed.
 */
DLB_SFRACT
DLB_SsrndA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert accumulator to short fractional using method of round down(i.e. 
 * round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_SFRACT
DLB_SrnddA
    (DLB_ACCU a
    DLB_XARGS
    );

/** Convert an accumulator to long fractional using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LtruncA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to long fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LsatA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to long fractional with rounding.
 * Typically no saturation will be done here and the accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LrndA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert an accumulator to long fractional using the best method available.
 * Will saturate if the result would otherwise wrap around, and will round if possible and necessary.
 */
DLB_LFRACT
DLB_LsrndA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Convert accumulator to long fractional using method of round down(i.e. 
 * round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_LFRACT
DLB_LrnddA
    (DLB_ACCU a
    DLB_XARGS
    );

/** Convert an accumulator to long fractional with an additional pre right shift.
 *  This is equivalent to DLB_LtruncA(DLB_AshrAU(a,i)). For RISC platforms
 *  the loss of the LSB can be avoided due to the accumulator 
 *  architecture.
 */
DLB_LFRACT
DLB_LshrAU
    (DLB_ACCU a
    ,unsigned i   
     DLB_XARGS
    );

/** Convert an accumulator to long fractional with an additional pre left shift.
 *  This is equivalent to DLB_LtruncA(DLB_AshlAU(a,i)).
 */
DLB_LFRACT
DLB_LshlAU
    (DLB_ACCU a
    ,unsigned i   
     DLB_XARGS
    );

/** Convert an accumulator to long fractional with an additional pre left shift.
 *  and saturate.
 */
DLB_LFRACT
DLB_LsshlAU
    (DLB_ACCU a
    ,unsigned i   
     DLB_XARGS
    );

/** Convert an accumulator to short fractional with an additional pre left shift.
 *  This is equivalent to DLB_StruncA(DLB_AshlAU(a,i)).
 */
DLB_SFRACT
DLB_SshlAU
    (DLB_ACCU a
    ,unsigned i   
     DLB_XARGS
    );

/** Convert an accumulator to short fractional with an additional pre left shift.
 *  and saturate.
 */
DLB_SFRACT
DLB_SsshlAU
    (DLB_ACCU a
    ,unsigned i   
     DLB_XARGS
    );

/** Clip an accumulator so that it lies within two values and then round the
 * result to long fractional.
 * This is useful for converting accumulators with saturation at
 * particular points.
 */
DLB_LFRACT /** @return Input clipped and converted to DLB_ACCU. */
DLB_LclipALL
    (DLB_ACCU   a   /**< Input value to clip */
    ,DLB_LFRACT lo  /**< Lower limit */
    ,DLB_LFRACT hi  /**< Upper limit */
     DLB_XARGS
    );

/** Clip a short fractional so that it lies within two values.
 */
DLB_SFRACT /**  @return Input clipped to range lo..hi. */
DLB_SclipSSS
    (DLB_SFRACT a   /**< Input value to clip. */
    ,DLB_SFRACT lo  /**< Lower limit. */
    ,DLB_SFRACT hi  /**< Upper limit. */
     DLB_XARGS
    );

/** Clip a long fractional so that it lies within two values.
 */
DLB_LFRACT /** @return Input clipped to range lo..hi. */
DLB_LclipLLL
    (DLB_LFRACT a   /**< Input value to clip. */
    ,DLB_LFRACT lo  /**< Lower limit. */
    ,DLB_LFRACT hi  /**< Upper limit. */
     DLB_XARGS
    );

/** Clip an accumulator so that it lies within two values.
 */
DLB_ACCU /** @return Input clipped to range lo..hi. */
DLB_AclipAAA
    (DLB_ACCU a   /**< Input value to clip. */
    ,DLB_ACCU lo  /**< Lower limit. */
    ,DLB_ACCU hi  /**< Upper limit. */
     DLB_XARGS
    );

/** Convert an extended accumulator to short fractional using the fastest
 * method available on the processor.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_StruncX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to short fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SsatX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to short fractional with rounding.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_SFRACT
DLB_SrndX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to short fractional using the best method
 * available.
 * Saturation and rounding must be performed.
 */
DLB_SFRACT
DLB_SsrndX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert extended accumulator to short fractional using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_SFRACT
DLB_SrnddX
    (DLB_XACCU a
    DLB_XARGS
    );

/** Convert an extended accumulator to long fractional using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LtruncX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to long fractional with saturation.
 * Rounding may optionally be performed if there is no performance penalty.
 * If necessary, the saturation can mitigate wrap-around errors (See \ref saturation).
 */
DLB_LFRACT
DLB_LsatX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to long fractional with rounding.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_LFRACT
DLB_LrndX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to long fractional using the best method
 * available.
 * Will saturate if the result would otherwise wrap around, and will round if possible and necessary.
 */
DLB_LFRACT
DLB_LsrndX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert extended accumulator to long fractional using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_LFRACT
DLB_LrnddX
    (DLB_XACCU a
    DLB_XARGS
    );

/** Convert an extended accumulator to accumulator using the fastest method
 * available on the processor.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1). Saturation and rounding may optionally be
 * performed if there is no performance penalty.
 */
DLB_ACCU
DLB_AtruncX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert an extended accumulator to accumulator with rounding.
 * Typically no saturation will be done here and the extended accumulator is assumed to
 * be within the range [-1,+1-epsilon), where epsilon is the rounding offset.
 * Passing in the value just below +1 on a fixed point processor may
 * cause wraparound.
 * Saturation may optionally be performed if there is no performance penalty.
 */
DLB_ACCU
DLB_ArndX
    (DLB_XACCU a
     DLB_XARGS
    );

/** Convert extended accumulator to accumulator fractional using method of round down
 * (i.e. round towards negtive infinity).
 * This is a real truncation operation. 
*/
DLB_ACCU
DLB_ArnddX
    (DLB_XACCU a
    DLB_XARGS
    );

/* @} */

/**************************************************************************//**
@name Accumulator Storage
@{
******************************************************************************/

/** Ready an accumulator for storage to memory.
 * This optionally saturates to [-1,+1) on processors where extra work would
 * need to be done to store the headroom (guard) bits (such as 56k).
 */
DLB_ACCU
DLB_AstoreA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Ready an extended accumulator for storage to memory.
 * This optionally saturates to [-1,+1) on processors where extra work would
 * need to be done to store the headroom (guard) bits (such as 56k).
 */
DLB_XACCU
DLB_XstoreX
    (DLB_XACCU a
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

/** Flush short fractional denormal values to 0 on floating point processors. */
DLB_SFRACT
DLB_SflushS
    (DLB_SFRACT a
     DLB_XARGS
    );

/** Flush long fractional denormal values to 0 on floating point processors. */
DLB_LFRACT
DLB_LflushL
    (DLB_LFRACT a
     DLB_XARGS
    );

/** Flush accumulator denormal values to 0 on floating point processors. */
DLB_ACCU
DLB_AflushA
    (DLB_ACCU a
     DLB_XARGS
    );

/** Flush extended accumulator denormal values to 0 on floating point processors. */
DLB_XACCU
DLB_XflushX
    (DLB_XACCU a
     DLB_XARGS
    );

/* @} */
/* @} */
#endif /* DLB_SKIP_INTRINSIC_PROTOTYPES */

/******************************************************************************
intrinsics debug macros (to collect file and line information in debug builds).
******************************************************************************/
/* DLB_SHOW_XARGS can be defined when the including C program wants to
 * explicitly use DLB_VALUE_XARGS or DLB_PASS_XARGS when calling intrinsics.
 */
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
#   define DLB_S_F(f)               DLB_S_F(f DLB_VALUE_XARGS)
#   define DLB_SqF(f)               DLB_SqF(f DLB_VALUE_XARGS)
#   define DLB_L_F(f)               DLB_L_F(f DLB_VALUE_XARGS)
#   define DLB_LqF(f)               DLB_LqF(f DLB_VALUE_XARGS)
#   define DLB_LsrndLU(a,i)         DLB_LsrndLU(a, i DLB_VALUE_XARGS)
#   define DLB_LtruncLU(a, i)       DLB_LtruncLU(a, i DLB_VALUE_XARGS)
#   define DLB_LrnddLU(a, i)        DLB_LrnddLU(a, i DLB_VALUE_XARGS)
#   define DLB_LrnddXU(a, i)        DLB_LrnddXU(a, i DLB_VALUE_XARGS)
#   define DLB_A_F(f)               DLB_A_F(f DLB_VALUE_XARGS)
#   define DLB_X_F(f)               DLB_X_F(f DLB_VALUE_XARGS)
#   define DLB_S_16(i16)            DLB_S_16(i16 DLB_VALUE_XARGS)
#   define DLB_L_16(i16)            DLB_L_16(i16 DLB_VALUE_XARGS)
#   define DLB_S_32(i32)            DLB_S_32(i32 DLB_VALUE_XARGS)
#   define DLB_L_32(i32)            DLB_L_32(i32 DLB_VALUE_XARGS)
#   define DLB_S_M(m)               DLB_S_M(m DLB_VALUE_XARGS)
#   define DLB_L_M(m)               DLB_L_M(m DLB_VALUE_XARGS)
#   define DLB_A_M(m)               DLB_A_M(m DLB_VALUE_XARGS)
#   define DLB_X_M(m)               DLB_X_M(m DLB_VALUE_XARGS)
#   define DLB_M_S(x)               DLB_M_S(x DLB_VALUE_XARGS)
#   define DLB_M_L(x)               DLB_M_L(x DLB_VALUE_XARGS)
#   define DLB_M_A(x)               DLB_M_A(x DLB_VALUE_XARGS)
#   define DLB_M_X(x)               DLB_M_X(x DLB_VALUE_XARGS)
#   define DLB_F_S(s)               DLB_F_S(s DLB_VALUE_XARGS)
#   define DLB_F_L(l)               DLB_F_L(l DLB_VALUE_XARGS)
#   define DLB_F_A(a)               DLB_F_A(a DLB_VALUE_XARGS)
#   define DLB_F_X(x)               DLB_F_X(x DLB_VALUE_XARGS)
#   define DLB_16srndS(s)           DLB_16srndS(s DLB_VALUE_XARGS)
#   define DLB_16sfloorS(s)         DLB_16sfloorS(s DLB_VALUE_XARGS)
#   define DLB_16srndL(l)           DLB_16srndL(l DLB_VALUE_XARGS)
#   define DLB_16sfloorL(l)         DLB_16sfloorL(l DLB_VALUE_XARGS)
#   define DLB_32srndS(s)           DLB_32srndS(s DLB_VALUE_XARGS)
#   define DLB_32sfloorS(s)         DLB_32sfloorS(s DLB_VALUE_XARGS)
#   define DLB_32srndL(l)           DLB_32srndL(l DLB_VALUE_XARGS)
#   define DLB_32sfloorL(l)         DLB_32sfloorL(l DLB_VALUE_XARGS)
#   define DLB_SminSS(a, b)         DLB_SminSS(a, b DLB_VALUE_XARGS)
#   define DLB_SmaxSS(a, b)         DLB_SmaxSS(a, b DLB_VALUE_XARGS)
#   define DLB_LminLL(a, b)         DLB_LminLL(a, b DLB_VALUE_XARGS)
#   define DLB_LmaxLL(a, b)         DLB_LmaxLL(a, b DLB_VALUE_XARGS)
#   define DLB_AminAA(a, b)         DLB_AminAA(a, b DLB_VALUE_XARGS)
#   define DLB_AmaxAA(a, b)         DLB_AmaxAA(a, b DLB_VALUE_XARGS)
#   define DLB_XminXX(a, b)         DLB_XminXX(a, b DLB_VALUE_XARGS)
#   define DLB_XmaxXX(a, b)         DLB_XmaxXX(a, b DLB_VALUE_XARGS)
#   define DLB_IltSS(a, b)          DLB_IltSS(a, b DLB_VALUE_XARGS)
#   define DLB_IleqSS(a, b)         DLB_IleqSS(a, b DLB_VALUE_XARGS)
#   define DLB_IeqSS(a, b)          DLB_IeqSS(a, b DLB_VALUE_XARGS)
#   define DLB_IltLL(a, b)          DLB_IltLL(a, b DLB_VALUE_XARGS)
#   define DLB_IleqLL(a, b)         DLB_IleqLL(a, b DLB_VALUE_XARGS)
#   define DLB_IeqLL(a, b)          DLB_IeqLL(a, b DLB_VALUE_XARGS)
#   define DLB_IltAA(a, b)          DLB_IltAA(a, b DLB_VALUE_XARGS)
#   define DLB_IleqAA(a, b)         DLB_IleqAA(a, b DLB_VALUE_XARGS)
#   define DLB_IeqAA(a, b)          DLB_IeqAA(a, b DLB_VALUE_XARGS)
#   define DLB_IltXX(a, b)          DLB_IltXX(a, b DLB_VALUE_XARGS)
#   define DLB_IleqXX(a, b)         DLB_IleqXX(a, b DLB_VALUE_XARGS)
#   define DLB_IeqXX(a, b)          DLB_IeqXX(a, b DLB_VALUE_XARGS)
#   define DLB_SshlSU(a, i)         DLB_SshlSU(a, i DLB_VALUE_XARGS)
#   define DLB_SsshlSU(a, i)        DLB_SsshlSU(a, i DLB_VALUE_XARGS)
#   define DLB_SshrSU(a, i)         DLB_SshrSU(a, i DLB_VALUE_XARGS)
#   define DLB_SshlSI(a, i)         DLB_SshlSI(a, i DLB_VALUE_XARGS)
#   define DLB_SshrSI(a, i)         DLB_SshrSI(a, i DLB_VALUE_XARGS)
#   define DLB_SsshlSI(a, i)        DLB_SsshlSI(a, i DLB_VALUE_XARGS)
#   define DLB_SsshrSI(a, i)        DLB_SsshrSI(a, i DLB_VALUE_XARGS)
#   define DLB_LshlLU(a, i)         DLB_LshlLU(a, i DLB_VALUE_XARGS)
#   define DLB_LsshlLU(a, i)        DLB_LsshlLU(a, i DLB_VALUE_XARGS)
#   define DLB_LsshlAU(a, i)        DLB_LsshlAU(a, i DLB_VALUE_XARGS)
#   define DLB_SshlAU(a, i)         DLB_SshlAU(a, i DLB_VALUE_XARGS)
#   define DLB_SsshlAU(a, i)        DLB_SsshlAU(a, i DLB_VALUE_XARGS)
#   define DLB_LshrLU(a, i)         DLB_LshrLU(a, i DLB_VALUE_XARGS)
#   define DLB_LshlLI(a, i)         DLB_LshlLI(a, i DLB_VALUE_XARGS)
#   define DLB_LshrLI(a, i)         DLB_LshrLI(a, i DLB_VALUE_XARGS)
#   define DLB_LsshlLI(a, i)        DLB_LsshlLI(a, i DLB_VALUE_XARGS)
#   define DLB_LsshrLI(a, i)        DLB_LsshrLI(a, i DLB_VALUE_XARGS)
#   define DLB_AshlAU(a, i)         DLB_AshlAU(a, i DLB_VALUE_XARGS)
#   define DLB_AshrAU(a, i)         DLB_AshrAU(a, i DLB_VALUE_XARGS)
#   define DLB_AshlAI(a, i)         DLB_AshlAI(a, i DLB_VALUE_XARGS)
#   define DLB_AshrAI(a, i)         DLB_AshrAI(a, i DLB_VALUE_XARGS)
#   define DLB_XshlXU(a, i)         DLB_XshlXU(a, i DLB_VALUE_XARGS)
#   define DLB_XshrXU(a, i)         DLB_XshrXU(a, i DLB_VALUE_XARGS)
#   define DLB_XshlXI(a, i)         DLB_XshlXI(a, i DLB_VALUE_XARGS)
#   define DLB_XshrXI(a, i)         DLB_XshrXI(a, i DLB_VALUE_XARGS)
#   define DLB_IlimsI(i)            DLB_IlimsI(i DLB_VALUE_XARGS)
#   define DLB_IlimlI(i)            DLB_IlimlI(i DLB_VALUE_XARGS)
#   define DLB_IlimaI(i)            DLB_IlimaI(i DLB_VALUE_XARGS)
#   define DLB_IlimxI(i)            DLB_IlimxI(i DLB_VALUE_XARGS)
#   define DLB_UpnormS(a)           DLB_UpnormS(a DLB_VALUE_XARGS)
#   define DLB_UpnormL(a)           DLB_UpnormL(a DLB_VALUE_XARGS)
#   define DLB_UpnormA(a)           DLB_UpnormA(a DLB_VALUE_XARGS)
#   define DLB_UpnormX(a)           DLB_UpnormX(a DLB_VALUE_XARGS)
#   define DLB_UnormS(a)            DLB_UnormS(a DLB_VALUE_XARGS)
#   define DLB_UnormL(a)            DLB_UnormL(a DLB_VALUE_XARGS)
#   define DLB_UnormA(a)            DLB_UnormA(a DLB_VALUE_XARGS)
#   define DLB_UnormX(a)            DLB_UnormX(a DLB_VALUE_XARGS)
#   define DLB_SaddSS(a, b)         DLB_SaddSS(a, b DLB_VALUE_XARGS)
#   define DLB_SsubSS(a, b)         DLB_SsubSS(a, b DLB_VALUE_XARGS)
#   define DLB_SsaddSS(a, b)        DLB_SsaddSS(a, b DLB_VALUE_XARGS)
#   define DLB_SssubSS(a, b)        DLB_SssubSS(a, b DLB_VALUE_XARGS)
#   define DLB_LaddLL(a, b)         DLB_LaddLL(a, b DLB_VALUE_XARGS)
#   define DLB_LsubLL(a, b)         DLB_LsubLL(a, b DLB_VALUE_XARGS)
#   define DLB_LabsdiffLL(a, b)     DLB_LabsdiffLL(a, b DLB_VALUE_XARGS)
#   define DLB_LsaddLL(a, b)        DLB_LsaddLL(a, b DLB_VALUE_XARGS)
#   define DLB_LssubLL(a, b)        DLB_LssubLL(a, b DLB_VALUE_XARGS)
#   define DLB_LsaddLS(a, b)        DLB_LsaddLS(a, b DLB_VALUE_XARGS)
#   define DLB_LssubLS(a, b)        DLB_LssubLS(a, b DLB_VALUE_XARGS)
#   define DLB_AaddAA(a, b)         DLB_AaddAA(a, b DLB_VALUE_XARGS)
#   define DLB_AsubAA(a, b)         DLB_AsubAA(a, b DLB_VALUE_XARGS)
#   define DLB_XaddXX(a, b)         DLB_XaddXX(a, b DLB_VALUE_XARGS)
#   define DLB_XsubXX(a, b)         DLB_XsubXX(a, b DLB_VALUE_XARGS)
#   define DLB_SnegS(a)             DLB_SnegS(a DLB_VALUE_XARGS)
#   define DLB_SsnegS(a)            DLB_SsnegS(a DLB_VALUE_XARGS)
#   define DLB_LnegL(a)             DLB_LnegL(a DLB_VALUE_XARGS)
#   define DLB_LsnegL(a)            DLB_LsnegL(a DLB_VALUE_XARGS)
#   define DLB_AnegA(a)             DLB_AnegA(a DLB_VALUE_XARGS)
#   define DLB_XnegX(a)             DLB_XnegX(a DLB_VALUE_XARGS)
#   define DLB_SabsS(a)             DLB_SabsS(a DLB_VALUE_XARGS)
#   define DLB_SsabsS(a)            DLB_SsabsS(a DLB_VALUE_XARGS)
#   define DLB_LabsL(a)             DLB_LabsL(a DLB_VALUE_XARGS)
#   define DLB_LsabsL(a)            DLB_LsabsL(a DLB_VALUE_XARGS)
#   define DLB_AabsA(a)             DLB_AabsA(a DLB_VALUE_XARGS)
#   define DLB_XabsX(a)             DLB_XabsX(a DLB_VALUE_XARGS)
#   define DLB_LmpySS(a, b)         DLB_LmpySS(a, b DLB_VALUE_XARGS)
#   define DLB_LmpyLS(a, b)         DLB_LmpyLS(a, b DLB_VALUE_XARGS)
#   define DLB_LmpyLL(a, b)         DLB_LmpyLL(a, b DLB_VALUE_XARGS)
#   define DLB_LsmpySS(a, b)        DLB_LsmpySS(a, b DLB_VALUE_XARGS)
#   define DLB_LsmpyLS(a, b)        DLB_LsmpyLS(a, b DLB_VALUE_XARGS)
#   define DLB_LsmpyLL(a, b)        DLB_LsmpyLL(a, b DLB_VALUE_XARGS)
#   define DLB_SrmpySS(a, b)        DLB_SrmpySS(a, b DLB_VALUE_XARGS)
#   define DLB_SsrmpySS(a, b)       DLB_SsrmpySS(a, b DLB_VALUE_XARGS)
#   define DLB_AmpySS(a, b)         DLB_AmpySS(a, b DLB_VALUE_XARGS)
#   define DLB_AmpyLS(a, b)         DLB_AmpyLS(a, b DLB_VALUE_XARGS)
#   define DLB_AmpyLL(a, b)         DLB_AmpyLL(a, b DLB_VALUE_XARGS)
#   define DLB_XmpyLS(a, b)         DLB_XmpyLS(a, b DLB_VALUE_XARGS)
#   define DLB_AmpyAS(a, b)         DLB_AmpyAS(a, b DLB_VALUE_XARGS)
#   define DLB_XmpyXS(a, b)         DLB_XmpyXS(a, b DLB_VALUE_XARGS)
#   define DLB_LsmacLSS(a, x, y)    DLB_LsmacLSS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LsmacLLS(a, x, y)    DLB_LsmacLLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LsmacLLL(a, x, y)    DLB_LsmacLLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmacLSS(a, x, y)     DLB_LmacLSS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmacLLS(a, x, y)     DLB_LmacLLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmacLLL(a, x, y)     DLB_LmacLLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmacASS(a, x, y)     DLB_AmacASS(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmacALS(a, x, y)     DLB_AmacALS(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmacALL(a, x, y)     DLB_AmacALL(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmacAAS(a, x, y)     DLB_AmacAAS(a, x, y DLB_VALUE_XARGS)
#   define DLB_XmacXLS(a, x, y)     DLB_XmacXLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_XmacXXS(a, x, y)     DLB_XmacXXS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LsmsuLSS(a, x, y)    DLB_LsmsuLSS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LsmsuLLS(a, x, y)    DLB_LsmsuLLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LsmsuLLL(a, x, y)    DLB_LsmsuLLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmsuLSS(a, x, y)     DLB_LmsuLSS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmsuLLS(a, x, y)     DLB_LmsuLLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmsuLLL(a, x, y)     DLB_LmsuLLL(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmsuASS(a, x, y)     DLB_AmsuASS(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmsuALS(a, x, y)     DLB_AmsuALS(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmsuALL(a, x, y)     DLB_AmsuALL(a, x, y DLB_VALUE_XARGS)
#   define DLB_AmsuAAS(a, x, y)     DLB_AmsuAAS(a, x, y DLB_VALUE_XARGS)
#   define DLB_XmsuXLS(a, x, y)     DLB_XmsuXLS(a, x, y DLB_VALUE_XARGS)
#   define DLB_XmsuXXS(a, x, y)     DLB_XmsuXXS(a, x, y DLB_VALUE_XARGS)
#   define DLB_LmpyLI(a, i)         DLB_LmpyLI(a, i DLB_VALUE_XARGS)
#   define DLB_ImpyIL(i, a)         DLB_ImpyIL(i, a DLB_VALUE_XARGS)
#   define DLB_IrmpyIL(i, a)        DLB_IrmpyIL(i, a DLB_VALUE_XARGS)
#   define DLB_UrmpyUL(i, a)        DLB_UrmpyUL(i, a DLB_VALUE_XARGS)
#   define DLB_ImpyIS(i, a)         DLB_ImpyIS(i, a DLB_VALUE_XARGS)
#   define DLB_IrmpyIS(i, a)        DLB_IrmpyIS(i, a DLB_VALUE_XARGS)
#   define DLB_UrmpyUS(i, a)        DLB_UrmpyUS(i, a DLB_VALUE_XARGS)
#   define DLB_X_S(a)               DLB_X_S(a DLB_VALUE_XARGS)
#   define DLB_A_S(a)               DLB_A_S(a DLB_VALUE_XARGS)
#   define DLB_L_S(a)               DLB_L_S(a DLB_VALUE_XARGS)
#   define DLB_X_L(a)               DLB_X_L(a DLB_VALUE_XARGS)
#   define DLB_A_L(a)               DLB_A_L(a DLB_VALUE_XARGS)
#   define DLB_X_A(a)               DLB_X_A(a DLB_VALUE_XARGS)
#   define DLB_StruncL(a)           DLB_StruncL(a DLB_VALUE_XARGS)
#   define DLB_SsatL(a)             DLB_SsatL(a DLB_VALUE_XARGS)
#   define DLB_SrndL(a)             DLB_SrndL(a DLB_VALUE_XARGS)
#   define DLB_SsrndL(a)            DLB_SsrndL(a DLB_VALUE_XARGS)
#   define DLB_SrnddL(a)            DLB_SrnddL(a DLB_VALUE_XARGS)
#   define DLB_StruncA(a)           DLB_StruncA(a DLB_VALUE_XARGS)
#   define DLB_SsatA(a)             DLB_SsatA(a DLB_VALUE_XARGS)
#   define DLB_SrndA(a)             DLB_SrndA(a DLB_VALUE_XARGS)
#   define DLB_SsrndA(a)            DLB_SsrndA(a DLB_VALUE_XARGS)
#   define DLB_SrnddA(a)            DLB_SrnddA(a DLB_VALUE_XARGS)
#   define DLB_StruncX(a)           DLB_StruncX(a DLB_VALUE_XARGS)
#   define DLB_SsatX(a)             DLB_SsatX(a DLB_VALUE_XARGS)
#   define DLB_SrndX(a)             DLB_SrndX(a DLB_VALUE_XARGS)
#   define DLB_SsrndX(a)            DLB_SsrndX(a DLB_VALUE_XARGS)
#   define DLB_SrnddX(a)            DLB_SrnddX(a DLB_VALUE_XARGS)
#   define DLB_LtruncA(a)           DLB_LtruncA(a DLB_VALUE_XARGS)
#   define DLB_LsatA(a)             DLB_LsatA(a DLB_VALUE_XARGS)
#   define DLB_LrndA(a)             DLB_LrndA(a DLB_VALUE_XARGS)
#   define DLB_LsrndA(a)            DLB_LsrndA(a DLB_VALUE_XARGS)
#   define DLB_LrnddA(a)            DLB_LrnddA(a DLB_VALUE_XARGS)
#   define DLB_LshrAU(a, b)         DLB_LshrAU(a, b DLB_VALUE_XARGS)
#   define DLB_LshlAU(a, b)         DLB_LshlAU(a, b DLB_VALUE_XARGS)
#   define DLB_LclipALL(a, lo, hi)  DLB_LclipALL(a, lo, hi DLB_VALUE_XARGS)
#   define DLB_SclipSSS(a, lo, hi)  DLB_SclipSSS(a, lo, hi DLB_VALUE_XARGS)
#   define DLB_LclipLLL(a, lo, hi)  DLB_LclipLLL(a, lo, hi DLB_VALUE_XARGS)
#   define DLB_AclipAAA(a, lo, hi)  DLB_AclipAAA(a, lo, hi DLB_VALUE_XARGS)
#   define DLB_LtruncX(a)           DLB_LtruncX(a DLB_VALUE_XARGS)
#   define DLB_LsatX(a)             DLB_LsatX(a DLB_VALUE_XARGS)
#   define DLB_LrndX(a)             DLB_LrndX(a DLB_VALUE_XARGS)
#   define DLB_LsrndX(a)            DLB_LsrndX(a DLB_VALUE_XARGS)
#   define DLB_AtruncX(a)           DLB_AtruncX(a DLB_VALUE_XARGS)
#   define DLB_ArndX(a)             DLB_ArndX(a DLB_VALUE_XARGS)
#   define DLB_ArnddX(a)            DLB_ArnddX(a DLB_VALUE_XARGS)
#   define DLB_AstoreA(a)           DLB_AstoreA(a DLB_VALUE_XARGS)
#   define DLB_XstoreX(a)           DLB_XstoreX(a DLB_VALUE_XARGS)
#   define DLB_SflushS(a)           DLB_SflushS(a DLB_VALUE_XARGS)
#   define DLB_LflushL(a)           DLB_LflushL(a DLB_VALUE_XARGS)
#   define DLB_AflushA(a)           DLB_AflushA(a DLB_VALUE_XARGS)
#   define DLB_XflushX(a)           DLB_XflushX(a DLB_VALUE_XARGS)
#   define DLB_I_S(a)               DLB_I_S(a DLB_VALUE_XARGS)
#   define DLB_I_L(a)               DLB_I_L(a DLB_VALUE_XARGS)
#   define DLB_IsrndS(a)            DLB_IsrndS(a DLB_VALUE_XARGS)
#   define DLB_S_I(i)               DLB_S_I(i DLB_VALUE_XARGS)
#   define DLB_IsrndL(l)            DLB_IsrndL(l DLB_VALUE_XARGS)
#   define DLB_L_I(i)               DLB_L_I(i DLB_VALUE_XARGS)
#   define DLB_IsrndLU(l, s)        DLB_IsrndLU(l, s DLB_VALUE_XARGS)
#   define DLB_IintLU(x, fp)        DLB_IintLU(x, fp DLB_VALUE_XARGS)
#   define DLB_LmantLU(x, fp)       DLB_LmantLU(x, fp DLB_VALUE_XARGS)

#endif

/******************************************************************************
compatibilty layer(s)
******************************************************************************/
#ifndef DLB_INTRINSICS_NEW_NAMES
/* Include the legacy names compatibility layer unless
 * DLB_INTRINSICS_NEW_NAMES is defined.
 */
#   include "legacy_compat/dlb_intrinsics/dlb_legacy.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
