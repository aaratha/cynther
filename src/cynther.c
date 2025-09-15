#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "pattern.c"
#include "stdio.h"
#endif

// void wrap_print(struct HelloWorld *hw) {
//   print_hello_world(&(struct HelloWorld){"Hello, World!"});
// }

int cynther_audio_main(int argc, char **argv) {
  audio_init();

  printf("Audio started. Press ENTER to exit.\n");

  printf("Press Enter to quit...\n");
  getchar();

  audio_exit();

  (void)argc;
  (void)argv;
  return 0;
}
