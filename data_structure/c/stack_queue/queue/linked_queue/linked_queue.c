#include "linked_queue.h"
#include <stdlib.h>

// 链式队列的实现思想同顺序队列类似，
// 只需创建两个指针（命名为top和rear）分别指向链表中队列的队头元素和队尾元素。
// 链式队列就不需要考虑空间利用的问题，因为链式队列本身就是实时申请空间。
// 因此，这可以算作是链式队列相比顺序队列的一个优势。

// 创建链式队列的函数
QNode* initQueue() {
  // 创建一个头节点
  QNode* queue = (QNode*)malloc(sizeof(QNode));
  // 对头节点进行初始化
  queue->next = NULL;
  return queue;
}

// 链式队列数据入队
// 链队队列中，当有新的数据元素入队，只需进行以下3步操作：
// 1.将该数据元素用节点包裹，例如新节点名称为elem；
// 2.与rear指针指向的节点建立逻辑关系，即执行rear->next=elem；
// 3.最后移动rear指针指向该新节点，即rear=elem。
// 由此，新节点就入队成功了。
QNode* enQueue(QNode* rear, int data) {
  // 1、用节点包裹入队元素
  QNode* enElem = (QNode*)malloc(sizeof(QNode));
  enElem->data = data;
  enElem->next = NULL;
  // 2、新节点与rear节点建立逻辑关系
  rear->next = enElem;
  // 3、rear指向新节点
  rear = enElem;
  // 返回新的rear，为后续新元素入队做准备
  return rear;
}

// 链式队列数据出队
// 当链式队列中，有数据元素需要出队时，按照"先进先出"的原则，
// 只需将存储该数据的节点以及它之前入队的元素节点按照原则依次出队即可。
// 链式队列中队头元素出队，需要做以下3步操作：
// 1.通过top指针直接找到队头节点，创建一个新指针p指向此即将出队的节点；
// 2.将p节点（即要出队的队头节点）从链表中摘除；
// 3.释放节点p，回收其所占的内存空间；
QNode* DeQueue(QNode* top, QNode* rear) {
  if (top->next == NULL) {
    printf("队列为空\n");
    return rear;
  }
  QNode* p = top->next;
  printf("%d\n", p->data);
  top->next = p->next;
  if (rear == p) {
    rear = top;
  }
  free(p);
  return rear;
}
// 将队头元素做出队操作时，需提前判断队列中是否还有元素，
// 如果没有，要提示用户无法做出队操作，保证程序的健壮性。
