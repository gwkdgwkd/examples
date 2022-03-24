#include <iostream>

using namespace std;

void f(int a) { cout << a << endl; }

// 尽量以const,enum,inline替换#define

// 或许可以改为“宁可以编译器替换预处理器”，因为#define不被视为语言的一部分。

#define ASPECT_PATIO 1.653
// ASPECT_PATIO也许从未被编译器看见；也许在编译器开始处理源码之前它就被预处理器移走了。
// 于是ASPECT_PATIO有可能没进入符号表内。
// 于是运用此常量但获得一个编译错误时，会带来困惑，提到的是1.653而不是ASPECT_PATIO。
// 特别是在一个别人写的头文件内。

const double AspectRatio = 1.653;  // 解决办法是以一个常量替换上述的宏
// AspectRatio肯定会被编译器看到，当然就会进入记号表。
// 此外对于浮点常量，使用常量可能比使用#define导致小量的码，
// 因为ASPECT_PATIO盲目替换，可能导致目标码出现多份，常量不会这样。

// 以常量替换#define，有两种特殊情况：
// 1.由于常量指针通常被放在头文件内（以便被不同的源码包含），
//   因此有必要将指针（而不只是指针所指之物）声明为const。
// 2.class专属常量，为了将常量的作用域限制于class内，必须让它成为class的一个成员；
//   为了确保只有一份实体，必须让它成为static成员。
class GamePlayer {
 private:
  static const int NumTurns = 5;  // 常量声明式
  int scores[NumTurns];
};
// 通常C++要求对所使用的任何东西提供一个定义式，
// 但如果它是个class专属常量又是static且为整数类型（int、char、bool）,则需特殊处理。
// 只要不取它们的地址，或就要写定义式，则必须另外提供定义式：
const int GamePlayer::NumTurns;  // NumTurns的定义，放进一个实现文件而非头文件。
// const int GamePlayer::NumTurns = 6;  // 声明时获得初值，定义时不可以再设初值。

// #define无法创建一个class的专属常量，因为#define并不重视作用域。
// 一旦被定义，在其后的编译过程中都有效（除非#undef)。
// 这意味着#define不仅不能够定义class专属常量，也不能提供任何封装型。

// 旧的编译器也许不支持上面的语法，不允许static成员在其声明时获得初值。
// 此外所谓的“in-class初值设定”也只允许对整数常量进行。
// 如果编译器不支持，那么可以将初值放在定义式：
class ConstEstimate {
 private:
  static const double FudgeRactior;  // 位于头文件内
};
const double ConstEstimate::FudgeRactior = 1.35;  // 位于实现文件内

// 唯一例外的是在class编译期间需要一个class常量值，
// 比如GamePlayer::NumTurns，编译器坚持必须在编译期间知道数组大小，
// 可以使用所谓的"the enum hack"补偿做法。GamePlayer可以定义如下：
class GamePlayer1 {
 private:
  enum { NumTurns = 5 };
  int scores[NumTurns];
};
// the enum hack的行为比较像#define而不像const，有时候正是想要的。
// 例如取const的地址是合法的，但取enum的地址就不合法，而取一个#define的地址通常也不合法。
// 如果不想让别人获得一个指针或引用指向你的某个常量，enum可以帮助实现这个约束。
// 使用the enum hack的第二个理由纯粹是为了实用主义，许多代码用了它，看到时必须认识。
// 事实上，它是模板元编程的基础技术。

// 另一个常见的#define误用情况是以它实现宏。宏看起来想函数，但不会招致函数调用带来的额外开销。
#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b))
// 写这种宏，必须记住宏中的所有实参都要加上小括号，否则产生歧义。
// 就算宏都加上了小括号，还会发送不可思议的事。
// 内联函数可以获得宏带来的效率以及一般函数的所有可预料行为和类型安全型。
template <typename T>
inline void callWithMax(const T& a, const T& b) {  // 产生一整群函数
  f(a > b ? a : b);  // 不需要加上括号，也不需要操心参数被求值多次
}
// callWithMax是个真正的函数，遵守作用域和访问规则。

// 有了const、enum和inline，对预处理器（特别是#define）的需求降低了，但并非完全消除。
// #include仍然是必须品，而#ifdef/#ifndef也继续扮演控制编译的重要角色，还不到预处理器全面隐退的时候。

// 请记住：
// 对于单纯常量，最好以const对象或enum替换#define。
// 对于形似函数的宏，最好改用inline函数替换#define。

int main() {
  int a = 5, b = 0;
  CALL_WITH_MAX(++a, b);       // a被累加两次
  CALL_WITH_MAX(++a, b + 10);  // a被累加一次

  cout << "a = " << a << ", b = " << b << endl;
  // 7
  // 10
  // a = 8, b = 0

  a = 5;
  b = 0;
  callWithMax(++a, b);
  callWithMax(++a, b + 10);
  cout << "a = " << a << ", b = " << b << endl;
  // 6
  // 10
  // a = 7, b = 0

  return 0;
}