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
 * @brief Dolby Intrinsics Compiler Abstraction.
 */

#ifndef DLB_COMPILER_H
#define DLB_COMPILER_H 1

/**************************************************************************//**
@anchor compiler_abstraction
@name Compiler Abstraction

Since we rely on some behaviour that is not fully specified by the C90 standard
and we want to compile without warnings, we need a way to abstract out the
quirks of each compiler. We do this by defining a macro named DLB_COMPILER
which is the path to the compiler abstraction to include. If you are using one
of the compilers natively supported by Dolby Intrinsics, then DLB_COMPILER is
automatically set. If you want to use a custom compiler abstraction, you can
define DLB_COMPILER before including this file (using a -D switch on your
compiler's command line, for example).

Each backend may have an keyword file, which gives us working "inline" and
"restrict" keywords.
This is needed for the debugging backend. The rest of the compiler abstraction
can then use the debugging backend.
******************************************************************************/
/* @{ */
#ifdef DLB_COMPILER
    /* User-defined compiler abstraction */
#elif defined(__INTEL_COMPILER) && defined(_MSC_VER)
    /* ICL under Windows */
#   define DLB_COMPILER_KEYWORD "compiler/intel/dlb_compiler_keyword_icl.h"
#   define DLB_COMPILER "compiler/intel/dlb_compiler_icl.h"
#elif defined(__INTEL_COMPILER) && !defined(_MSC_VER)
    /* ICC under Linux or Mac OS X */
#   define DLB_COMPILER_KEYWORD "compiler/intel/dlb_compiler_keyword_icc.h"
#   define DLB_COMPILER "compiler/intel/dlb_compiler_icc.h"
#elif defined _MSC_VER
    /* Microsoft Visual C */
    /* Intel Compiler icl uses the following compiler abstraction */
#   define DLB_COMPILER_KEYWORD "compiler/msvc/dlb_compiler_keyword_msvc.h"
#   define DLB_COMPILER "compiler/msvc/dlb_compiler_msvc.h"
#elif defined __ARMCC_VERSION
    /* RealView compilation tools */
#   define DLB_COMPILER_KEYWORD "compiler/armcc/dlb_compiler_keyword_armcc.h"
#   define DLB_COMPILER "compiler/armcc/dlb_compiler_armcc.h"
#elif defined __TI_COMPILER_VERSION__
    /* TI Code Composer Studio tools, cl6x compiler for C6000 series */
#   define DLB_COMPILER_KEYWORD "compiler/cl6x/dlb_compiler_keyword_cl6x.h"
#   define DLB_COMPILER "compiler/cl6x/dlb_compiler_cl6x.h"
#elif defined __clang__
    /* Clang, from the LLVM project */
#   define DLB_COMPILER_KEYWORD "compiler/clang/dlb_compiler_keyword_clang.h"
#   define DLB_COMPILER "compiler/clang/dlb_compiler_clang.h"
    /* MIPS gcc */
#elif defined __mips__
#   define DLB_COMPILER_KEYWORD "compiler/mipscc/dlb_compiler_keyword_mips_gcc.h"
#   define DLB_COMPILER "compiler/mipscc/dlb_compiler_mips_gcc.h"
#elif defined __GNUC__
    /* GCC: It turns out that several other compilers that target GNU libraries
     * also define __GNUC__, because the library headers depend on it.  Both armcc
     * and clang either do this by default or as an option.  So we must
     * check for __GNUC__ last (or just before the C99 and C90 standard options.) */
    /* Intel Compiler icc uses the following compiler abstraction */
#   define DLB_COMPILER_KEYWORD "compiler/gcc/dlb_compiler_keyword_gcc.h"
#   define DLB_COMPILER "compiler/gcc/dlb_compiler_gcc.h"
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    /* A C99 compiler */
#   define DLB_COMPILER_KEYWORD "compiler/c99/dlb_compiler_keyword_c99.h"
#   define DLB_COMPILER "compiler/c99/dlb_compiler_c99.h"
#else
    /* Fallback C90 abstraction */
#   define DLB_COMPILER_KEYWORD "compiler/c90/dlb_compiler_keyword_c90.h"
#   define DLB_COMPILER "compiler/c90/dlb_compiler_c90.h"
#endif


/* Give us inline and restrict keywords */
#ifdef DLB_COMPILER_KEYWORD
#   include DLB_COMPILER_KEYWORD
#endif

#include "debug/debug_tags.h"
#ifdef DLB_DEBUG
#include "debug/dlb_debug.h"

#if defined(DLB_DEBUG_BACKEND)
    /* User defined debug backend */
#elif defined WIN32 || defined WIN64
#define DLB_DEBUG_BACKEND "debug/windows/debug_windows.h"
#else
#define DLB_DEBUG_BACKEND "debug/std/debug_std.h"
#endif

#include DLB_DEBUG_BACKEND

#else /* !defined(DLB_DEBUG) - Strip out debugging messages */

