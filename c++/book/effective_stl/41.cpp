#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

// 理解ptr_fun、mem_fun和mem_fun_ref的来由

class Widget {
 public:
  void f() { std::cout << "Widget::f()" << std::endl; }
  void test() { std::cout << "Widget::test()" << std::endl; }
};
void f(Widget x) { std::cout << "f(Widget x)" << std::endl; }
void test(Widget& w) { std::cout << "test(Widget& w)" << std::endl; }

namespace n1 {
// 这些函数的一个主要任务是为了掩盖C++语言中一个内在的语法不一致问题：
void func() {
  Widget x;
  f(x);   // f(Widget x)
  x.f();  // Widget::f()
  Widget* p = &x;
  p->f();  // Widget::f()
}
}  // namespace n1

namespace n2 {
// STL中一种很普遍的惯例：函数或者函数对象在被调用时，总是使用非成员函数的语法形式。
// STL算法（包括for_each）都硬性采用了非成员函数的语法。
std::vector<Widget> vw(2);
void func1() {
  std::for_each(vw.begin(), vw.end(), test);
  // test(Widget& w)
  // test(Widget& w)
}

// ptr_fun只是将一个普通的函数（或者函数指针），适配成类pointer_to_binary_function，
// 而该类实际上是binary_function的子类，这样出来的functor就有利于同STL的算法等适配。
// 不知道什么时候使用ptr_fun可以一直使用，STL不在意，也不会带来运行时的性能损失，
// 顶多不便于阅读，或者迫不得已的时候才使用ptr_fun，如果没有用，编译器会提醒你。
void func2() {
  // 加上类型定义也无妨：
  std::for_each(vw.begin(), vw.end(), std::ptr_fun(test));
  // test(Widget& w)
  // test(Widget& w)
}

// mem_fun和mem_fun_ref被用来调整成员函数，使之能够通过非成员函数的语法被调用。
// template <typename R, typename C>
// mem_fun_t<R, C> mem_fun(R (C::*pmf)());
// 像mem_fun_t这样的类被称为函数对象配接器。
// mem_fun和mem_fun_ref在将一个成员函数传递给STL组件时，就要使用。
// 因为它们不仅仅引入一些类型定义（必要也可能不必要），还转换调用语法的形式来适应算法：
// 将针对成员函数的调用语法转换为STL组件使用的调用语法，如果不使用，编译不过。
void func3() {
  std::list<Widget*> lpw;
  lpw.insert(lpw.begin(), new Widget());
  lpw.insert(lpw.begin(), new Widget());
  // std::for_each(vw.begin(), vw.end(), &Widget::test);  // 编译报错

  // 当容器中存放的是对象实体的时候用mem_fun_ref：
  std::for_each(vw.begin(), vw.end(), std::mem_fun_ref(&Widget::test));
  // Widget::test()
  // Widget::test()
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n2

// 指针容器支持多态，对象容器却不支持多态。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}