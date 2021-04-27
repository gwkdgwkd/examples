#include "linked_queue.h"

int main() {
  QNode *queue, *top, *rear;
  queue = top = rear = initQueue();  // 创建头结点
  // 向链队列中添加结点，使用尾插法添加的同时，队尾指针需要指向链表的最后一个元素
  rear = enQueue(rear, 1);
  rear = enQueue(rear, 2);
  rear = enQueue(rear, 3);
  rear = enQueue(rear, 4);
  // 入队完成，所有数据元素开始出队列
  rear = DeQueue(top, rear);  // 1
  rear = DeQueue(top, rear);  // 2
  rear = DeQueue(top, rear);  // 3
  rear = DeQueue(top, rear);  // 4
  rear = DeQueue(top, rear);  // 队列为空

  return 0;
}