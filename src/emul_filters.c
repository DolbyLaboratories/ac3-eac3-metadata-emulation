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
 * Filters for the DD emulator for Dolby E
 * @}
 */

#include "emul_filters.h"

const DLB_LFRACT HPFCOEF = DLB_LcF(0.0001f);

/* table for indexing bandwidth-limiting filters by acmod */ 

static const int audbwcod[] =
    {
        0, 0, 0, 1, 1, 0, 0, 1, 0, 0
    };

/*      bandwidth-limiting filter
 *
 *      coefficients for 6th order elliptic, 1 kHz transition region,
 *      30db stop band rejection, 0.1 dB passband ripple
 *
 *             a1         a2         b0         b1         b2
 */
static const DLB_SFRACT bwlpfcoef[][BWLIMORDER * BQCOEFFS] =
{
    {
        DLB_ScF(0.5476869f/2), DLB_ScF(0.1947812f/2), DLB_ScF(0.5161066f/2), DLB_ScF(0.7102549f/2), DLB_ScF(0.5161066f/2),
        DLB_ScF(1.0442917f/2), DLB_ScF(0.7231065f/2), DLB_ScF(0.7097264f/2), DLB_ScF(1.3479454f/2), DLB_ScF(0.7097264f/2),
        DLB_ScF(1.2464693f/2), DLB_ScF(0.9454019f/2), DLB_ScF(0.8942602f/2), DLB_ScF(1.3668137f/2), DLB_ScF(0.8942602f/2)
    },
    {
        DLB_ScF(0.9655573f/2), DLB_ScF(0.3188062f/2), DLB_ScF(0.6028730f/2), DLB_ScF(1.0786175f/2), DLB_ScF(0.6028730f/2),
        DLB_ScF(1.4584312f/2), DLB_ScF(0.7902488f/2), DLB_ScF(0.8205224f/2), DLB_ScF(1.6061070f/2), DLB_ScF(0.8205224f/2),
        DLB_ScF(1.6327222f/2), DLB_ScF(0.9603400f/2), DLB_ScF(0.9567539f/2), DLB_ScF(1.6400965f/2), DLB_ScF(0.9567539f/2)
    },
};


/*      LFE lowpass filter
 *      coefficients for 8th order elliptic 120 Hz lpf
 *      assuming 48 kHz sampling:
 *        passband 0-120Hz              ripple < 0.01
 *        stopband 200Hz                ripple < 0.00001
 *
 *       a1        a2        b0        b1        b2
 */
static const DLB_LFRACT lfecoef[LFEORDER * BQCOEFFS] =
{
    DLB_LcF(-1.988872/4), DLB_LcF(0.988917/4), DLB_LcF(0.063059/4), DLB_LcF(-0.126074/4), DLB_LcF(0.063059/4),
    DLB_LcF(-1.994911/4), DLB_LcF(0.995118/4), DLB_LcF(0.222851/4), DLB_LcF(-0.445496/4), DLB_LcF(0.222851/4),
    DLB_LcF(-1.991431/4), DLB_LcF(0.991550/4), DLB_LcF(0.008162/4), DLB_LcF(-0.016206/4), DLB_LcF(0.008162/4),
    DLB_LcF(-1.998188/4), DLB_LcF(0.998448/4), DLB_LcF(0.134852/4), DLB_LcF(-0.269445/4), DLB_LcF(0.134852/4)
};

/* 90 degree phase shift
 *     Main phase-shift filter:
 *     Four first order all-pass filters with net 90 degree phase offset from
 *     surround phase-shift filters
*/

#define M48_PHS1_A1 -0.98809807188765
#define M48_PHS1_B0 -0.98809807188765
#define M48_PHS1_B1  1.00000000000000

#define M48_PHS2_A1 -0.93477693712497
#define M48_PHS2_B0 -0.93477693712497
#define M48_PHS2_B1  1.00000000000000

#define M48_PHS3_A1 -0.70439388923048
#define M48_PHS3_B0 -0.70439388923048
#define M48_PHS3_B1  1.00000000000000

#define M48_PHS4_A1  0.30259894541589
#define M48_PHS4_B0  0.30259894541589
#define M48_PHS4_B1  1.00000000000000

