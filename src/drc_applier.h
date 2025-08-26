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
 * Header for code to apply gains from DRC words to audio samples
 * @}
 */

#ifndef _DRC_APPLIER_H
#define _DRC_APPLIER_H


/********  Macros  ********/

#define DRC_MAX_NCHANS 8

#define TEMP_3DB_FIX

/********  Function Prototypes  ********/

#include "dd_emulation.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  Smooth the gain values from one block to the next.
  Apply the smoothed values to a block of audio data.

  audioBlockPtrs        - (I/O) array of pointers, one per channel. Gains are applied to samples in place
  gainValue             - (I) current gain value to apply
  lastGainValue         - (I) previous gain value to apply
  blocksize             - (I) number of samples per channel
  numchans              - (I) number of channels
  sampleOffset          - (I) offset to next sample in a channel
*/
void apply_gain(DLB_LFRACT **audioBlockPtrs, DLB_LFRACT gainValue, DLB_LFRACT lastGainValue, int blocksize, int numchans, int sampleOffset, DD_EMU_CHAN_MAP *chan_map);

/*
  Convert a compr value to a DLB_LFRACT gain.

  compr                 - (I) the compr value to convert

  return the compression gain as a DLB_LFRACT.
*/
DLB_LFRACT compr_to_gain(int16_t compr);

/*
  Convert a dynrng value to a DLB_LFRACT gain.

  dynrng                - (I) the dynrng value to convert

  return the dynrngession gain as a DLB_LFRACT.
*/
DLB_LFRACT dynrng_to_gain(int16_t dynrng);

/*
  Apply dynamic range control value to audio samples.  The DRC value can be either a compr or dynrng value.

  drc_type              - (I) type of DRC: 0 = dynrng, 1 = compr
  drc_value             - (I) the DRC value, either dynrng or compr
  history               - (I/O) storage for past gain values
  dialnorm              - (I) the dialnorm value (1-31 Db)
  audioBlockPtrs        - (I/O) array of pointers, one per channel. Gains are applied to samples in place
  blocksize             - (I) number of samples per channel
  numchans              - (I) number of channels
  sampleOffset          - (I) offset to next sample in a channel
  chan_map              - (I) indicates which channels are present
  perform_boost_cut     - (I) 1 = perform boost/cut, 0 = don't
  boost                 - (I) the boost value
  cut                   - (I) the cut value
*/
void apply_drc(int16_t drc_type,
               int16_t drc_value,
               DLB_LFRACT *history,
               int dialnorm,
               DLB_LFRACT **audioBlockPtrs,
               int blocksize,
               int numchans,
               int sampleOffset,
               DD_EMU_CHAN_MAP *chan_map,
               int16_t perform_boost_cut,
               DLB_LFRACT boost,
               DLB_LFRACT cut
               );

#ifdef __cplusplus
}
#endif

#endif
