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
 * @defgroup - Metadata emulation library public definition
 * @{
 * The API code for metadata emulation.
 * @}
 */

#ifndef DLB_MD_EMUL_API_H
#define DLB_MD_EMUL_API_H

#include<dlb_intrinsics.h>

#define DLB_MD_EMUL_V_API  1    /**< @brief <API version.> */
#define DLB_MD_EMUL_V_FCT  0    /**< @brief <functional change.> */
#define DLB_MD_EMUL_V_MTNC 0    /**< @brief <maintenance release.> */
 
#define DLB_MD_EMUL_BLOCK_SIZE       256 
#define DLB_MD_EMUL_MAX_CHANS        8

#define DLB_MD_EMUL_MAX_OUTPUTS      2

#define DLB_MD_EMUL_MAX_CHAN_MODE    5

/**
 * @brief Version definition structure for component dlb_md_emul.
 *
 * Assigning the defined macro values.
 */

typedef struct dlb_md_emul_version_info_s
{
    int         v_api;     /**< API version. */
    int         v_fct;     /**< Functional change. */
    int         v_mtnc;    /**< Maintenance release. */
    const char *text;      /**< Optional, set to NULL if unused. */
} dlb_md_emul_version_info_t;

typedef struct dlb_md_emul_size_s
{
  uint32_t    emul_dynamic_mem_size;
  uint32_t    emul_static_mem_size;

  uint32_t    compr_dynamic_mem_size;
  uint32_t    compr_static_mem_size;
  uint32_t    compr_ext_static_mem_size;

} dlb_md_emul_size_t;


typedef struct dlb_md_emul_handler_s
{
  void*     p_emul_hdl;      /**< metadata emulator handler pointer           */
} dlb_md_emul_hdl_t;

typedef enum
{
    DLB_MD_EMUL_CHAN_LEFT    = 0  /* Index for the Left Channel */
   ,DLB_MD_EMUL_CHAN_RGHT    = 1  /* Index for the Right Channel */
   ,DLB_MD_EMUL_CHAN_CNTR    = 2  /* Index for the Center Channel */
   ,DLB_MD_EMUL_CHAN_LFE     = 3  /* Index for the LFE Channel */
   ,DLB_MD_EMUL_CHAN_LSUR    = 4  /* Index for the Left Surround Channel */
   ,DLB_MD_EMUL_CHAN_RSUR    = 5  /* Index for the Right Surround Channel */
   ,DLB_MD_EMUL_CHAN_LBAK    = 6  /* Index for the Left Back Channel */
   ,DLB_MD_EMUL_CHAN_RBAK    = 7  /* Index for the Right Back Channel */
   ,DLB_MD_EMUL_CHAN_MSUR    = 3  /* Index for the Mono surround channel Channel */
   ,DLB_MD_EMUL_CHAN_NONE    = -1 /* Channel not in use */
} DLB_MD_EMUL_CHANNEL_MAP;

typedef enum 
{
  DLB_MD_EMUL_COMPR_NO_COMPRESSION     = 0 /*!< no compression */
 ,DLB_MD_EMUL_COMPR_FILM_STANDARD      = 1 /*!< film standard compression */
 ,DLB_MD_EMUL_COMPR_FILM_LIGHT         = 2 /*!< film light compression */
 ,DLB_MD_EMUL_COMPR_MUSIC_STANDARD     = 3 /*!< music standard compression */
 ,DLB_MD_EMUL_COMPR_MUSIC_LIGHT        = 4 /*!< music light compression */
 ,DLB_MD_EMUL_COMPR_SPEECH_COMPRESSION = 5 /*!< speech compression */
} DLB_MD_EMUL_COMPRESSION_PROFILE;


typedef enum 
{
    DLB_MD_EMUL_CM_NONE        = 0   /* No dialog normalization */
   ,DLB_MD_EMUL_CM_DIALNORM    = 1   /* Dialog normalization only */
   ,DLB_MD_EMUL_CM_CUSTOM      = 2   /* Custom mode */
   ,DLB_MD_EMUL_CM_LINE        = 3   /* Line mode */
   ,DLB_MD_EMUL_CM_RF          = 4   /* RF mode */
} DLB_MD_EMUL_COMPRESSION_MODE;

typedef enum
{  /**< front_surround_lfe   */
    DLB_MD_EMUL_CHMOD_1_0_0 = 0  /**<   C                     */
   ,DLB_MD_EMUL_CHMOD_2_0_0 = 1  /**< L   R                   */
   ,DLB_MD_EMUL_CHMOD_3_1_0 = 2  /**< L C R S                 */
   ,DLB_MD_EMUL_CHMOD_3_2_1 = 3  /**< L C R Ls Rs LFE         */
   ,DLB_MD_EMUL_CHMOD_3_4_1 = 4  /**< L C R Ls Rs LFE Lrs Rrs */
} DLB_MD_EMUL_CHANNEL_MODE;

