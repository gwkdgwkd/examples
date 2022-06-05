#include "double_linked_list.h"

int main() {
  // 创建一个头指针
  line* head = NULL;
  // 调用链表创建函数
  head = initLine(head);
  // 输出创建好的链表
  display(head);  // 1 <-> 2 <-> 3 <-> 4 <-> 5
  // 显示双链表的优点
  printf("链表中第4个节点的直接前驱是：%d\n",
         head->next->next->next->prior->data);
  // 链表中第4个节点的直接前驱是：3

  // 在表中第3的位置插入元素7
  head = insertLine(head, 7, 3);
  display(head);  // 1 <-> 2 <-> 7 <-> 3 <-> 4 <-> 5
  // 表中删除元素2
  head = delLine(head, 2);
  display(head);  // 1 <-> 7 <-> 3 <-> 4 <-> 5
  printf("元素3的位置是：%d\n", selectElem(head, 3));  // 元素3的位置是：3
  // 表中第3个节点中的数据改为存储6
  head = amendElem(head, 3, 6);
  display(head);  // 1 <-> 7 <-> 6 <-> 4 <-> 5

  return 0;
}