static const DLB_LFRACT LtRtCoeffs[MPHSTAGES * BQCOEFFS] =
{
        DLB_LcF((M48_PHS1_A1+M48_PHS2_A1)/4),               /* a1 */
        DLB_LcF((M48_PHS1_A1*M48_PHS2_A1)/4),               /* a2 */
        DLB_LcF((M48_PHS1_B0*M48_PHS2_B0)/4),               /* b0 */
        DLB_LcF((M48_PHS1_B0*M48_PHS2_B1+M48_PHS2_B0*M48_PHS1_B1)/4),   /* b1 */
        DLB_LcF(1.0/4),
        DLB_LcF((M48_PHS3_A1+M48_PHS4_A1)/4),               /* a1 */
        DLB_LcF((M48_PHS3_A1*M48_PHS4_A1)/4),               /* a2 */
        DLB_LcF((M48_PHS3_B0*M48_PHS4_B0)/4),               /* b0 */
        DLB_LcF((M48_PHS3_B0*M48_PHS4_B1+M48_PHS4_B0*M48_PHS3_B1)/4),   /* b1 */
        DLB_LcF(1.0/4)
};

/* 90 degree phase shift
 *     Surround phase-shift filter:
 *     Four first order all-pass filters
 */

#define S48_PHS1_A1 -0.99670588855362
#define S48_PHS1_B0 -0.99670588855362
#define S48_PHS1_B1  1.00000000000000

#define S48_PHS2_A1 -0.97106146544739
#define S48_PHS2_B0 -0.97106146544739
#define S48_PHS2_B1  1.00000000000000

#define S48_PHS3_A1 -0.85771200910643
#define S48_PHS3_B0 -0.85771200910643
#define S48_PHS3_B1  1.00000000000000

#define S48_PHS4_A1 -0.40504521656354
#define S48_PHS4_B0 -0.40504521656354
#define S48_PHS4_B1  1.00000000000000

static const DLB_LFRACT SurrCoeffs[SPHSTAGES * BQCOEFFS] =
{
        DLB_LcF((S48_PHS1_A1+S48_PHS2_A1)/4),               /* a1 */
        DLB_LcF((S48_PHS1_A1*S48_PHS2_A1)/4),               /* a2 */
        DLB_LcF((S48_PHS1_B0*S48_PHS2_B0)/4),               /* b0 */
        DLB_LcF((S48_PHS1_B0*S48_PHS2_B1+S48_PHS2_B0*S48_PHS1_B1)/4),   /* b1 */
        DLB_LcF(1.0/4),
        DLB_LcF((S48_PHS3_A1+S48_PHS4_A1)/4),               /* a1 */
        DLB_LcF((S48_PHS3_A1*S48_PHS4_A1)/4),               /* a2 */
        DLB_LcF((S48_PHS3_B0*S48_PHS4_B0)/4),               /* b0 */
        DLB_LcF((S48_PHS3_B0*S48_PHS4_B1+S48_PHS4_B0*S48_PHS3_B1)/4),   /* b1 */
        DLB_LcF(1.0/4)
};

/*
 * Private functions
 */
/*********************************************************************************
** function:    zero                                         
** description: Zeroes out channel
*********************************************************************************/
void
emul_zero
    (DLB_LFRACT * DLB_RESTRICT pcmptr              /* input:   -> data to filter */
    ,int16_t      sample_offset                    /* i:   increment to reach next sample */
    ,int          blk_size )                       /* emulation block size */
{
    int i;

    for (i = 0; i < blk_size; i++)
    {
        *pcmptr = DLB_LcF(0.0f);
        pcmptr += sample_offset;
    }
}

static void
emul_biquad
        (DLB_LFRACT       * iptr                  /*!< input: -> data to filter */
        ,int16_t            ioff                  /*!< input: step size of input data array */
        ,DLB_LFRACT       * optr                  /*!< output: -> filtered data */
        ,int16_t            ooff                  /*!< input: step size of output data array */
        ,DLB_LFRACT       * varptr                /*!< i/o: -> filter state */
        ,const DLB_SFRACT * coefptr               /*!< input -> filter coefficients */
        ,int16_t            numstages             /*!< input: number of filter stages */
        ,int16_t            numsamps);            /*!< input: number of samples to filter */


static void 
emul_biquad_ess
    ( DLB_LFRACT        *iptr      /*!< input: -> data to filter */
     ,int16_t            ioff      /*!< input: step size of input data array */
     ,DLB_LFRACT        *optr      /*!< output: -> filtered data */
     ,int16_t            ooff      /*!< input: step size of output data array */
     ,DLB_LFRACT        *DLB_RESTRICT varptr    /*!< i/o: -> filter state */
     ,const DLB_LFRACT  *DLB_RESTRICT coefptr   /*!< input: -> filter coefficients */
     ,int16_t            numstages /*!< input: number of filter stages */
     ,int16_t            numsamps);/*!< input: number of samples to filter */
      

