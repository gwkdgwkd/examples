#ifndef _DOUBLE_LINKED_LIST_
#define _DOUBLE_LINKED_LIST_

#include <stdio.h>
#include <stdlib.h>

// 双向链表中各节点包含以下3部分信息：
//  指针域：用于指向当前节点的直接前驱节点；
//  数据域：用于存储数据元素。
//  指针域：用于指向当前节点的直接后继节点；
typedef struct line {
  struct line* prior;  // 指向直接前趋
  int data;
  struct line* next;  // 指向直接后继
} line;

line* initLine(line* head);
void display(line* head);
line* insertLine(line* head, int data, int add);
line* delLine(line* head, int data);
int selectElem(line* head, int elem);
line* amendElem(line* p, int add, int newElem);

#endif  // _DOUBLE_LINKED_LIST_