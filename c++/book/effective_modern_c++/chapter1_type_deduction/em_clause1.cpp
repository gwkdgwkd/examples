#include <iostream>

#include "helper.h"

// 第一章：类型推导

// C++98只有一种类型推导规则，函数模板；
// C++11修改了一点规则，并添加了auto和decltype；
// C++14继续扩展了auto和decltype的使用情况。

// 条款1：理解模板类型推导

namespace n1 {
// template<typename T>
// void f(ParamType param);

// f(expr)
// 编译时编译器通过expr推导出两个类型：T和ParamType。
// 一般ParamType和T不同，因为ParamType通常包含装饰，比如const或引用。

template <typename T>
void f(const T& param) {
  printType(param);
  std::cout << param << std::endl;

  // param = 2;  // assignment of read-only reference ‘param’
  const_cast<int&>(param) = 2;
}

void func() {
  int x = 1;
  f(x);                         // i : int
                                // 1
  std::cout << x << std::endl;  // 2

  // T被推导成int，ParamType被推导成const int&
}
}  // namespace n1

namespace n2 {
// 对于n1中的f，一般会很自然的期望T的类型和expr的类型一致。
// 但并非所有情况都如n1中的f一样，T和expr的类型一致。
// T的类型不仅和expr的类型独立，而且还和ParamType的形式独立。

int x = 27;          // int
const int cx = x;    // const int
const int& rx = x;   // const int&
const int* px = &x;  // const int*

namespace test1 {
// 第一种情况：ParamType是个非通用的引用或者是一个指针

// 规则：
// 1.如果expr的类型是个引用，忽略引用的部分；
// 2.然后利用expr的类型和ParamType对比去判断T的类型。

template <typename T>
void f1(T& param) {
  printType(param);
}
void func1() {
  f1(x);   // T是int，ParamType是int &
  f1(cx);  // T是const int，ParamType是const int&
  f1(rx);  // T是const int，ParamType是const int&

  // i : int
  // i : int
  // i : int

  // 对于第二次和第三次调用，cx和rx为const类型变量，T被推导为const int，
  // 导致了ParamType被推导为const int&，这对调用者非常重要。
  // 当传递const对象给引用参数，期望保留其常量特性，即参数变成const的引用。
  // 这也就是为什么给以T&为参数的模板传递const对象是安全的，
  // 对象的const特性是T类型推导的一部分。

  // 在第三次调用时，尽管rx是引用，T扔被推导成了非引用的，rx的引用特性被忽略。

  // 上面的调用展示了左值引用参数的处理方式，类型推导在右值引用上也是如此。
  // 当然，右值参数只可能传递给右值引用参数，但是这个限制和类型推导没有关系。
}

template <typename T>
void f2(const T& param) {
  printType(param);
}
void func2() {
  f2(x);   // T是int，ParamType是const int&
  f2(cx);  // T是int，ParamType是const int&
  f2(rx);  // T是int，ParamType是const int&

  // i : int
  // i : int
  // i : int

  // 把f的参数从T&变成const T&，情况会变化，但并不会令人惊讶。
  // 由于param是const引用，cx和rx的const特性会被保留，
  // 这样的话T的const特性就没有必要了。

  // rx的引用特性在类型推导的过程中会被忽略。
}

// 如果param是一个指针或指向const的指针而不是引用，情况也类似：
template <typename T>
void f3(T* param) {
  printType(param);
}
void func3() {
  f3(&x);  // T是int，ParamType是int*
  f3(px);  // T是const int，ParamType是const int*

  // Pi : int*
  // PKi : int const*
}

template <typename T>
void f4(const T* param) {
  printType(param);
}
void func4() {
  f4(&x);  // T是int，ParamType是const int*
  f4(px);  // T是int，ParamType是const int*

  // PKi : int const*
  // PKi : int const*
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace test1

namespace test2 {
// 第二种情况：ParamType是个通用的引用（Universal Reference）

// 通用引用是模板参数声明为T&&，当传递右值参数时变得不一样了。

// 规则：
// 1.如果expr是左值，T和ParamType都会被推导成左值引用，
//   首先，这是模板类型T被推导成引用的唯一一种情况，
//   其次，尽管ParamType是右值引用，但最终推导出来的是左值引用；
// 2.如果expr是右值，那么就执行普通的法则（第一种情况）。

template <typename T>
void f(T&& param) {
  printType(param);
}
void func() {
  f(x);   // x是左值，所以T是int&，ParamType是int&
  f(cx);  // cx是左值，所以T是const int&，ParamType也是const int&
  f(rx);  // rx是左值，所以T是const int&，ParamType也是const int&
  f(27);  // 27是右值，所以T是int，ParamType的类型是int&&

  // i : int
  // i : int
  // i : int
  // i : int

  // 通用引用和左值引用或右值引用的法则大不相同，特殊情况下，
  // 使用了通用引用，左值参数和右值参数的类型推导大不相同。
}
}  // namespace test2

namespace test3 {
// 第三种情况：ParamType既不是指针也不是引用

// 规则：
// 1.和之前一样，如果expr的类型是引用，将会忽略引用部分；
// 2.如果在忽略expr的引用特性，expr是const的，也要忽略掉const，
//   如果是volatile，也要忽略掉valatile。

// 当ParamType既不是指针也不是引用，把它处理成pass-by-value，
// 这就以为这param就是完全传给他的参数的一份拷贝，一个全新的对象：
template <typename T>
void f(T param) {
  printType(param);
}
void func1() {
  f(x);   // T和ParamType的类型都是int
  f(cx);  // T和ParamType的类型都是int
  f(rx);  // T和ParamType的类型都是int

  // i : int
  // i : int
  // i : int

  // 尽管cx和rx都是const类型，param却不是const的，
  // 这是有道理的，param是一个和cx或rx独立的对象，是份拷贝，
  // cx和rx不能被修改和param能不能被修改是没有关系的。

  // 这就是为什么expr的CV特性在推导param的类型时被忽略掉了：
  // expr不能被修改并不意味这它的一份拷贝不能被修改。
}

void func2() {
  // 总结：
  // 1.CV特性在按值传递参数的时候会被忽略；
  // 2.引用的const或者是指针指向const，expr的const特性会被保留。

  const char* const ptr = "xxx";  // ptr是const指针，指向const对象
  f(ptr);                         // T和ParamType都是const char*，对吧？

  // PKc : char const*

  // ptr不能被修改指向另外一个不同的地址，并且也不能设置成nullptr。
  // ptr指向的字符串也是const的，不能被修改。

  // ptr传给f时，把这个指针的拷贝给param，这样ptr是按值传递的，
  // 根据规则，ptr的const被忽略，ptr指向东西的const被保留，
  // param是个可以被修改的指针，指向的是const字符串。
}

// 尽管函数不能定义成参数为数组，但是可以声明参数是数组的引用：
template <typename T>
void r(T& param) {
  printType(param);
}
// 声明数组的引用可以写出推导数组长度的模板：
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
  return N;
}
void func3() {
  // 很多情况下，数组会被退化成一个指向其第一个元素的指针：
  const char name[] = "xxxxxx";  // name的类型是const char[7]
  const char* ptr = name;        // 数组被退化成指针const char*

  f(name);  // name是数组，但T被推导成const char*
  r(name);  // T为const char[7]，ParamType为const char(&)[7]

  // PKc : char const*
  // A7_c : char [7]

  std::cout << arraySize(name) << std::endl;  // 7
}

void fun(int, double) {}
void func4() {
  // 数组并不是唯一可以退化成指针的东西，函数类型也可以被退化为指针：
  f(fun);  // ParamType被推导成函数指针，类型是void(*)(int,double)
  r(fun);  // ParamType被推导成函数指针，类型是void(&)(int,double)

  // PFvidE : void (*)(int, double)
  // FvidE : void (int, double)
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace test3
}  // namespace n2

// 要记住的东西：
// 1.在模板类型推导的时候，有引用特性的参数的引用特性会被忽略；
// 2.在推导通用引用参数的时候，左值会被特殊处理；
// 3.在推导按值传递的参数时，CV描述符会被忽略；
// 4.在模板类型推导时，参数如果是数组或函数名，会退化成指针，除非参数为引用类型。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}