#include "intersect.h"
#include "reverse.h"
#include "single_linked_list.h"

int main() {
  printf("初始化链表为：\n");
  link* q = initLinkNoHead();
  displayNoHead(q);  // 1 2 3 4
  link* p = initLinkWithHead();
  displayWithHead(p);  // 1 2 3 4

  printf("在第4的位置插入元素5：\n");
  p = insertElem(p, 5, 4);
  displayWithHead(p);  // 1 2 3 5 4
  printf("删除元素3:\n");
  p = delElem(p, 3);
  displayWithHead(p);  // 1 2 5 4
  printf("查找元素2的位置为：\n");
  int address = selectElem(p, 2);
  if (address == -1) {
    printf("没有该元素");
  } else {
    printf("元素2的位置为：%d\n", address);  // 2
  }
  printf("更改第3的位置上的数据为7:\n");
  p = amendElem(p, 3, 7);
  displayWithHead(p);  // 1 2 7 4

  link* r1 = iteration_reverse(q);
  displayNoHead(r1);  // 4 3 2 1
  link* r2 = recursive_reverse(r1);
  displayNoHead(r2);  // 1 2 3 4
  link* r3 = head_reverse(r2);
  displayNoHead(r3);  // 4 3 2 1
  link* r4 = local_reverse(r3);
  displayNoHead(r4);  // 1 2 3 4

  link* L1 = initLinkWithHead();
  displayWithHead(L1);  // 1 2 3 4
  link* L2 = initLinkWithHead();
  displayWithHead(L2);  // 1 2 3 4
  link* L3 = initLinkWithHead();
  displayWithHead(L3);  // 1 2 3 4
  L2 = amendElem(L2, 1, 10);
  L2 = amendElem(L2, 2, 9);
  L2 = amendElem(L2, 3, 8);
  L2 = amendElem(L2, 4, 7);
  L2 = insertElem(L2, 6, 5);
  displayWithHead(L2);  // 1 2 3 4
  link I1, I2, I3;
  I1.elem = 11;
  I1.next = &I2;
  I2.elem = 12;
  I2.next = &I3;
  I3.elem = 13;
  I3.next = NULL;
  link* t1 = L1;
  while (t1->next) {
    t1 = t1->next;
  }
  t1->next = &I1;
  link* t2 = L2;
  while (t2->next) {
    t2 = t2->next;
  }
  t2->next = &I1;
  displayWithHead(L1);                                 // 1 2 3 4 11 12 13
  displayWithHead(L2);                                 // 10 9 8 7 6 11 12 13
  printf("1 L1 == L2, %d\n", LinkIntersect1(L1, L2));  // 1
  printf("1 L1 == L3, %d\n", LinkIntersect1(L1, L3));  // 0
  printf("2 L1 == L2, %d\n", LinkIntersect2(L1, L2));  // 1
  printf("2 L2 == L3, %d\n", LinkIntersect2(L2, L3));  // 0
  printf("3 L1 == L2, %d\n", LinkIntersect3(L1, L2));  // 1
  printf("3 L2 == L3, %d\n", LinkIntersect3(L2, L3));  // 0

  return 0;
}