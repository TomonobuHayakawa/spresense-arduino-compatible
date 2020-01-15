/*
 *  oscillator.ino - oscillator example application
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
#include <Audio.h>
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
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

SDClass theSD;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Publuc Functions
 ****************************************************************************/

void setup()
{
  puts("Initialization Audio Library");

  /* Get static audio instance */

  AudioClass  *theAudio = AudioClass::getInstance();

  /* Start audio system */

  theAudio->begin();

  /* Mount SD card */

  theSD.begin();

  /* Activate synthesizer */

  theAudio->setSynthesizer(true);

  /* Initialize PostProcess DSP. */

  InitParam initpost;

  initpost.input_channel_num = OSC_CH_NUM;

  theAudio->initPostProcessDsp(&initpost, sizeof(initpost));

  /* Initialization synthesizer */

  theAudio->initSynthesizer(AsSynthesizerSinWave,
                            OSC_CH_NUM,
                            DSPBIN_PATH,
                            OSC_EFFECT_ATTACK,
                            OSC_EFFECT_DECAY,
                            OSC_EFFECT_SUSTAIN,
                            OSC_EFFECT_RELEASE);

  /* Set master volume */

  theAudio->setVolume(VOLUME_MASTER);

  /* Start synthesizer */

  theAudio->startSynthesizer();
}

/* ------------------------------------------------------------------------ */
void loop()
{
  AudioClass  *theAudio = AudioClass::getInstance();

  puts("loop!!");

  /* Set frequency */

  theAudio->setFreqSynthesizer(0, M_C_4);
  theAudio->setFreqSynthesizer(1, M_C_4);

  sleep(1);

  theAudio->setFreqSynthesizer(0, M_E_4);
  theAudio->setFreqSynthesizer(1, M_E_4);

  sleep(1);

  theAudio->setFreqSynthesizer(0, M_G_4);
  theAudio->setFreqSynthesizer(1, M_G_4);

  sleep(1);

  theAudio->setFreqSynthesizer(0, M_C_4);
  theAudio->setFreqSynthesizer(1, M_C_4);
  theAudio->setFreqSynthesizer(2, M_E_4);
  theAudio->setFreqSynthesizer(3, M_E_4);
  theAudio->setFreqSynthesizer(4, M_G_4);
  theAudio->setFreqSynthesizer(5, M_G_4);
  theAudio->setFreqSynthesizer(6, M_C_5);
  theAudio->setFreqSynthesizer(7, M_C_5);

  sleep(2);

  theAudio->setFreqSynthesizer(0, 0);
  theAudio->setFreqSynthesizer(1, 0);
  theAudio->setFreqSynthesizer(2, 0);
  theAudio->setFreqSynthesizer(3, 0);
  theAudio->setFreqSynthesizer(4, 0);
  theAudio->setFreqSynthesizer(5, 0);
  theAudio->setFreqSynthesizer(6, 0);
  theAudio->setFreqSynthesizer(7, 0);

  sleep(1);
}
