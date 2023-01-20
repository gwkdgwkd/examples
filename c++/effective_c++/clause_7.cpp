#include <iostream>

using namespace std;

// 为多态基类声明virtual析构函数

// C++明确指出，当派生类对象经由一个基类指针被删除，
// 而该基类带着一个non-virtual析构函数，其结果未定义。
// 实际执行时通常发生的是对象的派生成分没被销毁，
// 基类通常会被销毁，造成诡异的局部销毁对象。
// 这可是造成资源泄露、败坏的数据结构、在调试上浪费许多时间的绝佳途径。
class A {
 public:
  A() { cout << "A" << endl; }
  ~A() { cout << "~A" << endl; }
};
class B {
 public:
  B() { cout << "B" << endl; }
  ~B() { cout << "~B" << endl; }
};
class Base1 {
 public:
  Base1() { cout << "Base1" << endl; }
  ~Base1() { cout << "~Base1" << endl; }

 private:
  A a;
};
class Derived1 : public Base1 {
 public:
  Derived1() { cout << "Derived1" << endl; }
  ~Derived1() { cout << "~Derived1" << endl; }

 private:
  B a;
};

// 解决办法很简单：给base class一个virtual析构函数，就会销毁整个对象。
// 任何class只要带有virtual函数，都几乎应该也有一个virtual析构函数。
class Base2 {
 public:
  Base2() { cout << "Base2" << endl; }
  virtual ~Base2() { cout << "~Base2" << endl; }

 private:
  A a;
};
class Derived2 : public Base2 {
 public:
  Derived2() { cout << "Derived2" << endl; }
  ~Derived2() { cout << "~Derived2" << endl; }

 private:
  B a;
};

// 如果class不含virtual函数，通常表示它并不被用作base class，
// 此时令析构函数为virtual是个馊主意。
// 含有virtual函数，对象体积会增加，多出了一个指针的大小（vptr指针）。
class Point1 {
  int x, y;
};
class Point2 {
 public:
  virtual ~Point2() {}

 private:
  int x, y;
};

// string不含任何virtual函数，但有时候会错误地把它当做base class。
// 所有STL容器也都不含virtual析构函数。
// 所以不要企图继承一个标准容器，
// 或任何其他带有non-virtual析构函数的class。
// 很不幸C++没有提供类似Java的final classes，
// 或C#的sealed classes那样的禁止派生机制。

// 纯虚函数
class AWOV {
 public:
  virtual ~AWOV() = 0;
};
// 析构函数调用顺序是，最深层class的析构函数先被调用。
AWOV::~AWOV() {}
// 在DD的析构函数中会调用~AWOV，如果上面~AWOV没有定义，发生链接错误。
class DD : public AWOV {};

// 请记住：
// 1.带多态性质的base class应该声明一个virtual析构函数。
// 2.如果class带有任何virtual函数，就应该拥有一个virtual析构函数。
// 3.class的设计目的如果不是作为base class使用，
//   或不是为了具备多态，就不该声明virtual析构函数。

int main() {
  Base1 *ptr1 = new (Derived1);
  delete ptr1;  // 基类被销毁，派生类没有被销毁
  // A
  // Base1
  // B
  // Derived1
  // ~Base1
  // ~A

  Base2 *ptr2 = new (Derived2);
  delete ptr2;  // 基类和派生类都被销毁
  // A
  // Base2
  // B
  // Derived2
  // ~Derived2
  // ~B
  // ~Base2
  // ~A

  cout << sizeof(Point1) << endl;  // 8
  cout << sizeof(Point2) << endl;  // 16

  DD dd;

  return 0;
}