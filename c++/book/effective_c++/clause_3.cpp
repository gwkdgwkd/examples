#include <cstring>
#include <iostream>
#include <string>

// 尽可能使用const

// 想要某值只读就应该说出来，这样可以获得编译器的帮助，确保约束不被违反。
// const多才多艺，可以：
// 1.在class外部修饰全局或命名空间中的常量，
//   或修饰文件、函数、或区块作用域中被声明为static的对象；
// 2.修饰class内部的static和non-static成员变量；
// 3.面对指针，也可以指出指针本身、指针所指物，或两者都（不）是const。

namespace n1 {
char greeting[] = "Hello";
char* p1 = greeting;
const char* p2 = greeting;  // char const* p = greeting;
char* const p3 = greeting;
const char* const p4 = greeting;

// const语法虽然变化多端，但并不高深莫测：
// 1.如果关键字const出现在星号左边，表示被指物是常量；
// 2.如果const出现在星号右边，表示指针自身是常量；
// 3.如果出现在星号两边，表示被指物和指针两者都是常量。

// 如果被指物是常量，const和类型的顺序不重要：
void f1(const int* p);
void f2(int const* p);
}  // namespace n1

// const最具威力的用法是面对函数声明的应用。
// 在一个函数内，const可以修饰：
// 1.函数返回值；
// 2.函数参数；
// 3.函数自身（成员函数）。
namespace n2 {
class A {};
// 函数返回常量，可以降低因错误而造成的意外，而不至于放弃安全性和高效性：
const A operator*(const A& lhs, const A& rhs) {}

// const参数，没有什么特别新颖的概念，与local const对象一样，
// 除非需要改动，否则请将它们声明为const。
// 多打了6个字符，却可以省下恼人的错误，比如把==写成=的错误。
A operator/(const A& lhs, const A& rhs) {}

void func() {
  A a, b, c;
  // (a * b) = c;  // 返回const对象，报错
  (a / b) = c;  // 不返回const对象，不报错
}
}  // namespace n2

namespace n3 {
// 将const用于成员函数的目的，是为了确认该成员函数可作用于const对象身上。
// 这一类成员函数之所以重要，基于两个理由：
// 1.使class接口比较容易理解，可以得知哪个函数可以修改对象而哪个函数不行；
// 2.使操作const对象成为可能，这对编写高效代码是个关键。
//   改善C++程序效率的一个根本办法是以常引用的方式传递对象，
//   此技术可行的前提是，有const成员函数可以用来处理取得的const对象。

// 两个成员函数如果只是常量性不同，可以被重载，这是一个重要的C++特性。
class A {
 public:
  A(std::string s) : text(s) {}
  const char& operator[](std::size_t index) const {
    std::cout << "const char& operator[" << index << "]:" << std::endl;
    return text[index];
  }
  char& operator[](std::size_t index) {
    std::cout << "char& operator[" << index << "]:" << std::endl;
    return text[index];
  }

 private:
  std::string text;
};
void print(const A& ctb) { std::cout << ctb[0] << std::endl; }

void func() {
  A a("Hello");
  std::cout << a[0] << std::endl;
  // char& operator[0]:
  // H
  const A ca("World");
  std::cout << ca[0] << std::endl;
  // const char& operator[0]:
  // W

  // 只要重载operator[]并对不同的版本给予不同的返回类型，
  // 就可以令const和non-const获得不同的处理：
  a[0] = 'x';
  // ca[0] = 'x';  // 错误
  // 上述错误只因operator[]的返回类型导致的，operator[]调用没问题。
  // 错误是对const版的operator[]返回的const char&进行赋值导致的。

  // non-const operator[]的返回类型是个char的引用，不是char。
  // 如果operator[]只是返回一个char,下面的句子就无法通过编译：
  a[0] = 'x';
  // 因为，如果函数返回类型是个内置类型，那么改动函数返回值从来就不合法。
  // 就算合法，改动的也是副本，不是a[0]自身。

  print(a);
  // const char& operator[0]:
  // x
  print(ca);
  // const char& operator[0]:
  // W
}
}  // namespace n3

namespace n4 {
// 成员函数如果是const，意味着什么？有两个流行概念：
namespace test1 {
// 1.bitwise constness(physical constness)相信，
//   成员函数只有在不更改对象任何变量（static除外）时才可以说是const。
//   这正是C++对常量性的定义，许多成员函数虽然不十足具备const性质却能满足bitwise。
class A {
 public:
  A(char* s) {
    p = new char[strlen(s) + 1];
    strcpy(p, s);
  }
  char& operator[](std::size_t index) const { return p[index]; }
  void show() const { std::cout << p << std::endl; }

 private:
  char* p;
};
void func() {
  // 创建一个常量对象并初始化，而且只调用了const成员函数，但终究改变了它的值。
  const A a("Hello");
  char* pc = &a[0];
  *pc = 'J';
  a.show();  // Jello
}
}  // namespace test1

namespace test2 {
// 2.logical constness主张，一个const成员函数可以修改它所处理的对象的某些bit，
//   但只有在客户端侦测不到的情况下才得这样。
//   textLength和lengthIsValid不是bitwise constness，可以被修改。
//   编译器坚持bitwise constness，解决方法为：
//   利用C++的一个与const相关的关键字：mutable(可变的)。
//   mutable释放掉none-static成员变量的bitwise constness约束。
class A {
 public:
  A(char* s) : p(s) {}
  std::size_t length() const;

 private:
  char* p;
  // mutable也是为了突破const的限制而设置的：
  mutable std::size_t textLength;
  mutable bool lengthIsValid;
};
std::size_t A::length() const {
  if (!lengthIsValid) {
    // 被mutable修饰的变量，将永远处于可变的状态，即使在const函数中：
    textLength = strlen(p);  // 如果没有mutable修饰，错误！
    lengthIsValid = true;    // 如果没有mutable修饰，错误！
  }
  return textLength;
}
}  // namespace test2
}  // namespace n4

// 请记住：
// 1.声明为const可帮助编译器侦测出错误用法，
//   const可被施加于任何作用域内的对象、函数参数、函数返回类型、成员函数本体；
// 2.编译器强制实施bitwise constness，但编写程序时应该使用概念上的常量性；
// 3.当const和non-const成员函数实现相同时，令non-const调用const可避免重复，
//   不能用const成员函数调用non-const成员函数，这是一种错误行为。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
      break;
    case 1:
      n3::func();
      break;
    case 2:
      n4::test1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}