typedef enum 
{
    DLB_MD_EMUL_CONTROL_DISABLE_ALL     = 0 /* Disable all     */
   ,DLB_MD_EMUL_CONTROL_ENCODER_ENABLE  = 1 /* Encoder         */
   ,DLB_MD_EMUL_CONTROL_DECODER_ENABLE  = 2 /* Decoder         */
   ,DLB_MD_EMUL_CONTROL_DRC_CALC_ENABLE = 4 /* DRC calculation */
} DLB_MD_EMUL_PROCESS_CONTROL_FLAGS;


typedef struct dlb_md_emul_process_config_s
{
    DLB_LFRACT                 *pa_in_data[DLB_MD_EMUL_MAX_OUTPUTS];

    DLB_MD_EMUL_CHANNEL_MAP     a_chan_map[DLB_MD_EMUL_MAX_CHANS];
    DLB_MD_EMUL_CHANNEL_MODE    channel_mode;
    DLB_MD_EMUL_CHANNEL_MODE    dolbye_channel_mode; /* Chan mode as indicated by program config */

    uint32_t                    sample_offset;
    uint32_t                    num_samples;
    uint32_t                    sample_rate;
    uint32_t                    lfe_on;
    
    uint32_t                    control;

    uint32_t                    use_bitstream_gainwords[DLB_MD_EMUL_MAX_OUTPUTS];

    /* Compression parameters */
    DLB_MD_EMUL_COMPRESSION_PROFILE  comp_profile; 
    DLB_MD_EMUL_COMPRESSION_PROFILE  drc_profile;  
    DLB_MD_EMUL_COMPRESSION_MODE     comp_mode[DLB_MD_EMUL_MAX_OUTPUTS];
    
    DLB_LFRACT                    custom_boost[DLB_MD_EMUL_MAX_OUTPUTS];
    DLB_LFRACT                    custom_cut[DLB_MD_EMUL_MAX_OUTPUTS];
    uint32_t                      dialnorm;
    uint32_t                      compr_dd;           /* compr val from DD bitstream */
    uint32_t                      dynrng_dd;          /* dynrng val from DD bitstream */

    /* Encoder parameters */
    uint32_t                      sur90on;            /* 90 Phase-shift of surrounds */
    uint32_t                      suratton;           /* Attenute surrounds by 3dB */
    uint32_t                      hpfon;              /* High-pass filter */
    uint32_t                      bwlpfon;            /* Low-pass filter */
    uint32_t                      lfelpfon;           /* Low-pass filter of LFE */
   
}dlb_md_emul_process_config_t;

#ifdef __cplusplus
extern "C" {
#endif
 
/**
 * @brief Get component dlb_md_emul version.
 *
 * @return Version definition structure.
 */
const dlb_md_emul_version_info_t *
dlb_md_emul_get_version(void);

/*
 * Query the latency of the library
 */
uint32_t
dlb_md_emul_query_latency( dlb_md_emul_process_config_t  *p_md_emul_conf);


/*
 * Query the memory size required by the open call
 */
int32_t
dlb_md_emul_query_mem
    (
     dlb_md_emul_size_t *p_md_emul_size        /**< [in/out] required 'dynamic' memory size */
    );

/*
 * Open emulator
 */
int32_t
dlb_md_emul_open
    (
     dlb_md_emul_size_t *p_dlb_md_emul_size     /**< [in] dlb_md_emul handler pointer            */
    ,dlb_md_emul_hdl_t  *p_dlb_md_emul_hdl      /**< [in/out] dlb_md_emul handler pointer            */
    ,void               *p_static_mem           /**< [in] pointer to externally allocated memory */
    ,void               *p_dynamic_mem          /**< [in] pointer to externally allocated memory */
    );

/*
 * Close emulator and return memory block
 */
int32_t
dlb_md_emul_close
    (
     dlb_md_emul_hdl_t    *p_dlb_md_emul_hdl   /**< [in/out] pointer to metadata emulation handler */
    );

/*
 * Perform the metadata encode emulation
 */
int32_t
dlb_md_emul_process
    (
     dlb_md_emul_hdl_t*               /**< [in/out] pointer to metadata emulation handler */
    ,dlb_md_emul_process_config_t*    /**< [in/out] pointer to metadata emulation process control structure */
    ,int                              /**< [in] number of outputs for independent DRC & dialnorm application */
    );

#ifdef __cplusplus
}
#endif

#endif /* DLB_MD_EMUL_API_H */
