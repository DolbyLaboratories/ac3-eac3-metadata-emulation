/****************************************************************************
 *
 *
 * Copyright (c) 2010-2013 Dolby International AB.
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
 * @brief API Prototypes - Blocklib Functions
 */
#ifndef BLK_H
#define BLK_H

#include "dlb_intrinsics.h"          /* defines the backend */
#include "dlb_nonlinear.h"
#include DLB_DSPLIB                  /* defines the backend header files */
#include DLB_VEC_INLINES             /* includes the backend specific inline functions */

#ifndef DLB_BLK_N
/** The block library provides functions to be performed on fixed-length arrays (blocks).
 * The array length is defined at compile time using DLB_BLK_N. Optimised processor backends may
 * define this value differently. Therefore, DLB_BLK_N is used by client code that calls 
 * blocklib functions to advance its pointers or loop counters appropriately. For example:
 @verbatim
 for (i=0; i<len; i+=DLB_BLK_N){
   blk_lib_function1(&data[i]);
   blk_lib_function2(&data[i]);
 }
 @endverbatim
 */
#define DLB_BLK_N 32
#endif

/**
  @addtogroup dlb_dsplib_blk
  @{
 */

/**
 * First order Direct Form 1 IIR Tap.
 * This struct is used by DLB_blk_Liir1L(), DLB_blk_Liir1L_set_coef(),
 * DLB_blk_Liir1_shl_addLL(), DLB_blk_Liir1_shlLU(), DLB_blk_Liir1xL_set_coef(),
 * and DLB_blk_Liir1xL().
 */
typedef struct
{
	DLB_SFRACT b0; /**< B0 */
	DLB_SFRACT b1; /**< B1 */
	DLB_SFRACT a1; /**< A1 */
} DLB_BLK_IIR1_COEF;

/**
 * State Space IIR Tap.
 * This struct is used by DLB_blk_Liir2ssL_set_coef(), DLB_blk_Liir2ssv1_dsmpL(),
 * DLB_blk_Liir2ssefL_set_coef(), DLB_blk_Liir2ssv1efL(), DLB_blk_Liir2ssv1L(),
 * DLB_blk_Liir2ssv1_usmpL(), DLB_blk_Liir2ssv2L(), DLB_blk_Liir2ssv2L_inplace(),
 * DLB_blk_Liir2ssv2_shlLU(), DLB_blk_Liir2ssv3aLU_inplace(), and
 * DLB_blk_Liir2ssv3L_inplace().
 */
typedef struct
{
	DLB_SFRACT K; /**< K */
	DLB_SFRACT Fa; /**< Fa */
	DLB_SFRACT Fb; /**< Fb */
	DLB_SFRACT Gaa; /**< Gaa */
	DLB_SFRACT Gab; /**< Gab */
} DLB_BLK_IIR2SS_COEF;

/**
 * Second Order Direct Form 1 IIR Tap.
 * This struct is used by DLB_blk_Liir2df1L_set_coef() and DLB_blk_Liir2df1L().
 */
typedef struct
{
	DLB_SFRACT b0; /**< b0 */
	DLB_SFRACT b1; /**< b1 */
	DLB_SFRACT b2; /**< b2 */
	DLB_SFRACT a1; /**< -a1/2 */
	DLB_SFRACT a2; /**< -a2 */
} DLB_BLK_IIR2DF1_COEF;

/**
 * Attack-Decay IIR Tap.
 * This struct is used by DLB_blk_Lattack_decay_iir1L_set_coef()
 * and DLB_blk_Lattack_decay_iir1L().
 */
typedef struct
{
	DLB_SFRACT attk; /**< Attack */
	DLB_SFRACT dec;  /**< Decay */
} DLB_BLK_ATTACK_DECAY_IIR1_COEF;

/**
 * Attack-Decay-Peak IIR Tap.
 * This struct is used by DLB_blk_Lattack_decay_peak_iir1L_set_coef()
 * and DLB_blk_Lattack_decay_peak_iir1L().
 */
typedef struct
{
	DLB_SFRACT attk; /**< Attack */
	DLB_SFRACT dec;  /**< Decay */
	DLB_SFRACT peak; /**< Peak */
} DLB_BLK_ATTACK_DECAY_PEAK_IIR1_COEF;

/**
 * Block delay state.
 * This struct is used by DLB_blk_delayL(), DLB_blk_LdelayU(), and DLB_blk_refLdelayU().
 */
typedef struct
{
    unsigned int idx; /**< Current Index */
    unsigned int len; /**< Delay Length */
    DLB_LFRACT *dly;  /**< Delay Buffer  */
} DLB_BLK_DELAY_STATE;

/**
 * Crossfade state.
 * This struct is used by DLB_blk_Lcrossfade_gainLS().
 */
typedef struct
{
    unsigned int idx; /**< Current Index */
    unsigned int len; /**< Crossfade Length. It must be an
                           integer multiply of the processing block size DLB_BLK_N */
} DLB_BLK_CROSSFADE_GAIN_STATE;

/** @} */

#include DLB_BLK               /* includes the backend specific types & inline functions */

/**
  @addtogroup dlb_dsplib_blk
  Contains a toolkit of functions for blocks like FIR, IIR and block operations.
  Blocks are arrays whose size is defined at compile-time by DLB_BLK_N.

  @{
 */

