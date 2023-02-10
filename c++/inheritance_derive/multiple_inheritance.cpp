#include <iostream>

// 派生类都只有一个基类，称为单继承（Single Inheritance）。
// 除此之外，C++也支持多继承（Multiple Inheritance），
// 即一个派生类可以有两个或多个基类。
// 多继承容易让代码逻辑复杂、思路混乱，一直备受争议，
// 中小型项目中较少使用，后来的Java、C#、PHP等干脆取消了多继承。
// 多继承的语法也很简单，将多个基类用逗号隔开即可。

namespace n1 {
// 多继承形式下的构造函数和单继承形式基本相同，
// 只是要在派生类的构造函数中调用多个基类的构造函数。
// 基类构造函数的调用顺序和和它们在派生类构造函数中出现的顺序无关，
// 而是和声明派生类时基类出现的顺序相同。
// 多继承形式下析构函数的执行顺序和构造函数的执行顺序相反。
class A {
 public:
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }
};

class B {
 public:
  B() { std::cout << "B" << std::endl; }
  ~B() { std::cout << "~B" << std::endl; }
};

namespace test1 {
class C : public A, public B {
 public:
  C() : B(), A() { std::cout << "C" << std::endl; }
  C(int) : A(), B() { std::cout << "C" << std::endl; }
  ~C() { std::cout << "~C" << std::endl; }
};
void func1() {
  C c;
  // A
  // B
  // C
  // ~C
  // ~B
  // ~A
}
void func2() {
  C c(5);
  // A
  // B
  // C
  // ~C
  // ~B
  // ~A
}
void func() {
  func1();
  func2();
}
}  // namespace test1

namespace test2 {
class C : public B, public A {
 public:
  C() : B(), A() { std::cout << "C" << std::endl; }
  C(int) : A(), B() { std::cout << "C" << std::endl; }
  ~C() { std::cout << "~C" << std::endl; }
};

void func1() {
  C c;
  // B
  // A
  // C
  // ~C
  // ~A
  // ~B
}
void func2() {
  C c(5);
  // B
  // A
  // C
  // ~C
  // ~A
  // ~B
}
void func() {
  func1();
  func2();
}
}  // namespace test2
}  // namespace n1

namespace n2 {
// 当两个或多个基类中有同名的成员时，如果直接访问该成员，
// 就会产生命名冲突，编译器不知道使用哪个基类的成员。
// 这个时候需要在成员名字前面加上类名和域解析符::，
// 以显式地指明到底使用哪个类的成员，消除二义性。
class A {
 public:
  int a = 1;
  int b = 3;
};

class B {
 public:
  int a = 2;
  int c = 4;
};

class C : public A, public B {
 public:
  int b = 5;
  void show() {
    std::cout << c << std::endl;
    std::cout << b << std::endl;
    std::cout << A::b << std::endl;
  }
};

void func() {
  C c;
  // std::cout << c.a << std::endl;
  std::cout << c.A::a << std::endl;  // 1
  std::cout << c.B::a << std::endl;  // 2

  c.show();
  // 4
  // 5
  // 3
}
}  // namespace n2

namespace n3 {
// 基类对象成员内存地址的排列顺序和继承时声明的顺序相同。
class A {
 protected:
  int a = 1;
  int b = 2;
};
class B {
 protected:
  int b = 3;
  int c = 4;
};
namespace test1 {
class C : public A, public B {
 public:
  void display() {
    std::cout << "A::a=" << A::a << ", A::b=" << A::b << std::endl;
    std::cout << "B::b=" << B::b << ", B::c=" << B::c << std::endl;
    std::cout << "C::a=" << C::a << ", C::c=" << C::c << ", C::d=" << d
              << std::endl;
    std::cout.setf(std::ios::hex);
    std::cout.setf(std::ios_base::showbase);
    std::cout << "A::a : " << &(A::a) << std::endl;
    std::cout << "A::b : " << &(A::b) << std::endl;
    std::cout << "B::b : " << &(B::b) << std::endl;
    std::cout << "B::c : " << &(B::c) << std::endl;
    std::cout << "C::a : " << &(C::a) << std::endl;
    std::cout << "C::c : " << &(C::c) << std::endl;
    std::cout << "C::d : " << &(C::d) << std::endl;
  }

 private:
  int a = 5;
  int c = 6;
  int d = 7;
};

void func() {
  C c;
  c.display();

  // A::a=1, A::b=2
  // B::b=3, B::c=4
  // C::a=5, C::c=6, C::d=7
  // A::a : 0x7ffcd9605d20
  // A::b : 0x7ffcd9605d24
  // B::b : 0x7ffcd9605d28
  // B::c : 0x7ffcd9605d2c
  // C::a : 0x7ffcd9605d30
  // C::c : 0x7ffcd9605d34
  // C::d : 0x7ffcd9605d38
}
}  // namespace test1

namespace test2 {
class C : public B, public A {
 public:
  void display() {
    std::cout.setf(std::ios::dec);
    std::cout << "A::a=" << A::a << ", A::b=" << A::b << std::endl;
    std::cout << "B::b=" << B::b << ", B::c=" << B::c << std::endl;
    std::cout << "C::a=" << C::a << ", C::c=" << C::c << ", C::d=" << d
              << std::endl;
    std::cout.setf(std::ios::hex);
    std::cout.setf(std::ios_base::showbase);
    std::cout << "B::b : " << &(B::b) << std::endl;
    std::cout << "B::c : " << &(B::c) << std::endl;
    std::cout << "A::a : " << &(A::a) << std::endl;
    std::cout << "A::b : " << &(A::b) << std::endl;
    std::cout << "C::a : " << &(C::a) << std::endl;
    std::cout << "C::c : " << &(C::c) << std::endl;
    std::cout << "C::d : " << &(C::d) << std::endl;
  }

 private:
  int a = 5;
  int c = 6;
  int d = 7;
};

void func() {
  C c;
  c.display();

  // A::a=1, A::b=2
  // B::b=3, B::c=4
  // C::a=5, C::c=6, C::d=7
  // B::b : 0x7ffd7f1f87d0
  // B::c : 0x7ffd7f1f87d4
  // A::a : 0x7ffd7f1f87d8
  // A::b : 0x7ffd7f1f87dc
  // C::a : 0x7ffd7f1f87e0
  // C::c : 0x7ffd7f1f87e4
  // C::d : 0x7ffd7f1f87e8
}
}  // namespace test2
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n2::func();
      break;
    case 3:
      n3::test1::func();
      break;
    case 4:
      n3::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}