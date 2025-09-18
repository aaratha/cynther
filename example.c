#define CYNTHER_IMPLEMENTATION
#include "cynther.h"
#include <unistd.h>

// Example usage:
//
// int main() {
//   audio_init();
//
//   Voice sine = make_sine_voice();
//   Pattern kick = pattern("x---x---");
//   Pattern hat = pattern("--x---x-");
//
//   play_pattern(&sine, kick, 120, 60.0f); // freq=60Hz
//   play_pattern(&sine, hat, 120, 200.0f); // freq=200Hz
//
//   audio_start();
//   while (1)
//     sleep(1); // keep running
// }

int main(int argc, char **argv) {
  cyn_voice *voices = cyn_init_voices();
  cyn_init(voices);

  cyn_osc osc1 = cyn_new_osc(220.0f, 0.2f, 0.0f, SINE);
  cyn_osc lfo1 = {.freq = 2.0f, .amp = 100.0f, .phase = 0.0f, .type = SINE};
  cyn_pattern *pattern1 = cyn_new_pattern(2, "A3", "C3");
  cyn_voice voice1 = cyn_new_voice("wow", &osc1, pattern1, &lfo1, NULL);
  cyn_add_voice(voice1);

  cyn_osc osc2 = cyn_new_osc(440.0f, 0.1f, 0.0f, SAW);
  cyn_pattern *pattern2 = cyn_new_pattern(4, "C3", "Bf3", "A3", "G2");
  cyn_adsr env2 = cyn_new_adsr(0.01f, 0.2f, 0.5f, 0.2f);
  cyn_voice voice2 = cyn_new_voice("v2", &osc2, pattern2, NULL, &env2);
  cyn_add_voice(voice2);

  // cyn_osc kick = cyn_new_osc(60.0f, 0.5f, 0.0f, SINE);
  // cyn_pattern *pattern_kick = cyn_new_pattern(1, "C0");
  // cyn_adsr env_kick = cyn_new_adsr(0.01f, 0.1f, 0.0f, 0.1f);
  // cyn_voice kick_voice = cyn_new_voice(&kick, pattern_kick, NULL, &env_kick);
  // // cyn_add_voice(kick_voice);

  printf("Audio system initialized. Active voices: %d\n", gAM.activeVoices);

  // Modify ADSR parameters in real-time
  while (CyntherRunning) {
    for (int i = 0; i < MAX_VOICES; i++) {
      float new_sustain = 0.4f + 0.4f * sinf(2.0f * cyn_time() + (M_PI / 1));
      cyn_set_adsr_sustain("v2", new_sustain);
      cyn_set_adsr_sustain("wow", new_sustain);
    }
  }

  // cyn_free_pattern(pattern1);
  cyn_free_pattern(pattern2);
  //  cyn_free_pattern(pattern_kick);
  (void)argc;
  (void)argv;
}