/**
  @name FIR Block Filters
  Various IIR filters on blocks.
  Blocks are arrays whose size is defined at compile-time by #DLB_BLK_N define.
  @{
 */


/**
 * Initialize 2 short fractional tap FIR state.
 */
void
DLB_blk_Lfir2L_setup
    (DLB_BLK_FIR2_STATE *state /**< FIR state */
    ,const DLB_SFRACT c[2] /**< 2 short fractional taps */
    );

/**
 * Update 2 short fractional taps FIR values.
 */
void
DLB_blk_Lfir2L_set_coef
    (DLB_BLK_FIR2_STATE *state /**< FIR state */
    ,const DLB_SFRACT c[2] /**< 2 short fractional taps */
    );

/**
 * Each @a dest long fractional block element is the result of convolution
 * using 2 short fractional taps.
 */
void
DLB_blk_Lfir2L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_SFRACT c[2]  /**< 2 short fractional taps */
	,DLB_BLK_FIR2_STATE *state /**< FIR state */
	);

/**
 * Initialize 3 short fractional tap FIR state.
 */
void
DLB_blk_Lfir3L_setup
    (DLB_BLK_FIR3_STATE *state /**< FIR state */
    ,const DLB_SFRACT c[3] /**< 3 short fractional taps */
    );

/**
 * Update 3 short fractional taps FIR values.
 */
void
DLB_blk_Lfir3L_set_coef
    (DLB_BLK_FIR3_STATE *state /**< FIR state */
    ,const DLB_SFRACT c[3] /**< 3 short fractional taps */
    );

/**
 * Each @a dest long fractional block element is the result of convolution
 * using 3 short fractional taps.
 */
void
DLB_blk_Lfir3L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_SFRACT c[3] /**< 3 short fractional taps */
	,DLB_BLK_FIR3_STATE *state /**< FIR state */
	);

/* state should be a pointer to a DLB_BLK_FIRU_STATE(N) allocated by caller */

/**
 * Initialize N short fractional tap FIR state.
 */
void
DLB_blk_LfirUL_setup
	(DLB_BLK_FIRU_STATE_DECL state /**< FIR state */
	,unsigned N /**< Number of taps */
	,const DLB_SFRACT *c /**< N short fractional taps */
	);

/**
 * Update 3 short fractional taps FIR values.
 */
void
DLB_blk_LfirUL_set_coef
	(DLB_BLK_FIRU_STATE_DECL state /**< FIR state */
	,unsigned N /**< Number of taps */
	,const DLB_SFRACT *c /**< N short fractional taps */
	);

/**
 * Each @a dest long fractional block element is the result of convolution
 * using N short fractional taps.
 */
void
DLB_blk_LfirUL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const unsigned int N /**< Number of taps */
	,const DLB_SFRACT c[] /**< N short fractional taps */
	,DLB_BLK_FIRU_STATE_DECL state /**< FIR state */
	);

/** @} */

/**
  @name IIR Block Filters
  Various IIR filters on blocks.
  Blocks can be considered as arrays whose size is defined at compile-time by #DLB_BLK_N define.
  @{
 */

/**
 * Initialize one-tap Direct Form 1 IIR state.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir1L_setup
	(DLB_BLK_IIR1_STATE *state /**< IIR state */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Update one-tap Direct Form 1 IIR values.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir1L_set_coef
	(DLB_BLK_IIR1_STATE *state /**< IIR state */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is the result of performing
 * a one-tap Direct Form 1 IIR filter. IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir1L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR1_STATE *state /**< IIR state */
	);

/**
 * Initialize one-tap Direct Form 1 IIR state.
 * IIR state Y1 is stored as an extended accumulator for more accuracy.
 */
void
DLB_blk_Liir1xL_setup
	(DLB_BLK_IIR1X_STATE *state /**< IIR state */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Update one-tap Direct Form 1 IIR values.
 * IIR state Y1 is stored as an extended accumulator for more accuracy.
 */
void
DLB_blk_Liir1xL_set_coef
	(DLB_BLK_IIR1X_STATE *state /**< IIR state */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is the result of performing
 * a one-tap Direct Form 1 IIR filter.
 * IIR state Y1 is stored as an extended accumulator for more accuracy.
 */
void
DLB_blk_Liir1xL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR1X_STATE *state /**< IIR state */
	);

/**
 * Similar to DLB_blk_Liir1L() except each @a dest long fractional block element
 * is the accumulated result shifted left by 1 and added to the corresponding @a src2
 * long fractional value.
 */
void
DLB_blk_Liir1_shl_addLL
	(DLB_LFRACT * dest /**< Output long fractional block */
	,const DLB_LFRACT *src1 /**< Input long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src2
	,const DLB_BLK_IIR1_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR1_STATE *state /**< IIR State */
	);

/**
 * Similar to DLB_blk_Liir1L() except each @a dest long fractional block element
 * is the accumulated result shifted left by @a shift.
 */
void
DLB_blk_Liir1_shlLU
	(DLB_LFRACT  *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT  *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR1_COEF * c /**< IIR coefficients */
	,DLB_BLK_IIR1_STATE * state /**< IIR State */
	,unsigned int shift /**< Shift amount */
	);

/**
 * Initialize two-tap State Space IIR state.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2ssL_setup
	(DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	);

/**
 * Update two-tap State Space IIR values.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2ssL_set_coef
	(DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is calculated using the
 * following state space filter:<br>
 * acc1        = src[i] * Fa + s0 * Gaa - s1 * Gab<br>
 * acc2        = src[i] * Fb + s0 * Gab + s1 * Gaa<br>
 * Update s0, s1 <br>
 * dest[i + 1] = 2 * ((acc1 + acc2) - src[i+1] * K)<br>
 */
void
DLB_blk_Liir2ssv1L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	);

