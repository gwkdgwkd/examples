#include <stdio.h>
#include <stdlib.h>

// 无论是静态链表还是动态链表，有时在解决具体问题时，需要我们对其结构进行稍微地调整。
// 比如，可以把链表的两头连接，使其成为了一个环状链表，通常称为循环链表。
// 需要注意的是，虽然循环链表成环状，但本质上还是链表，因此在循环链表中，依然能够找到头指针和首元节点等。
// 循环链表和普通链表相比，唯一的不同就是循环链表首尾相连，其他都完全一样。

// 约瑟夫环问题，是一个经典的循环链表问题。
// 题意是：已知n个人（分别用编号1，2，3，…，n表示）围坐在一张圆桌周围，从编号为k的人开始顺时针报数，数到m的那个人出列；
// 他的下一个人又从1开始，还是顺时针开始报数，数到m的那个人又出列；依次重复下去，直到圆桌上剩余一个人。
// 约瑟夫环问题有多种变形，比如顺时针转改为逆时针等，虽然问题的细节有多种变数，但解决问题的中心思想是一样的，即使用循环链表。

// 循环链表和动态链表唯一不同在于它的首尾连接，这也注定了在使用循环链表时，附带最多的操作就是遍历链表。
// 在遍历的过程中，尤其要注意循环链表虽然首尾相连，但并不表示该链表没有第一个节点和最后一个结点。所以，不要随意改变头指针的指向。

typedef struct node {
  int number;
  struct node* next;
} person;

person* initLink(int n) {
  person* head = (person*)malloc(sizeof(person));
  head->number = 1;
  head->next = NULL;
  person* cyclic = head;
  for (int i = 2; i <= n; i++) {
    person* body = (person*)malloc(sizeof(person));
    body->number = i;
    body->next = NULL;
    cyclic->next = body;
    cyclic = cyclic->next;
  }
  cyclic->next = head;  // 首尾相连
  return head;
}

void findAndKillK(person* head, int k, int m) {
  person* tail = head;
  // 找到链表第一个结点的上一个结点，为删除操作做准备
  while (tail->next != head) {
    tail = tail->next;
  }
  person* p = head;
  // 找到编号为k的人
  while (p->number != k) {
    tail = p;
    p = p->next;
  }
  // 从编号为k的人开始，只有符合p->next==p时，说明链表中除了p结点，所有编号都出列了，
  while (p->next != p) {
    // 找到从p报数1开始，报m的人，并且还要知道数m-1的人的位置tail，方便做删除操作。
    for (int i = 1; i < m; i++) {
      tail = p;
      p = p->next;
    }
    tail->next = p->next;  // 从链表上将p结点摘下来
    printf("出列人的编号为:%d\n", p->number);
    free(p);
    p = tail->next;  // 继续使用p指针指向出列编号的下一个编号，游戏继续
  }
  printf("出列人的编号为:%d\n", p->number);
  free(p);
}

int main() {
  int n = 5;
  printf("输入圆桌上的人数n:%d\n", n);
  person* head = initLink(n);
  int k = 3;
  printf("从第k人开始报数(k>1且k<%d)：%d\n", n, k);
  int m = 2;
  printf("数到m的人出列：%d\n", m);
  findAndKillK(head, k, m);

  return 0;
}