/*
 *  IIR.h - IIR(biquad cascade) Library Header
 *  Copyright 2019, 2021 Sony Semiconductor Solutions Corporation
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

#ifndef _IIR_H_
#define _IIR_H_

/**
 * @file IIR.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief SignalProcessing Library for Arduino
 */

/**
 * @defgroup signalprocessing  SignalProcessing Library API
 * @brief API for using SignalProcessing
 * @{
 */

/* Use CMSIS library */
#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <cmsis/arm_math.h>

#include "RingBuff.h"

/*------------------------------------------------------------------*/
/* Configurations                                                   */
/*------------------------------------------------------------------*/

/** @def
 * The bit length definition (Only Support 16bit)
 */
#define BITLEN 16
//#define BITLEN 32

/** @def
 * The default number of samples in an execution frame
 */
#define DEFAULT_FRAMSIZE 768

/** @def
 * The minimum number of samples in an execution frame
 */
#define MIN_FRAMSIZE 240

/* An execution sample of frame */
#define FRAMSIZE DEFAULT_FRAMSIZE

/** @def
 * The Maximum number of channels
 */
#define MAX_CHANNEL_NUM 8

/*------------------------------------------------------------------*/
/* Type Definition                                                  */
/*------------------------------------------------------------------*/
/**
 * @enum filterType_t
 * The definition of filter types
 */
typedef enum e_filterType {
  //! Low Pass Filter
  TYPE_LPF,
  //! High Pass Filter
  TYPE_HPF,
  //! Band Pass Filter
  TYPE_BPF,
  //! Band Elimination Filter
  TYPE_BEF
} filterType_t;

/*------------------------------------------------------------------*/
/* IIR Class                                                        */
/*------------------------------------------------------------------*/
/**
 * @class IIRClass
 *
 * @brief Biquad IIR filter class
 */
class IIRClass
{
public:

  IIRClass() {
    for (int i = 0; i < MAX_CHANNEL_NUM; i++) {
      m_ringbuff[i] = NULL;
    }
    m_tmpInBuff = NULL;
    m_tmpOutBuff = NULL;
    m_InterleaveBuff = NULL;
  }

  /**
   * @enum format_t
   * The output data format (In the class scope)
   */
  typedef enum e_format {
    //! the channel interleave format
    Interleave,
    //! the channel planar format
    Planar
  } format_t;

  /**
   * @enum error_t
   * The error codes (In the class scope)
   */
  typedef enum e_error {
    //! No error
    ERR_OK = 0,
    //! Wrong channel setting
    ERR_CH_NUM = -1,
    //! Wrong output format setting
    ERR_FORMAT = -2,
    //! Lack of memory area
    ERR_MEMORY = -3,
    //! Wrong filter type setting
    ERR_FILTER_TYPE = -4,
    //! Wrong number of samples
    ERR_FRAME_SIZE = -5,
    //! Failture of write as buffer is full
    ERR_BUF_FULL = -6,
    //! Wrong sampling rate
    ERR_FS = -7
  } error_t;


  /**
   * @brief   Initialize the IIR library.
   *
   * @return  OK(true) or Failure(false)
   * @details This function is called only once when using the IIR library.
   *
   */
  bool begin(
    filterType_t type,  /**< The execution filter type */
    int channel,        /**< The number of channels */
    int cutoff,         /**< The cutoff frequency(BPF/HPF) or the center frequency(BPF/BEF) */
    float q,            /**< The Q value(BPF/HPF) or the bandwidth[octave](BPF/BEF) */
    int sample = DEFAULT_FRAMSIZE,   /**< The number of samples in an execution filter(default size is DEFAULT_FRAMSIZE) */
    format_t output = Planar,        /**< The output format(default is Planar) */
    int fs = 48000      /**< The Sampling rate */
  );

  /**
   * @brief   Put input data into the IIR library
   *
   * @return  OK(true) or Failure(false)
   * @details Put input data into the IIR library. Multi-channel input data support interleave format.
   *
   */
  bool put(
    q15_t* pSrc, /**< The pointer of input data address */
    int size     /**< The number of input data sample */
  );

  /**
   * @brief   Get the execution data of each channel
   *
   * @return  The size of an execution data sample(Error code when negative numbers)
   * @details Get the execution data of each channel. This API can be called only for Planar format.
   *
   */
  int  get(
    q15_t* pDst, /**< The pointer of area that output data is written */
    int channel  /**< The each channel number of the execution data */
  );

  /**
   * @brief   Get the execution data of all channels
   *
   * @return  The size of execution data sample(Error code when negative numbers)
   * @details Get the execution data of all channels. This API can be called only for Interleave format.
   *
   */
  int  get(
    q15_t* pDsts /**< The pointer of area that output data is written */
  );

  /**
   * @brief Finalize the IIR library.
   *
   * @details This function is called when you want to exit the IIR library.
   *
   */
  void end();

  /**
   * @brief Is the buffer empty or not of each channel
   *
   * @return  Empty(true) or Not empty(false)
   * @details Is the buffer empty or not of each channel.
   *
   */
  bool empty(
    int channel /**< The channel number that you want to check */
  );

  /**
   * @brief Get error information
   *
   * @return  Error code[IIRClass::error_t]
   * @details When an error occurs, you call this function and get error cause information.
   *
   */
  error_t getErrorCause(){ return m_err; }


private:

  int      m_channel;
  int      m_framesize;
  format_t m_output;
  error_t  m_err;
  int      m_fs;

  arm_biquad_cascade_df2T_instance_f32 S[MAX_CHANNEL_NUM];

  float32_t m_coef[5];
  float32_t m_buffer[4];

  RingBuff* m_ringbuff[MAX_CHANNEL_NUM];

  /* Temporary buffer */
  float* m_tmpInBuff;
  float* m_tmpOutBuff;

  q15_t* m_InterleaveBuff;

  bool create_coef(filterType_t, int cutoff, float q);

};

#endif /*_IIR_H_*/
