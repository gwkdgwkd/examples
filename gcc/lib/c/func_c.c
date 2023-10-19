#include <stdio.h>

void funcC() {
  printf("func C form libfunc_c.%s\n",
#if SO
         "so"
#else
         "a"
#endif
  );
}