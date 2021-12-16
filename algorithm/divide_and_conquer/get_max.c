#include <stdio.h>

// 自定义函数，其中[left,right]表示arr数组中查找最大值的范围
int get_max(int* arr, int left, int right) {
  int max_left = 0, max_right = 0, middle = 0;
  // 如果数组不存在
  if (arr == NULL) {
    return -1;
  }
  // 如果查找范围中仅有一个数字
  if (right - left == 0) {
    return arr[left];
  }
  // 如果查找范围中有2个数字，直接比较即可
  if (right - left <= 1) {
    if (arr[left] >= arr[right]) {
      return arr[left];
    }
    return arr[right];
  }
  // 等量划分成2个区域
  middle = (right - left) / 2 + left;
  // 得到左侧区域中的最大值
  max_left = get_max(arr, left, middle);
  // 得到右侧区域中的最大值
  max_right = get_max(arr, middle + 1, right);
  // 比较左、右两侧的最大值，找到[left,right]整个区域的最大值
  if (max_left >= max_right) {
    return max_left;
  } else {
    return max_right;
  }
}

int main() {
  int arr[4] = {3, 7, 2, 1};
  int max = get_max(arr, 0, 3);
  printf("最大值：%d\n", max);
  return 0;
}