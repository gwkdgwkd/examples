#include "hello.h"
#include <stdio.h>
void hello() {
    printf("hello, go\n");
}

// gcc -fPIC -shared -o libhello.so hello.c