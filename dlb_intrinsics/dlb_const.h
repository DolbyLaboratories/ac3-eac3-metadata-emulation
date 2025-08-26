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
 * @brief Static Definitions of Frequently Used Constants
 */


#ifndef DLB_CONST_H
#define DLB_CONST_H

/**
@addtogroup dlb_intrinsics_core
@{

@name Constant definitions
Frequently used constants are provided by the Dolby Intrinsics headers to 
each source file individually as static definitions. 
In contrast to the initializer operators DLB_LcF() and DLB_ScF(), 
these can be used directly in expressions.

Unfortunately, because of a quirk in the C90 standard that was fixed
in C99 but not picked up by some important compilers, these can't be
used in struct initialisers, or other "static initialiser" context,
so use the DLB_LcF and DLB_ScF forms directly, there.

@{
*/

static const DLB_LFRACT DLB_L10     = DLB_LcF(1.0);               /**< DLB_LFRACT constant for 1.0       */
static const DLB_LFRACT DLB_LSQRT05 = DLB_LcF(0.70710678118655);  /**< DLB_LFRACT constant for sqrt(0.5) */
static const DLB_LFRACT DLB_L05     = DLB_LcF(0.5);               /**< DLB_LFRACT constant for 0.5       */
static const DLB_LFRACT DLB_L00	    = DLB_LcF(0.0);               /**< DLB_LFRACT constant for 0.0       */
static const DLB_LFRACT DLB_LNEG05  = DLB_LcF(-0.5);              /**< DLB_LFRACT constant for -0.5      */
static const DLB_LFRACT DLB_LNEG10  = DLB_LcF(-1.0);              /**< DLB_LFRACT constant for -1.0      */

static const DLB_SFRACT DLB_S10     = DLB_ScF(1.0);               /**< DLB_SFRACT constant for 1.0       */
static const DLB_SFRACT DLB_SSQRT05 = DLB_ScF(0.70710678118655);  /**< DLB_SFRACT constant for sqrt(0.5) */
static const DLB_SFRACT DLB_S05     = DLB_ScF(0.5);               /**< DLB_SFRACT constant for 0.5       */
static const DLB_SFRACT DLB_S00     = DLB_ScF(0.0);               /**< DLB_SFRACT constant for 0.0       */
static const DLB_SFRACT DLB_SNEG05  = DLB_ScF(-0.5);              /**< DLB_SFRACT constant for -0.5      */
static const DLB_SFRACT DLB_SNEG10  = DLB_ScF(-1.0);              /**< DLB_SFRACT constant for -1.0      */

/** 
@} @} 
*/

#endif /*DLB_CONST_H*/
