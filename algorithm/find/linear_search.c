// 顺序查找算法又称顺序搜索算法或者线性搜索算法，是所有查找算法中最基本、最简单的，对应的时间复杂度为O(n)。
// 顺序查找算法适用于绝大多数场景，既可以在有序序列中查找目标元素，也可以在无序序列中查找目标元素。

#include <stdio.h>
#define N 10  // 待查找序列的元素个数
// 实现顺序查找，arr[N] 为待查找序列，value为要查找的目标元素
int linear_search(int arr[N], int value) {
  int i;
  // 从第1个元素开始遍历
  for (i = 0; i < N; i++) {
    // 匹配成功，返回元素所处的位置下标
    if (arr[i] == value) {
      return i;
    }
  }
  // 匹配失败，返回-1
  return -1;
}
int main() {
  int arr[N] = {10, 14, 19, 26, 27, 31, 33, 35, 42, 44};
  int add = linear_search(arr, 33);
  if (add != -1) {
    printf("查找成功，为序列中第%d个元素\n", add + 1);
  } else {
    printf("查找失败\n");
  }

  // 查找成功，为序列中第7个元素

  return 0;
}