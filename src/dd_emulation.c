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
 * Top level DD emulation code
 * @}
 */

#include "dlb_intrinsics.h"
#include "md_compr.h"
#include "dd_emulation.h"
#include "drc_applier.h"
#include "emul_filters.h"
#include <string.h> /* for memset */

typedef struct
{
    /* Compressor handle */
    HANDLE_COMPR compr_handle;

    /* Compressor memory */
    uint32_t* comp_static_internal;
    uint32_t* comp_dynamic_internal;
    uint32_t* comp_static_external;

    COMPR_CHMODE channel_mode;
    uint16_t lfe_on;
    int      emu_blk_size;
    uint16_t sample_rate;
    uint16_t num_blocks;

    /* Gain window */
    DLB_LFRACT last_gain[DD_EMU_MAX_OUTPUTS];

    /* Filter states */
    DLB_LFRACT hpf_history[DD_EMU_MAX_CHANS];
    DLB_LFRACT lpf_history[DD_EMU_MAX_CHANS][BWLIMORDER * BQHISTORY];
    DLB_LFRACT lfe_history[DD_EMU_MAX_CHANS][LFEORDER * BQHISTORY];
    DLB_LFRACT psf_history[DD_EMU_MAX_CHANS][MPHSTAGES * BQCOEFFS];
    DLB_LFRACT psf_surr_history[DD_EMU_MAX_CHANS][SPHSTAGES * BQCOEFFS];

} dd_emu_internal_data;

/* Mapping channel mode -> channel count, no LFE included */
static const int channel_number[DD_EMU_CHMOD_LAST] = {2, 1, 2, 3, 3, 4, 4, 5, 6, 7};

/* Mapping channel mode -> channel count, LFE included */
static const int channel_number_lfe[DD_EMU_CHMOD_LAST] = {2, 1, 2, 3, 3, 4, 4, 6, 7, 8};


/*
 * Private Functions
 */
static void initialize_filters(dd_emu_internal_data* p_dd_emul_data);
static 
void 
clear_channels
    (dd_emu_internal_data   *p_dd_emul_data
    ,dd_emu_process_config  *p_buf_config
    );

/* encoder */
static 
void 
encoder_emulation
     (dd_emu_internal_data  *p_dd_emul_data
     ,dd_emu_process_config *p_buf_config
     );
/* decoder */
static
int
decoder_emulation
     (dd_emu_internal_data  *p_dd_emul_data
     ,dd_emu_process_config *p_buf_config
     ,int                    num_outputs
     );

#if !defined(DLB_UT)
static
#endif
void
attenuate_surrounds
    (DLB_LFRACT *pcmptr
    ,int16_t     sample_offset
    ,int16_t     chan
    ,int16_t     acmod
    ,int         emu_blk_size
    );



/*
 * Public Functions
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
    )
{
    int32_t err;

    /* get the required memory size for the 'worst' case */
    err = md_ComprGetRequiredBufferSize(COMPR_CHMODE_3_4
                                       ,DD_EMU_MAX_BLOCKS
                                       ,p_compr_static_mem_size
                                       ,p_compr_dynamic_mem_size
                                       ,p_compr_ext_static_mem_size
                                       ,DD_EMU_COMPR_BUFFER_SIZE
                                       );

    *p_emul_static_mem_size  = sizeof(dd_emu_internal_data);
    *p_emul_dynamic_mem_size  = 0;

    return err;
}

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
    )
{
    int32_t err = DD_EMU_STATUS_OK;

    dd_emu_internal_data* p_dd_emul_data;

    if(!p_static_mem || !p_dynamic_mem)
    {
        return DD_EMU_STATUS_MEM_ALLOC_ERR;
    }

    p_dd_emul_data = (dd_emu_internal_data*)p_static_mem;

   /*initialize drc pointers to externally allocated space */
/*    p_dd_emul_data->comp_static_internal  = (uint32_t*)((uint32_t)p_dd_emul_data + emul_static_mem_size);
    p_dd_emul_data->comp_static_external  = (uint32_t*)((uint32_t)(p_dd_emul_data->comp_static_internal) + compr_static_mem_size); */

    p_dd_emul_data->comp_static_internal  = (uint32_t*)((uint8_t *)p_dd_emul_data + emul_static_mem_size);
    p_dd_emul_data->comp_static_external  = (uint32_t*)((uint8_t *)(p_dd_emul_data->comp_static_internal) + compr_static_mem_size);


    p_dd_emul_data->comp_dynamic_internal = (uint32_t*)(p_dynamic_mem);

    err = dd_emulation_reset
              (
               p_dd_emul_data
              ,emul_blk_size
              );
    return err;
}

int32_t dd_emulation_reset(void *const p_dd_emul_hdl, uint32_t emul_blk_size)
{
    dd_emu_internal_data* p_dd_emul_data;

    if(NULL == p_dd_emul_hdl)
    {
        return DD_EMU_STATUS_INVALID_HANDLE;
    }

    p_dd_emul_data = (dd_emu_internal_data*)p_dd_emul_hdl;

    p_dd_emul_data->channel_mode = COMPR_CHMODE_3_4;
    p_dd_emul_data->lfe_on       = 1;
    p_dd_emul_data->emu_blk_size = emul_blk_size;
    p_dd_emul_data->sample_rate  = 48000;
    p_dd_emul_data->num_blocks   = DD_EMU_MAX_BLOCKS;

    /* Initialize last gain */
    p_dd_emul_data->last_gain[0] = DLB_LcF(1.0/16.0);
    p_dd_emul_data->last_gain[1] = DLB_LcF(1.0/16.0);

    /* Initialize filters */
    initialize_filters(p_dd_emul_data);

    /* Initialize compression module (calculates DRC) */
    p_dd_emul_data->compr_handle = md_ComprOpen
            (p_dd_emul_data->comp_static_internal
            ,p_dd_emul_data->comp_dynamic_internal
            ,p_dd_emul_data->comp_static_external
            ,p_dd_emul_data->channel_mode
            ,p_dd_emul_data->lfe_on
            ,p_dd_emul_data->num_blocks
            ,p_dd_emul_data->sample_rate
            ,DD_EMU_COMPR_BUFFER_SIZE
            );
    if(p_dd_emul_data->compr_handle == NULL)
    {
       return DD_EMU_STATUS_INVALID_PARAM_ERR;
    }

    return DD_EMU_STATUS_OK;
}

int32_t dd_emulation_close(void *p_dd_emul_hdl)
{
    dd_emu_internal_data* p_dd_emul_data = (dd_emu_internal_data*)p_dd_emul_hdl;
    memset((void*)p_dd_emul_data, 0, sizeof(dd_emu_internal_data));

    return DD_EMU_STATUS_OK;
}

DD_EMU_STATUS 
dd_emulation_process
         (
          void                  *p_dd_emu_handle
         ,dd_emu_process_config *p_buf_config
         ,int                    num_outputs
         )
{
    dd_emu_internal_data* p_dd_emul_data;
    int num_blocks;
    DLB_LFRACT *p_main, *p_aux;
    int i, j;
    int num_samples;
    int num_chans;
    DD_EMU_STATUS ret = DD_EMU_STATUS_OK;

    if(NULL == p_dd_emu_handle)
    {
        return DD_EMU_STATUS_INVALID_HANDLE;
    }
    else if(!p_buf_config)
    {
        return DD_EMU_STATUS_INVALID_PARAM_ERR;
    }
    else if(p_buf_config->num_samples < p_buf_config->emu_blk_size)
    {
        return DD_EMU_STATUS_NOT_ENOUGH_DATA;
    }

    p_dd_emul_data = (dd_emu_internal_data*)p_dd_emu_handle;

    num_blocks = p_buf_config->num_samples / p_buf_config->emu_blk_size;

    /* Check to see if compr needs to be reinitialized */
    if(p_dd_emul_data->channel_mode != (COMPR_CHMODE)p_buf_config->channel_mode ||
            p_dd_emul_data->lfe_on != p_buf_config->lfe_on ||
            p_dd_emul_data->emu_blk_size != p_buf_config->emu_blk_size ||
            p_dd_emul_data->sample_rate != p_buf_config->sample_rate ||
            p_dd_emul_data->num_blocks != num_blocks)
    {
        p_dd_emul_data->channel_mode = (COMPR_CHMODE)p_buf_config->channel_mode;
        p_dd_emul_data->lfe_on = p_buf_config->lfe_on;
        p_dd_emul_data->emu_blk_size = p_buf_config->emu_blk_size;
        p_dd_emul_data->sample_rate = p_buf_config->sample_rate;
        p_dd_emul_data->num_blocks = num_blocks;

        p_dd_emul_data->compr_handle = md_ComprOpen
                                          (p_dd_emul_data->comp_static_internal
                                          ,p_dd_emul_data->comp_dynamic_internal
                                          ,p_dd_emul_data->comp_static_external
                                          ,p_dd_emul_data->channel_mode
                                          ,p_dd_emul_data->lfe_on
                                          ,p_dd_emul_data->num_blocks
                                          ,p_dd_emul_data->sample_rate
                                          ,DD_EMU_COMPR_BUFFER_SIZE
                                          );

        if(p_dd_emul_data->compr_handle == 0)
        {
            return DD_EMU_STATUS_INVALID_PARAM_ERR;
        }
    }

    if( p_buf_config->control & DD_EMU_CONTROL_DECODER_ENABLE || p_buf_config->control & DD_EMU_CONTROL_ENCODER_ENABLE)
    {
        /* Zero out channels that are not specified in channel config */
       clear_channels(p_dd_emul_data, p_buf_config);
    }

    if(p_buf_config->control & DD_EMU_CONTROL_ENCODER_ENABLE)
    {
        /* Run encoder emulation filters */
        encoder_emulation(p_dd_emul_data, p_buf_config);
    }

    /* Make a copy of the processed data for decoder emulation on secondary output */
    if (num_outputs == 2)
    {
        p_main = p_buf_config->pa_app_data[MASTER_BUF];
        p_aux = p_buf_config->pa_app_data[AUX_BUF];

        /* Determine number of channels in selected program */
        num_chans = channel_number_lfe[p_buf_config->channel_mode];

        num_samples = p_dd_emul_data->num_blocks * p_dd_emul_data->emu_blk_size;

        for(i = 0u; i < num_samples; i++)
        {
            for(j = 0u; j < num_chans; j++)
            {
                p_aux[j] = p_main[j];
            }

            p_aux += DD_EMU_MAX_CHANS;
            p_main += DD_EMU_MAX_CHANS;
        }
    }

    if( p_buf_config->control & DD_EMU_CONTROL_DECODER_ENABLE )
    {
        /* Run decoder emulation - DRC calculation and apply compression */
        ret = (DD_EMU_STATUS) decoder_emulation(p_dd_emul_data, p_buf_config, num_outputs);
    }

    return ret;
}

static 
int 
decoder_emulation
     (dd_emu_internal_data  *p_dd_emul_data
     ,dd_emu_process_config *p_buf_config
     ,int                    num_outputs
     )
{
    int drc = 0;
    int dialnorm;
    int16_t compr_status;
    int block;
    DLB_LFRACT* app_chan_ptrs[DD_EMU_MAX_CHANS];
    int chan, chan_num;
    int output;
    int i, j;
    int16_t perform_boost_cut = 0;

    /* Output gains */
    DLB_LFRACT gain_drc[DD_EMU_MAX_BLOCKS] = {0};
    DLB_LFRACT gain_compr = 0;

    /* Convert to Q7.24 dB format */
    DLB_LFRACT gain_dlnrm = DLB_L_32((-(int32_t)p_buf_config->dialnorm) << 24);

    /* Possible downmix types */
    uint16_t dmx_type_mask = COMPR_DMX_LORO_CUSTOM_ACTIVE | COMPR_DMX_LTRT_DEFAULT_ACTIVE |
            COMPR_DMX_LTRT_CUSTOM_ACTIVE | COMPR_DMX_PLII_DEFAULT_ACTIVE | COMPR_DMX_ITU_ACTIVE;

    if(p_buf_config->control & DD_EMU_CONTROL_DRC_CALC_ENABLE )
    {
        /* Compute DRC values from profile, and worst-case downmix */
        compr_status = md_ComprProcess(p_dd_emul_data->compr_handle
                                      ,gain_dlnrm
                                      ,(COMPR_PROFILE_TYPE)p_buf_config->drc_profile
                                      ,(COMPR_PROFILE_TYPE)p_buf_config->comp_profile
                                      ,p_buf_config->pa_app_data[0] 
                                      ,dmx_type_mask
                                      ,0
                                      ,0
                                      ,gain_drc
                                      ,&gain_compr
                                      ,DD_EMU_COMPR_BUFFER_SIZE
                                      ,p_buf_config->sample_offset);

        if(compr_status != COMPR_OK) 
        {
            return DD_EMU_STATUS_EMULATION_ERROR;
        }
    }

    /* Apply DRC/dialnorm independently for up to 2 outputs */
    for (output = 0; output < num_outputs; output++)
    {
        for(block = 0; block < p_dd_emul_data->num_blocks; block++)
        {
            perform_boost_cut = (p_buf_config->comp_mode[output] == DD_EMU_CM_CUSTOM);

            if(p_buf_config->control & DD_EMU_CONTROL_DRC_CALC_ENABLE && !p_buf_config->use_bitstream_gainwords[output] )
            {
                if(p_buf_config->comp_mode[output] == DD_EMU_CM_LINE || p_buf_config->comp_mode[output] == DD_EMU_CM_CUSTOM)
                {
                    drc = convCompressorGainToDD(gain_drc[block], 1);
                }
                else if(p_buf_config->comp_mode[output] == DD_EMU_CM_RF)
                {
                    drc = convCompressorGainToDD(gain_compr, 0);
                }
            }
            else
            {
                if(p_buf_config->comp_mode[output] == DD_EMU_CM_LINE || p_buf_config->comp_mode[output] == DD_EMU_CM_CUSTOM)
                {
                    drc = p_buf_config->dynrng_dd;
                }
                else if(p_buf_config->comp_mode[output] == DD_EMU_CM_RF)
                {
                    drc = p_buf_config->compr_dd;
                }
            }

            dialnorm = p_buf_config->dialnorm;

            if(p_buf_config->comp_mode[output] == DD_EMU_CM_NONE || p_buf_config->comp_mode[output] == DD_EMUL_CM_DIALNORM)
            {
                /* Bypass compression by setting drc to 0dB */
                drc = 255;
            }
            /* Also bypass compression if comp_mode indicates DRC but we're outputting on MAIN and
             * "no compression" is chosen for profile */
            if(output == 0 && p_buf_config->use_bitstream_gainwords[0] == 0 )
            {
                if(p_buf_config->comp_mode[0] == DD_EMU_CM_LINE || p_buf_config->comp_mode[0] == DD_EMU_CM_CUSTOM)
                {
                    if((COMPR_PROFILE_TYPE)p_buf_config->drc_profile == COMPR_NO_COMPRESSION)
                    {
                        drc = 255;
                        perform_boost_cut = 0;
                    }
                }
                else if(p_buf_config->comp_mode[0] == DD_EMU_CM_RF)
                {
                    if((COMPR_PROFILE_TYPE)p_buf_config->comp_profile == COMPR_NO_COMPRESSION)
                    {
                        drc = 255;
                    }
                }
            }

            if(p_buf_config->comp_mode[output] != DD_EMU_CM_NONE)
            {
                /* Determine number of full bandwidth channels */
                chan_num = channel_number[(int)(p_buf_config->channel_mode)]; 

                /* Add placeholder for LFE channel as needed (whether or not it is present) */
                if(p_buf_config->channel_mode == DD_EMU_CHMODE_3_2 || p_buf_config->channel_mode == DD_EMU_CHMODE_3_4)
                {
                    chan_num++;
                }

                for(chan = 0; chan < DD_EMU_MAX_CHANS; chan++)
                {
                    DD_EMU_CHAN_MAP channel = p_buf_config->a_chan_map[chan];

                    if (DD_EMU_CHAN_NONE == channel)
                    {
                        continue;
                    }

                    app_chan_ptrs[channel] = p_buf_config->pa_app_data[output] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan; // dbg - chan_map[chan]
                }

                /* Apply gain */
                apply_drc(p_buf_config->comp_mode[output] == DD_EMU_CM_RF
                         ,drc
                         ,&p_dd_emul_data->last_gain[output]
                         ,dialnorm
                         ,app_chan_ptrs
                         ,p_buf_config->emu_blk_size
                         ,chan_num
                         ,p_buf_config->sample_offset
                         ,p_buf_config->a_chan_map,
                         perform_boost_cut,
                         p_buf_config->custom_boost[output],
                         p_buf_config->custom_cut[output]
                         );
            }
        }
    }

    return DD_EMU_STATUS_OK;
}


/* New functions */

/* For unit testing this function can't be static */
#if !defined(DLB_UT)
static
#endif
void
attenuate_surrounds
    (DLB_LFRACT *pcmptr
    ,int16_t     sample_offset
    ,int16_t     chan
    ,int16_t     acmod
    ,int         emu_blk_size
    )
{
    int i;
    if(acmod >= DD_EMU_CHMODE_2_1 && (chan == DD_EMU_CHAN_LSUR || chan == DD_EMU_CHAN_RSUR))
    {
        for (i = 0; i < emu_blk_size; i++)
        {
            *pcmptr = DLB_LmpyLS(*pcmptr, DLB_ScF(0.707106781));  /*-3 dB (Scale Factor)*/
            pcmptr += sample_offset;
        }
    }
}


static void initialize_filters(dd_emu_internal_data* p_dd_emul_data)
{
   int i, j;

   /* Init the highpass filter state */
   for (i = 0; i < DD_EMU_MAX_CHANS; i++)
   {
       p_dd_emul_data->hpf_history[i] = 0;
   }

   /* Init the bandwidth-limiting lowpass filter state */
   for (i = 0; i < DD_EMU_MAX_CHANS; i++)
   {
       for (j = 0; j < BWLIMORDER * BQHISTORY; j++)
       {
           p_dd_emul_data->lpf_history[i][j] = 0;
       }

       for (j = 0; j < SPHSTAGES * BQCOEFFS; j++)
       {
           p_dd_emul_data->psf_surr_history[i][j] = 0;
       }
       for (j = 0; j < MPHSTAGES * BQCOEFFS; j++)
       {
           p_dd_emul_data->psf_history[i][j] = 0;
       }

       for (j = 0; j < LFEORDER * BQHISTORY; j++)
       {
           p_dd_emul_data->lfe_history[i][j] = 0;
       }

   }
}

/* Zero out channels that are not specified */
static 
void 
clear_channels
    (dd_emu_internal_data   *p_dd_emul_data
    ,dd_emu_process_config  *p_buf_config
    )
{
    int block, chan, num_chans;
    DD_EMU_CHAN_MAP channel;

    for(block = 0; block < p_dd_emul_data->num_blocks; block++)
    {
       /* Note this only clears unused channels within the active program.
          Any other programs will be undisturbed. */

       /* Determine how many channels are indicated by Dolby E metadata (derived from program cfg) */
       num_chans = channel_number[p_buf_config->dolbye_channel_mode];
       if(p_buf_config->dolbye_channel_mode == DD_EMU_CHMODE_3_2 || p_buf_config->dolbye_channel_mode == DD_EMU_CHMODE_3_4)
       {
          num_chans++;
       }

       for(chan = 0; chan < num_chans; chan++)
       {
            channel = p_buf_config->a_chan_map[chan];

            /* Clear channel if not indicated or if indicated as LFE and lfe_on is 0 */
            if (DD_EMU_CHAN_NONE == channel || (DD_EMU_CHAN_LFE == channel && !p_buf_config->lfe_on) )
            {
                emul_zero(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                          ,p_buf_config->sample_offset
                          ,p_buf_config->emu_blk_size
                          );
            }
        }
    }
}

static
void 
encoder_emulation
    (dd_emu_internal_data   *p_dd_emul_data
    ,dd_emu_process_config  *p_buf_config
    )
{
    int block, chan;
    DD_EMU_CHAN_MAP channel;

    for(block = 0; block < p_dd_emul_data->num_blocks; block++)
    {
        for(chan = 0; chan < DD_EMU_MAX_CHANS; chan++)
        {
            channel = p_buf_config->a_chan_map[chan];

            /* Skip channel if indicated as not present */
            if(channel == -1) continue;

            /* Attenuate surrounds */
            if (p_buf_config->suratton)
            {
                attenuate_surrounds(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                                   ,p_buf_config->sample_offset
                                   ,channel
                                   ,p_buf_config->channel_mode
                                   ,p_buf_config->emu_blk_size
                                   );
            }

            /* DC blocking (high-pass) filter */
            if (p_buf_config->hpfon)
            {
                emul_hpf(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                        ,p_buf_config->sample_offset
                        ,&p_dd_emul_data->hpf_history[chan]
                        ,p_buf_config->emu_blk_size
                        );
            }

            /* lowpass filter - both band-limiting and LFE */
            if (channel != DD_EMU_CHAN_LFE)
            {
                if (p_buf_config->bwlpfon) 
                {
                    emul_lpf_bwlimit(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                                    ,p_buf_config->sample_offset
                                    ,p_buf_config->channel_mode
                                    ,p_dd_emul_data->lpf_history[chan]
                                    ,p_buf_config->emu_blk_size
                                    );
                }
            }
            else
            {
                if (p_buf_config->lfelpfon)
                {
                    emul_lpf_lfe(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                                ,p_buf_config->sample_offset
                                ,p_dd_emul_data->lfe_history[chan]
                                ,p_buf_config->emu_blk_size
                                );
                }
            }

            /* 90-degree phase shift filter */
            if (p_buf_config->sur90on) 
            {
                if (channel == DD_EMU_CHAN_LSUR || channel == DD_EMU_CHAN_RSUR)
                {
                    emul_psf_surr(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                                 ,p_buf_config->sample_offset
                                 ,p_dd_emul_data->psf_surr_history[chan]
                                 ,p_buf_config->emu_blk_size
                                 );
                }
                else
                {
                    emul_psf_main(p_buf_config->pa_app_data[MASTER_BUF] + (block * p_buf_config->emu_blk_size * p_buf_config->sample_offset) + chan
                                 ,p_buf_config->sample_offset
                                 ,p_dd_emul_data->psf_history[chan]
                                 ,p_buf_config->emu_blk_size
                                 );
                }
            }
        }
    }
}