/**
 * Doubles the number of samples in the long fractional input block
 * using the state space filter.
 */
void
DLB_blk_Liir2ssv1_usmpL
	(DLB_LFRACT * DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT * DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	);

/**
 * Halfs the number of samples in the long fractional input block
 * using the state space filter.
 */
void
DLB_blk_Liir2ssv1_dsmpL
	(DLB_LFRACT * dest /**< Output long fractional block */
	,const DLB_LFRACT * src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	);

/**
 * Initialize two-tap Direct Form 1 IIR state.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2df1L_setup
	(DLB_BLK_IIR2DF1_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2DF1_COEF *c /**< IIR coefficients */
	);

/**
 * Update two-tap Direct Form 1 IIR values.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2df1L_set_coef
	(DLB_BLK_IIR2DF1_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2DF1_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is the result of performing
 * a two-tap Direct Form 1 IIR filter. IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2df1L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2DF1_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2DF1_STATE *state /**< IIR State */
	);

/**
 * Each @a dest long fractional block element is calculated using the
 * following state space filter:<br>
 * acc1        = src[i] * Fa + s0 * Gaa + s1<br>
 * acc2        = src[i] * Fb + s0 * Gab + s1 * Gaa<br>
 * Update s0, s1 <br>
 * dest[i + 1] = 2 * (acc1 - src[i+1] * K)<br>
 */
void
DLB_blk_Liir2ssv2L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	);

/**
 * Same as DLB_blk_Liir2ssv2L() but using the same input/output buffer.
 */
void
DLB_blk_Liir2ssv2L_inplace
	(DLB_LFRACT *inout /**< Input/Output long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	);

/**
 * Each @a dest long fractional block element is calculated using the
 * following state space filter:<br>
 * acc1        = src[i] * Fa + s0 * Gaa + s1<br>
 * acc2        = src[i] * Fb + s0 * Gab + s1 * Gaa<br>
 * Update s0, s1 <br>
 * dest[i + 1] = (acc1 - src[i+1] * K) << shift<br>
 */
void
DLB_blk_Liir2ssv2_shlLU
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SS_STATE *state /**< IIR State */
	,unsigned int shift /**< Shift amount */
	);


/**
 * Each @a dest long fractional block element is calculated using the
 * following state space filter:<br>
 * acc1        = src[i] * Fa + s0 * Gaa + s1 * 0.0625<br>
 * acc2        = src[i] * Fb + s0 * Gab + s1 * Gaa<br>
 * Update s0, s1 <br>
 * dest[i + 1] = 2 * (acc1 - src[i+1] * K)<br>
 */
void
DLB_blk_Liir2ssv3L_inplace
  (DLB_LFRACT *inout /**< Input/Output long fractional block */
  ,const DLB_BLK_IIR2SS_COEF *c  /**< IIR coefficients */
  ,DLB_BLK_IIR2SS_STATE *state  /**< IIR State */
  );

/**
 * Same as DLB_blk_Liir2ssv3L_inplace() but extending it to multiple channels.<br>
 * The channels will be processed with the same set of coefficents.
 */
void
DLB_blk_Liir2ssv3aLU_inplace
  (DLB_LFRACT *inout[]
  ,const DLB_BLK_IIR2SS_COEF *c
  ,DLB_BLK_IIR2SS_STATE *pp_states[]
  ,unsigned int num_ch
  );

/**
 * Initialize State Space Error Feedback IIR state.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2ssefL_setup
	(DLB_BLK_IIR2SSEF_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	);

/**
 * Update State Space Error Feedback IIR values.
 * IIR state is kept as long fractionals.
 */
void
DLB_blk_Liir2ssefL_set_coef
	(DLB_BLK_IIR2SSEF_STATE *state /**< IIR State */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is set using the state space
 * filter similar to DLB_blk_Liir2ssv2L() but also accounts for error
 * accumulated from loss of precision when converting from an accumulator
 * to a long fractional.
 */
void
DLB_blk_Liir2ssv1efL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_IIR2SS_COEF *c /**< IIR coefficients */
	,DLB_BLK_IIR2SSEF_STATE *state /**< IIR State */
	);

/**
 * Initialize Attack decay IIR state.
 * IIR state is stored using extended accumulators.
 */
