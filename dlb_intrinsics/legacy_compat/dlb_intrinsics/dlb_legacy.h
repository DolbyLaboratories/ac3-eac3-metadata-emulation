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
 * Do not include this file directly. It is included by
 * dlb_abstraction.h, via dlb_intrinsics.h.
 *
 * @addtogroup dlb_legacy_compat
 * @{
 *
 * @deprecated This is a deprecated module and will not be supported in future release.
 *
 * @name  legacy type renaming
 * @{
 * These are macros instead of typedefs so it's a true find and replace
 * that's completely invisible to the compiler (after preprocessing).
 */

 #ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS

/** Legacy long fractional type.
 * @note It's no longer generally true that this type is in Q1.31 format.
 */
#define DLB_fract32 DLB_LFRACT

/** Legacy short fractional type.
 * @note It's no longer generally true that this type is in Q1.15 format.
 */
#define DLB_fract16 DLB_SFRACT

/** Legacy 32 bit integer type.
 * New code should use the C99 standard type int32_t.
 */
#define DLB_int32 int32_t

/** Legacy 16 bit integer type.
 * New code should use the C99 standard type int16_t.
 */
#define DLB_int16 int16_t

/** Legacy 32 bit unsigned integer type.
 * New code should use the C99 standard type uint32_t.
 */
#define DLB_uint32 uint32_t

/** Legacy 16 bit unsigned integer type.
 * New code should use the C99 standard type uint16_t.
 */
#define DLB_uint16 uint16_t

/* @} */

/**************************************************************************//**
@name legacy type sizes
@{
******************************************************************************/

#define SFRACT_BITS DLB_SFRACT_BITS
#define FRACT_BITS  DLB_LFRACT_BITS
#define DFRACT_BITS 32
#define SHORT_BITS  16
#define INT_BITS    32

/* @} */

/**************************************************************************//**
@name legacy intrinsic function renaming
Map old names to new names.
@{
******************************************************************************/

#define SFRACT(f)                   DLB_ScF(f)
#define FRACT(f)                    DLB_LcF(f)

#define SFRACT_FIX_SCALE            (2u*DLB_ScF(0.5))
#define FRACT_FIX_SCALE             (2u*DLB_LcF(0.5))
#define SFRACT2FRACT(x)             DLB_L_S(x)
#define FRACT2SFRACT(x)             DLB_StruncL(x)

#define SFRACT2INT(x)               DLB_16srndS(x)
#define FRACT2INT(x)                DLB_32srndL(x)
#define INT2SFRACT(x)               DLB_S_16(x)
#define INT2FRACT(x)                DLB_L_32(x)

#define SFRACT2FLOAT(x)             DLB_F_S(x)
#define FRACT2FLOAT(x)              DLB_F_L(x)

#define DLB_INT_MAX                 (0x7fffffff)
#define DLB_INT_MIN                 (-0x7fffffff-1)
#define DLB_SHORT_MAX               (0x7fff)
#define DLB_SHORT_MIN               (-0x7fff-1)
#define int64                       int64_t

#define DLB_min(a, b)               ((a) < (b) ? (a) : (b))
#define DLB_max(a, b)               ((a) > (b) ? (a) : (b))

#define DLB_shl(x, i)               DLB_LshlLI(x, i)
#define DLB_shr(x, i)               DLB_LshrLI(x, i)
#define DLB_sshl(x, i)              DLB_LsshlLI(x, DLB_IlimlI(i))
#define DLB_sshr(x, i)              DLB_LsshrLI(x, DLB_IlimlI(i))

#define DLB_unishl(x, i)            DLB_LshlLU(x, i)
#define DLB_unishr(x, i)            DLB_LshrLU(x, i)
#define DLB_sunishl(x, i)           DLB_LsshlLU(x, i)

#define DLB_sadd(x, y)              DLB_LsaddLL(x, y)
#define DLB_sadd16(x, y)            DLB_SsaddSS(x, y)
#define DLB_add32(x, y)             DLB_LaddLL(x, y)
#define DLB_add16(x, y)             DLB_SaddSS(x, y)

#define DLB_sneg32(x)               DLB_LsnegL(x)
#define DLB_sneg16(x)               DLB_SsnegS(x)

#define DLB_ssub(x, y)              DLB_LssubLL(x, y)
#define DLB_ssub16(x, y)            DLB_SssubSS(x, y)
#define DLB_sub32(x, y)             DLB_LsubLL(x, y)
#define DLB_sub16(x, y)             DLB_SsubSS(x, y)

#define DLB_srnd(x)                 DLB_SsrndL(x)

#define DLB_sabs32(x)               DLB_LsabsL(x)
#define DLB_sabs16(x)               DLB_SsabsS(x)

#define DLB_norm32(x)               DLB_UnormL(x)
#define DLB_norm16(x)               DLB_UnormS(x)

#define DLB_mpy16x16ll(x, y)        DLB_LsmpySS(DLB_L(x), DLB_L(y))
#define DLB_mpy16x16hl(x, y)        DLB_LsmpySS(DLB_H(x), DLB_L(y))
#define DLB_mpy32x16l(x, y)         DLB_LsmpyLS(x, DLB_L(y))
#define DLB_mpy16x16hh(x, y)        DLB_LsmpySS(DLB_H(x), DLB_H(y))
#define DLB_mpy32x16h(x, y)         DLB_LsmpyLS(x, DLB_H(y))
#define DLB_mpy32x32(x, y)          DLB_LsmpyLL(x, y)

#define DLB_mpy16x16llx(x, y)       DLB_LmpySS(DLB_L(x), DLB_L(y))
#define DLB_mpy16x16hlx(x, y)       DLB_LmpySS(DLB_H(x), DLB_L(y))
#define DLB_mpy32x16lx(x, y)        DLB_LmpyLS(x, DLB_L(y))
#define DLB_mpy16x16hhx(x, y)       DLB_LmpySS(DLB_H(x), DLB_H(y))
#define DLB_mpy32x16hx(x, y)        DLB_LmpyLS(x, DLB_H(y))
#define DLB_mpy32x32x(x, y)         DLB_LmpyLL(x, y)

#define DLB_mpyr16x16ll(x, y)       DLB_SsrmpySS(x, y)

#define DLB_mac16x16ll(a, x, y)     DLB_LsmacLSS(a, DLB_L(x), DLB_L(y))
#define DLB_mac16x16hl(a, x, y)     DLB_LsmacLSS(a, DLB_H(x), DLB_L(y))
#define DLB_mac32x16l(a, x, y)      DLB_LsmacLLS(a, x, DLB_L(y))
#define DLB_mac16x16hh(a, x, y)     DLB_LsmacLSS(a, DLB_H(x), DLB_H(y))
#define DLB_mac32x16h(a, x, y)      DLB_LsmacLLS(a, x, DLB_H(y))
#define DLB_mac32x32(a, x, y)       DLB_LsmacLLL(a, x, y)

#define DLB_mac16x16llx(a, x, y)    DLB_LmacLSS(a, DLB_L(x), DLB_L(y))
#define DLB_mac16x16hlx(a, x, y)    DLB_LmacLSS(a, DLB_H(x), DLB_L(y))
#define DLB_mac32x16lx(a, x, y)     DLB_LmacLLS(a, x, DLB_L(y))
#define DLB_mac16x16hhx(a, x, y)    DLB_LmacLSS(a, DLB_H(x), DLB_H(y))
#define DLB_mac32x16hx(a, x, y)     DLB_LmacLLS(a, x, DLB_H(y))
#define DLB_mac32x32x(a, x, y)      DLB_LmacLLL(a, x, y)

#define DLB_msu16x16ll(a, x, y)     DLB_LsmsuLSS(a, DLB_L(x), DLB_L(y))
#define DLB_msu16x16hl(a, x, y)     DLB_LsmsuLSS(a, DLB_H(x), DLB_L(y))
#define DLB_msu32x16l(a, x, y)      DLB_LsmsuLLS(a, x, DLB_L(y))
#define DLB_msu16x16hh(a, x, y)     DLB_LsmsuLSS(a, DLB_H(x), DLB_H(y))
#define DLB_msu32x16h(a, x, y)      DLB_LsmsuLLS(a, x, DLB_H(y))
#define DLB_msu32x32(a, x, y)       DLB_LsmsuLLL(a, x, y)

#define DLB_msu16x16llx(a, x, y)    DLB_LmsuLSS(a, DLB_L(x), DLB_L(y))
#define DLB_msu16x16hlx(a, x, y)    DLB_LmsuLSS(a, DLB_H(x), DLB_L(y))
#define DLB_msu32x16lx(a, x, y)     DLB_LmsuLLS(a, x, DLB_L(y))
#define DLB_msu16x16hhx(a, x, y)    DLB_LmsuLSS(a, DLB_H(x), DLB_H(y))
#define DLB_msu32x16hx(a, x, y)     DLB_LmsuLLS(a, x, DLB_H(y))
#define DLB_msu32x32x(a, x, y)      DLB_LmsuLLL(a, x, y)

/** @} */


/**************************************************************************//**
@name Depricated legacy intrinsic function replacements

Generic C version of legacy intrinsic functions that cannot be directly mapped
to one of the new operators.

@{
******************************************************************************/
static inline
DLB_fract32 DLB_add16x16ll_16x16ll (DLB_fract16 x, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 y, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 a, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 b  /*!< 16-bit operand, use lower halfword */
                                  )
{
  DLB_fract32 result;


  result = DLB_sadd(DLB_unishr(DLB_mpy16x16ll(x,y), 1), DLB_unishr(DLB_mpy16x16ll(a,b), 1));

  return result;
}

/**

Sum of 16x16 products, don't care about saturation.

This function calculates the sum of the half of two products,
i.e.: (x*y)/2 + (a*b)/2.

\return 32-bit accumulated value
*/
static inline
DLB_fract32 DLB_add16x16llx_16x16llx(DLB_fract16 x, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 y, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 a, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 b  /*!< 16-bit operand, use lower halfword */
                                  )
{
  DLB_fract32 result;


  result = DLB_add32(DLB_unishr(DLB_mpy16x16llx(x,y), 1), DLB_unishr(DLB_mpy16x16llx(a,b), 1));

  return result;
}

/**

Sum of 32x16 products with saturation.

This function calculates the sum of the half of two products,
i.e.: (x*y)/2 + (a*b)/2.

\return 32-bit accumulated value
*/
static inline
DLB_fract32 DLB_add32x16l_32x16l (DLB_fract32 x, /*!< 32-bit operand */
                                DLB_fract16 y, /*!< 16-bit operand */
                                DLB_fract32 a, /*!< 32-bit operand */
                                DLB_fract16 b  /*!< 16-bit operand */
                                )
{
  DLB_fract32 result;

  result = DLB_sadd(DLB_unishr(DLB_mpy32x16l(x,y), 1), DLB_unishr(DLB_mpy32x16l(a,b), 1));

  return result;
}

/**

Sum of 32x16 products, don't care about saturation.

This function calculates the sum of the half of two products,
i.e.: (x*y)/2 + (a*b)/2.

\return 32-bit accumulated value
*/
static inline
DLB_fract32 DLB_add32x16lx_32x16lx(DLB_fract32 x, /*!< 32-bit operand */
                                DLB_fract16 y, /*!< 16-bit operand */
                                DLB_fract32 a, /*!< 32-bit operand */
                                DLB_fract16 b  /*!< 16-bit operand */
                                )
{
  DLB_fract32 result;

  result = DLB_add32(DLB_unishr(DLB_mpy32x16lx(x,y), 1), DLB_unishr(DLB_mpy32x16lx(a,b), 1));

  return result;
}

/**

Difference of 16x16 products.

This function calculates the difference of the half of two products,
i.e.: (x*y)/2 - (a*b)/2.

\return 32-bit result
*/
static inline
DLB_fract32 DLB_sub16x16ll_16x16ll (DLB_fract16 x, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 y, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 a, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 b  /*!< 16-bit operand, use lower halfword */
                                  )
{
  DLB_fract32 result;

  result = DLB_ssub(DLB_unishr(DLB_mpy16x16ll(x,y), 1), DLB_unishr(DLB_mpy16x16ll(a,b), 1));

  return result;
}

/**

Difference of 16x16 products, don't care about saturation.

This function calculates the difference of the half of two products,
i.e.: (x*y)/2 - (a*b)/2.

\return 32-bit result
*/
static inline
DLB_fract32 DLB_sub16x16llx_16x16llx(DLB_fract16 x, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 y, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 a, /*!< 16-bit operand, use lower halfword */
                                  DLB_fract16 b  /*!< 16-bit operand, use lower halfword */
                                  )
{
  DLB_fract32 result;

  result = DLB_sub32(DLB_unishr(DLB_mpy16x16llx(x,y), 1), DLB_unishr(DLB_mpy16x16llx(a,b), 1));

  return result;
}

/**

Difference of 32x16 products.

This function calculates the difference of the half of two products,
i.e.: (x*y)/2 - (a*b)/2.

\return 32-bit result
*/
static inline
DLB_fract32 DLB_sub32x16l_32x16l (DLB_fract32 x, /*!< 32-bit operand */
                                DLB_fract16 y, /*!< 16-bit operand */
                                DLB_fract32 a, /*!< 32-bit operand */
                                DLB_fract16 b  /*!< 16-bit operand */
                                )
{
  DLB_fract32 result;

  result = DLB_ssub(DLB_unishr(DLB_mpy32x16l(x,y), 1), DLB_unishr(DLB_mpy32x16l(a,b), 1));

  return result;
}

/**

Difference of 32x16 products, don't care about saturation.

This function calculates the difference of the half of two products,
i.e.: (x*y)/2 - (a*b)/2.

\return 32-bit result
*/
static inline
DLB_fract32 DLB_sub32x16lx_32x16lx(DLB_fract32 x, /*!< 32-bit operand */
                                DLB_fract16 y, /*!< 16-bit operand */
                                DLB_fract32 a, /*!< 32-bit operand */
                                DLB_fract16 b  /*!< 16-bit operand */
                                )
{
  DLB_fract32 result;

  result = DLB_sub32(DLB_unishr(DLB_mpy32x16lx(x,y), 1), DLB_unishr(DLB_mpy32x16lx(a,b), 1));

  return result;
}

/** @} */


/**
  Cast a DLB_fract32 to a DLB_fract16 with saturation but without scaling.

  Actually, the floating point version explicitly does no saturation, so
  we must take care to do the same here.

\return 16-bit result
*/
static inline
DLB_fract16 DLB_sat16(DLB_fract32 x /*!< 32-bit operand */
                     )
{
  /* When no saturation occurs, the shift amounts in DLB_StruncL()
     and DLB_LsshlLU() compensate. */
  return DLB_StruncL(DLB_LsshlLU(x, DLB_LFRACT_MANTBITS-DLB_SFRACT_MANTBITS));
}

/**
  Get size of object a in units of b, rounded to next higher number.

  The macro SIZEOF is used to get the size of an object (e.g. an array or
  structure) in a given unit.

  The most common application is aligning objects in memory. Because most
  32-bit platforms use a 32-bit alignment, the second argument is usually
  DLB_fract32, e.g.:

\verbatim
  typedef struct DECODER_STRUCT
  {
    DLB_int16         maxNumChannels;
    ...
  } DECODER_STRUCT;

  DLB_int16           buffer[2048]
  DLB_uint32          MemorySize;

  MemorySize += SIZEOF(DECODER_STRUCT, DLB_fract32);
  MemorySize += SIZEOF(buffer,         DLB_fract32);
\endverbatim

  In this example, the result from SIZEOF() is in 32-bit units, because
  DLB_fract32 was specified as the second argument. Therefore, in order
  to get the value in bytes, the result from the macro must be multiplied
  by the size of the unit, which is four in this case.

  The resulting value is always rounded to the the next number for the
  given unit. That is, if the size of the first argument is 7 bytes, and
  the unit is 32-bits, the resulting value is two 32-bit words, which is
  equivalent to 8 bytes.

  \return Result in the specified unit
*/
#ifndef SIZEOF
#define SIZEOF(a,b) ((sizeof(a) + sizeof(b)-1) / sizeof(b))
#endif


/**
  Get size of the value a in units of b, rounded to next higher number.

  The macro SIZE is used to get convert a given size in bytes to a given
  unit. For example, it can be used to convert sizes in bytes to sizes in
  32-bit words.

  The most common application is aligning objects in memory. Because most
  32-bit platforms use a 32-bit alignment, the second argument is usually
  DLB_fract32, e.g.:

\verbatim
  #define MAX_NUMBER_COEFF      7

  DLB_uint32          MemorySize;

  MemorySize += SIZE(MAX_NUMBER_COEFFS * sizeof(DLB_fract16), DLB_fract32);
\endverbatim

  In this example, the result from SIZE() is in 32-bit units, because
  DLB_fract32 was specified as the second argument. Therefore, in order
  to get the value in bytes, the result from the macro must be multiplied
  by the size of the unit, which is four in this case.

  The resulting value is always rounded to the the next number for the
  given unit. That is, if the first argument is 14 bytes, and the unit is
  32-bits, the resulting value is four 32-bit words, which is equivalent to
  16 bytes.

  \return Result in the specified unit
*/
#ifndef SIZE
#define SIZE(a,b) ((a) % sizeof(b) ? (a)/sizeof(b)+1 : (a)/sizeof(b))
#endif


#if defined(DLB_BACKEND_ARM5TE)  || defined(DLB_BACKEND_ARM6) || defined(DLB_BACKEND_ARM7INT) || defined(DLB_BACKEND_ARM7INT_NEON)
typedef unsigned long long uint64;
#endif

/**
  @name Legacy mappings for dlb_dsplib functions

  This mapping does not insert open/close calls for FFTs and DCTs and
  will not work with backends storing coefficients in the handle.
  It does also not perform (de)interleaving of input/output data which
  may be required for some backends.
  There are currently no mappings for real-valued FFTs provided.
*/
/** @{ */
/* QMF/hybrid filter bank */
#define QueryMemSizeQmfAnalysis(a,b,c,d,e,f)   DLB_qmf_analysis_query_mem(a,b,c,d,e,f)
#define QueryMemSizeQmfSynthesis(a,b,c,d,e,f)  DLB_qmf_synthesis_query_mem(a,b,c,d,e,f)
#define OpenQmfAnalysis(a,b,c,d,e,f,g,h)       DLB_qmf_analysis_open(a,b,c,d,e,f,g,h) 
#define OpenQmfSynthesis(a,b,c,d,e,f,g,h)      DLB_qmf_synthesis_open(a,b,c,d,e,f,g,h)
#define InitQmfAnalysis(a)                     DLB_qmf_analysis_init(a)
#define SetQmfAnalysisSlotIncrement(a,b)       DLB_qmf_analysis_set_slot_increment(a,b)
#define InitQmfSynthesis(a)                    DLB_qmf_synthesis_init(a)
#define SetQmfSynthesisSlotIncrement(a,b)      DLB_qmf_synthesis_set_slot_increment(a,b)
#define ProcessQmfAnalysis(a,b,c)              DLB_qmf_analysis(a,b,c)
#define ProcessQmfSynthesis(a,b,c)             DLB_qmf_synthesis(a,b,c)
#define CopyQmfSynthesisStates(a,b)            DLB_qmf_synthesis_copy_states(a,b)
#define QueryMemSizeHybridAnalysis(a,b,c,d)    DLB_hybrid_analysis_query_mem(a,b,c,d)
#define QueryMemSizeHybridSynthesis(a,b,c,d)   DLB_hybrid_synthesis_query_mem(a,b,c,d)
#define OpenHybridAnalysis(a,b,c,d,e)          DLB_hybrid_analysis_open(a,b,c,d,e)
#define OpenHybridSynthesis(a,b,c,d,e)         DLB_hybrid_synthesis_open(a,b,c,d,e)
#define InitHybridAnalysis(a)                  DLB_hybrid_analysis_init(a)
#define InitHybridSynthesis(a)                 DLB_hybrid_synthesis_init(a)
#define ProcessHybridAnalysis(a,b,c)           DLB_hybrid_analysis(a,b,c)
#define ProcessHybridSynthesis(a,b,c)          DLB_hybrid_synthesis(a,b,c)
#define SetScaleHybridStates(a,b)              DLB_hybrid_analysis_shl(a,b)

#define GetScaleHybridStates(h,p)     *(p) = DLB_hybrid_analysis_get_scale(h)
#define GetDelayHybridAnalysis(h,p)   *(p) = DLB_hybrid_analysis_get_delay(h)
#define GetDelayHybridSynthesis(h,p)  *(p) = DLB_hybrid_synthesis_get_delay(h)
#define GetDelayQmfAnalysis(h,p)      *(p) = DLB_qmf_analysis_get_delay(h)
#define GetDelayQmfSynthesis(h,p)     *(p) = DLB_qmf_synthesis_get_delay(h)

/* DCTs for 16 bit data */
#define dctII_16x16_IP(d,s,l)    do { DLB_HANDLE_DCT2 hDCT=DLB_dct2_open(l); DLB_Sdct2SU(hDCT, d, d, l); } while(0)
#define dctIII_16x16_IP(d,s,l)   do { DLB_HANDLE_DCT3 hDCT=DLB_dct3_open(l); DLB_Sdct3SU(hDCT, d, d, l); } while(0)
#define dctIV_16x16_IP(d,s,l)    do { DLB_HANDLE_DCT4 hDCT=DLB_dct4_open(l); DLB_Sdct4SU(hDCT, d, d, l); } while(0)
/* DCTs for 32 bit data */
#define dctII_32x16_IP(d,s,l)    do { DLB_HANDLE_DCT2 hDCT=DLB_dct2_open(l); DLB_Ldct2LU(hDCT, d, d, l); } while(0)
#define dctIII_32x16_IP(d,s,l)   do { DLB_HANDLE_DCT3 hDCT=DLB_dct3_open(l); DLB_Ldct3LU(hDCT, d, d, l); } while(0)
#define dctIV_32x16_IP(d,s,l)    do { DLB_HANDLE_DCT4 hDCT=DLB_dct4_open(l); DLB_Ldct4LU(hDCT, d, d, l); } while(0)

/* Variable length forward FFTs for 16 bit data (in-place/out-of-place, scaled/unscaled) */
#define fft_scale_16x16_IP(d,l)        DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,l), DLB_CSVEC_NEW(d,l), l)
#define fft_scale_16x16_OP(i,o,l)      DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,l), DLB_CSVEC_NEW(i,l), l)
#define fft_noscale_16x16_IP(d,l)      DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,l), DLB_CSVEC_NEW(d,l), l)
#define fft_noscale_16x16_OP(i,o,l)    DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,l), DLB_CSVEC_NEW(i,l), l)
/* Variable length forward FFTs for 32 bit data (in-place/out-of-place, scaled/unscaled) */
#define fft_scale_32x16_IP(d,l)        DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,l), DLB_CLVEC_NEW(d,l), l)
#define fft_scale_32x16_OP(i,o,l)      DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,l), DLB_CLVEC_NEW(i,l), l)
#define fft_noscale_32x16_IP(d,l)      DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,l), DLB_CLVEC_NEW(d,l), l)
#define fft_noscale_32x16_OP(i,o,l)    DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,l), DLB_CLVEC_NEW(i,l), l)

/* Variable length inverse FFTs for 16 bit data (in-place/out-of-place, scaled/unscaled) */
#define ifft_scale_16x16_IP(d,l)       DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,l), DLB_CSVEC_NEW(d,l), l)
#define ifft_scale_16x16_OP(i,o,l)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,l), DLB_CSVEC_NEW(i,l), l)
#define ifft_noscale_16x16_IP(d,l)     DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,l), DLB_CSVEC_NEW(d,l), l)
#define ifft_noscale_16x16_OP(i,o,l)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,l), DLB_CSVEC_NEW(i,l), l)
/* Variable length inverse FFTs for 32 bit data (in-place/out-of-place, scaled/unscaled) */
#define ifft_scale_32x16_IP(d,l)       DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,l), DLB_CLVEC_NEW(d,l), l)
#define ifft_scale_32x16_OP(i,o,l)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,l), DLB_CLVEC_NEW(i,l), l)
#define ifft_noscale_32x16_IP(d,l)     DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,l), DLB_CLVEC_NEW(d,l), l)
#define ifft_noscale_32x16_OP(i,o,l)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,l), DLB_CLVEC_NEW(i,l), l)

/* Fixed length forward FFTs for 16 bit data (in-place/out-of-place, scaled/unscaled) */
#define fft_8_scale_16x16_IP(d)        DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,8),  DLB_CSVEC_NEW(d,8), 8)
#define fft_16_scale_16x16_IP(d)       DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,16), DLB_CSVEC_NEW(d,16), 16)
#define fft_32_scale_16x16_IP(d)       DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,32), DLB_CSVEC_NEW(d,32), 32)
#define fft_64_scale_16x16_IP(d)       DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,64),  DLB_CSVEC_NEW(d,64) , 64)
#define fft_128_scale_16x16_IP(d)      DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,128), DLB_CSVEC_NEW(d,128), 128)
#define fft_256_scale_16x16_IP(d)      DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,256), DLB_CSVEC_NEW(d,256), 256)
#define fft_512_scale_16x16_IP(d)      DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,512), DLB_CSVEC_NEW(d,512), 512)
#define fft_8_scale_16x16_OP(i,o)      DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,8),  DLB_CSVEC_NEW(i,8), 8)
#define fft_16_scale_16x16_OP(i,o)     DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,16), DLB_CSVEC_NEW(i,16), 16)
#define fft_32_scale_16x16_OP(i,o)     DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,32), DLB_CSVEC_NEW(i,32), 32)
#define fft_64_scale_16x16_OP(i,o)     DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,64),  DLB_CSVEC_NEW(i,64) , 64)
#define fft_128_scale_16x16_OP(i,o)    DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,128), DLB_CSVEC_NEW(i,128), 128)
#define fft_256_scale_16x16_OP(i,o)    DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,256), DLB_CSVEC_NEW(i,256), 256)
#define fft_512_scale_16x16_OP(i,o)    DLB_CSfft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,512), DLB_CSVEC_NEW(i,512), 512)
#define fft_8_noscale_16x16_IP(d)      DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,8),  DLB_CSVEC_NEW(d,8), 8)
#define fft_16_noscale_16x16_IP(d)     DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,16), DLB_CSVEC_NEW(d,16), 16)
#define fft_32_noscale_16x16_IP(d)     DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,32), DLB_CSVEC_NEW(d,32), 32)
#define fft_64_noscale_16x16_IP(d)     DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,64),  DLB_CSVEC_NEW(d,64) , 64)
#define fft_128_noscale_16x16_IP(d)    DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,128), DLB_CSVEC_NEW(d,128), 128)
#define fft_256_noscale_16x16_IP(d)    DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,256), DLB_CSVEC_NEW(d,256), 256)
#define fft_512_noscale_16x16_IP(d)    DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d,512), DLB_CSVEC_NEW(d,512), 512)
#define fft_8_noscale_16x16_OP(i,o)    DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,8),  DLB_CSVEC_NEW(i,8), 8)
#define fft_16_noscale_16x16_OP(i,o)   DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,16), DLB_CSVEC_NEW(i,16), 16)
#define fft_32_noscale_16x16_OP(i,o)   DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,32), DLB_CSVEC_NEW(i,32), 32)
#define fft_64_noscale_16x16_OP(i,o)   DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,64),  DLB_CSVEC_NEW(i,64) , 64)
#define fft_128_noscale_16x16_OP(i,o)  DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,128), DLB_CSVEC_NEW(i,128), 128)
#define fft_256_noscale_16x16_OP(i,o)  DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,256), DLB_CSVEC_NEW(i,256), 256)
#define fft_512_noscale_16x16_OP(i,o)  DLB_CSfft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o,512), DLB_CSVEC_NEW(i,512), 512)
/* Fixed length forward FFTs for 32 bit data (in-place/out-of-place, scaled/unscaled) */
#define fft_8_scale_32x16_IP(d)        DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,8),  DLB_CLVEC_NEW(d,8), 8)
#define fft_16_scale_32x16_IP(d)       DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,16), DLB_CLVEC_NEW(d,16), 16)
#define fft_32_scale_32x16_IP(d)       DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,32), DLB_CLVEC_NEW(d,32), 32)
#define fft_64_scale_32x16_IP(d)       DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,64),  DLB_CLVEC_NEW(d,64) , 64)
#define fft_128_scale_32x16_IP(d)      DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,128), DLB_CLVEC_NEW(d,128), 128)
#define fft_256_scale_32x16_IP(d)      DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,256), DLB_CLVEC_NEW(d,256), 256)
#define fft_512_scale_32x16_IP(d)      DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,512), DLB_CLVEC_NEW(d,512), 512)
#define fft_8_scale_32x16_OP(i,o)      DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,8),  DLB_CLVEC_NEW(i,8), 8)
#define fft_16_scale_32x16_OP(i,o)     DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,16), DLB_CLVEC_NEW(i,16), 16)
#define fft_32_scale_32x16_OP(i,o)     DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,32), DLB_CLVEC_NEW(i,32), 32)
#define fft_64_scale_32x16_OP(i,o)     DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,64),  DLB_CLVEC_NEW(i,64) , 64)
#define fft_128_scale_32x16_OP(i,o)    DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,128), DLB_CLVEC_NEW(i,128), 128)
#define fft_256_scale_32x16_OP(i,o)    DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,256), DLB_CLVEC_NEW(i,256), 256)
#define fft_512_scale_32x16_OP(i,o)    DLB_CLfft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,512), DLB_CLVEC_NEW(i,512), 512)
#define fft_8_noscale_32x16_IP(d)      DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,8),  DLB_CLVEC_NEW(d,8), 8)
#define fft_16_noscale_32x16_IP(d)     DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,16), DLB_CLVEC_NEW(d,16), 16)
#define fft_32_noscale_32x16_IP(d)     DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,32), DLB_CLVEC_NEW(d,32), 32)
#define fft_64_noscale_32x16_IP(d)     DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,64),  DLB_CLVEC_NEW(d,64) , 64)
#define fft_128_noscale_32x16_IP(d)    DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,128), DLB_CLVEC_NEW(d,128), 128)
#define fft_256_noscale_32x16_IP(d)    DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,256), DLB_CLVEC_NEW(d,256), 256)
#define fft_512_noscale_32x16_IP(d)    DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d,512), DLB_CLVEC_NEW(d,512), 512)
#define fft_8_noscale_32x16_OP(i,o)    DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,8),  DLB_CLVEC_NEW(i,8), 8)
#define fft_16_noscale_32x16_OP(i,o)   DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,16), DLB_CLVEC_NEW(i,16), 16)
#define fft_32_noscale_32x16_OP(i,o)   DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,32), DLB_CLVEC_NEW(i,32), 32)
#define fft_64_noscale_32x16_OP(i,o)   DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,64),  DLB_CLVEC_NEW(i,64) , 64)
#define fft_128_noscale_32x16_OP(i,o)  DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,128), DLB_CLVEC_NEW(i,128), 128)
#define fft_256_noscale_32x16_OP(i,o)  DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,256), DLB_CLVEC_NEW(i,256), 256)
#define fft_512_noscale_32x16_OP(i,o)  DLB_CLfft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o,512), DLB_CLVEC_NEW(i,512), 512)

/* Fixed length inverse FFTs for 16 bit data (in-place/out-of-place, scaled/unscaled) */
#define ifft_4_scale_16x16_IP(d)       DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 4), DLB_CSVEC_NEW(d, 4), 4)
#define ifft_8_scale_16x16_IP(d)       DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 8), DLB_CSVEC_NEW(d, 8), 8)
#define ifft_16_scale_16x16_IP(d)      DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 16), DLB_CSVEC_NEW(d, 16), 16)
#define ifft_32_scale_16x16_IP(d)      DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 32), DLB_CSVEC_NEW(d, 32), 32)
#define ifft_64_scale_16x16_IP(d)      DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 64), DLB_CSVEC_NEW(d, 64), 64)
#define ifft_128_scale_16x16_IP(d)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 128), DLB_CSVEC_NEW(d, 128), 128)
#define ifft_256_scale_16x16_IP(d)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 256), DLB_CSVEC_NEW(d, 256), 256)
#define ifft_512_scale_16x16_IP(d)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 512), DLB_CSVEC_NEW(d, 512), 512)
#define ifft_4_scale_16x16_OP(i,o)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 4), DLB_CSVEC_NEW(i, 4), 4)
#define ifft_8_scale_16x16_OP(i,o)     DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 8), DLB_CSVEC_NEW(i, 8), 8)
#define ifft_16_scale_16x16_OP(i,o)    DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 16), DLB_CSVEC_NEW(i, 16), 16)
#define ifft_32_scale_16x16_OP(i,o)    DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 32), DLB_CSVEC_NEW(i, 32), 32)
#define ifft_64_scale_16x16_OP(i,o)    DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 64), DLB_CSVEC_NEW(i, 64), 64)
#define ifft_128_scale_16x16_OP(i,o)   DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 128), DLB_CSVEC_NEW(i, 128), 128)
#define ifft_256_scale_16x16_OP(i,o)   DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 256), DLB_CSVEC_NEW(i, 256), 256)
#define ifft_512_scale_16x16_OP(i,o)   DLB_CSifft_scaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 512), DLB_CSVEC_NEW(i, 512), 512)
#define ifft_4_noscale_16x16_IP(d)     DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 4), DLB_CSVEC_NEW(d, 4), 4)
#define ifft_8_noscale_16x16_IP(d)     DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 8), DLB_CSVEC_NEW(d, 8), 8)
#define ifft_16_noscale_16x16_IP(d)    DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 16), DLB_CSVEC_NEW(d, 16), 16)
#define ifft_32_noscale_16x16_IP(d)    DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 32), DLB_CSVEC_NEW(d, 32), 32)
#define ifft_64_noscale_16x16_IP(d)    DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 64), DLB_CSVEC_NEW(d, 64), 64)
#define ifft_128_noscale_16x16_IP(d)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 128), DLB_CSVEC_NEW(d, 128), 128)
#define ifft_256_noscale_16x16_IP(d)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 256), DLB_CSVEC_NEW(d, 256), 256)
#define ifft_512_noscale_16x16_IP(d)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(d, 512), DLB_CSVEC_NEW(d, 512), 512)
#define ifft_4_noscale_16x16_OP(i,o)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 4), DLB_CSVEC_NEW(i, 4), 4)
#define ifft_8_noscale_16x16_OP(i,o)   DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 8), DLB_CSVEC_NEW(i, 8), 8)
#define ifft_16_noscale_16x16_OP(i,o)  DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 16), DLB_CSVEC_NEW(i, 16), 16)
#define ifft_32_noscale_16x16_OP(i,o)  DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 32), DLB_CSVEC_NEW(i, 32), 32)
#define ifft_64_noscale_16x16_OP(i,o)  DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 64), DLB_CSVEC_NEW(i, 64), 64)
#define ifft_128_noscale_16x16_OP(i,o) DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 128), DLB_CSVEC_NEW(i, 128), 128)
#define ifft_256_noscale_16x16_OP(i,o) DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 256), DLB_CSVEC_NEW(i, 256), 256)
#define ifft_512_noscale_16x16_OP(i,o) DLB_CSifft_unscaledCSU((DLB_HANDLE_FFT)0, DLB_CSVEC_NEW(o, 512), DLB_CSVEC_NEW(i, 512), 512)
/* Fixed length inverse FFTs for 32 bit data (in-place/out-of-place, scaled/unscaled) */
#define ifft_4_scale_32x16_IP(d)       DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 4), DLB_CLVEC_NEW(d, 4), 4)
#define ifft_8_scale_32x16_IP(d)       DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 8), DLB_CLVEC_NEW(d, 8), 8)
#define ifft_16_scale_32x16_IP(d)      DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 16), DLB_CLVEC_NEW(d, 16), 16)
#define ifft_32_scale_32x16_IP(d)      DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 32), DLB_CLVEC_NEW(d, 32), 32)
#define ifft_64_scale_32x16_IP(d)      DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 64), DLB_CLVEC_NEW(d, 64), 64)
#define ifft_128_scale_32x16_IP(d)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 128), DLB_CLVEC_NEW(d, 128), 128)
#define ifft_256_scale_32x16_IP(d)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 256), DLB_CLVEC_NEW(d, 256), 256)
#define ifft_512_scale_32x16_IP(d)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 512), DLB_CLVEC_NEW(d, 512), 512)
#define ifft_4_scale_32x16_OP(i,o)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 4), DLB_CLVEC_NEW(i, 4), 4)
#define ifft_8_scale_32x16_OP(i,o)     DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 8), DLB_CLVEC_NEW(i, 8), 8)
#define ifft_16_scale_32x16_OP(i,o)    DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 16), DLB_CLVEC_NEW(i, 16), 16)
#define ifft_32_scale_32x16_OP(i,o)    DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 32), DLB_CLVEC_NEW(i, 32), 32)
#define ifft_64_scale_32x16_OP(i,o)    DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 64), DLB_CLVEC_NEW(i, 64), 64)
#define ifft_128_scale_32x16_OP(i,o)   DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 128), DLB_CLVEC_NEW(i, 128), 128)
#define ifft_256_scale_32x16_OP(i,o)   DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 256), DLB_CLVEC_NEW(i, 256), 256)
#define ifft_512_scale_32x16_OP(i,o)   DLB_CLifft_scaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 512), DLB_CLVEC_NEW(i, 512), 512)
#define ifft_4_noscale_32x16_IP(d)     DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 4), DLB_CLVEC_NEW(d, 4), 4)
#define ifft_8_noscale_32x16_IP(d)     DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 8), DLB_CLVEC_NEW(d, 8), 8)
#define ifft_16_noscale_32x16_IP(d)    DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 16), DLB_CLVEC_NEW(d, 16), 16)
#define ifft_32_noscale_32x16_IP(d)    DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 32), DLB_CLVEC_NEW(d, 32), 32)
#define ifft_64_noscale_32x16_IP(d)    DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 64), DLB_CLVEC_NEW(d, 64), 64)
#define ifft_128_noscale_32x16_IP(d)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 128), DLB_CLVEC_NEW(d, 128), 128)
#define ifft_256_noscale_32x16_IP(d)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 256), DLB_CLVEC_NEW(d, 256), 256)
#define ifft_512_noscale_32x16_IP(d)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(d, 512), DLB_CLVEC_NEW(d, 512), 512)
#define ifft_4_noscale_32x16_OP(i,o)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 4), DLB_CLVEC_NEW(i, 4), 4)
#define ifft_8_noscale_32x16_OP(i,o)   DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 8), DLB_CLVEC_NEW(i, 8), 8)
#define ifft_16_noscale_32x16_OP(i,o)  DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 16), DLB_CLVEC_NEW(i, 16), 16)
#define ifft_32_noscale_32x16_OP(i,o)  DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 32), DLB_CLVEC_NEW(i, 32), 32)
#define ifft_64_noscale_32x16_OP(i,o)  DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 64), DLB_CLVEC_NEW(i, 64), 64)
#define ifft_128_noscale_32x16_OP(i,o) DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 128), DLB_CLVEC_NEW(i, 128), 128)
#define ifft_256_noscale_32x16_OP(i,o) DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 256), DLB_CLVEC_NEW(i, 256), 256)
#define ifft_512_noscale_32x16_OP(i,o) DLB_CLifft_unscaledCLU((DLB_HANDLE_FFT)0, DLB_CLVEC_NEW(o, 512), DLB_CLVEC_NEW(i, 512), 512)


/** @} */


/**
  @name Legacy mappings for dlb_mathlib functions

  The legacy dlb_mathlib is partly replaced by functionality integrated
  in dlb_intrinsics/dlb_dsplib.
  The corresponding function names are mapped here to a more consistent name space.
  Less generic parts of the legacy dlb_mathlib are maintained outside the scope
  of the Dolby Intrinsics.
*/
/** @{ */

/*
  Mapping of v0.3 function names to v0.9 names
*/
#define LSI_pow2_xy(x, y)                      DLB_pow2_xy(x, y)
#define LSI_divideInt(x, y)                    DLB_divideInt(x, y)
#define LSI_sqrt(x, a)                         DLB_sqrt(x, a)
#define LSI_reciprt(x)                         DLB_reciprt(x)
#define LSI_divide(x, y)                       DLB_divide(x, y)
#define LSI_divide16(x, y)                     DLB_divide16(x, y)
#define LSI_scaleShortVector(p, l, s)          DLB_scaleShortVector(p, l, s)
#define LSI_scaleIntVector(p, l, s)            DLB_scaleIntVector(p, l, s)
#define LSI_getScalefactorOfShortVector(p, l)  DLB_getScalefactorOfShortVector(p, l)
#define LSI_getScalefactorOfIntVector(p, l)    DLB_getScalefactorOfIntVector(p, l)
#define LSI_addIntVectors(x, a, b, l)          DLB_addIntVectors(x, a, b, l)

/*
  Mapping of v0.9 function names to v1.0 names
*/
#define DLB_pow2_xy(x, y)                      DLB_Lpow2xy3232(x, y)
#define DLB_divideInt(x, y)                    ((x)/(y))
#define DLB_divide(x, y)                       DLB_LdivLL(x, y)
#define DLB_div32_32(x, y)                     DLB_LdivLL(x, y)
#define DLB_divide16(x, y)                     DLB_SdivLL(x, y)
#define DLB_sqrt(x, a)                         (a > DLB_SFRACT_MANTBITS ? DLB_LsqrtL(x) : DLB_SsqrtS(x))
#define DLB_reciprt(x)                         DLB_LreciprtL(x)
#define DLB_sin(x)                             DLB_Lsin4L(x)
#define DLB_cos(x)                             DLB_Lcos4L(x)
#define DLB_getScalefactorOfShortVector(v, l)  DLB_vec_UnormSU(v, l)
#define DLB_getScalefactorOfIntVector(v, l)    DLB_vec_UnormLU(v, l)
#define DLB_scaleShortVector(p, l, s)          DLB_vec_SshlSIU_inplace(p, s, l)
#define DLB_scaleIntVector(p, l, s)            DLB_vec_LshlLIU_inplace(p, s, l)
#define DLB_addIntVectors(x, a, b, l)          do { DLB_LFRACT *dest, *src0, *src1; \
                                                 for (dest=x,src0=a,src1=b; dest<x+l; dest+=DLB_BLK_N,src0+=DLB_BLK_N,src1+=DLB_BLK_N) \
                                                   DLB_blk_LsaddLL(dest, src0, src1); \
                                               } while(0)


/** @} */

/* Older versions of the compiler abstraction gave an assert() statement. You
 * should use the DLB_assert() or DLB_print() statements instead.
 */
#include <assert.h>

/** @} */

#endif /* DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS */
