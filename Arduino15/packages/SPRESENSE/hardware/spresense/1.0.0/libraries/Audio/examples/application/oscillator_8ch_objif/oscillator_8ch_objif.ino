/*
 *  oscillator_8ch.ino - oscillator example application
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <SDHCI.h>
#include <Synthesizer.h>
#include <OutputMixer.h>
#include <MemoryUtil.h>
#include "userproc_command.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Maximum number of channels */

#define CHANNEL_NUMBER        8

/* Oscillator parameter */

#define OSC_CH_NUM            CHANNEL_NUMBER
#define OSC_EFFECT_ATTACK     1
#define OSC_EFFECT_DECAY      1
#define OSC_EFFECT_SUSTAIN    100
#define OSC_EFFECT_RELEASE    1

/* Set volume[db] */

#define VOLUME_MASTER         -120

/* DSP file path */

#define DSPBIN_PATH           "/mnt/sd0/BIN/OSCPROC"

/* Musical scale */

#define M_C_4   262   /* Do */
#define M_CC4   277
#define M_D_4   294   /* Re */
#define M_DD4   311
#define M_E_4   330   /* Mi */
#define M_F_4   349   /* Fa */
#define M_FF4   370
#define M_G_4   392   /* So */
#define M_GG4   415
#define M_A_4   440   /* Ra */
#define M_AA4   466
#define M_B_4   494   /* Si */
#define M_C_5   523

/****************************************************************************
 * Private Data
 ****************************************************************************/

OutputMixer *theMixer;
Synthesizer *theSynthesizer;

SDClass theSD;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * @brief Mixer done callback procedure
 *
 * @param [in] requester_dtq    MsgQueId type
 * @param [in] reply_of         MsgType type
 * @param [in,out] done_param   AsOutputMixDoneParam type pointer
 */

static void outputmixer_done_callback(MsgQueId              requester_dtq,
                                      MsgType               reply_of,
                                      AsOutputMixDoneParam *done_param)
{
  return;
}


/**
 * @brief Synthesizer done callback function
 *
 * @param [in] event        AsSynthesizerEvent type indicator
 * @param [in] result       Result
 * @param [in] sub_result   Sub result
 *
 * @return true on success, false otherwise
 */

static void synthesizer_done_callback(AsSynthesizerEvent event,
                                      uint32_t           result,
                                      void              *param)
{
  return true;
}

/**
 * @brief Audio attention callback
 *
 * When Internal error occur, this function will be called back.
 */

static void attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
}

/****************************************************************************
 * Publuc Functions
 ****************************************************************************/

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  /* Mount SD card */

  theSD.begin();

  /* Initialize memory pools and message libs */

  initMemoryPools();
  createStaticPools(MEM_LAYOUT_PLAYER);

  /* Get static synthesizer instance */

  theSynthesizer = Synthesizer::getInstance();
  theSynthesizer->begin();

  /* Get static mixer instance */

  theMixer = OutputMixer::getInstance();
  theMixer->activateBaseband();

  /* Create objects */

  theMixer->create(attention_cb);
  theSynthesizer->create(attention_cb);

  /* Activate objects */

  theSynthesizer->activate(synthesizer_done_callback, NULL);

  /* Specifying true for the fourth argument enables PostPorocess */

  theMixer->activate(OutputMixer0, HPOutputDevice, outputmixer_done_callback, true);

  /* Initialize PostProcess DSP. */

  InitParam initpost;

  initpost.input_channel_num = OSC_CH_NUM;

  theMixer->initPostproc(OutputMixer0, &initpost, sizeof(initpost));

  /* Initialize synthesizer. */

  theSynthesizer->init(AsSynthesizerSinWave,
                       OSC_CH_NUM,
                       DSPBIN_PATH,
                       OSC_EFFECT_ATTACK,
                       OSC_EFFECT_DECAY,
                       OSC_EFFECT_SUSTAIN,
                       OSC_EFFECT_RELEASE);

  /* Main volume set to -12.0 dB */

  theMixer->setVolume(VOLUME_MASTER, 0, 0);

  /* Start synthesizer */

  theSynthesizer->start();
}

/* ------------------------------------------------------------------------ */
void loop()
{
  puts("loop!!");

  theSynthesizer->set(0, M_C_4);
  theSynthesizer->set(1, M_C_4);

  sleep(1);

  theSynthesizer->set(0, M_E_4);
  theSynthesizer->set(1, M_E_4);

  sleep(1);

  theSynthesizer->set(0, M_G_4);
  theSynthesizer->set(1, M_G_4);

  sleep(1);

  theSynthesizer->set(0, M_C_4);
  theSynthesizer->set(1, M_C_4);
  usleep(10 * 1000);
  theSynthesizer->set(2, M_E_4);
  theSynthesizer->set(3, M_E_4);
  usleep(10 * 1000);
  theSynthesizer->set(4, M_G_4);
  theSynthesizer->set(5, M_G_4);
  usleep(10 * 1000);
  theSynthesizer->set(6, M_C_5);
  theSynthesizer->set(7, M_C_5);

  sleep(2);

  theSynthesizer->set(0, 0);
  theSynthesizer->set(1, 0);
  usleep(10 * 1000);
  theSynthesizer->set(2, 0);
  theSynthesizer->set(3, 0);
  usleep(10 * 1000);
  theSynthesizer->set(4, 0);
  theSynthesizer->set(5, 0);
  usleep(10 * 1000);
  theSynthesizer->set(6, 0);
  theSynthesizer->set(7, 0);

  sleep(1);
}