/*********************************************************************************
** function:    hpf                                         
** description: DC blocking highpass filter
*********************************************************************************/
void
emul_hpf
    (DLB_LFRACT * DLB_RESTRICT pcmptr              /* input:   -> data to filter */
    ,int16_t      sample_offset                    /* i:   increment to reach next sample */
    ,DLB_LFRACT * DLB_RESTRICT dcoff               /* i/o:     filter state */
    ,int          blk_size )                       /* emulation block size */
{
    DLB_LFRACT off = *dcoff;
    int i;

    for (i = 0; i < blk_size; i++)
    {
        *pcmptr = DLB_LssubLL(*pcmptr, off);
        off = DLB_LmacLLL(off, *pcmptr, HPFCOEF);
        pcmptr += sample_offset;

    }
    *dcoff = off;
}

/*********************************************************************************
** function:    lpf_bwlimit
** description: Bandwidth-limiting lowpass filter
*********************************************************************************/
void
emul_lpf_bwlimit                       /* low pass filter */
    (DLB_LFRACT * pcmptr               /* i/o: -> data to filter */
    ,int16_t      sample_offset        /* i:   increment to reach next sample */
    ,int16_t      acmod                /* i:   channel mode, used to select correct filter */
    ,DLB_LFRACT * history              /* i/o: -> filter history */
    ,int          blk_size )           /* emulation block size */
{
    /* Bandwidth limiting lowpass filter (to prevent TDAC aliasing) */
    emul_biquad(pcmptr, sample_offset, pcmptr, sample_offset, history, bwlpfcoef[audbwcod[acmod]], BWLIMORDER, blk_size);
}

/*********************************************************************************
** function:    lpf_lfe
** description: LFE lowpass filter 
*********************************************************************************/
void 
emul_lpf_lfe                     /* LFE low pass filter */
    ( DLB_LFRACT *pcmptr         /* i/o: -> data to filter */
     ,int16_t     sample_offset  /* i:   increment to reach next sample */
     ,DLB_LFRACT *history        /* i/o: -> filter history */
     ,int         blk_size)      /* emulation block size */
{       
    /* LFE lowpass filter (to ensure maximum 120 Hz bandwidth) */
    emul_biquad_ess(pcmptr, sample_offset, pcmptr, sample_offset, history, lfecoef, LFEORDER, blk_size);
}

/*********************************************************************************
** function: psf_main
** main 90 degree phase-shift filter
*********************************************************************************/
void 
emul_psf_main                      /* phase-shift filter */
    ( DLB_LFRACT  *pcmptr          /* i/o: -> data to filter */
      ,int16_t     sample_offset   /* i:   increment to reach next sample */
      ,DLB_LFRACT *history         /* i/o: -> filter history */
      ,int         blk_size)       /* emulation block size */
{
    /* Use main channel coefficients */
    emul_biquad_ess(pcmptr, sample_offset, pcmptr, sample_offset, history, LtRtCoeffs, MPHSTAGES, blk_size);
}

/*********************************************************************************
** function: psf_surr
** surround 90 degree phase-shift filter
*********************************************************************************/
void 
emul_psf_surr                    /* phase-shift filter */
    (DLB_LFRACT  *pcmptr         /* i/o: -> data to filter */
     ,int16_t     sample_offset  /* i:   increment to reach next sample */
     ,DLB_LFRACT *history        /* i/o: -> filter history */
     ,int         blk_size)      /* emulation block size */
{
    /* Use surround channel coefficients */
    emul_biquad_ess(pcmptr, sample_offset, pcmptr, sample_offset, history, SurrCoeffs, SPHSTAGES, blk_size);
}