/* C90 doesn't support variadic macros which is unfortunate, as it prevents
 * us from completely removing the call to DLB_print(). Assuming you don't
 * have any side effects in the argument list, then this should compile out
 * to nothing. However if you have side effects in the argument list for
 * a debugging call, then you probably should rethink your design.
 */
static inline void DLB_print(const char *fmt, ...) {(void)fmt;}

/* The function DLB_is_debug_tag() should typically be used in if statements
 * which should compile out to nothing in non-debug mode.
 */
#define DLB_is_debug_tag(str) 0

#define DLB_assert(expr, tags, str)

#define DLB_assert_xargs(expr, tags, str)

/* Theoretically, this doesn't need to be stripped out for a release build
 * since all the checking is done at compile time. However, if a compiler
 * doesn't notice the unused identifier and therefore uses extra space on the
 * stack, then there will be a minor performance hit, so we play it safe and
 * strip these checks out too.
 */
#define DLB_const_assert(const_expr, name)

#endif

/* Now that we've worked out which compiler abstraction to include, and
 * we also have a debugging backend that can be used, we can bring in the
 * full compiler abstraction.
 * Yes, this is allowed. See example 2 in section $6.3.2 of ISO 9899:1990.
 */
#include DLB_COMPILER

/** Compiler abstraction type probe.
 * This function prototype probe should fail to compile if the compiler
 * abstraction has failed to define any of the basic types.
 */
static inline
void
dlb_compiler_probe
    (int16_t    s16  /**< signed 16 bit integer type */
    ,uint16_t   u16  /**< unsigned 16 bit integer type */
    ,int32_t    s32  /**< signed 32 bit integer type */
    ,uint32_t   u32  /**< unsigned 32 bit integer type */
    ,intmax_t   smax /**< largest signed integer type available */
    ,uintmax_t  umax /**< largest unsigned integer type available */
    )
{
    (void) s16;
    (void) u16;
    (void) s32;
    (void) u32;
    (void) smax;
    (void) umax;
}
/* @} */

/**************************************************************************//**
@name Version Information

This is the API for discovering the version number of the entire
Dolby Intrinsics module.

There are 3 levels of versioning; reflecting API changes, functionality changes
and the maintenance releases as three numbers. The maintenance version shall
be reset for every increase in versions of the API and functionality. The
functional version shall never be reset.
Since the functional version will always be incremented for a functional
change, also including an API change, the API version number is redundant but
there for convenience.
******************************************************************************/
/* @{ */
#define DLB_VERSION_API   1
#define DLB_VERSION_FCT   6
#define DLB_VERSION_MTNC  1

typedef struct STRUCT_DLB_VERSION_INFO {
  int vApi;
  int vFct;
  int vMtnc;
} DLB_VERSION_INFO;

static inline const DLB_VERSION_INFO *DLB_version(void)
{
    static const DLB_VERSION_INFO v =
        {DLB_VERSION_API
        ,DLB_VERSION_FCT
        ,DLB_VERSION_MTNC
        };
    return &v;
}
/* @} */


/**************************************************************************//**
@anchor instrumentation_abstraction
@name Instrumentation Abstraction

The instrumentation abstraction provides the ability to instrument the Dolby 
Intrinsics to collect statistics on their usage to obtain rough resource 
estimates.

Since the specifics of calculating estimates based on these statistics
should be tailored to the higher level code style, and your system and
processor architectures we provide an abstraction with a default
implementation that can be extended easily replaced with a custom
implementation.

An instrumentation abstraction may be selected by either defining one of the
instrumentation IDs below or by defining DLB_INSTRUMENT with the path
to an instrumentation abstraction file to include.
******************************************************************************/
/* @{ */


#if defined DLB_INSTRUMENT
    /* User-defined instrumentation abstraction. DLB_INSTRUMENTATION must be defined to
     * the relative path of the backend abstraction file.
     */
#elif defined DLB_INSTRUMENT_NULL
#   define DLB_INSTRUMENT "instrument/null/dlb_instrument_null.h"
#elif defined DLB_INSTRUMENT_COUNT
#   define DLB_INSTRUMENT "instrument/count/dlb_instrument_count.h"
#elif defined DLB_INSTRUMENT_PROFILER
#   define DLB_INSTRUMENT "instrument/profiler/dlb_instrument_profiler.h"
#else
    /** Relative path to instrumentation abstraction header to include.
     * DLB_INSTRUMENT will be automatically populated if one
     * of the natively supported targets is selected using a define such
     * as DLB_INSTRUMENT_SIMPLE. You can also define DLB_INSTRUEMENT before
     * including dlb_intrinsics (typically by using the -D option on your
     * compiler's command line) to use a custom abstraction.
     * If nothing is specified, no instrumentation will be performed.
     */
#   define DLB_INSTRUMENT "instrument/null/dlb_instrument_null.h"
#endif

/* Now we've got the path to the backend abstraction, include it. */
#include DLB_INSTRUMENT

/** Intrument abstraction probe.
 * This function prototype probe should fail to compile if the backend
 * abstraction has failed to define any of the basic functions.
 */
static inline
void
dlb_instrument_probe
    (void)
{
;
}

/* @} */

#endif

