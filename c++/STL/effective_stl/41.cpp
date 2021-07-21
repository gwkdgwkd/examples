#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

// 理解ptr_fun、mem_fun和mem_fun_ref的来由

// 这些函数的一个主要任务是为了掩盖C++语言中一个内在的语法不一致问题。
class Widget {
 public:
  void f() {}
  void test() {}
};
void f(Widget x) {}
void test(Widget& w) {}

// STL中一种很普遍的惯例：函数或者函数对象在被调用的时候，总是使用非成员函数的语法形式。
// STL算法（包括for_each）都硬性采用了非成员函数的语法。
// mem_fun和mem_fun_ref被用来调整成员函数，使之能够通过非成员函数的语法被调用。
// template <typename R, typename C>
// mem_fun_t<R, C> mem_fun(R (C::*pmf)());
// 像mem_fun_t这样的类被称为函数对象配接器。

// 不知道什么时候使用ptr_fun可以一直使用，STL不会在意，也不会带来运行时的性能损失。顶多不便于阅读。
// 或者迫不得已的时候才使用ptr_fun，如果没有用，编译器会提醒你。
// mem_fun和mem_fun_ref在将一个成员函数传递给一个STL组件的时候，就要使用它们。
// 因为它们不仅仅引入一些类型定义（可能必要，也可能不必要），而且它们还转换调用语法的形式来适应算法：
//  将针对成员函数的调用语法转换为STL组件使用的调用语法。如果不使用，编译不过。

// 指针容器支持多态，对象容器却不支持多态。

int main() {
  Widget x;
  f(x);            // 调用非成员函数
  x.f();           // 调用成员函数
  Widget* p = &x;  // 调用成员函数
  p->f();

  vector<Widget> vw;
  for_each(vw.begin(), vw.end(), test);           // 使用非成员函数
  for_each(vw.begin(), vw.end(), ptr_fun(test));  // 加上类型定义也无妨

  // for_each(vw.begin(), vw.end(), &Widget::test);  // 不能编译
  for_each(vw.begin(), vw.end(), mem_fun_ref(&Widget::test));

  list<Widget*> lpw;
  // for_each(lpw.begin(), lpw.end(), &Widget::test);  // 不能编译
  for_each(lpw.begin(), lpw.end(), mem_fun(&Widget::test));

  return 0;
}