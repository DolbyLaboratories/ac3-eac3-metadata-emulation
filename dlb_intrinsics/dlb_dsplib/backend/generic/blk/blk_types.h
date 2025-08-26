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
 * @brief Block Types
 */

/**
  @addtogroup dlb_dsplib_blk
  @{
 */

#ifndef DLB_BLK_TYPES_H
#define DLB_BLK_TYPES_H

/**
 * FIR with 2 taps
 */
typedef struct 
{
    DLB_LFRACT s[1]; /**< s[0] */
}   DLB_BLK_FIR2_STATE;

/**
 * FIR with 3 taps
 */
typedef struct 
{
    DLB_LFRACT s[2]; /**< s[0],s[1] */
}   DLB_BLK_FIR3_STATE;

/* N is the maximum number of taps in the FIR. */
#define DLB_BLK_FIRU_STATE(N) struct {DLB_LFRACT s[N-1];}
typedef void *DLB_BLK_FIRU_STATE_DECL;

/**
 * One-tap Direct Form 1 IIR State
 */
typedef struct 
{
    DLB_LFRACT x1; /**< x1 */
    DLB_LFRACT y1; /**< y1 */
} 
    DLB_BLK_IIR1_STATE;

/**
 * One-tap Direct Form 1 IIR State with 
 * extended accumulator precision for y1.
 */
typedef struct 
{
    DLB_LFRACT x1; /**< x1 */
    DLB_XACCU y1; /**< y1 */
} 
    DLB_BLK_IIR1X_STATE;

/**
 * State Space IIR with Error Feedback.
 */
typedef struct 
{
    DLB_ACCU s[2]; /**< s[0], s[1] */
}   DLB_BLK_IIR2SSEF_STATE;

/**
 * State Space IIR without Error Feedback.
 */
typedef struct 
{
    DLB_LFRACT s[2]; /**< s[0], s[1] */
}   DLB_BLK_IIR2SS_STATE;

/**
 * One-tap Direct Form 1 IIR State
 */
typedef struct
{
	DLB_LFRACT x[2]; /**< x[0], x[1] */
	DLB_LFRACT y[2]; /**< y[0], y[1] */
} DLB_BLK_IIR2DF1_STATE;

/** 
 * Attack Decay IIR State.
 */
typedef struct 
{
    DLB_XACCU y; /**< y */
}   DLB_BLK_ATTACK_DECAY_IIR1_STATE;

/**
 * Attack Decay with Peak II State
 */
typedef struct 
{
    DLB_XACCU y; /**< y */
}   DLB_BLK_ATTACK_DECAY_PEAK_IIR1_STATE;

/**
 * Integrator State
 */
typedef struct 
{
    DLB_LFRACT s; /**< s */
}   DLB_BLK_INTEGRATOR_STATE;

/**
 * Newton-Raphson reciprocal approximation state
 */
typedef struct
{
    DLB_SFRACT y; /**< previous best estimate */
}   DLB_BLK_NRRECIP_STATE;

/** @} */
#endif /* DLB_BLK_TYPES_H */

