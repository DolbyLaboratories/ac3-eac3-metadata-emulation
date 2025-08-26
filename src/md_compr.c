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
 * @file
 * @brief  Audio Compressor for DRC and Compr metadata generation including downmix clipping protection
 */

#include <string.h>         /* for memset */
#include "md_compr.h"


#define MAX_DMX_TYPES   5                               /*< max # of possible downmix types */
#define NSAMPRATES      3                               /*< # of sample rates */
#define NCHANS          COMPR_MAX_CHANNELS          /*< max # of channels including lfe */

#define N_COMP_PRESETS  5                               /*< max # of predefined compressor profiles */

static const DLB_LFRACT DYNBIAS = DLB_LcF(0.0078125/2.0); /*< dynrng clip bias */
static const DLB_LFRACT DGAININC = DLB_LcF(0.00025/2.0);  /*< dynrng clip increment value */
#define DMAXHOLD    30               /*< dynrng clip holdoff count */
static const DLB_LFRACT CMPBIAS = DLB_LcF(0.0078125);    /*< compr clip bias */

/* CGAININC is tuned for the framesize of AC3 (1536 samples). We can compensate for
   this, by scaling this value like that */
static const DLB_LFRACT CGAININC = DLB_LcF(0.0015);                     /*< compr clip increment value */
static const DLB_LFRACT CGAININC_AAC = DLB_LcF(0.0015*(1024.0/1536.0)); /*< compr clip increment value (AAC) */
static const DLB_LFRACT CGAININC_SBR = DLB_LcF(0.0015*(2048.0/1536.0)); /*< compr clip increment value (SBR) */

/* CMAXHOLD is tuned for the framesize of AC3 (1536 samples). We can compensate for
   this, by scaling this value like that */
#define CMAXHOLD        5               /*< compr clip holdoff count */
#define CMAXHOLD_AAC    8 /* (5*(1536.0/1024.0)+0.5) */ /*< compr clip holdoff count (AAC) */
#define CMAXHOLD_SBR    4 /* (5*(1536.0/2048.0)+0.5) */ /*< compr clip holdoff count (SBR) */


#define CLIPSHFT    7

static const DLB_LFRACT LOGCLIPSCL = DLB_LcF((CLIPSHFT)/48.0);

#define LOGDIALTABSZQ   (31*4+1)                /*< logdialtab size */

#define M3DB            DLB_LcF(0.707106781)
#define M3DB_2          DLB_LcF(0.707106781 / 2.0)    /* -3.0 dB divided by 2 */

/* Standard PLII mixing scale factors */
#define M1_2DB          DLB_LcF(0.870963590)        /* -1.2 dB  */
#define M6_2DB          DLB_LcF(0.489778819)        /* -6.2 dB  */
#define M1_2DB_2        DLB_LcF(0.870963590 / 2.0)  /* -1.2 dB divided by 2 */
#define M6_2DB_2        DLB_LcF(0.489778819 / 2.0)  /* -6.2 dB divided by 2 */

#define DIVISOR7Q24     128.0

static const DLB_LFRACT PLUS11DB = DLB_LcF(-0.03819444443971);
#define MAXLOG      48


static const DLB_LFRACT  ONE_OVER_SIX_DB = DLB_LcF(1.0 / 6.0206 );
static const DLB_SFRACT  SIX_DB = DLB_ScF(6.0206 / 8.0 );  /* Q3.12 */
static const DLB_LFRACT  SIX_DB_2 = DLB_LcF(6.020599913 / 16.0 );  /* Q3.12 */


static inline
DLB_LFRACT
DLB_vec_Labs_maxLU_2
(const DLB_LFRACT *a
 ,int16_t sample_offset
 ,unsigned int n
	)
{
	unsigned int i;
	DLB_LFRACT min = a[0];
	DLB_LFRACT max = a[0];
	for (i = 1; i < n; i++)
	{
		max = DLB_LmaxLL(max, a[i*sample_offset]);
		min = DLB_LminLL(min, a[i*sample_offset]);
	}
	return DLB_LmaxLL(max, DLB_LsnegL(min));
}


/*
  \brief Channel ordering equates
*/
typedef enum {
  LEFT = 0,     /*< left channel */
  RGHT = 1,     /*< right channel */
  CNTR = 2,     /*< center channel */
  LFE  = 3,     /*< low frequency effects channel */
  LSUR = 4,     /*< left surround channel */
  RSUR = 5,     /*< right surround channel */
  LSIDE = 6,        /*< side surround channel */
  RSIDE = 7,        /*< side surround channel */
  BACK = 9,     /*< side surround channel */  
  NONE = -1,        /*< channel not in use */
  MSUR = 4
} CHANNEL_POSITION;

/* Indices of different downmix types */
enum{
  COMPR_DMX_LORO_CUSTOM_INDEX  = 0,
  COMPR_DMX_LTRT_DEFAULT_INDEX = 1,
  COMPR_DMX_LTRT_CUSTOM_INDEX  = 2,
  COMPR_DMX_PLII_DEFAULT_INDEX = 3,
  COMPR_DMX_ITU_INDEX          = 4
};


typedef struct COMPR
{
  /* user set variables */
  COMPR_CHMODE channelMode;     /*< Channel Mode */
  int16_t nchans;           /*< number of channels */
  int16_t lfeon;            /*< LFE flag */
  int16_t srIndex;                 /*< Samplerate index used for accessing tables */
  int16_t numBlocksPerFrame;       /*< Number of blocks (each 256 samples) per frame */

  /* static */
  DLB_LFRACT dyn_gain;    /* states for calcDrcGain (DRC) */
  DLB_LFRACT dyn_state;
  int16_t dyn_hold;

  DLB_LFRACT compr_gain;  /* states for calcDrcGain (compr) */
  DLB_LFRACT compr_state;
  int16_t compr_hold;

  DLB_LFRACT dlim_gain;   /* states for calcClipGain (DRC) */
  int16_t dlim_hold;

  DLB_LFRACT clim_gain;   /* states for calcClipGain (compr) */
  int16_t clim_hold;

  DLB_LFRACT *lastmaxpcm; /* [COMPR_MAX_CHANNELS] */

  DLB_LFRACT lastmaxmix;

  HANDLE_DMX *hDmx;       /* [MAX_DMX_TYPES] array of structs holding downmix information */

  /* intern static memory */
  DLB_LFRACT **lwfstate;                    /* [COMPR_MAX_CHANNELS][3] */

  /* dynamic memory */
  DLB_LFRACT *log_loudness;                 /* [NBLOCKS] */
  DLB_LFRACT loudness;
  DLB_LFRACT maxpcm;                        /* max magnitude */

  DLB_LFRACT *maxmix;                       /* [NBLOCKS], calculated in pcmCalc, used in compE */

  DLB_LFRACT *lwfPCM;                       /* [compr_blk_len], holds the loudness weighted PCM data, used in comprPcmCalc */

  DLB_LFRACT *dmixLeft;                     /* [compr_blk_len], holds downmix of left channel for N samples */
  DLB_LFRACT *dmixRight;                    /* [compr_blk_len], holds downmix of right channel for N samples */
} COMPR;



/* Audio Compressor Profile */
typedef struct {
  DLB_LFRACT afastfilt;      /*< attack fast filter coefficient */
  DLB_LFRACT aslowfilt;      /*< attack slow filter coefficient */
  DLB_LFRACT athresh;        /*< attack fast/slow relative threshold */
  DLB_LFRACT dfastfilt;      /*< decay fast filter coefficient */
  DLB_LFRACT dslowfilt;      /*< decay slow filter coefficient */
  DLB_LFRACT dthresh;        /*< decay fast/slow relative threshold */
  DLB_LFRACT lfilt;          /*< low-level filter coefficient */
  DLB_LFRACT lthresh;        /*< low-level absolute threshold */
  int16_t holdoff;           /*< holdoff count */
  DLB_LFRACT thresh1;        /*< threshold #1 */
  DLB_LFRACT gain1;          /*< gain value #1 */
  DLB_LFRACT thresh2;        /*< threshold #2 */
  DLB_LFRACT gain2;          /*< gain value #2 */
  DLB_LFRACT thresh3;        /*< threshold #3 */
  DLB_LFRACT gain3;          /*< gain value #3 */
  DLB_LFRACT thresh4;        /*< threshold #4 */
  DLB_LFRACT gain4;          /*< gain value #4 */
  DLB_LFRACT thresh5;        /*< threshold #5 */
  DLB_LFRACT gain5;          /*< gain value #5 */
  DLB_LFRACT thresh6;        /*< threshold #6 */
  DLB_LFRACT gain6;          /*< gain value #6 */
  DLB_LFRACT thresh7;        /*< threshold #7 */
  DLB_LFRACT gain7;          /*< gain value #7 */
} COMPR_PROFILE;

