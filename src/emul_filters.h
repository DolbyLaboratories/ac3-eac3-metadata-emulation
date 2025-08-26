/****************************************************************************
 *
 *
 * Copyright (c) 2013 Dolby International AB.
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
 *
 * @defgroup - Metadata emulation library private definitions
 * @{
* Header for Filters for the DD emulator for Dolby E
* @}
 */

#ifndef _FILTERS_H
#define _FILTERS_H

/********  Macros  ********/

#include <stdint.h>
#include "dlb_intrinsics.h"

#define LFEORDER   4              /* number of biquad stages in the LFE lowpass filter */
#define BWLIMORDER 3              /* number of biquad stages in the bandwidth-limiting lowpass filter */
#define MPHSTAGES  2              /* number of biquad stages in the main channel phase-shift filter */
#define SPHSTAGES  2              /* number of biquad stages in the surround channel phase-shift filter */
#define BQCOEFFS   5              /* number of coefficients per stage for biquad filters */
#define BQHISTORY  4

/********  Function Prototypes  ********/
void
emul_zero
    (DLB_LFRACT * DLB_RESTRICT pcmptr              /* input:   -> data to filter */
    ,int16_t      sample_offset                    /* i:   increment to reach next sample */
    ,int          blk_size );                      /* emulation block size */

void
emul_hpf
    (DLB_LFRACT * DLB_RESTRICT pcmptr              /* input:   -> data to filter */
    ,int16_t      sample_offset       /* i:   increment to reach next sample */
    ,DLB_LFRACT * DLB_RESTRICT dcoff             /* i/o:     filter state */
    ,int          emu_blk_size);

void
emul_lpf_bwlimit                 /* low pass filter */
    (DLB_LFRACT * pcmptr              /* i/o: -> data to filter */
    ,int16_t      sample_offset       /* i:   increment to reach next sample */
    ,int16_t      acmod               /* i:   channel mode, used to select correct filter */
    ,DLB_LFRACT * history             /* i/o: -> filter history */
    ,int          emu_blk_size);

void
emul_lpf_lfe  /* LFE low pass filter */
    (DLB_LFRACT *pcmptr          /* i/o: -> data to filter */
    ,int16_t     sample_offset   /* i:   increment to reach next sample */
    ,DLB_LFRACT *history         /* i/o: -> filter history */
    ,int         emu_blk_size);

void 
emul_psf_main  /* phase-shift filter */
    (DLB_LFRACT  *pcmptr          /* i/o: -> data to filter */
    ,int16_t      sample_offset   /* i:   increment to reach next sample */
    ,DLB_LFRACT  *history         /* i/o: -> filter history */
    ,int          emu_blk_size);

void 
emul_psf_surr  /* phase-shift filter */
    (DLB_LFRACT  *pcmptr         /* i/o: -> data to filter */
    ,int16_t      sample_offset  /* i:   increment to reach next sample */
    ,DLB_LFRACT  *history        /* i/o: -> filter history */
    ,int          emu_blk_size);
#endif