void
DLB_blk_Lattack_decay_iir1L_setup
	(DLB_BLK_ATTACK_DECAY_IIR1_STATE *state /**< IIR State */
	,const DLB_BLK_ATTACK_DECAY_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Update Attack decay IIR state.
 * IIR state is stored using extended accumulators.
 */
void
DLB_blk_Lattack_decay_iir1L_set_coef
	(DLB_BLK_ATTACK_DECAY_IIR1_STATE *state /**< IIR State */
	,const DLB_BLK_ATTACK_DECAY_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is set using the attack decay
 * IIR filter. Intermediate and IIR state is stored using extended accumulators
 * for higher precision.
 */
void
DLB_blk_Lattack_decay_iir1L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_ATTACK_DECAY_IIR1_COEF *c /**< IIR coefficients */
	,DLB_BLK_ATTACK_DECAY_IIR1_STATE *state /**< IIR State */
	);

/**
 * Initialize Attack decay peak IIR state.
 * IIR state is stored using extended accumulators.
 */
void
DLB_blk_Lattack_decay_peak_iir1L_setup
	(DLB_BLK_ATTACK_DECAY_PEAK_IIR1_STATE *state /**< IIR State */
	,const DLB_BLK_ATTACK_DECAY_PEAK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Update Attack decay peak IIR state.
 * IIR state is stored using extended accumulators.
 */
void
DLB_blk_Lattack_decay_peak_iir1L_set_coef
	(DLB_BLK_ATTACK_DECAY_PEAK_IIR1_STATE *state /**< IIR State */
	,const DLB_BLK_ATTACK_DECAY_PEAK_IIR1_COEF *c /**< IIR coefficients */
	);

/**
 * Each @a dest long fractional block element is set using the attack decay
 * IIR filter with a maximum peak value. Intermediate and IIR state is
 * stored using extended accumulators for higher precision.
 */
void
DLB_blk_Lattack_decay_peak_iir1L
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_BLK_ATTACK_DECAY_PEAK_IIR1_COEF *c /**< IIR coefficients */
	,DLB_BLK_ATTACK_DECAY_PEAK_IIR1_STATE *state /**< IIR State */
	);

/** 
 * State initialiser for DLB_blk_LnrrecipLU().
 */
void
DLB_blk_LnrrecipLU_setup
    (DLB_BLK_NRRECIP_STATE *state
    ,unsigned int shift
    );

/** Calculate a Newton-Raphson approximation to 1 / (src[i] * 2^shift) for
 * each element in the input block @a src.
 * One iteration of Newton-Raphson is performed per sample and state is held
 * across blocks in @a state, which must be initialised with DLB_blk_LnrrecipLU_setup().
 */
void
DLB_blk_LnrrecipLU
    (DLB_LFRACT       *DLB_RESTRICT dest    /**< Output block */
    ,const DLB_LFRACT *DLB_RESTRICT src     /**< Input block */
    ,unsigned int                   shift   /**< Shift count */
    ,DLB_BLK_NRRECIP_STATE         *state   /**< approximation state */
    );

/** @} */

/**
  @name Block Operations
  Contains functions to perform common operations on blocks.
  Blocks can be considered as arrays whose size is defined at compile-time by #DLB_BLK_N define.
  @{
 */


/**
 * Each @a dest long fractional block element is the result of
 * adding the corresponding @a src0 & @a src1 long fractional block elements.
 */
void
DLB_blk_LsaddLL
	(DLB_LFRACT  *DLB_RESTRICT dest
	,const DLB_LFRACT  *DLB_RESTRICT src0
	,const DLB_LFRACT  *DLB_RESTRICT src1
	);

/**
 * Each @a dest long fractional block element is the result of
 * subtracting the corresponding @a src1 long fractional block element from
 * @a src0 long fractional block element.
 */
void
DLB_blk_LsubLL
	(DLB_LFRACT  *DLB_RESTRICT dest
	,const DLB_LFRACT  *DLB_RESTRICT src0
	,const DLB_LFRACT  *DLB_RESTRICT src1
	);

/**
 * Multiplies each long fractional @a src block element by short fractional @a c[0]
 * and  stores the saturated & rounded result into the corresponding @a dest
 * long fractional block element. All intermediate results are stored using
 * the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_LmixL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[1] /**< Scalar multipler for src */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	);

/**
 * Extension of DLB_blk_LmixL() for 2 sources.
 * dest[i] = (src0[i] * c[0] + src1[i] * c[1])
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_LmixLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[2] /**< Scalar multipler for src0, src1 */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	);

/**
 * Extension of DLB_blk_LmixL() for 3 sources.
 * dest[i] = (src0[i] * c[0] + ... + src2[i] * c[2])
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_LmixLLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[3] /**< Scalar multipler for src0, src1, src2 */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	);

/**
 * Extension of DLB_blk_LmixL() for 4 sources.
 * dest[i] = (src0[i] * c[0] + ... + src3[i] * c[3])
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_LmixLLLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[4] /**< Scalar multipler for src0, src1, src2, src3 */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	,const DLB_LFRACT *DLB_RESTRICT src3 /**< Input long fractional block 4 */
	);

/**
 * Extension of DLB_blk_LmixL().
 * Handles mixing of an arbitrary number of sources scaled with corresponding
 * coefficients. All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_LmixnL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[] /**< Scalar multiplers for N sources */
	,const unsigned int N/**< Number of sources */
	,...
	);

/**
 * Multiplies each long fractional @a src block element by short fractional @a c[0],
 * shifts this result left by @a shift and  stores the saturated & rounded result
 * into the corresponding @a dest long fractional block element. All intermediate
 * results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_Lmix_shlUL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[1] /**< Scalar multipler for src */
	,const unsigned shift /**< Shift factor */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	);

/**
 * Extension of DLB_blk_Lmix_shlUL() for 2 sources.
 * dest[i] = (src0[i] * c[0] + src1[i] * c[1]) << shift
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_Lmix_shlULL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[2] /**< Scalar multiplers for src0, src1 */
	,const unsigned shift /**< Shift factor */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	);

