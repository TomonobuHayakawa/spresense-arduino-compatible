/*
 *  IIR.cpp - IIR(biquad cascade) Library
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

#include "IIR.h"

bool IIRClass::begin(filterType_t type, int channel, int cutoff, float q)
{
  begin(type, channel, cutoff, q, DEFAULT_FRAMSIZE, Planar);
}

bool IIRClass::begin(filterType_t type, int channel, int cutoff, float q, int sample, format_t output)
{
  if(channel > MAX_CHANNEL_NUM) return false;
  if(sample < MIN_FRAMSIZE) return false;

  m_channel = channel;
  m_framesize = sample;
  for(int i=0;i<m_channel;i++){
    m_ringbuff[i] = new RingBuff(channel*sizeof(q15_t)*sample*3);
    if(!m_ringbuff[i]) return false;
  }

  /* Temporary buffer */
  m_tmpInBuff  = new float[m_framesize];
  m_tmpOutBuff = new float[m_framesize];

  if(create_coef(type, cutoff, q) == false){
    return false;
  }

  for (int i = 0; i < channel; i++) {
    arm_biquad_cascade_df2T_init_f32(&S[i],1,m_coef,m_buffer);
  }

  return true;
}

void IIRClass::end()
{
  for(int i=0;i<m_channel;i++){
    delete m_ringbuff[i];
  }
  delete m_tmpInBuff;
  delete m_tmpOutBuff;
}

bool IIRClass::create_coef(filterType_t type, int cutoff, float q)
{
  float w,k0,k1,a0,a1,a2,b0,b1,b2;

  w = 2.0f * PI * cutoff / 48000;

  a1 = -2.0f * cos(w);

  switch(type){
  case (TYPE_LPF):
  case (TYPE_HPF):
    k0 = sin(w) / (2.0f * q);

    a0 =  1.0f + k0;
    a2 =  1.0f - k0;

    break;
  case (TYPE_BPF):
  case (TYPE_BEF):
    k0 = sin(w) * sinh(log(2.0f) / 2.0 * q * w / sin(w));
    a0 =  1.0f + k0;
    a2 =  1.0f - k0;
    break;
  default:
    return false;
  }

  switch(type){
  case TYPE_LPF:
    k1 = 1.0f - cos(w);
    b0 = k1 / 2.0f;
    b1 = k1;
    b2 = k1 / 2.0f;
    break;
  case TYPE_HPF:
    k1 = 1.0f + cos(w);
    b0 = k1 / 2.0f;
    b1 = -k1;
    b2 = k1 / 2.0f;
    break;
  case TYPE_BPF:
    b0 =  k0;
    b1 =  0.0f;
    b2 = -k0;
    break;
  case TYPE_BEF:
    b0 =  1.0f;
    b1 = -2.0f * cos(w);
    b2 =  1.0f;
    break;
  default:
    return false;
  }

  m_coef[0] = b0/a0;
  m_coef[1] = b1/a0;
  m_coef[2] = b2/a0;
  m_coef[3] = -(a1/a0);
  m_coef[4] = -(a2/a0);

  return true;
}


bool IIRClass::put(q15_t* pSrc, int sample)
{
  /* Ringbuf size check */
  if(m_channel > MAX_CHANNEL_NUM) return false;
  if(sample > m_ringbuff[0]->remain()) return false;

  if (m_channel == 1) {
    /* the faster optimization */
    m_ringbuff[0]->put((q15_t*)pSrc, sample);
  } else {
    for (int i = 0; i < m_channel; i++) {
      m_ringbuff[i]->put(pSrc, sample, m_channel, i);
    }
  }
  return  true;
}

bool IIRClass::empty(int channel)
{
   return (m_ringbuff[channel]->stored() < m_framesize);
}

int IIRClass::get(q15_t* pDst, int channel)
{

  if(channel >= m_channel) return false;
  if (m_ringbuff[channel]->stored() < m_framesize) return 0;

  /* Read from the ring buffer */
  m_ringbuff[channel]->get(m_tmpInBuff, m_framesize);

  arm_biquad_cascade_df2T_f32(&S[channel], m_tmpInBuff, m_tmpOutBuff, m_framesize);
  arm_float_to_q15(m_tmpOutBuff, pDst, m_framesize);

  return m_framesize;
}
