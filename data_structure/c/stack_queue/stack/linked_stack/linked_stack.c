#include "linked_stack.h"
#include <stdlib.h>

// 链栈的实现思路同顺序栈类似，顺序栈是将数顺序表（数组）的一端作为栈底，另一端为栈顶；
// 链栈也如此，通常我们将链表的头部作为栈顶，尾部作为栈底。
// 将链表头部作为栈顶的一端，可以避免在实现数据入栈和出栈操作时做大量遍历链表的耗时操作。

// 链表的头部作为栈顶，意味着：
// 1.在实现数据入栈操作时，需要将数据从链表的头部插入；
// 2.在实现数据出栈操作时，需要删除链表头部的首元节点；
// 因此，链栈实际上就是一个只能采用头插法插入或删除数据的链表。

// 链栈元素入栈
// stack为当前的链栈，a表示入栈元素
lineStack* push(lineStack* stack, int a) {
  // 创建存储新元素的节点
  lineStack* line = (lineStack*)malloc(sizeof(lineStack));
  line->data = a;
  // 新节点与头节点建立逻辑关系
  line->next = stack;
  // 更新头指针的指向
  stack = line;
  return stack;
}

// 链栈元素出栈
// 栈顶元素出链栈的实现函数
lineStack* pop(lineStack* stack) {
  if (stack) {
    // 声明一个新指针指向栈顶节点
    lineStack* p = stack;
    // 更新头指针
    stack = stack->next;
    printf("出栈元素：%d\n", p->data);
    if (stack) {
      printf("新栈顶元素：%d\n", stack->data);
    } else {
      printf("栈已空\n");
    }
    free(p);
  } else {
    printf("栈内没有元素\n");
    return stack;
  }
  return stack;
}