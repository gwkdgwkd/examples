#include <iostream>
#include <string>

using namespace std;

// 透传了解inline的里里外外

// 除了免除函数调用成本外，编译器还会代码进行优化。
// 没有免费的午餐，inline函数也不例外。
// 对此函数的每一个调用都以函数本体替换，可能增加你的目标码大小。
// inline只是对编译器的一个申请，不是强制命令。
// 这项申请可以隐喻提出，也可以明确提出，
// 隐喻方式是将函数定义与class定义内。
class Person {
 public:
  // 一个隐喻的inline申请，通常是成员函数：
  int age() const { return theAge; }
  inline void set();

 private:
  int theAge;
  // friend函数也可以被定义于class内，
  // 如果那样，它们也是被隐喻声明的inline：
  friend int getAge(const Person& p) { return p.theAge; }
};
// 明确声明inline函数的做法则是在其定义前加上inline关键字：
inline void Person::set() {}

// template <typename T>
// inline const T& std::max(const T& a, const T& b) {
//   return a < b ? b : a;
// }
// inline函数和template两者通常都被定义于头文件内。
// 让好多人认为function template一定必须是inline，
// 这个结论不但无效而且有可能有害。
// inline函数一般被置于头文件内，
// 因为大多数编译环境在编译过程中进行inline，
// 而为了将函数调用替换为函数本体，编译器必须知道函数长什么样子。
// template通常也被置于头文件内，因为一旦被使用，
// 编译器为了将它具现化，需要知道它长什么样子。

// 大部分编译器拒绝将太过复杂（例如带有循环或递归）的函数inline，
// 而对所有virtual函数（除非是最平淡无奇的）inline，也会失败。
// 因为virtual意味等待，直到运行期才确定调用哪个函数，
// 而inline意味执行前，先将调用动作替换为函数体。
// 如果编译器不知道调用哪个函数，怎么替换？
// 一个表面上看似inline的函数是否真是inline，
// 取决于你的编译环境，主要取决于编译器。
// 幸运的是大多数编译器提供了诊断级别，无法inline会给警告信息。

// 有时候虽然编译器有意愿inline某个函数，
// 但还是可能为该函数生成一个函数本体。
// 如果程序要取某个函数的地址，
// 编译器通常必须为此函数生成一个outline函数本体。
// 毕竟编译没有能力给出一个指向并不存在的函数的指针。
// 编译器通常不对通过函数指针而进行的调用进行inline，
// 这意味着对inline函数的调用有可能被inline，
// 也可能不被inline，取决于调用的实施方式。
// 即使从未使用函数指针，也可能不inline。
// 因为程序员并非唯一要求函数指针的人，
// 有时候编译会生成构造函数和析构函数的outline副本，
// 如此来获得指向函数的指针。

// 实际上构造函数和析构函数往往是inline的糟糕候选人：
class Base {
 private:
  string bm1, bm2;
};
class Derived : public Base {
 public:
  // 看起来是inline的绝佳候选人，因为根本不含任何代码，但不是。
  Derived() {}
  // C++对于对象被创建和被销毁时发生什么事做了各式各样的保证：
  // 1.使用new，动态创建的对象被其构造函数自动初始化；
  // 2.当使用delete，对应的析构函数会被调用。
  // 3.当创建一个对象，每个base class及每个成员变量都会被自动构造；
  // 4.当销毁一个对象，反向程序的析构行为也会自动发生。
  // 这些情况中C++描述了什么一定发生，但没说如何发生。
  // 这有编译器来负责，所以编译器会生成代码并安插到构造函数和析构函数中。
  // 所以Derived看起来是空的，但是不一定是inline函数。

 private:
  string dm1, dm2, dm3;
};

// 将函数声明为inline，将无法随程序库升级而升级。
// 如果改变inline函数，所有用到f的客户端程序必须重新编译。
// 如果是non-inline函数，一旦有任何修改，
// 客户端只需要重新连接就好，远比重新编译的负担少很多。
// 如果采用动态库，升级函数甚至可以不知不觉地被使用。

// 大部分调试器面对inline函数都束手无策。
// 毕竟如何在一个并不存在的函数内设置断点呢？

// 请记住：
// 1.将大多数函数inline限制在小型、被频繁调用的函数身上。
// 2.这可使日后的调试过程和二进制升级更容易，
//   也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。
// 3.不要只因为function template出现在头文件，就将它们声明为inline。

int main() { return 0; }