#ifndef _LINKED_QUEUE_
#define _LINKED_QUEUE_

#include <stdio.h>

// 在创建链式队列时，强烈建议创建一个带有头节点的链表，这样实现链式队列会更简单。
typedef struct QNode {
  int data;
  struct QNode* next;
} QNode;

QNode* initQueue();
QNode* enQueue(QNode* rear, int data);
QNode* DeQueue(QNode* top, QNode* rear);

#endif  // _LINKED_QUEUE_