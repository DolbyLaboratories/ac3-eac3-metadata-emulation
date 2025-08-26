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
 * @brief  API Prototypes - DRC and Compr Metadata Generation for Audio Compression
 *
 * @addtogroup dlb_dsplib_compr
 * The dynamic range compressor module calculates the DRC and COMPR gains according
 * to the provided compressor profile.
 *
 * @section intro Introduction
 *
 * The dynamic range compressor module calculates the DRC and COMPR gains according
 * to the provided compressor profile. It also includes clipping protection for different
 * downmix types like LoRo, LtRt and PLII. The interface description can be found in
 * compr.h
 * @section example Calling sequence
 *
 * comprtest.c shows how to use the API of the compressor library.
 * The calling sequence can be summerized in the following pseudo code snippet:
 *
 * @verbatim
 *   ; get the required buffer sizes
 *   ComprGetRequiredBufferSize();
 *
 *   ; allocate memory for the required buffers
 *
 *   ; pass allocated memory buffers to the compressor library
 *   ComprOpen();
 *
 *   while (1) {
 *     ; get next samples
 *     ComprProcess()
 *    }
 * @endverbatim
 *
 * @section MEM Data-RAM and Data-ROM
 *
 * Data-RAM is separated into
 *
 * @li intern dynamic data: should be located in the processors internal RAM for performance reasons and
 *                          could be reused in other sections of the code (non-persistent)
 * @li intern static data: should be located in the processors internal RAM for performance reasons and
 *                          must not be overwritten in other sections of the code (persistent)
 * @li extern static data: must not be overwritten in other sections of the code (persistent)
 *
 * @htmlonly
 * <TABLE border=1 cellspacing=3 cellpadding=3 align="center">
 *   <caption align=bottom>Memory requirements for one 5.1 instance</caption>
 *   <TR><TD></TD><TD>Data-RAM   </TD><TD>
 *   <TR><TD>intern dynamic RAM</TD><TD>  3136 Bytes  </TD><TD>
 *   <TR><TD>intern static RAM</TD><TD>   96 Bytes  </TD><TD>
 *   <TR><TD>extern static RAM</TD><TD>  264 Bytes  </TD><TD>
 * </TABLE>
 *  @endhtmlonly
 *
 * @{
 */

#ifndef _DLB_COMPR_H_
#define _DLB_COMPR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dlb_dsplib/blk.h"          /* includes vec_inlines (#defines +backend, header files) */
#include "dlb_dsplib/math.h"         /* includes math inlines (#defines +backend, header files) */

#ifndef COMPR_BLOCK_LEN
#define COMPR_BLOCK_LEN       32                       /*!< # of samples per compressor time block */
#endif
#define COMPR_MAX_CHANNELS      8                       /*!< # of possible channels */

/*!
  \name ORable flags for activeDmxBitmask
*/
/*@{*/
#define COMPR_DMX_LORO_CUSTOM_ACTIVE   1 /*!< Downmix LoRo Custom Active Flag */
#define COMPR_DMX_LTRT_DEFAULT_ACTIVE  2 /*!< Downmix LoRo Default Active Flag */
#define COMPR_DMX_LTRT_CUSTOM_ACTIVE   4 /*!< Downmix LtRt Custom Active Flag */
#define COMPR_DMX_PLII_DEFAULT_ACTIVE  8 /*!< Downmix PlII Default Active Flag */
#define COMPR_DMX_ITU_ACTIVE          16 /*!< Downmix Itu Active Flag */
/*@}*/

/*!
  \name Constants for Dolby Digital metadata values
*/
/*@{*/
#define COMPR_UNITY    (0xFF)   /*!< Compr value indicating close to zero compression. Avoid COMPR=0 (problematic for some decoders), 0xFF => -0.28dB. */
#define DYNRNG_UNITY   (0x00)   /*!< Dynrange value indicating no compression. */
#define DIALNORM_UNITY   (31)   /*!< Dialnorm value indicating no level adjustment in the Dolby Digital decoder. */
#define MAX_DIALNORM     (31)   /*!< Highest allowed DD dialnorm value. Corresponds to the lowest indicated loudness. */
/*@}*/

/**
 * PCM Type.
 */
typedef DLB_LFRACT PCM_TYPE;

/*!
  \name Compressor Return Values
*/
/*@{*/
enum
{
  COMPR_OK = 0,                 /*!< no error */
  COMPR_INVALID_BLOCK_NUMBER,   /*!< allowed number of blocks is 4, 6, 8, which correspond to frame sizes of 1024, 1536 and 2048 */
  COMPR_INVALID_SAMPLE_RATE,    /*!< allowed sample rates are 32000, 44100, 48000 */
  COMPR_INVALID_CHANNEL_MODE,   /*!< allowed channel modes are 1/0, 2/0, 3/0, 2/1, 3/1, 2/2 3/2, 3/3, 3/4, 1+1 shall be handled with two 1/0 instances of the compressor */
  COMPR_INVALID_COMPR_PROFILE,  /*!< invalid compressor profile */
  COMPR_INVALID_PTR             /*!< zero pointer given as arguments which was not expected */
};
/*@}*/


/*!
  \brief Compression characteristics for line mode and RF mode compression

  Further information about the compression profiles are available in the "DD Professional Encoding Guidelines"
  and the whitepaper "All about Metadata". Both can be downloaded on www.dolby.com
*/
typedef enum {
  COMPR_NO_COMPRESSION = 0,    /*!< no compression */
  COMPR_FILM_STANDARD = 1,     /*!< film standard compression */
  COMPR_FILM_LIGHT = 2,        /*!< film light compression */
  COMPR_MUSIC_STANDARD = 3,    /*!< music standard compression */
  COMPR_MUSIC_LIGHT = 4,       /*!< music light compression */
  COMPR_SPEECH_COMPRESSION = 5 /*!< speech compression */
} COMPR_PROFILE_TYPE;


/*!
  \brief compressor channel modes

  The channel mode is set in the open call and cannot be switched at runtime.
  To switch the channel mode the compressor instance must be re-opened

  Please note that Dual Channel mode is rejected as this mode shall be handled
  by two seperate mono compressor instances. The LFE channel is signaled in an seperate
  function parameter.
*/
typedef enum {
  COMPR_CHMODE_DUALCHANNEL        = 0,  /*!< 1+1, 2 independent mono channels, not allowed! */
  COMPR_CHMODE_MONO               = 1,  /*!< 1_0, 1 front channel */
  COMPR_CHMODE_STEREO             = 2,  /*!< 2_0, 2 front channels */
  COMPR_CHMODE_3_0                = 3,  /*!< 3_0, 3 front channels (L,C,R), 1 back channel */
  COMPR_CHMODE_2_1                = 4,  /*!< 2_1, 2 front channels, 1 back channel */
  COMPR_CHMODE_3_1                = 6,  /*!< 3_1, 3 front channels (L,C,R), 1 back channel */
  COMPR_CHMODE_2_2                = 5,  /*!< 2_2, 2 front channels, 2 back channels */
  COMPR_CHMODE_3_2                = 7,  /*!< 3_2, 3 front channels (L,C,R), 2 back channels */
  COMPR_CHMODE_3_3                = 8,  /*!< 3_3, 3 front channels (L,C,R), 3 back channels */
  COMPR_CHMODE_3_4                = 9   /*!< 3_4, 3 front channels (L,C,R), 4 back channels */
} COMPR_CHMODE;



/*!
  \brief Custom downmix coefficients in Q1.30 linear format, not dB values (e.g. 0dB is DLB_LcF(0.5))
*/
typedef struct {
  DLB_LFRACT globalGain;          /*!< Gain applied to all channels (default 0dB), if centerLevel is greater -3dB, it normalizes the center level back to -3dB */
  DLB_LFRACT centerLevel;         /*!< Gain applied to the center channel before adding it to the left and right channel, default -3dB */
  DLB_LFRACT surroundLevel;       /*!< Gain applied to the left and right surround channels before mixing it into front left and right (def. -3dB) */
} DMX_COEFS, *HANDLE_DMX_COEFS /*! Handle to Downmix coefficients */;

/*
  \brief Internal downmix variables
*/
typedef struct {
  uint16_t bitMaskVal;
  uint16_t bPhaseShiftLeftCh;
  DLB_LFRACT centerLevel;
  DLB_LFRACT surroundLevelOwnSide;
  DLB_LFRACT surroundLevelOppositeSide;
  DLB_LFRACT globalGain;
  DLB_LFRACT maxVal;
} DMX, *HANDLE_DMX /*! Handle to the internal downmix structure */;


/*
  \brief Handle containing all internal variables
*/
typedef struct COMPR *HANDLE_COMPR;


/*!
  \name Interface Functions
*/
/*@{*/

/*!
  \brief Get memory requirements for compressor instance

  \return COMPR_OK if successful
*/
  int16_t md_ComprGetRequiredBufferSize(COMPR_CHMODE cm,             /*!< IN Channel mode */
                                      uint16_t numBlocksPerFrame,    /*!< IN Number of blocks (each 256 samples), which form one frame */
                                      uint32_t *internStaticSize,    /*!< OUT Pointer to size of internal static memory */
                                      uint32_t *internDynamicSize,   /*!< OUT Pointer to size of internal dynamic memory */
                                      uint32_t *externStaticSize,    /*!< OUT Pointer to size of external static memory */
                                      uint32_t compr_blk_len );

/*!
  \brief Initialize the memory for one compressor instance, maps internal buffers to the static and dynamic memory pointers

  \return Handle to one compressor instance, zero pointer on error
*/

  HANDLE_COMPR md_ComprOpen( void * pInternStatic,          /*!< IN Pointer to static internal memory */
                          void * pInternDynamic,            /*!< IN Pointer to dynamic internal memory */
                          void * pExternStatic,             /*!< IN Pointer to static external memory */
                          COMPR_CHMODE cm,                  /*!< IN Channel mode, see comprChanTab for channel ordering */
                          int16_t bLfeOn,                   /*!< IN Indicates if LFE is on or off */
                          uint16_t numBlocksPerFrame,       /*!< IN Number of blocks (each 256 samples), which form one frame */
                          uint32_t fs,                      /*!< IN Sampling frequency [Hz], 32000, 44100 and 48000Hz are allowed */
                          uint32_t compr_blk_len );         /*!< IN Size of native processing buffer */

/*!
  \brief Calculates the clipping protection and the DRC and compr gains

  \return COMPR_OK if successful
*/
  int16_t md_ComprProcess(HANDLE_COMPR hCompr,              /*!< IN/OUT Handle to one compressor instance */
                            DLB_LFRACT prl,                  /*!< IN Program reference level in Q7.24 [dB] format, will be limited to [-31, 0]dB */
                            COMPR_PROFILE_TYPE profileDRC,   /*!< IN Compressor profile for DRC calculation */
                            COMPR_PROFILE_TYPE profileCompr, /*!< IN Compressor profile for compr calculation */
                            PCM_TYPE *ppPcmInput,           /*!< IN PCM input (pcmIn[numChannels][frame length]) */
                            uint16_t activeDmxBitmask,       /*!< IN Bitmask indicating which downmix types are activated */
                            HANDLE_DMX_COEFS LoRoCoeffs,     /*!< IN Custom coefficients for LoRo downmix, 0 indicates using default values */
                            HANDLE_DMX_COEFS LtRtCoeffs,     /*!< IN Custom coefficients for LtRt downmix, 0 indicates using default values */
                            DLB_LFRACT *gainDRC,             /*!< OUT DRC gain for each block in Q7.24 [dB] format*/
                            DLB_LFRACT *gainCompr,           /*!< OUT Compr gain in Q7.24 [dB] format */
                            uint32_t compr_blk_len,          /*!< IN Size of native processing buffer */
                            uint32_t sample_offset );        /*!< IN Stride of PCM input buffer */


/*!
  \brief Converts Q7.24 dB gains to DD bitstream format for dynrange and compr

  \return DD bitstream formated value

  When the compressor is used in conjuction with a Dolby Digital encoder,
  this function allows the conversion of the compressor results to the
  Dolby Digital bitstream format.
  The DD bitstream format specification can be found in ETSI TS 102 366.
  Both dynrng and compr are encoded with 8 bits.
  Dynrng has 3 bits (denoted as X) representing shifts of the linear gain from 3 to -4.
  The 5 LSBs represent  0.1YYYYY (32..63 / 64) linear gain.
  The resulting linear gain is (0.1YYYYY) << (XXX + 1)
  Compr has 4 bits (denoted as X) representing shifts of the linear gain from 7 to -8.
  The 4 LSBs represent  0.1YYYY (16..31 / 32) linear gain.
  The resulting linear gain is (0.1YYYY) << (XXXX + 1)

*/
  uint16_t convCompressorGainToDD(const DLB_LFRACT x,   /*!< Q7.24 dB gain as the Compressor produces */
                                   const uint16_t bDRC  /*!< Indicates if dynrng or compr format is returned */
                                   );
/*@}*/


#ifdef __cplusplus
}
#endif

#endif /* _DLB_COMPR_H_ */
/*@}*/
