#include <iostream>

using namespace std;

// 类其实也是一种作用域，每个类都会定义它自己的作用域，在这个作用域内我们再定义类的成员。
// 当存在继承关系时，派生类的作用域嵌套在基类的作用域之内，如果一个名字在派生类的作用域内无法找到，编译器会继续到外层的基类作用域中查找该名字的定义。
// 换句话说，作用域能够彼此包含，被包含（或者说被嵌套）的作用域称为内层作用域（inner scope），包含着别的作用域的作用域称为外层作用域（outer scope）。
// 一旦在外层作用域中声明（或者定义）了某个名字，那么它所嵌套着的所有内层作用域中都能访问这个名字。同时，允许在内层作用域中重新定义外层作用域中已有的名字。
// 恰恰因为类作用域有这种继承嵌套的关系，所以派生类才能像使用自己的成员一样来使用基类的成员。
// 对于成员变量这个过程很好理解，对于成员函数要引起注意，编译器仅仅是根据函数的名字来查找的，不会理会函数的参数。换句话说，一旦内层作用域有同名的函数，不管
// 有几个，编译器都不会再到外层作用域中查找，编译器仅把内层作用域中的这些同名函数作为一组候选函数；这组候选函数就是一组重载函数。
// 只有一个作用域内的同名函数才有重载关系，不同作用域内的同名函数会造成遮蔽，使得外层函数无效。派生类和基类有不同的作用域，所以它们的同名函数不具有重载关系。
class A {
 public:
  void func();

 public:
  int n;
};
void A::func() { cout << "c.biancheng.net" << endl; }
class B : public A {
 public:
  int n;
  int m;
};
class C : public B {
 public:
  int n;
  int x;
};

// 没有继承时对象内存的分布情况。内存模型很简单，成员变量和成员函数会分开存储：
//  对象的内存中只包含成员变量，存储在栈区或堆区（使用new创建对象）；
//  成员函数与对象内存分离，存储在代码区。
// 有继承关系时，派生类的内存模型可以看成是基类成员变量和新增成员变量的总和，而所有成员函数仍然存储在另外一个区域——代码区，由所有对象共享。'
// 基类的成员变量排在前面，派生类的排在后面。成员变量按照派生的层级依次排列，新增成员变量始终在最后。
// 在派生类的对象模型中，会包含所有基类的成员变量。这种设计方案的优点是访问效率高，能够在派生类对象中直接访问基类变量，无需经过好几层间接计算。
class A1 {
 public:
  A1(int a, int b);

 public:
  void display();

 protected:
  int m_a;
  int m_b;
};
A1::A1(int a, int b) : m_a(a), m_b(b) {}
void A1::display() { printf("m_a=%d, m_b=%d\n", m_a, m_b); }
class B1 : public A1 {
 public:
  B1(int a, int b, int c);
  void display();

 protected:
  int m_c;
};
B1::B1(int a, int b, int c) : A1(a, b), m_c(c) {}
void B1::display() { printf("m_a=%d, m_b=%d, m_c=%d\n", m_a, m_b, m_c); }
class C1 : public B1 {
 public:
  C1(char a, int b, int c, int d);

 public:
  void display();

 private:
  int m_d;
};
C1::C1(char a, int b, int c, int d) : B1(a, b, c), m_d(d) {}
void C1::display() {
  printf("m_a=%d, m_b=%d, m_c=%d, m_d=%d\n", m_a, m_b, m_c, m_d);
  printf("m_a=0x%x, m_b=0x%x, m_c=0x%x, m_d=0x%x\n", &m_a, &m_b, &m_c, &m_d);
}

class C2 : public B1 {
 public:
  C2(char a, int b, int c, int d);

 public:
  void display();

 private:
  // 当基类A、B的成员变量被遮蔽时，仍然会留在派生类对象obj_c的内存中，C类新增的成员变量始终排在基类A、B的后面。
  int m_b;  // 遮蔽A类的成员变量
  int m_c;  // 遮蔽B类的成员变量
  int m_d;  // 新增成员变量
};
C2::C2(char a, int b, int c, int d) : B1(a, b, c), m_b(b), m_c(c), m_d(d) {}
void C2::display() {
  printf("A::m_a=%d, A::m_b=%d, B::m_c=%d\n", m_a, A1::m_b, B1::m_c);
  printf("C::m_b=%d, C::m_c=%d, C::m_d=%d\n", m_b, m_c, m_d);
  printf("m_a=0x%x, A1::m_b=0x%x, B1::m_c=0x%x,m_b=0x%x, m_c=0x%x, m_d=0x%x\n",
         &m_a, &(A1::m_b), &(B1::m_c), &m_b, &m_c, &m_d);
}

int main() {
  // 通过obj访问成员函数func()时，在C类的作用域中没有找到func这个名字，编译器继续到B类的作用域（外层作用域）中查找，仍然没有找到，再继续到A类的作用域中查找，
  // 结果就发现了func这个名字，于是查找结束，编译器决定调用A类作用域中的func()函数。
  // 这个过程叫做名字查找（name lookup），也就是在作用域链中寻找与所用名字最匹配的声明（或定义）的过程。
  C obj;
  obj.n;
  obj.func();                 // c.biancheng.net
  cout << sizeof(C) << endl;  // 20

  C1 obj_c(84, 23, 95, 60);
  obj_c.display();
  // m_a=84, m_b=23, m_c=95, m_d=60
  // m_a=0x7aabd5d0, m_b=0x7aabd5d4, m_c=0x7aabd5d8, m_d=0x7aabd5dc

  C2 obj_c1(84, 23, 95, 60);
  obj_c1.display();
  // A::m_a=84, A::m_b=23, B::m_c=95
  // C::m_b=23, C::m_c=95, C::m_d=60
  // m_a=0x60b81a40, A1::m_b=0x60b81a44, B1::m_c=0x60b81a48,m_b=0x60b81a4c, m_c=0x60b81a50, m_d=0x60b81a54

  return 0;
}