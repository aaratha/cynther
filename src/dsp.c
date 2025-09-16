#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#endif

float dsp_sine(float phase) { return sinf(2.0f * M_PI * phase); }

float dsp_square(float phase) { return phase < 0.5f ? 1.0f : -1.0f; }

float dsp_saw(float phase) { return 2.0f * phase - 1.0f; }

float dsp_mix(float *inputs, int count) {
  float sum = 0.0f;
  for (int i = 0; i < count; i++)
    sum += inputs[i];
  return sum / count;
}

void dsp_osc_callback(cyn_osc *osc, float phase) {
  switch (osc->type) {
  case SINE:
    osc->level = dsp_sine(phase);
    break;
  case SQUARE:
    osc->level = dsp_square(phase);
    break;
  case SAW:
    osc->level = dsp_saw(phase);
    break;
  default:
    osc->level = 0.0f;
  }
}

void dsp_adsr_callback(cyn_adsr *env) {
  switch (env->state) {

  case 0: // Attack
    if (env->attack <= 0.0f) {
      env->level = 1.0f;
      env->state = 1; // move to decay immediately
    } else {
      float attack_rate = 1.0f / (env->attack * DEVICE_SAMPLE_RATE);
      env->level += attack_rate;
      if (env->level >= 1.0f) {
        env->level = 1.0f;
        env->state = 1;
      }
    }
    break;

  case 1: // Decay
    if (env->decay <= 0.0f) {
      env->level = env->sustain;
      env->state = 2; // Move to Sustain immediately
    } else {
      float decay_rate =
          (1.0f - env->sustain) / (env->decay * DEVICE_SAMPLE_RATE);
      env->level -= decay_rate;
      if (env->level <= env->sustain) {
        env->level = env->sustain;
        env->state = 2; // Move to Sustain
      }
    }
    break;
  case 2: // Sustain
    // Hold sustain level
    break;
  case 3: // Release
    if (env->release <= 0.0f) {
      env->level = 0.0f;
      env->state = 4;
    } else {
      float release_rate = env->level / (env->release * DEVICE_SAMPLE_RATE);
      env->level -= release_rate;
      if (env->level <= 0.0f) {
        env->level = 0.0f;
        env->state = 4;
      }
    }
    break;
  case 4: // Inactive
    env->level = 0.0f;
    break;
  default:
    break;
  };
}
