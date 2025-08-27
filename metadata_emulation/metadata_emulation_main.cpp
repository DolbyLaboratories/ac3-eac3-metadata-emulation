/****************************************************************************
 *
 *
 * Copyright (c) 2025 Dolby International AB.
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
 * @brief  Main code for metadata emulation command line application
 *
 *
 */

#include <vector>
#include <iostream>

#include <sndfile.hh>
#include "dlb_md_emul_api.h"


#define    SAMPLES_PER_BLOCK = (DLB_MD_EMUL_BLOCK_SIZE / 2)
#define    AGG_ACMOD_71 21

#define    MAJOR_VERSION 1
#define    MINOR_VERSION 0
#define    FUNC_VERSION 0


typedef struct
{
    DLB_MD_EMUL_COMPRESSION_MODE    compression_mode_main;
    DLB_MD_EMUL_COMPRESSION_MODE    compression_mode_aux;

    uint32_t                        custom_boost_main;
    uint32_t                        custom_cut_main;

    uint32_t                        custom_boost_aux;
    uint32_t                        custom_cut_aux;
} metadata_emulation_params;


typedef struct
{
    uint32_t               num_outputs;                     /**< Number of active outputs */       

    /* User selected program */
    uint16_t               program_select;

    /* Dolby E metadata */
    uint16_t               program_config;

    /* Dolby Digital metadata */
    /* NOTE: These sizes match the sizes in dlb_md_emul_process_config_t */
    uint16_t               acmod;
    uint32_t               dialnorm;
    uint32_t               lfeon;
    uint16_t               compre;
    uint32_t               compr;
    uint16_t               dynrnge;
    uint32_t               dynrng;
    uint32_t               sur90on;
    uint32_t               suratton;
    uint32_t               hpfon;
    uint32_t               bwlpfon;
    uint32_t               lfelpfon;

    /* Control parameters */
    DLB_LFRACT             custom_boost[2];
    DLB_LFRACT             custom_cut[2];

    dlb_md_emul_hdl_t      emul_hdl;

    void                   *emul_static_mem;
    void                   *emul_dynamic_mem;

    uint16_t                 prog_remap_enable;
    DLB_MD_EMUL_CHANNEL_MODE channel_mode;

    /**
      * Parameters
      */
    metadata_emulation_params   params;

} metadata_emulation_state;

#define MAX_PROG_CFG    26
#define MAX_PROGRAMS    8

static const int prog2acmod[MAX_PROG_CFG][MAX_PROGRAMS] =
{
   { 7,  2, -1, -1, -1, -1, -1, -1 },  /* 5.1 + 2 */
   { 7,  1,  1, -1, -1, -1, -1, -1 },  /* 5.1 + 1 + 1 */
   { 5,  5, -1, -1, -1, -1, -1, -1 },  /* 4 + 4 */
   { 5,  2,  2, -1, -1, -1, -1, -1 },  /* 4 + 2 + 2 */
   { 5,  2,  1,  1, -1, -1, -1, -1 },  /* 4 + 2 + 1 + 1 */
   { 5,  1,  1,  1,  1, -1, -1, -1 },  /* 4 + 1 + 1 + 1 + 1 */
   { 2,  2,  2,  2, -1, -1, -1, -1 },  /* 2 + 2 + 2 + 2 */
   { 2,  2,  2,  1,  1, -1, -1, -1 },  /* 2 + 2 + 2 + 1 + 1 */
   { 2,  2,  1,  1,  1,  1, -1, -1 },  /* 2 + 2 + 1 + 1 + 1 + 1*/
   { 2,  1,  1,  1,  1,  1,  1, -1 },  /* 2 + 1 + 1 + 1 + 1 + 1 + 1*/
   { 1,  1,  1,  1,  1,  1,  1,  1 },  /* 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 */
   { 7, -1, -1, -1, -1, -1, -1, -1 },  /* 5.1 */
   { 5,  2, -1, -1, -1, -1, -1, -1 },  /* 4 + 2 */
   { 5,  1,  1, -1, -1, -1, -1, -1 },  /* 4 + 1 + 1 */
   { 2,  2,  2, -1, -1, -1, -1, -1 },  /* 2 + 2 + 2 */
   { 2,  2,  1,  1, -1, -1, -1, -1 },  /* 2 + 2 + 1 + 1 */
   { 2,  1,  1,  1,  1, -1, -1, -1 },  /* 2 + 1 + 1 + 1 + 1 */
   { 1,  1,  1,  1,  1,  1, -1, -1 },  /* 1 + 1 + 1 + 1 + 1 + 1 */
   { 5, -1, -1, -1, -1, -1, -1, -1 },  /* 4 */
   { 2,  2, -1, -1, -1, -1, -1, -1 },  /* 2 + 2 */
   { 2,  1,  1, -1, -1, -1, -1, -1 },  /* 2 + 1 + 1 */
   { 1,  1,  1,  1, -1, -1, -1, -1 },  /* 1 + 1 + 1 + 1 */
   {-1, -1, -1, -1, -1, -1, -1, -1 },  /* 7.1 */
   {-1, -1, -1, -1, -1, -1, -1, -1 },  /* 7.1 Screen */    
   {-1, -1, -1, -1, -1, -1, -1, -1 },  /* PCM Bypass */
   {-1, -1, -1, -1, -1, -1, -1, -1 }   /* Other */
};