/**
 * Extension of DLB_blk_Lmix_shlUL() for 3 sources.
 * dest[i] = (src0[i] * c[0] + ... + src2[i] * c[2]) << shift
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_Lmix_shlULLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[3] /**< Scalar multiplers for src0, src1, src2 */
	,const unsigned shift /**< Shift factor */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	);

/**
 * Extension of DLB_blk_Lmix_shlUL() for 4 sources.
 * dest[i] = (src0[i] * c[0] + ... + src3[i] * c[3]) << shift
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_Lmix_shlULLLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[4] /**< Scalar multiplers for src0, src1, src2, src3 */
	,const unsigned shift /**< Shift factor */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	,const DLB_LFRACT *DLB_RESTRICT src3 /**< Input long fractional block 4 */
	);

/**
 * Extension of DLB_blk_Lmix_shlUL().
 * Handles mixing of an arbitrary number of sources scaled with corresponding
 * coefficients & shifted. All intermediate results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity can be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code.
 */
void
DLB_blk_Lmix_shlUnL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT c[] /**< Scalar multiplers for N sources */
	,const unsigned shift /**< Shift factor */
	,const unsigned int N /**< Number of sources */
	,...
	);

typedef DLB_LFRACT DLB_BLK_LFRACT[DLB_BLK_N];

/**
 * Each @a dest long fractional block element is the result of
 * scaling and adding of corresponding @a src long fractional block
 * with short fractional coefficients @a c, shifted left by @a shift_left.
 */
void
DLB_blk_Lmix_shlaL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_SFRACT  c[] /**< mix coefficients */
	,const unsigned int N /**< Number of short fractional coeff & long fractional blocks */
	,DLB_BLK_LFRACT *src /**< N long fractional source blocks  */
	,const unsigned shift_left /**< Shift amount */
	);

/**
 * Each @a dest long fractional block element is set to the corresponding
 * saturated absolute @a src long fractional block element.
 */
void
DLB_blk_LabsL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	);

/**
 * Each @a dest long fractional block element is set either c[0] if src0[i] < src1[i],
 * or c[1] otherwise.
 */
void
DLB_blk_LcmpLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT c[2] /**< Possible results. */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< First long fractional block to compare */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Second long fractional block to compare. */
	);

/**
 * Each @a dest long fractional block element is the @a src long fractional block element clipped to range c[0]:c[1].
 * All intermediate results are stored using the accumulator (DLB_ACCU).
 */
void
DLB_blk_LclipL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT c[2] /**< Long fractional clip range. c[0]:low c[1]:high */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	);

/**
 * Returns the smallest multiple of N greater than x + N, at least 2*N.
 */
#define DLB_blk_delay_round_up(x, N) (((x + N - 1)/N + 1)*N)

/**
 * Incorporates wrap-around padding block
 */
#define DLB_blk_delay_store_len_for_max_taps(len) \
	(DLB_blk_delay_round_up(len, DLB_BLK_N) + DLB_BLK_N)

/**
 * Setup Block delay line.
 *
 */
void
DLB_blk_delay_setup
	(DLB_BLK_DELAY_STATE *state/**< Delay State */
	,DLB_LFRACT *store /**< Delay Line Memory */
	,unsigned int len  /**< Desired max delay length */
	);

/**
 * Clear the delay line. The clear parameter is the clearing state. Pass in NULL
 * the first time then just pass in the result from the previous call. When the
 * output is NULL you know the delay line is clear. This incremental approach is
 * done to reduce MIPS by allowing to clear a chunk at a time as only a small
 * part of the delay line may be used.
 */
DLB_LFRACT* /**< @returns Updated clearing state */
DLB_blk_delay_clear
	(DLB_BLK_DELAY_STATE *bs    /**< Delay State */
	,DLB_LFRACT  *clear         /**< Clear state */
	,unsigned int clear_size    /**< Number of long fractionals to clear this call */
	);

/**
 * Appends the incoming @a src long fractional block to
 * the delay line.
 */
void
DLB_blk_delayL
	(DLB_BLK_DELAY_STATE *state /**< Delay State */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	);

/**
 * Returns (by copying to @a dest) the next DLB_BLK_N long fractional values
 * into the destination buffer from the starting index. The starting index is
 * @a tap offset behind the current delay buffer index.
 */
void
DLB_blk_LdelayU
	(DLB_BLK_DELAY_STATE *state /**< Delay State */
	,DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,unsigned int tap /**< Tap offset */
	);

/**
 * Returns (by reference to internal buffer) the next DLB_BLK_N
 * long fractional values from the starting index. The starting index is
 * @a tap offset behind the current delay buffer index.
 */
const DLB_LFRACT * /**< @returns Reference to internal buffer */
DLB_blk_refLdelayU
	(DLB_BLK_DELAY_STATE *state /**< Delay State */
	,unsigned int tap /**< Tap offset */
	);

/**
 * Initialize block integrator state.
 */
void
DLB_blk_LintegrateL_setup
	(DLB_BLK_INTEGRATOR_STATE *state /**< Integrator state */
	);

/**
 * Each @a dest block element is the result of integrating the @a src
 * block contained with a range [c[0]:c[1]].
 */
void
DLB_blk_LintegrateL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Input long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input long fractional block */
	,const DLB_LFRACT c[2] /**< c[0] high value, c[1] low value */
	,DLB_BLK_INTEGRATOR_STATE *state /**< Integrator state */
	);

