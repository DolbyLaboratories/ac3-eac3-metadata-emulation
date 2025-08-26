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
 * @brief Abstraction layers for Dolby Intrinsics.
 *
 *        This is broken into a separate file to keep dlb_intrinsics.h simple.
 *        Do not include this file directly. It is included by dlb_intrinsics.h.
 *        There are different kinds of abstraction layers:
 *        @li @ref backend_abstraction "Backend abstraction"
 *        @li @ref compiler_abstraction "Compiler abstraction"
 *        @li @ref debugging_abstraction "Debugging abstraction"
 *        @li @ref instrumentation_abstraction "Instrumentation abstraction"
 *        @li @ref dlb_legacy_compat
 */

/* Compatibility with legacy debugging define */
#ifndef DLB_INTRINSICS_NEW_NAMES
#include "legacy_compat/dlb_intrinsics/dlb_legacy_abstraction.h"
#endif

/* Compiler abstraction */
#include "dlb_compiler.h"

/**************************************************************************//**
@name Dolby Intrinsics Declaration
Helper macros for Dolby Intrinsics function declarations.
These are defined before the backend abstraction is included because they
are probably needed to define the Dolby Intrinsics in the backend abstraction.
******************************************************************************/
/* @{ */
/** DLB_XMAKE_NAME is used by DLB_MAKE_NAME. This two stage macro is required
 * to ensure that all macros within the arguments are fully expanded.
 */
#define DLB_XMAKE_NAME(a, b) DLB_##a##_##b

/** DLB macro defined by backend abstraction should use DLB_MAKE_NAME
 *  to glue together the parts of the name.
 */
#define DLB_MAKE_NAME(a, b) DLB_XMAKE_NAME(a, b)

#ifdef DLB_DEBUG_XARGS

#   define DLB_XARGS , const char* file, int line
#   define DLB_DECL_XARGS const char* file; int line;
#   define DLB_PASS_XARGS , file, line
#   define DLB_ASSIGN_XARGS(x) x.file = file; x.line = line;
#   define DLB_VALUE_XARGS ,__FILE__, __LINE__
#   define DLB_FMT_XARGS "%s(%d)"

#else

/** Debugging argument declaration macro.
 * In debug builds, the Dolby Intrinsics functions also take the current filename
 * and line number so they can produce diagnostic messages. DLB_XARGS is
 * a macro which can be used in a Dolby Intrinsics function declaration to declare
 * these extra arguments. In a release build, this expands to nothing.
 */
#   define DLB_XARGS

/** Debugging member declaration macro.
 * Similar to DLB_XARGS, but suitable for declaring struct members instead
 * of function arguments.
 */
#   define DLB_DECL_XARGS

/** Debugging argument passing macro.
 * DLB_PASS_XARGS can be used to pass the extra debugging arguments declared
 * using DLB_XARGS to a called function within the intrinsic implementation.
 * In a release build, this expands to nothing.
 */
#   define DLB_PASS_XARGS

/** Debugging argument member assignment macro.
 * Set debugging members in struct x (declared with DLB_DECL_XARGS) to the
 * arguments of the current function (declared with DLB_XARGS)
 */
#   define DLB_ASSIGN_XARGS(x)

/** Debugging value macro.
 * DLB_VALUE_XARGS uses predefined macros to get the current value of the
 * XARGS, such as __FILE__ and __LINE__. The point of doing all of this in
 * macros is to potentially make the debugging information compiler-dependent.
 * For example, GCC provides more information than just __FILE__ and __LINE__,
 * such as __func__, the name of the current function.
 */
#   define DLB_VALUE_XARGS

/** Debugging argument printf format macro.
 * This macro should expand to a printf format string which prints values
 * passed to printf using DLB_PASS_XARGS.
 */
#   define DLB_FMT_XARGS

#endif

/* @} */

