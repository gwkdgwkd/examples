#include <stdio.h>
#include <string.h>

// 归并排序算法是在分治算法(分而后立)基础上设计出来的一种排序算法，对应的时间复杂度为O(nlogn)。
// 归并排序算法实现排序的思路是：
// 1.将整个待排序序列划分成多个不可再分的子序列，每个子序列中仅有1个元素；
// 2.所有的子序列进行两两合并，合并过程中完成排序操作，最终合并得到的新序列就是有序序列。

// 对于含有n个记录的无序表，首先默认表中每个记录各为一个有序表（只不过表的长度都为1），然后进行两两合并，
// 使n个有序表变为⌈n/2⌉个长度为2或者1的有序表（例如4个小有序表合并为2个大的有序表），
// 通过不断地进行两两合并，直到得到一个长度为n的有序表为止。
// 这种归并排序方法称为：2-路归并排序。

// 归并过程中，每次得到的新的子表本身有序，所以最终得到的为有序表。
// 归并排序算法在具体实现时，首先需要将整个记录表进行折半分解，
// 直到分解为一个记录作为单独的一张表为止，然后在进行两两合并。

// 该算法相比于堆排序和快速排序，其主要的优点是：
// 当记录表中含有值相同的记录时，排序前和排序后在表中的相对位置不会改变。

void Merge(int *nums, int start, int mid, int end) {
  int llen = mid - start + 1;
  int rlen = end - mid;
  int left[llen], right[rlen];
  memcpy(left, nums + start, sizeof(int) * llen);
  memcpy(right, nums + mid + 1, sizeof(int) * rlen);

  int li = 0, ri = 0, k = start;
  while (li < llen && ri < rlen) {
    nums[k++] = left[li] < right[ri] ? left[li++] : right[ri++];
  }
  while (li < llen) {
    nums[k++] = left[li++];
  }
}
void MergeSort(int *nums, int start, int end) {
  if (start < end) {
    int mid = start + (end - start) / 2;
    // 归
    MergeSort(nums, start, mid);
    MergeSort(nums, mid + 1, end);
    // 并
    Merge(nums, start, mid, end);
  }
}

void print(int *list, int size) {
  for (int i = 0; i < size; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");
}

int main() {
  int a[] = {35, 33, 42, 10, 14, 19, 27, 44};
  MergeSort(a, 0, sizeof(a) / sizeof(int) - 1);
  print(a, sizeof(a) / sizeof(int));
  // 10 14 19 27 33 35 42 44

  return 0;
}