static const signed char comprChanTab[10][8] = {
  {LEFT,  RGHT, NONE,   NONE,   NONE,   NONE,   NONE,   NONE},      /* 1+1 COMPR_CHMODE_DUALCHANNEL */
  {NONE,  NONE, CNTR,   NONE,   NONE,   NONE,   NONE,   NONE},      /* 1/0 COMPR_CHMODE_MONO */
  {LEFT,  RGHT, NONE,   NONE,   NONE,   NONE,   NONE,   NONE},      /* 2/0 COMPR_CHMODE_STEREO */
  {LEFT,  RGHT, CNTR,   NONE,   NONE,   NONE,   NONE,   NONE},      /* 3/0 COMPR_CHMODE_3_0 */
  {LEFT,  RGHT, NONE,   LFE,    MSUR,   NONE,   NONE,   NONE},      /* 2/1 COMPR_CHMODE_2_1 */
  {LEFT,  RGHT, CNTR,   LFE,    MSUR,   NONE,   NONE,   NONE},      /* 3/1 COMPR_CHMODE_3_1 */
  {LEFT,  RGHT, NONE,   LFE,    LSUR,   RSUR,   NONE,   NONE},      /* 2/2 COMPR_CHMODE_2_2 */
  {LEFT,  RGHT, CNTR,   LFE,    LSUR,   RSUR,   NONE,   NONE},      /* 3/2 COMPR_CHMODE_3_2 */
  {LEFT,  RGHT, CNTR,   LFE,    LSUR,   RSUR,   BACK,   NONE},      /* 3/3 COMPR_CHMODE_3_3 */
  {LEFT,  RGHT, CNTR,   LFE,    LSUR,   RSUR,   LSIDE,  RSIDE}      /* 3/4 COMPR_CHMODE_3_4 */
};

/*
  compression profile presets
    Note: threshold values are offset by 9.03dB to compensate for
        rms of FS sin wave (3.01dB), and
        amplitude divide by two in hpf() routine (6.02dB)
*/
static const COMPR_PROFILE comprPreset[N_COMP_PRESETS] = {
  {
    /*
      standard film compressor:
      10 msec attack time constant, 100 msec within 15 dB
      1 sec release time constant, 3 sec within 20 dB
      low level time constant disabled
      10 block decay holdoff
      2:1 boost ratio, max boost 6 dB
      5 dB null band
      2:1 early cut ratio
      20:1 final cut ratio, max cut 24 dB @ +4dB
    */
    DLB_LcF(0.5866462),                   /* afast */
    DLB_LcF(0.9480639),                   /* aslow */
    DLB_LcF(15.0/144.4943979),      /* athresh */
    DLB_LcF(0.9946809),                   /* dfast */
    DLB_LcF(0.9982238),                   /* dslow */
    DLB_LcF(20.0/144.4943979),      /* dthresh */
    DLB_LcF(0),                           /* llcoef */
    DLB_LcF(1.0),                         /* llthresh */
    10,                 /* holdoff */
    DLB_LcF(1.0),                         /* -oo (thresh) */
    DLB_LcF(6.0/48),                /* +6 (gain) */
    DLB_LcF((43.0+9.03)/144.4943979),     /* -43 */
    DLB_LcF(6.0/48),            /* +6 */
    DLB_LcF((31.0+9.03)/144.4943979),     /* -31 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((26.0+9.03)/144.4943979),     /* -26 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((16.0+9.03)/144.4943979),     /* -16 */
    DLB_LcF(-5.0/48),           /* -5 */
    DLB_LcF((-4.0+9.03)/144.4943979),     /* +4 */
    DLB_LcF(-24.0/48),          /* -24 */
    DLB_LcF(0.0),
    DLB_LcF(-24.0/48)           /* end */
  },
  {
    /*
      light film compressor:
      10 msec attack time constant, 100 msec within 15 dB
      1 sec release time constant, 3 sec within 20 dB
      low level time constant disabled
      10 block decay holdoff
      2:1 boost ratio, max boost 6 dB
      20 dB null band, 0 dB dialnorm offset
      2:1 early cut ratio
      20:1 final cut ratio, max cut 24 dB @ +9dB
    */
    DLB_LcF(0.5866462),                   /* afast */
    DLB_LcF(0.9480639),                   /* aslow */
    DLB_LcF(15.0/144.4943979),      /* athresh */
    DLB_LcF(0.9946809),                   /* dfast */
    DLB_LcF(0.9982238),                   /* dslow */
    DLB_LcF(20.0/144.4943979),      /* dthresh */
    DLB_LcF(0),                           /* llcoef */
    DLB_LcF(1.0),                         /* llthresh */
    10,                 /* holdoff */
    DLB_LcF(1.0),                         /* -oo (thresh) */
    DLB_LcF(6.0/48),            /* +6 (gain) */
    DLB_LcF((53.0+9.03)/144.4943979),     /* -53 */
    DLB_LcF(6.0/48),            /* +6 */
    DLB_LcF((41.0+9.03)/144.4943979),     /* -41 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((21.0+9.03)/144.4943979),     /* -21 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((11.0+9.03)/144.4943979),     /* -11 */
    DLB_LcF(-5.0/48),           /* -5 */
    DLB_LcF((-9.0+9.03)/144.4943979),     /* +9 */
    DLB_LcF(-24.0/48),          /* -24 */
    DLB_LcF(0.0),
    DLB_LcF(-24.0/48)           /* end */
  },
  {
    /*
      standard music compressor:
      10 msec attack time constant, 100 msec within 15 dB
      1 sec release time constant, 10 sec within 20 dB
      low level time constant disabled
      10 block decay holdoff
      2:1 boost ratio, max boost 12 dB
      5 dB null band
      2:1 early cut ratio
      20:1 cut ratio, max cut 24 dB @ +4dB
    */
    DLB_LcF(0.5866462),                   /* afast */
    DLB_LcF(0.9480639),                   /* aslow */
    DLB_LcF(15.0/144.4943979),      /* athresh */
    DLB_LcF(0.9946809),                   /* dfast */
    DLB_LcF(0.9994668),                   /* dslow */
    DLB_LcF(20.0/144.4943979),      /* dthresh */
    DLB_LcF(0),                           /* llcoef */
    DLB_LcF(1.0),                         /* llthresh */
    10,                 /* holdoff*/
    DLB_LcF(1.0),                         /* -oo (thresh) */
    DLB_LcF(12.0/48),           /* +12 (gain) */
    DLB_LcF((55.0+9.03)/144.4943979),     /* -55 */
    DLB_LcF(12.0/48),           /* +12 */
    DLB_LcF((31.0+9.03)/144.4943979),     /* -31 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((26.0+9.03)/144.4943979),     /* -26 */
    DLB_LcF(0.0/48),                /* 0 */
    DLB_LcF((16.0+9.03)/144.4943979),     /* -16 */
    DLB_LcF(-5.0/48),           /* -5 */
    DLB_LcF((-4.0+9.03)/144.4943979),     /* +4 */
    DLB_LcF(-24.0/48),          /* -24 */
    DLB_LcF(0.0),
    DLB_LcF(-24.0/48)           /* end */
  },
  {
    /*
      light music compressor:
      10 msec attack time constant, 100 msec within 15 dB
      1 sec release time constant, 3 sec within 20 dB
      low level time constant disabled
      10 block decay holdoff
      2:1 boost ratio, max boost 12 dB
      20 dB null band
      2:1 cut ratio, max cut 15 dB @ +9 dB
    */
    DLB_LcF(0.5866462),                   /* afast */
    DLB_LcF(0.9480639),                   /* aslow */
    DLB_LcF(15.0/144.4943979),      /* athresh */
    DLB_LcF(0.9946809),                   /* dfast */
    DLB_LcF(0.9982238),                   /* dslow */
    DLB_LcF(20.0/144.4943979),      /* dthresh */
    DLB_LcF(0.0),                         /* llcoef */
    DLB_LcF(1.0),                         /* llthresh */
    10,                 /* holdoff*/
    DLB_LcF(1.0),                         /* -oo (thresh) */
    DLB_LcF(12.0/48),           /* +12 (gain) */
    DLB_LcF((65.0+9.03)/144.4943979),     /* -65 */
    DLB_LcF(12.0/48),           /* +12 */
    DLB_LcF((41.0+9.03)/144.4943979),     /* -41 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((21.0+9.03)/144.4943979),     /* -21 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((-27.0+9.03)/144.4943979),    /* 27 */
    DLB_LcF(-24.0/48),          /* -24 */
    DLB_LcF(0.0),
    DLB_LcF(-24.0/48),          /* end */
    DLB_LcF(0.0),
    DLB_LcF(0.0)
  },
  {
    /*
      speech compressor:
      10 msec attack time constant, 100 msec within 10 dB
      200 msec release time constant, 1 sec within 10 dB
      low level time constant disabled
      10 block decay holdoff
      ca 5:1 boost ratio, max boost 15 dB
      5 dB null band
      2:1 early cut ratio
      20:1 final cut ratio, max cut 24 dB @ +4dB
    */
    DLB_LcF(0.5866462),                   /* afast */
    DLB_LcF(0.9480639),                   /* aslow */
    DLB_LcF(10.0/144.4943979),      /* athresh */
    DLB_LcF(0.9736857),                   /* dfast */
    DLB_LcF(0.9946809),                   /* dslow */
    DLB_LcF(10.0/144.4943979),      /* dthresh */
    DLB_LcF(0),                           /* llcoef */
    DLB_LcF(1.0),                         /* llthresh */
    10,                 /* holdoff */
    DLB_LcF(1.0),                         /* -oo (thresh) */
    DLB_LcF(15.0/48),           /* +15 (gain) */
    DLB_LcF((49.75+9.03)/144.4943979),    /* -50 */
    DLB_LcF(15.0/48),           /* +15 */
    DLB_LcF((31.0+9.03)/144.4943979),     /* -31 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((26.0+9.03)/144.4943979),     /* -26 */
    DLB_LcF(0.0/48),            /* 0 */
    DLB_LcF((16.0+9.03)/144.4943979),     /* -16 */
    DLB_LcF(-5.0/48),           /* -5 */
    DLB_LcF((-4.0+9.03)/144.4943979),     /* +4 */
    DLB_LcF(-24.0/48),          /* -24 */
    DLB_LcF(0.0),
    DLB_LcF(-24.0/48)           /* end */
  }
};



