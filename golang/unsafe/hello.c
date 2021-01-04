#include "hello.h"
#include <stdio.h>
void hello() {
    printf("hello, go, i am c library\n");
}

// gcc -fPIC -shared -o libhello.so hello.c