static std::string program_config_str[MAX_PROG_CFG] = {
   " 5.1 + 2 "
   ,  " 5.1 + 1 + 1 "
   ,  " 4 + 4 "
   ,  " 4 + 2 + 2 "
   ,  " 4 + 2 + 1 + 1 "
   ,  " 4 + 1 + 1 + 1 + 1 "
   ,  " 2 + 2 + 2 + 2 "
   ,  " 2 + 2 + 2 + 1 + 1 "
   ,  " 2 + 2 + 1 + 1 + 1 + 1"
   ,  " 2 + 1 + 1 + 1 + 1 + 1 + 1"
   ,  " 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 "
   ,  " 5.1 "
   ,  " 4 + 2 "
   ,  " 4 + 1 + 1 "
   ,  " 2 + 2 + 2 "
   ,  " 2 + 2 + 1 + 1 "
   ,  " 2 + 1 + 1 + 1 + 1 "
   ,  " 1 + 1 + 1 + 1 + 1 + 1 "
   ,  " 4 "
   ,  " 2 + 2 "
   ,  " 2 + 1 + 1 "
   ,  " 1 + 1 + 1 + 1 "
   ,  " 7.1 "
   ,  " 7.1 Screen "    
   ,  " PCM Bypass "
   ,  " Other "};


static std::string compression_mode_string[DLB_MD_EMUL_CM_RF + 1];

static void populate_debug_strings(void)
{
    compression_mode_string[DLB_MD_EMUL_CM_NONE]        = "No dialog normalization";
    compression_mode_string[DLB_MD_EMUL_CM_DIALNORM ]   = "Dialog normalization only";
    compression_mode_string[DLB_MD_EMUL_CM_CUSTOM]      = "Custom mode";
    compression_mode_string[DLB_MD_EMUL_CM_LINE]        = "Line mode";
    compression_mode_string[DLB_MD_EMUL_CM_RF]          = "RF mode";
}

