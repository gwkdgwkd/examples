#include <iostream>

// C++自带4种字面量：整形 123、浮点型 12.3、字符 '1'、字符串 "123"
// 字面量又可添加后缀来表明具体类型：无符号整形(unsigned int): 123u、长整形(long): 123l

// 在C++03中，我们可以定义一个浮点数height
// double height = 3.4
// 那么，痛点来了，此处的 height 的单位是什么呢？米？厘米？又或是英尺？
// 在面对此类问题时，如果我们能编写如下代码，事情就会简单许多:
// height = 3cm;
// ratio = (3 * 10) / 2
// ratio = 3cm / 2mm;

// C++11只允许字面量后缀函数的参数为以下类型，即整数，浮点以及字符串：
// unsigned long long
// long double
// char const*
// char const*, std::size_t
// wchar_t const*, std::size_t
// char16_t const*, std::size_t
// char32_t const*, std::size_t
// 返回值则无类型限制

// 自定义字面量的限制：
// 1.如果字面量是字符串，那么字面量操作符函数只能接受const char*, size_t为参数，
//   系统会主动对字符串进行解析，并完成形参赋值匹配；
// 2 如果目标字面量的形式是整数型，那么字面量操作符函数只能接受unsigned long long或const char*为其参数，
//   当unsigned long long溢出后，编译器会自动切换到const char*版本;
// 3 如果目标字面量的形式是浮点数型，那么字面量操作符函数只能接受long double或const char*为其参数，规则同2；
// 4 如果目标字面量为char，那么字面量操作符函数只可接受一个char为参数。

// 此外需要额外注意的是：operator "" _X中的_X中下划线不是标准规定的，但是为避免混乱，下划线最好带上。

namespace n1 {
// C++11允许用户自定义字面量后缀:
long double operator"" _cm(long double x) { return x * 10; }
long double operator"" _m(long double x) { return x * 1000; }
long double operator"" _mm(long double x) { return x; }
// 定义了一个用户自定义的新的类型的操作符，称为字面量操作符literal operator

auto height = 3.0_cm;  // height = 30.0
auto length = 1.23_m;  // length = 1230.0

// 如果使用这种写法，_cm, _m, _mm等函数将在运行时被调用，
// 如果希望在编译时就调用字面量后缀函数，则需要把函数定义为constexpr
constexpr long double operator"" _km(long double x) { return x * 1000000; }

void testN1() {
  std::cout << 1.0_cm << std::endl;           // 10
  std::cout << 1.0_m << std::endl;            // 1000
  std::cout << 1.0_mm << std::endl;           // 1
  std::cout << length / height << std::endl;  // 41
}
}  // namespace n1

namespace n2 {
// 在C++编程中，用户自定义类或结构体是难以避免的事情，所以一旦遇到接受用户自定义的类或结构体作为参数的函数，
// 那么必然难逃”先初始化变量–变量传值或传引用调用函数“，这就导致了编程过程中出现的不直观。
// struct T {};
// void func(T t1, T t2) {}
// T temp1(xxxx);
// T temp2(xxxx);
// func(temp1, temp2);
// 正所谓没有对比就没有伤害，为什么对于void func(int, double)可以直接使用func(12, 3.14)调用，而不用
// int a = 12;
// double b = 3.14;
// func(a, b);
// 这便是字面量的威力，可以直接更加直观的字符串信息进行隐式的类型转换和函数调用，从而使得逻辑更为直观。
// 那么用户自定义的类是否也可以实现这种通过字面量直接进行参数传递的效果呢？
// 答案在C++11是肯定的。这便是operator "" _X字面量操作符函数的引入。

typedef unsigned char uint8;
struct RGBA {
  uint8 r;
  uint8 g;
  uint8 b;
  uint8 a;
  RGBA(uint8 R, uint8 G, uint8 B, uint8 A = 0) : r(R), g(G), b(B), a(A) {}
};

// 数会主动解析以_C为后缀的字符串，并返回一个RGBA右值对象
RGBA operator"" _C(const char* col, size_t n) {
  std::cout << "This round target content:" << static_cast<std::string>(col)
            << " with char-num:" << n << std::endl;
  const char* p = col;
  const char* end = col + n;
  const char *r, *g, *b, *a;
  r = g = b = a = nullptr;

  for (; p != end; ++p) {
    if (*p == 'r')
      r = p;
    else if (*p == 'g')
      g = p;
    else if (*p == 'b')
      b = p;
    else if (*p == 'a')
      a = p;
  }

  if ((r == nullptr) || (g == nullptr) || (b == nullptr))
    throw;
  else if (a == nullptr)
    return RGBA(atoi(r + 1), atoi(g + 1), atoi(b + 1));
  else
    return RGBA(atoi(r + 1), atoi(g + 1), atoi(b + 1), atoi(a + 1));
}

std::ostream& operator<<(std::ostream& out, RGBA& col) {
  return out << "r: " << (int)col.r << ",g: " << (int)col.g
             << ",b: " << (int)col.b << ",a: " << (int)col.a << std::endl;
}

// blend函数形参是右值引用，是专门为RGBA字面量化设计的，普通的RGBA对象是无法调用的
void blend(RGBA&& col1, RGBA&& col2) {
  std::cout << "rvluae version blend" << std::endl;
  std::cout << col1 << col2 << std::endl;
}

// 函数重载，这里是可以接受正常左值类型参数的函数入口
void blend(RGBA& col1, RGBA& col2) {
  std::cout << "lvluae version blend" << std::endl;
  std::cout << col1 << col2 << std::endl;
}

void testN2() {
  // 传统逻辑下，需要至少三行代码才能搞定一次blend测试：
  RGBA col1(255, 240, 155);
  RGBA col2(15, 255, 10, 7);
  blend(col1, col2);
  // lvluae version blend
  // r: 255,g: 240,b: 155,a: 0
  // r: 15,g: 255,b: 10,a: 7

  // 定义了字面量操作符函数后，只需要一句代码即可搞定：
  blend("r255 g240 b155"_C, "r15 g255 b10 a7"_C);
  // This round target content:r15 g255 b10 a7 with char-num:15
  // This round target content:r255 g240 b155 with char-num:14
  // rvluae version blend
  // r: 255,g: 240,b: 155,a: 0
  // r: 15,g: 255,b: 10,a: 7
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}