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
 * @file  dlb_util_intops.h
 *
 * @brief Useful integer operations that are not provided by C itself, with function
 *        names that match the rest of the intrinsics, reducing surprise.
 *
 *        Applies the functions min, max and abs to the data types
 *        int, int32_t, int16_t and unsigned int in the expected fashion.
 */

#ifndef DLB_UTIL_INTOPS_H
#define DLB_UTIL_INTOPS_H

#include "dlb_compiler.h"

/* Include working definitions: possibly back-end specific */
#ifndef DLB_UTIL_INTOPS
#include "backend/generic/dlb_std_util_intops.h"
#else
#include DLB_UTIL_INTOPS
#endif /* DLB_UTIL_INTOPS */


#ifndef DLB_SKIP_INTRINSIC_PROTOTYPES
/**
  @addtogroup dlb_util_intops
  Utility integer operations not part of standard C
  @{
  */

#ifndef DLB_UmaxUU
unsigned int /** @return unsigned maximum of two unsigned arguments */
DLB_UmaxUU( unsigned int a, unsigned int b);
#endif

#ifndef DLB_UminUU
unsigned int /** @return unsigned minimum of two unsigned arguments */
DLB_UminUU(unsigned int a, unsigned int b);
#endif

#ifndef DLB_UclipUUU
unsigned int /** @return unsigned argument constrained to range [lo, hi] */
DLB_UclipUUU(unsigned int a, unsigned int lo, unsigned int hi);
#endif

#ifndef DLB_ImaxII
int /** @return signed maximum of two int arguments */
DLB_ImaxII(int a, int b);
#endif

#ifndef DLB_IminII
int /** @return signed minimum of two int arguments */
DLB_IminII(int a, int b);
#endif

#ifndef DLB_IabsI
int /** @return signed absolute value of int argument.
     *  Result for a == INT_MIN is undefined */
DLB_IabsI(int a);
#endif

#ifndef DLB_IclipIII
int /** @return signed argument constrained to range [lo, hi]. */
DLB_IclipIII(int a, int lo, int hi);
#endif

#ifndef DLB_32max3232
int32_t /** @return signed 32-bit integer maximum of two int32_t arguments */
DLB_32max3232(int32_t a, int32_t b);
#endif

#ifndef DLB_32min3232
int32_t /** @return signed 32-bit integer minimum of two int32_t arguments */
DLB_32min3232(int32_t a, int32_t b);
#endif

#ifndef DLB_32abs32
int32_t /** @return signed 32-bit integer absolute value int32_t argument.
         *  Result for a == INT32_MIN is undefined */
DLB_32abs32(int32_t a);
#endif

#ifndef DLB_32clip323232
int32_t /** @return signed argument constrained to range [lo, hi]. */
DLB_32clip323232(int32_t a, int32_t lo, int32_t hi);
#endif

#ifndef DLB_16max1616
int16_t /** @return signed 16-bit integer maximum of two int16_t arguments */
DLB_16max1616(int a, int b);
#endif

#ifndef DLB_16min1616
int16_t /** @return signed 16-bit integer minimum of two int16_t arguments */
DLB_16min1616(int a, int b);
#endif

#ifndef DLB_16abs16
int16_t /** @return signed 16-bit integer absolute value of int16_t argument.
         *  Result for a == INT16_MIN is undefined */
DLB_16abs16(int a);
#endif

#ifndef DLB_16clip161616
int16_t /** @return signed argument constrained to range [lo, hi]. */
DLB_16clip161616(int a, int lo, int hi);
#endif

#ifndef DLB_Uones32
unsigned int /** @return population count of set bits in uint32_t argument */
DLB_Uones32(uint32_t a);
#endif

#ifndef DLB_Uclz32
unsigned int /** @return count of leading zeros in uint32_t argument.
              *  Result for a == 0 is 32. */
DLB_Uclz32(uint32_t a);
#endif

#ifndef DLB_Uclz64
unsigned int /** @return count of leading zeros in uint64_t argument.
              *  Result for a == 0 is 64 */
DLB_Uclz64(uint64_t a);
#endif

/**
 * A bit-reverse word function.
 */
#ifndef DLB_UbitrevU
unsigned int /** @return bits of i in opposite bit-order */
DLB_UbitrevU(unsigned int i);
#endif

#endif /* DLB_SKIP_INTRINSIC_PROTOTYPES */

#endif /* DLB_UTIL_INTOPS_H */
