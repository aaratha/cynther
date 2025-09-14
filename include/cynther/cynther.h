#pragma once

#include <stdio.h>

#include "../../external/miniaudio/miniaudio.h"

struct HelloWorld {
  const char *message;
};

void print_hello_world(struct HelloWorld *hw);
