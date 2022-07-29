#include <iostream>

// C++98/03内存对齐实现，不同编译器有不同方法：
// 1 MSVC
// __declspec(align(#))，其#的内容可以是预编译宏，但不能是编译期数值
// #progma pack
// __alignof
// 2 gcc
// __attribute__((__aligned__((#))))
// __alignof__

namespace n1 {
// c++11之前，经常这么用:
struct A1 {
  int a;
  char b;
  int c;
};
#pragma pack(1)
#pragma pack(push)  // 保存当前的对齐方式
#pragma pack(2)     // 指定接下来，2字节对齐
struct A2 {
  int a;
  char b;
  int c;
};
#pragma pack()
struct A3 {
  int a;
  char b;
  int c;
};
#pragma pack(pop)  // 指定恢复保存的对齐方式
struct A4 {
  int a;
  char b;
  int c;
};
#pragma pack()

// #pragma(n)和#pragma pack()一起用是在这期间保证n字节对齐，而且要成对存在。
// #pragma(push)和#pragma(pop)期间不受外接对齐条件影响，而且要成对存在。

// 明显这个相当暴力了，如果每个人都这么做，至少3行代码。
// 为了省事，总容易有人不push和pop，直接改，某人就会在不知情的情况下，
// 包含了这个头文件，进而受影响。
// 于是，编译器突破规范，定义了扩展：__declspec(align(n))等，但也是各做各的，不统一。

void testN1() {
  std::cout << "A1 " << sizeof(A1) << " " << alignof(A1) << std::endl;  // 12 4
  std::cout << "A2 " << sizeof(A2) << " " << alignof(A2) << std::endl;  // 10 2
  std::cout << "A3 " << sizeof(A3) << " " << alignof(A3) << std::endl;  // 12 4
  std::cout << "A3 " << sizeof(A4) << " " << alignof(A4) << std::endl;  // 9 1
}
}  // namespace n1

namespace n2 {
// 基于之前方法的弊端，c++11火速规范化制定了这个标准：alignas和alignof。
// alignas：用于内存对齐，以替代如GCC的__attribute__((__aligned__((#))))，并且更强大。
// alignof：获取内存对齐，替代__alignof__。

// alignas指定符可应用到变量或非位域类数据成员的声明，
// 或能应用于class/struct/union或枚举的定义。
// 它不能应用于函数参数或catch子句的异常参数。
// 以此声明声明的对象或类型的对齐要求将等于用于声明的所有alignas指定符
// 最严格（最大）的非零expression，除非这会削弱类型的自然对齐。

// 若声明上的最严格（最大）alignas弱于假如它无alignas指定符的情况下本应有的对齐，
// 即弱于其原生对齐或弱于同一对象或类型的另一声明上的alignas，则错误：
struct alignas(8) A {};
struct alignas(1) B {  // 1小于8，alignas不生效
  A a;
};

struct C {};
struct alignas(0) D {};  // 始终忽略alignas(0)
// struct alignas(3) E {};  // 非法的非零对齐，对齐不是2的N次方

// 忽略在同一声明上弱于另一alignas的合法的非零对齐。

void testN2() {
  std::cout << "A " << sizeof(A) << " " << alignof(A) << std::endl;  // 8 8
  std::cout << "B " << sizeof(B) << " " << alignof(B) << std::endl;  // 8 8
  std::cout << "C " << sizeof(C) << " " << alignof(C) << std::endl;  // 1 1
  std::cout << "D " << sizeof(D) << " " << alignof(D) << std::endl;  // 1 1
}
}  // namespace n2

namespace n3 {
// alignof返回std::size_t类型值。
// 返回由类型标识所指示的类型的任何实例所要求的对齐字节数，
// 该类型可以为完整类型、数组类型或者引用类型。
// 若类型为引用类型，则运算符返回被引用类型的对齐；
// 若类型为数组类型，则返回元素类型的对齐要求。
struct Foo {
  int i;
  float f;
  char c;
};
struct Empty {};
struct alignas(64) Empty64 {};
Foo f[10];

struct A {
  int a;
  char b;
  int c;
};

void testN3() {
  std::cout << "char             : " << alignof(char) << std::endl;     // 1
  std::cout << "pointer          : " << alignof(int *) << std::endl;    // 8
  std::cout << "class Foo        : " << alignof(Foo) << std::endl;      // 4
  std::cout << "class Foo array  : " << alignof(f) << std::endl;        // 4
  std::cout << "empty class      : " << alignof(Empty) << std::endl;    // 1
  std::cout << "alignas(64) Empty: " << alignof(Empty64) << std::endl;  // 64
}
}  // namespace n3

namespace n4 {
// alignas修饰结构体时，不是对齐结构体每个成员，而是对齐结构体本身
struct alignas(2) A {  // 大小不是4*2=8，是4
  char a;
  char b;
  char c;
  char d;
};
struct alignas(2) B {  // 再增加一个char成员，则大小6，不是5
  char a;
  char b;
  char c;
  char d;
  char e;
};

// 对齐低于结构体成员最大对齐，2小于a的4
struct alignas(2) C {  // 大小不是6，而是8，因为a已经是4对齐了，所以b会补到8
  int a;
  char b;
};
struct alignas(16) D {
  int a;
  char b;
};
struct alignas(32) E {
  int a;
  alignas(8) char b;
};
struct F {
  A a;
  A b;
  alignas(1) char c;
  A d;
};

void testN4() {
  std::cout << "A " << sizeof(A) << " " << alignof(A) << std::endl;  // 4 2
  std::cout << "B " << sizeof(B) << " " << alignof(B) << std::endl;  // 6 2
  std::cout << "C " << sizeof(C) << " " << alignof(C) << std::endl;  // 8 4
  std::cout << "D " << sizeof(D) << " " << alignof(D) << std::endl;  // 16 16
  std::cout << "E " << sizeof(E) << " " << alignof(E) << std::endl;  // 32 32
  std::cout << "F " << sizeof(F) << " " << alignof(F) << std::endl;  // 14 2
}
}  // namespace n4

namespace n5 {
// C++11还新增了几个内存对齐的函数，每个函数有特定作用:
// std::alignment_of
// std::aligned_storage
// std::max_align_t
// std::align

}  // namespace n5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}