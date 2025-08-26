/****************************************************************************
 *
 *
 * Copyright (c) 2011-2013 Dolby International AB.
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
 * @brief Macros that abstract the TI CL6X compiler language extension keywords 
 */

#include <assert.h>

/* cl6x has "inline" and "restrict" keywords already */
#define DLB_RESTRICT restrict
/* cl6x probably has a way to indicate that functions are pure */
/* but I haven't figured it out, yet */

/* SPRU187S says this works when compiled with --relaxed-ansi or --gcc options
 * (we should do that!) */
#ifdef __TI_GNU_ATTRIBUTE_SUPPORT__
#define DLB_ALIGN          __attribute__ ((aligned (32)))
#define DLB_PURE_FN        
#else
#define DLB_PURE_FN
#define DLB_ALIGN
#endif

#define PRAGMA(x) _Pragma(#x)
#define DLB_LOOP_COUNT_INFO(min, multiple) \
   PRAGMA(MUST_ITERATE(min, , multiple))
#define DLB_LOOP_UNROLL(x) \
   PRAGMA(UNROLL(x))
#define DLB_PTR_IS_ALIGNED(ptr, pow2) \
   _nassert(((unsigned int) (ptr) & (pow2)-1) == 0);

#ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS 
/* Memory alignment macros.
 * Note: The following macros are deprecated and should not be used anymore. */
#define DLB_ALIGN_4_BYTES  DLB_ALIGN /* Align memory to (at least) 4 bytes. */
#define DLB_ALIGN_8_BYTES  DLB_ALIGN /* Align memory to (at least) 8 bytes. */
#define DLB_ALIGN_16_BYTES DLB_ALIGN /* Align memory to (at least) 16 bytes. */
#endif /* DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS */

