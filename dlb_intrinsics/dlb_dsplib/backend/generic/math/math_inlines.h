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
 * @brief Generic implementation of a subset of math.h by inline functions.
 */

#ifndef _math_inlines_h_
#define _math_inlines_h_

#include <assert.h>
#include "dlb_intrinsics.h"


#ifdef DLB_METHOD_IS_FLOAT

static inline DLB_LFRACT
DLB_LdivLL(DLB_LFRACT x, DLB_LFRACT y)
{
  assert(DLB_F_L(y) != 0.0);
  return DLB_L_F( DLB_F_L(x)/DLB_F_L(y) );
}


static inline DLB_SFRACT
DLB_SdivLL(DLB_LFRACT x, DLB_LFRACT y)
{
  assert(DLB_F_L(y) != 0.0);
  return DLB_S_F( DLB_F_L(x)/DLB_F_L(y) );
}


/* use standard mathlib for transcendent functions on floating point targets */
#include <math.h>


/* Map standard double precision math functions to 'f' flavour definitions 
 * for compilers supporting C99 math and use single precision backend.
 * Condition used for double precision backend is: DLB_LFRACT_BITS > 32 for
 * functions operating on and returning LFRACT and DLB_SFRACT_BITS > 32 for
 * functions operating on and returning SFRACT.
 *
 * A future backend could map, for ex., DLB_LsqrtL to double precision
 * DLB_SsqrtS to single precision using LFRACT and SFRACT BITS.
 * 
 */


/* For functions operating on and returning DLB_LFRACT */

static inline DLB_LFRACT
DLB_LreciprtL(DLB_LFRACT op)
{
  assert(DLB_F_L(op) >= 0.25);
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  return DLB_L_F( 0.5/sqrtf((float)DLB_F_L(op)) );
#else
  return DLB_L_F( 0.5/sqrt(DLB_F_L(op)) );
#endif
}

static inline DLB_LFRACT
DLB_LsqrtL(DLB_LFRACT op)
{
  assert(DLB_F_L(op) >= 0.0);
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  return DLB_L_F( sqrtf((float)DLB_F_L(op)) );
#else
  return DLB_L_F( sqrt(DLB_F_L(op)) );
#endif
}

static inline DLB_LFRACT
DLB_Lsin4L(DLB_LFRACT xdiv4)
{
  DLB_LFRACT retval;
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  retval = DLB_L_F( sinf((float)DLB_F_L(xdiv4)*4.0f) );
#else
  retval = DLB_L_F( sin(DLB_F_L(xdiv4)*4.0) );
#endif
  return(retval);
}

static inline DLB_LFRACT
DLB_Lcos4L(DLB_LFRACT xdiv4)
{
  DLB_LFRACT retval;
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  retval = DLB_L_F( cosf((float)DLB_F_L(xdiv4)*4.0f) );
#else
  retval = DLB_L_F( cos(DLB_F_L(xdiv4)*4.0) );
#endif
  return(retval);
}

static inline DLB_LFRACT
DLB_Llog2poly2normL(DLB_LFRACT  logarg)
{
  DLB_LFRACT retval;
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  retval = DLB_L_F( logf((float)DLB_F_L(logarg))/logf(2.0f) );
#else
  retval = DLB_L_F( log(DLB_F_L(logarg))/log(2.0) );
#endif
  return(retval);
}

static inline DLB_LFRACT
DLB_Llog2poly2LU(DLB_LFRACT   logarg
                ,unsigned int scale
                )
{
  DLB_LFRACT retval;
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  retval = DLB_L_F( logf((float)DLB_F_L(logarg))/logf(2.0f)*powf(0.5f,(float)scale) );
#else
  retval = DLB_L_F( log(DLB_F_L(logarg))/log(2.0)*pow(0.5,(double)scale) );
#endif
  return(retval);
}

/* For functions operating on and returning DLB_SFRACT */

static inline DLB_SFRACT
DLB_SreciprtS(DLB_SFRACT op)
{
  assert(DLB_F_S(op) >= 0.25);
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_SFRACT_BITS <= 32))
  return DLB_S_F( 0.5/sqrtf((float)DLB_F_S(op)) );
#else
  return DLB_S_F( 0.5/sqrt(DLB_F_S(op)) );
#endif
}

static inline DLB_SFRACT
DLB_SsqrtS(DLB_SFRACT op)
{
  assert(DLB_F_S(op) >= 0.0);
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_SFRACT_BITS <= 32))
  return DLB_S_F( sqrtf((float)DLB_F_S(op)) );
#else
  return DLB_S_F( sqrt(DLB_F_S(op)) );
#endif
}

static inline DLB_LFRACT
DLB_Lpow2xy3232(int32_t x,
                int32_t y
                )
{
#if (defined(DLB_HAVE_C99_FLOAT_MATH_FN) && (DLB_LFRACT_BITS <= 32))
  return DLB_L_F(powf(2.0f,(float)x /(float)y));
#else
  return DLB_L_F(pow(2.0,(double)x /(double)y));
#endif
}


static inline DLB_SFRACT
DLB_SrecipI(int x)
{
    return DLB_S_F(1.0 / x);
}

static inline DLB_LFRACT
DLB_LrecipI(int x)
{
    return DLB_L_F(1.0 / x);
}

#endif /* DLB_METHOD_IS_FLOAT */

#endif /* #ifndef _math_inlines_h_ */

