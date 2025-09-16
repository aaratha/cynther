#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "pattern.c"
#include "stdio.h"
#endif

float pattern_midi_freqs[NUM_NOTES];

void cyn_init() {
  audio_init();
  pattern_create_midi_freqs(pattern_midi_freqs);
}

void cyn_add_voice(cyn_osc *osc, cyn_osc *lfo, cyn_pattern *pat) {
  if (gAM.activeVoices >= MAX_VOICES) {
    printf("Max voices reached!\n");
    return;
  }

  float sample_time = 0;
  float max_sample_time = DEVICE_SAMPLE_RATE / (float)pat->count;
  osc->freq = pat->freqs[0]; // start with the first note

  // Find the first inactive voice slot
  for (int i = 0; i < MAX_VOICES; i++) {
    if (!gAM.voices[i].active) {
      gAM.voices[i].osc = *osc;
      gAM.voices[i].lfo = *lfo;
      gAM.voices[i].pattern = *pat;
      gAM.voices[i].sample_time = sample_time;
      gAM.voices[i].max_sample_time = max_sample_time;
      gAM.voices[i].active = true;
      gAM.activeVoices++;
      printf("Added voice %d, total active voices: %d\n", i, gAM.activeVoices);
      return;
    }
  }
}

void cyn_play(int argc, char **argv) {
  printf("Audio started. Press ENTER to exit.\n");

  printf("Press Enter to quit...\n");
  getchar();

  audio_exit();

  (void)argc;
  (void)argv;
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
