#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "effect.c"
#include "pattern.c"
#include "stdio.h"
#endif

float pattern_midi_freqs[NUM_NOTES];
bool CYNTHER_RUNNING = false;

float cyn_time() { return (float)(clock()) / CLOCKS_PER_SEC; }

cyn_voice *cyn_init_voices() {
  cyn_voice *voices = malloc(MAX_VOICES * sizeof(cyn_voice));
  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].active = 0;
    voices[i].osc = NULL;
    voices[i].pattern = NULL;
    voices[i].effects = NULL;
  }
  return voices;
}

void cyn_init(cyn_voice *voices) {
  audio_init(voices);
  pattern_create_midi_freqs(pattern_midi_freqs);
  CYNTHER_RUNNING = true;
}

void cyn_add_voice(cyn_voice voice) {
  if (gAM.activeVoices >= MAX_VOICES) {
    printf("Max voices reached!\n");
    return;
  }

  float sample_time = 0;
  float max_sample_time = DEVICE_SAMPLE_RATE / (float)voice.pattern->count;
  voice.osc->base_freq = voice.pattern->freqs[0]; // start with the first note
  voice.osc->read_freq = voice.pattern->freqs[0]; // start with the first note

  // Find the first inactive voice slot
  for (int i = 0; i < MAX_VOICES; i++) {
    if (!gAM.voices[i].active) {
      gAM.voices[i] = voice;
      gAM.activeVoices++;
      printf("Added voice %d, total active voices: %d\n", i, gAM.activeVoices);
      return;
    }
  }
}

void cyn_begin() {
  printf("Audio system initialized. Active voices: %d/8\n", gAM.activeVoices);
}

cyn_osc cyn_new_osc(float freq, float amp, float phase, cyn_osc_type type) {
  cyn_osc osc;
  osc.base_freq = freq;
  osc.read_freq = freq;
  osc.amp = amp;
  osc.phase = phase;
  osc.level = 0.0f;
  osc.type = type;
  return osc;
}

cyn_voice cyn_new_voice(char *name, cyn_osc *osc, cyn_pattern *pat) {
  cyn_voice voice;
  if (!osc) {
    fprintf(stderr, "Error: cyn_new_voice() called with NULL osc\n");
    exit(EXIT_FAILURE); // terminate with error
  }
  if (pat == NULL) {
    fprintf(stderr, "Error: cyn_new_voice() called with NULL pattern\n");
    fprintf(stderr, "Must pass pattern with at least one note\n");
    exit(EXIT_FAILURE); // terminate with error
  }

  // Always start inactive until explicitly added
  voice.active = true;
  voice.sample_time = 0.0f;
  voice.name = name;

  // Assign only what was provided, else NULL
  voice.osc = osc;
  voice.pattern = pat;

  voice.effects = NULL;

  // Protect against NULL pattern
  voice.max_sample_time = DEVICE_SAMPLE_RATE / (float)pat->count;

  return voice;
}

cyn_pattern *cyn_new_pattern(int count, ...) {
  if (count <= 0)
    return NULL;

  cyn_pattern *pat = malloc(sizeof(cyn_pattern));
  if (!pat)
    return NULL;

  pat->count = count;
  pat->current = 0; // start before the first note
  pat->freqs = malloc(count * sizeof(float));
  if (!pat->freqs) {
    free(pat);
    return NULL;
  }

  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; i++) {
    const char *note = va_arg(args, const char *);
    int midi = pattern_note_to_midi(note);
    if (midi < 0) {
      pat->freqs[i] = 0.0f; // fallback for invalid note
    } else {
      pat->freqs[i] = pattern_midi_to_freq(midi);
    }
  }
  va_end(args);

  return pat;
}

void cyn_free_pattern(cyn_pattern *pat) {
  if (!pat)
    return;
  free(pat->freqs);
  free(pat);
}

cyn_adsr cyn_new_adsr(float attack, float decay, float sustain, float release) {
  cyn_adsr env;
  env.attack = attack;
  env.decay = decay;
  env.sustain = sustain;
  env.release = release;
  env.level = 0.0f;
  env.state = CYN_ADSR_ATTACK; // idle
  return env;
}

cyn_lfo *cyn_new_lfo(float freq, float amp, _Atomic float *base,
                     _Atomic float *target) {
  cyn_lfo *lfo = malloc(sizeof(cyn_lfo));
  lfo->freq = freq;
  lfo->amp = amp;
  lfo->phase = 0.0f;
  lfo->base = base;
  lfo->target = target;
  return lfo;
}

cyn_voice *cyn_get_voice(char *name) {
  for (int i = 0; i < MAX_VOICES; i++) {
    if (gAM.voices[i].active && strcmp(gAM.voices[i].name, name) == 0) {
      return &gAM.voices[i];
    }
  }
  fprintf(stderr, "Error: Voice '%s' not found\n", name);
  CYNTHER_RUNNING = false; // stop if voice not found
  return NULL;
}

cyn_effect *cyn_new_effect(cyn_effect_type type, void *data) {
  cyn_effect *effect = malloc(sizeof(cyn_effect));
  if (!effect)
    return NULL;

  effect->type = type;
  effect->data = data;
  effect->next = NULL;

  switch (type) {
  case CYN_ADSR:
    effect->process = effect_adsr_callback;
    break;
  case CYN_LFO:
    effect->process = effect_lfo_callback;
    break;
  default:
    free(effect);
    return NULL; // unknown effect type
  }

  return effect;
}

void cyn_add_effect(cyn_voice *voice, cyn_effect *effect) {
  if (!voice || !effect)
    return;

  if (voice->effects == NULL) {
    voice->effects = effect;
  } else {
    cyn_effect *current = voice->effects;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = effect;
  }
}

void cyn_set_adsr_attack(char *name, float value) {
  cyn_voice *voice = cyn_get_voice(name);
  if (!voice)
    return;

  for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
    if (fx->type == CYN_ADSR) {
      ((cyn_adsr *)fx->data)->attack = value;
      return;
    }
  }
}

void cyn_set_adsr_decay(char *name, float value) {
  cyn_voice *voice = cyn_get_voice(name);
  if (!voice)
    return;

  for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
    if (fx->type == CYN_ADSR) {
      ((cyn_adsr *)fx->data)->decay = value;
      return;
    }
  }
}
void cyn_set_adsr_sustain(char *name, float value) {
  cyn_voice *voice = cyn_get_voice(name);
  if (!voice)
    return;

  for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
    if (fx->type == CYN_ADSR) {
      ((cyn_adsr *)fx->data)->sustain = value;
      return;
    }
  }
}

void cyn_set_adsr_release(char *name, float value) {
  cyn_voice *voice = cyn_get_voice(name);
  if (!voice)
    return;

  for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
    if (fx->type == CYN_ADSR) {
      ((cyn_adsr *)fx->data)->release = value;
      return;
    }
  }
}

void cyn_exit() {
  for (int i = 0; i < gAM.activeVoices; i++) {
    if (gAM.voices[i].pattern) {
      cyn_free_pattern(gAM.voices[i].pattern);
    }
  }
  printf("Cynther exited cleanly.\n");
}
