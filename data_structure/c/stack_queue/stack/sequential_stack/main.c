#include "sequential_stack.h"

int main() {
  int a[100];
  int top = -1;
  top = push(a, top, 1);
  top = push(a, top, 2);
  top = push(a, top, 3);
  top = push(a, top, 4);
  top = pop(a, top);
  top = pop(a, top);
  top = pop(a, top);
  top = pop(a, top);
  top = pop(a, top);

  // 弹栈元素：4
  // 弹栈元素：3
  // 弹栈元素：2
  // 弹栈元素：1
  // 空栈

  return 0;
}