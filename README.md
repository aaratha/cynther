# Cynther

A single header C library for scripting and interacting with synthesizers.

- Uses miniaudio.h for audio output.

## Roadmap

- [x] Working build script for compiling header
- [x] Programmable audio output
- [x] User customizable oscillators
- [x] LFOs, ADSR envelopes
- [x] User thread for modifying effect parameters
- [x] Universal effect chain for complex effect sequences
- [ ] Complete parameter modulation system
- [ ] CLI for modifying parameters real-time
- [ ] Robust sound library for quick CLI scripting

## Getting Started

Simply copy cynther.h into your project and include it in one of your C files with the `CYNTHER_IMPLEMENTATION` macro defined.

## Example Usage

```C
#define CYNTHER_IMPLEMENTATION
#include "cynther.h"

int main(int argc, char **argv) {
  // Initializes voice container
  cyn_voice *voices = cyn_init_voices();
  cyn_init(voices);

  // Creates first voice with oscillator and pattern
  cyn_osc osc1 = cyn_new_osc(220.0f, 0.2f, 0.0f, CYN_SINE);
  cyn_pattern *pattern1 = cyn_new_pattern(1, "A3");
  cyn_voice wow = cyn_new_voice("wow", &osc1, pattern1);

  // Creates and adds lfo control effect targeting oscillator frequency
  cyn_lfo *lfo1 = cyn_new_lfo(1.0f, 100.0f, &osc1.base_freq, &osc1.read_freq);
  cyn_effect *lfo_effect = cyn_new_effect(CYN_LFO, lfo1);
  cyn_add_effect(&wow, lfo_effect);

  // Adds first voice
  cyn_add_voice(wow);

  // Creates second voice with more complexe pattern
  cyn_osc osc2 = cyn_new_osc(440.0f, 0.1f, 0.0f, CYN_SAW);
  cyn_pattern *pattern2 = cyn_new_pattern(4, "C3", "Bf3", "A3", "G2");
  cyn_voice arp = cyn_new_voice("arp", &osc2, pattern2);

  // Creates and adds ADSR envelope effect
  cyn_adsr env2 = cyn_new_adsr(0.01f, 0.2f, 0.5f, 0.2f);
  cyn_effect *adsr_effect = cyn_new_effect(CYN_ADSR, &env2);
  cyn_add_effect(&arp, adsr_effect);

  // Adds second voice
  cyn_add_voice(arp);

  // Begins audio thread
  cyn_begin();

  // Defines user thread behavior
  while (CYNTHER_RUNNING) {
    // Modulate the ADSR sustain level of the arp voice with a slow LFO
    float adsr_lfo = 0.5 * sinf(4.0f * M_PI * 0.1f * cyn_time()) + 0.5f;
    cyn_set_adsr_sustain("arp", adsr_lfo);
  }

  // Exits program
  cyn_exit();
  (void)argc;
  (void)argv;
}
```
