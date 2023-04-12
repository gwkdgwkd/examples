#include "sequential_stack.h"

// 栈和队列，严格意义上来说，也属于线性表，因为它们也都用于存储逻辑关系为一对一的数据。
// 使用栈结构存储数据，讲究先进后出，即最先进栈的数据，最后出栈；
// 使用队列存储数据，讲究先进先出，即最先进队列的数据，也最先出队列。
// 既然栈和队列都属于线性表，根据线性表分为顺序表和链表的特点，
// 栈也可分为顺序栈和链表，队列也分为顺序队列和链队列。

// 栈对数据存和取的过程有特殊的要求：
// 1.栈只能从表的一端存取数据，另一端是封闭的；
// 2.在栈中，无论是存数据还是取数据，都必须遵循先进后出的原则，即最先进栈的元素最后出栈。
// 因此，可以给栈下一个定义，即栈是一种只能从表的一端存取数据且遵循先进后出原则的线性存储结构。
// 通常，栈的开口端被称为栈顶，相应地，封口端被称为栈底。
// 因此，栈顶元素指的就是距离栈顶最近的元素。

// 进栈和出栈
// 基于栈结构的特点，在实际应用中，通常只会对栈执行以下两种操作：
// 1.向栈中添加元素，此过程被称为进栈（入栈或压栈）；
// 2.从栈中提取出指定元素，此过程被称为出栈（或弹栈）。

// 栈是一种特殊的线性存储结构，因此栈的具体实现有以下两种方式：
// 1.顺序栈：采用顺序存储结构可以模拟栈存储数据的特点，从而实现栈存储结构；
// 2.链栈：采用链式存储结构实现栈结构。
// 两种实现方式的区别，仅限于数据元素在实际物理空间上存放的相对位置，
// 顺序栈底层采用的是数组，链栈底层采用的是链表。

// 栈的应用
// 浏览器回退功能、检测代码中的括号匹配问题、数值的进制转换功能。

// 顺序栈，即用顺序表实现栈存储结构。

// 顺序栈元素入栈
// 元素elem进栈，a为数组，top值为当前栈的栈顶位置
int push(int* a, int top, int elem) {
  a[++top] = elem;
  return top;
}

// 顺序栈元素出栈
int pop(int* a, int top) {
  if (top == -1) {
    printf("空栈\n");
    return -1;
  }
  printf("弹栈元素：%d\n", a[top]);
  top--;
  return top;
}
