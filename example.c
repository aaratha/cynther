#define CYNTHER_IMPLEMENTATION
#include "cynther.h"

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

int main() {
  struct HelloWorld hw = {"Hello, World!"};
  wrap_print(&hw);
}
