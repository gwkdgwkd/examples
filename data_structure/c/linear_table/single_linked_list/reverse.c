#include "reverse.h"

// 反转链表，又可以称为翻转或逆置链表，常用的实现方案有4种：
// 1.迭代反转法；
// 2.递归反转法；
// 3.就地逆置法；
// 4.头插法。
// 递归反转法更适用于反转不带头节点的链表，其它3种有无头节点都适合。

// 迭代反转链表：
// 该算法的实现思想非常直接，就是从当前链表的首元节点开始，一直遍历至链表的最后一个节点，
// 这期间会逐个改变所遍历到的节点的指针域，另其指向前一个节点，需要借助3个指针来实现。
link* iteration_reverse(link* head) {  // head为无头节点链表的头指针
  if (head == NULL || head->next == NULL) {
    return head;
  }

  link* left = NULL;
  link* mid = head;
  link* right;  // 保存mid的下一个节点

  while (mid) {
    right = mid->next;  // 保存mid的下一个节点，因为接下来要改变mid->next
    mid->next = left;  // 翻转操作
    left = mid;        // 更新left
    mid = right;       // 更新right
  }

  return left;
}

// 递归反转链表
// 和迭代反转法的思想恰好相反，递归反转法的实现思想是从链表的尾节点开始，
// 依次向前遍历，遍历过程依次改变各节点的指向，即另其指向前一个节点。
link* recursive_reverse(link* head) {
  // 递归的出口
  if (head == NULL || head->next == NULL) {
    // 空链或只有一个结点，直接返回头指针
    return head;
  }

  // 一直递归，找到链表中最后一个节点
  link* new_head = recursive_reverse(head->next);
  // 当逐层退出时，new_head的指向都不变，一直指向原链表中最后一个节点；
  // 递归每退出一层，函数中head指针的指向都会发生改变，都指向上一个节点。
  // 每退出一层，都需要改变head->next节点的指向，同时令head节点的指针域为NULL。
  head->next->next = head;
  head->next = NULL;
  // 每一层递归结束，都要将新的头指针返回给上一层。
  // 由此，即可保证整个递归过程中，能够一直找得到新链表的表头。
  return new_head;
}

// 头插法反转链表
// 所谓头插法，是指在原有链表的基础上，依次将位于链表头部的节点摘下，
// 然后采用从头部插入的方式生成一个新链表，则此链表即为原链表的反转版。
link* head_reverse(link* head) {
  if (head == NULL || head->next == NULL) {
    return head;
  }
  link* new_head = NULL;
  link* temp = NULL;
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

// 就地逆置法反转链表
// 就地逆置法和头插法的思想类似，唯一的区别是头插法通过建立一个新链表实现的，
// 而就地逆置法则是直接对原链表做修改，从而实现将原链表反转。
link* local_reverse(link* head) {
  if (head == NULL || head->next == NULL) {
    return head;
  }
  link* beg = head;
  link* end = head->next;
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
// 1.使用迭代反转法实现时，初始状态忽略头节点（直接将mid指向首元节点），
//   仅需在最后一步将头节点的next改为和mid同向即可；
// 2.使用头插法或者就地逆置法实现时，仅需将要插入的节点插入到头节点和首元节点之间即可；
// 3.递归法并不适用反转有头结点的链表（但并非不能实现），该方法更适用于反转无头结点的链表。
