#include "linked_stack.h"

int main() {
  lineStack* stack = NULL;
  stack = push(stack, 1);
  stack = push(stack, 2);
  stack = push(stack, 3);
  stack = push(stack, 4);
  stack = pop(stack);
  stack = pop(stack);
  stack = pop(stack);
  stack = pop(stack);
  stack = pop(stack);

  // 出栈元素：4
  // 新栈顶元素：3
  // 出栈元素：3
  // 新栈顶元素：2
  // 出栈元素：2
  // 新栈顶元素：1
  // 出栈元素：1
  // 栈已空
  // 栈内没有元素

  return 0;
}