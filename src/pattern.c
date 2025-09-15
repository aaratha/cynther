#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#endif

float pattern_midi_to_freq(int midi) {
  if (midi < 0 || midi > 127)
    return -1.0f; // invalid MIDI
  return 440.0f * powf(2.0f, (midi - 69) / 12.0f);
}

int pattern_note_to_midi(const char *name) {
  // Base note offsets
  int offset = -1;
  switch (name[0]) {
  case 'C':
    offset = 0;
    break;
  case 'D':
    offset = 2;
    break;
  case 'E':
    offset = 4;
    break;
  case 'F':
    offset = 5;
    break;
  case 'G':
    offset = 7;
    break;
  case 'A':
    offset = 9;
    break;
  case 'B':
    offset = 11;
    break;
  default:
    return -1; // invalid note
  }

  // Adjust for sharp '#' or flat 'b'/'f'
  if (name[1] == '#' || name[1] == 's')
    offset += 1;
  else if (name[1] == 'b' || name[1] == 'f')
    offset -= 1;

  // Parse octave
  int octave = 0;
  if (name[1] == '#' || name[1] == 's' || name[1] == 'b' || name[1] == 'f')
    octave = atoi(name + 2);
  else
    octave = atoi(name + 1);

  // MIDI number: C-1 = 0
  int midi = (octave + 1) * 12 + offset;
  if (midi < 0 || midi > 127)
    return -1; // out of MIDI range

  return midi;
}

void pattern_create_midi_freqs(float *midi_freqs) {
  for (int i = 0; i < NUM_NOTES; i++) {
    midi_freqs[i] = pattern_midi_to_freq(i);
  }
};
