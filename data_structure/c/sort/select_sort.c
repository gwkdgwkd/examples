#include <stdio.h>

// 对数据量较少的序列实现升序或降序，可以考虑使用选择排序算法，它的时间复杂度为O(n^2)。
// 简单选择排序的实现思想为：
// 对于具有n个记录的无序表遍历n-1次，第i次从无序表中第i个记录开始，
// 找出后序关键字中最小的记录，然后放置在第i的位置上。

void SelectionSort(int* nums, int numsSize) {
  for (int i = 0; i < numsSize - 1; ++i) {
    for (int j = i + 1; j < numsSize; ++j) {
      if (nums[i] > nums[j]) {
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
      }
    }
  }
}

int main() {
  int array[] = {13, 27, 38, 49, 49, 65, 76, 97};
  SelectionSort(array, sizeof(array) / sizeof(int));
  for (int i = 0; i < sizeof(array) / sizeof(int); i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
  // 13 27 38 49 49 65 76 97

  return 0;
}