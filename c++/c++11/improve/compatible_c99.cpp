#include <iostream>

// C++11对以下C99特性的支持纳入了新标准之中：
// 1.C99中的预定义宏；
// 2.__func__预定义标识符；
// 3._Pragma操作符；
// 4.不定参数宏定义以及__VA_ARGS__；
// 5.宽窄字符串连接。

namespace n1 {
// C99预定义宏：
// 1.__STDC_HOSTED__：编译器的目标系统环境中，
//   是否包含标准C库，宏定义为1，否则为0。
// 2.__STDC__：C中表示编译器是否与C标准一致，
//   C++中由编译器决定这个宏是否定义，定义成什么值。
// 3.__STDC__VERSION__：C中表示编译器支持的标准版本，
//   C++中由编译器决定这个宏是否定义，定义成什么值。
// 4.__STDC_ISO_10646__：
//   用于表示C++编译环境符合某个版本的ISO/IEC 10646标准。

void testN1() {
  std::cout << "Standard Clib: " << __STDC_HOSTED__ << std::endl;
  // Standard Clib: 1
  std::cout << "Standard C: " << __STDC__ << std::endl;
  // Standard C: 1

  // std::cout << "C Standard Version: " << __STDC__VERSION__ << std::endl;
  std::cout << "ISO/IEC " << __STDC_ISO_10646__ << std::endl;
  // ISO/IEC 201706
}
}  // namespace n1

namespace n2 {
// __func__预定义标识符：返回所在函数的名字，
// 按标准定义，编译器会在函数定义之后，隐式定义__func__，
// 所以将__func__作为函数参数默认值是不允许的。
const char *hello() { return __func__; }
const char *world() { return __func__; }
// 在C++11标准中，还允许在类和结构体中使用__func__：
struct TestStruct {
  TestStruct() : name(__func__) {}
  const char *name;
};
// 将__func__标识符作为函数参数的默认值是不允许的，报错：
// void FuncFail(std::string func_name = __func__){};
// ‘__func__’ is not defined outside of function scope

void testN2() {
  std::cout << hello() << std::endl;  // hello
  std::cout << world() << std::endl;  // world

  TestStruct ts;
  std::cout << ts.name << std::endl;  // TestStruct
}
}  // namespace n2

namespace n3 {
// 变长参数的宏定义以及__VA_ARGS__：
// C99标准中，变长参数的宏定义是指在宏定义中，
// 参数列表的最后一个参数为省略号，
// 而预定义宏__VA_ARGS__则可以在宏定义的实现部分，
// 替换省略号所代表的字符串。
// #define PR(...) printf{__VA_ARGS__}
#define LOG(...)                                           \
  {                                                        \
    fprintf(stderr, "%s: Line %d:\t", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__);                          \
    fprintf(stderr, "\n");                                 \
  }

void testN3() {
  LOG("x = %d", 3);
  // compatible_c99.cpp: Line 61:    x = 3
  LOG("%d,%f,%s", 5, 3.4, "nihao");
  // compatible_c99.cpp: Line 62:    5,3.400000,nihao
}
}  // namespace n3

namespace n4 {
// 宏__cpluscplus：
// C/C++代码混合编程时运用，主要用途就是实现用C方式进行编译，
// 因为用C++方式编译它会将函数进行名字重组（为了实现函数重载），
// 所以要混合编程必须统一按C方式编译代码。
#ifdef __cplusplus
extern "C" {
#endif
// some C code here
#ifdef __cplusplus
}
#endif

// __cplusplus实际是一个随编译器的不同而变化的值：
// 03标准中，199711L。
// 11标准中，201103L。
// 可以写出以下代码来判断现在用的编译器支不支持C++11:
#if __cplusplus < 201103L
#error "should use C++11 implementation"
#endif

// _Pragma操作符：
// _Pragma(字符串字面值)，下面3中方法，都能保证头文件只被包含一次：
// _Pragma("once")      // 1
// #pragma once         // 2
#ifndef THIS_HEADER  // 3
#define THIS_HEADER
#endif

// 宽窄字符串的连接：
// 在之前char转换为wchar_t是UB；
// 11标准中，char字符串和wchar_t字符串连接，
// 会先将char转换为wchar_t，再连接字符串。

void testN4() {
  std::cout << __cplusplus << std::endl;  // 201103
}
}  // namespace n4

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