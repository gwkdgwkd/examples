#include <iostream>

// 模板

// 类
template <typename T>
class A {};

// 静态成员方法
template <typename T>
class B {
 public:
  static void f_m(T t) { std::cout << "B::f_m t: " << t << std::endl; };
};

// 静态普通函数
template <typename T>
static void f1(T t) {
  std::cout << "static f1" << std::endl;
};

// 普通函数
template <typename T>
void f2(T t) {
  std::cout << "f2" << std::endl;
};