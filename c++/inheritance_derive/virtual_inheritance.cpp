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

// 在虚继承中，虚基类是由最终的派生类初始化的，换句话说，最终派生类的构造函数必须要调用虚基类的构造函数。对最终的派生类来说，虚基类是间接基类，而不是直接基类。
// 这跟普通继承不同，在普通继承中，派生类构造函数中只能调用直接基类的构造函数，不能调用间接基类的。
// 虚继承时构造函数的执行顺序与普通继承时不同：在最终派生类的构造函数调用列表中，不管各个构造函数出现的顺序如何，编译器总是先调用虚基类的构造函数，
// 再按照出现的顺序调用其他的构造函数；而对于普通继承，就是按照构造函数出现的顺序依次调用的。
class A2 {
 public:
  A2(int a);

 protected:
  int m_a;
};
A2::A2(int a) : m_a(a) { cout << "A2 constructors" << endl; }
class B2 : virtual public A2 {
 public:
  B2(int a, int b);

 public:
  void display();

 protected:
  int m_b;
};
B2::B2(int a, int b) : A2(a), m_b(b) { cout << "B2 constructors" << endl; }
void B2::display() { cout << "m_a=" << m_a << ", m_b=" << m_b << endl; }
class C2 : virtual public A2 {
 public:
  C2(int a, int c);

 public:
  void display();

 protected:
  int m_c;
};
C2::C2(int a, int c) : A2(a), m_c(c) { cout << "C2 constructors" << endl; }
void C2::display() { cout << "m_a=" << m_a << ", m_c=" << m_c << endl; }
class D2 : public B2, public C2 {
 public:
  D2(int a, int b, int c, int d);

 public:
  void display();

 private:
  int m_d;
};
// 在最终派生类D的构造函数中，除了调用B和C的构造函数，还调用了A的构造函数，这说明D不但要负责初始化直接基类B和C，还要负责初始化间接基类A。
// 而在以往的普通继承中，派生类的构造函数只负责初始化它的直接基类，再由直接基类的构造函数初始化间接基类，用户尝试调用间接基类的构造函数将导致错误。
// 虚基类A在最终派生类D中只保留了一份成员变量m_a，如果由B和C初始化m_a，那么B和C在调用A的构造函数时很有可能给出不同的实参，编译器不知道使用哪个实参初始化m_a。
// 为了避免出现这种矛盾的情况，C++干脆规定必须由最终的派生类D来初始化虚基类A，直接派生类B和C对A的构造函数的调用是无效的。
// D2::D2(int a, int b, int c, int d) : A2(a), B2(90, b), C2(100, c), m_d(d) {
D2::D2(int a, int b, int c, int d) : C2(100, c), B2(90, b), A2(a), m_d(d) {
  cout << "D2 constructors" << endl;
}
void D2::display() {
  cout << "m_a=" << m_a << ", m_b=" << m_b << ", m_c=" << m_c << ", m_d=" << m_d
       << endl;
}

