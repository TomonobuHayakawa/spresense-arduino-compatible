/*
 *  ObjectConnector.h - Audio include file for the Arduino on Spresense.
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
 * @file ObjcectConnector.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief ObjcectConnector Class for Arduino on Spresense.
 */

#ifndef ObjectConnector_h
#define ObjectConnector_h

#ifdef SUBCORE
#error "Audio library is NOT supported by SubCore."
#endif

#include <audio/audio_message_types.h>
#include "MemoryUtil.h"

/*--------------------------------------------------------------------------*/

class ObjectConnector
{
public:

  /**< Connection Destination */

  enum Destination
  {
    ConnectToMediaRecorder = 0,
    ConnectToRecognizer,
    ConnectToOutputMixer,
    ConnectToApplication,
  };

  ObjectConnector() {}
  ~ObjectConnector() {}

  static AsDataDest connect(Destination dest)
  {
    AsDataDest ret = { 0 };

    switch (dest)
      {
        case ConnectToMediaRecorder:
          ret.msg.msgqid  = MSGQ_AUD_RECORDER;
          ret.msg.msgtype = MSG_AUD_MRC_CMD_ENCODE;
          break;

        case ConnectToRecognizer: 
          ret.msg.msgqid  = MSGQ_AUD_RECOGNIZER;
          ret.msg.msgtype = MSG_AUD_RCG_EXEC;
          break;

        case ConnectToOutputMixer: 
          ret.msg.msgqid  = MSGQ_AUD_OUTPUT_MIX;
          ret.msg.msgtype = MSG_AUD_MIX_CMD_DATA;
          break;

        case ConnectToApplication: 
          ret.msg.msgqid  = MSGQ_AUD_OUTPUT_MIX;
          ret.msg.msgtype = MSG_AUD_MIX_CMD_DATA;
          break;

      	default:
          ret.msg.msgqid  = MSGQ_AUD_RECORDER;
          ret.msg.msgtype = MSG_AUD_MRC_CMD_ENCODE;
          break;
      }

    return ret;
  }
};

#endif // ObjectConnector_h

