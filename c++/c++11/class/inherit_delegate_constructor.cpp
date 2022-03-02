#include <iostream>

namespace n1 {
// 继承构造函数的引入原因：

// 基类的构造函数很多
class Base {
 public:
  Base(int i) { std::cout << "Base(int i)" << std::endl; }
  Base(double d, int i) { std::cout << "Base(double d, int i)" << std::endl; }
  Base(float f, int i, const char* s) {
    std::cout << "Base(float f, int i, const char* s)" << std::endl;
  }
};

// 那么子类的构造函数想要实现同样多的构造接口，必须一一调用基类的构造函数，有点麻烦
class Derived1 : public Base {
 public:
  Derived1(int i) : Base(i) { std::cout << "Derived1(int i)" << std::endl; }
  Derived1(double d, int i) : Base(d, i) {
    std::cout << "Derived1(double d, int i)" << std::endl;
  }
  Derived1(float f, int i, const char* s) : Base(f, i, s) {
    std::cout << "Derived1(float f, int i, const char* s)" << std::endl;
  }
};

// 于是乎C++11引入继承构造函数:
class Derived2 : public Base {
 public:
  using Base::Base;  // 使用Base中的构造函数
  // 这种继承构造函数，如果没有被使用，是不会生成目标代码的，会更节省目标代码空间。

  // 但是：
  // 1.C++11中的继承构造函数特性最有用的场合就是，派生类只是在基类的基础上添加了几个新的接口,
  //   这个时候继承构造函数最能够展现威力。
  //   缺陷就是, 继承构造函数只会初始化基类中的成员变量, 对于派生类中的变量无能为力。
  // 2.不过，可以配合另一个C++11中的新特性: 数据成员就地初始化，来解决这个问题。
  //   但是，这种做法导致子类的成员不能在构造函数的参数列表中体现出来。
  // 3 一旦子类继承了基类的构造函数，那么子类就不会自动生成默认构造函数。
  //   所以，如果子类新增了自己的成员变量，乖乖回到以前的初始化列表方法中吧。
};

void testN1() {
  Derived1 d1(5);
  Derived1 d2(2.5, 6);
  Derived1 d3(2.5f, 7, "nihao");
  // Base(int i)
  // Derived1(int i)
  // Base(double d, int i)
  // Derived1(double d, int i)
  // Base(float f, int i, const char* s)
  // Derived1(float f, int i, const char* s)

  Derived2 d4(5);
  Derived2 d5(2.5, 6);
  Derived2 d6(2.5f, 7, "nihao");
  // Base(int i)
  // Base(double d, int i)
  // Base(float f, int i, const char* s)

  // Derived2 d7;  // 已经没有默认构造函数了
}
}  // namespace n1

namespace n2 {
// 托构造函数允许类中的一个构造函数通过初始化列表方式来调用同一个类中的另一个构造函数。
// 委托构造函数：允许在同一个类中一个构造函数可以调用另外一个构造函数，从而可以在初始化时简化变量的初始化。

class A1 {  // 普通写法
 public:
  int max;
  int min;
  int middle;
  A1(int my_max) {
    max = my_max > 0 ? my_max : 10;
    std::cout << "A1(int my_max)" << std::endl;
  }
  A1(int my_max, int my_min) {
    max = my_max > 0 ? my_max : 10;
    min = my_min > 0 && my_min < max ? my_min : 1;
    std::cout << "A1(int my_max, int my_min)" << std::endl;
  }
  A1(int my_max, int my_min, int my_middle) {
    max = my_max > 0 ? my_max : 10;
    min = my_min > 0 && my_min < max ? my_min : 1;
    middle = my_middle < max && my_middle > min ? my_middle : 5;
    std::cout << "A1(int my_max, int my_min, int my_middle)" << std::endl;
  }
};

class A2 {  // 委托构造函数的写法
 public:
  int max;
  int min;
  int middle;
  A2(int my_max) {
    max = my_max > 0 ? my_max : 10;
    std::cout << "A2(int my_max)" << std::endl;
  }
  A2(int my_max, int my_min) : A2(my_max) {
    min = my_min > 0 && my_min < max ? my_min : 1;
    std::cout << "A2(int my_max, int my_min)" << std::endl;
  }
  A2(int my_max, int my_min, int my_middle) : A2(my_max, my_min) {
    middle = my_middle < max && my_middle > min ? my_middle : 5;
    std::cout << "A2(int my_max, int my_min, int my_middle)" << std::endl;
  }
};

void testN2() {
  A1 a1(1);        // A1(int my_max)
  A1 a2(1, 2);     // A1(int my_max, int my_min)
  A1 a3(1, 2, 3);  // A1(int my_max, int my_min, int my_middle)

  A2 a4(1);  // A2(int my_max)
  A2 a5(1, 2);
  // A2(int my_max)
  // A2(int my_max, int my_min)
  A2 a6(1, 2, 3);
  // A2(int my_max)
  // A2(int my_max, int my_min)
  // A2(int my_max, int my_min, int my_middle)
}
}  // namespace n2

namespace n3 {
// 使用委托构造函数需要注意：使用了代理构造函数就不能用类成员初始化了，即不能同时委派构造函数和使用初始化列表。
class A {
 public:
  A() {}
  // member initialization here, no delegate
  A(std::string str) : s(str) {}

  // 调用了委托构造函数，不能用类成员初始化了
  // error C3511: a call to a delegating constructor shall be the only member-initializer
  // A(string str, double dbl) : A(str), m_double(dbl) {}

  // 只能通过成员赋值来初始化
  A(std::string str, double dbl) : A(str) { d = dbl; }
  double d{1.0};
  std::string s;
};

void testN3() { A a("nihao", 1.8); }
}  // namespace n3

namespace n4 {
// 不要形成委托环
struct A {
  int i, c;
  A() : A(2) { std::cout << "A()" << std::endl; }
  A(int i) : A('c') { std::cout << "A(i)" << std::endl; }
  A(char c) : A(2) { std::cout << "A(c)" << std::endl; }
};
// A中，A()、A(int)和A(char)都依赖于别的构造函数，形成环委托构造关系。
// 这样的代码通常会导致编译错误。能编译通过，为什么？但运行会崩溃

void testN4() { A a('c'); }
}  // namespace n4

int main(int argc, char* argv[]) {
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