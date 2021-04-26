#ifndef _LINKED_STACK_
#define _LINKED_STACK_

#include <stdio.h>

typedef struct lineStack {
  int data;
  struct lineStack* next;
} lineStack;

lineStack* push(lineStack* stack, int a);
lineStack* pop(lineStack* stack);

#endif  // _LINKED_STACK_