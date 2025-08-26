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
 * @brief Compiler abstraction for clang (C99 language)
 */

/* clang already uses the standard C99 type names int32_t etc, so we don't
 * need to define them.
 */

#include <stdlib.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <limits.h> /* INT_MIN, INT_MAX */

#ifndef DLB_HAVE_UCLZ32
static DLB_PURE_FN inline unsigned int
DLB_Uclz32(uint32_t a) {
    return a ? (unsigned int)__builtin_clz(a) : 32; }
/* see http://comments.gmane.org/gmane.comp.compilers.clang.scm/45494 for a germane discussion about
 * clang/llvm issues related to gcc's explicitly undefined behaviour for zero.  Apparently the conditional
 * expression above will be folded to straight clz on ARM (where clz(0) is 32 as expected). */
#define DLB_HAVE_UCLZ32
#endif /* DLB_HAVE_UCLZ32 */

/* Helper to check that a float is not a NaN or a denorm */
static inline void
DLB_checkF(double x)
{
    switch(__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x))
    {
    case FP_NAN:
        DLB_print(DLB_ERROR("checkF+nan") "NaN detected.\n");
        break;
    case FP_INFINITE:
        DLB_print(DLB_ERROR("checkF+inf") "Infinity detected.\n");
        break;
    case FP_NORMAL:
        break;
    case FP_SUBNORMAL:
        DLB_print(DLB_ERROR("checkF+denorm") "Denorm detected.\n");
        break;
    case FP_ZERO:
        break;
    }
}
