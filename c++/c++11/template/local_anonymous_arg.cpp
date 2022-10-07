#include <iostream>

// 在C++98中，标准对模板实参的类型还有一些限制。
// 具体地讲，局部的类型和匿名的类型在C++98中都不能做模板类的实参。
template <typename T>
class X {};
template <typename T>
void TempFun(T t){};

struct A {
} a;
struct {
  int i;
} b;  // b是匿名类型变量
typedef struct {
  int i;
} B;  // B是匿名类型

template <typename T>
struct MyTemplate {};

int main() {
  struct C {
  } c;  // C是局部类型

  X<A> x1;  // C++98通过，C++11通过
  X<B> x2;  // 匿名类型：C++98错误，C++11通过
  X<C> x3;  // 局部类型：C++98错误，C++11通过

  TempFun(a);  // C++98通过，C++11通过
  TempFun(b);  // 匿名类型：C++98错误，C++11通过
  TempFun(c);  // 局部类型：C++98错误，C++11通过

  // 可以看到，使用c++98编译时，局部类型，
  // 匿名类型都无法作为模板实参，这些限制都是没有任何意义的，
  // 所以c++11作了改进，以上代码在c++11中编译是OK的。

  // 需要注意的是，虽然匿名类型可以被模板参数接受，如下写法是不可以的：
  // MyTemplate<struct { int a; } > t;
  // 无法编译通过，匿名类型的声明不能在模板实参位置。

  return 0;
}