static
void
limit_channel_mode
    (metadata_emulation_state       *md_emul    /**< [in]  State */
    ,dlb_md_emul_process_config_t   *emul_conf  /**< [out] Emulation configuration */
    )
{
    uint16_t acmod;

    /* limit channel mode to the supported options */
    acmod = md_emul->acmod;

    /* Set Dolby E source channel mode based on program config and program select */
    switch (prog2acmod[md_emul->program_config][md_emul->program_select])
    {
        default:
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_3_2_1;
            break;

        case 1: 
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_1_0_0;
            break;

        case 0:
        case 2:
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_2_0_0;
            break;
           
        case 5:
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_3_1_0;
            break;

        case 7:
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_3_2_1;
            break;
    }

    /* Set channel mode */
    switch (acmod)
    {
        default:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_3_2_1;
            break;

        case 1:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_1_0_0;
            break;

        case 0:
            /* fall-through */
        case 2:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_2_0_0;
            break;

        case 5:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_3_1_0;
            break;

        case 7:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_3_2_1;
            break;

        /* Special case for DD+ 7.1 */
        case AGG_ACMOD_71:
            emul_conf->channel_mode = DLB_MD_EMUL_CHMOD_3_4_1;
            emul_conf->dolbye_channel_mode = DLB_MD_EMUL_CHMOD_3_4_1;
            break;
    }

    // Force MD emulation to act on the 'program config acmod'
	// If we decide to allow the Dolby E 'individual program
	// acmod' to dictate how MD Emulation is applied, we can
    // remove this line.
    emul_conf->channel_mode = emul_conf->dolbye_channel_mode;

    /* Check that channel mode is not larger than it can be */
    if(emul_conf->channel_mode > emul_conf->dolbye_channel_mode)
    {
        /* Protect against illegal configuration */
        emul_conf->channel_mode = emul_conf->dolbye_channel_mode;
    }
}

static
void
init_channel_map
    (dlb_md_emul_process_config_t   *emul_conf  /**< [in/out] Emulation configuration */
    )
{
    unsigned int i;

    /* Initialize channel map */
    for (i = 0u; i < DLB_MD_EMUL_MAX_CHANS; ++i)
    {
        emul_conf->a_chan_map[i] = DLB_MD_EMUL_CHAN_NONE;
    }

    /* Initialize channel map based on channel mode */
    switch (emul_conf->channel_mode)
    {
        default:
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->a_chan_map[1] = DLB_MD_EMUL_CHAN_RGHT;
            emul_conf->a_chan_map[2] = DLB_MD_EMUL_CHAN_CNTR;
            emul_conf->a_chan_map[3] = DLB_MD_EMUL_CHAN_LFE;
            emul_conf->a_chan_map[4] = DLB_MD_EMUL_CHAN_LSUR;
            emul_conf->a_chan_map[5] = DLB_MD_EMUL_CHAN_RSUR;
            break;

        case DLB_MD_EMUL_CHMOD_1_0_0:
            /* Use LEFT instead of CNTR to match DP572 mapping */
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->lfe_on = 0;
            break;

        case DLB_MD_EMUL_CHMOD_2_0_0:
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->a_chan_map[1] = DLB_MD_EMUL_CHAN_RGHT;
            emul_conf->lfe_on = 0;
            break;

        case DLB_MD_EMUL_CHMOD_3_1_0:
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->a_chan_map[1] = DLB_MD_EMUL_CHAN_RGHT;
            emul_conf->a_chan_map[2] = DLB_MD_EMUL_CHAN_CNTR;
            emul_conf->a_chan_map[3] = DLB_MD_EMUL_CHAN_MSUR;
            emul_conf->lfe_on = 0;
            break;

        case DLB_MD_EMUL_CHMOD_3_2_1:
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->a_chan_map[1] = DLB_MD_EMUL_CHAN_RGHT;
            emul_conf->a_chan_map[2] = DLB_MD_EMUL_CHAN_CNTR;
            emul_conf->a_chan_map[3] = DLB_MD_EMUL_CHAN_LFE;
            emul_conf->a_chan_map[4] = DLB_MD_EMUL_CHAN_LSUR;
            emul_conf->a_chan_map[5] = DLB_MD_EMUL_CHAN_RSUR;
            break;

        case DLB_MD_EMUL_CHMOD_3_4_1:
            emul_conf->a_chan_map[0] = DLB_MD_EMUL_CHAN_LEFT;
            emul_conf->a_chan_map[1] = DLB_MD_EMUL_CHAN_RGHT;
            emul_conf->a_chan_map[2] = DLB_MD_EMUL_CHAN_CNTR;
            emul_conf->a_chan_map[3] = DLB_MD_EMUL_CHAN_LFE;
            emul_conf->a_chan_map[4] = DLB_MD_EMUL_CHAN_LSUR;
            emul_conf->a_chan_map[5] = DLB_MD_EMUL_CHAN_RSUR;
            emul_conf->a_chan_map[6] = DLB_MD_EMUL_CHAN_LBAK;
            emul_conf->a_chan_map[7] = DLB_MD_EMUL_CHAN_RBAK;
            break;
    }
}


