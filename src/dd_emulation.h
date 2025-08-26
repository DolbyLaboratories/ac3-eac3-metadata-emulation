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
 * Main DD emulation header
 * @}
 */


#ifndef _DD_EMULATION_H
#define _DD_EMULATION_H


#define DD_EMU_MIN_BLOCKS   1
#define DD_EMU_MAX_BLOCKS   8
#define DD_EMU_MAX_BLOCK_SIZE   256
#define DD_EMU_MIN_BLOCK_SIZE   32

#define DD_EMU_MAX_CHANS    8

#define DD_EMU_MAX_OUTPUTS  2

#define DD_EMU_COMPR_BUFFER_SIZE 256

enum
{
    MASTER_BUF,  // buffer for encoder emulation, is copied to AUX buf for decoder emulation
    AUX_BUF      
};

typedef enum
{
    DD_EMU_STATUS_OK,
    DD_EMU_STATUS_INVALID_HANDLE    = -1,
    DD_EMU_STATUS_MEM_ALLOC_ERR     = -2,
    DD_EMU_STATUS_INVALID_PARAM_ERR = -3,
    DD_EMU_STATUS_NOT_ENOUGH_DATA   = -4,
    DD_EMU_STATUS_EMULATION_ERROR   = -5 

} DD_EMU_STATUS;

typedef enum
{
    DD_EMU_CHAN_LEFT    = 0,          /* Index for the Left Channel */
    DD_EMU_CHAN_RGHT    = 1,          /* Index for the Right Channel */
    DD_EMU_CHAN_CNTR    = 2,          /* Index for the Center Channel */
    DD_EMU_CHAN_LFE     = 3,          /* Index for the LFE Channel */
    DD_EMU_CHAN_LSUR    = 4,          /* Index for the Left Surround Channel */
    DD_EMU_CHAN_RSUR    = 5,          /* Index for the Right Surround Channel */
    DD_EMU_CHAN_LBAK    = 6,          /* Index for the Left Back Channel */
    DD_EMU_CHAN_RBAK    = 7,          /* Index for the Right Back Channel */
    DD_EMU_CHAN_MSUR    = 3,          /* Index for the Mono surround channel Channel */
    DD_EMU_CHAN_NONE    = -1          /* Channel not in use */
} DD_EMU_CHAN_MAP;


typedef enum
{
    DD_EMU_CHMODE_DUALCHANNEL    = 0  /* 1+1, 2 independent mono channels, not allowed! */
   ,DD_EMU_CHMODE_MONO           = 1  /* 1_0, 1 front channel */
   ,DD_EMU_CHMODE_STEREO         = 2  /* 2_0, 2 front channels */
   ,DD_EMU_CHMODE_3_0            = 3  /* 3_0, 3 front channels (L,C,R), 1 back channel */
   ,DD_EMU_CHMODE_2_1            = 4  /* 2_1, 2 front channels, 1 back channel */
   ,DD_EMU_CHMODE_3_1            = 6  /* 3_1, 3 front channels (L,C,R), 1 back channel */
   ,DD_EMU_CHMODE_2_2            = 5  /* 2_2, 2 front channels, 2 back channels */
   ,DD_EMU_CHMODE_3_2            = 7  /* 3_2, 3 front channels (L,C,R), 2 back channels */
   ,DD_EMU_CHMODE_3_3            = 8  /* 3_3, 3 front channels (L,C,R), 3 back channels */
   ,DD_EMU_CHMODE_3_4            = 9  /* 3_4, 3 front channels (L,C,R), 4 back channels */
   ,DD_EMU_CHMOD_LAST            = 10  /* Not allowed! Indicates number of existing channel modes */
} DD_EMU_CHANNEL_MODE;

typedef enum
{
     DD_EMU_CM_NONE       = 0  /* No dialog normalization */
    ,DD_EMUL_CM_DIALNORM  = 1  /* Dialog normalization only */
    ,DD_EMU_CM_CUSTOM     = 2  /* Custom mode */
    ,DD_EMU_CM_LINE       = 3  /* Line mode */
    ,DD_EMU_CM_RF         = 4  /* RF mode */
} DD_EMU_COMPRESSION_MODE;


