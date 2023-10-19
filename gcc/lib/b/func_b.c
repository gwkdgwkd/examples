#include <stdio.h>

#include "../c/func_c.h"

extern void funcC();

#if CIRCULAR
extern void func_a();
#endif

void funcB() {
  funcC();
  printf("func B form libfunc_b.%s\n",
#if SO
         "so"
#else
         "a"
#endif
  );

#if CIRCULAR
  func_a();
#endif
}

#if CIRCULAR
void func_b() { printf("func b form libfunc_b.a\n"); }
#endif