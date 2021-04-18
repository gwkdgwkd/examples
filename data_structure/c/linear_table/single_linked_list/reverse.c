#include "reverse.h"

// 反转链表，又可以称为翻转或逆置链表，它们表达的是同一个意思。
// 常用的实现方案有4种，这里分别将它们称为迭代反转法、递归反转法、就地逆置法和头插法。
//  递归反转法更适用于反转不带头节点的链表；
//  其它3种方法既能反转不带头节点的链表，也能反转带头节点的链表。

// 1、迭代反转链表
// 该算法的实现思想非常直接，就是从当前链表的首元节点开始，一直遍历至链表的最后一个节点，这期间会逐个改变所遍历到的节点的指针域，另其指向前一个节点。
// 具体的实现方法也很简单，借助3个指针即可。
// 迭代反转法，head为无头节点链表的头指针
link* iteration_reverse(link* head) {
  if (head == NULL || head->next == NULL) {
    return head;
  } else {
    link* beg = NULL;
    link* mid = head;
    link* end = head->next;
    while (1) {
      // 修改mid所指节点的指向
      mid->next = beg;
      // 此时判断end是否为NULL，如果成立则退出循环
      if (end == NULL) {
        break;
      }
      // 整体向后移动3个指针
      beg = mid;
      mid = end;
      end = end->next;
    }
    // 最后修改head头指针的指向
    head = mid;
    return head;
  }
}

// 2、递归反转链表
// 和迭代反转法的思想恰好相反，递归反转法的实现思想是从链表的尾节点开始，依次向前遍历，遍历过程依次改变各节点的指向，即另其指向前一个节点。
link* recursive_reverse(link* head) {
  // 递归的出口
  if (head == NULL || head->next == NULL) {
    // 空链或只有一个结点，直接返回头指针
    return head;
  } else {
    // 一直递归，找到链表中最后一个节点
    link* new_head = recursive_reverse(head->next);
    // 当逐层退出时，new_head的指向都不变，一直指向原链表中最后一个节点；
    // 递归每退出一层，函数中head指针的指向都会发生改变，都指向上一个节点。
    // 每退出一层，都需要改变head->next节点指针域的指向，同时令head所指节点的指针域为NULL。
    head->next->next = head;
    head->next = NULL;
    // 每一层递归结束，都要将新的头指针返回给上一层。由此，即可保证整个递归过程中，能够一直找得到新链表的表头。
    return new_head;
  }
}

// 3、头插法反转链表
// 所谓头插法，是指在原有链表的基础上，依次将位于链表头部的节点摘下，然后采用从头部插入的方式生成一个新链表，则此链表即为原链表的反转版。
link* head_reverse(link* head) {
  link* new_head = NULL;
  link* temp = NULL;
  if (head == NULL || head->next == NULL) {
    return head;
  }
  while (head != NULL) {
    temp = head;
    // 将temp从head中摘除
    head = head->next;
    // 将temp插入到new_head的头部
    temp->next = new_head;
    new_head = temp;
  }
  return new_head;
}

// 4、就地逆置法反转链表
// 就地逆置法和头插法的实现思想类似，唯一的区别在于，头插法是通过建立一个新链表实现的，而就地逆置法则是直接对原链表做修改，从而实现将原链表反转。
link* local_reverse(link* head) {
  link* beg = NULL;
  link* end = NULL;
  if (head == NULL || head->next == NULL) {
    return head;
  }
  beg = head;
  end = head->next;
  while (end != NULL) {
    // 将end从链表中摘除
    beg->next = end->next;
    // 将end移动至链表头
    end->next = head;
    head = end;
    // 调整end的指向，另其指向beg后的一个节点，为反转下一个节点做准备
    end = beg->next;
  }
  return head;
}

// 实际上，对于有头节点的链表反转：
//  使用迭代反转法实现时，初始状态忽略头节点（直接将 mid 指向首元节点），仅需在最后一步将头节点的next改为和mid同向即可；
//  使用头插法或者就地逆置法实现时，仅需将要插入的节点插入到头节点和首元节点之间即可；
//  递归法并不适用反转有头结点的链表（但并非不能实现），该方法更适用于反转无头结点的链表。
