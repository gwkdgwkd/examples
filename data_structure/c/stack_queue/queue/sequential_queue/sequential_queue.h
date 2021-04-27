#ifndef _SEQUENTIAL_QUEUE_
#define _SEQUENTIAL_QUEUE_

#include <stdio.h>

#if 0
int enQueue(int *a, int rear, int data);
void deQueue(int *a, int front, int rear);
#else
#define max 5  // 表示顺序表申请的空间大小
int enQueue(int *a, int front, int rear, int data);
int deQueue(int *a, int front, int rear);
#endif

#endif  // _SEQUENTIAL_QUEUE_