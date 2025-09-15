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

  Oscillator osc1 = {.freq = 220.0f, .amp = 0.1f, .phase = 0.0f, .type = SINE};
  Oscillator lfo1 = {.freq = 2.0f, .amp = 100.0f, .phase = 0.0f, .type = SINE};

  Oscillator osc2 = {.freq = 440.0f, .amp = 0.05f, .phase = 0.0f, .type = SAW};
  Oscillator lfo2 = {.freq = 0.0f, .amp = 0.0f, .phase = 0.0f, .type = SINE};

  cyn_add_voice(&osc1, &lfo1);
  cyn_add_voice(&osc2, &lfo2);

  cyn_play(argc, argv);

  // cynther_audio_main(argc, argv);
}
