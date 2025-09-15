#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "pattern.c"
#include "stdio.h"
#endif

// void wrap_print(struct HelloWorld *hw) {
//   print_hello_world(&(struct HelloWorld){"Hello, World!"});
// }

void cyn_init() { audio_init(); }

void cyn_add_voice(Oscillator *osc, Oscillator *lfo) {
  if (gAM.activeVoices >= MAX_VOICES) {
    printf("Max voices reached!\n");
    return;
  }

  // Find the first inactive voice slot
  for (int i = 0; i < MAX_VOICES; i++) {
    if (!gAM.voices[i].active) {
      gAM.voices[i].osc = *osc;
      gAM.voices[i].lfo = *lfo;
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
