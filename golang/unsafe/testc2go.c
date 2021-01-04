#include <stdio.h>
#include "libhello.h"

// .so
// gcc testc2go.c -o c2go -I./ -L./ -lhello

// .a
// gcc testc2go.c -o c2go -I./ -L./ -lhello -lpthread
void main() {
    GoString str;
    str = Hello();   
    Test();
    printf("%d\n",str.n);
}