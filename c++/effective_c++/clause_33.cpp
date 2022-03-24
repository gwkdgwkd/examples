#include <iostream>

using namespace std;

// 避免遮掩继承而来的名称

// 其实和继承无关，和作用域有关。
// C++的名称遮掩规则所做的唯一事情就是：遮掩名称。至于名称是否类型相同，并不重要。
int x;
void sonmeFunc() {
  double x;
  cin >> x;
}

// derived class作用域被嵌套在base class作用域内
class Base {
 private:
  int x;

 public:
  virtual void mf1() = 0;  // pure virtual
  virtual void mf1(int) {}
  virtual void mf2() {}  // impure virtual
  void mf3() {}          // non-virtual
  void mf3(double) {}
};
class Derived1 : public Base {
 public:
  // Base class内所有名为mf1和mf3的函数都被derived class内的mf1和mf3函数遮掩掉了。
  // 从名称查找观点来看，Base::mf1和Base::mf3不再被Derived继承。
  // 不论函数的参数类型是否相同、函数是否为virtual，上面的规则都适用。
  virtual void mf1() {}
  void mf3() {}
  void mf4() {
    // 首先查找local作用域，mf4函数内，没找到任何东西名为mf2。
    // 于是查找其外围作用域，也就是class Derived覆盖的作用域。还是没找到mf2。
    // 于是再往外围移动，Base class，在那找到一个名给mf2的东西了，停止查找。
    mf2();
    // 如果Base内还是没有mf2，查找还会继续下去，首先是Base所在的namespace，再然后是global作用域。
  };
};

// 使用public继承而又不继承那些重载函数，就违反了base和derived class之间的is-a关系，is-a关系是public继承的基石。
// 因此几乎总会想要推翻(override)C++对“继承而来的名称的缺省遮掩行为。
class Derived2 : public Base {
 public:
  // 如果base class有重载函数，而derived希望重新定义或覆写（推翻）其中一部分，
  // 那么你必须为原本会被遮掩的每个名称用using引入derived class
  // 让Base class内名为mf1和mf3的所有东西在Derived作用域内都可见，并且为public
  using Base::mf1;
  using Base::mf3;
  virtual void mf1() {}
  void mf3() {}
  void mf4() {}
};

// 有时并不想继承base class的所有函数，这是可以理解的。在public继承不能发生，违背了public继承的is-a关系。
// 但在private继续下，是有意义的。此时using声明排不上用场。
class Derived3 : private Base {
 public:
  virtual void mf1() { Base::mf1(5); }  // 转交函数，暗自成为inline
  // inline转交函数（forwarding function）的另一个用途是为那些不支持using声明的老旧编译器另辟一条新路，
  // 将继承而得的名称汇入derived class作用域内。
};

// 当继承结合template时，有将面对“继承名称被遮掩”的一个全然不同的形式。

// 请记住：
// derived class内的名称会遮掩base class内的名称。在public继承下从来不会有人希望如此。
// 为了让被遮掩的名称再见天日，可以使用using声明或转交函数（forwarding functions）。

int main() {
  int x;

  Derived1 d1;
  d1.mf1();  // 调用Derived::mf1
  // d1.mf1(x);  // 错误，Derived::mf1遮掩了Base::mf1
  d1.mf2();  // 调用Base::mf2
  d1.mf3();  // 调用Derived::mf3
  // d1.mf3(x);  // 错误，Derived::mf3遮掩了Base::mf3

  Derived2 d2;
  d2.mf1();   // 调用Derived::mf1
  d2.mf1(x);  // 调用Base::mf1
  d2.mf2();   // 调用Base::mf2
  d2.mf3();   // 调用Derived::mf3
  d2.mf3(x);  // 调用Base::mf3

  Derived3 d3;
  d3.mf1();  // 调用Derived::mf1
  // d3.mf1(x);  // 错误，Base::mf1()被遮掩了

  return 0;
}