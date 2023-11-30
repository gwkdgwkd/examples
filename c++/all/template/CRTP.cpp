#include <iostream>
#include <memory>

// CRTP的全称是Curiously Recurring Template Pattern，即奇异递归模板模式。
// CRTP是一种特殊的模板技术和使用方式，是C++模板编程中的一种惯用法。
// CRTP的特性表现为：
// 1.基类是一个模板类；
// 2.派生类继承该基类时，将派生类自身作为模板参数传递给基类。

// CRTP的优点：
// 1.CRTP完全消除了动态绑定，省去动态绑定、查询虚函数表带来的开销；
// 2.通过CRTP，基类可以获得到派生类的类型，提供各种操作，比普通的继承更加灵活，
//   但CRTP基类并不会单独使用，只是作为一个模板的功能。
// CRTP的缺点：
// 1.模板的通病，即影响代码的可读性。

namespace n1 {
// 基类是模板类：
template <typename T>
class Base {
 public:
  virtual ~Base() {}
  void func() {
    // 使用static_cast，将this指针转换为模板参数T的指针，然后调用T的方法：
    if (auto t = static_cast<T*>(this)) {
      t->op();
    }
    // static_cast转换安全吗？
    // 当static_cast用于类层次结构中基类和派生类之间指针或引用的转换，
    // 在进行上行转换（把派生类的指针或引用转换成基类）是安全的；
    // 而下行转换（把基类指针或引用转换为派生类）由于没有动态类型检查，不一定安全。
    // 而在实际使用时，只会使用派生类不会直接使用基类型定义对象。
    // 所以当static_cast被执行时，基类的指针一定指向子类的对象，因此是安全的。
  }
};

// 派生类Derived继承自Base，并以自身作为模板参数传递给基类：
class Derived1 : public Base<Derived1> {
 public:
  void op() { std::cout << "Derived1::op()" << std::endl; }
};

class Derived2 : public Base<Derived2> {
 public:
  void op() { std::cout << "Derived2::op()" << std::endl; }
};

void func1() {
  Derived1 d1;
  d1.op();  // Derived1::op()

  Derived2 d2;
  d2.op();  // Derived2::op()
}

// 静态多态
// 多态是指同一个方法在基类和不同派生类之间有不同的行为。
// 但CRTP中每个派生类继承的基类随着模板参数的不同而不同，
// 也就是说，Base<Derived1>和Base<Derived2>并不是同一个基类类型。
// 因此，这里的静态多态并不是一种严格意义上的多态。
template <typename DerivedClass>
void helperFunc(Base<DerivedClass>& d) {  // 辅助函数
  // 模板类或模板函数在调用时才会实例化。
  // 因此当基类的Base<Derived1>::func()被调用时，
  // Base<Derived1>已经知道Derived1::op()的存在：
  d.func();
}
void func2() {
  Derived1 d1;
  helperFunc(d1);  // Derived1::op()
  Derived2 d2;
  helperFunc(d2);  // Derived2::op()
}
}  // namespace n1

namespace n2 {
// 实现不同子类的计数器
template <typename T>
class Counter {
 public:
  static int count;
  Counter() { ++Counter<T>::count; }
  ~Counter() { --Counter<T>::count; }
};
template <typename T>
int Counter<T>::count = 0;
class Counter1 : public Counter<Counter1> {
 public:
  int getCount() { return this->count; }
};
class Counter2 : public Counter<Counter2> {
 public:
  int getCount() { return this->count; }
};
void func() {
  Counter1 c1_1;
  std::cout << "Count1 : " << c1_1.getCount() << std::endl;  // 1
  {
    Counter1 c1_2;
    std::cout << "Count1 : " << c1_1.getCount() << std::endl;  // 2
  }
  std::cout << "Count1 : " << c1_1.getCount() << std::endl;  // 1

  Counter2 c2_1, c2_2, c2_3, c2_4, c2_5[3];
  Counter2* pc = new Counter2;
  std::cout << "Count2 : " << c2_1.getCount() << std::endl;     // 8
  std::cout << "Count2 : " << c2_5[2].getCount() << std::endl;  // 8
  delete pc;
  std::cout << "Count2 : " << c2_1.getCount() << std::endl;  // 7
}
}  // namespace n2