static
void
setup_emulation_params
    (metadata_emulation_state       *md_emul    /**< [in]  State */
    ,dlb_md_emul_process_config_t   *emul_conf  /**< [in]  Emulation configuration */
    )
{

    emul_conf->comp_mode[0] = md_emul->params.compression_mode_main;
    emul_conf->comp_mode[1]  = md_emul->params.compression_mode_aux;

    /* Set DRC parameters */
    emul_conf->custom_boost[0] = md_emul->custom_boost[0];
    emul_conf->custom_boost[1] = md_emul->custom_boost[1];

    emul_conf->custom_cut[0] = md_emul->custom_cut[0];
    emul_conf->custom_cut[1] = md_emul->custom_cut[1];

    emul_conf->dialnorm = md_emul->dialnorm;

    /* Set encoder emulation params */
    emul_conf->sur90on = md_emul->sur90on;
    emul_conf->suratton = md_emul->suratton;
    emul_conf->hpfon = md_emul->hpfon;
    emul_conf->bwlpfon = md_emul->bwlpfon;
    emul_conf->lfelpfon = md_emul->lfelpfon;


    /* Enable decoder and encoder emulation, and enable DRC calculation */
    emul_conf->control = (DLB_MD_EMUL_CONTROL_ENCODER_ENABLE | DLB_MD_EMUL_CONTROL_DECODER_ENABLE | DLB_MD_EMUL_CONTROL_DRC_CALC_ENABLE);

    /* Populate DRC profiles */
    /* TODO: use appropriate default if dynrng / compr are not present as metadata values */
    emul_conf->drc_profile = (DLB_MD_EMUL_COMPRESSION_PROFILE) md_emul->dynrng;
    emul_conf->comp_profile = (DLB_MD_EMUL_COMPRESSION_PROFILE) md_emul->compr;

    /* Since PCM does not carry gainwords, set these to 0 */
    emul_conf->use_bitstream_gainwords[0] = 0u;
    emul_conf->use_bitstream_gainwords[1] = 0u;
}

