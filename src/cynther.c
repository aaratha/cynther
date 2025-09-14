#ifndef CYNTHER_IMPLEMENTATION
#include "../include/cynther/cynther.h"
#include "audio.c"
#include "dsp.c"
#include "pattern.c"
#include "stdio.h"
#endif

void wrap_print(struct HelloWorld *hw) {
  print_hello_world(&(struct HelloWorld){"Hello, World!"});
}