/**************************************************************************//**
@anchor backend_abstraction
@name Backend Abstraction

Dolby Intrinsics is adaptable to a wide range of floating and fixed point
processors with different word sizes. These are known as processor backends.
Dolby Intrinsics also supports parametric model backends for simulating the
behaviour of an algorithm on custom architectures. It also supports generic 
backends which do not target a specific processor.

The specifics of a backend are specified in the backend abstraction header.
Zero or more C or assembly source files may implement the backend abstraction
defined in the header, or the header may contain inline implementations using
compiler intrinsics or inline assembly.

A backend abstraction may be selected by either defining one of the backend
IDs below or by defining DLB_BACKEND with the path to a backend abstraction
file to include.
******************************************************************************/
/* @{ */

#if defined DLB_BACKEND
    /* User-defined backend abstraction. DLB_BACKEND must be defined to
     * the relative path of the backend abstraction file.
     */

/* These are processor (and probably compiler) specific backends */
#elif defined DLB_BACKEND_X86_IPP_FLOAT_NON_INTERLEAVED
#   define DLB_BACKEND "backend/generic/dlb_backend_float32.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/x86_float_ipp/dlb_dsplib_x86_float_ipp.h"
#	define DLB_USING_IPP 1
#elif defined DLB_BACKEND_X86_IPP_FLOAT_INTERLEAVED
#   define DLB_BACKEND "backend/generic/dlb_backend_float32.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/x86_float_ipp/dlb_dsplib_x86_float_ipp.h"
#	define DLB_USING_IPP 1
#elif defined DLB_BACKEND_ARM5TE
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm5te.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/arm/dlb_dsplib_arm.h"
#elif defined DLB_BACKEND_ARM6
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm6.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/arm/dlb_dsplib_arm.h"
#elif defined DLB_BACKEND_ARM7INT
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm7int.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/arm/dlb_dsplib_arm.h"
#elif defined DLB_BACKEND_ARM7INT_NEON
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm7int_neon.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/arm/dlb_dsplib_arm.h"
#elif defined DLB_BACKEND_ARM7FLOAT
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm7float.h"
#elif defined DLB_BACKEND_ARM7FLOAT_NEON
#   define DLB_BACKEND "backend/processor/arm/dlb_backend_arm7float.h"
#elif defined DLB_BACKEND_C64
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c64.h"
#	define DLB_DSPLIB  "dlb_dsplib/backend/processor/c64/dlb_dsplib_c64.h"
#elif defined DLB_BACKEND_C64PLUS
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c64plus.h"
#	define DLB_DSPLIB  "dlb_dsplib/backend/processor/c64/dlb_dsplib_c64.h"
#elif defined DLB_BACKEND_C66_FIXED
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c66_fixed.h"
#	define DLB_DSPLIB  "dlb_dsplib/backend/processor/c66_fixed/dlb_dsplib_c66_fixed.h"
#elif defined DLB_BACKEND_C66_FLOAT
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c66_float.h"
#	define DLB_DSPLIB  "dlb_dsplib/backend/processor/c66_float/dlb_dsplib_c66_float.h"
#elif defined DLB_BACKEND_C67
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c67.h"
#elif defined DLB_BACKEND_C67PLUS
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c67.h"
#elif defined DLB_BACKEND_MIPS32R2_DSPR1
#   define DLB_BACKEND "backend/processor/mips/dlb_backend_mips32r2_dspr1.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/mips/dlb_dsplib_mips.h"
#elif defined DLB_BACKEND_C674_FLOAT
#   define DLB_BACKEND "backend/processor/TI/dlb_backend_c674_float.h"
#   define DLB_DSPLIB  "dlb_dsplib/backend/processor/c674_float/dlb_dsplib_c674_float.h"

#elif defined DLB_BACKEND_MODEL_DLB16X32
    /* This is the original dlb_intrinsics DLBDSP target. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_dlb16x32.h"

/* These are plain C backend models written for the bit exact behavior of their
 * corresponding processor specific backend.
 */
