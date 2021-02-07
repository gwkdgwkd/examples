#include <iostream>

using namespace std;

// alignas：用于内存对齐，以替代如GCC的__attribute__((__aligned__((#))))，并且更强大。
// alignof：获取内存对齐，替代__alignof__。

// alignas指定符可应用到变量或非位域类数据成员的声明，或能应用于class/struct/union 或枚举的定义。它不能应用于函数参数或catch子句的异常参数。
// 以此声明声明的对象或类型的对齐要求将等于用于声明的所有alignas指定符最严格（最大）的非零expression，除非这会削弱类型的自然对齐。
// 若声明上的最严格（最大）alignas弱于假如它无alignas指定符的情况下本应有的对齐（即弱于其原生对齐或弱于同一对象或类型的另一声明上的alignas），则程序为病式：
struct alignas(8) S {};
struct alignas(1) U {
  S s;
};  // 错误：若无alignas(1)则U的对齐将为8
// 非法的非零对齐，例如alignas(3)为病式。
// 忽略在同一声明上弱于另一alignas的合法的非零对齐。
// 始终忽略alignas(0) 。

// alignof返回std::size_t类型值。返回由类型标识所指示的类型的任何实例所要求的对齐字节数，该类型可以为完整类型、数组类型或者引用类型。
// 若类型为引用类型，则运算符返回被引用类型的对齐；若类型为数组类型，则返回元素类型的对齐要求。
struct Foo {
  int i;
  float f;
  char c;
};
struct Empty {};
struct alignas(64) Empty64 {};
Foo f[10];

struct A1 {
  int a;
  char b;
  int c;
};

// alignas错误：
// 对齐不是2的N次方,error: requested alignment is not a power of 2
// struct alignas(3) E1 {
//   char a;
//   char b;
//   char c;
//   char d;
// };
// 理解错了对齐对象:alignas修饰结构体时，不是对齐结构体每个成员，而是对齐结构体本身，所以大小不是4*2=8，则是4不变。如果再增加一个char成员，则大小6，不是5。
struct alignas(2) E2 {
  char a;
  char b;
  char c;
  char d;
};
// 对齐低于结构体成员最大对齐:sizeof(A)不是6，而是8，因为a已经是4对齐了，所以b之后会补到8。alignas(16)则大小是16。
struct alignas(2) E3 {
  int a;
  char b;
};
struct alignas(16) E4 {
  int a;
  char b;
};

struct alignas(32) A {
  int a;
  alignas(8) char b;
};
struct B {
  A a;
  A b;
  alignas(1) char c;
  A d;
};

// c++11之前，我们经常这么用:
#pragma pack(push)  // 保存当前的对齐方式
#pragma pack(1)     // 指定接下来，1字节对齐
struct A2 {
  int a;
  char b;
  int c;
};
#pragma pop(pop)  // 指定恢复保存的对齐方式（没生效？）
struct A3 {
  int a;
  char b;
  int c;
};

// 明显这个相当暴力了，如果每个人都这么做，至少3行代码。为了省事，总容易有人不push和pop，直接改，某人就会在不知情的情况下，包含了这个头文件，进而受影响。
// 于是，编译器突破规范，定义了扩展：__declspec(align(n))等，记住，是等，也就是各做各的，不统一，于是c++11火速规范化制定了这个标准：alignas和alignof。

// C++11还新增了几个内存对齐的函数，每个函数有特定作用:
// std::alignment_of
// std::aligned_storage
// std::max_align_t
// std::align

// C++98/03 内存对齐实现
// 此情况下主要是由编译器实现，不同编译器有不同方法。
// 1 MSVC
// __declspec(align(#))，其#的内容可以是预编译宏，但不能是编译期数值
// #progma pack
// __alignof
// 2 gcc
// __attribute__((__aligned__((#))))
// __alignof__

int main() {
  std::cout << "char             : " << alignof(char) << endl;     // 1
  std::cout << "pointer          : " << alignof(int*) << endl;     // 8
  std::cout << "class Foo        : " << alignof(Foo) << endl;      // 4
  std::cout << "class Foo array  : " << alignof(f) << endl;        // 4
  std::cout << "empty class      : " << alignof(Empty) << endl;    // 1
  std::cout << "alignas(64) Empty: " << alignof(Empty64) << endl;  // 64

  std::cout << "E2 " << sizeof(E2) << " " << alignof(E2) << endl;  // 4 2
  std::cout << "E3 " << sizeof(E3) << " " << alignof(E3) << endl;  // 8 4
  std::cout << "E4 " << sizeof(E4) << " " << alignof(E4) << endl;  // 16 16

  std::cout << "A " << sizeof(A) << " " << alignof(A) << endl;  // 32 32
  std::cout << "B " << sizeof(B) << " " << alignof(B) << endl;  // 128 32

  std::cout << "A1 " << sizeof(A1) << " " << alignof(A1) << endl;  // 12 4
  std::cout << "A2 " << sizeof(A2) << " " << alignof(A2) << endl;  // 9 1
  std::cout << "A3 " << sizeof(A3) << " " << alignof(A3) << endl;  // 9 1
  return 0;
}