#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#endif

float effect_adsr_callback(cyn_effect *effect, float input) {
  cyn_adsr *env = (cyn_adsr *)effect->data;

  // advance ADSR state
  switch (env->state) {
  case CYN_ADSR_ATTACK: {
    env->level += 1.0f / (env->attack * DEVICE_SAMPLE_RATE);
    if (env->level >= 1.0f) {
      env->level = 1.0f;
      env->state = CYN_ADSR_DECAY;
    }
  } break;

  case CYN_ADSR_DECAY: {
    float decayStep = (1.0f - env->sustain) / (env->decay * DEVICE_SAMPLE_RATE);
    env->level -= decayStep;
    if (env->level <= env->sustain) {
      env->level = env->sustain;
      env->state = CYN_ADSR_SUSTAIN;
    }
  } break;

  case CYN_ADSR_SUSTAIN:
    // hold sustain level until note off
    break;

  case CYN_ADSR_RELEASE: {
    float releaseStep = env->sustain / (env->release * DEVICE_SAMPLE_RATE);
    env->level -= releaseStep;
    if (env->level <= 0.0f) {
      env->level = 0.0f;
      env->state = CYN_ADSR_IDLE;
    }
  } break;

  case CYN_ADSR_IDLE:
  default:
    env->level = 0.0f;
    break;
  }

  // apply envelope to input
  return input * env->level;
}

float effect_lfo_callback(cyn_effect *effect, float input) {
  cyn_lfo *lfo = (cyn_lfo *)effect->data;

  // generate LFO value (sine wave in range [-1, 1])
  float lfo_value = lfo->amp * sinf(2.0f * M_PI * lfo->phase);

  // advance phase
  lfo->phase += lfo->freq / DEVICE_SAMPLE_RATE;
  if (lfo->phase >= 1.0f)
    lfo->phase -= 1.0f;

  atomic_store(lfo->target, atomic_load(lfo->base) + lfo_value);
  return input;
}

void effect_adsr_on(cyn_adsr *env) {
  env->state = CYN_ADSR_ATTACK;
  env->level = 0.0f;
}

void effect_adsr_off(cyn_adsr *env) {
  if (env->state != CYN_ADSR_IDLE) {
    env->state = CYN_ADSR_RELEASE;
  }
}

float effect_chain_callback(cyn_effect *chain, float input) {
  cyn_effect *e = chain;
  float sample = input;

  while (e) {
    sample = e->process(e, sample);
    e = e->next;
  }

  return sample;
}