typedef enum
{
  DD_EMU_CONTROL_DISABLE_ALL     = 0 /* Disable all     */
 ,DD_EMU_CONTROL_ENCODER_ENABLE  = 1 /* Encoder         */
 ,DD_EMU_CONTROL_DECODER_ENABLE  = 2 /* Decoder         */
 ,DD_EMU_CONTROL_DRC_CALC_ENABLE = 4 /* DRC calculation */
} DD_EMU_PROCESS_CONTROL_FLAGS;

typedef enum
{
  DD_EMU_COMPR_NO_COMPRESSION     = 0  /*!< no compression */
 ,DD_EMU_COMPR_FILM_STANDARD      = 1  /*!< film standard compression */
 ,DD_EMU_COMPR_FILM_LIGHT         = 2  /*!< film light compression */
 ,DD_EMU_COMPR_MUSIC_STANDARD     = 3  /*!< music standard compression */
 ,DD_EMU_COMPR_MUSIC_LIGHT        = 4  /*!< music light compression */
 ,DD_EMU_COMPR_SPEECH_COMPRESSION = 5  /*!< speech compression */
} DD_EMU_COMPRESSION_PROFILE_TYPE;

typedef struct
{
    DLB_LFRACT                 *pa_app_data[DD_EMU_MAX_OUTPUTS];
    DD_EMU_CHAN_MAP             a_chan_map[DD_EMU_MAX_CHANS];
    int                         emu_blk_size;
    int                         sample_offset;
    int                         num_samples;
    int                         sample_rate;
    DD_EMU_CHANNEL_MODE         channel_mode;
    DD_EMU_CHANNEL_MODE         dolbye_channel_mode;
    int                         lfe_on;

    unsigned int                control;

    /* Compression parameters */
    DD_EMU_COMPRESSION_MODE          comp_mode[DD_EMU_MAX_OUTPUTS];
    DD_EMU_COMPRESSION_PROFILE_TYPE  comp_profile;
    DD_EMU_COMPRESSION_PROFILE_TYPE  drc_profile;

    uint32_t                    use_bitstream_gainwords[DD_EMU_MAX_OUTPUTS];

    DLB_LFRACT                  custom_boost[DD_EMU_MAX_OUTPUTS];
    DLB_LFRACT                  custom_cut[DD_EMU_MAX_OUTPUTS];
    int                         dialnorm;
    int                         compr_dd;           /* compr val from DD bitstream */
    int                         dynrng_dd;          /* dynrng val from DD bitstream */

    /* Encoder parameters */
    int                         sur90on;            /* 90 Phase-shift of surrounds */
    int                         suratton;           /* Attenute surrounds by 3dB */
    int                         hpfon;              /* High-pass filter */
    int                         bwlpfon;            /* Low-pass filter */
    int                         lfelpfon;           /* Low-pass filter of LFE */

} dd_emu_process_config;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Query the memory size required by the open call
 */
int32_t
dd_emulation_get_required_mem_size
    (
      uint32_t   *p_emul_static_mem_size
     ,uint32_t   *p_emul_dynamic_mem_size
     ,uint32_t   *p_compr_static_mem_size
     ,uint32_t   *p_compr_dynamic_mem_size
     ,uint32_t   *p_compr_ext_static_mem_size
     ,uint32_t    emu_blk_size
    );
/*
 * Open emulator
 */

int32_t dd_emulation_open
    (
     void *p_static_mem
    ,void *p_dynamic_mem
    ,uint32_t emul_static_mem_size
    ,uint32_t emul_dynamic_mem_size
    ,uint32_t compr_static_mem_size
    ,uint32_t compr_dynamic_mem_size
    ,uint32_t compr_ext_static_mem_size
    ,uint32_t emul_blk_size
    );
/*
 * Reset emulator
 */
int32_t dd_emulation_reset(void *const p_dd_emul_hdl, uint32_t emu_blk_size);

/*
 * Close emulator and return memory block
 */
int32_t  dd_emulation_close(void *p_dd_emul_hdl);

/*
 * Perform the DD encode emulation
 *
 * Perform filtering based on enc_params
 * Compute DRC gains
 */
DD_EMU_STATUS 
dd_emulation_process
    (
     void                  *p_dd_emul_hdl
    ,dd_emu_process_config *p_buf_config
    ,int                    num_outputs
    );

#ifdef __cplusplus
}
#endif

#endif
