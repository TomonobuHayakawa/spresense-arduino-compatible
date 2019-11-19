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

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* DSP file path */

#define DSPBIN_PATH           "/mnt/sd0/BIN/OSCPROC"

/* Maximum number of channels */

#define CHANNEL_NUMBER        8

/* Oscillator parameter */

#define OSC_CH_NUM            CHANNEL_NUMBER
#define OSC_BIT_LEN           AS_BITLENGTH_16
#define OSC_SAMPLING_RATE     AS_SAMPLINGRATE_48000

/* Set volume[db] */

#define VOLUME_MASTER         -160

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

  /* Set rendering clock */

  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);

  /* Activate synthesizer */

  theAudio->setSynthesizer();

  /* Initialization synthesizer */

  theAudio->initSynthesizer(DSPBIN_PATH, OSC_CH_NUM, OSC_SAMPLING_RATE, OSC_BIT_LEN);

  /* Set master volume */

  theAudio->setVolume(VOLUME_MASTER, VOLUME_MASTER, VOLUME_MASTER);

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

  sleep(2);

  theAudio->setFreqSynthesizer(0, 0);
  theAudio->setFreqSynthesizer(1, 0);
  theAudio->setFreqSynthesizer(2, 0);
  theAudio->setFreqSynthesizer(3, 0);
  theAudio->setFreqSynthesizer(4, 0);
  theAudio->setFreqSynthesizer(5, 0);
}