static void show_usage(void)
{
    std::cout << "Dolby AC-3 & EC-3 Metadata Emulation, Version " << MAJOR_VERSION << "." << MINOR_VERSION << "." << FUNC_VERSION << std::endl;
    std::cout << "Copyright (c) 1993-2025 Dolby Laboratories, Inc.  All rights reserved." << std::endl;

	std::cout << "Usage:" << std::endl;
	std::cout << "\tMdEmu [options] infile outfile" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << ""
"        -a     Audio coding mode [-a7 = 3/2 mode]" << std::endl <<
"                 0 = 1+1 (L, R)" << std::endl <<
"                 1 = 1/0 (C)" << std::endl <<
"                 2 = 2/0 (L, R)" << std::endl <<
"                 3 = 3/0 (L, C, R)" << std::endl <<
"                 4 = 2/1 (L, R, l)" << std::endl <<
"                 5 = 3/1 (L, C, R, l)" << std::endl <<
"                 6 = 2/2 (L, R, l, r)" << std::endl <<
"                 7 = 3/2 (L, C, R, l, r)" << std::endl <<
"        -c     Dynamic range compression mode [-c2 = line out mode)" << std::endl <<
"                 0 = custom mode, analog dialnorm" << std::endl <<
"                 1 = custom mode, digital dialnorm" << std::endl <<
"                 2 = line out mode" << std::endl <<
"                 3 = RF remod mode" << std::endl <<
"        -dn    Dialog Normalization (1..31) [-dn27 = -27dB]" << std::endl <<
"                 1  = -1dB (loudest input)" << std::endl <<
"                   ..." << std::endl <<
"                 31 = -31dB (quiet input)" << std::endl <<
"        -g     LFE filter flag [-g1 = LFE enabled]" << std::endl <<
"        -h     Show this usage message and abort" << std::endl <<
"        -j     DC filter flag [-j0 = DC filter disabled]" << std::endl <<
"        -k     Global compression profile (0..5) [-k0 = disabled]" << std::endl <<
"        -kd    'dynrng' profile (overrides global profile) [-kd0]" << std::endl <<
"        -kc    'compr' profile (overrides global profile) [-kc0]" << std::endl <<
"                 0 = no compression" << std::endl <<
"                 1 = film standard compression" << std::endl <<
"                 2 = film light compression" << std::endl <<
"                 3 = music standard compression" << std::endl <<
"                 4 = music light compression" << std::endl <<
"                 5 = speech compression" << std::endl <<
"        -l     Low frequency effects channel on/off [-l1 = LFE on]" << std::endl <<
"        -w     Bandwidth filter flag [-w0 = disabled]" << std::endl <<
"        -9     90 deg phase shift surrounds [-90 = disabled]" << std::endl <<
"        -$     Enable 3 dB surround attenuation [-$0 = disabled]" << std::endl <<
"        -p     Dolby E program configuration [-p0 = 5.1+2]" << std::endl <<
"                 0 = 5.1+2           1 = 5.1+1+1           2 = 4+4" << std::endl <<
"                 3 = 4+2+2           4 = 4+2+1+1           5 = 4+1+1+1+1" << std::endl <<
"                 6 = 2+2+2+2         7 = 2+2+2+1+1         8 = 2+2+1+1+1+1" << std::endl <<
"                 9 = 2+1+1+1+1+1+1  10 = 1+1+1+1+1+1+1+1  11 = 5.1" << std::endl <<
"                12 = 4+2            13 = 4+1+1            14 = 2+2+2" << std::endl <<
"                15 = 2+2+1+1        16 = 2+1+1+1+1        17 = 1+1+1+1+1+1" << std::endl <<
"                18 = 4              19 = 2+2  20 = 2+1+1  21 = 1+1+1+1" << std::endl <<
"                22 = 7.1            23 = 7.1 Screen" << std::endl <<
"        -s      Program selection (0..7) [-s0 = first program]" << std::endl;

}

