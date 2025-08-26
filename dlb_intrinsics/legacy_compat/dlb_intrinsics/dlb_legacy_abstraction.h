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
 * @brief Dolby Intrinsics legacy names compatibility layer.
 * @deprecated This is a deprecated file and will not exist in future release.
 *
 * Do not include this file directly. It is included by dlb_abstraction.h,
 * via dlb_intrinsics.h.
 *
 * @addtogroup dlb_legacy_compat
 * For codebases that use legacy versions of Dolby Intrinsics, this file
 * defines mappings between the old and new #define names.
 * You can compile with DLB_INTRINSICS_NEW_NAMES defined to prevent this file
 * from being included.
 * @{
 */

#ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS


/**************************************************************************//**
@name legacy debug flag
@{
******************************************************************************/
#if defined DEBUG_TOOLS && !defined DLB_DEBUG
#define DLB_DEBUG 1
#endif

/** @} */

/**************************************************************************//**
@name legacy backend names
@{
******************************************************************************/
#if defined _X86_INT_
#   define DLB_BACKEND_GENERIC_RISC16X32
#elif defined _X86_FLOAT_ 
#   define DLB_BACKEND_GENERIC_FLOAT32
#elif defined _ETSI_         && !(defined DLB_BACKEND_ETSI)
#   define DLB_BACKEND_ETSI
#elif defined _ARMv4_  && !(defined DLB_BACKEND_ARM4)
#   define DLB_BACKEND_ARM4
#elif defined _ARMv5TE_  && !(defined DLB_BACKEND_ARM5TE)
#   define DLB_BACKEND_ARM5TE
#elif defined _ARMv6_  && !(defined DLB_BACKEND_ARM6)
#   define DLB_BACKEND_ARM6
#elif defined _ARMv7_  && !(defined DLB_BACKEND_ARM7INT)
#   define DLB_BACKEND_ARM7INT
#elif defined _ARMv7_NEON_ && !(defined DLB_BACKEND_ARM7INT_NEON)
#	define DLB_BACKEND_ARM7INT_NEON
#elif defined _SHARC_        && !(defined DLB_BACKEND_SHARC)
#   define DLB_BACKEND_SHARC
#elif defined _BF533_ && !(defined BIT_TRUE) && !(defined DLB_BACKEND_BF533)
#   define DLB_BACKEND_BF533
#elif defined _C55_          && !(defined DLB_BACKEND_C55)
#   define DLB_BACKEND_C55
#elif defined _C64_          && !(defined DLB_BACKEND_C64)
#   define DLB_BACKEND_C64
#elif (defined _C67_ || defined _C62_) && !(defined DLB_BACKEND_C6X)
#   define DLB_BACKEND_C6X
#elif defined _MIPS_         && !(defined DLB_BACKEND_MIPS)
#   define DLB_BACKEND_MIPS
#elif defined _XTENSA_       && !(defined DLB_BACKEND_XTENSA)
#   define DLB_BACKEND_XTENSA
#elif defined _TRIMEDIA_     && !(defined DLB_BACKEND_TRIMEDIA)
#   define DLB_BACKEND_TRIMEDIA
#elif defined _DLBDSP_       && !(defined DLB_BACKEND_MODEL_DLB16X32)
#   define DLB_BACKEND_MODEL_DLB16X32
#elif defined _DLBDSP_FLOAT_ && !(defined DLB_BACKEND_GENERIC_FLOAT32)
/*  Map deprecated DLBDSP_FLOAT to GENERIC_FLOAT32, too. */
#   define DLB_BACKEND_GENERIC_FLOAT32
#endif
/** @} */
/** @} */
#endif /* DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS */
