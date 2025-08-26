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

#include<dlb_md_emul_api.h>
#include"dlb_md_emul_pvt.h"


static const uint32_t EMUL_BLK_SIZE = DLB_MD_EMUL_BLOCK_SIZE;

/** < API Version definition structure */
static const dlb_md_emul_version_info_t v =
{
    DLB_MD_EMUL_V_API
   ,DLB_MD_EMUL_V_FCT
   ,DLB_MD_EMUL_V_MTNC
   ,"Dolby Metadata Emulation"
};

 
/**
 * @brief Get component dlb_md_emul version.
 *
 * @return Version definintion structure.
 */
const dlb_md_emul_version_info_t *
dlb_md_emul_get_version(void)
{
    return &v;
}

/**
 *  @brief Query the latency of the library
 *  @return block size.
 */
uint32_t
dlb_md_emul_query_latency
    ( 
     dlb_md_emul_process_config_t  *p_md_emul_conf
    )
{
/* encoder latencies */
#define DLB_MD_EMUL_LATENCY_ENC_PH90    1
#define DLB_MD_EMUL_LATENCY_ENC_SURRAT  1
#define DLB_MD_EMUL_LATENCY_ENC_HPF     1
#define DLB_MD_EMUL_LATENCY_ENC_LPF     1
#define DLB_MD_EMUL_LATENCY_ENC_LFELPF  1

/* decoder latencies */
#define DLB_MD_EMUL_LATENCY_DEC_DRC_OFF 1
#define DLB_MD_EMUL_LATENCY_DEC_DRC_ON  1

   uint32_t latency = 0;

   if(p_md_emul_conf->control & DLB_MD_EMUL_CONTROL_ENCODER_ENABLE)
   {
        if(p_md_emul_conf->sur90on)
        {
            latency += DLB_MD_EMUL_LATENCY_ENC_PH90; 
        }
        if(p_md_emul_conf->suratton)
        {
            latency += DLB_MD_EMUL_LATENCY_ENC_SURRAT; 
        }

        if(p_md_emul_conf->hpfon)
        {
            latency += DLB_MD_EMUL_LATENCY_ENC_HPF; 
        }

        if(p_md_emul_conf->bwlpfon)
        {
            latency += DLB_MD_EMUL_LATENCY_ENC_LPF; 
        }

        if(p_md_emul_conf->lfelpfon)
        {
            latency += DLB_MD_EMUL_LATENCY_ENC_LFELPF; 
        }
   }

   if(p_md_emul_conf->control & DLB_MD_EMUL_CONTROL_DECODER_ENABLE)
   {
       if(p_md_emul_conf->control & DLB_MD_EMUL_CONTROL_DRC_CALC_ENABLE)
       {
            latency += DLB_MD_EMUL_LATENCY_DEC_DRC_ON; 
       }
       else
       {
            latency += DLB_MD_EMUL_LATENCY_DEC_DRC_OFF; 
       } 
   }


   latency += DLB_MD_EMUL_BLOCK_SIZE;

   return latency;
}

/**
 *  @brief Query the memory size required by the open call.
 *  @return Error code.
 */

int32_t
dlb_md_emul_query_mem
    (
     dlb_md_emul_size_t *p_md_emul_size        /**< [in/out] downmixer required 'dynamic' memory size */
    )
{
  int32_t err = 0;

  err = dd_emulation_get_required_mem_size
            (
             &p_md_emul_size->emul_static_mem_size
            ,&p_md_emul_size->emul_dynamic_mem_size
            ,&p_md_emul_size->compr_static_mem_size
            ,&p_md_emul_size->compr_dynamic_mem_size
            ,&p_md_emul_size->compr_ext_static_mem_size
            ,EMUL_BLK_SIZE
            );
  if(err)
  {
     return err;
  }
  return 0;
}

/*
 * Open emulator
 */

int32_t
dlb_md_emul_open
    (
     dlb_md_emul_size_t           *p_dlb_md_emul_size     /**< [in] dlb_md_emul handler pointer            */
    ,dlb_md_emul_hdl_t            *p_dlb_md_emul_hdl      /**< [in/out] dlb_md_emul handler pointer            */
    ,void                         *p_static_mem           /**< [in] pointer to externally allocated memory */
    ,void                         *p_dynamic_mem          /**< [in] pointer to externally allocated memory */
    )
{
  int32_t err = 0;

  p_dlb_md_emul_hdl->p_emul_hdl = p_static_mem;

  err = dd_emulation_open(
                          p_static_mem
                         ,p_dynamic_mem
                         ,p_dlb_md_emul_size->emul_static_mem_size
                         ,p_dlb_md_emul_size->emul_dynamic_mem_size
                         ,p_dlb_md_emul_size->compr_static_mem_size
                         ,p_dlb_md_emul_size->compr_dynamic_mem_size
                         ,p_dlb_md_emul_size->compr_ext_static_mem_size
                         ,EMUL_BLK_SIZE
                         );

 return err;

}



/*
 * Close emulator and return memory block
 */
int32_t
dlb_md_emul_close
    (
     dlb_md_emul_hdl_t *p_dlb_md_emul_hdl
    )
{

  dd_emulation_close(p_dlb_md_emul_hdl->p_emul_hdl);
  return 0;

}

/*
 * Perform the encode emulation
 *
 * Perform filtering based on
 * Compute DRC gains
 * Perform downmix
 */
int32_t
dlb_md_emul_process
    (
     dlb_md_emul_hdl_t             *p_dlb_md_emul_hdl /**< [in/out] pointer to metadata emulation handler */
    ,dlb_md_emul_process_config_t  *p_config          /**< [in/out] pointer to metadata emulation process control structure */
    ,int                            num_outputs       /**< [in] number of outputs for independent DRC & dialnorm application */
    )
{
   int32_t err = 0;

   md_emul_trans_config_t trans_config;

   if (p_config->control & (DLB_MD_EMUL_CONTROL_DECODER_ENABLE | DLB_MD_EMUL_CONTROL_ENCODER_ENABLE))
   {
       dlb_md_emul_to_dd_emu(p_config, &trans_config.emul_process_config);

       err = dd_emulation_process
                  (
                   p_dlb_md_emul_hdl->p_emul_hdl
                  ,&trans_config.emul_process_config
                  ,num_outputs
                  );
       if (err)
       {
          return err;
       }
  }
  return 0;

}

/* translate dlb_md_emul_process_config_t to dd_emu_process_config */
static
void
dlb_md_emul_to_dd_emu
    (
     dlb_md_emul_process_config_t   *p_config      /**< [in/out] pointer to metadata emulation process control structure */
    ,dd_emu_process_config          *p_dd_emu_process_config
    )
{
    int i;

    /* Point to master input/output buffer */
    p_dd_emu_process_config->pa_app_data[MASTER_BUF] = p_config->pa_in_data[MASTER_BUF];

    /* Point to secondary output buffer */
    p_dd_emu_process_config->pa_app_data[AUX_BUF] = p_config->pa_in_data[AUX_BUF];


    for (i = 0; i < DLB_MD_EMUL_MAX_CHANS; i++)
    {
       p_dd_emu_process_config->a_chan_map[i]  = p_config->a_chan_map[i];
    }

    p_dd_emu_process_config->emu_blk_size  = EMUL_BLK_SIZE;
    p_dd_emu_process_config->sample_offset = p_config->sample_offset;
    p_dd_emu_process_config->num_samples   = p_config->num_samples;
    p_dd_emu_process_config->sample_rate   = p_config->sample_rate;
    p_dd_emu_process_config->lfe_on        = p_config->lfe_on;
    p_dd_emu_process_config->control       = p_config->control;

    switch(p_config->channel_mode)
    {
      case DLB_MD_EMUL_CHMOD_1_0_0:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_MONO;
           break;
      case DLB_MD_EMUL_CHMOD_2_0_0:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_STEREO;
           break;
      case DLB_MD_EMUL_CHMOD_3_1_0:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_3_1;
           break;
      case DLB_MD_EMUL_CHMOD_3_2_1:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_3_2;
           break;
      case DLB_MD_EMUL_CHMOD_3_4_1:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_3_4;
           break;
      default:
           p_dd_emu_process_config->channel_mode  = DD_EMU_CHMODE_3_2;
      break;
    }

    switch(p_config->dolbye_channel_mode)
    {
      case DLB_MD_EMUL_CHMOD_1_0_0:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_MONO;
           break;
      case DLB_MD_EMUL_CHMOD_2_0_0:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_STEREO;
           break;
      case DLB_MD_EMUL_CHMOD_3_1_0:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_3_1;
           break;
      case DLB_MD_EMUL_CHMOD_3_2_1:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_3_2;
           break;
      case DLB_MD_EMUL_CHMOD_3_4_1:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_3_4;
           break;
      default:
           p_dd_emu_process_config->dolbye_channel_mode  = DD_EMU_CHMODE_3_2;
      break;
    }

    p_dd_emu_process_config->comp_mode[0] = (DD_EMU_COMPRESSION_MODE)p_config->comp_mode[0];
    p_dd_emu_process_config->comp_mode[1] = (DD_EMU_COMPRESSION_MODE)p_config->comp_mode[1];
    p_dd_emu_process_config->comp_profile = (DD_EMU_COMPRESSION_PROFILE_TYPE)p_config->comp_profile;
    p_dd_emu_process_config->drc_profile  = (DD_EMU_COMPRESSION_PROFILE_TYPE)p_config->drc_profile;

    p_dd_emu_process_config->use_bitstream_gainwords[0] = p_config->use_bitstream_gainwords[0];
    p_dd_emu_process_config->use_bitstream_gainwords[1] = p_config->use_bitstream_gainwords[1];

    p_dd_emu_process_config->custom_boost[0] = p_config->custom_boost[0];
    p_dd_emu_process_config->custom_boost[1] = p_config->custom_boost[1];
    p_dd_emu_process_config->custom_cut[0]   = p_config->custom_cut[0];
    p_dd_emu_process_config->custom_cut[1]   = p_config->custom_cut[1];
    p_dd_emu_process_config->dialnorm        = p_config->dialnorm;
    p_dd_emu_process_config->compr_dd        = p_config->compr_dd;
    p_dd_emu_process_config->dynrng_dd       = p_config->dynrng_dd;

    p_dd_emu_process_config->sur90on  = p_config->sur90on;
    p_dd_emu_process_config->suratton = p_config->suratton;
    p_dd_emu_process_config->hpfon    = p_config->hpfon;
    p_dd_emu_process_config->bwlpfon  = p_config->bwlpfon;
    p_dd_emu_process_config->lfelpfon = p_config->lfelpfon;

}

