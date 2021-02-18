#include <iostream>

using namespace std;

// 继承构造函数的引入原因：如果基类的构造函数很多，那么子类的构造函数想要实现同样多的构造接口，必须一一调用基类的构造函数，有点麻烦。于是乎C++11引入继承构造函数
class _A {
 public:
  _A(int _InInt) { ; }
  _A(double _InDouble, int _InInt) { ; }
  _A(float _InFloat, int _InInt, const char* _Char) { ; }
};
class _B : public _A {
 public:
  using _A::_A;  // 使用_A中的构造函数
  virtual void _ExtraInterface() { ; }
};
// 一句using _A::_A; 把基类中的构造函数都继承到派生类_B中, 这样最开始的那段代码就可以摆脱基类那么多构造函数接口了。
// 这种继承构造函数，如果没有被使用，是不会生成目标代码的，会更节省目标代码空间。
// 但是：
// 1 C++11中的继承构造函数特性最有用的场合就是，派生类只是在基类的基础上添加了几个新的接口, 这个时候继承构造函数最能够展现威力.
// 但是缺陷就是, 继承构造函数只会初始化基类中的成员变量(毕竟只是从基类继承下来的), 对于派生类中的变量无能为力。
// 2 不过，可以配合另一个C++11中的新特性: 数据成员就地初始化，来解决这个问题。但是，这种做法导致子类的成员不能在构造函数的参数列表中体现出来。
// 3 一旦子类继承了基类的构造函数，那么子类就不会自动生成默认构造函数。所以，如果子类新增了自己的成员变量，乖乖回到以前的初始化列表方法中吧。
struct _Base {
  _Base(int) {}
};
struct _Derived : _Base {
  using _Base::_Base;
};

struct Base {
  int x;
  double y;
  string s;

  Base(int i) : x(i), y(0) {}
  Base(int i, double j) : x(i), y(j) {}
  Base(int i, double j, const string& str) : x(i), y(j), s(str) {}
};
// 直接派生，会报错
struct Derived1 : Base {};
//在派生类中定义构造函数
struct Derived2 : Base {
  Derived2(int i) : Base(i) {}
  Derived2(int i, double j) : Base(i, j) {}
  Derived2(int i, double j, const string& str) : Base(i, j, str) {}
};
struct Derived3 : Base {
  using Base::Base;  // 声明使用基类构造函数
};

// 托构造函数允许类中的一个构造函数通过初始化列表方式来调用同一个类中的另一个构造函数。
// 委托构造函数：允许在同一个类中一个构造函数可以调用另外一个构造函数，从而可以在初始化时简化变量的初始化。
class class_c1 {  // 普通写法
 public:
  int max;
  int min;
  int middle;
  class_c1(int my_max) { max = my_max > 0 ? my_max : 10; }
  class_c1(int my_max, int my_min) {
    max = my_max > 0 ? my_max : 10;
    min = my_min > 0 && my_min < max ? my_min : 1;
  }
  class_c1(int my_max, int my_min, int my_middle) {
    max = my_max > 0 ? my_max : 10;
    min = my_min > 0 && my_min < max ? my_min : 1;
    middle = my_middle < max && my_middle > min ? my_middle : 5;
  }
};
class class_c2 {  // 委托构造函数的写法
 public:
  int max;
  int min;
  int middle;
  class_c2(int my_max) { max = my_max > 0 ? my_max : 10; }
  class_c2(int my_max, int my_min) : class_c2(my_max) {
    min = my_min > 0 && my_min < max ? my_min : 1;
  }
  class_c2(int my_max, int my_min, int my_middle) : class_c2(my_max, my_min) {
    middle = my_middle < max && my_middle > min ? my_middle : 5;
  }
};
// 使用委托构造函数需要注意：使用了代理构造函数就不能用类成员初始化了。即不能同时委派构造函数和使用初始化列表。
class class_a {
 public:
  class_a() {}
  // member initialization here, no delegate
  class_a(string str) : m_string(str) {}

  // 调用了委托构造函数，不能用类成员初始化了
  // error C3511: a call to a delegating constructor shall be the only member-initializer
  // class_a(string str, double dbl) : class_a(str), m_double(dbl) {}

  // 只能通过成员赋值来初始化
  class_a(string str, double dbl) : class_a(str) { m_double = dbl; }
  double m_double{1.0};
  string m_string;
};
// 不要形成委托环
struct Rule2 {
  int i, c;
  Rule2() : Rule2(2) {}
  Rule2(int i) : Rule2('c') {}
  Rule2(char c) : Rule2(2) {}
};
// Rule2定义中，Rule2()、Rule2(int)和Rule2(char)都依赖于别的构造函数，形成环委托构造关系。这样的代码通常会导致编译错误。

int main() {
  // _Derived __derived;  // 编译失败, 因为没有提供_Derived的默认构造函数

  // Derived1 d(1, 2.5, "ok");  // 编译错误，没有合适的构造函数
  int i = 1;
  double j = 1.23;
  Derived2 d2(i);
  Derived2 d21(i, j);
  Derived2 d22(i, j, "");
  Derived3 d3(i);  // 直接使用基类构造函数来构造派生类对象
  Derived3 d31(i, j);
  Derived3 d32(i, j, "");
  return 0;
}