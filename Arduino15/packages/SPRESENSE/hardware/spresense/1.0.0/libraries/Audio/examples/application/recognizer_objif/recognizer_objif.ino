/*
 *  recognizer_objIf.ino - Object I/F based sound recognizer example application
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,  MA 02110-1301  USA
 */

#include <SDHCI.h>
#include <FrontEnd.h>
#include <Recognizer.h>
#include <MemoryUtil.h>

#define ANALOG_MIC_GAIN  0 /* +0dB */

SDClass theSD;
FrontEnd *theFrontend;
Recognizer *theRecognizer;

bool ErrEnd = false;

/**
 * @brief Audio attention callback
 *
 * When audio internal error occurc, this function will be called back.
 */

static void recognizer_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}

/* Number of input channels
 * Set either 1, 2, or 4.
 */

static const uint8_t  recognizer_cannel_number = 1;

/* Audio bit depth
 * Set 16 or 24
 */

static const uint8_t  recognizer_bit_length = 16;

/* recognizer time[second] */

static const uint32_t recognizer_time = 10;

/* Buffer size
 * Align in 512byte units based on frame size.
 */


/**
 * @brief Recorder done callback procedure
 *
 * @param [in] event        AsRecorderEvent type indicator
 * @param [in] result       Result
 * @param [in] sub_result   Sub result
 *
 * @return true on success, false otherwise
 */
static bool frontend_done_callback(AsMicFrontendEvent ev, uint32_t result, uint32_t sub_result)
{
  return true;
}

static void recognizer_done_callback(RecognizerResult *result)
{
  printf("recognizer cb\n");

  return;
}

static void recognizer_find_callback(AsRecognitionInfo info)
{
  printf("recognizer find cb\n");
  return;
}

/**
 * @brief Setup Recorder
 *
 * Set input device to Mic <br>
 * Initialize recorder to encode stereo mp3 stream with 48kHz sample rate <br>
 * System directory "/mnt/sd0/BIN" will be searched for MP3 encoder (MP3ENC file)
 * Open "Sound.mp3" file <br>
 */

void setup()
{
  theSD.begin();

  /* Initialize memory pools and message libs */

  initMemoryPools();
  createStaticPools(MEM_LAYOUT_RECOGNIZER);

  /* start audio system */

  theFrontend = FrontEnd::getInstance();
  theRecognizer = Recognizer::getInstance();

  theFrontend->begin();
  theRecognizer->begin(recognizer_attention_cb);

  puts("Initialize Recognizer");

  /* Activate Objects. Set output device to Speakers/Headphones */

  theFrontend->activate(frontend_done_callback);
  theRecognizer->activate(recognizer_done_callback);

  usleep(100 * 1000); /* Waiting for Mic startup */

  /*
   * Initialize main player to decode stereo mp3 stream with 48 kb/s sample rate
   * Search for MP3 codec in "/mnt/sd0/BIN" directory
   */

  theFrontend->init(recognizer_cannel_number, recognizer_bit_length, 384, ObjectConnector::ConnectToRecognizer);

  theRecognizer->init(recognizer_find_callback,
                      "/mnt/sd0/BIN/RCGPROC");

  usleep(200 * 1000); /* Waiting for recognizer initialize */

  /* Init Recognizer DSP */
  
  struct InitRcgProc : public CustomprocCommand::CmdBase
  {
    uint32_t ch_num;
    uint32_t bit_width;
    uint32_t reserve0;
    uint32_t reserve1;
  };

  static InitRcgProc s_initrcgproc;
  s_initrcgproc.ch_num = recognizer_cannel_number;
  s_initrcgproc.bit_width = recognizer_bit_length / 2;
  
  theRecognizer->initRecognizerDsp((uint8_t *)&s_initrcgproc, sizeof(s_initrcgproc));

  /* Set Recognizer DSP */
  
  struct SetRcgProc : public CustomprocCommand::CmdBase
  {
    uint32_t enable;
    uint32_t reserve0;
    uint32_t reserve1;
    uint32_t reserve2;
  };

  static SetRcgProc s_setrcgproc;
  s_setrcgproc.enable = true;
  
  theRecognizer->setRecognizerDsp((uint8_t *)&s_setrcgproc, sizeof(s_setrcgproc));

  /* Start Recorder */

  theFrontend->start();
  theRecognizer->start();
  puts("Recognizing Start!");

}

/**
 * @brief Record audio frames
 */

void loop()
{
  static int time = 0;
  printf("loop %d\n", time);

  sleep(1);
  
  /* Stop Recognize */
  
  if (time++ > 10)
    {
      theFrontend->stop();
      theRecognizer->stop();
      sleep(1);

      goto exitRecognize;
    }

  if (ErrEnd)
    {
      printf("Error End\n");
      theRecognizer->stop();
      goto exitRecognize;
    }

  return;

exitRecognize:

  theFrontend->deactivate();
  theRecognizer->deactivate();
  theFrontend->end();
  theRecognizer->end();
  
  puts("End Recognize");
  exit(1);
}
