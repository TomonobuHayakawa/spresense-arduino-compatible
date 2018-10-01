/*
 *  sound_fft.ino - PCM capture (Sound) and FFT example application
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

#include <Audio.h>
#include <SD.h>

#include "dsp_rpc.h"

AudioClass *theAudio;
File myFile;

const int32_t recoding_frames = 2000;
const int32_t buffer_sample = 768;
//const int32_t buffer_sample = 256;
const int32_t buffer_size = buffer_sample*sizeof(int16_t);

char buffer[buffer_size];
float buffer_f[buffer_sample*2];


//#define MAX_BLOCKSIZE   128
#define MAX_BLOCKSIZE   256
//#define MAX_BLOCKSIZE   512
#define DSPMATH_LIBFILE "/mnt/sd0/BIN/DSPFFT"

#define POWER_THRESHOLD   20
#define LENGTH_THRESHOLD   2
#define INTERVAL_THRESHOLD   10


bool detect_sound(int bottom, int top, float32_t* pdata )
{
  static int continuity = 0;
  static int interval = 0;

  if(bottom > top) return false;

  if(interval> 0){
    interval--;
    continuity=0;
    return false;
  }

  for(int i= bottom;i<=top;i++){
    if(*(pdata+i) > POWER_THRESHOLD){
//      printf("!!%2.8f\n",*(pdata+i));
      continuity++;
//      printf("con=%d\n",continuity);
      if(continuity > LENGTH_THRESHOLD){
        interval = INTERVAL_THRESHOLD;
        return true;
      }else{
        puts("break");
        return false;  
      }
    }
  }
  continuity=0;
  return false;  
}


/**
 *  @brief Setup audio device to capture PCM stream
 *
 *  Select input device as analog microphone, AMIC  <br>
 *  Set PCM capture sapling rate parameters to 48 kb/s <br>
 *  Set channel number 4 to capture audio from 4 microphones simultaneously <br>
 *  System directory "/mnt/sd0/BIN" will be searched for PCM codec
 */
void setup()
{
  theAudio = AudioClass::getInstance();
  theAudio->begin();

  SD.begin();
//  myFile = SD.open("Sound.pcm", FILE_WRITE); // temtative for SD car initialize. And Debug.

  puts("initialization Audio Library");

  load_library(DSPMATH_LIBFILE);
  init_fft_f32( MAX_BLOCKSIZE, 0, 1); /* BLOCKNUM, FFT, Bit Reverse */

  /* Select input device as AMIC */
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC,180);

  /*
   * Set PCM capture sapling rate parameters to 48 kb/s. Set channel number 4
   * Search for PCM codec in "/mnt/sd0/BIN" directory
   */
//  theAudio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_48000, AS_CHANNEL_MONO);
  theAudio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_16000, AS_CHANNEL_MONO);
  puts("Init Recorder!");

  puts("Rec!");
  theAudio->startRecorder();
}

/**
 * @brief Capture frames of PCM data into buffer
 */
float32_t output[MAX_BLOCKSIZE];

void loop() {

  static int cnt = 0;
  uint32_t read_size;

  /* recording end condition */
  if (cnt > recoding_frames)
    {
      puts("End Recording");
      theAudio->stopRecorder();
      myFile.close();
      exit(1);
    }

  /* Read frames to record in buffer */
  int err = theAudio->readFrames(buffer, buffer_size, &read_size);

  if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA)
    {
      printf("Error End! =%d\n",err);
      sleep(1);
      theAudio->stopRecorder();
      exit(1);
    }

  /* The actual signal processing will be coding here.
     For example, prints capture data. */

  if ((read_size != 0) && (read_size == buffer_size)) /* Frame size is fixed as it is capture */
  {
    /* Convert q15 to f32. */
    int16_t* pt = (int16_t*)buffer;
    for(int i=0 ; i<buffer_sample;i++){ /* 256 is 768/3. Because 48k -> 16k.*/
      buffer_f[i*2] = ((float)*pt)/0x7FFF;
      buffer_f[i*2+1] = 0.0F;
      pt++;
    }

    /* File write for debug. */
//    int ret = myFile.write((uint8_t*)buffer_f, 256*4*2);

    float32_t* in  = buffer_f;
    float32_t* out = output;
    rev_fft_f32(in, out);

//  exec_fft_f32(in, out);
    send_fft_f32(in, out);

    if(detect_bark(24,30,out)){
      puts("Find sound!");
    }

    rev_fft_f32(in, out);
    send_fft_f32(in+256, out);
    
    if(detect_bark(24,30,out)){
      puts("Find sound!");
    }

    rev_fft_f32(in, out);
    send_fft_f32(in+512, out);
    
    if(detect_bark(24,30,out)){
      puts("Find sound!");
    }

    cnt++;

  }else{
   usleep(1);
  }
}