#elif defined DLB_BACKEND_MODEL_ARM5TE
#   define DLB_BACKEND "backend/model/arm/dlb_backend_model_arm5te.h"
#   define DLB_DSPLIB "dlb_dsplib/backend/model/arm/dlb_dsplib_model_arm.h" 
#elif defined DLB_BACKEND_MODEL_ARM6
#   define DLB_BACKEND "backend/model/arm/dlb_backend_model_arm6.h"
#   define DLB_DSPLIB "dlb_dsplib/backend/model/arm/dlb_dsplib_model_arm.h" 
#elif defined DLB_BACKEND_MODEL_ARM7INT
#   define DLB_BACKEND "backend/model/arm/dlb_backend_model_arm7int.h"
#   define DLB_DSPLIB "dlb_dsplib/backend/model/arm/dlb_dsplib_model_arm.h" 
#elif (defined DLB_BACKEND_MODEL_C64 || defined DLB_BACKEND_MODEL_C64PLUS)
#   define DLB_BACKEND "backend/model/TI/dlb_backend_model_c64.h"
#   define DLB_DSPLIB "dlb_dsplib/backend/model/c64/dlb_dsplib_model_c64.h" 

/* These are plain C, non-processor specific backends, which can be used as
 * starting points for processor specific backends, or for creating C code
 * reference of a processor specific backend.
 */
#elif defined DLB_BACKEND_GENERIC_RISC16X32
    /* Like model_risc16x32 but tuned specifically for 16x32, not parametric */
#   define DLB_BACKEND "backend/generic/dlb_backend_risc16x32.h"
#elif defined DLB_BACKEND_GENERIC_FLOAT64
    /* This is the generic scaled 64 bit model - it is the golden
     * reference for testing.
     */
#   define DLB_BACKEND "backend/generic/dlb_backend_float64.h"
#elif defined DLB_BACKEND_GENERIC_UFLOAT64
    /* Unscaled floating point 64 bit backend - useful during algorithm
     * development.
     */
#   define DLB_BACKEND "backend/generic/dlb_backend_ufloat64.h"
#elif defined DLB_BACKEND_GENERIC_FLOAT32
#   define DLB_BACKEND "backend/generic/dlb_backend_float32.h"


/* These are non-processor specific backends with some extra functionality to
 * enable debugging through a type wrapper.
 */
#elif defined DLB_BACKEND_MODEL_DSP16
    /* This is the Hummingbird 16 bit DSP model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_dsp16.h"
#elif defined DLB_BACKEND_MODEL_DSP24
    /* This is the Hummingbird 24 bit DSP model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_dsp24.h"
#elif defined DLB_BACKEND_MODEL_RISC16X32
    /* This is the Hummingbird (ARM-style) 16x32 integer model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_risc16x32.h"
#elif defined DLB_BACKEND_MODEL_DSP16X32
    /* This is the Hummingbird (C64-style) 16x32 fractional model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_dsp16x32.h"
#elif defined DLB_BACKEND_MODEL_Q31ACCU
    /* This is the Hummingbird (dsp-style-without-accumulator-guard-bits) 16x32 fractional model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_q31accu.h"
#elif defined DLB_BACKEND_MODEL_FLOAT32
    /* This is the Hummingbird floating point model. */
#   define DLB_BACKEND "backend/model/dlb_backend_model_float32.h"
#else
    /** Relative path to backend abstraction header to include.
     * DLB_BACKEND will be automatically populated if one
     * of the natively supported targets is selected using a define such
     * as DLB_BACKEND_ARM. You can also define DLB_BACKEND before
     * including dlb_intrinsics (typically by using the -D option on your
     * compiler's command line) to use a custom backend abstraction.
     */
#   define DLB_BACKEND
#   error You must define one of the DLB_BACKEND macros.
#endif

#ifndef DLB_DSPLIB
/* Use generic dsplib backend when no optimized one is available */
#   define DLB_DSPLIB  "dlb_dsplib/backend/generic/dlb_dsplib_generic.h"
#endif

/* Now we've got the path to the backend abstraction, include it. */
#include DLB_BACKEND

/** Backend abstraction type probe.
 * This function prototype probe should fail to compile if the backend
 * abstraction has failed to define any of the basic types.
 */
static inline
void
dlb_backend_probe
    (DLB_NACCU  n
    ,DLB_XACCU  x
    ,DLB_ACCU   a
    ,DLB_LFRACT l
    ,DLB_SFRACT s
    )
{
    (void) n;
    (void) x;
    (void) a;
    (void) l;
    (void) s;
}

/* @} */
