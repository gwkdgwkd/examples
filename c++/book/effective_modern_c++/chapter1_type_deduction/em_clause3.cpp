#include <deque>
#include <iostream>
#include <vector>

#include "helper.h"

// 条款3：理解decltype

namespace n1 {
// 给一个变量名或者表达式，decltype会告诉这个变量或表达式的类型。
// 与template和auto的类型推导相比，decltype只是复述一遍变量名或表达式的类型，
// 返回的类型往往是人们期望的，不会有令人惊讶的行为。

struct Point {
  int x, y;
};
struct Widget {};
bool f(const Widget& w) {
  decltype(w) ww = w;  // const Widget&类型，需要初始化
  printType(ww);
}

template <typename T>
class Vector {
 public:
  T& operator[](std::size_t index);
};

void func() {
  const int i = 0;
  decltype(i) p = 6;
  // p = 7;  // 报错，p是const int
  printType(p);  // i : int

  Widget w;
  decltype(w) ww;
  printType(ww);  // N2n16WidgetE : n1::Widget

  decltype(f(w)) b;
  printType(b);  // b : bool

  decltype(f) ff;
  // printType(ff);

  f(w);  // N2n16WidgetE : n1::Widget

  Vector<int> v;
  decltype(v) vv;
  printType(vv);  // N2n16VectorIiEE : n1::Vector<int>

  int j;
  decltype(v[0]) v0 = j;  // int&类型
  printType(v0);          // i : int
}
}  // namespace n1

namespace n2 {
// 在C++11中，decltype最主要的用处就是用来声明函数模板时，
// 在函数模板中返回类型取决于参数的类型。

// 例如，一个函数接受支持[]的容器和索引，函数返回值类型与索引操作返回类型一致。
// []的作用在一个对象类型为T的容器上得到的返回值类型为T&，
// 对于std::deque总是这样，但是std::vector不总是这样，
// std::vector<bool>的[]返回的不是bool&，而是一个全新的对象。

template <typename Container, typename Index>
auto f(Container& c, Index i) -> decltype(c[i]) {
  // 模板定义使用了C++11的尾随返回类型技术
  return c[i];
}

void func() {
  std::deque<int> q1{1, 2, 3};
  printType(f(q1, 0));  // i : int

  std::deque<bool> q2{true, false};
  printType(f(q2, 0));  // b : bool

  std::vector<int> v1{1, 2, 3};
  printType(f(v1, 0));  // i : int

  std::vector<bool> v2{true, false};
  printType(f(v2, 0));  // St14_Bit_reference : std::_Bit_reference
}
}  // namespace n2

namespace n3 {
// C++11允许单语句的lambda表达式的返回类型推导，
// C++14扩展到包含多语句的所有的lambda表达式和函数。
// 意味着，C++14可以忽略尾随返回类型，仅仅保留开头的auto。

template <typename Container, typename Index>
auto f(Container& c, Index i) {
  return c[i];  // 编译器从函数实现来推导函数的返回类型
}

void func() {
  std::deque<int> q1{1, 2, 3};
  printType(f(q1, 0));  // i : int

  std::deque<bool> q2{true, false};
  printType(f(q2, 0));  // b : bool

  std::vector<int> v1{1, 2, 3};
  printType(f(v1, 0));  // i : int

  std::vector<bool> v2{true, false};
  printType(f(v2, 0));  // St14_Bit_reference : std::_Bit_reference

  // 条款2中说，对于使用auto来表明函数返回类型的情况，编译器使用模板类型推导。
  // 而条款1中说，在模板类型推导过程中，初始表达式的引用会被忽略。
  // f(q1, 0) = 10;  // 报错
  // q1[0]返回的是int&，但是f的auto返回类型将会忽略这个引用，得到int类型，
  // int作为右值成为函数的返回类型，给右值int赋值10，在C++中被禁止。
}
}  // namespace n3

