/*
 *  Synthesizer.cpp - SPI implement file for the Spresense SDK
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

//***************************************************************************
// Included Files
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arch/board/board.h>

#include "Synthesizer.h"
#include "MemoryUtil.h"

extern "C" {

static void attentionCallback(const ErrorAttentionParam *attparam)
{
  print_err("Attention!! Level 0x%x Code 0x%x\n", attparam->error_code, attparam->error_att_sub_code);
}

}

/****************************************************************************
 * Public API on Synthesizer Class
 ****************************************************************************/

err_t Synthesizer::begin(void)
{
  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::create(AudioAttentionCb attcb)
{
  AsCreateSynthesizerParam_t synthe_create_param;

  synthe_create_param.msgq_id.mixer       = MSGQ_AUD_OUTPUT_MIX;
  synthe_create_param.msgq_id.synthesizer = MSGQ_AUD_SYNTHESIZER;
  synthe_create_param.msgq_id.mng         = MSGQ_AUD_MGR;
  synthe_create_param.msgq_id.dsp         = MSGQ_AUD_DSP;
  synthe_create_param.pool_id.input       = S0_NULL_POOL;
  synthe_create_param.pool_id.output      = S0_REND_PCM_BUF_POOL;
  synthe_create_param.pool_id.dsp         = S0_DEC_APU_CMD_POOL;
  
  bool result = AS_CreateMediaSynthesizer(&synthe_create_param,
                                          (attcb != NULL) ? attcb : attentionCallback);

  if (!result)
    {
      print_err("Error: AS_CreateMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::activate(SynthesizerCallback synthcb, void *param)
{
  AsActivateSynthesizer actparam;

  actparam.cb    = synthcb;
  actparam.param = param;

  /* Activate synthesizer. */

  bool result = AS_ActivateMediaSynthesizer(&actparam);

  if (!result)
    {
      print_err("Error: AS_ActivateMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }
   
  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::init(AsSynthesizerWaveMode type,
                        uint8_t               channel_num,
                        const char           *dsp_path,
                        uint16_t              attack,
                        uint16_t              decay,
                        uint16_t              sustain,
                        uint16_t              release)
{
  AsInitSynthesizerParam initparam;

  initparam.type          = type;
  initparam.channel_num   = channel_num;
  initparam.sampling_rate = AS_SAMPLINGRATE_48000;
  initparam.bit_width     = AS_BITLENGTH_16;
  initparam.sample_size   = 240;
  initparam.attack        = attack;
  initparam.decay         = decay;
  initparam.sustain       = sustain;
  initparam.release       = release;

  if (channel_num > AsSynthesizerMaxChannelNum)
    {
      print_err("Wrong number of channels!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  /* Keep effect value */

  for (int i = 0; i < channel_num; i++)
    {
      m_attack[i]  = attack;
      m_decay[i]   = decay;
      m_sustain[i] = sustain;
      m_release[i] = release;
    }

  snprintf(initparam.dsp_path, AS_AUDIO_DSP_PATH_LEN, "%s", dsp_path);

  /* Init synthesizer. */

  bool result = AS_InitMediaSynthesizer(&initparam);

  if (!result)
    {
      print_err("Error: AS_InitMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::init(AsSynthesizerWaveMode type,
                        uint8_t               channel_num,
                        const char           *dsp_path)
{
  return init(type,
              channel_num,
              dsp_path,
              1,
              1,
              100,
              1);
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::start(void)
{
  /* Unmute external amp. */

  board_external_amp_mute_control(false);

  /* Start synthesizer. */

  bool result = AS_StartMediaSynthesizer();

  if (!result)
    {
      print_err("Error: AS_StartMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::stop(void)
{
  /* Stop synthesizer. */

  bool result = AS_StopMediaSynthesizer();

  if (!result)
    {
      print_err("Error: AS_StopMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}
err_t Synthesizer::set(uint8_t  channel_no,
                       uint32_t frequency,
                       uint16_t attack,
                       uint16_t decay,
                       uint16_t sustain,
                       uint16_t release)
{
  AsSetSynthesizer setparam;

  setparam.channel_no = channel_no;
  setparam.frequency  = frequency;
  setparam.attack     = attack;
  setparam.decay      = decay;
  setparam.sustain    = sustain;
  setparam.release    = release;

  /* Keep effect value */

  m_attack[channel_no]  = attack;
  m_decay[channel_no]   = decay;
  m_sustain[channel_no] = sustain;
  m_release[channel_no] = release;

  bool result = AS_SetMediaSynthesizer(&setparam);

  if (!result)
    {
      print_err("Error: AS_SetMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::set(uint8_t channel_no, uint32_t frequency)
{
  return set(channel_no,
             frequency,
             m_attack[channel_no],
             m_decay[channel_no],
             m_sustain[channel_no],
             m_release[channel_no]);
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::deactivate(void)
{
  bool result = AS_DeactivateMediaSynthesizer();

  if (!result)
    {
      print_err("Error: AS_DeactivateMediaSynthesizer() failure!\n");
      return SYNTHESIZER_ECODE_COMMAND_ERROR;
    }

  return SYNTHESIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Synthesizer::end(void)
{
  return SYNTHESIZER_ECODE_OK;
}

