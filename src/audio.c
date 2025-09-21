#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#endif

// void print_hello_world(struct HelloWorld *hw) { printf("%s\n", hw->message);
// }

cyn_audio_manager gAM = {.audioInitialized = false};

void audio_init(cyn_voice *voices) {
  if (gAM.audioInitialized)
    return;

  gAM.activeVoices = 0;
  gAM.voices = voices;

  gAM.deviceConfig = ma_device_config_init(ma_device_type_playback);
  gAM.deviceConfig.playback.format = DEVICE_FORMAT;
  gAM.deviceConfig.playback.channels = DEVICE_CHANNELS;
  gAM.deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
  gAM.deviceConfig.dataCallback = audio_data_callback;
  // gAM.deviceConfig.pUserData = &gAM.voices;

  if (ma_device_init(NULL, &gAM.deviceConfig, &gAM.device) != MA_SUCCESS) {
    // LOGI("Failed to initialize audio device");
    return;
  }

  if (ma_device_start(&gAM.device) != MA_SUCCESS) {
    // LOGI("Failed to start audio device");
    ma_device_uninit(&gAM.device);
    return;
  }

  gAM.audioInitialized = true;
}

void audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount) {
  cyn_voice *voices = gAM.voices;
  float *out = (float *)pOutput;
  float sr = (float)pDevice->sampleRate;

  double t = (double)clock() / CLOCKS_PER_SEC;

  float inputs[gAM.activeVoices];

  for (ma_uint32 i = 0; i < frameCount; i++) {
    for (int v = 0; v < gAM.activeVoices; v++) {
      cyn_voice *voice = &voices[v];
      cyn_osc *osc = voice->osc;
      cyn_pattern *pat = voice->pattern;

      // handle next note in pattern
      if (voice->sample_time >= voice->max_sample_time) {
        // Advance to next note
        pat->current = (pat->current + 1) % pat->count;
        osc->base_freq = pat->freqs[pat->current];
        osc->read_freq = pat->freqs[pat->current];
        voice->sample_time = 0;

        // Trigger new note on
        for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
          if (fx->type == CYN_ADSR) {
            effect_adsr_on((cyn_adsr *)fx->data);
          }
        }
      } else if (voice->sample_time >=
                 voice->max_sample_time - (DEVICE_SAMPLE_RATE * 0.01f)) {
        // Trigger release slightly before the note ends
        for (cyn_effect *fx = voice->effects; fx; fx = fx->next) {
          if (fx->type == CYN_ADSR) {
            effect_adsr_off((cyn_adsr *)fx->data);
          }
        }
      }

      voice->sample_time++;

      dsp_osc_callback(osc, osc->phase);
      float freq = osc->read_freq;

      osc->phase += freq / sr;
      if (osc->phase >= 1.0f)
        osc->phase -= 1.0f;

      // Process effects and store result
      float processed = effect_chain_callback(voice->effects, osc->level);
      inputs[v] = osc->amp * processed;
    }

    float sample = dsp_mix(inputs, gAM.activeVoices);

    *out++ = sample;
    *out++ = sample;
  }

  (void)pInput;
}

void audio_exit() {
  if (gAM.audioInitialized) {
    ma_device_uninit(&gAM.device);
    gAM.audioInitialized = false;
  }
}
