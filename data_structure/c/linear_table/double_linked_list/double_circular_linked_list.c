#include "double_circular_linked_list.h"

// 单链表通过首尾连接可以构成单向循环链表，同样，双向链表也可以进行首尾连接，构成双向循环链表。

// 双向循环链表的创建
// 创建双向循环链表，只需在创建完成双向链表的基础上，将其首尾节点进行双向连接即可。
line* initLine1(line* head) {
  head = (line*)malloc(sizeof(line));
  head->prior = NULL;
  head->next = NULL;
  head->data = 1;
  line* list = head;
  for (int i = 2; i <= 3; i++) {
    line* body = (line*)malloc(sizeof(line));
    body->prior = NULL;
    body->next = NULL;
    body->data = i;
    list->next = body;
    body->prior = list;
    list = list->next;
  }
  // 通过以上代码，已经创建好双线链表，接下来将链表的首尾节点进行双向连接
  list->next = head;
  head->prior = list;
  return head;
}

void display1(line* head) {
  line* temp = head;
  // 由于是循环链表，所以当遍历指针temp指向的下一个节点是head时，证明此时已经循环至链表的最后一个节点
  while (temp->next != head) {
    if (temp->next == NULL) {
      printf("%d\n", temp->data);
    } else {
      printf("%d->", temp->data);
    }
    temp = temp->next;
  }
  // 输出循环链表中最后一个节点的值
  printf("%d", temp->data);
}