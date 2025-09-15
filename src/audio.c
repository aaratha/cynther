#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#endif

// void print_hello_world(struct HelloWorld *hw) { printf("%s\n", hw->message);
// }

cyn_audio_manager gAM = {.audioInitialized = false};

void audio_init_voices() {
  for (int i = 0; i < MAX_VOICES; i++) {
    gAM.voices[i].active = 0; // mark all voices inactive
    gAM.voices[i].osc.freq = 0.0f;
    gAM.voices[i].osc.phase = 0.0f;
    gAM.voices[i].osc.amp = 0.0f;
    gAM.voices[i].osc.type = SINE;
  }
}

void audio_init() {
  if (gAM.audioInitialized)
    return;

  audio_init_voices();

  gAM.activeVoices = 0;

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

float audio_wave_callback(OscType type, float phase) {
  switch (type) {
  case SINE:
    return dsp_sine(phase);
  case SQUARE:
    return dsp_square(phase);
  case SAW:
    return dsp_saw(phase);
  default:
    return 0.0f;
  }
}

void audio_data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                         ma_uint32 frameCount) {
  cyn_voice *voices = gAM.voices;
  float *out = (float *)pOutput;
  float sr = (float)pDevice->sampleRate;

  double t = (double)clock() / CLOCKS_PER_SEC;

  // Compute frequency modulation directly here
  // float lfoFreq = 20.0f;
  // float lfo = sinf(2.0f * M_PI * lfoFreq * t); // 0.5 Hz LFO
  // float freq = 300.0f + 100.0f * lfo;

  float inputs[gAM.activeVoices];

  for (ma_uint32 i = 0; i < frameCount; i++) {
    float inputs[gAM.activeVoices];

    for (int i = 0; i < gAM.activeVoices; i++) {
      cyn_osc *osc = &voices[i].osc;
      cyn_osc *lfo = &voices[i].lfo;

      float wave = audio_wave_callback(osc->type, osc->phase);
      float lfoWave = audio_wave_callback(lfo->type, lfo->phase);
      float freq = osc->freq + lfoWave * lfo->amp;

      inputs[i] = osc->amp * wave;

      osc->phase += freq / sr;
      lfo->phase += lfo->freq / sr; // This was missing!

      if (osc->phase >= 1.0f)
        osc->phase -= 1.0f;
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