namespace n3 {
// 多态链
namespace test1 {
class Printer {
 public:
  Printer(std::ostream& pstream) : m_stream(pstream) {}
  template <typename T>
  Printer& print(T&& t) {
    m_stream << t;
    return *this;
  }

  template <typename T>
  Printer& println(T&& t) {
    m_stream << t << std::endl;
    return *this;
  }

 private:
  std::ostream& m_stream;
};

class CoutPrinter : public Printer {
 public:
  CoutPrinter() : Printer(std::cout) {}
  CoutPrinter& SetConsoleColor(int c) { return *this; }
};
void func() {
  // CoutPrinter().print("Hello ").SetConsoleColor(5).println("Printer!");
  //   前半段CoutPrinter().print("Hello ")调用的是Printer实例，
  //   后面接着SetConsoleColor(5)又需要调用CoutPrinter实例，这样编译器就会报错。
}
}  // namespace test1

namespace test2 {
template <typename ConcretePrinter>
class Printer {
 public:
  Printer(std::ostream& pstream) : m_stream(pstream) {}

  template <typename T>
  ConcretePrinter& print(T&& t) {
    m_stream << "\033["
             << static_cast<ConcretePrinter&>(*this).GetConsoleColor() << "m"
             << t << "\033[0m";
    return static_cast<ConcretePrinter&>(*this);
  }

  template <typename T>
  ConcretePrinter& println(T&& t) {
    m_stream << "\033["
             << static_cast<ConcretePrinter&>(*this).GetConsoleColor() << "m"
             << t << "\033[0m" << std::endl;
    return static_cast<ConcretePrinter&>(*this);
  }

 private:
  std::ostream& m_stream;
};
class CoutPrinter : public Printer<CoutPrinter> {
 public:
  CoutPrinter() : Printer(std::cout) {}
  CoutPrinter& SetConsoleColor(int c) {
    color = c;
    return *this;
  }
  int GetConsoleColor() { return color; }

 private:
  int color;
};
void func() {
  CoutPrinter().print("Hello ").SetConsoleColor(32).println("Printer!");
  // Hello Printer!

  CoutPrinter()
      .SetConsoleColor(31)
      .print("A ")
      .SetConsoleColor(32)
      .print("B ")
      .SetConsoleColor(33)
      .print("C ")
      .SetConsoleColor(0)
      .println("D");
  // A B C D
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n3

namespace n4 {
// 多态的拷贝构造
// 当用到多态时，经常会需要通过基类的指针来复制子对象。
// 通常可以通过在基类里面构造一个clone()虚函数，
// 然后在每个子类里面定义这个clone()函数。
// 使用CRTP可以避免反复地在子类中去定义clone()函数。
// 看到virtual，就说明这里还是需要运行时的多态，
// 因为实例化后的子类类型无法在编译期进行确定，CRTP实现不了动态多态
class AbstractShape {
 public:
  virtual ~AbstractShape() = default;
  virtual std::unique_ptr<AbstractShape> clone() const = 0;
};

// This CRTP class implements clone() for Derived
template <typename Derived>
class Shape : public AbstractShape {
 public:
  std::unique_ptr<AbstractShape> clone() const override {
    return std::make_unique<Derived>(static_cast<Derived const&>(*this));
  }

 protected:
  Shape() = default;
  Shape(const Shape&) = default;
};
class Square : public Shape<Square> {};
class Circle : public Shape<Circle> {};

void func() {
  // 怎么使用？
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n2::func();
      break;
    case 3:
      n3::func();
      break;
    case 4:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}