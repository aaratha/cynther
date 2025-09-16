#pragma once

#include <assert.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../external/miniaudio/miniaudio.h"

#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000
#define MAX_VOICES 8
#define NUM_NOTES 127
#define MAX_EVENTS 64

typedef enum { SINE, SQUARE, SAW } cyn_osc_type;

typedef struct {
  _Atomic float freq;
  _Atomic float amp;
  float phase; // not atomic, only used inside callback
  float level;
  cyn_osc_type type;
} cyn_osc;

typedef struct {
  float *freqs; // array of note frequencies
  int count;    // number of notes
  int current;  // current note index
} cyn_pattern;

typedef struct {
  float attack, decay, sustain, release;
  float level;
  int state;
} cyn_adsr;

typedef struct {
  cyn_osc *osc;
  cyn_osc *lfo;
  cyn_pattern *pattern;
  cyn_adsr *env;
  float sample_time;
  float max_sample_time;
  bool active;
} cyn_voice;

typedef struct {
  float a0, a1, a2, b1, b2;
  float z1, z2;
} cyn_biquad;

typedef struct {
  ma_device_config deviceConfig;
  ma_device device;

  int activeVoices;
  cyn_voice *voices;

  bool audioInitialized;
} cyn_audio_manager;

// Audio API
void audio_init(cyn_voice *voices);
void audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount);
void audio_exit();

// DSP API
float dsp_sine(float phase);
float dsp_square(float phase);
float dsp_saw(float phase);

void dsp_osc_callback(cyn_osc *osc, float phase);
void dsp_adsr_callback(cyn_adsr *env);

void dsp_biquad_init_lowpass(cyn_biquad *bq, float cutoff, float Q, float sr);
float dsp_biquad_process(cyn_biquad *bq, float in);

float dsp_mix(float *inputs, int count);

// Pattern API

int pattern_note_to_midi(const char *name);
float pattern_midi_to_freq(int midi);
void pattern_create_midi_freqs(float midi_freqs[NUM_NOTES]);

// Public Cynther API
cyn_voice *cyn_init_voices();
void cyn_init(cyn_voice *voices);
void cyn_play(int argc, char **argv);
void cyn_add_voice(cyn_voice voice);

cyn_osc cyn_new_osc(float freq, float amp, float phase, cyn_osc_type type);
cyn_voice cyn_new_voice(cyn_osc *osc, cyn_pattern *pat, cyn_osc *lfo,
                        cyn_adsr *env);

cyn_pattern *cyn_new_pattern(int count, ...);
void cyn_free_pattern(cyn_pattern *pat);

cyn_adsr cyn_new_adsr(float attack, float decay, float sustain, float release);
