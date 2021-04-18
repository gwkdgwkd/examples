#include "intersect.h"

// 判断2个单链表是否相交，是一个老生常谈但又极具思考性的面试题。
// 所谓相交，是指有公共的部分，而2个单链表相交，则意味着它们有公共的节点，公共节点的数量可以是1个或者多个。

// 相交的几种情况：
//  1 □→□→□→□↘   ↗□→□→□→□
//             □
//    □→□→□→□↗   ↘□→□→□→□
//  2 □→□→□→□↘
//             □→□→□→□
//    □→□→□→□↗
//  3         ↗□→□→□→□
//    □→□→□→□
//            ↘□→□→□→□
// 单链表中每个节点有且仅有1个指针域，这3种情况中，只有第2种情况符合单链表的特性，另外2种情况则破坏了此特性。

// 1) 分别遍历链表1和链表2，对于链表1中的每个节点，依次和链表2中的各节点进行比对，查看它们的存储地址是否相同，如果相同，则表明它们相交；
//    反之，如果链表1中各节点的存储地址，和链表2中的各个节点都不相同，则表明它们不相交。
//    注意，此方法中比较的是节点的存储地址，而非数据域中存储的元素。原因很简单，数据域中存储元素值相同，并不是2个单链表相交的充分条件，因为2个不相交的链表中很可能存有相同的元素。
bool LinkIntersect1(link* L1, link* L2) {
  link* p1 = L1;
  link* p2 = L2;
  // 逐个遍历L1链表中的各个节点
  while (p1) {
    // 遍历L2链表，针对每个p1，依次和p2所指节点做比较
    p2 = L2;
    while (p2) {
      // p1、p2 中记录的就是各个节点的存储地址，直接比较即可
      if (p1 == p2) {
        printf("  intersect %d\n", p1->elem);
        return True;
      }
      p2 = p2->next;
    }
    p1 = p1->next;
  }
  return False;
}
// 无论2个链表是否相交，此实现方式的时间复杂度为O(n2)。

// 2) 实际上，第1种实现方案还可以进一步优化。结2个单链表相交有一个必然结果，即这2个链表的最后一个节点必定相同；反之，如果2个链表不相交，则这2个链表的最后一个节点必定不相同。
bool LinkIntersect2(link* L1, link* L2) {
  link* p1 = L1;
  link* p2 = L2;
  // 找到L1链表中的最后一个节点
  while (p1->next) {
    p1 = p1->next;
  }
  // 找到L2链表中的最后一个节点
  while (p2->next) {
    p2 = p2->next;
  }
  // 判断L1和L2链表最后一个节点是否相同
  if (p1 == p2) {
    return True;
  }
  return False;
}
// 该函数的时间复杂度就缩小为O(n)。

// 如果2个链表相交，那么它们相交部分所包含的节点个数一定相等。
bool LinkIntersect3(link* L1, link* L2) {
  link* plong = L1;
  link* pshort = L2;
  link* temp = NULL;
  int num1 = 0, num2 = 0, step = 0;
  // 得到L1的长度
  while (plong) {
    num1++;
    plong = plong->next;
  }
  // 得到L2的长度
  while (pshort) {
    num2++;
    pshort = pshort->next;
  }
  // 重置plong和pshort，使plong代表较长的链表，pshort代表较短的链表
  plong = L1;
  pshort = L2;
  step = num1 - num2;
  if (num1 < num2) {
    plong = L2;
    pshort = L1;
    step = num2 - num1;
  }
  // 在plong链表中找到和pshort等长度的子链表
  temp = plong;
  while (step) {
    temp = temp->next;
    step--;
  }
  // 逐个比较temp和pshort链表中的节点是否相同
  while (temp && pshort) {
    if (temp == pshort) {
      printf("  intersect %d\n", temp->elem);
      return True;
    }
    temp = temp->next;
    pshort = pshort->next;
  }
  return False;
}
// 相比第2种方案，此方法的实现逻辑虽然复杂，但优点是，该方法可以找到2个单链表相交的交点（也就是相交时的第一个节点），也就是返回True时的temp指针指向的那个节点。
// 此方案的时间复杂度也为O(n)。

// 从另一个角度比较这3种方案，第1种和第3种在判断“2个链表是否相交”的同时，还能找到它们相交的交点，而第2种实现方案则不具备这个功能。
