#include <iostream>

// 条款12：使用override关键字声明覆盖的函数

// 在C++面向对象的世界里，涉及的概念有类，继承，虚函数。
// 这个世界最基本的概念是派生类的虚函数重写基类同名函数。
// 令人遗憾的是虚函数重写可能一不小心就错了。
// 似乎这部分语言的设计理念是不仅仅要遵守墨菲定律，还应该尊重它。

// 虽然重写（overriding）听起来像重载（overloading），然而两者完全不相关，
// 所以澄清一下，正是虚函数重写机制的存在，才可以通过基类的接口调用派生类的成员函数。

namespace n1 {
// 要想重写一个函数，必须满足下列要求：
// 1.基类函数必须是virtual；
// 2.基类和派生类函数名必须完全一样（除非是析构函数）；
// 3.基类和派生类函数形参类型必须完全一样；
// 4.基类和派生类函数常量性const特性必须完全一样；
// 5.基类和派生类函数的返回值和异常说明必须兼容。
// 除了这些C++98就存在的约束外，C++11又添加了一个：
// 1.函数的引用限定符（reference qualifiers）必须完全一样，
//   成员函数的引用限定符是C++11很少抛头露脸的特性，所以如果从没听过它无需惊讶，
//   它可以限定成员函数只能用于左值或者右值，成员函数不需要virtual也能使用它们。

class Base {
 public:
  void doWork() & {  // 只有*this为左值的时候才能被调用
    std::cout << "void Base::doWork() &" << std::endl;
  }
  void doWork() && {  // 只有*this为右值的时候才能被调用
    std::cout << "void Base::doWork() &&" << std::endl;
  }
};

namespace test1 {
Base makeBase() {  // 工厂函数，返回右值
  return Base();
}

void func() {
  Base w;  // 普通对象，左值

  w.doWork();             // void Base::doWork() &
  std::move(w).doWork();  // void Base::doWork() &&
  makeBase().doWork();    // void Base::doWork() &&
}
}  // namespace test1

namespace test2 {
class Derived : public Base {
 public:
  // 如果基类的虚函数有引用限定符，派生类的重写就必须具有相同的引用限定符，
  // 如果没有，派生类声明中声明的那个函数也会存在，但是不会重写父类的任何函数：
  void doWork() { std::cout << "void Derived::doWork()" << std::endl; }
};

void func() {
  Derived d;
  d.doWork();  // void Derived::doWork()

  Base *pb = &d;
  pb->doWork();  // void Base::doWork() &

  [] { return Base(); }().doWork();  // void Base::doWork() &&

  // Derived没有重写Base的虚函数，而是重新定义了doWork。
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n1

namespace n2 {
namespace test1 {
class Base {
 public:
  virtual void mf1() const;
  virtual void mf2(int x);
  virtual void mf3() &;
  void mf4() const;
};

// Derived没有任何虚函数重写，但代码是合法的，而且编译器不一定会有warnings：
class Derived1 : public Base {
 public:
  // mf1在Base基类声明为const，但是Derived派生类没有这个常量限定符：
  virtual void mf1();

  // mf2在Base声明为接受int参数，但是在Derived声明为接受unsigned int参数：
  virtual void mf2(unsigned int x);

  // mf3在Base基类声明为左值引用限定，但是在Derived派生类声明为右值引用限定：
  virtual void mf3() &&;

  // mf4在Base基类没有声明为virtual虚函数：
  void mf4() const;
};

// 由于正确声明派生类的重写函数很重要，但很容易出错，C++11提供一个方法，
// 可以显式地指定一个派生类函数是基类版本的重写，将它声明为override：
class Derived2 : public Base {
 public:
  // 下面的重写函数都是错的，这正是需要达到的目的：
  // virtual void mf1() override;
  // virtual void mf2(unsigned int x) override;
  // virtual void mf3() && override;
  // virtual void mf4() const override;
};
}  // namespace test1

namespace test2 {
class Base {
 public:
  virtual void mf1() const;
  virtual void mf2(int x);
  virtual void mf3() &;
  virtual void mf4() const;
};

// 比起让编译器通过warnings提示实际没有重写，不如给派生类重写函数全都加上override：
class Derived : public Base {
 public:
  virtual void mf1() const override;
  virtual void mf2(int x) override;
  virtual void mf3() & override;
  void mf4() const override;  // 可以添加virtual，但不是必要
};

}  // namespace test2
}  // namespace n2

namespace n3 {
// C++已经有很多关键字，但C++11又引入了两个上下文关键字，override和final，
// 向虚函数添加final可以防止派生类重写，final也能用于类，这时这个类不能用作基类。
// 这两个关键字的特点是它们是保留的，它们只是位于特定上下文才被视为关键字。
// 对于override，它只在成员函数声明结尾处才被视为关键字。
// 这意味着如果你以前写的代码里面已经用过override这个名字，那么换到C++11无需修改代码：
class Warning {  // C++98潜在的传统类代码
 public:
  void override();  // C++98和C++11都合法且含义相同
};
}  // namespace n3

namespace n4 {
namespace test1 {
struct Widget {};
// 想写一个函数只接受左值实参，声明一个non-const左值引用形参：
void doSomething(Widget &w);  // 只接受左值Widget对象
// 如果想写一个函数只接受右值实参，声明一个右值引用形参：
void doSomething(Widget &&w);  // 只接受右值Widget对象
}  // namespace test1

class Widget {
 public:
  Widget() { std::cout << "Widget()" << std::endl; }
  Widget(const Widget &) { std::cout << "Widget(const Widget&)" << std::endl; }
  Widget(Widget &&) { std::cout << "Widget(Widget &&)" << std::endl; }
};

namespace test2 {
// 成员函数的引用限定可以很容易的区分一个成员函数被哪个对象（即*this）调用。
// 它和在成员函数声明尾部添加const很相似，暗示了调用这个成员函数的对象是const的。

class A {
 public:
  Widget &data() { return w; }

 private:
  Widget w;
};
A makeA() { return A(); };

void func() {
  A a;  // Widget()

  // A::data函数的返回值是一个左值引用, 因为左值引用是左值，
  // 所以w1是从左值初始化的，因此w1由a.w拷贝构造而得：
  auto w1 = a.data();  // Widget(const Widget&)

  // A是makeA返回的临时对象（即右值），所以将其中的Widget进行拷贝纯属浪费，
  // 最好是移动，但是因为data返回左值引用，C++的规则要求编译器不得不生成一个拷贝：
  auto w2 = makeA().data();
  // Widget()
  // Widget(const Widget&)
  // 这其中有些优化空间，被称作as if rule，但是依赖编译器使用优化规则就有点傻，
  // as if rule简单来说就是在不影响程序的外在表现情况下做一些改变。
}
}  // namespace test2

namespace test3 {
// 需要的是指明当data被右值A对象调用的时候结果也应该是一个右值：
class A {
 public:
  // 注意data重载的返回类型是不同的，左值引用重载版本返回一个左值引用（即一个左值）：
  Widget &data() & { return w; }
  // 右值引用重载返回一个临时对象（即一个右值）：
  Widget data() && { return std::move(w); }

 private:
  Widget w;
};
A makeA() { return A(); };
void func() {
  A a;  // Widget()

  auto w1 = a.data();  // Widget(const Widget&)

  auto w2 = makeA().data();
  // Widget()
  // Widget(Widget &&)
}
}  // namespace test3
}  // namespace n4

// 要记住的东西：
// 1.为重写函数加上override；
// 2.成员函数引用限定让代码可以区别对待左值对象和右值对象，即*this。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n4::test2::func();
      break;
    case 2:
      n4::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}