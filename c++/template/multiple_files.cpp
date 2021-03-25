#include "multiple_files.h"

// 反面教材，它将函数模板的声明和实现分别放到了头文件和源文件:
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

// 一个类模板的反面教材，它将类模板的声明和实现分别放到了头文件和源文件。
template <class T1, class T2>
void Point<T1, T2>::display() const {
  cout << "x=" << m_x << ", y=" << m_y << endl;
}