#pragma once

#include <assert.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>

#include "../../external/miniaudio/miniaudio.h"

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000
#define MAX_VOICES 8

typedef enum { SINE, SQUARE, SAW } OscType;

typedef struct {
  float freq;
  float amp;
  float phase;
} LFO;

typedef struct {
  _Atomic float freq;
  _Atomic float amp;
  float phase; // not atomic, only used inside callback
  OscType type;
} Oscillator;

typedef struct {
  Oscillator osc;
  Oscillator lfo;
  bool active;
} Voice;

typedef struct {
  float attack, decay, sustain, release;
  float level;
  int state;
} ADSR;

typedef struct {
  float a0, a1, a2, b1, b2;
  float z1, z2;
} Biquad;

typedef struct {
  ma_device_config deviceConfig;
  ma_device device;

  int activeVoices;

  bool audioInitialized;
} AudioManager;

// Audio API
void audio_init();
void audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount);
void audio_exit();

// DSP API
float dsp_sine(float phase);
float dsp_square(float phase);
float dsp_saw(float phase);

float dsp_adsr_process(ADSR *env);

void dsp_biquad_init_lowpass(Biquad *bq, float cutoff, float Q, float sr);
float dsp_biquad_process(Biquad *bq, float in);

float dsp_mix(float *inputs, int count);