/**
 * Each output long fractional block element is set as the maximum of the corresponding
 * 2 input block long fractional elements.
 */
void
DLB_blk_LmaxLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	);

/**
 * Each output long fractional block element is set as the maximum of the corresponding
 * input block long fractional element and a scalar long fractional.
 */
void
DLB_BLmaxBLL
	(DLB_LFRACT  *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT  *DLB_RESTRICT blk_src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT  sclr_src1 /**< Input long fractional scalar */
	);

/**
 * Each output long fractional block element is set as the maximum of the corresponding
 * 3 input block long fractional elements.
 */
void
DLB_blk_LmaxLLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	);

/**
 * Each output long fractional block element is set as the minimum of the corresponding
 * 2 input block long fractional elements.
 */
void
DLB_blk_LminLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	);

/**
 * Each output long fractional block element is set as the minimum of the corresponding
 * 3 input block long fractional elements.
 */
void
DLB_blk_LminLLL
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output long fractional block */
	,const DLB_LFRACT *DLB_RESTRICT src0 /**< Input long fractional block 1 */
	,const DLB_LFRACT *DLB_RESTRICT src1 /**< Input long fractional block 2 */
	,const DLB_LFRACT *DLB_RESTRICT src2 /**< Input long fractional block 3 */
	);

/**
 * Each output long fractional block element is set as the sum of the corresponding
 * input block long fractional element and a scalar long fractional.
 */
void
DLB_BLsaddBLL
    (DLB_LFRACT       *DLB_RESTRICT dest
    ,const DLB_LFRACT *DLB_RESTRICT src
    ,DLB_LFRACT                     a
    );

/**
 * Sets all block elements to value @a src.
 */
void
DLB_blk_Lconst
	(DLB_LFRACT *DLB_RESTRICT dest /**< Long fractional block */
	,DLB_LFRACT src /**< Value to set */
	);

/**
 * Multiplies each long fractional block element by short fractional @a g,
 * shifts this result left by @a shift and  stores the saturated & rounded result
 * into the corresponding output long fractional block element. All intermediate
 * results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity should be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code unless employing a constant shift factor.
 */
void
DLB_BLmpy_scaleBLSU
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output Long fractional block. */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input Long fractional block. */
	,DLB_SFRACT g /**< Input scalar multiplier . */
	,unsigned int shift /**< Shift amount. */
	);

/**
 * Multiplies each long fractional block element by short fractional @a g,
 * shifts this result left by @a shift and  stores the saturated & rounded result
 * into the corresponding output long fractional block element. All intermediate
 * results are stored using the extended accumulator (DLB_XACCU).
 *
 * Prior to calling this function shift validity should be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code unless employing a constant shift factor.
 */
void
DLB_BLmpy_scale_xBLSU
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output Long fractional block. */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input Long fractional block. */
	,DLB_SFRACT g /**< Input scalar multiplier . */
	,unsigned int shift /**< Shift amount. */
    );

/**
 * Multiplies each long fractional block element by the corresponding element
 * from short fractional block @a g,
 * shifts this result left by @a shift and  stores the saturated & rounded result
 * into the corresponding output long fractional block element. All intermediate
 * results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity should be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code unless employing a constant shift factor.
 */
void
DLB_blk_Lmpy_shlLSU
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output Long fractional block. */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input Long fractional block. */
	,const DLB_SFRACT *DLB_RESTRICT g /**< Input block multiplier . */
	,unsigned int shift /**< Shift amount. */
	);

/**
 * Multiplies each long fractional block element by the corresponding element
 * from long fractional block @a g,
 * shifts this result left by @a shift and  stores the saturated & rounded result
 * into the corresponding output long fractional block element. All intermediate
 * results are stored using the accumulator (DLB_ACCU).
 *
 * Prior to calling this function shift validity should be checked by using DLB_IlimlI()
 * but it will add extra cycles to your code unless employing a constant shift factor.
 */
void
DLB_blk_Lmpy_shlLLU
	(DLB_LFRACT *DLB_RESTRICT dest /**< Output Long fractional block. */
	,const DLB_LFRACT *DLB_RESTRICT src /**< Input Long fractional block. */
	,const DLB_LFRACT *DLB_RESTRICT g /**< Input block multiplier . */
	,unsigned int shift /**< Shift amount. */
	);

/**
 * (In-place)Multiplies each long fractional block element by a short fractional scalar value @a g.
 */
void
DLB_BLmpyBLS_inplace
    (DLB_LFRACT *inout  /**< Input/Output Long fractional block. */
    ,DLB_SFRACT g       /**< Short fractional scaler */
    );
    
/**
 * Shuffles left-right input into sum-diff output.
 */
void
DLB_blk_LLshuffleLL_inplace
    (DLB_LFRACT  *DLB_RESTRICT l_sum  /**< Input/Output Long fractional block. */
    ,DLB_LFRACT  *DLB_RESTRICT r_diff /**< Input/Output Long fractional block. */
    );

/**
 * Deshuffles sum-diff input into left-right output.
 */
void
DLB_blk_LLdeshuffleLL_inplace
    (DLB_LFRACT  *DLB_RESTRICT l_sum  /**< Input/Output Long fractional block. */
    ,DLB_LFRACT  *DLB_RESTRICT r_diff /**< Input/Output Long fractional block. */
    );