int main(int argc, char *argv[])
{
	std::vector<std::string>              args(argv + 1, argv + argc);
	metadata_emulation_state	md_emul;
	dlb_md_emul_size_t          dlb_md_emul_size;
    int32_t                     err;
    size_t                      static_memory_sz;
    size_t                      dynamic_memory_sz;
    char                        *static_memory;
    char						*dynamic_memory;
    dlb_md_emul_process_config_t emul_conf;
    std::string					input_wav_file_str;
    std::string					output_wav_file_str;
    SndfileHandle               input_wav_file;
    SndfileHandle               output_wav_file;
    sf_count_t                  input_file_size;
    sf_count_t                  input_frames_read;
    DLB_LFRACT                  primary_io_samples[DLB_MD_EMUL_MAX_CHANS * DLB_MD_EMUL_BLOCK_SIZE];
    DLB_LFRACT                  secondary_op_samples[DLB_MD_EMUL_MAX_CHANS * DLB_MD_EMUL_BLOCK_SIZE];
    DLB_LFRACT                  *input_data[DLB_MD_EMUL_MAX_OUTPUTS];

    input_wav_file_str.clear();
    output_wav_file_str.clear();

    populate_debug_strings();

    /* default values  as per ddpe / ddcd */
    md_emul.program_config = 0; //5.1 + 2
    md_emul.program_select = 0; //5.1
    md_emul.params.compression_mode_main = DLB_MD_EMUL_CM_LINE; // Line Mode
    md_emul.acmod = 7;
    md_emul.dialnorm = 27;
    md_emul.lfeon = 1;
    md_emul.compre = 0;
    md_emul.compr = 0;
    md_emul.dynrnge = 0;
    md_emul.dynrng = 0;
    md_emul.sur90on = 0;
    md_emul.suratton = 0;
    md_emul.hpfon = 0;
    md_emul.bwlpfon = 0;
    md_emul.lfelpfon = 1;

    char c;
    for (std::string arg : args)
    {
        if (arg[0] == '-')
        {
            arg.erase(0, arg.find_first_not_of("-"));
            c = arg[0];
            arg.erase(arg.begin());
            switch(c)
            {
            case 'a':
                md_emul.acmod = std::stoi(arg);
                break;
            case 'd':
                if (arg[0] == 'n')
                {
                    arg.erase(arg.begin());
                    md_emul.dialnorm = std::stoi(arg);                    
                }
                break;
            case 'c':
                switch(std::stoi(arg))
                {
                case 0: 
                    md_emul.params.compression_mode_main = DLB_MD_EMUL_CM_NONE;
                    break;
                case 1:
                    md_emul.params.compression_mode_main = DLB_MD_EMUL_CM_CUSTOM;
                    break;
                case 2:
                    md_emul.params.compression_mode_main = DLB_MD_EMUL_CM_LINE;
                    break;
                case 3:
                    md_emul.params.compression_mode_main = DLB_MD_EMUL_CM_RF;
                    break;
                default:
                    throw std::runtime_error("Invalid compression mode");
                }

            case 'g':
                md_emul.lfelpfon = std::stoi(arg);
                break;
            case 'h':
                show_usage();
                exit(0);
            case 'j':
                md_emul.hpfon = std::stoi(arg);
                break;
            case 'k':
                c = arg[0];
                arg.erase(arg.begin());
                switch (c)
                {
                case 'c':
                    md_emul.compr = std::stoi(arg);
                    break;
                case 'd':
                    md_emul.dynrng = std::stoi(arg);
                    break;
                default:
                    md_emul.compr = md_emul.dynrng = c - '0';
                }
                break;
            case 'l':
                md_emul.bwlpfon = std::stoi(arg);
                break;
            case '9':
                md_emul.sur90on = std::stoi(arg);
                break;
            case '$':
                md_emul.suratton = std::stoi(arg);
                break;
            case 'p':
                md_emul.program_config = std::stoi(arg);
                break;
            case 's':
                md_emul.program_select = std::stoi(arg);
                break;
            }
        }
        else
        {
            if (input_wav_file_str.empty())
            {
                input_wav_file_str = arg;
            }
            else
            {
                if (output_wav_file_str.empty())
                {
                    output_wav_file_str = arg;
                }
                else
                {
                    throw std::runtime_error("Too many files specified, only 2 allowed");
                }
            }
        }
    }

    if (args.size() < 2)
    {
        throw std::runtime_error("Insufficient arguments, must specify at least input and output file");
        show_usage();
        exit(-1);
    }

	err = dlb_md_emul_query_mem(&dlb_md_emul_size);
	if (err)
	{
		throw std::runtime_error("Failed to query memory");
		exit(-1);
	}

	static_memory_sz  = (  dlb_md_emul_size.emul_static_mem_size
                         + dlb_md_emul_size.compr_static_mem_size
                         + dlb_md_emul_size.compr_ext_static_mem_size);

    dynamic_memory_sz = (  dlb_md_emul_size.emul_dynamic_mem_size
                         + dlb_md_emul_size.compr_dynamic_mem_size);

    static_memory = new char[static_memory_sz];

    dynamic_memory = new char[dynamic_memory_sz];
    md_emul.emul_static_mem = static_memory;
    md_emul.emul_dynamic_mem = dynamic_memory;

//    std::cout << "md_emul.emul_static_mem: " << std::hex << md_emul.emul_static_mem << " - " << (void *)(static_memory + static_memory_sz - 1) << std::endl;
//    std::cout << "md_emul.emul_dynamic_mem: " << std::hex << md_emul.emul_dynamic_mem << " - " << (void *)(dynamic_memory + dynamic_memory_sz - 1) << std::endl;

    if ((static_memory == nullptr) || (dynamic_memory == nullptr))
    {
    	throw std::runtime_error("Memory Allocation Failed");
    }
    err = dlb_md_emul_open(&dlb_md_emul_size,
                        &md_emul.emul_hdl,
                        md_emul.emul_static_mem,
                        md_emul.emul_dynamic_mem);

    if (err)
    {
        throw std::runtime_error("Metadata Emulation Open Returned Error: " + std::to_string(err));
    }


    input_wav_file = SndfileHandle(input_wav_file_str.c_str());

    if (input_wav_file.error())
    {
        throw std::runtime_error("Input File not opened: " + std::string(input_wav_file.strError()));
    }


/*
    if (input_wav_file.channels() != 8)
    {
        throw std::runtime_error("Only 8 channel wav file input supported, the file supplied has " + std::to_string(input_wav_file.channels()) + " channels");
    }
*/

    input_file_size = input_wav_file.frames();
    input_frames_read = 0;
    input_data[0] = primary_io_samples;
    input_data[1] = secondary_op_samples;

    md_emul.num_outputs = 1; // Only using main output for now

    output_wav_file = SndfileHandle(output_wav_file_str.c_str(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, input_wav_file.channels(), 48000);

    if (output_wav_file.error())
    {
        throw std::runtime_error("Output File not opened: " + std::string(output_wav_file.strError()));
    }


    std::cout << "Input File: " << input_wav_file_str << std::endl;
    std::cout << "Output File: " << output_wav_file_str << std::endl;
    std::cout << "Frames to read: " << input_file_size << std::endl;
    std::cout << "Program Configuration: " << program_config_str[md_emul.program_config] << std::endl;
    std::cout << "Program Selection: " << md_emul.program_select << std::endl;
    std::cout << "Compression Mode: " << compression_mode_string[md_emul.params.compression_mode_main] << std::endl;
    std::cout << "acmod: " << md_emul.acmod << std::endl;
    std::cout << "lfeon: " << md_emul.lfeon << std::endl;
    std::cout << "dialnorm: " << md_emul.dialnorm << std::endl;
    std::cout << "compre: " << md_emul.compre << std::endl;
    std::cout << "compr: " << md_emul.compr << std::endl;
    std::cout << "dynrnge: " << md_emul.dynrnge << std::endl;
    std::cout << "dynrng: " << md_emul.dynrng << std::endl;
    std::cout << "sur90on: " << md_emul.sur90on << std::endl;
    std::cout << "suratton: " << md_emul.suratton << std::endl;
    std::cout << "hpfon: " << md_emul.hpfon << std::endl;
    std::cout << "bwlpfon: " << md_emul.bwlpfon << std::endl;
    std::cout << "lfelpfon: " << md_emul.lfelpfon << std::endl << std::endl;

    while(input_frames_read < input_file_size)
    {
        input_wav_file.read(primary_io_samples, input_wav_file.channels() * DLB_MD_EMUL_BLOCK_SIZE);
        input_frames_read += DLB_MD_EMUL_BLOCK_SIZE;

    	limit_channel_mode(&md_emul, &emul_conf);

	    /* Save channel mode for prepare_output() */
	    md_emul.channel_mode = emul_conf.channel_mode;

	    emul_conf.pa_in_data[0] = primary_io_samples;
        emul_conf.pa_in_data[1] = secondary_op_samples;

	    emul_conf.lfe_on = md_emul.lfeon;

	    init_channel_map(&emul_conf);

	    /* Set audio metadata */
	    emul_conf.sample_offset = input_wav_file.channels();
	    emul_conf.num_samples = DLB_MD_EMUL_BLOCK_SIZE; 
	    emul_conf.sample_rate = 48000u;

	    setup_emulation_params(&md_emul, &emul_conf);

    	err = dlb_md_emul_process(&md_emul.emul_hdl, &emul_conf, md_emul.num_outputs);

        if (err)
        {
            throw std::runtime_error("Metadata Emulation Process Returned Error: " + std::to_string(err));
        }

        output_wav_file.write(primary_io_samples, input_wav_file.channels() * DLB_MD_EMUL_BLOCK_SIZE);
        std::cout << "\rWrote: " << input_frames_read << " frames" << std::flush;
    }

    std::cout << std::endl << "Metadata Emulation Process Complete" << std::endl;
}


