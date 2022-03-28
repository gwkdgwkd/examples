#include "sequential_queue.h"

int main() {
#if 0
  int a[100];
  int front, rear;
  // 设置队头指针和队尾指针，当队列中没有元素时，队头和队尾指向同一块地址
  front = rear = 0;
  // 入队
  rear = enQueue(a, rear, 1);
  rear = enQueue(a, rear, 2);
  rear = enQueue(a, rear, 3);
  rear = enQueue(a, rear, 4);
  // 出队
  deQueue(a, front, rear);
  // 出队元素：1
  // 出队元素：2
  // 出队元素：3
  // 出队元素：4
#else
  int a[max];
  int front, rear;
  // 设置队头指针和队尾指针，当队列中没有元素时，队头和队尾指向同一块地址
  front = rear = 0;
  // 入队
  rear = enQueue(a, front, rear, 1);
  rear = enQueue(a, front, rear, 2);
  rear = enQueue(a, front, rear, 3);
  rear = enQueue(a, front, rear, 4);
  // 出队
  front = deQueue(a, front, rear);  // 1
  // 再入队
  rear = enQueue(a, front, rear, 5);
  // 再出队
  front = deQueue(a, front, rear);  //  2
  // 再入队
  rear = enQueue(a, front, rear, 6);
  // 再出队
  front = deQueue(a, front, rear);  // 3
  front = deQueue(a, front, rear);  // 4
  front = deQueue(a, front, rear);  // 5
  front = deQueue(a, front, rear);  // 6
#endif

  return 0;
}