namespace n4 {
// 为了让f如预期工作，需要使用decltype类型推导，即f的返回类型是c[i]的返回类型，
// 在C++14中通过decltype(auto)来实现，这使得原本的冤家在一起完美工作，
// auto指定需要推导的类型，decltype表明在推导规则中使用decltype推导规则。

template <typename Container, typename Index>
decltype(auto) f(Container& c, Index i) {
  return c[i];  // 现在f的返回类型就是c[i]的返回类型
}

void func1() {
  std::deque<int> q1{1, 2, 3};
  printType(f(q1, 0));  // i : int

  std::deque<bool> q2{true, false};
  printType(f(q2, 0));  // b : bool

  std::vector<int> v1{1, 2, 3};
  printType(f(v1, 0));  // i : int

  std::vector<bool> v2{true, false};
  printType(f(v2, 0));  // St14_Bit_reference : std::_Bit_reference

  f(q1, 0) = 10;
}

void func2() {
  // decltype并不仅仅使用在函数返回类型上，也可以用在变量的推导上：
  int i = 1;
  const int& ci = i;
  auto y = ci;            // y的类型是int
  decltype(auto) z = ci;  // z的类型是const int&

  std::cout << y << " " << z << std::endl;  // 1 1
  i = 2;
  std::cout << y << " " << z << std::endl;  // 1 2

  y = 6;
  // z = 7; // 报错

  // i修改后，y不变而z改变，所以z是引用，y可以修改而z不能修改，所以z是const的。
}

std::deque<int> makeDeque() {}
void func3() {
  // 容器f的参数c是通过左值引用传入的，通过返回容器元素的引用来修改容器是被允许的。
  // 但这也意味着不可能将右值传入这个函数，右值不能和左值引用绑定，除非是常量左值引用。

  // auto q = f(makeDeque(), 5);  // 报错

  // 传给f右值是一种极端情况，右值容器是临时对象，在语句最后被销毁，
  // 意味着对容器中元素的引用在创建它的语句结束后被悬空。
  // 然而有时也是有意义的，可能仅仅想拷贝临时容器中的一个元素。
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n4

namespace n5 {
#if __cplusplus == 201402L
// 要使f可以既传入左值又传入右值，可以通过重载解决，但要维护两个函数。
// 可以使用通用引用来解决这个问题：
template <typename Container, typename Index>
decltype(auto) f(Container&& c, Index i) {
  // 不知道容器类型，意味着忽略了索引对象的类型，
  // 对于不知道类型的对象按值传递会有风险，复制性能低，对象切片等问题，
  // 但是标准库的索引，也都是按值传递的，所以也按值传递。
  return std::forward<Container>(c)[i];
}
#elif __cplusplus == 201103L
template <typename Container, typename Index>
auto f(Container&& c, Index i) -> decltype(std::forward<Container>(c)[i]) {
  return std::forward<Container>(c)[i];
}
#endif

std::deque<int> makeDeque() { return std::deque<int>{1, 2, 3, 4, 5, 6}; }
void func() {
  std::deque<int> q1{1, 2, 3};
  printType(f(q1, 0));  // i : int

  std::deque<bool> q2{true, false};
  printType(f(q2, 0));  // b : bool

  std::vector<int> v1{1, 2, 3};
  printType(f(v1, 0));  // i : int

  std::vector<bool> v2{true, false};
  printType(f(v2, 0));  // St14_Bit_reference : std::_Bit_reference

  f(q1, 0) = 10;

  auto q = f(makeDeque(), 5);
}
}  // namespace n5

namespace n6 {
// decltype几乎所有时候都会输出期望的类型，但是有时候它的输出也会令人惊讶。
// 对一个变量名使用decltype得到这个变量名的类型，变量名是左值，但并不影响decltype的行为。
// 然而，对于更复杂的左值表达式，decltype返回的类型是左值引用。

void func1() {
  int x = 0;
  decltype(x) y = x;    // x是变量名左值表达式，y的类型是int
  decltype((x)) z = x;  // (x)是更复杂的左值表达式，z的类型是int&

  std::cout << x << " " << y << " " << z << std::endl;  // 0 0 0
  x = 1;
  std::cout << x << " " << y << " " << z << std::endl;  // 1 0 1
}

decltype(auto) f1() {
  int x = 0;
  return x;  // 返回int
}
decltype(auto) f2() {
  int x = 0;   // reference to local variable ‘x’ returned
  return (x);  // 返回int&，返回了局部对象的引用，行为未定义
}
void func2() {
  int i = f1();
  int j = f2();  // 段错误
}

void func() {
  func1();
  func2();
}
}  // namespace n6

// 要记住的东西：
// 1.decltype几乎总是得到一个变量或表达式的类型而不需要任何修改；
// 2.对于非变量名的类型为T的左值表达式，decltype总是返回T&；
// 3.C++14支持decltype(auto)，它的行为就像auto，
//   从初始化操作来推导类型，但是它推导类型时使用decltype的规则。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    case 5:
      n6::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}