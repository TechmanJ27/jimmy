#include "random.h"

#include <stdlib.h>
#include <time.h>

int random_int(int min, int max) {
  srand(time(NULL));
  return rand() % (max - min + 1) + min;
}