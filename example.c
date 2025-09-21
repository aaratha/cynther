#define CYNTHER_IMPLEMENTATION
#include "cynther.h"
#include <unistd.h>

int main(int argc, char **argv) {
  cyn_voice *voices = cyn_init_voices();
  cyn_init(voices);

  // Test with LFO tremolo effect
  cyn_osc osc1 = cyn_new_osc(220.0f, 0.2f, 0.0f, CYN_SINE);
  cyn_pattern *pattern1 = cyn_new_pattern(1, "A3");
  cyn_voice wow = cyn_new_voice("wow", &osc1, pattern1);
  cyn_lfo *lfo1 = cyn_new_lfo(
      1.0f, 100.0f, &osc1.base_freq,
      &osc1.read_freq); // NULL target since we're using it as tremolo
  cyn_effect *lfo_effect = cyn_new_effect(CYN_LFO, lfo1);
  cyn_add_effect(&wow, lfo_effect);

  cyn_add_voice(wow);

  cyn_osc osc2 = cyn_new_osc(440.0f, 0.1f, 0.0f, CYN_SAW);
  cyn_pattern *pattern2 = cyn_new_pattern(4, "C3", "Bf3", "A3", "G2");
  cyn_adsr env2 = cyn_new_adsr(0.01f, 0.2f, 0.5f, 0.2f);
  cyn_effect *adsr_effect = cyn_new_effect(CYN_ADSR, &env2);
  cyn_voice v2 = cyn_new_voice("v2", &osc2, pattern2);
  cyn_add_effect(&v2, adsr_effect);
  cyn_add_voice(v2);

  cyn_begin();

  // Modify ADSR parameters in real-time
  while (CYNTHER_RUNNING) {
    float adsr_lfo = 0.5 * sinf(4.0f * M_PI * 0.1f * cyn_time()) + 0.5f;
    cyn_set_adsr_sustain("v2", adsr_lfo);
  }

  cyn_exit();
  (void)argc;
  (void)argv;
}
