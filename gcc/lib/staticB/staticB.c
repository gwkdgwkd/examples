#include <stdio.h>

#include "funcA.h"

extern void funcA();

void funcB() {
  funcA();
  printf("func B form libstaticB.a\n");
}