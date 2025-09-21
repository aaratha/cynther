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
  case CYN_SINE:
    osc->level = dsp_sine(phase);
    break;
  case CYN_SQUARE:
    osc->level = dsp_square(phase);
    break;
  case CYN_SAW:
    osc->level = dsp_saw(phase);
    break;
  default:
    osc->level = 0.0f;
  }
}