static const DLB_LFRACT defaultDmxCoefTab[3][3] = {
  {
    M3DB_2,
    M3DB_2,
    DLB_LcF(0.0)
  },
  {
    M3DB_2,
    M3DB_2,
    M3DB_2
  },
  {
    M3DB_2,
    M1_2DB_2,
    M6_2DB_2
  }
};


/*
   matches the original DD table for integer values [1:1:31]
   y = -log2(10^( (x[dB] - c_const[dB]) / 20) ) / 24
   c_const = (20*log(0.5)/(6/0.25)) * 4 * 31 = 31.10643....
   x = [0:20*log(0.5)/(6/0.25):c_const]
 */
static const DLB_LFRACT comprLogdialtabq[LOGDIALTABSZQ] = {
  DLB_LcF(0.21527777777777773), DLB_LcF(0.21354166666666663), DLB_LcF(0.21180555555555552),
  DLB_LcF(0.21006944444444439), DLB_LcF(0.20833333333333329), DLB_LcF(0.20659722222222221),
  DLB_LcF(0.20486111111111108), DLB_LcF(0.20312499999999997), DLB_LcF(0.20138888888888884),
  DLB_LcF(0.19965277777777776), DLB_LcF(0.19791666666666663), DLB_LcF(0.1961805555555555),
  DLB_LcF(0.19444444444444442), DLB_LcF(0.19270833333333329), DLB_LcF(0.19097222222222218),
  DLB_LcF(0.18923611111111108), DLB_LcF(0.18749999999999994), DLB_LcF(0.18576388888888887),
  DLB_LcF(0.18402777777777773), DLB_LcF(0.1822916666666666), DLB_LcF(0.18055555555555552),
  DLB_LcF(0.17881944444444436), DLB_LcF(0.17708333333333329), DLB_LcF(0.17534722222222215),
  DLB_LcF(0.17361111111111105)/*-6dB*/, DLB_LcF(0.17187499999999997), DLB_LcF(0.17013888888888881),
  DLB_LcF(0.16840277777777771), DLB_LcF(0.1666666666666666), DLB_LcF(0.16493055555555547),
  DLB_LcF(0.16319444444444439), DLB_LcF(0.16145833333333323), DLB_LcF(0.15972222222222215),
  DLB_LcF(0.15798611111111102), DLB_LcF(0.15624999999999994), DLB_LcF(0.15451388888888887),
  DLB_LcF(0.15277777777777771), DLB_LcF(0.15104166666666663), DLB_LcF(0.1493055555555555),
  DLB_LcF(0.14756944444444442), DLB_LcF(0.14583333333333331), DLB_LcF(0.14409722222222218),
  DLB_LcF(0.14236111111111108), DLB_LcF(0.14062499999999997), DLB_LcF(0.13888888888888887),
  DLB_LcF(0.13715277777777776), DLB_LcF(0.13541666666666666), DLB_LcF(0.13368055555555555),
  DLB_LcF(0.13194444444444442)/*-12dB*/, DLB_LcF(0.13020833333333334), DLB_LcF(0.12847222222222224),
  DLB_LcF(0.1267361111111111), DLB_LcF(0.125), DLB_LcF(0.12326388888888888), DLB_LcF(0.12152777777777779),
  DLB_LcF(0.11979166666666669), DLB_LcF(0.11805555555555558), DLB_LcF(0.11631944444444446),
  DLB_LcF(0.11458333333333337), DLB_LcF(0.11284722222222225), DLB_LcF(0.11111111111111115),
  DLB_LcF(0.10937500000000003), DLB_LcF(0.10763888888888892), DLB_LcF(0.10590277777777783),
  DLB_LcF(0.10416666666666673), DLB_LcF(0.10243055555555559), DLB_LcF(0.10069444444444448),
  DLB_LcF(0.098958333333333356), DLB_LcF(0.097222222222222238), DLB_LcF(0.095486111111111133),
  DLB_LcF(0.093749999999999986), DLB_LcF(0.092013888888888867), DLB_LcF(0.090277777777777762),
  DLB_LcF(0.088541666666666644), DLB_LcF(0.086805555555555525), DLB_LcF(0.085069444444444392),
  DLB_LcF(0.083333333333333273), DLB_LcF(0.081597222222222168), DLB_LcF(0.079861111111111049),
  DLB_LcF(0.078124999999999931), DLB_LcF(0.076388888888888784), DLB_LcF(0.074652777777777679),
  DLB_LcF(0.07291666666666656), DLB_LcF(0.071180555555555441), DLB_LcF(0.069444444444444323),
  DLB_LcF(0.067708333333333204), DLB_LcF(0.065972222222222071), DLB_LcF(0.064236111111110966),
  DLB_LcF(0.062499999999999847), DLB_LcF(0.060763888888888722), DLB_LcF(0.059027777777777596),
  DLB_LcF(0.057291666666666484), DLB_LcF(0.055555555555555358), DLB_LcF(0.053819444444444246),
  DLB_LcF(0.052083333333333121), DLB_LcF(0.050347222222222009), DLB_LcF(0.048611111111110883),
  DLB_LcF(0.046874999999999757), DLB_LcF(0.045138888888888645), DLB_LcF(0.043402777777777526),
  DLB_LcF(0.041666666666666401), DLB_LcF(0.039930555555555282), DLB_LcF(0.03819444444444417),
  DLB_LcF(0.036458333333333051), DLB_LcF(0.034722222222221925), DLB_LcF(0.032986111111110807),
  DLB_LcF(0.031249999999999691), DLB_LcF(0.029513888888888569), DLB_LcF(0.02777777777777745),
  DLB_LcF(0.026041666666666331), DLB_LcF(0.024305555555555206), DLB_LcF(0.022569444444444097),
  DLB_LcF(0.020833333333332968), DLB_LcF(0.019097222222221849), DLB_LcF(0.017361111111110727),
  DLB_LcF(0.015624999999999606), DLB_LcF(0.013888888888888489), DLB_LcF(0.012152777777777372),
  DLB_LcF(0.01041666666666625), DLB_LcF(0.0086805555555551292), DLB_LcF(0.0069444444444440121),
  DLB_LcF(0.0052083333333328907), DLB_LcF(0.0034722222222217749), DLB_LcF(0.0017361111111106513),
  DLB_LcF(0.0)
};




/*
  loudness weighting filter (approx. B-weighting)
   a1           b0          b1          b2
*/
static const DLB_LFRACT comprLwfCoef[NSAMPRATES][4] = {
  { DLB_LcF(0.9750000/2), DLB_LcF(0.5750100/4), DLB_LcF(-0.1150020/4), DLB_LcF(-0.4600080/4) }, /* 48 kHz */
  { DLB_LcF(0.9727783/2), DLB_LcF(0.6895142/4), DLB_LcF(-0.3447876/4), DLB_LcF(-0.3447876/4) }, /* 44 kHz */
  { DLB_LcF(0.9624634/2), DLB_LcF(0.8249207/4), DLB_LcF(-0.6187134/4), DLB_LcF(-0.2062378/4) }  /* 32 kHz */
};

/* PROTOTYPES for internal functions */

static void comprE(HANDLE_COMPR drc,                  /*< In/Out: Dynamic range compression */
                   const COMPR_PROFILE *profileDRC,   /*< IN Compressor profile for DRC calculation, default settings are available in table compPreset[] */
                   const COMPR_PROFILE *profileCompr, /*< IN Compressor profile for compr calculation, default settings are available in table compPreset[]*/
                   DLB_LFRACT prl,                    /*< IN Program reference level */
                   DLB_LFRACT *gainDRC,               /*< OUT DRC gain for each block in Q7.24 format*/
                   DLB_LFRACT *gainCompr              /*< OUT Compr gain in Q7.24 format */
                   );

static void comprLoudnessCalc(PCM_TYPE *pcmptr,      /*< pointer to pcm data */
                              int16_t sample_offset,  /*< stride of pcm data buffer */
                              HANDLE_COMPR hCompr,    /*< Out: Dynamic range compression */
                              int16_t blknum,         /*< the current block index */
                              DLB_LFRACT prl,         /*< IN Program reference level */
                              uint32_t  compr_blk_len );

static void comprDmxCalc(PCM_TYPE *pcmptr,           /*< pointer to pcm data */
                         int16_t sample_offset,       /*< stride of pcm data buffer */
                         HANDLE_COMPR hCompr,         /*< Out: Dynamic range compression */
                         int16_t blknum,              /*< the current block index */
                         HANDLE_DMX hDmx,             /*< struct that holds the current downmix infos */
                         uint32_t  compr_blk_len );

