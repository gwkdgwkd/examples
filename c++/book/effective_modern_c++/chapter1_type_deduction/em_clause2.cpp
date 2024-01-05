#include <iostream>
#include <vector>

#include "helper.h"

// 条款2：理解auto类型推导

namespace n1 {
// 除了一个例外，auto类型推导就是模板类型推导。

// template<typename T>
// void f(ParamType param);
// f(expr)

// auto相当于模板中的T，对变量做的相关类型的限定就向ParamType：
// auto x = 27;  相当于
// template<typename T>
// void func_for_x(T param) {}
// func_for_x(27);

// const auto cx = x;  相当于
// template<typename T>
// void func_for_cx(const T param) {}
// func_for_cx(x);

// const auto& rx = x;  相当于
// template<typename T>
// void func_for_rx(const T& param) {}
// func_for_rx(x);

// 与条款1类似，auto类型推导也分三种情况：
// 1.类型声明是一个指针或者是一个引用，但不是一个通用引用；
// 2.类型声明是一个通用引用；
// 3.类型声明既不是一个指针也不是一个引用。

void fun(int, double) {}
void func() {
  auto x = 27;         // 情况3，x既不是指针也不是引用
  const auto cx = x;   // 情况3，cx既不是指针也不是引用
  const auto& rx = x;  // 情况1，rx是一个非通用引用
  printType(x);        // i : int
  printType(cx);       // i : int
  printType(rx);       // i : int

  auto&& uf1 = x;   // 情况2，x是int的左值，所以uf1是int&
  auto&& uf2 = cx;  // 情况2，cx是int的左值，所以uf2是const int&
  auto&& uf3 = 27;  // 情况2，27是int的右值，所以uf3是int&&
  printType(uf1);   // i : int
  printType(uf2);   // i : int
  printType(uf3);   // i : int

  const char name[] = "123456";  // name的类型是const char[7]
  auto arr1 = name;              // arr1的类型是const char*
  auto& arr2 = name;             // arr2的类型是const char(&)[7]
  printType(name);               // A7_c : char [7]
  printType(arr1);               // PKc : char const*
  printType(arr2);               // A7_c : char [7]

  auto fun1 = fun;
  auto& fun2 = fun;
  printType(fun);   // FvidE : void (int, double)
  printType(fun1);  // PFvidE : void (*)(int, double)
  printType(fun2);  // FvidE : void (int, double)
}
}  // namespace n1

namespace n2 {
void func1() {
  int x1 = 27;    // C++98
  int x2(27);     // C++98
  int x3 = {27};  // C++11
  int x4{27};     // C++11
  // 上面四种语法，都只有一种结果，定义一个int变量，初值为27。
}

// 对待花括号初始化的行为是auto唯一和模板类型推导不一样的地方，
// auto假设花括号初始化代表的是std::initializer_list，模板却不是。
void func2() {
  auto x1 = 27;
  auto x2(27);
  auto x3 = {27};
  auto x4{27};

  printType(x1);  // i : int
  printType(x2);  // i : int
  printType(x3);  // St16initializer_listIiE : std::initializer_list<int>
  printType(x4);  // i : int
  // x4和书上说的不一样，还是int类型

  // auto x5 = {1, 2, 3.0};  // 错误
  auto x6 = {1, 2, 3};
  // auto x7{1, 2, 3};  // 错误
}

template <typename T>
void f1(T param) {
  printType(param);
}
template <typename T>
void f2(std::initializer_list<T> initList) {
  printType(initList);
}
void func3() {
  f1(3);  // i : int
  // f1({3});  // 报错

  // f2(3);  // 报错
  f2({2});     // St16initializer_listIiE : std::initializer_list<int>
  f2({2, 3});  // St16initializer_listIiE : std::initializer_list<int>
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n2

namespace n3 {
// C++14允许auto表示推导的函数返回值，而且C++14的lambda参数声明可以使用auto。
// 但这里面是复用了模板类型推导，而不是auto的类型推导，
// 所以使用auto声明返回值的函数，返回一个花括号初始化就无法编译：
// auto f1() { return {1, 2, 3}; }  // 报错
auto f2() { return 5; }  // g++ -std=c++14 em_clause2.cpp

void func() {
  std::vector<int> v;
  auto l = [&v](const auto& n) { v = n; };
  // l({1, 2, 3});  // 报错，按模板推导规则，不会当成initializer_list
  l(std::vector<int>{1, 2, 3});
}
}  // namespace n3

// 要记住的东西：
// 1.auto类型推导通常和模板类型推导类似，但当涉及到花括号初始化时，
//   auto类型推导假定为std::initializer_list类型，模板却不这样；
// 2.auto在函数返回值或者lambda参数里执行模板的类型推导，
//   而不是通常意义的auto类型推导。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}