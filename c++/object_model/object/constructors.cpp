#include <iostream>

using namespace std;

// 2.1 Default Constructor的构建操作

// 默认构造函数在需要（是编译器需要，不是程序需要。程序需要，是程序员的责任）的时候被编译器产生出来。
// 如果没有任何用户定义构造函数，那么会有一个默认构造函数暗中被声明出来，但没啥作用。

// 有用的默认构造函数的四种情况：

// 第一、“带有Default constructor”的对象成员
// 如果一个类没有任何构造函数，但它内含一个对象成员，而后者有默认构造函数，那么这个类的隐式默认构造函数就是有用的。
// 编译器需要为这个类合成出一个默认构造函数，不过这个合成操作只有在构造函数真正需要被调用时才会发生。
// 在C++各个不同的编译模块中，编译器如何避免合成出多个默认构造函数呢？
// 解决方法是把合成的默认构造函数、拷贝构造函数、析构函数、赋值操作符都以inline方式完成。
// 一个内联函数有静态链接，不会被模块以外看到。如果函数太复杂，不时候做成内联，就会合成出一个explicit non-inline static实体。
class Foo1 {
 public:
  Foo1() { cout << "Foo1" << endl; }
  Foo1(int);
};
class Bar1 {
 public:
  // 被合成的Bar默认构造函数内必要的代码，能够调用Foo的默认构造函数来处理Bar::foo，但它并不产生任何代码来初始化Bar::str。
  // 将Bar::foo初始化是编译器的责任，将Bar::str初始化则是程序员的责任。
  // 被合成的默认构造函数看起来像：inline Bar::Bar() { foo.Foo::Foo(); }
  Foo1 foo;
  char *str;
};
void foo_bar1() { Bar1 bar; }
class Bar2 {
 public:
  Bar2() {
    // 定义了默认构造函数，初始化了str。现在程序的需求满足了，但是编译器还需要初始化对象成员foo。
    // 由于默认构造函数已经被明确定义出来了，编译没办法合成第二个。
    // 编译器的行动是：“如果类A内含一个或一个以上的对象成员，那么类A的每一个构造函数必须调用每一个对象成员的默认构造函数”
    // 编译器会扩展已存在的构造函数，在其中安插一些码，使得用户代码在被执行之前，先调用默认构造函数。
    // foo.Foo::Foo();  // 安插的代码
    cout << "Bar2" << endl;
    str = 0;
  }
  Foo1 foo;
  char *str;
};
void foo_bar2() { Bar2 bar; }
// 如果有多个对象成员都要求初始化操作，C++要求以“对象成员在class中的声明次序”来调用各个构造函数。
// 这一点由编译器完成，它为每一个构造函数安插代码，这些代码将被安插在explicit user code之前。
class Dopey {
 public:
  Dopey() { cout << "Dopey" << endl; }
};
class Sneezy {
 public:
  Sneezy(int) { cout << "Sneezy int" << endl; }
  Sneezy() { cout << "Sneezy" << endl; }
};
class Bashful {
 public:
  Bashful() { cout << "Bashful" << endl; }
};
class Snow_White1 {
 public:
  // 没有定义默认构造函数，有用的构造函数被合成出来，依存调用
  Dopey dopey;
  Sneezy sneezy;
  Bashful bashful;

 private:
  int mumble;
};
class Snow_White2 {
 public:
  // 如果定义了默认构造函数，编译器会扩张默认构造函数
  Snow_White2() : sneezy(1024) { mumble = 2048; }
  Dopey dopey;
  Sneezy sneezy;
  Bashful bashful;

 private:
  int mumble;
};

// 第二、“带有Default constructor”的Base Class
// 类似的道理，如果一个没有任何构造函数的类派生自一个“带有默认构造函数”的base class，那么这个派生类的默认构造函数会是有用的，比被合成出来。
// 它将调用上一层基类的默认构造函数（根据它们的声明次序）。
// 对一个后继派生的类而言，这个合成的构造函数和一个被明确提供的默认构造函数没什么差异。
class BaseA {
 public:
  BaseA() { cout << "BaseA" << endl; }
};
class BaseB {
 public:
  BaseB() { cout << "BaseB" << endl; }
};
class Derived1 : public BaseB, BaseA {};
// 如果设计这提供多个构造函数，但其中没有默认构造函数，编译器会扩张现有的每一个构造函数，将“用以调用所有必要的构造函数”的代码加进去。
class Derived2 : public BaseB, BaseA {
 public:
  // 不会合成一个新的默认构造函数，这是因为其它“有用户提供的构造函数”存在的缘故。
  Derived2(int) {}
};
// 如果同时亦存在着“带有默认构造函数”的对象成员，那些默认构造函数也会被调用，在所有基类构造函数被调用之后。
class Derived3 : public BaseA, BaseB {
 public:
  Dopey dopey;
  Sneezy sneezy;
  Bashful bashful;
};

// 第二、“带有一个Virtual Function”的Class

int main() {
  foo_bar1();  // Foo1  如果不调用这个函数，那么不会有打印

  foo_bar2();
  // Foo1
  // Bar2

  Snow_White1 sw1;
  // Dopey
  // Sneezy
  // Bashful

  Snow_White2 sw2;
  // Dopey
  // Sneezy int
  // Bashful

  Derived1 d1;
  // BaseB
  // BaseA

  // Derived2 d2;  // 编译不过

  Derived3 d3;
  // BaseA
  // BaseB
  // Dopey
  // Sneezy
  // Bashful

  return 0;
}