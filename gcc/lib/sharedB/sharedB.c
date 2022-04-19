#include <stdio.h>

extern void funcA();

void funcB() {
  printf("func B form libsharedB.so\n");
  funcA();
}