static void comprDrcCalc(HANDLE_COMPR drc,            /*< In/Out: Dynamic range compression */
                         int16_t blknum               /*< Block number */
                         );

static DLB_LFRACT DSPlog(DLB_LFRACT logarg);   /*< log argument */

static DLB_LFRACT DSPmeansq(DLB_LFRACT *iptr,   /*< input data */
                            uint16_t headroom,  /*< headroom of input data in bits */
                            uint32_t  compr_blk_len );

static DLB_LFRACT calcRfLev(DLB_LFRACT mixval[], /*< in: max pcm value for each block */
                            int16_t   nBlocks    /*< number of blocks to process */
                            );

static void calcClipGain(DLB_LFRACT mixval,      /* in: max pcm value */
                         DLB_LFRACT *clipgain_state, /* i/o: clip protection gain */
                         int16_t *limholdcnt,        /* i/o: decay holdoff count */
                         DLB_LFRACT clipbias,        /* in: limit offset */
                         DLB_LFRACT gaininc,         /* in: gain slew rate during decay */
                         int16_t maxhold         /* in: decay holdoff count init */
                         );

static DLB_LFRACT calcDrcGain (                       /* "artistic compression", nee calc_dynrng */
                               DLB_LFRACT powval,         /* in: loudness measure */
                               const COMPR_PROFILE *compptr,  /* in: compression profile */
                               DLB_LFRACT lim_gain,           /* in: limit gain */
                               DLB_LFRACT *state,             /* i/o: loudness filter state */
                               int16_t *holdcnt,              /* i/o: decay holdoff count */
                               DLB_LFRACT *gain               /* i/o: gain filter state */
                               );

static void biquad_lwf(DLB_LFRACT *iptr,                         /*< [in]     data to filter      */
                       int16_t sample_offset,                   /*< input: -> stride of input buffer  */
                       DLB_LFRACT *optr,                         /*< [out]    filtered data       */
                       DLB_LFRACT * DLB_RESTRICT varptr,         /*< [in/out] filter state        */
                       const DLB_LFRACT * DLB_RESTRICT coefptr,  /*< {in]     filter coefficients */
                       uint32_t  compr_blk_len,
                       void *hCompr );


/*
  Documentation for public functions resides _only_ in compr.h
  to avoid confusion of doxygen
*/
int16_t md_ComprGetRequiredBufferSize(COMPR_CHMODE cm,
                                    uint16_t numBlocksPerFrame,
                                    uint32_t *internStaticSize,
                                    uint32_t *internDynamicSize,
                                    uint32_t *externStaticSize,
                                    uint32_t  compr_blk_len )

