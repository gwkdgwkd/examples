#include <iostream>

using namespace std;

// 派生类都只有一个基类，称为单继承（Single Inheritance）。除此之外，C++也支持多继承（Multiple Inheritance），即一个派生类可以有两个或多个基类。
// 多继承容易让代码逻辑复杂、思路混乱，一直备受争议，中小型项目中较少使用，后来的Java、C#、PHP等干脆取消了多继承。
// 多继承的语法也很简单，将多个基类用逗号隔开即可。例如已声明了类A、类B和类C，那么可以这样来声明派生类D：
// class D : public A, private B, protected C {}
// 多继承形式下的构造函数和单继承形式基本相同，只是要在派生类的构造函数中调用多个基类的构造函数。
// D(形参列表) : A(实参列表), B(实参列表), C(实参列表) {}
// 基类构造函数的调用顺序和和它们在派生类构造函数中出现的顺序无关，而是和声明派生类时基类出现的顺序相同。
// D(形参列表) : B(实参列表), C(实参列表), A(实参列表) {}  // 先调用A类的构造函数，再调用B类构造函数，最后调用C类构造函数。
// 多继承形式下析构函数的执行顺序和构造函数的执行顺序相反。

// 当两个或多个基类中有同名的成员时，如果直接访问该成员，就会产生命名冲突，编译器不知道使用哪个基类的成员。
// 这个时候需要在成员名字前面加上类名和域解析符::，以显式地指明到底使用哪个类的成员，消除二义性。

// 基类对象成员内存地址的排列顺序和继承时声明的顺序相同。

class BaseA {
 public:
  BaseA(int a, int b);
  ~BaseA();
  void show();

 protected:
  int m_a;
  int m_b;
};
BaseA::BaseA(int a, int b) : m_a(a), m_b(b) {
  cout << "BaseA constructor" << endl;
}
BaseA::~BaseA() { cout << "BaseA destructor" << endl; }
void BaseA::show() {
  cout << "m_a = " << m_a << endl;
  cout << "m_b = " << m_b << endl;
}

class BaseB {
 public:
  BaseB(int c, int d);
  ~BaseB();
  void show();

 protected:
  int m_c;
  int m_d;
};
BaseB::BaseB(int c, int d) : m_c(c), m_d(d) {
  cout << "BaseB constructor" << endl;
}
BaseB::~BaseB() { cout << "BaseB destructor" << endl; }
void BaseB::show() {
  cout << "m_c = " << m_c << endl;
  cout << "m_d = " << m_d << endl;
}

class Derived : public BaseA, public BaseB {
 public:
  Derived(int a, int b, int c, int d, int e);
  ~Derived();

 public:
  void display();

 private:
  int m_e;
};
Derived::Derived(int a, int b, int c, int d, int e)
    : BaseB(c, d), BaseA(a, b), m_e(e) {
  cout << "Derived constructor" << endl;
}
Derived::~Derived() { cout << "Derived destructor" << endl; }
void Derived::display() {
  BaseA::show();  // 调用BaseA类的show()函数
  BaseB::show();  // 调用BaseB类的show()函数
  cout << "m_e = " << m_e << endl;
}

class A {
 public:
  A(int a, int b);

 protected:
  int m_a;
  int m_b;
};
A::A(int a, int b) : m_a(a), m_b(b) {}
class B {
 public:
  B(int b, int c);

 protected:
  int m_b;
  int m_c;
};
B::B(int b, int c) : m_b(b), m_c(c) {}
class C : public A, public B {
 public:
  C(int a, int b, int c, int d);

 public:
  void display();

 private:
  int m_a;
  int m_c;
  int m_d;
};
C::C(int a, int b, int c, int d) : A(a, b), B(b, c), m_a(a), m_c(c), m_d(d) {}
void C::display() {
  printf("A::m_a=%d, A::m_b=%d\n", A::m_a, A::m_b);
  printf("B::m_b=%d, B::m_c=%d\n", B::m_b, B::m_c);
  printf("C::m_a=%d, C::m_c=%d, C::m_d=%d\n", C::m_a, C::m_c, m_d);
  cout << hex << showbase << "A::m_a : " << &(A::m_a) << endl;
  cout << hex << showbase << "A::m_b : " << &(A::m_b) << endl;
  cout << hex << showbase << "B::m_b : " << &(B::m_b) << endl;
  cout << hex << showbase << "B::m_c : " << &(B::m_c) << endl;
  cout << hex << showbase << "C::m_a : " << &(C::m_a) << endl;
  cout << hex << showbase << "C::m_c : " << &(C::m_c) << endl;
  cout << hex << showbase << "C::m_d : " << &(C::m_d) << endl;
}

int main() {
  Derived obj(1, 2, 3, 4, 5);
  obj.display();
  // BaseA constructor
  // BaseB constructor
  // Derived constructor
  // m_a = 1
  // m_b = 2
  // m_c = 3
  // m_d = 4
  // m_e = 5
  // Derived destructor
  // BaseB destructor
  // BaseA destructor

  C obj_c(10, 20, 30, 40);
  obj_c.display();
  // A::m_a=10, A::m_b=20
  // B::m_b=20, B::m_c=30
  // C::m_a=10, C::m_c=30, C::m_d=40
  // A::m_a : 0x7ffde3f4f120
  // A::m_b : 0x7ffde3f4f124
  // B::m_b : 0x7ffde3f4f128
  // B::m_c : 0x7ffde3f4f12c
  // C::m_a : 0x7ffde3f4f130
  // C::m_c : 0x7ffde3f4f134
  // C::m_d : 0x7ffde3f4f138

  return 0;
}
