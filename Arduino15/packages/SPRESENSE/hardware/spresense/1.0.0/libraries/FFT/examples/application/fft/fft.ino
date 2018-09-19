#include "dsp_rpc.h"

#include <SDHCI.h>
SDClass theSD;
File myFile;

#define DSPMATH_LIBFILE "/mnt/sd0/BIN/DSPFFT"

/* ----------------------------------------------------------------------
* Defines each of the tests performed
* ------------------------------------------------------------------- */
#define MAX_BLOCKSIZE   128

/* ----------------------------------------------------------------------
* Test input data for Floating point Convolution example for 32-blockSize
* Generated by the MATLAB randn() function
* ------------------------------------------------------------------- */
float32_t testInputA_f32[64] =
{
0.0000000000,    0.1305276650,    0.2588274789,    0.3826715903, 
0.4999847407,    0.6087527085,    0.7071138646,    0.7933591723,
0.8660237434,    0.9238868374,    0.9659108249,    0.9914548173, 
1.0000000000,    0.9914548173,    0.9659108249,    0.9238868374, 
0.8660237434,    0.7933591723,    0.7071138646,    0.6087527085, 
0.5000152593,    0.3826715903,    0.2588274789,    0.1305276650, 
0.0000000000,    -0.1305276650,   -0.2588274789,   -0.3826715903, 
-0.4999847407,   -0.6087527085,   -0.7071138646,   -0.7933591723,
-0.8660237434,   -0.9238868374,   -0.9659108249,   -0.9914548173, 
-1.0000000000,   -0.9914548173,   -0.9659108249,   -0.9238868374, 
-0.8660237434,   -0.7933591723,   -0.7071138646,   -0.6087527085, 
-0.5000152593,   -0.3826715903,   -0.2588274789,  -0.1305276650, 
0.0000000000,    0.1305276650,    0.2588274789,    0.3826715903, 
0.4999847407,    0.6087527085,    0.7071138646,    0.7933591723, 
0.8660237434,    0.9238868374,    0.9659108249,    0.9914548173
};

//  arm_status lib_status;                           /* Status of the example */
  float32_t testOutput[64];

void setup()
{
 myFile = theSD.open("test.txt", FILE_WRITE);

  load_library(DSPMATH_LIBFILE);
  init_fft_f32( MAX_BLOCKSIZE, 0, 1);
}

void loop()
{

  // put your main code here, to run repeatedly:

  puts("loop!!");

  for(int i=0;i<32;i=i+2){
    printf("%2.8f, %2.8f\n",testInputA_f32[i],testInputA_f32[i+1]);
  }

  /* Transform input a[n] from time domain to frequency domain A[k] */
  float32_t* in  = testInputA_f32;
  float32_t* out = testOutput;
  exec_fft_f32(in, out);
  
  puts("result");
 
  for(int i=0;i<(64/2);i++){
    printf("%2.8f\n",testOutput[i]);
  }

  while(1);
  usleep(40000);
}
