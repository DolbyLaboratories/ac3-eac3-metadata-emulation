/****************************************************************************
 *
 *
 * Copyright (c) 2010-2011 Dolby International AB.
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
 * @brief Backend specific dsplib macros or wrapper functions
 */

#ifndef _dlb_dsplib_generic_h_
#define _dlb_dsplib_generic_h_

/* 
 * The backend specific header files are #defined here. The actual 
 * #includes of these files should take place in dlb_dsplib/<lib>.h.
 */

#define DLB_MATH             "dlb_dsplib/backend/generic/math/math_inlines.h"

#ifndef DLB_USE_RADIX2_FFT
#define DLB_FFT              "dlb_dsplib/backend/generic/fft-radix4/fft_generic.h"
#else
#define DLB_FFT              "dlb_dsplib/backend/generic/fft-radix2/fft_generic.h"
#endif

#define DLB_DCT_II           "dlb_dsplib/backend/generic/dct/dctII.h"
#define DLB_DCT_III          "dlb_dsplib/backend/generic/dct/dctIII.h"
#define DLB_DCT_IV           "dlb_dsplib/backend/generic/dct/dctIV.h"

#define DLB_MDCT             "dlb_dsplib/backend/generic/mdct/mdct_generic.h" 
#define DLB_IMDCT            "dlb_dsplib/backend/generic/mdct/imdct_generic.h" 

#define DLB_QMF              "dlb_dsplib/backend/generic/qmf/qmf_generic.h"

#define DLB_VEC_INLINES      "dlb_dsplib/backend/generic/blk/vec_inlines.h"
#ifndef DLB_BLK
#define DLB_BLK              "dlb_dsplib/backend/generic/blk/blk_types.h"
#endif
#ifndef DLB_COMPLEXVEC_TYPES
#ifndef DLB_USE_IN_ORDER_COMPLEX
#define DLB_COMPLEXVEC_TYPES "dlb_dsplib/backend/generic/complexvec/interleaved.h"
#else /* DLB_USE_IN_ORDER_COMPLEX */
#define DLB_COMPLEXVEC_TYPES "dlb_dsplib/backend/generic/complexvec/noninterleaved.h"
#endif /* DLB_USE_IN_ORDER_COMPLEX */
#endif
#define DLB_ALIGNMENT        "dlb_dsplib/backend/generic/alignment/dlb_std_alignment.h"

#endif  /* #ifndef _dlb_dsplib_generic_h_ */

