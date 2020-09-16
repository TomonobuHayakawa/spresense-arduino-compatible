/*
 *  Recognizer.cpp - Recognizer implement file for the Spresense SDK
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

//***************************************************************************
// Included Files
//***************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arch/board/cxd56_audio.h>

#include "Recognizer.h"
#include "MemoryUtil.h"


/*--------------------------------------------------------------------------*/
extern "C" {

static void attentionCallback(const ErrorAttentionParam *attparam)
{
  print_err("Attention!! Level 0x%x Code 0x%x\n", attparam->error_code, attparam->error_att_sub_code);
}

}

/****************************************************************************
 * Public API on Recognizer Class
 ****************************************************************************/

err_t Recognizer::begin(void)
{
  return begin(NULL);
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::begin(AudioAttentionCb attcb)
{
  /* Create Recognizer. */

  AsCreateRecognizerParam_t recognizer_create_param;
  recognizer_create_param.msgq_id.recognizer = MSGQ_AUD_RECOGNIZER;
  recognizer_create_param.msgq_id.mng        = MSGQ_AUD_MGR;
  recognizer_create_param.msgq_id.dsp        = MSGQ_AUD_RCGDSP;
  recognizer_create_param.pool_id.out        = S0_OUTPUT_BUF_POOL;
  recognizer_create_param.pool_id.dsp        = S0_RCG_APU_CMD_POOL;

  bool result = AS_CreateRecognizer(&recognizer_create_param, (attcb) ? attcb : attentionCallback);
  if (!result)
    {
      print_err("Error: AS_CreateRecognizer() failure!\n");
      return RECOGNIZER_ECODE_COMMAND_ERROR;
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::end(void)
{
  /* Delete Recognizer */

  bool result = AS_DeleteRecognizer();
  if (!result)
    {
      print_err("Error: AS_DeleteFrontend() failure!\n");
      return RECOGNIZER_ECODE_COMMAND_ERROR;
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::activate(void)
{
  return activate(NULL);
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::activate(RecognizerCallback recognizercb)
{
  /* Activate Recognizer */

  RecognizerCommand recognizer_command;
  recognizer_command.act_param.cb = recognizercb;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_ACT,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  m_recognizer_callback = recognizercb;

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::init(RecognizerNotifyCallback findcb, const char *dsp_path)
{
  /* Init Recognizer */

  RecognizerCommand recognizer_command;
  recognizer_command.init_param.type        = AsRecognizerTypeUserCustom;
  snprintf(recognizer_command.init_param.dsp_path,
            sizeof(recognizer_command.init_param.dsp_path),
            "%s", dsp_path);
  recognizer_command.init_param.notify_path = AsNotifyPathCallback;
  recognizer_command.init_param.dest.cb     = findcb;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_INIT,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::start(void)
{
  /* Start Recognizer */

  RecognizerCommand recognizer_command;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_START,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::stop(void)
{
  /* Stop Recognizer */

  RecognizerCommand recognizer_command;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_STOP,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::initRecognizerDsp(uint8_t *packet_addr, uint32_t packet_size)
{
  /* Init Recognizer DSP */

  RecognizerCommand recognizer_command;

  recognizer_command.initrcgproc_param.packet_addr = packet_addr;
  recognizer_command.initrcgproc_param.packet_size = packet_size;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_INITRCGPROC,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::setRecognizerDsp(uint8_t *packet_addr, uint32_t packet_size)
{
  /* Set Recognizer DSP */

  RecognizerCommand recognizer_command;

  recognizer_command.setrcgproc_param.packet_addr = packet_addr;
  recognizer_command.setrcgproc_param.packet_size = packet_size;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_SETRCGPROC,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
err_t Recognizer::deactivate(void)
{
  /* Deactivate Recognizer */

  RecognizerCommand recognizer_command;

  err_t er = MsgLib::send<RecognizerCommand>(MSGQ_AUD_RECOGNIZER,
                                             MsgPriNormal,
                                             MSG_AUD_RCG_DEACT,
                                             NULL,
                                             recognizer_command);
  F_ASSERT(er == ERR_OK);

  if (!m_recognizer_callback)
    {
      RecognizerResult reply_info;
      bool result = receiveReply(&reply_info);
      if (!result)
        {
          print_err("Error: receiveReply() failure!\n");
          return RECOGNIZER_ECODE_COMMAND_ERROR;
        }
    }

  return RECOGNIZER_ECODE_OK;
}

/*--------------------------------------------------------------------------*/
bool Recognizer::receiveReply(RecognizerResult *reply)
{
  err_t           err_code;
  FAR MsgQueBlock *que;
  FAR MsgPacket   *msg;

  if (reply == NULL)
    {
      return false;
    }

  /* Get an instance of the specified message ID. */

  err_code = MsgLib::referMsgQueBlock(MSGQ_AUD_MGR, &que);
  if (err_code != ERR_OK)
    {
      return false;
    }

  /* Waiting to receive a message. */

  err_code = que->recv(TIME_FOREVER, &msg);
  if (err_code != ERR_OK)
    {
      return false;
    }

  if (msg->getType() != MSG_AUD_MGR_RST)
    {
      return false;
    }

  /* Store reply information. */

  *reply = msg->moveParam<RecognizerResult>();

  /* Delete received data. */

  err_code = que->pop();
  if (err_code != ERR_OK)
    {
      return false;
    }

  return true;
}


