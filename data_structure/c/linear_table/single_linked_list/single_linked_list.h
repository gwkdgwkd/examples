#ifndef _SINGLE_LINKED_LIST_
#define _SINGLE_LINKED_LIST_

#include <stdio.h>
#include <stdlib.h>

typedef enum bool { False = 0, True = 1 } bool;

// 链表，别名链式存储结构或单链表，用于存储逻辑关系为"一对一"的数据。
// 与顺序表不同，链表不限制数据的物理存储状态，换句话说，使用链表存储的数据元素，其物理存储位置是随机的。
// 数据元素随机存储，并通过指针表示数据之间逻辑关系的存储结构就是链式存储结构。
typedef struct Link {
  char elem;          // 代表数据域
  struct Link* next;  // 代表指针域，指向直接后继元素
} link;  // link为节点名，每个节点都是一个link结构体

link* initLinkNoHead();
link* initLinkWithHead();
void displayNoHead(link* p);
void displayWithHead(link* p);
link* insertElem(link* p, int elem, int add);
link* delElem(link* p, int add);
int selectElem(link* p, int elem);
link* amendElem(link* p, int add, int newElem);

#endif  // _SINGLE_LINKED_LIST_
