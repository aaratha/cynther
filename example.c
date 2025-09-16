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
  cyn_init();

  cyn_osc osc1 = {.freq = 220.0f, .amp = 0.2f, .phase = 0.0f, .type = SINE};
  cyn_osc lfo1 = {.freq = 2.0f, .amp = 100.0f, .phase = 0.0f, .type = SINE};
  cyn_pattern *pattern1 = cyn_new_pattern(1, "A3");
  cyn_adsr env1 = {.attack = 0.05f,
                   .decay = 0.0f,
                   .sustain = 1.0f,
                   .release = 0.1f,
                   .level = 0.0f,
                   .state = 0};
  cyn_add_voice(&osc1, &lfo1, pattern1, env1);

  cyn_osc osc2 = {.freq = 440.0f, .amp = 0.1f, .phase = 0.0f, .type = SAW};
  cyn_osc lfo2 = {.freq = 0.0f, .amp = 0.0f, .phase = 0.0f, .type = SINE};
  cyn_pattern *pattern2 = cyn_new_pattern(4, "C3", "Bf3", "A3", "G2");
  cyn_adsr env2 = {.attack = 0.02f,
                   .decay = 0.2f,
                   .sustain = 0.0f,
                   .release = 0.2f,
                   .level = 0.0f,
                   .state = 0};
  cyn_add_voice(&osc2, &lfo2, pattern2, env2);

  cyn_play(argc, argv);
  cyn_free_pattern(pattern1);
  cyn_free_pattern(pattern2);
}
