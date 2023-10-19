#include <stdio.h>

#include "../c/func_c.h"

extern void funcC();

#if CIRCULAR
extern void func_b();
#endif

void funcA() {
  funcC();
  printf("func A form libfunc_a.%s\n",
#if SO
         "so"
#else
         "a"
#endif
  );
}

#if CIRCULAR
void func_a() {
  printf("func a form libfunc_a.a\n");
  func_b();
}
#endif