{
  int16_t i, numChannels = 0;

  if((cm > COMPR_CHMODE_3_4) || (cm < COMPR_CHMODE_MONO))
    return COMPR_INVALID_CHANNEL_MODE;

  for(i=0; i<COMPR_MAX_CHANNELS; i++){
    if(comprChanTab[cm][i] == NONE)
      continue; 
    numChannels++;
  }


  *internStaticSize  = numChannels * ((sizeof(DLB_LFRACT*) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT))*4 + 3 * sizeof(DLB_LFRACT) * numChannels;     /* lwfstate[numChannel][3] */

  /* dynamic */
  *internDynamicSize  = 2*compr_blk_len * sizeof(DLB_LFRACT);  /* dmixLeft[N], dmixRight[compr_blk_len] */
  *internDynamicSize +=   compr_blk_len * sizeof(DLB_LFRACT);  /* lwfPCM[compr_blk_len] */
  *internDynamicSize += numBlocksPerFrame * sizeof(DLB_LFRACT);  /* maxmix[numBlocksPerFrame] */
  *internDynamicSize += numBlocksPerFrame * sizeof(DLB_LFRACT);  /* log_loudness[numBlocksPerFrame] */

  *externStaticSize  = ((sizeof(COMPR) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * 4;            /* COMPR struct */
  *externStaticSize += numChannels * sizeof(DLB_LFRACT);         /* lastmaxpcm[numChannels] */
  *externStaticSize += ((sizeof(DMX) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * MAX_DMX_TYPES * 4 + MAX_DMX_TYPES * ((sizeof(HANDLE_DMX) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * 4;

  return COMPR_OK;
}




/*
  Documentation for public functions resides _only_ in compr.h
  to avoid confusion of doxygen
*/
HANDLE_COMPR md_ComprOpen( void * pInternStatic,
                        void * pInternDynamic,
                        void * pExternStatic,
                        COMPR_CHMODE cm,
                        int16_t bLfeOn,
                        uint16_t numBlocksPerFrame,
                        uint32_t fs,
                        uint32_t  compr_blk_len )
{

  HANDLE_COMPR hCompr = (HANDLE_COMPR)pExternStatic;
  int16_t i, numChannels = 0;

  unsigned char *pInternStaticMem = pInternStatic;
  unsigned char *pInternDynamicMem = pInternDynamic;
  unsigned char *pExternStaticMem = pExternStatic;

  /* initialize all variables in the instance handle with 0 */
  memset( hCompr, 0, ((sizeof(COMPR) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * sizeof(DLB_LFRACT));

  pExternStaticMem += ((sizeof(COMPR) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * sizeof(DLB_LFRACT);

  /* check for valid channel modes */
  if((cm > COMPR_CHMODE_3_4) || (cm < COMPR_CHMODE_MONO))
    return NULL;

  /* extract the number of channels from the channel mode */
  for(i=0; i<COMPR_MAX_CHANNELS; i++){
    if(comprChanTab[cm][i] == LFE){
      if(bLfeOn){
        numChannels++;
      }
    }
    else if(comprChanTab[cm][i] == NONE)
      continue; 
    else
      numChannels++;
  }
  hCompr->nchans = numChannels;

  hCompr->lfeon = bLfeOn;
  hCompr->numBlocksPerFrame = numBlocksPerFrame;

  switch(fs){
  case 32000:
    hCompr->srIndex = 2;
    break;
  case 44100:
    hCompr->srIndex = 1;
    break;
  case 48000:
    hCompr->srIndex = 0;
    break;
  default:
    hCompr->srIndex = -1;
    return NULL;
  }

  hCompr->channelMode = cm;

  hCompr->dyn_state = DLB_L10;
  hCompr->compr_state = DLB_L10;

  /* Map internal buffers to the memory provided in the interface */
  hCompr->lwfstate = (DLB_LFRACT**)(pInternStaticMem);
  pInternStaticMem += sizeof(DLB_LFRACT *) * numChannels;

  for(i=0; i<numChannels; i++){
    hCompr->lwfstate[i] = (DLB_LFRACT*)(pInternStaticMem);
    pInternStaticMem += sizeof(DLB_LFRACT) * 3;
    memset(hCompr->lwfstate[i], 0, sizeof(DLB_LFRACT) * 3);
  }

  hCompr->lastmaxpcm = (DLB_LFRACT*)(pExternStaticMem);
  memset(hCompr->lastmaxpcm, 0, sizeof(DLB_LFRACT) * numChannels);
  pExternStaticMem += sizeof(DLB_LFRACT) * numChannels;

  hCompr->hDmx = (HANDLE_DMX*)pExternStaticMem;
  pExternStaticMem += ((sizeof(HANDLE_DMX*) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * MAX_DMX_TYPES * sizeof(DLB_LFRACT);
  for(i=0; i<MAX_DMX_TYPES; i++){
    hCompr->hDmx[i] = (HANDLE_DMX)pExternStaticMem;
    pExternStaticMem += ((sizeof(DMX) + sizeof(DLB_LFRACT)-1) / sizeof(DLB_LFRACT)) * sizeof(DLB_LFRACT);
  }

  hCompr->hDmx[0]->bitMaskVal = COMPR_DMX_LORO_CUSTOM_ACTIVE;
  hCompr->hDmx[0]->bPhaseShiftLeftCh = 0;
  hCompr->hDmx[0]->centerLevel = defaultDmxCoefTab[0][0];
  hCompr->hDmx[0]->surroundLevelOwnSide = defaultDmxCoefTab[0][1];
  hCompr->hDmx[0]->surroundLevelOppositeSide = defaultDmxCoefTab[0][2];
  hCompr->hDmx[0]->globalGain = DLB_L05;

  hCompr->hDmx[1]->bitMaskVal = COMPR_DMX_LTRT_DEFAULT_ACTIVE;
  hCompr->hDmx[1]->bPhaseShiftLeftCh = 0;
  hCompr->hDmx[1]->centerLevel = defaultDmxCoefTab[0][0];
  hCompr->hDmx[1]->surroundLevelOwnSide = defaultDmxCoefTab[0][1];
  hCompr->hDmx[1]->surroundLevelOppositeSide = defaultDmxCoefTab[0][2];
  hCompr->hDmx[1]->globalGain = DLB_L05;

  hCompr->hDmx[2]->bitMaskVal = COMPR_DMX_LTRT_CUSTOM_ACTIVE;
  hCompr->hDmx[2]->bPhaseShiftLeftCh = 1;
  hCompr->hDmx[2]->centerLevel = defaultDmxCoefTab[1][0];
  hCompr->hDmx[2]->surroundLevelOwnSide = defaultDmxCoefTab[1][1];
  hCompr->hDmx[2]->surroundLevelOppositeSide = defaultDmxCoefTab[1][2];
  hCompr->hDmx[2]->globalGain = DLB_L05;

  hCompr->hDmx[3]->bitMaskVal = COMPR_DMX_PLII_DEFAULT_ACTIVE;
  hCompr->hDmx[3]->bPhaseShiftLeftCh = 1;
  hCompr->hDmx[3]->centerLevel = defaultDmxCoefTab[2][0];
  hCompr->hDmx[3]->surroundLevelOwnSide = defaultDmxCoefTab[2][1];
  hCompr->hDmx[3]->surroundLevelOppositeSide = defaultDmxCoefTab[2][2];
  hCompr->hDmx[3]->globalGain = DLB_L05;

  /* ITU Downmix: */
  hCompr->hDmx[4]->bitMaskVal = COMPR_DMX_ITU_ACTIVE;
  hCompr->hDmx[4]->bPhaseShiftLeftCh = 0;
  hCompr->hDmx[4]->centerLevel = defaultDmxCoefTab[0][0];
  hCompr->hDmx[4]->surroundLevelOwnSide = defaultDmxCoefTab[0][1];
  hCompr->hDmx[4]->surroundLevelOppositeSide = defaultDmxCoefTab[0][2];
  hCompr->hDmx[4]->globalGain = DLB_L05;

  hCompr->log_loudness = (DLB_LFRACT*)(pInternDynamicMem);
  pInternDynamicMem += sizeof(DLB_LFRACT) * numBlocksPerFrame;

  hCompr->maxmix = (DLB_LFRACT*)(pInternDynamicMem);
  pInternDynamicMem += sizeof(DLB_LFRACT) * numBlocksPerFrame;

  hCompr->dmixLeft = (DLB_LFRACT*)(pInternDynamicMem);
  pInternDynamicMem += sizeof(DLB_LFRACT) * compr_blk_len;
  hCompr->dmixRight = (DLB_LFRACT*)(pInternDynamicMem);
  pInternDynamicMem += sizeof(DLB_LFRACT) * compr_blk_len;

  hCompr->lwfPCM = (DLB_LFRACT*)(pInternDynamicMem);
  pInternDynamicMem += sizeof(DLB_LFRACT) * compr_blk_len;

  return hCompr;
}


/*
  Documentation for public functions resides _only_ in compr.h
  to avoid confusion of doxygen
*/
int16_t md_ComprProcess(HANDLE_COMPR hCompr,
                      DLB_LFRACT prl,
                      COMPR_PROFILE_TYPE profileDRC,
                      COMPR_PROFILE_TYPE profileCompr,
                      PCM_TYPE *pcmIn,
                      uint16_t activeDmxBitmask,
                      HANDLE_DMX_COEFS LoRoCoefs,
                      HANDLE_DMX_COEFS LtRtCoefs,
                      DLB_LFRACT *gainDRC,
                      DLB_LFRACT *gainCompr,
                      uint32_t compr_blk_len,
                      uint32_t sample_offset )
{
  uint16_t blknum, dmx;
  unsigned int  index;
  HANDLE_DMX hDmix;
  const COMPR_PROFILE *pProfileCompr;
  const COMPR_PROFILE *pProfileDRC;

  if((hCompr == 0) || (pcmIn == 0))
    return COMPR_INVALID_PTR;

  if(profileDRC > 5)
    return COMPR_INVALID_COMPR_PROFILE;

  if(profileCompr > 5)
    return COMPR_INVALID_COMPR_PROFILE;

  if(profileCompr)
    pProfileCompr = &comprPreset[profileCompr-1];
  else
    pProfileCompr = 0;

  if(profileDRC)
    pProfileDRC = &comprPreset[profileDRC-1];
  else
    pProfileDRC = 0;


  if(LoRoCoefs != 0){
    /* convert downmix coef parameter to internal format */
    hCompr->hDmx[COMPR_DMX_LORO_CUSTOM_INDEX]->globalGain = LoRoCoefs->globalGain;
    hCompr->hDmx[COMPR_DMX_LORO_CUSTOM_INDEX]->centerLevel = LoRoCoefs->centerLevel;
    hCompr->hDmx[COMPR_DMX_LORO_CUSTOM_INDEX]->surroundLevelOppositeSide = LoRoCoefs->surroundLevel;
    hCompr->hDmx[COMPR_DMX_LORO_CUSTOM_INDEX]->surroundLevelOwnSide = LoRoCoefs->surroundLevel;
  }

  if(LtRtCoefs != 0){
    /* convert downmix coef parameter to internal format */
    hCompr->hDmx[COMPR_DMX_LTRT_CUSTOM_INDEX]->globalGain = LtRtCoefs->globalGain;
    hCompr->hDmx[COMPR_DMX_LTRT_CUSTOM_INDEX]->centerLevel = LtRtCoefs->centerLevel;
    hCompr->hDmx[COMPR_DMX_LTRT_CUSTOM_INDEX]->surroundLevelOppositeSide = LtRtCoefs->surroundLevel;
    hCompr->hDmx[COMPR_DMX_LTRT_CUSTOM_INDEX]->surroundLevelOwnSide = LtRtCoefs->surroundLevel;
  }

  index = (unsigned int) -(DLB_32srndL(DLB_LminLL(DLB_L00, prl)) >> 22);
  index = index > LOGDIALTABSZQ-1 ? LOGDIALTABSZQ-1 : index;
  prl = comprLogdialtabq[index];

  /* loop over all blocks of compr_blk_len samples */
  for ( blknum = 0; blknum < hCompr->numBlocksPerFrame; blknum++ ) 
  {
    /* Calculate the loudness of the input signal for each block */
    comprLoudnessCalc(pcmIn, sample_offset, hCompr, blknum, prl, compr_blk_len);

    if (hCompr->channelMode >= COMPR_CHMODE_3_0) 
	{
		/* In case of multichannel input, calculate all activated dmx types
          and return their max values for clipping protection              */
		for ( dmx = 0; dmx < MAX_DMX_TYPES; dmx++ ) 
		{
			hDmix = hCompr->hDmx[dmx];
			hDmix->maxVal = DLB_L00;
			if(hDmix->bitMaskVal & activeDmxBitmask)
			{
				comprDmxCalc( pcmIn, sample_offset, hCompr, blknum, hDmix, compr_blk_len);
			}
		}
    }

    comprDrcCalc( hCompr, blknum);
  }

  /* Calculates gain values depending on the compressor profile and the
     ensures that possible downmixes will not clip                      */
  comprE( hCompr, pProfileDRC, pProfileCompr, prl, gainDRC, gainCompr);

  return COMPR_OK;
}


/*
  \brief  Calculates gain values depending on the compressor profile and the
          ensures that possible downmixes will not clip


  Note that when compr is set to zero, the decoder is signalled to use
  the dynrng value instead of the compr value.  There is currently no
  checking done to ensure that this will prevent clipping.  Thus any
  compr value of zero is overwritten with the value -1.

  \return
*/
static void comprE(HANDLE_COMPR hCompr,               /*< In/Out: Dynamic range compression */
                   const COMPR_PROFILE *profileDRC,   /*< IN Compressor profile for DRC calculation, default settings are available in table compPreset[] */
                   const COMPR_PROFILE *profileCompr, /*< IN Compressor profile for compr calculation, default settings are available in table compPreset[]*/
                   DLB_LFRACT prl,                    /*< IN Program reference level (Q7.24) */
                   DLB_LFRACT *gainDRC,               /*< OUT DRC gain for each block in Q7.24 format*/
                   DLB_LFRACT *gainCompr              /*< OUT Compr gain in Q7.24 format */
                   )
{
  int blknum;
  DLB_LFRACT mincomprgain = DLB_L10;
  DLB_LFRACT comprGainInc = CGAININC;
  int16_t comprMaxHold = CMAXHOLD;

  /* Compute dynamic range compression values */

  for (blknum = 0; blknum < hCompr->numBlocksPerFrame; blknum++) {

    /* Convert to Log and adjust level measure for dialog norm effects */
    hCompr->maxmix[blknum] = DSPlog(hCompr->maxmix[blknum]);

    hCompr->maxmix[blknum] = DLB_LsaddLL(hCompr->maxmix[blknum], DLB_LshrLU(prl, 1));

    /* Calculate clip protection gain */
    calcClipGain(hCompr->maxmix[blknum], &hCompr->dlim_gain, &hCompr->dlim_hold,
                 DYNBIAS, DGAININC, DMAXHOLD);

    /* calc "artistic" compression gain, and limit by clip gain */
    calcDrcGain(hCompr->log_loudness[blknum],
                profileDRC,
                hCompr->dlim_gain,
                &hCompr->dyn_state,
                &hCompr->dyn_hold,
                &hCompr->dyn_gain);

    gainDRC[blknum] = DLB_LmpyLL(hCompr->dyn_gain, SIX_DB_2);
  } /* end for (blknum) */

  switch(hCompr->numBlocksPerFrame){
  case 4:
    comprGainInc = CGAININC_AAC;
    comprMaxHold = CMAXHOLD_AAC;
    break;
  case 6:
    comprGainInc = CGAININC;  /* DD */
    comprMaxHold = CMAXHOLD;  /* DD */
    break;
  case 8:
    comprGainInc = CGAININC_SBR;
    comprMaxHold = CMAXHOLD_SBR;
  }

  /*  Compute peak compression values */
  /*  Compute compr gain word(s) */
  /*  Calculate clip protection gain */

  calcClipGain(calcRfLev(hCompr->maxmix, hCompr->numBlocksPerFrame),
               &hCompr->clim_gain,
               &hCompr->clim_hold,
               CMPBIAS,
               comprGainInc,
               comprMaxHold);

  /*  Limit compr_gain to clip protection value (clim_gain) */

  /* calc "artistic" compression gain, and limit by clip gain */
  mincomprgain = DLB_L10;
  for (blknum=0; blknum< hCompr->numBlocksPerFrame; blknum++)   {
    calcDrcGain(hCompr->log_loudness[blknum],
                profileCompr,
                hCompr->clim_gain,
                &hCompr->compr_state,
                &hCompr->compr_hold,
                &hCompr->compr_gain);
    mincomprgain = DLB_LminLL(hCompr->compr_gain, mincomprgain);
  }

  gainCompr[0] = DLB_LmpyLL(mincomprgain, SIX_DB_2);

} /* aacEncCompE */




/****************************************************************************/
/*
     \brief Calculates the loudness weighted input signal and computes the
            logarithm of the maximum of the absolute values. The program
            reference level is added on the result.

     \return
*/
/****************************************************************************/
static void comprLoudnessCalc(PCM_TYPE *pcmptr,    /*< pointer to pcm data */
                              int16_t sample_offset,/*< stride of pcm data buffer */
                              HANDLE_COMPR hCompr,  /*< handle to memory */
                              int16_t blknum,       /*< the current block index */
                              DLB_LFRACT prl,       /*< IN Program reference level */
                              uint32_t  compr_blk_len )
{
  DLB_LFRACT lastmax;
  DLB_LFRACT maxsamp;
  uint16_t headroom;
  int16_t chan;

  PCM_TYPE* pCurrPcmBlock;

  hCompr->loudness = DLB_L00;
  hCompr->maxpcm = DLB_L00;

  for(chan = 0; chan < hCompr->nchans; chan++){

    pCurrPcmBlock = pcmptr + (compr_blk_len * blknum * sample_offset) + chan; 

    maxsamp = DLB_vec_Labs_maxLU_2(pCurrPcmBlock, sample_offset, compr_blk_len); /* Compensate for scale in hpf() */

    headroom = DLB_UnormL(maxsamp);                            /* save headroom for meansquare */

    lastmax = hCompr->lastmaxpcm[chan];
    hCompr->lastmaxpcm[chan] = maxsamp;

    /* max with previous compr_blk_len samples */
    maxsamp = DLB_LmaxLL(maxsamp, lastmax);

    hCompr->maxpcm = DLB_LmaxLL(maxsamp, hCompr->maxpcm); 

    if ( comprChanTab[hCompr->channelMode][chan] != LFE ) {  /* for each chan, except LFE */
      /* apply loudness weighting */
      biquad_lwf(pCurrPcmBlock, sample_offset, hCompr->lwfPCM, hCompr->lwfstate[chan], comprLwfCoef[hCompr->srIndex], compr_blk_len, hCompr);

      /* compute MS power */
      hCompr->loudness = DLB_LsaddLL(hCompr->loudness, DSPmeansq(hCompr->lwfPCM, headroom, compr_blk_len ));
    }
  }

  hCompr->log_loudness[blknum] = DSPlog(hCompr->loudness);

  /* Adjust loudness for dialnorm */
  hCompr->log_loudness[blknum] = DLB_LsaddLL(hCompr->log_loudness[blknum], prl);
}


/****************************************************************************/
/*
     \brief Calculates worst case downmix level for the activated downmix types

     \return
*/
/****************************************************************************/
static void comprDmxCalc(PCM_TYPE *pcmptr,         /*< pointer to pcm data */
                         int16_t sample_offset,     /*< stride of pcm data buffer */
                         HANDLE_COMPR hCompr,       /*< Out: Dynamic range compression */
                         int16_t blknum,            /*< the current block index */
                         HANDLE_DMX hDmx,           /*< struct that holds the current downmix infos */
                         uint32_t  compr_blk_len )
{
  int i, chan;
  PCM_TYPE *currPcmPtr;

  DLB_LFRACT facLeft, facRight, dmixCommon;
  DLB_LFRACT *dmixLeft, *dmixRight;

  /* Estimate Input Level (worst case downmix) */

  /*
    Accumulate current channel into Lt/Rt and Lo/Ro downmixes
    shift by CLIPSHFT to avoid overflow in downmix
  */
  memset(hCompr->dmixRight, 0, sizeof(hCompr->dmixRight[0]) * compr_blk_len);
  memset(hCompr->dmixLeft,  0, sizeof(hCompr->dmixLeft[0]) * compr_blk_len);

  dmixLeft = hCompr->dmixLeft;
  dmixRight = hCompr->dmixRight;

  for(chan=0; chan<COMPR_MAX_CHANNELS /*hCompr->nchans*/; chan++)
  { 
	currPcmPtr = pcmptr + (blknum * compr_blk_len * sample_offset) + chan;

	switch(comprChanTab[hCompr->channelMode][chan])
	{
    case LEFT:
      facLeft = DLB_LshrLU(hDmx->globalGain, CLIPSHFT - 1);  /* scale 1 less, global gain is Q1.30 */

      /* Roll in 1dB boost to better match Pro Licensing's reference plots */
      /* See also biquad_lwf() */
      facLeft = DLB_LsmacLLL(facLeft, facLeft, DLB_LcF(0.12201));
      
      for(i=0; i<compr_blk_len; i++)
	  {
        dmixLeft[i] = DLB_LsmacLLL(dmixLeft[i], facLeft, currPcmPtr[i*sample_offset]);
      }
      break;
    case CNTR:  /* center channel is symmetric! scale one less because level is Q1.30 */
      facLeft = DLB_LshrLU(DLB_LsmpyLL(hDmx->centerLevel, hDmx->globalGain), CLIPSHFT - 2);

      /* Roll in 1dB boost to better match Pro Licensing's reference plots */
      /* See also biquad_lwf() */
      facLeft = DLB_LsmacLLL(facLeft, facLeft, DLB_LcF(0.12201));
      
      for(i=0; i<compr_blk_len; i++)
	  { 
        dmixCommon = DLB_LmpyLL(facLeft, currPcmPtr[i*sample_offset]);
        dmixLeft[i] = DLB_LsaddLL(dmixLeft[i], dmixCommon);
        dmixRight[i] = DLB_LsaddLL(dmixRight[i], dmixCommon);
      }
      break;
    case RGHT:
      facRight = DLB_LshrLU(hDmx->globalGain, CLIPSHFT - 1);

      /* Roll in 1dB boost to better match Pro Licensing's reference plots */
      /* See also biquad_lwf() */
      facRight = DLB_LsmacLLL(facRight, facRight, DLB_LcF(0.12201));
      
      for(i=0; i<compr_blk_len; i++)
	  {
        dmixRight[i] = DLB_LsmacLLL(dmixRight[i], facRight, currPcmPtr[i*sample_offset]);
      }
      break;
    case LSUR:
      facLeft = DLB_LshrLU(DLB_LsmpyLL(hDmx->surroundLevelOwnSide, hDmx->globalGain), CLIPSHFT - 2);
      facRight = DLB_LshrLU(DLB_LsmpyLL(hDmx->surroundLevelOppositeSide, hDmx->globalGain), CLIPSHFT - 2);

      /* Roll in 1dB boost to better match Pro Licensing's reference plots */
      /* See also biquad_lwf() */
      facLeft = DLB_LsmacLLL(facLeft, facLeft, DLB_LcF(0.12201));
      facRight = DLB_LsmacLLL(facRight, facRight, DLB_LcF(0.12201));

      if(hDmx->bPhaseShiftLeftCh)
        facLeft = DLB_LsnegL(facLeft);

      for(i=0; i<compr_blk_len; i++)
	  {
        dmixLeft[i] = DLB_LsmacLLL(dmixLeft[i], facLeft, currPcmPtr[i*sample_offset]);
      }
      if(!DLB_IeqLL(facRight, DLB_L00))          /* usually LSUR is only fed into the left channel */
	  {
		  for(i=0; i<compr_blk_len; i++)
		  {
			dmixRight[i] = DLB_LsmacLLL(dmixRight[i], facRight, currPcmPtr[i*sample_offset]);
	      }
      }
      break;
    case RSUR:
      facLeft = DLB_LshrLU(DLB_LsmpyLL(hDmx->surroundLevelOppositeSide, hDmx->globalGain), CLIPSHFT - 2);
      facRight = DLB_LshrLU(DLB_LsmpyLL(hDmx->surroundLevelOwnSide, hDmx->globalGain), CLIPSHFT - 2);

      /* Roll in 1dB boost to better match Pro Licensing's reference plots */
      /* See also biquad_lwf() */
      facLeft = DLB_LsmacLLL(facLeft, facLeft, DLB_LcF(0.12201));
      facRight = DLB_LsmacLLL(facRight, facRight, DLB_LcF(0.12201));

      if(hDmx->bPhaseShiftLeftCh)
        facLeft = DLB_LsnegL(facLeft);

      if(!DLB_IeqLL(facLeft, DLB_L00))          /* usually RSUR is only fed into the right channel */
      {
		  for(i=0; i<compr_blk_len; i++)
		  {
			dmixLeft[i] = DLB_LsmacLLL(dmixLeft[i], facLeft, currPcmPtr[i*sample_offset]);
          }
      }
      for(i=0; i<compr_blk_len; i++)
	  {
		dmixRight[i] = DLB_LsmacLLL(dmixRight[i], facRight, currPcmPtr[i*sample_offset]);
      }
      break;
    case NONE:
      break;
    }
  }
  hDmx->maxVal = DLB_vec_Labs_maxLU(dmixLeft, compr_blk_len);
  hDmx->maxVal = DLB_LmaxLL(hDmx->maxVal, DLB_vec_Labs_maxLU(dmixRight, compr_blk_len));

} /* comprDmxCalc */


/*
  \brief  dynamic range compression calculations...
  estimate input level (find worst-case downmix)
*/
static void comprDrcCalc(HANDLE_COMPR hCompr,       /*< In/Out: Dynamic range compression */
                         int16_t blknum            /*< Block number */
                         )
{

  DLB_LFRACT maxsamp, tmp = DLB_L00;
  int16_t dmx;

  /* Isolate worst-case downmix results for current block (2nSamples samples) */

  /* scale by CLIPSCL to match downmix scaling */
  maxsamp = DLB_LshrLU(hCompr->maxpcm, CLIPSHFT);

  if (hCompr->channelMode >= COMPR_CHMODE_3_0) {
    /* max non-dmix with worst-case downmix of previous compr_blk_len samples */
    maxsamp = DLB_LmaxLL(hCompr->lastmaxmix, maxsamp);
    for(dmx=0; dmx < MAX_DMX_TYPES; dmx++){
      tmp = DLB_LmaxLL(hCompr->hDmx[dmx]->maxVal, tmp);
    }
    /* combine current d'mix with non-d'mix and previous d'mix */
    maxsamp = DLB_LmaxLL(maxsamp, tmp);
  }
  hCompr->lastmaxmix = tmp;

  hCompr->maxmix[blknum] = maxsamp;
} /* comprDrcCalc */



/*
  \brief   Log approximation:  y = -log2(logarg) / maxlog
*/
static DLB_LFRACT DSPlog(DLB_LFRACT logarg)    /* log argument */
{
  DLB_LFRACT powval, logpoly;
  unsigned int powexp;

  const DLB_LFRACT coeff0 = DLB_LcF(0.6626105);
  const DLB_LFRACT coeff1 = DLB_LcF(0.9981958);
  const DLB_LFRACT coeff2 = DLB_LcF(0.3372223);
  const DLB_LFRACT cf1dmax = DLB_LcF(1.0/MAXLOG);
  const DLB_LFRACT cf4dmax = DLB_LcF(4.0/MAXLOG);

  /* Normalize power measure */

  powexp = DLB_UnormL(logarg);
  if (powexp >= (DLB_LFRACT_MANTBITS-1)) {
    return DLB_L10;
  }
  else {
    logarg = DLB_LshlLU(logarg, powexp);

    /* Perform log using polynomial approximation */

    powval = DLB_L_32((int32_t)powexp * DLB_32srndL(cf1dmax));

    logpoly = DLB_LaddLL(DLB_LsubLL(coeff0, DLB_LmpyLL(logarg, coeff1)),
             DLB_LmpyLL(DLB_LmpyLL(logarg, logarg), coeff2));

    powval = DLB_LmacLLL(powval, logpoly, cf4dmax);

    return powval;
  }
} /* DSPlog */



/*
  \brief   Calculate mean square value

  \return  mean square value
*/
static DLB_LFRACT DSPmeansq(DLB_LFRACT *iptr,  /*< input data */
                            uint16_t headroom /*< headroom of input data in bits */
                            ,uint32_t  compr_blk_len )
{
  int16_t count;
  DLB_LFRACT sumsq;
  uint16_t ils;          /* inner loop shift */

  ils = 4-headroom > 0 ? 4-headroom : 0;

  sumsq = DLB_L00;
  for (count = 0; count < compr_blk_len; count++) {
    DLB_LFRACT tmp = DLB_LshrLU(*iptr, ils);
    sumsq = DLB_LsmacLLL(sumsq, tmp, tmp);
    iptr++;
  }

  sumsq = DLB_LshrLU(sumsq, 8-(ils+ils));
  sumsq = DLB_LmaxLL(sumsq, DLB_L00);

  return(sumsq);
}


/*
  \brief  Calculates the maximum gain allowed without clipping

  \return
*/
static void calcClipGain(DLB_LFRACT mixval,        /* in: max pcm value (signal and downmix) */
                         DLB_LFRACT *clipgain_state,   /* i/o: clip protection gain */
                         int16_t *limholdcnt,         /* i/o: decay holdoff count */
                         DLB_LFRACT clipbias,          /* in: limit offset */
                         DLB_LFRACT gaininc,           /* in: gain slew rate during decay */
                         int16_t maxhold           /* in: decay holdoff count init */
                         )
{
  DLB_LFRACT clipgain;

  /* Calc (clip level)/(max pcm sample) */

  clipgain = DLB_LssubLL(mixval, LOGCLIPSCL);

  /* re-scale to units of dB/48 */

  clipgain = DLB_LsaddLL(DLB_LsshlLU(clipgain, 2), DLB_LsshlLU(clipgain, 1)); /* clipgain*=6, use saturation */

  /* Make clip gain a little more conservative */

  clipgain = DLB_LssubLL(clipgain, clipbias);

  /* Check for attack */

  if (DLB_IltLL(clipgain, DLB_LsaddLL(*clipgain_state, gaininc))) { /* attack */
    *limholdcnt = maxhold; /* reset hold count */
  }
  else { /* decay */
    if (*limholdcnt) { /* hold current gain */
      clipgain = *clipgain_state;
      *limholdcnt -= 1;
    }
    else { /* slowly increase allowable gain */
      clipgain = DLB_LsaddLL(*clipgain_state, gaininc);
    }
  }

  *clipgain_state = clipgain; /* save gain state */

} /* calcClipGain */



/****************************************************************************
;*
;*   Subroutine Name:   calcDrcGain
;*
;*   Description:   Compute dynamic range compression value.
;*
;******************************************************************************/
static DLB_LFRACT calcDrcGain (                      /* "artistic compression", nee calc_dynrng */
                               DLB_LFRACT powval,            /* in: loudness measure */
                               const COMPR_PROFILE *compptr, /* in: compression profile */
                               DLB_LFRACT lim_gain,      /* in: limit gain */
                               DLB_LFRACT *state,        /* i/o: loudness filter state */
                               int16_t *holdcnt,            /* i/o: decay holdoff count */
                               DLB_LFRACT *gain              /* i/o: gain filter state */
                               )
{
  DLB_LFRACT powdiff, filtcoef, interp_gain;
  DLB_LFRACT x0, y0, x1, y1;
  const DLB_LFRACT *curveptr;

  /* Check for compression disabled */

  if (compptr == 0) {
    *state = DLB_L10;
    *holdcnt = 0;
    *gain = DLB_LminLL(DLB_L00, lim_gain);

    return *gain;
  }

  /* Determine compression characteristic */

  if (DLB_IleqLL(compptr->thresh1, powval)) {
    interp_gain = compptr->gain1;
  }
  else {
    curveptr = &(compptr->thresh1);
    while (DLB_IltLL(powval, *curveptr)) {
      curveptr += 2;                /* skip to next threshold */
    }

    /*  Linear interpolation */

    x0 = *(curveptr - 2);
    y0 = *(curveptr - 1);
    x1 = *(curveptr);
    y1 = *(curveptr + 1);

    interp_gain = DLB_LsaddLL(y0, DLB_LsmpyLL(DLB_LssubLL(y1, y0), DLB_LdivLL(DLB_LssubLL(powval, x0), DLB_LssubLL(x1, x0))));
  }

  /* MS energy smoothing filter */

  powdiff = DLB_LssubLL(powval, *state);
  if (DLB_IltLL(powdiff, DLB_L00)) {                /*  Attack case */
    if (DLB_IltLL(*gain, interp_gain)) {/*  v6.8 DRC fix */
      filtcoef = compptr->dslowfilt;
    }
    else {
      *holdcnt = compptr->holdoff;
      if (DLB_IltLL(DLB_LnegL(powdiff), compptr->athresh)) {
        filtcoef = compptr->aslowfilt;
      }
      else {
        filtcoef = compptr->afastfilt;
      }
    }
  }
  else {                            /* Decay case */
    if (*holdcnt) {
      *holdcnt -= 1;
      filtcoef = DLB_L10;
    }
    else {
      if (DLB_IltLL(compptr->lthresh, powval)) {
        filtcoef = compptr->lfilt;
      }
      else if (DLB_IltLL(powdiff, compptr->dthresh)) {  /* slow decay */
        filtcoef = compptr->dslowfilt;
      }
      else { /* fast decay */
        filtcoef = compptr->dfastfilt;
      }
    }
  } /* powdiff */

  *state = DLB_LsaddLL(DLB_LsmpyLL(*state, filtcoef), DLB_LsmpyLL(powval, DLB_LssubLL(DLB_L10, filtcoef)) );

  /* Gain smoothing filter */
  *gain = DLB_LsaddLL(DLB_LsmpyLL(*gain, filtcoef), DLB_LsmpyLL(interp_gain, DLB_LssubLL(DLB_L10, filtcoef)) );

  *gain = DLB_LminLL(*gain, lim_gain);

  return (*gain);
} /* end calcDrcGain() */



/*
  \brief  Compute worst case rf signal level

  \return max possible rf signal level
*/

static DLB_LFRACT calcRfLev(DLB_LFRACT mixval[], /*< in: max pcm value for each block */
                            int16_t   nBlocks   /*< number of blocks to process */
                            )
{
  DLB_LFRACT rflev;
  int blknum;

  /* Compute compr downmixing protection */

  rflev = DLB_L10;

  for (blknum = 0; blknum < nBlocks; blknum++) {
    if (DLB_IltLL(mixval[blknum], rflev)) {
      rflev = mixval[blknum];
    }
  }
  // Dolby Digital Plus or later decoders do not offer the RF overmod protection requiring this 11dB
  // This means there that early decoders will clip earlier in RF mode than the emulation
  // To truly emulate early decoders uncomment this line 
  //rflev = DLB_LsaddLL(rflev, PLUS11DB); /* add 11dB RF output offest */

  return rflev;

} /* end calcRfLev */




/*
  \brief  Biquad IIR filter with one stages, coefficient a2 is 0
*/
static void biquad_lwf(DLB_LFRACT *iptr,                        /*< input:  -> data to filter      */
                       int16_t sample_offset,                   /*< input: -> stride of input buffer  */
                       DLB_LFRACT *optr,                        /*< output: -> filtered data       */
                       DLB_LFRACT * DLB_RESTRICT varptr,        /*< i/o:    -> filter state        */
                       const DLB_LFRACT * DLB_RESTRICT coefptr  /*< input   -> filter coefficients */
                       ,unsigned int compr_blk_len
                       ,void * hCompr)
{
  DLB_LFRACT accum, a1, b0, b1, b2;
  DLB_LFRACT x1, x2;
  int j;

  a1 = coefptr[0];            /* negative sign is already in table */
  b0 = coefptr[1];            /* additional right shift for b0,b1,b2 is already done in table */
  b1 = coefptr[2];
  b2 = coefptr[3];

  x1 = varptr[1]; /* read input states into x1 and x2 */
  x2 = varptr[2];

  accum = varptr[0];            /* read output state y1 into accum */
  
  /* Amplify input signal by 1dB in order to better match Pro Licensing's reference plots */
  /* As an optimization this is "rolled in" via b0 coefficient */
  /* See also comprDmxCalc() */
  b0 = DLB_LsmacLLL(b0, b0, DLB_LcF(0.12201));

  for (j = 0; j < compr_blk_len; j++) {
    accum = DLB_LmpyLL(accum, a1);                            /* acc = -y1*a1 */
    accum = DLB_LmacLLL(accum, b0, iptr[j*sample_offset]);    /* acc += x0*b0 (with 1dB boost) */
    accum = DLB_LmacLLL(accum, b1, x1);                       /* acc += x1*b1 */
    accum = DLB_LmacLLL(accum, b2, x2);                       /* acc += x2*b2 */
    accum = DLB_LsshlLU(accum, 1);                            /* acc *=2, compensate for coeff scaling */

    x2 = x1;                     /* x2 = x1  */
    
    /* Apply same 1dB boost to filter delay */ 
    x1 = DLB_LsmacLLL(iptr[j*sample_offset], iptr[j*sample_offset], DLB_LcF(0.12201));  /* x1 = x0  */

    optr[j] = accum;             /* y0 = acc */
  }

  /* update states */
  varptr[1] = x1;
  varptr[2] = x2;
  varptr[0] = accum;                /* y1 = y0 */
}


/* Table for DRC conversion into DD bitstream format */
static const DLB_LFRACT dB64Conv[32] =
{
  DLB_LcF(-101008904/2147483648.0), /* 32/64 linear in Q7.24 dB */
  DLB_LcF(-96524703/2147483648.0),  /* 33/64 linear in Q7.24 dB */
  DLB_LcF(-92174378/2147483648.0),  /* 34/64 linear in Q7.24 dB */
  DLB_LcF(-87950168/2147483648.0),  /* 35/64 linear in Q7.24 dB */
  DLB_LcF(-83844966/2147483648.0),  /* 36/64 linear in Q7.24 dB */
  DLB_LcF(-79852249/2147483648.0),  /* 37/64 linear in Q7.24 dB */
  DLB_LcF(-75966017/2147483648.0),  /* 38/64 linear in Q7.24 dB */
  DLB_LcF(-72180739/2147483648.0),  /* 39/64 linear in Q7.24 dB */
  DLB_LcF(-68491300/2147483648.0),  /* 40/64 linear in Q7.24 dB */
  DLB_LcF(-64892968/2147483648.0),  /* 41/64 linear in Q7.24 dB */
  DLB_LcF(-61381351/2147483648.0),  /* 42/64 linear in Q7.24 dB */
  DLB_LcF(-57952368/2147483648.0),  /* 43/64 linear in Q7.24 dB */
  DLB_LcF(-54602219/2147483648.0),  /* 44/64 linear in Q7.24 dB */
  DLB_LcF(-51327361/2147483648.0),  /* 45/64 linear in Q7.24 dB */
  DLB_LcF(-48124484/2147483648.0),  /* 46/64 linear in Q7.24 dB */
  DLB_LcF(-44990491/2147483648.0),  /* 47/64 linear in Q7.24 dB */
  DLB_LcF(-41922482/2147483648.0),  /* 48/64 linear in Q7.24 dB */
  DLB_LcF(-38917736/2147483648.0),  /* 49/64 linear in Q7.24 dB */
  DLB_LcF(-35973695/2147483648.0),  /* 50/64 linear in Q7.24 dB */
  DLB_LcF(-33087957/2147483648.0),  /* 51/64 linear in Q7.24 dB */
  DLB_LcF(-30258255/2147483648.0),  /* 52/64 linear in Q7.24 dB */
  DLB_LcF(-27482456/2147483648.0),  /* 53/64 linear in Q7.24 dB */
  DLB_LcF(-24758544/2147483648.0),  /* 54/64 linear in Q7.24 dB */
  DLB_LcF(-22084615/2147483648.0),  /* 55/64 linear in Q7.24 dB */
  DLB_LcF(-19458867/2147483648.0),  /* 56/64 linear in Q7.24 dB */
  DLB_LcF(-16879596/2147483648.0),  /* 57/64 linear in Q7.24 dB */
  DLB_LcF(-14345183/2147483648.0),  /* 58/64 linear in Q7.24 dB */
  DLB_LcF(-11854096/2147483648.0),  /* 59/64 linear in Q7.24 dB */
  DLB_LcF(-9404878/2147483648.0),   /* 60/64 linear in Q7.24 dB */
  DLB_LcF(-6996145/2147483648.0),   /* 61/64 linear in Q7.24 dB */
  DLB_LcF(-4626580/2147483648.0),   /* 62/64 linear in Q7.24 dB */
  DLB_LcF(-2294929/2147483648.0)    /* 63/64 linear in Q7.24 dB */
};


/*
  Documentation in compr.h
*/
uint16_t convCompressorGainToDD(const DLB_LFRACT x,
                                 const uint16_t bDRC
                                 )
{
  DLB_LFRACT remainder;
  int i, j, exponent;
  DLB_LFRACT diff;

  exponent = DLB_32srndL(DLB_LsmpyLL(x, ONE_OVER_SIX_DB)) >> 24;   /* divide by 6dB, convert to integer */
  if(bDRC){
    exponent = exponent > -4 ? exponent : -4;
    exponent = exponent <  3 ? exponent :  3;
  }
  else{
    exponent = exponent > -8 ? exponent : -8;
    exponent = exponent <  7 ? exponent :  7;
  }

  /* SIX_DB is in Q3.12 format */
  remainder = DLB_LsubLL(x, DLB_LshlLU(DLB_LsmpySS(DLB_S_16(exponent+1), SIX_DB), 24-16+3)); /* subtract the "6dB" exponent + 1 */

  /* find value closest to remainder, but not greater to prevent clipping */
  for(i=0; i<32;i++){
    diff = (DLB_LsubLL(dB64Conv[i], remainder));
    if(DLB_IltLL(DLB_L00, diff)){
      break;
    }
  }
  j = i>0 ? i-1 : 0;

  if(bDRC)
    return (((exponent<<5) & 0x000000E0) + (j & 0x0000001F));
  else{
    if((exponent == 0) && ((j>>1) == 0))
      return COMPR_UNITY;
    else
      return (((exponent<<4) & 0x000000F0) + ((j>>1) & 0x0000000F));
  }
}