/**
 * State initialiser for DLB_blk_Lcrossfade_gainLS().
 */
void
DLB_blk_Lcrossfade_gainLS_setup
    (DLB_BLK_CROSSFADE_GAIN_STATE *state     /**< State of the process */
    ,const DLB_SFRACT *win                   /**< Window */
    ,unsigned int length                     /**< Length of the window. It must be an integer multiply of the processing block 
                                                  size DLB_BLK_N */
    );

/**
 * Perform a gain crossfade by multiplying the new gain @a c1
 * by the window @a win and the old gain @a c0 by the reverse of the window @a win
 * and adding the result of the two multiplications. The source @a src is then 
 * multiplied to the result and written to the destination @a dest.
 */
void
DLB_blk_Lcrossfade_gainLS
    (DLB_LFRACT  *DLB_RESTRICT dest          /**< Output long fractional block*/
    ,DLB_BLK_CROSSFADE_GAIN_STATE *state     /**< Crossfade gain state */
    ,const DLB_LFRACT *DLB_RESTRICT src      /**< Input long fractional block*/
    ,const DLB_SFRACT *win                   /**< Window of short fractional coefficients */
    ,DLB_SFRACT c0                           /**< Short fractional gain */
    ,DLB_SFRACT c1                           /**< Short fractional gain */
    ); 
    
/** @} */
/** @} */

/**
  @addtogroup dlb_dsplib_vec
  Contains a toolkit of functions for vector operations.
  Vectors can be considered as aribtrary length arrays.
  As a result, each vector operation takes the length of the vector as an argument.

  @{
 */

/**
  @name Vector Operations
  Vectors can be considered as aribtrary length arrays.
  As a result, each vector operation takes the length of the vector as an argument.
  @{
 */

/**
 * Computes the dot-product (sum of products) of two long fractional vectors of length @a n
 */
inline DLB_NACCU/** @returns Non-linear dot product result */
DLB_vec_NdotLLU
	(const DLB_LFRACT *DLB_RESTRICT a /**< Long fractional vector a. */
	,const DLB_LFRACT *DLB_RESTRICT b /**< Long fractional vector b. */
	,unsigned int n /**< Vector length */
	);

/**
 * Finds the largest absolute coefficient in the short fractional vector.
 */
inline DLB_SFRACT /** @return Largest absolute short fractional coefficient */
DLB_vec_Sabs_maxSU 
	(const DLB_SFRACT *a /**< Input vector */
	,unsigned int n      /**< Vector length (number of DLB_SFRACT elements) */
	);

/**
 * Finds the largest absolute coefficient in the long fractional vector.
 */
inline DLB_LFRACT /** @return Largest absolute long fractional coefficient */
DLB_vec_Labs_maxLU
	(const DLB_LFRACT *a /**< Input accumulator vector */
	,unsigned int n /**< Vector length */
	);

/**
 * Finds 'block exponent' of DLB_SFRACT vector.
 *
 * Determines headroom for left-shifting all vector elements.
 * The returned value can be used for DLB_vec_SshlSIU_inplace() to
 * normalize the vector such that the element with the largest
 * absolute value is in [0.5, 1.0] (unless only zeros).
*/
inline unsigned int            /** @return Number of headroom bits for left-shift */
DLB_vec_UnormSU
        (const DLB_SFRACT *a   /**< Pointer to input vector */
	,unsigned int n        /**< Vector length (number of DLB_SFRACT elements) */
	);

/**
 * Finds 'block exponent' of DLB_LFRACT vector.
 *
 * Determines headroom for left-shifting all vector elements.
 * The returned value can be used for DLB_vec_LshlLIU() to
 * normalize the vector such that the element with the largest
 * absolute value is in [0.5, 1.0] (unless only zeros).
*/
inline unsigned int            /** @return Number of headroom bits for left-shift */
DLB_vec_UnormLU
        (const DLB_LFRACT *a   /**< Pointer to input vector */
        ,unsigned int n        /**< Vector length (number of DLB_LFRACT elements) */
        );

/**
 * Multiplies input long fractional with the short fractional coefficient.
 * dest[i] = x[i] * w[i]
 */
inline void
DLB_vec_LmaskLSU
	(DLB_LFRACT dest[] /**< Output long fractional vector */
	,const DLB_LFRACT x[] /**< Input long fractional vector */
	,const DLB_SFRACT w[] /**< Input short fractional vector */
	,unsigned int n /**< Vector length */
	);

/**
 * Multiplies input long fractional with the reversed short
 * fractional coefficient.
 * dest[i] = x[i] * w[N - i]
 */
inline void
DLB_vec_Lmask_rev_coefLSU
	(DLB_LFRACT dest[] /**< Output long fractional vector */
	,const DLB_LFRACT x[] /**< Input long fractional vector */
	,const DLB_SFRACT w[] /**< Input short fractional vector */
	,unsigned int n /**< Vector length */
	);

/**
 * In-place bidirectional shift of each short fractional vector component by a valid @a shift amount.
 * Shift validity is checked by DLB_IlimsI().
 */
inline void
DLB_vec_SshlSIU_inplace
	(DLB_SFRACT *a  /**< Input/Output short fractional vector */
	,int shift      /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);

/**
 * In-place bidirectional shift of each long fractional vector component by a valid @a shift amount.
 * Shift validity is checked by DLB_IlimlI().
 */
