#include <stdio.h>

#if USEA
#include "a/func_a.h"
extern void funcA();
#endif

#if USEB
#include "b/func_b.h"
extern void funcB();
#endif

#if USEC
#include "c/func_c.h"
extern void funcC();
#endif

int main() {
#if USEA
  funcA();
#endif

#if USEB
  funcB();
#endif

#if USEC
  funcC();
#endif

  printf("main\n");

  return 0;
}