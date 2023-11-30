#include <iostream>

// 为多态基类声明virtual析构函数

// C++明确指出，当派生类对象经由一个基类指针被删除，
// 而该基类带着一个non-virtual析构函数，其结果未定义。
// 实际执行时通常发生的是对象的派生成分没被销毁，
// 基类通常会被销毁，造成诡异的局部销毁对象。
// 这可是造成资源泄露、败坏的数据结构、在调试上浪费许多时间的绝佳途径。
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

namespace n1 {
class Base {
 public:
  Base() { std::cout << "Base" << std::endl; }
  ~Base() { std::cout << "~Base" << std::endl; }

 private:
  A a;
};  // namespace Base
class Derived : public Base {
 public:
  Derived() { std::cout << "Derived" << std::endl; }
  ~Derived() { std::cout << "~Derived" << std::endl; }

 private:
  B a;
};
void func() {
  Base* ptr = new (Derived);
  delete ptr;  // 基类被销毁，派生类没有被销毁

  // A
  // Base
  // B
  // Derived
  // ~Base
  // ~A
}
}  // namespace n1

namespace n2 {
// 解决办法很简单，给base class一个virtual析构函数，就会销毁整个对象。
// 任何class只要带有virtual函数，都几乎应该也有一个virtual析构函数。
class Base {
 public:
  Base() { std::cout << "Base" << std::endl; }
  virtual ~Base() { std::cout << "~Base" << std::endl; }

 private:
  A a;
};
class Derived : public Base {
 public:
  Derived() { std::cout << "Derived" << std::endl; }
  ~Derived() { std::cout << "~Derived" << std::endl; }

 private:
  B a;
};
void func() {
  Base* ptr = new (Derived);
  delete ptr;  // 基类和派生类都被销毁

  // A
  // Base
  // B
  // Derived
  // ~Derived
  // ~B
  // ~Base
  // ~A
}
}  // namespace n2

namespace n3 {
// 如果class不含virtual函数，通常表示它并不被用作base class，
// 此时令析构函数为virtual是个馊主意。
// 含有virtual函数，对象体积会增加，多出了一个指针的大小（vptr指针）。
class A1 {
  int x, y;
};
class A2 {
 public:
  virtual ~A2() {}

 private:
  int x, y;
};

// string不含任何virtual函数，但有时候会错误地把它当做base class。
// 所有STL容器也都不含virtual析构函数。
// 所以不要企图继承一个标准容器，或任何其他带有non-virtual析构函数的class。
// 很不幸C++没有Java的final class或C#的sealed class那样的禁止派生机制。

// 纯虚函数
class B {
 public:
  virtual ~B() = 0;
};
// 析构函数调用顺序是，最深层class的析构函数先被调用：
B::~B() {}
// 在D的析构函数中会调用~B，如果上面~B没有定义，发生链接错误：
class D : public B {};

void func() {
  std::cout << sizeof(A1) << std::endl;  // 8
  std::cout << sizeof(A2) << std::endl;  // 16

  D d;
}
}  // namespace n3

// 请记住：
// 1.带多态性质的base class应该声明一个virtual析构函数；
// 2.如果class带有任何virtual函数，就应该拥有一个virtual析构函数；
// 3.class的设计目的如果不是作为base class使用，
//   或不是为了具备多态，就不该声明virtual析构函数。

int main(int argc, char* argv[]) {
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
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}