static void
emul_biquad
        (DLB_LFRACT       * iptr                  /*!< input: -> data to filter */
        ,int16_t            ioff                  /*!< input: step size of input data array */
        ,DLB_LFRACT       * optr                  /*!< output: -> filtered data */
        ,int16_t            ooff                  /*!< input: step size of output data array */
        ,DLB_LFRACT       * varptr                /*!< i/o: -> filter state */
        ,const DLB_SFRACT * coefptr               /*!< input -> filter coefficients */
        ,int16_t            numstages             /*!< input: number of filter stages */
        ,int16_t            numsamps)             /*!< input: number of samples to filter */
{
    int i, j;
    DLB_LFRACT *outptr = optr;
    DLB_LFRACT *ystate = varptr;
    DLB_LFRACT *xstate = varptr + 0x2;
    const DLB_SFRACT *coef = coefptr;
    DLB_ACCU accum;
    DLB_LFRACT insamp;
#if defined (DLB_METHOD_IS_FLOAT)
    const DLB_LFRACT denorm_guard = DLB_LcF(1e-20);
#endif

    for (i = 0; i < numstages; i++)
    {
        for (j = 0; j < numsamps; j++)
        {
            insamp = *iptr;

            #if defined (DLB_METHOD_IS_FLOAT)
            if (0 == i)                                             /* first stage only */
            {
                insamp = DLB_LsaddLL(insamp, denorm_guard);           /* Adding -400dB to prevent Intel denorm of near zero values */
            }
            #endif

            accum = DLB_AmpyLS(xstate[1], coef[4]);                 /* accum  = b2 * x[k - 2] */
            accum = DLB_AmacALS(accum, xstate[0], coef[3]);         /* accum += b1 * x[k - 1] */
            accum = DLB_AmacALS(accum, insamp, coef[2]);            /* accum += b0 * x[k] */
            accum = DLB_AmsuALS(accum, ystate[1], coef[1]);         /* accum -= a2 * y[k - 2] */
            accum = DLB_AmsuALS(accum, ystate[0], coef[0]);         /* accum -= a1 * y[k - 1] */

            /* update the delay line */
            xstate[1] = xstate[0];                                  /* x[k - 2] = x[k - 1] */
            xstate[0] = insamp;                                     /* x[k - 1] = x[k] */
            *optr = DLB_LsshlLU(DLB_LtruncA(accum), 0x1);           /* y[k] = accum * 2, compensate for coef scaling by 0.5 */
            ystate[1] = ystate[0];                                  /* y[k - 2] = y[k - 1] */
            ystate[0] = *optr;                                      /* y[k - 1] = y[k] */

            iptr += ioff;                                           /* next input sample */
            optr += ooff;                                           /* next output sample */
        }

        /* rewind and update pointers for the next stage */
        iptr = optr = outptr;
        ioff = ooff;
        coef += 0x5;
        ystate += 0x4;
        xstate += 0x4;
    }
}


/* 
 * The left-shift operation was increased to 2 and all of the coeffs used with this function
 * were changed from /2 to /4. This is due to an instability found when using the lfe lpf.
 * It has been theorized that due to the small ripple in the passband a full-scale input signal
 * could push the signal over the top.
 */
static void 
emul_biquad_ess
    ( DLB_LFRACT        *iptr                 /*!< input: -> data to filter */
      ,int16_t           ioff                 /*!< input: step size of input data array */
      ,DLB_LFRACT       *optr                 /*!< output: -> filtered data */
      ,int16_t           ooff                 /*!< input: step size of output data array */
      ,DLB_LFRACT       *DLB_RESTRICT varptr  /*!< i/o: -> filter state */
      ,const DLB_LFRACT *DLB_RESTRICT coefptr /*!< input: -> filter coefficients */
      ,int16_t           numstages            /*!< input: number of filter stages */
      ,int16_t           numsamps)            /*!< input: number of samples to filter */
{
    DLB_ACCU accum = 0;
    
    DLB_LFRACT *saveptr = optr;

    int i, j;
    
    for (i = 0; i < numstages; i++)
    {
        for (j = 0; j < numsamps; j++)
        {
            accum = DLB_AmpyLL(DLB_LnegL(*varptr), (*coefptr));  /* acc = -y1*a1 */
            varptr++;coefptr++;
            accum = DLB_AmsuALL(accum, (*varptr), (*coefptr));   /* acc -= y2*a2 */
            varptr++;coefptr++;
            accum = DLB_AmacALL(accum, (*iptr), (*coefptr));     /* acc += x0*b0 */
            coefptr++;
            accum = DLB_AmacALL(accum, (*varptr), (*coefptr));   /* acc += x1*b1 */
            varptr++;coefptr++;
            accum = DLB_AmacALL(accum, (*varptr), (*coefptr));   /* acc += x2*b2 */
            coefptr++;
            accum = DLB_A_L(DLB_LsshlLU(DLB_LtruncA(accum),2));  /* compensate for coeff scaling by 0.25 */
            *varptr  = *(varptr - 1);                            /* x2 = x1    */
            varptr--;
            *varptr-- = *iptr;                                   /* x1 = x0  */
            *optr = DLB_LtruncA(accum);                          /* y0 = acc */
            *varptr = *(varptr - 1);                             /* y2 = y1 */
            varptr--;
            *varptr = *optr;                                     /* y1 = y0 */
            iptr += ioff;
            optr += ooff;
            coefptr -= 5;
        }
        optr = saveptr;
        iptr = optr;
        ioff = ooff;
        coefptr += 5;
        varptr += 4;
    }
}
