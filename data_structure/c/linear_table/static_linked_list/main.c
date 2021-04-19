#include <stdio.h>
#include "static_linked_list.h"

int main() {
  component array[maxSize];
  int body = initArr(array);
  printf("静态链表为：");
  displayArr(array, body);
  // 静态链表为： ,2 a,3 b,4 c,5 d,0

  printf("在第3的位置上插入结点‘e’:");
  insertArr(array, body, 3, 'e');
  displayArr(array, body);
  // 在第3的位置上插入结点‘e’: ,2 a,3 b,6 e,4 c,5 d,0

  printf("删除数据域为‘a’的结点:");
  deletArr(array, body, 'a');
  displayArr(array, body);
  // 删除数据域为‘a’的结点: ,3 b,6 e,4 c,5 d,0

  printf("查找数据域为‘e’的结点的位置:");
  int selectAdd = selectElem(array, body, 'e');
  printf("%d\n", selectAdd);
  // 查找数据域为‘e’的结点的位置:6

  printf("将结点数据域为‘e’改为‘h’:");
  amendElem(array, body, 'e', 'h');
  displayArr(array, body);
  // 将结点数据域为‘e’改为‘h’: ,3 b,6 h,4 c,5 d,0

  return 0;
}