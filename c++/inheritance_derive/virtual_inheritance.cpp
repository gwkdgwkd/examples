#include <iostream>

using namespace std;

// 多继承（Multiple Inheritance）是指从多个直接基类中产生派生类的能力，多继承的派生类继承了所有父类的成员。
// 尽管概念上非常简单，但是多个基类的相互交织可能会带来错综复杂的设计问题，命名冲突就是不可回避的一个。
// 多继承时很容易产生命名冲突，即使我们很小心地将所有类中的成员变量和成员函数都命名为不同的名字，命名冲突依然有可能发生，比如典型的是菱形继承。
// 在一个派生类中保留间接基类的多份同名成员，虽然可以在不同的成员变量中分别存放不同的数据，但大多数情况下这是多余的：因为保留多份成员变量不仅占用较多的存储空间，还容易产生命名冲突。
// 为了解决多继承时的命名冲突和冗余数据问题，C++提出了虚继承，使得在派生类中只保留一份间接基类的成员。
// 虚继承的目的是让某个类做出声明，承诺愿意共享它的基类。其中，这个被共享的基类就称为虚基类（Virtual Base Class）。
// 在这种机制下，不论虚基类在继承体系中出现了多少次，在派生类中都只包含一份虚基类的成员。
// 必须在虚派生的真实需求出现前就已经完成虚派生的操作。换个角度讲，虚派生只影响从指定了虚基类的派生类中进一步派生出来的类，它不会影响派生类本身。
// C++标准库中的iostream类就是一个虚继承的实际应用案例。iostream从istream和ostream直接继承而来，而istream和ostream又都继承自一个共同的名为base_ios的类，是典型的菱形继承。
// 此时 istream和ostream必须采用虚继承，否则将导致iostream类中保留两份base_ios类的成员。
// 因为在虚继承的最终派生类中只保留了一份虚基类的成员，所以该成员可以被直接访问，不会产生二义性。
// 此外，如果虚基类的成员只被一条派生路径覆盖，那么仍然可以直接访问这个被覆盖的成员。
// 但是如果该成员被两条或多条路径覆盖了，那就不能直接访问了，此时必须指明该成员属于哪个类。
// 假设A定义了一个名为x的成员变量，当在D中直接访问x时，会有三种可能性：
//  如果B和C中都没有x的定义，那么x将被解析为A的成员，此时不存在二义性。
//  如果B或C其中的一个类定义了x，也不会有二义性，派生类的x比虚基类的x优先级更高。
//  如果B和C中都定义了x，那么直接访问x将产生二义性问题。
// 不提倡在程序中使用多继承，只有在比较简单和不易出现二义性的情况或实在必要时才使用多继承，能用单一继承解决的问题就不要使用多继承。
// 也正是由于这个原因，C++ 之后的很多面向对象的编程语言，例如 Java、C#、PHP 等，都不支持多继承。

class A {
 protected:
  int m_a;
};

class B : public A {
 protected:
  int m_b;
};
class C : public A {
 protected:
  int m_c;
};

class D : public B, public C {
 public:
  // 因为类B和类C中都有成员变量 m_a（从A类继承而来），编译器不知道选用哪一个，所以产生了歧义。
  // void seta(int a) { m_a = a; }  // 命名冲突
  void seta(int a) { B::m_a = a; }
  // void seta(int a) { C::m_a = a; }
  void setb(int b) { m_b = b; }  // 正确
  void setc(int c) { m_c = c; }  // 正确
  void setd(int d) { m_d = d; }  // 正确

 private:
  int m_d;
};

class A1 {
 protected:
  int m_a;
};
class B1 : virtual public A1 {  // 虚继承
 protected:
  int m_b;
};
class C1 : virtual public A1 {  // 虚继承
 protected:
  int m_c;
};
class D1 : public B1, public C1 {
 public:
  void seta(int a) { m_a = a; }  // 正确
  void setb(int b) { m_b = b; }  // 正确
  void setc(int c) { m_c = c; }  // 正确
  void setd(int d) { m_d = d; }  // 正确

 private:
  int m_d;
};

int main() {
  D d;
  cout << sizeof(d) << endl;  // 20
  D1 d1;
  cout << sizeof(d1) << endl;  // 40

  return 0;
}