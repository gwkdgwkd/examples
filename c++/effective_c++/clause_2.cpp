#include <iostream>

// 尽量以const，enum，inline替换#define
// 或许可以改为宁可以编译器替换预处理器，因为#define不是语言的一部分。

namespace n1 {
#define ASPECT_PATIO 1.653
// ASPECT_PATIO从未被编译器看见，在开始编译之前就被预处理器替换了。
// 于是ASPECT_PATIO有可能没进入符号表内，当编译错误时会带来困惑，
// 提到的是1.653而不是ASPECT_PATIO，特别是在一个别人写的头文件内。

// 解决办法是以一个常量替换上述的宏：
const double AspectRatio = 1.653;
// AspectRatio肯定会被编译器看到，当然就会进入记号表。
// 此外对于浮点常量，使用常量可能比使用#define导致小量的码，
// 因为ASPECT_PATIO盲目替换，可能导致目标码出现多份，常量不会这样。

// 以常量替换#define，有两种特殊情况：
// 1.由于常量指针通常被放在头文件内（以便被不同的源码包含），
//   因此有必要将指针（而不只是指针所指之物）声明为const；
// 2.class专属常量，为了将常量的作用域限制于class内，必须是成员，
//   为了确保只有一份实体，必须让它成为static成员。
namespace test1 {
class A {
 private:
  static const int num = 5;  // 常量声明式
  int a[num];
};
// 通常C++要求变量提供定义式，但如果它同时满足下面3点，则需特殊处理：
// 1.class专属常量；
// 2.是static的；
// 3.为整数类型（int、char、bool）。
// 只要不取它们的地址，或就要写定义式，则必须另外提供定义式：
const int A::num;
// NumTurns的定义，放进一个实现文件而非头文件。
// 声明时已经获得初值，定义时不可以再像下面一样设置初值：
// const int A::num = 6;

// #define无法创建一个class的专属常量，因为#define并不重视作用域。
// 一旦被定义，在其后的编译过程中都有效（除非#undef)。
// 这意味着#define不仅不能够定义class专属常量，也不能提供任何封装型。
}  // namespace test1

namespace test2 {
// 旧的编译器也许不支持上面的语法，不允许static成员在其声明时获得初值。
// 此外所谓的in-class初值设定也只允许对整数常量进行。
// 如果编译器不支持，那么可以将初值放在定义式：
class A {
 private:
  static const int num;  // 位于头文件内
};
const int A::num = 5;  // 位于实现文件内
}  // namespace test2

namespace test3 {
// 唯一例外的是在class编译期间需要一个class常量值，
// 比如GamePlayer::NumTurns，编译器坚持必须在编译期间知道数组大小，
// 可以使用所谓的the enum hack补偿做法，GamePlayer可以定义如下：
class A {
 private:
  enum { num = 5 };
  int scores[num];
};
// the enum hack的行为比较像#define而不像const，有时候正是想要的。
// 例如取const的地址是合法的，但取enum的地址就不合法，
// 而取一个#define的地址通常也不合法。
// 如果不想让别人获得一个指针或引用指向某个常量，enum可以帮助实现这个约束。
// 使用the enum hack的第二个理由纯粹是为了实用主义，
// 许多代码用了它，看到时必须认识，事实上，它是模板元编程的基础技术。
}  // namespace test3
}  // namespace n1

namespace n2 {
void f(int a) { std::cout << a << std::endl; }
// 另一个常见的#define误用情况是以它实现宏。
// 宏看起来想函数，但不会招致函数调用带来的额外开销。
#define MAX(a, b) f((a) > (b) ? (a) : (b))
// 写这种宏，必须记住宏中的所有实参都要加上小括号，否则产生歧义。
// 就算宏都加上了小括号，还会发送不可思议的事。
void func1() {
  int a = 5, b = 0;
  MAX(++a, b);       // a被累加两次
  MAX(++a, b + 10);  // a被累加一次

  std::cout << "a = " << a << ", b = " << b << std::endl;
  // 7
  // 10
  // a = 8, b = 0
}

// 内联函数可以获得宏带来的效率以及一般函数的所有可预料行为和类型安全型，
// callWithMax是个真正的函数，遵守作用域和访问规则：
template <typename T>
inline void Max(const T& a, const T& b) {
  f(a > b ? a : b);  // 不需要加上括号，也不需要操心参数被求值多次
}
void func2() {
  int a = 5, b = 0;
  Max(++a, b);
  Max(++a, b + 10);
  std::cout << "a = " << a << ", b = " << b << std::endl;
  // 6
  // 10
  // a = 7, b = 0
}
}  // namespace n2

// 有了const、enum和inline，
// 对预处理器（特别是#define）的需求降低了，但并非完全消除。
// #include仍然是必须品，而#ifdef/#ifndef也继续扮演控制编译的重要角色，
// 还不到预处理器全面隐退的时候。

// 请记住：
// 1.对于单纯常量，最好以const对象或enum替换#define；
// 2.对于形似函数的宏，最好改用inline函数替换#define。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func1();
      break;
    case 1:
      n2::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}