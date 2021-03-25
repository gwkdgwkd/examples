#include "explicit_instantiate.h"

// 交换两个数的值
template <typename T>
void Swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
// 冒泡排序算法
void bubble_sort(int arr[], int n) {
  for (int i = 0; i < n - 1; i++) {
    bool isSorted = true;
    for (int j = 0; j < n - 1 - i; j++) {
      if (arr[j] > arr[j + 1]) {
        isSorted = false;
        Swap(arr[j], arr[j + 1]);  // 调用Swap()函数
      }
    }
    if (isSorted) break;
  }
}
template void Swap(double &a, double &b);  // 显式实例化定义

template <class T1, class T2>
void Point<T1, T2>::display() const {
  cout << "x=" << m_x << ", y=" << m_y << endl;
}
// 显式实例化定义
template class Point<char *, char *>;
template class Point<int, int>;