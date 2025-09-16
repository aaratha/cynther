#define CYNTHER_IMPLEMENTATION
#include "cynther.h"

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

  cyn_osc osc1 = {.freq = 220.0f, .amp = 0.2f, .phase = 0.0f, .type = SINE};
  cyn_osc lfo1 = {.freq = 2.0f, .amp = 100.0f, .phase = 0.0f, .type = SINE};
  cyn_pattern *pattern1 = cyn_new_pattern(2, "A3", "C3");
  cyn_voice voice1 = cyn_new_voice(&osc1, pattern1, &lfo1, NULL);
  cyn_add_voice(voice1);

  cyn_osc osc2 = {.freq = 440.0f, .amp = 0.1f, .phase = 0.0f, .type = SAW};
  cyn_pattern *pattern2 = cyn_new_pattern(4, "C3", "Bf3", "A3", "G2");
  cyn_adsr env2 = cyn_new_adsr(0.01f, 0.2f, 0.5f, 0.2f);
  cyn_voice voice2 = cyn_new_voice(&osc2, pattern2, NULL, &env2);
  cyn_add_voice(voice2);

  cyn_play(argc, argv);
  cyn_free_pattern(pattern1);
  cyn_free_pattern(pattern2);
  (void)argc;
  (void)argv;
}
