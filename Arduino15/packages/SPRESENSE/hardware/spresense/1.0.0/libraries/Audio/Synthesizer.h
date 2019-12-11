/*
 *  Synthesizer.h - Audio include file for the Spresense SDK
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

/**
 * @file Synthesizer.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief Mediat Player Class for Arduino on Spresense.
 */

#ifndef Synthesizer_h
#define Synthesizer_h

#ifdef SUBCORE
#error "Audio library is NOT supported by SubCore."
#endif

// #ifdef __cplusplus

#include <audio/audio_synthesizer_api.h>

/*--------------------------------------------------------------------------*/

/**
 * Synthesizer log output definition
 */
#define SYNTHESIZER_LOG_DEBUG

#define print_err printf

#ifdef SYNTHESIZER_LOG_DEBUG
#define print_dbg(...) printf(__VA_ARGS__)
#else
#define print_dbg(x...)
#endif

/**
 * Synthesizer Error Code Definitions.
 */

#define SYNTHESIZER_ECODE_OK            0
#define SYNTHESIZER_ECODE_COMMAND_ERROR 1
#define SYNTHESIZER_ECODE_SIMPLEFIFO_ERROR 2
#define SYNTHESIZER_ECODE_FILEACCESS_ERROR 3
#define SYNTHESIZER_ECODE_FILEEND 4
#define SYNTHESIZER_ECODE_SHARED_MEMORY_ERROR 5
#define SYNTHESIZER_ECODE_WAV_PARSER_ERROR 6
#define SYNTHESIZER_ECODE_BUFFERSIZE_ERROR 7
#define SYNTHESIZER_ECODE_BUFFERALLOC_ERROR 8

#define MEDIAPLAYER_BUF_FRAME_NUM  8
#define MEDIAPLAYER_BUF_FRAME_SIZE 6144
#define MEDIAPLAYER_BUF_SIZE (MEDIAPLAYER_BUF_FRAME_NUM * MEDIAPLAYER_BUF_FRAME_SIZE)

/*--------------------------------------------------------------------------*/

/**
 * @class Synthesizer
 * @brief Synthesizer Class Definitions.
 */

class Synthesizer
{
public:

  /**
   * @brief Get instance of MediaRecorder for singleton.
   */

  static Synthesizer* getInstance()
    {
      static Synthesizer instance;
      return &instance;
    }

   /**
   * @enum PlayerId
   *
   * @brief Audio library allows you to use two players simultaneously.
   *        Please set Player ID that player instance id created to use.
   */

  typedef enum
  {
    Player0,  /**< Indicates Player0 */
    Player1   /**< Indicates Player1 */
  } PlayerId;

  /**
   * @brief Initialize the Synthesizer.
   *
   * @details This function is called only once when using the Synthesizer.
   *          In this function, alloc memory area of FIFO for ES data supply.
   *
   */

  err_t begin(void);


  /**
   * @brief Creation of the Synthesizer.
   *
   * @details This function can set callback funtion which receive attention notify.
   *
   */

  err_t create(
      AudioAttentionCb attcb /**< Attention callback */
  );

  /**
   * @brief Activate the Synthesizer
   *
   * @details This function activates media player system.
   *          The result of APIs will be returnd by callback function which is
   *          specified by this function.
   *
   */

  err_t activate(
      SynthesizerCallback mpcb, /**< Sepcify callback function which is called to notify API results. */
      void *param
  );

  /**
   * @brief Initialize the Synthesizer (Abridged version).
   *
   * @details This is abridged version on initialize API.
   *          You can init media player with codec type, sampling rate, and number of channels.
   *          When this API is called, other parameters are fixed as below.
   *
   *          Bit length      : 16bit
   *          DSP binary path : /mnt/sd0/BIN
   *
   */

  err_t init(
      AsSynthesizerWaveMode type,
      uint8_t channel_num,
      uint32_t sampling_rate,
      uint8_t bit_width,
      const char *dsp_path
  );

  err_t set(
      uint8_t channel_no,
      uint32_t frequency
  );

  /**
   * @brief Start playing.
   *
   * @details This function starts playing audio data.
   *          When you call this API, player system pull out audio data from FIFO.
   *          So, you have to push audio data which will play into FIFO by "write frame API".
   *          Decode completion and decodec data will be notified by callback function which is
   *          specified by this API. You can process them.
   *
   */

  err_t start(void);

  /**
   * @brief Stop playing.
   *
   * @details This function stops playing audio data.
   *          You can call this API when playing is activate.
   *          When you call this API, return of API will back soon but internal work
   *          cannot stop immediately. So, after API call, decodec data will be returned
   *          to application while supplied data runs out. It is better to process them.
   *
   */

  err_t stop(void);

  /**
   * @brief Deactivate the Synthesizer
   *
   * @details This function deactivates media player system.
   *
   */

  err_t deactivate(void);

  err_t end(void);

private:

  /**
   * To avoid create multiple instance
   */

  Synthesizer() {}
  Synthesizer(const Synthesizer&);
  Synthesizer& operator=(const Synthesizer&);
  ~Synthesizer() {}
};

// #endif // __cplusplus
#endif // Synthesizer_h

