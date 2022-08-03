#include <iostream>

namespace n1 {
// 类其实也是一种作用域，每个类都会定义它自己的作用域，
// 在这个作用域内再定义类的成员。
// 当存在继承关系时，派生类的作用域嵌套在基类的作用域之内，
// 如果一个名字在派生类的作用域内无法找到，
// 编译器会继续到外层的基类作用域中查找该名字的定义。
// 换句话说，作用域能够彼此包含，
// 被包含（或者说被嵌套）的作用域称为内层作用域（inner scope），
// 包含着别的作用域的作用域称为外层作用域（outer scope）。
// 一旦在外层作用域中声明（或者定义）了某个名字，
// 那么它所嵌套着的所有内层作用域中都能访问这个名字。
// 同时，允许在内层作用域中重新定义外层作用域中已有的名字。
// 恰恰因为类作用域有这种继承嵌套的关系，
// 所以派生类才能像使用自己的成员一样来使用基类的成员。
// 对于成员变量这个过程很好理解，对于成员函数要引起注意，
// 编译器仅仅是根据函数的名字来查找的，不会理会函数的参数。
// 换句话说，一旦内层作用域有同名的函数，
// 不管有几个，编译器都不会再到外层作用域中查找，
// 编译器仅把内层作用域中的这些同名函数作为一组候选函数，
// 这组候选函数就是一组重载函数。
// 只有一个作用域内的同名函数才有重载关系，
// 不同作用域内的同名函数会造成遮蔽，使得外层函数无效。
// 派生类和基类有不同的作用域，所以它们的同名函数不具有重载关系。

class A {
 public:
  void func() { std::cout << "A::func" << std::endl; }

 public:
  int n = 1;
};
class B : public A {
 public:
  int n = 2;
  int m;
};
class C : public B {
 public:
  int n = 3;
  int x;
};

void testN1() {
  // 通过obj访问成员函数func()时，在C类的作用域中没有找到func这个名字，
  // 编译器继续到B类的作用域（外层作用域）中查找，
  // 仍然没有找到，再继续到A类的作用域中查找，结果就发现了func这个名字，
  // 于是查找结束，编译器决定调用A类作用域中的func()函数。
  // 这个过程叫做名字查找，也就是在作用域链中寻找名字最匹配的声明或定义的过程。
  C obj;
  obj.func();                           // A::func
  std::cout << obj.n << std::endl;      // 3
  std::cout << sizeof(C) << std::endl;  // 20
}
}  // namespace n1

namespace n2 {
// 没有继承时对象内存的分布情况，内存模型很简单，成员变量和成员函数会分开存储：
// 1.对象的内存中只包含成员变量，存储在栈区或堆区（使用new创建对象）；
// 2.成员函数与对象内存分离，存储在代码区。
// 有继承关系时，派生类的内存模型可以看成是基类成员变量和新增成员变量的总和，
// 而所有成员函数仍然存储在代码区，由所有对象共享。
// 基类的成员变量排在前面，派生类的排在后面。
// 成员变量按照派生的层级依次排列，新增成员变量始终在最后。
// 在派生类的对象模型中，会包含所有基类的成员变量。
// 这种设计方案的优点是访问效率高，能够在派生类对象中直接访问基类变量，
// 无需经过好几层间接计算。
class A {
 public:
  A(int a, int b) : m_a(a), m_b(b) {}

 public:
  void display() { std::cout << "m_a=" << m_a << ",m_b=" << m_b << std::endl; }

 protected:
  int m_a;
  int m_b;
};
class B : public A {
 public:
  B(int a, int b, int c) : A(a, b), m_c(c) {}
  void display() {
    std::cout << "m_a=" << m_a << ",m_b=" << m_b << ",m_c=" << m_c << std::endl;
  }

 protected:
  int m_c;
};
class C1 : public B {
 public:
  C1(char a, int b, int c, int d) : B(a, b, c), m_d(d) {}

 public:
  void display() {
    std::cout << "m_a=" << m_a << ",m_b=" << m_b << ",m_c=" << m_c
              << ",m_d=" << m_d << std::endl;
    std::cout << "m_a=0x" << &m_a << ",m_b=0x" << &m_b << ",m_c=0x" << &m_c
              << ",m_d=0x" << &m_d << std::endl;
  }

 private:
  int m_d;
};

class C2 : public B {
 public:
  C2(char a, int b, int c, int d) : B(a, b, c), m_b(b), m_c(c), m_d(d) {}

 public:
  void display() {
    std::cout << "A::m_a=" << m_a << ",A::m_b=" << A::m_b
              << ",B::m_c=" << B::m_c << std::endl;
    std::cout << "C::m_a=" << m_a << ",C::m_b=" << m_b << ",C::m_c=" << m_c
              << std::endl;
    std::cout << "m_a=0x" << &m_a << ",A::m_b=0x" << &(A::m_b) << ",B::m_c=0x"
              << &(B::m_c) << std::endl;
    std::cout << "m_b=0x" << &m_b << ",m_c=0x" << &m_c << ",m_d=0x" << &m_d
              << std::endl;
  }

 private:
  // 当基类A、B的成员变量被遮蔽时，仍然会留在派生类对象的内存中，
  // C类新增的成员变量始终排在基类A、B的后面。
  int m_b;  // 遮蔽A类的成员变量
  int m_c;  // 遮蔽B类的成员变量
  int m_d;  // 新增成员变量
};

void testN2() {
  C1 c1(84, 23, 95, 60);
  c1.display();
  // m_a=84,m_b=23,m_c=95,m_d=60
  // m_a=0x0x7fff0fe00e80,m_b=0x0x7fff0fe00e84,m_c=0x0x7fff0fe00e88,m_d=0x0x7fff0fe00e8c

  C2 c2(84, 23, 95, 60);
  c2.display();
  // A::m_a=84,A::m_b=23,B::m_c=95
  // C::m_a=84,C::m_b=23,C::m_c=95
  // m_a=0x0x7ffd24a10700,A::m_b=0x0x7ffd24a10704,B::m_c=0x0x7ffd24a10708
  // m_b=0x0x7ffd24a1070c,m_c=0x0x7ffd24a10710,m_d=0x0x7ffd24a10714
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