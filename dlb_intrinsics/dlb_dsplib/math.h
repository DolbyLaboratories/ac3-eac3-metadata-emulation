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
 * @brief Declarations for mathematical operations like
 *        divisions, transcendental functions etc.
 */

#ifndef _math_h_
#define _math_h_

#include "dlb_intrinsics.h"          /* defines the backend */
#include DLB_DSPLIB                  /* defines the backend header files */
#include DLB_MATH                    /* includes the backend specific types */

/**
  @addtogroup dlb_dsplib_math
  Scalar non-linear operations (power, log, division, transcendentals, etc).
  @{
  */

/**
   Square root of long fractional number.
*/
DLB_LFRACT
DLB_LsqrtL(DLB_LFRACT x  /**< Operand to square root in [0, 1.0] */
           );


/**
   Square root of short fractional number.
*/
DLB_SFRACT
DLB_SsqrtS(DLB_SFRACT x  /**< Operand to square root in [0, 1.0] */
           );


/**
   Reciprocal square root of long fractional number.

   Calculates \f$1/sqrt(x)\f$ and shifts down the result down by 1 bit
   to achieve a result between 0.5 and 1.0 for the allowed input range.
   Hence the function effectively calculates \f$0.5/sqrt(x)\f$.
*/
DLB_LFRACT  /** @return  Result of calculation */
DLB_LreciprtL(DLB_LFRACT x  /**< Operand in [0.25, 1.0] */
              );


/**
   Reciprocal square root of short fractional number.

   Calculates \f$1/sqrt(x)\f$ and shifts down the result down by 1 bit
   to achieve a result between 0.5 and 1.0 for the allowed input range.
   Hence the function effectively calculates \f$0.5/sqrt(x)\f$.
*/
DLB_SFRACT  /** @return  Result of calculation */
DLB_SreciprtS(DLB_SFRACT x  /**< Operand in [0.25, 1.0] */
              );


/**
  Calculates 2 ^ (x/y) for x<=0, y>0.
*/
DLB_LFRACT   /** @return  Result of calculation in [0, 1.0] */
DLB_Lpow2xy3232(int32_t x, /**< Numerator of fractional exponent in [INT32_MIN, 0] */
                int32_t y  /**< Denominator of fractional exponent in (0, INT32_MAX] */
                );
                
/**
  Calculates log2(x) for x in [0.5, 1.0].
  This is the logarithm calculation for normalized input.
*/
DLB_LFRACT   /** @return  Result of calculation in [-1.0, 0] */
DLB_Llog2poly2normL (DLB_LFRACT   x  /**< Argument of the log2 as positive fractional value in [0.5, 1.0] */
                    );
                
/**
  Calculates log2(x)/(2^scale) for x in [0, 1.0].
  This normalizes the input x and calls the DLB_Llog2poly2L() function.
  Because the input is not normalized, the output has to be scaled.
  For possible input values in the full range of [0, 1.0] the 2^scale value should be at least DLB_LFRACT_MANTBITS.
*/
DLB_LFRACT   /** @return  Result of calculation in [-1.0, 0] */
DLB_Llog2poly2LU(DLB_LFRACT   x      /**< Argument of the log2 as positive fractional value in [0, 1.0] */
                ,unsigned int scale  /**< Shift value for output scaling */
                ); 
                 
/**
   Calculates sin(4*x), -PI<4*x<PI
*/
DLB_LFRACT   /** @return  Result of calculation in [-1.0, 1.0] */
DLB_Lsin4L(DLB_LFRACT x   /**< Operand in [-PI/4, PI/4] */
	   );


/**
   Calculates cos(4*x), -PI<4*x<PI
*/
DLB_LFRACT   /** @return  Result of calculation in [-1.0, 1.0] */
DLB_Lcos4L(DLB_LFRACT x   /**< Operand in [-PI/4, PI/4] */
	   );


/**
   Division of two long fractional numbers with long fractional result.
*/
DLB_LFRACT   /** @return  Quotient x/y */
DLB_LdivLL(DLB_LFRACT x,  /**< Dividend */
           DLB_LFRACT y   /**< Divisor */
           );


/**
   Division of two long fractional numbers with short fractional result.
*/
DLB_SFRACT   /** @return  Quotient x/y */
DLB_SdivLL(DLB_LFRACT x,  /**< Dividend in [-1.0, 1.0] */
           DLB_LFRACT y   /**< Divisor in [-1.0, 1.0] with abs(y) >= abs(x) */
           );

/**
    Reciprocal of an integer with a short fractional result.
*/
DLB_SFRACT            /** @return 1/x */
DLB_SrecipI(int x     /**< Divisor (of one) */
           );

/**
    Reciprocal of an integer with a long fractional result.
*/
DLB_LFRACT            /** @return 1/x */
DLB_LrecipI(int x     /**< Divisor (of one) */
           );

/** @} */

#endif /* #ifndef _math_h_ */

