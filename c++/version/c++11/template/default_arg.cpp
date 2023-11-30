#include <iomanip>
#include <iostream>

namespace n1 {
namespace test1 {
// 在C++98/03标准中，类模板可以有默认的模板参数，如下：
template <typename T = int, typename U = int, U N = 0>
struct A {};
// 但是却不支持函数的默认模板参数：
template <typename T = int>
void func(T t) {
  std::cout << typeid(T).name() << ":" << t << std::endl;
}
// error: default template arguments may not be used in
// function templates without -std=c++11 or -std=gnu++11

// g++-4.8 default_arg.cpp
void func() {
  func(5);           // i:5
  func<int>(4);      // i:4
  func<float>(2.2);  // f:2.2
}
}  // namespace test1

namespace test2 {
// 类模板的默认模板参数必须从右往左定义：
template <class T1, class T2 = int>
class A1;
// template<class T1 = int, class T2> class A2; // 报错
template <class T, int i = 0>
class A3;
// template<int i = 0, class T> class A4; // 报错

// 函数模板的默认模板参数则没这个限定：
template <class T1 = int, class T2>
void func(T1 a, T2 b) {
  std::cout << std::setiosflags(std::ios::fixed);
  std::cout << std::setprecision(1) << a << "," << b << std::endl;
}
void func() {
  func<int>(5, 3);             // 5,3
  func<int, float>(5, 3.3);    // 5,3.3
  func<float, float>(5, 3.3);  // 5.0,3.3
}
}  // namespace test2

namespace test3 {
template <int i = 0, class T>
void func(T a) {
  std::cout << a << "|" << i << std::endl;
}
void func() {
  func<5, int>(3);      // 3|5
  func<7, float>(3.6);  // 3.6|7
}
}  // namespace test3
}  // namespace n1

namespace n2 {
namespace test1 {
// 根据实际场景中函数模板被调用的情形，编译器还可以自行推导出部分参数的类型。
// 函数模板的参数推导规则也并不复杂，简单地讲：
// 如果能够从函数实参中推导出类型的话，那么默认模板参数就不会被使用，
// 反之，默认模板参数则可能会被使用。
// 可以一部分模板参数采用默认参数，而另一部分使用自动推导：
template <typename R = int, typename U>
R func(U val) {
  std::cout << "R:" << typeid(R).name() << ",U:" << typeid(U).name()
            << std::endl;
  return val;
}
void func() {
  // 根据实参97自行推导出模板参数U的类型为int，
  // 并且根据返回值val=97推导出R的类型也为int：
  func(97);  // R:i,U:i

  // 手动指定了模板参数R的类型为char（默认模板参数将无效），
  // 并通过实参97推导出了U=int：
  func<char>(97);  // R:c,U:i

  // 手动指定的R和U的类型值，因此无需编译器自行推导：
  func<double, int>(97);  // R:d,U:i
}
}  // namespace test1

namespace test2 {
// 当默认模板参数和自行推导的模板参数同时使用时，
// 若无法推导出函数模板参数的类型，编译器会选择使用默认模板参数；
// 如果模板参数即无法推导出来，又未设置其默认值，则编译器直接报错：
template <typename T, typename U = double>
void func(T t = 0, U u = 0) {
  std::cout << "T:" << typeid(T).name() << ",U:" << typeid(U).name()
            << std::endl;
}
void func() {
  // 通过实参'c'可以推导出T=char，但由于未传递第2个实参，
  // 因此模板参数U使用的是默认参数double：
  func('c');  // T:c,U:d

  // 虽然有默认值，编译器无法通过该默认值推导出模板参数T的类型，
  // 由此不难看出，编译器的自动推导能力并没有想象的那么强大。
  // func(); // 编译报错
}
}  // namespace test2

namespace test3 {
// 默认模板参数通常是需要跟默认函数参数默认值一起使用的：
template <class T, class U = double>  // 默认模板参数
void func(T t = 0, U u = 0) {        // 参数默认值
  std::cout << "T:" << typeid(T).name() << ",U:" << typeid(U).name()
            << std::endl;
};
// 模板函数的默认形参值不是模板参数推导的依据，是由函数的实参推导而来的。
void func() {
  // 由于函数的模板参数可以由函数的实参推导而出：
  func(1, 'c');  // T:i,U:c
  func(1);       // T:i,U:d，使用了默认模板参数double
  // func(); // 错误: T无法被推导出来
  func<int>();        // T:i,U:d, 使用了默认模板参数double
  func<int, char>();  // T:i,U:c
}
}  // namespace test3

// 总的来说，C++11支持为函数模板中的参数设置默认值，在实际使用过程中，
// 可以选择使用默认值，也可以尝试由编译器自行推导得到，还可以指定参数类型。
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n2::test1::func();
      break;
    case 4:
      n2::test2::func();
      break;
    case 5:
      n2::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}