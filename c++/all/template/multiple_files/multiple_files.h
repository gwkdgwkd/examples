
#ifndef _FUNC_H
#define _FUNC_H
#include <iostream>

// 模板函数声明
template <typename T>
void Swap(T &a, T &b);

// 普通函数
void bubble_sort(int arr[], int n);

// 模板类
template <class T1, class T2>
class Point {
 public:
  Point(T1 x, T2 y) : m_x(x), m_y(y) {}

 public:
  T1 getX() const { return m_x; }
  void setX(T1 x) { m_x = x; }
  T2 getY() const { return m_y; };
  void setY(T2 y) { m_y = y; };
  void display() const;  // 没有定义，只有声明

 private:
  T1 m_x;
  T2 m_y;
};
#endif