// 简单的面向对象，只有单继承或多继承的情况下，内存模型很好理解，编译器实现起来也容易，C++的效率和C的效率不相上下。
// 一旦和virtual关键字扯上关系，使用到虚继承或虚函数，内存模型就变得混乱起来，各种编译器的实现也不一致，让人抓狂。
// 这是因为C++标准仅对C++的实现做了框架性的概述，并没有规定细节如何实现，所以不同厂商的编译器在具体实现方案上会有所差异。
// 对于普通继承，基类子对象始终位于派生类对象的前面（也即基类成员变量始终在派生类成员变量的前面），而且不管继承层次有多深，它相对于派生类对象顶部的偏移量是固定的。
// 编译器在知道对象首地址的情况下，通过计算偏移来存取成员变量。对于普通继承，基类成员变量的偏移是固定的，不会随着继承层级的增加而改变，存取起来非常方便。
// 而对于虚继承，恰恰和普通继承相反，大部分编译器会把基类成员变量放在派生类成员变量的后面，这样随着继承层级的增加，基类成员变量的偏移就会改变，就得通过其他方案来计算偏移量。
// 虚继承时的派生类对象被分成了两部分：
//  不会随着继承层次的增加而改变，称为固定部分；
//  偏移量会随着继承层次的增加而改变，称为共享部分。
// 当要访问对象的成员变量时，需要知道对象的首地址和变量的偏移，对象的首地址很好获得，关键是变量的偏移。对于固定部分，偏移是不变的，很好计算；
// 而对于共享部分，偏移会随着继承层次的增加而改变，这就需要设计一种方案，在偏移不断变化的过程中准确地计算偏移。
// 各个编译器正是在设计这一方案时出现了分歧，不同的编译器设计了不同的方案来计算共享部分的偏移。
// 对于虚继承，将派生类分为固定部分和共享部分，并把共享部分放在最后，几乎所有的编译器都在这一点上达成了共识。主要的分歧就是如何计算共享部分的偏移，可谓是百花齐放，没有统一标准。
// 早期的cfront编译器会在派生类对象中安插一些指针，每个指针指向一个虚基类的子对象，要存取继承来的成员变量，可以使用指针间接完成。
// 通过上面的分析可以发现，这种方案的一个缺点就是，随着虚继承层次的增加，访问顶层基类需要的间接转换会越来越多，效率越来越低。
// 这种方案另外的一个缺点是：当有多个虚基类时，派生类要为每个虚基类都安插一个指针，会增加对象的体积。
// VC引入了虚基类表，如果某个派生类有一个或多个虚基类，编译器就会在派生类对象中安插一个指针，指向虚基类表。虚基类表其实就是一个数组，数组中的元素存放的是各个虚基类的偏移。
// 虚继承表中保存的是所有虚基类（包括直接继承和间接继承到的）相对于当前对象的偏移，这样通过派生类指针访问虚基类的成员变量时，不管继承层次都多深，只需要一次间接转换就可以。
// 另外，这种方案还可以避免有多个虚基类时让派生类对象额外背负过多的指针。
class A3 {
 protected:
  int m_a1;
  int m_a2;
};
class B3 : public A3 {
 protected:
  int b1;
  int b2;
};
class C3 : public B3 {
 protected:
  int c1;
  int c2;
};
class D3 : public C3 {
 protected:
  int d1;
  int d2;
};

int main() {
  D d;
  cout << sizeof(d) << endl;  // 20

  D1 d1;
  cout << sizeof(d1) << endl;  // 40

  B2 b(10, 20);
  b.display();  // m_a=10, m_b=20
  // A2 constructors
  // B2 constructors
  C2 c(30, 40);
  c.display();  // m_a=30, m_c=40
  // A2 constructors
  // C2 constructors
  D2 d2(50, 60, 70, 80);
  d2.display();  // m_a=50, m_b=60, m_c=70, m_d=80
  // A2 constructors
  // B2 constructors
  // C2 constructors
  // D2 constructors

  // A是最顶层的基类，在派生类B、C、D的对象中，A类子对象始终位于最前面，偏移量是固定的，为0。b1、b2是派生类B的新增成员变量，它们的偏移量也是固定的，分别为8和12。
  // c1、c2、d1、d2 也是同样的道理。
  A3 obj_a;  // a1 a2
  B3 obj_b;  // A(a1 a2) b1 b2
  C3 obj_c;  // B(A(a1 a2) b1 b2) c1 c2
  D3 obj_d;  // C(B(A(a1 a2) b1 b2) c1 c2) d1 d2
  // class B3: virtual public A3
  // obj_b: b1 b2 A(a1 a2)
  // obj_c: c1 c2 B(b1 b2 A(a1 a2))
  // obj_d: c1 c2 d1 d2 B(b1 b2 A(a1 a2))

  return 0;
}