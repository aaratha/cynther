#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "pattern.c"
#include "stdio.h"
#endif

float pattern_midi_freqs[NUM_NOTES];

cyn_voice *cyn_init_voices() {
  cyn_voice *voices = malloc(MAX_VOICES * sizeof(cyn_voice));
  for (int i = 0; i < MAX_VOICES; i++) {
    voices[i].active = 0;
    voices[i].osc = NULL;
    voices[i].lfo = NULL;
    voices[i].env = NULL;
    voices[i].pattern = NULL;
  }
  return voices;
}

void cyn_init(cyn_voice *voices) {
  audio_init(voices);
  pattern_create_midi_freqs(pattern_midi_freqs);
}

void cyn_add_voice(cyn_voice voice) {
  if (gAM.activeVoices >= MAX_VOICES) {
    printf("Max voices reached!\n");
    return;
  }

  float sample_time = 0;
  float max_sample_time = DEVICE_SAMPLE_RATE / (float)voice.pattern->count;
  voice.osc->freq = voice.pattern->freqs[0]; // start with the first note

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

void cyn_play(int argc, char **argv) {
  printf("Audio started. Press ENTER to exit.\n");
  getchar();

  audio_exit();
}

cyn_osc cyn_new_osc(float freq, float amp, float phase, cyn_osc_type type) {
  cyn_osc osc;
  osc.freq = freq;
  osc.amp = amp;
  osc.phase = phase;
  osc.level = 0.0f;
  osc.type = type;
  return osc;
}

cyn_voice cyn_new_voice(cyn_osc *osc, cyn_pattern *pat, cyn_osc *lfo,
                        cyn_adsr *env) {
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

  // Assign only what was provided, else NULL
  voice.osc = osc;
  voice.pattern = pat;
  voice.lfo = lfo ? lfo : NULL;
  voice.env = env ? env : NULL;

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
  env.state = 0; // idle
  return env;
}
