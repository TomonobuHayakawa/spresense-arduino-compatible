/*
 *  Recognizer.h - Audio include file for the Spresense SDK
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

/**
 * @file Recognizer.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief Recognizer Class for Arduino on Spresense.
 * @details By using this library, you can use the follow features on SPRESSENSE.
 *          - Voice recording
 */

#ifndef Recognizer_h
#define Recognizer_h

#ifdef SUBCORE
#error "Audio library is NOT supported by SubCore."
#endif

#include <audio/audio_high_level_api.h>
#include <audio/audio_message_types.h>
#include <audio/utilities/frame_samples.h>
#include <audio/utilities/wav_containerformat.h>
#include <audio/dsp_framework/customproc_command_base.h>
#include <memutils/simple_fifo/CMN_SimpleFifo.h>

/*--------------------------------------------------------------------------*/

/**
 * Recognizer log output definition
 */

#define RECOGNIZER_LOG_DEBUG

#define print_err printf

#ifdef RECOGNIZER_LOG_DEBUG
#define print_dbg(...) printf(__VA_ARGS__)
#else
#define print_dbg(x...)
#endif

/**
 * Recognizer Error Code Definitions.
 */

#define RECOGNIZER_ECODE_OK            0
#define RECOGNIZER_ECODE_COMMAND_ERROR 1
#define RECOGNIZER_ECODE_BASEBAND_ERROR 2

/**
 * MediaRecorder capturing clock mode.
 */

#define RECOGNIZER_CAPCLK_NORMAL (0)
#define RECOGNIZER_CAPCLK_HIRESO (1)

/*--------------------------------------------------------------------------*/

/**
 * @class Recognizer
 * @brief Recognizer Class Definitions.
 */

class Recognizer
{
public:

  /**
   * @brief Get instance of Recognizer for singleton.
   */

  static Recognizer* getInstance()
    {
      static Recognizer instance;
      return &instance;
    }

  /**
   * @brief Initialize the Recognizer.
   *
   * @details This function is called only once when using the Recognizer.
   *          In this function, creat objects for recognizing.
   *
   */

  err_t begin(void);

  /**
   * @brief Initialize the Recognizer.
   *
   * @details This function can set callback funtion which receive attention notify.
   *
   */

  err_t begin(
      AudioAttentionCb attcb /**< Attention callback  */
  );

  /**
   * @brief Finalize the Recognizer.
   *
   * @details This function is called only once when finish the Recognizer.
   *
   */

  err_t end(void);

  /**
   * @brief Activate the Recognizer.
   *
   * @details This function activates frontend system and audio HW.
   *          You can specify pre-process type.
   *
   */

  err_t activate(void);

  /**
   * @brief Activate the Recognizer.
   *
   * @details This function activates recognizer system.
   *          If you activate Recognizer by this API, The result of all APIs
   *          will be returnd by callback function which is specified by this function.
   *          (It means that the return of API is not completion of internal process)
   *
   */

  err_t activate(
      RecognizerCallback recognizercb /**< Recognizer event complete callback */
  );

  /**
   * @brief Initialize the Recognizer
   *
   * @details This is full version on initialize API.
   *          In this API, you can should set all of initialize parameters.
   *          Before you start Recognizer, you must initialize by this API.
   *
   */

  err_t init(
    RecognizerNotifyCallback findcb, /**< Recognition callback */
    const char *dsp_path             /**< Path to recognizer DSP */
  );

  /**
   * @brief Start Recognizer
   *
   * @details This function starts Recognizer.
   *          Once you call this function, the recognizer will be in active state.
   *          Recognizer can accept input data and take recognition process.
   *          This will continue until you call "stop API".
   *
   */

  err_t start(void);

  /**
   * @brief Stop Recording
   *
   * @details This function stops Recognizer.
   *          You can call this API when Recognizer is activate.
   *
   */

  err_t stop(void);

  /**
   * @brief Send init command to recognizer DSP.
   *
   * @details This function sends Init command to recognizer DSP.
   *
   */

  err_t initRecognizerDsp(
    uint8_t *packet_addr, /**< Address of init command packet */
    uint32_t packet_size  /**< Size of init command packet */
  );

  /**
   * @brief Send set command to recognizer DSP.
   *
   * @details This function sends Set command to recognizer DSP.
   *
   */

  err_t setRecognizerDsp(
    uint8_t *packet_addr, /**< Address of set command packet */
    uint32_t packet_size  /**< Size of set command packet */
  );

  /**
   * @brief Deactivate the Recognizer
   *
   * @details This function deactivates Recognizer system.
   *
   */

  err_t deactivate(void);

private:

  /**
   * To avoid create multiple instance
   */

  Recognizer()
    : m_recognizer_callback(NULL)
  {}
  Recognizer(const Recognizer&);
  Recognizer& operator=(const Recognizer&);
  ~Recognizer() {}

  /**
   * Private members
   */

  RecognizerCallback m_recognizer_callback;

  bool receiveReply(RecognizerResult *reply);
};

#endif // Recognizer_h

