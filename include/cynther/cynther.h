/*
    cynther.h - single-header synth library
    Copyright (c) 2025 Aseem Ratha

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#pragma once

// Standard library headers
#include <assert.h>
#include <math.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External libraries
#include "../../external/miniaudio/miniaudio.h"

// -------------------------
// Macros / Constants
// -------------------------
#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2
#define DEVICE_SAMPLE_RATE 48000
#define MAX_VOICES 8
#define NUM_NOTES 127
#define MAX_EVENTS 64

// -------------------------
// Enums
// -------------------------
typedef enum { CYN_SINE, CYN_SQUARE, CYN_SAW } cyn_osc_type;
typedef enum { CYN_LOWPASS, CYN_HIGHPASS } cyn_filter_type;
typedef enum { CYN_ADSR, CYN_LFO, CYN_FILTER, CYN_DELAY } cyn_effect_type;
typedef enum {
  CYN_ADSR_ATTACK,
  CYN_ADSR_DECAY,
  CYN_ADSR_SUSTAIN,
  CYN_ADSR_RELEASE,
  CYN_ADSR_IDLE
} cyn_adsr_state;

// -------------------------
// Structs
// -------------------------

// Oscillator
typedef struct {
  _Atomic float base_freq;
  _Atomic float read_freq;
  _Atomic float amp;
  float phase; // not atomic, only used inside callback
  float level;
  cyn_osc_type type;
} cyn_osc;

// Pattern
typedef struct {
  float *freqs; // array of note frequencies
  int count;    // number of notes
  int current;  // current note index
} cyn_pattern;

// ADSR envelope
typedef struct {
  _Atomic float attack, decay, sustain, release;
  _Atomic float level;
  int state;
} cyn_adsr;

// Filter
typedef struct {
  float a0, a1, a2, b1, b2;
  float z1, z2;
  cyn_filter_type type;
} cyn_filter;

typedef struct {
  float freq;
  float amp;
  float phase;

  _Atomic float *base;
  _Atomic float *target;
} cyn_lfo;

// Effects and Effect Chain
struct cyn_effect;

typedef float (*cyn_effect_callback)(struct cyn_effect *effect, float input);

typedef struct cyn_effect {
  cyn_effect_type type;
  cyn_effect_callback process;
  void *data;
  struct cyn_effect *next;
} cyn_effect;

// Voice
typedef struct {
  cyn_osc *osc;
  cyn_pattern *pattern;
  cyn_effect *effects;
  float sample_time;
  float max_sample_time;
  bool active;
  char *name;
} cyn_voice;

// Audio manager
typedef struct {
  ma_device_config deviceConfig;
  ma_device device;

  int activeVoices;
  cyn_voice *voices;

  bool audioInitialized;
} cyn_audio_manager;

// -------------------------
// Audio API
// -------------------------
void audio_init(cyn_voice *voices);
void audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount);
void audio_exit();

// -------------------------
// DSP API
// -------------------------
float dsp_sine(float phase);
float dsp_square(float phase);
float dsp_saw(float phase);

void dsp_osc_callback(cyn_osc *osc, float phase);
float _dsp_filter_process(cyn_filter *fl, float in);
float dsp_mix(float *inputs, int count);

// -------------------------
// Pattern API
// -------------------------
int pattern_note_to_midi(const char *name);
float pattern_midi_to_freq(int midi);
void pattern_create_midi_freqs(float midi_freqs[NUM_NOTES]);

// -------------------------
// Public Cynther API
// -------------------------
void effect_adsr_on(cyn_adsr *v);
void effect_adsr_off(cyn_adsr *v);

float effect_adsr_callback(cyn_effect *effect, float input);
float effect_lfo_callback(cyn_effect *effect, float input);
float effect_chain_callback(cyn_effect *chain, float input);

// -------------------------
// Public Cynther API
// -------------------------
float cyn_time();
cyn_voice *cyn_init_voices();
void cyn_init(cyn_voice *voices);
void cyn_begin();
void cyn_exit();

void cyn_add_voice(cyn_voice voice);
cyn_voice *cyn_get_voice(char *name);

cyn_effect *cyn_new_effect(cyn_effect_type type, void *data);
void cyn_add_effect(cyn_voice *voice, cyn_effect *effect);

// ADSR setters
void cyn_set_adsr_attack(char *name, float value);
void cyn_set_adsr_decay(char *name, float value);
void cyn_set_adsr_sustain(char *name, float value);
void cyn_set_adsr_release(char *name, float value);

// Constructors
cyn_osc cyn_new_osc(float freq, float amp, float phase, cyn_osc_type type);
cyn_voice cyn_new_voice(char *name, cyn_osc *osc, cyn_pattern *pat);
cyn_pattern *cyn_new_pattern(int count, ...);
void cyn_free_pattern(cyn_pattern *pat);
cyn_adsr cyn_new_adsr(float attack, float decay, float sustain, float release);
cyn_lfo *cyn_new_lfo(float freq, float amp, _Atomic float *base,
                     _Atomic float *target);
void _cyn_new_filter(cyn_filter_type type, float cutoff, float Q, float sr);