inline void
DLB_vec_LshlLIU_inplace
	(DLB_LFRACT *a /**< Input/Output long fractional vector */
	,int shift /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);

/**
 * Bidirectional shift of each long fractional vector component by a valid @a shift amount.
 * Shift validity is checked by DLB_IlimaI().
 */
inline void
DLB_vec_LshlLIU
	(DLB_LFRACT *DLB_RESTRICT y /**< Output long fractional vector */
    ,const DLB_LFRACT *DLB_RESTRICT x /**< Input long fractional vector */
	,int shift /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);

/**
 * Sets all vector components to value @a v.
 */
inline void
DLB_vec_LsetLU
	(DLB_LFRACT *a  /**< Vector to set. */
	,DLB_LFRACT v   /**< Value to set. */
	,unsigned int n /**< Vector length */
	);

/**
 * Check if each vector component of two long fractional vectors @a a and @a b are
 * within an epsilon tolerance @a eps long fractional.
 */
inline int /** @return 1 iff all components are within tolerance, 0 if not. */
DLB_vec_IeqLLU
	(const DLB_LFRACT *DLB_RESTRICT a /**< Vector 1 to compare */
	,const DLB_LFRACT *DLB_RESTRICT b /**< Vector 2 to compare */
	,DLB_LFRACT eps /**< Epsilon tolerance */
	,unsigned int n /**< Vector length of both a & b. */
	);

/**
 * Long fractional vector multiplication by a short fractional.
 */
inline void
DLB_vec_LmpyLSU
	(DLB_LFRACT *DLB_RESTRICT a /**< Output Vector */
	,const DLB_LFRACT *DLB_RESTRICT b /**< Input Vector */
	,DLB_SFRACT g   /**< Scalar value */
	,unsigned int n /**< Vector length */
	);

/**
 * In-place long fractional vector multiplication by a short fractional.
 */
inline void
DLB_vec_LmpyLSU_inplace
	(DLB_LFRACT *inout /**< Input/Output Vector */
	,DLB_SFRACT g /**< Scalar value */
	,unsigned int n /**< Vector length */
	);

/** @} */
/** @} */

#ifndef DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS
/**
 * @addtogroup dlb_dsplib_blk
 * @{
 */

/**
 * Legacy compatibility mapping for old function name:
 * DLB_blk_Lmpy_scaleLSU() has been renamed to DLB_BLmpy_scaleBLSU() in Dolby Intrinsics v1.3.
 * @deprecated This is a deprecated function name that will not be supported in future releases.
 */
#define DLB_blk_Lmpy_scaleLSU   DLB_BLmpy_scaleBLSU

/**
 * Legacy compatibility mapping for old function name:
 * DLB_blk_Lmpy_scale_xLSU() has been renamed to DLB_BLmpy_scale_xBLSU() in Dolby Intrinsics v1.3.
 * @deprecated This is a deprecated function name that will not be supported in future releases.
 */
#define DLB_blk_Lmpy_scale_xLSU   DLB_BLmpy_scale_xBLSU

/** @} */

/**
 * @addtogroup dlb_dsplib_vec
 * @{
 */
 	
/**
 * @name Deprecated Vectorlib Functions
 * Arrays of accumulators can result in inefficient code and should therefore be avoided.
 * @{
 */

/**
 * Finds the largest coefficient in the accumulator vector.
 * @deprecated This is a deprecated function and will not be supported in future releases.
 */
inline DLB_ACCU /** @return Largest vector coefficient */
DLB_vec_AmaxAU
	(const DLB_ACCU *a /**< Input accumulator vector */
	,unsigned int n /**< Vector length */
	);

/**
 * In-place bidirectional shift of each accumulator vector component by a valid @a shift amount.
 * Shift validity is checked by DLB_IlimaI().
 * @deprecated This is a deprecated function and will not be supported in future releases.
 */
inline void
DLB_vec_AshlAIU_inplace
	(DLB_ACCU *a /**< Input/Output accumulator vector */
	,int shift /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);

/**
 * Left shifts each accumulator vector component by a valid @a shift amount,
 * and truncates the result into the corresponding long fractional
 * vector component. Shift validity is checked by DLB_IlimaI().
 * @deprecated This is a deprecated function and will not be supported in future releases.
 */
inline void
DLB_vec_Ltrunc_shlAUU
	(DLB_LFRACT *DLB_RESTRICT y /**< Output long fractional vector */
	,const DLB_ACCU *DLB_RESTRICT x /**< Input accumulator vector */
	,unsigned int shift /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);

/**
 * Multiplies each accumulator vector component by short fractional @a c,
 * shifts left (if valid) and truncates the result into the
 * corresponding long fractional vector component. Shift validity is
 * checked by DLB_IlimaI().
 * @deprecated This is a deprecated function and will not be supported in future releases.
 */
inline void
DLB_vec_Lscale_upASUU
	(DLB_LFRACT *DLB_RESTRICT y /**< Output long fractional vector */
	,const DLB_ACCU *DLB_RESTRICT x /**< Input accumulator vector */
	,DLB_SFRACT c /**< Scalar multiply quantity. */
	,unsigned int shift /**< Shift amount. */
	,unsigned int n /**< Vector length */
	);
#endif /*DLB_INTRINSICS_EXCLUDE_DEPRECATED_OPS*/

/** @} */
/** @} */

#endif /* BLK_H */

