#include <iostream>

// 对象的内存中包含了成员变量，不同的对象占用不同的内存，
// 这使得不同对象的成员变量相互独立，它们的值不受其他对象的影响。
// 可是有时候希望在多个对象之间共享数据，典型使用场景是计数。
// 在C++中，可以使用静态成员变量来实现多个对象共享数据的目标。
// 静态成员变量是一种特殊的成员变量，它被关键字static修饰。
// static成员变量属于类，不属于某个具体的对象，即使创建多个对象，
// 也只分配一份内存，所有对象使用的都是这份内存中的数据。

namespace n1 {
class Demo {
 public:
  Demo(int d) : d_(d) {}

  // 一个类中可以有一个或多个静态成员变量，
  // 所有的对象都共享这些静态成员变量，都可以引用它：
  static int a;
  static int b;
  static int c;

  int d_;

 private:
  static int d;
};

// static成员变量的内存既不是在声明类时分配，
// 也不是在创建对象时分配，而是在（类外）初始化时分配。
// 静态成员变量必须初始化，而且只能在类体外进行。
// 被private、protected、public修饰的静态成员变量都可以用这种方式初始化。
// 静态成员初始化时可以赋初值，也可以不赋值。
// 全局数据区的变量都有默认的初始值0，
// 而动态数据区（堆区、栈区）变量的默认值是不确定的，一般认为是垃圾值。
// 静态成员变量在初始化时不能再加static，但必须要有数据类型：
int Demo::a = 5;
int Demo::b;
int Demo::d = 6;

int g;

void func1() {
  // static成员变量不占用对象的内存，
  // 而是在所有对象之外开辟内存，即使不创建对象也可以访问：
  std::cout << Demo::a << " " << Demo::b << std::endl;  // 5 0

  // static成员变量和普通static变量一样，
  // 都在内存分区中的全局数据区分配内存，到程序结束时才释放。
  // 这就意味着，static成员变量不随对象的创建而分配内存，
  // 也不随对象的销毁而释放内存。
  // 而普通成员变量在对象创建时分配内存，在对象销毁时释放内存。
  std::cout << &g << std::endl;        // 0x55dd66b20158
  std::cout << &Demo::a << std::endl;  // 0x55e8dac7e010
  std::cout << &Demo::b << std::endl;  // 0x55e8dac7e154

  Demo d(1);
  // 静态成员变量既可以通过对象名访问，也可以通过类名访问：
  std::cout << Demo::a << " " << d.a << std::endl;  // 5 5
  std::cout << Demo::b << " " << d.b << std::endl;  // 0 0

  // 没有在类外初始化的static成员变量不能使用：
  // std::cout << Demo::c << std::endl;
  // undefined reference to `n1::Demo::c'

  // 要遵循private、protected和public的访问权限限制：
  // std::cout << Demo::d << std::endl;
  // d is private within this context
}

void func2() {
  Demo d1(10);
  Demo d2(20);
  // 当通过对象名访问时，对于不同的对象，访问的是同一份内存：
  std::cout << "Demo(" << d1.d_ << ").a = " << d1.a << std::endl;  // 5
  std::cout << "Demo(" << d1.d_ << ").a = " << d1.b << std::endl;  // 0
  std::cout << "Demo(" << d2.d_ << ").a = " << d2.a << std::endl;  // 5
  std::cout << "Demo(" << d2.d_ << ").a = " << d2.b << std::endl;  // 0

  Demo::a = 6;
  Demo::b = 9;
  std::cout << "Demo(" << d1.d_ << ").a = " << d1.a << std::endl;  // 6
  std::cout << "Demo(" << d1.d_ << ").a = " << d1.b << std::endl;  // 9
  std::cout << "Demo(" << d2.d_ << ").a = " << d2.a << std::endl;  // 6
  std::cout << "Demo(" << d2.d_ << ").a = " << d2.b << std::endl;  // 9
}

void testN1() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 静态成员函数与普通成员函数的根本区别在于：
// 1.普通成员函数有this指针，可以访问类中的任意成员；
// 2.而静态成员函数没有this指针，只能访问静态成员，
//   包括静态成员变量和静态成员函数。

class A {
 public:
  A() { m = 6; }

  // 在类中，static除了可以声明静态成员变量，还可以声明静态成员函数：
  static void f1() {
    std::cout << "static f1" << std::endl;
    // 静态成员函数只能访问静态成员
    f3();
    std::cout << s << std::endl;

    // f4();
    // std::cout << m << std::endl;
  }
  void f2() {
    std::cout << "member f2" << std::endl;
    // 普通成员函数可以访问所有成员，包括成员变量和成员函数
    f3();
    f4();
    std::cout << s << std::endl;
    std::cout << m << std::endl;
  }
  static int s;
  int m;

 private:
  static void f3();  // 声明时要加static
  void f4() { std::cout << "member f4" << std::endl; }
};
int A::s = 9;
// 和静态成员变量类似，静态成员函数在声明时要加static，在定义时不能加static：
void A::f3() { std::cout << "static f3" << std::endl; }

void testN2() {
  // 静态成员函数可以通过类来直接调用，编译器不会为它增加形参this，
  // 它不需要当前对象的地址，所以不管有没有创建对象，都可以调用静态成员函数。
  // 普通成员变量占用对象的内存，静态成员函数没有this指针，
  // 不知道指向哪个对象，无法访问对象的成员变量，
  // 也就是说静态成员函数不能访问普通成员变量，只能访问静态成员变量。
  A::f1();
  // static f1
  // static f3
  // 9

  // 编译器在编译一个普通成员函数时，会隐式地增加一个形参this，
  // 并把当前对象的地址赋值给this，
  // 所以普通成员函数只能在创建对象后通过对象来调用，因为它需要当前对象的地址。
  A a;
  a.f2();
  // member f2
  // static f3
  // member f4
  // 9
  // 6

  // 静态成员函数可以通过类来调用（一般都是这样做），也可以通过对象来调用：
  a.f1();
  // static f1
  // static f3
  // 9
}
}  // namespace n2

int main(int argc, char *argv[]) {
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