// 插值查找算法又称插值搜索算法，是在二分查找算法的基础上改进得到的一种查找算法。
// 插值查找算法只适用于有序序列，换句话说，它只能在升序序列或者降序序列中查找目标元素。
// 作为“改进版”的二分查找算法，当有序序列中的元素呈现均匀分布时，插值查找算法的查找效率要优于二分查找算法；
// 反之，如果有序序列不满足均匀分布的特征，插值查找算法的查找效率不如二分查找算法。
// 所谓均匀分布，是指序列中各个相邻元素的差值近似相等。
// 例如，{10,20,30,40,50}就是一个均匀分布的升序序列，各个相邻元素的差值为10。
// 再比如{100,500,2000,5000}是一个升序序列，但各相邻元素之间的差值相差巨大，不具备均匀分布的特征。

#include <stdio.h>
// 实现插值查找算法，ele表示要查找的目标元素，[begin,end]指定查找区域
int interpolation_search(int* arr, int begin, int end, int ele) {
  int mid = 0;
  // 如果[begin,end]不存在，返回 -1
  if (begin > end) {
    return -1;
  }
  // 如果搜索区域内只有一个元素，判断其是否为目标元素
  if (begin == end) {
    if (ele == arr[begin]) {
      return begin;
    }
    // 如果该元素非目标元素，则查找失败
    return -1;
  }
  // 找到"中间元素"所在的位置
  mid = begin + ((end - begin) / (arr[end] - arr[begin]) * (ele - arr[begin]));
  // 递归的出口
  if (ele == arr[mid]) {
    return mid;
  }
  // 比较ele和arr[mid]的值，缩小ele可能存在的区域
  if (ele < arr[mid]) {
    // 新的搜索区域为[begin,mid-1]
    return interpolation_search(arr, begin, mid - 1, ele);
  } else {
    // 新的搜索区域为[mid+1,end]
    return interpolation_search(arr, mid + 1, end, ele);
  }
}
int main() {
  int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  // 输出元素2所在位置的下标
  int pos = interpolation_search(arr, 0, 9, 2);
  if (pos != -1) {
    printf("%d\n", interpolation_search(arr, 0, 9, 2));
  } else {
    printf("查找失败\n");
  }

  // 1

  return 0;
}