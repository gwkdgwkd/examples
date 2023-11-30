#include <iostream>

// 在C++中，类中有public、protected、private三种属性的成员：
// 1.通过对象可以访问public成员；
// 2.只有本类中的函数可以访问本类的private成员；
// 3.借助友元，可以使得其他类的成员函数以及全局函数访问private成员。

// 在当前类以外定义的、不属于当前类的函数也可以在类中声明，
// 但要在前面加friend关键字，这样就构成了友元函数。
// 友元函数可以是不属于任何类的非成员函数，也可以是其他类的成员函数。
// 友元函数与当前类的成员函数一样，可以访问当前类中的所有成员，
// 包括public、protected、private属性的。

namespace n1 {
// 一般情况下，类必须在正式声明之后才能使用，
// 但是某些情况下，只要做好提前声明，也可以先使用。
class A;

void func() {
  // 类的提前声明的使用范围是有限的，不能直接创建对象，
  // 只有在正式声明一个类以后才能用它去创建对象。
  // 因为创建对象时要为对象分配内存，在正式定义类之前，
  // 编译器无法确定应该为对象分配多大的内存。
  // A a;
  // aggregate ‘n1::A a’ has incomplete type and cannot be defined

  // 提前声明后，可以定义指向该类型对象的指针变量或引用变量，
  // 因为指针和引用本身的大小是固定的，与它所指向的数据的大小无关。
  A *p;
  A &r = *p;

  std::cout << sizeof(p) << std::endl;  // 8
  // std::cout << sizeof(r) << std::endl;  // 报错
}
}  // namespace n1

namespace n2 {
class A;  // 提前声明，为了B中的函数能使用A类型的指针
class B {
 public:
  // 只能声明，不能定义，因为A是提前声明，此时不知道A的成员：
  void print(A *p);
};
class A {
 public:
  A() : a(1), b(2), c(3) {}

  // friend函数不仅可以是全局函数（非成员函数）：
  friend void print(A *p);
  // 还可以是另外一个类的成员函数：
  friend void B::print(A *p);
  // 甚至还可以是友元类：
  friend class C;
  // 有的编译器也可以不写class关键字，不过为了增强兼容性还是建议写上。
  // 除非有必要，一般不建议把整个类声明为友元类，
  // 而只将某些成员函数声明为友元函数，这样更安全一些。

 public:
  int a;

 protected:
  int b;

 private:
  int c;
};

// 在A定义过后，再实现B的print函数，此时才知道A的成员变量：
void B::print(A *p) {
  std::cout << "B print:" << p->a << " " << p->b << " " << p->c << std::endl;
}

void print(A *p) {
  std::cout << "global print:" << p->a << " " << p->b << " " << p->c
            << std::endl;
}

class C {  // 友元类中的所有成员函数都是另外一个类的友元函数
 public:
  // 友元函数不同于类的成员函数，在友元函数中不能直接访问类的成员，必须要借助对象：
  void print(A *p) {  // 指针
    std::cout << "C func *:" << p->a << " " << p->b << " " << p->c << std::endl;
  }
  void print(A &r) {  // 引用
    std::cout << "C func &:" << r.a << " " << r.b << " " << r.c << std::endl;
  }
};

void func() {
  A a;
  print(&a);  // global print:1 2 3

  B b;
  b.print(&a);  // B print:1 2 3

  C c;
  c.print(&a);  // C func *:1 2 3
  c.print(a);   // C func &:1 2 3
}
}  // namespace n2

namespace n3 {
class B;
class A {
 public:
  friend void print(A *, B &);

 private:
  int a = 5;
};
class B {
 public:
  friend void print(A *, B &);

 private:
  int b = 6;
};
// 一个函数可以被多个类声明为友元函数，这样就可以访问多个类中的private成员：
void print(A *pa, B &pb) { std::cout << pa->a << " " << pb.b << std::endl; }

void func() {
  A a;
  B b;
  print(&a, b);  // 5 6
}
}  // namespace n3

namespace n4 {
class B;
class A {
 public:
  friend class B;

  // 友元的关系是单向的而不是双向的，类B是类A的友元类，不等于类A是类B的友元类：
  void print(B *);

 private:
  int a = 4;
};

class B {
 public:
  void print(A *p) { std::cout << p->a << std::endl; }
  friend class C;

  int b1 = 1;

 private:
  int b2 = 2;
};

void A::print(B *p) {
  std::cout << p->b1 << std::endl;
  // A不是B的友元函数，不能访问B私有数据：
  // std::cout << p->b2 << std::endl;
}

class C {
 public:
  void print(B *p) { std::cout << p->b2 << std::endl; }
  // 友元的关系不能传递：
  void print(A *p) {
    // B是A的友元类，C是B的友元类，不等于C是A的友元类，不能访问A的私有成员：
    // std::cout << p->a << std::endl;
  }
};

void func() {
  A a;
  B b;
  b.print(&a);  // 4
  a.print(&b);  // 1

  C c;
  c.print(&b);  // 2
  c.print(&a);
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}