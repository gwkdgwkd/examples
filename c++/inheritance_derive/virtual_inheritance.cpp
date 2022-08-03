#include <iostream>

// 多继承（Multiple Inheritance）是指从多个直接基类中产生派生类的能力，
// 多继承的派生类继承了所有父类的成员。
// 尽管概念上非常简单，但是多个基类的相互交织可能会带来错综复杂的设计问题，
// 命名冲突就是不可回避的一个。
// 多继承时很容易产生命名冲突，
// 即使很小心地将所有类中的成员变量和成员函数都命名为不同的名字，
// 命名冲突依然有可能发生，比如典型的是菱形继承。
// 在一个派生类中保留间接基类的多份同名成员，
// 虽然可以在不同的成员变量中分别存放不同的数据，
// 但大多数情况下这是多余的：
// 因为保留多份成员变量不仅占用较多的存储空间，还容易产生命名冲突。

// 为了解决多继承时的命名冲突和冗余数据问题，
// C++提出了虚继承，使得在派生类中只保留一份间接基类的成员。
// 虚继承的目的是让某个类做出声明，承诺愿意共享它的基类。
// 其中，这个被共享的基类就称为虚基类（Virtual Base Class）。
// 在这种机制下，不论虚基类在继承体系中出现了多少次，
// 在派生类中都只包含一份虚基类的成员。

// 必须在虚派生的真实需求出现前就已经完成虚派生的操作。
// 换个角度讲，虚派生只影响从指定了虚基类的派生类中进一步派生出来的类，
// 它不会影响派生类本身。
// C++标准库中的iostream类就是一个虚继承的实际应用案例：
// iostream从istream和ostream直接继承而来，
// 而istream和ostream又都继承自一个共同的名为base_ios的类，是典型的菱形继承。
// 此时 istream和ostream必须采用虚继承，
// 否则将导致iostream类中保留两份base_ios类的成员。
// 因为在虚继承的最终派生类中只保留了一份虚基类的成员，
// 所以该成员可以被直接访问，不会产生二义性。
// 此外，如果虚基类的成员只被一条派生路径覆盖，
// 那么仍然可以直接访问这个被覆盖的成员。
// 但是如果该成员被两条或多条路径覆盖了，
// 那就不能直接访问了，此时必须指明该成员属于哪个类。

// 假设A定义了一个名为x的成员变量，当在D中直接访问x时，会有三种可能性：
// 1.如果B和C中都没有x的定义，那么x将被解析为A的成员，此时不存在二义性。
// 2.如果B或C其中的一个类定义了x，也不会有二义性，派生类的x比虚基类的x优先级更高。
// 3.如果B和C中都定义了x，那么直接访问x将产生二义性问题。
// 不提倡在程序中使用多继承，能用单一继承解决的问题就不要使用多继承。
// 也正是由于这个原因，C++之后的很多面向对象的编程语言，
// 例如Java、C#、PHP等，都不支持多继承。

namespace n1 {
class A {
 public:
  int a = 1;
};
class B : public A {
 public:
  int b = 2;
};
class C : public A {
 public:
  int c = 3;
};
class D : public B, public C {
 public:
  int d = 4;
};

void testN1() {
  D obj;
  std::cout << sizeof(obj) << std::endl;  // 20

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;  // 0x7fffae6cfa00
  // std::cout << d.a << std::endl;  // 有两个a，产生歧义
  std::cout << &(obj.B::a) << std::endl;  // 0x7fffae6cfa00
  std::cout << &(obj.b) << std::endl;     // 0x7fffae6cfa04
  std::cout << &(obj.C::a) << std::endl;  // 0x7fffae6cfa08
  std::cout << &(obj.c) << std::endl;     // 0x7fffae6cfa0c
  std::cout << &(obj.d) << std::endl;     // 0x7fffae6cfa10
}
}  // namespace n1

namespace n2 {
class A {
 public:
  int a = 1;
};
class B : virtual public A {
 public:
  int b = 2;
};
class C : virtual public A {
 public:
  int c = 3;
};
class D : public B, public C {
 public:
  int d = 4;
};
void func1() {
  A obj;
  std::cout << sizeof(obj) << std::endl;  // 4

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7fff0e1be5b4
  std::cout << &(obj.a) << std::endl;  // 0x7fff0e1be5b4
}

void func2() {
  B obj;
  std::cout << sizeof(obj) << std::endl;  // 16

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7fff0e1be5a0
  std::cout << &(obj.b) << std::endl;  // 0x7fff0e1be5a8
  std::cout << &(obj.a) << std::endl;  // 0x7fff0e1be5ac
}

void func3() {
  C obj;
  std::cout << sizeof(obj) << std::endl;  // 16

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7fff0e1be5a0
  std::cout << &(obj.c) << std::endl;  // 0x7fff0e1be5a8
  std::cout << &(obj.a) << std::endl;  // 0x7fff0e1be5ac
}

void func4() {
  D obj;
  std::cout << sizeof(obj) << std::endl;  // 40

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;         // 0x7fff0e1be590
  std::cout << &(obj.b) << std::endl;     // 0x7fff0e1be598
  std::cout << &(obj.c) << std::endl;     // 0x7fff0e1be5a8
  std::cout << &(obj.d) << std::endl;     // 0x7fff0e1be5ac
  std::cout << &(obj.a) << std::endl;     // 0x7fff0e1be5b0
  std::cout << &(obj.B::a) << std::endl;  // 0x7fff0e1be5b0
  std::cout << &(obj.C::a) << std::endl;  // 0x7fff0e1be5b0
}
void testN2() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n2

namespace n3 {
// 在虚继承中，虚基类是由最终的派生类初始化的，
// 换句话说，最终派生类的构造函数必须要调用虚基类的构造函数。
// 对最终的派生类来说，虚基类是间接基类，而不是直接基类。
// 这跟普通继承不同，在普通继承中，
// 派生类构造函数中只能调用直接基类的构造函数，不能调用间接基类的。
// 虚继承时构造函数的执行顺序与普通继承时不同：
// 在最终派生类的构造函数调用列表中，不管各个构造函数出现的顺序如何，
// 编译器总是先调用虚基类的构造函数，再按照出现的顺序调用其他的构造函数；
// 而对于普通继承，就是按照构造函数出现的顺序依次调用的。
class A {
 public:
  A(int a) : m_a(a) { std::cout << "A constructors" << std::endl; }

 protected:
  int m_a;
};
class B : virtual public A {
 public:
  B(int a, int b) : A(a), m_b(b) { std::cout << "B constructors" << std::endl; }
  void display() { std::cout << "m_a=" << m_a << ", m_b=" << m_b << std::endl; }

 protected:
  int m_b;
};
class C : virtual public A {
 public:
  C(int a, int c) : A(a), m_c(c) { std::cout << "C constructors" << std::endl; }
  void display() { std::cout << "m_a=" << m_a << ", m_c=" << m_c << std::endl; }

 protected:
  int m_c;
};
class D : public B, public C {
 public:
  // 在最终派生类D的构造函数中，除了调用B和C的构造函数，还调用了A的构造函数，
  // 这说明D不但要负责初始化直接基类B和C，还要负责初始化间接基类A。
  // 而在以往的普通继承中，派生类的构造函数只负责初始化它的直接基类，
  // 再由直接基类的构造函数初始化间接基类，
  // 用户尝试调用间接基类的构造函数将导致错误。
  // 虚基类A在最终派生类D中只保留了一份成员变量m_a，如果由B和C初始化m_a，
  // 那么B和C在调用A的构造函数时很有可能给出不同的实参，
  // 编译器不知道使用哪个实参初始化m_a。
  // 为了避免出现这种矛盾的情况，C++干脆规定必须由最终的派生类D来初始化虚基类A，
  // 直接派生类B和C对A的构造函数的调用是无效的。
  D(int a, int b, int c, int d) : C(100, c), B(90, b), A(a), m_d(d) {
    std::cout << "D constructors" << std::endl;
  }
  void display() {
    std::cout << "m_a=" << m_a << ", m_b=" << m_b << ", m_c=" << m_c
              << ", m_d=" << m_d << std::endl;
  }

 private:
  int m_d;
};

void testN3() {
  B b(10, 20);
  b.display();
  // A constructors
  // B constructors
  // m_a=10, m_b=20

  C c(30, 40);
  c.display();
  // A constructors
  // C constructors
  // m_a=30, m_c=40

  D d(50, 60, 70, 80);
  d.display();
  // A constructors
  // B constructors
  // C constructors
  // D constructors
  // m_a=50, m_b=60, m_c=70, m_d=80
}
}  // namespace n3

namespace n4 {
// vbptr指的是虚基类表指针（virtual base table pointer），
// 该指针指向了一个虚表（virtual table），
// 虚表中记录了vbptr与本类的偏移地址；
// 第二项是vbptr到共有基类元素之间的偏移量。

// 简单的面向对象，只有单继承或多继承的情况下，内存模型很好理解，
// 编译器实现起来也容易，C++的效率和C的效率不相上下。
// 一旦和virtual关键字扯上关系，使用到虚继承或虚函数，
// 内存模型就变得混乱起来，各种编译器的实现也不一致，让人抓狂。
// 这是因为C++标准仅对C++的实现做了框架性的概述，并没有规定细节如何实现，
// 所以不同厂商的编译器在具体实现方案上会有所差异。
// 对于普通继承，基类子对象始终位于派生类对象的前面，
// 即基类成员变量始终在派生类成员变量的前面，
// 而且不管继承层次有多深，它相对于派生类对象顶部的偏移量是固定的。
// 编译器在知道对象首地址的情况下，通过计算偏移来存取成员变量。
// 对于普通继承，基类成员变量的偏移是固定的，
// 不会随着继承层级的增加而改变，存取起来非常方便。
// 而对于虚继承，恰恰和普通继承相反，
// 大部分编译器会把基类成员变量放在派生类成员变量的后面，
// 这样随着继承层级的增加，基类成员变量的偏移就会改变，
// 就得通过其他方案来计算偏移量。
// 虚继承时的派生类对象被分成了两部分：
// 1.不会随着继承层次的增加而改变，称为固定部分；
// 2.偏移量会随着继承层次的增加而改变，称为共享部分。
// 当要访问对象的成员变量时，需要知道对象的首地址和变量的偏移，
// 对象的首地址很好获得，关键是变量的偏移。
// 对于固定部分，偏移是不变的，很好计算；
// 而对于共享部分，偏移会随着继承层次的增加而改变，
// 这就需要设计一种方案，在偏移不断变化的过程中准确地计算偏移。
// 各个编译器正是在设计这一方案时出现了分歧，
// 不同的编译器设计了不同的方案来计算共享部分的偏移。
// 对于虚继承，将派生类分为固定部分和共享部分，
// 并把共享部分放在最后，几乎所有的编译器都在这一点上达成了共识。
// 主要的分歧就是如何计算共享部分的偏移，可谓是百花齐放，没有统一标准。
// 早期的cfront编译器会在派生类对象中安插一些指针，
// 每个指针指向一个虚基类的子对象，要存取继承来的成员变量，
// 可以使用指针间接完成。
// 通过上面的分析可以发现，这种方案的一个缺点就是，随着虚继承层次的增加，
// 访问顶层基类需要的间接转换会越来越多，效率越来越低。
// 这种方案另外的一个缺点是：
// 当有多个虚基类时，派生类要为每个虚基类都安插一个指针，会增加对象的体积。
// VC引入了虚基类表，如果某个派生类有一个或多个虚基类，
// 编译器就会在派生类对象中安插一个指针，指向虚基类表。
// 虚基类表其实就是一个数组，数组中的元素存放的是各个虚基类的偏移。
// 虚继承表中保存的是所有虚基类（包括直接继承和间接继承到的）相对于当前对象的偏移，
// 这样通过派生类指针访问虚基类的成员变量时，
// 不管继承层次都多深，只需要一次间接转换就可以。
// 另外，这种方案还可以避免有多个虚基类时让派生类对象额外背负过多的指针。
namespace test1 {
class A {
 public:
  int a;
};
class B : public A {
 public:
  int b;
};
class C : public B {
 public:
  int c;
};
class D : public C {
 public:
  int d;
};

void func1() {
  A obj;  // 内存布局： a
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;  // 4

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7ffe22396f04
  std::cout << &(obj.a) << std::endl;  // 0x7ffe22396f04
}
void func2() {
  B obj;  // 内存布局： A(a) b
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;  // 8

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7ffe22396f00
  std::cout << &(obj.a) << std::endl;  // 0x7ffe22396f00
  std::cout << &(obj.b) << std::endl;  // 0x7ffe22396f04
}
void func3() {
  C obj;  // 内存布局： B(A(a) b) c
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;  // 12

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7ffe22396efc
  std::cout << &(obj.a) << std::endl;  // 0x7ffe22396efc
  std::cout << &(obj.b) << std::endl;  // 0x7ffe22396f00
  std::cout << &(obj.c) << std::endl;  // 0x7ffe22396f04
}
void func4() {
  D obj;  // 内存布局： C(B(A(a) b) c) d
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;  // 16

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;      // 0x7ffe22396ef0
  std::cout << &(obj.a) << std::endl;  // 0x7ffe22396ef0
  std::cout << &(obj.b) << std::endl;  // 0x7ffe22396ef4
  std::cout << &(obj.c) << std::endl;  // 0x7ffe22396ef8
  std::cout << &(obj.d) << std::endl;  // 0x7ffe22396efc
}
}  // namespace test1

namespace test2 {
// 为什么B、C和D的第二个虚基类表中的内容都不对？
// D的第一个虚基类表中的16和-16是怎么来的？

// 通过虚基类表指针往正负两个方向寻址，
// 可以获得不同偏移值，也就是说有两个功能一样的虚函数表。
// 不过在实际应用的时候，不知道虚基类表是否真的有用。
// 发现编译器可能做了优化，根本就没有用虚基类表来寻址虚基类实例。

class A {
 public:
  int a = 1;
};
class B : virtual public A {
 public:
  int b = 2;
};
class C : virtual public A {
 public:
  int c = 3;
};
class D : public B, public C {
 public:
  int d = 4;
};

void func1() {
  A obj;  // 内存布局： a
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;
  std::cout << &(obj.a) << " (" << obj.a << ")" << std::endl;

  // 4
  // 0x7fffa746baf4
  // 0x7fffa746baf4 (1)
}
void func2() {
  B obj;  // 内存布局： vbptr b A(a)
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << " " << std::endl;

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;
  std::cout << (int*)&obj << ": " << *(long*)(*(long*)&obj) << std::endl;
  std::cout << &(obj.b) << " (" << obj.b << ")" << std::endl;
  std::cout << &(obj.a) << " (" << obj.a << ")" << std::endl;

  // 16
  // 0x7fffa746bae0
  // 0x7fffa746bae0: 94109290195024
  // 0x7fffa746bae8 (2)
  // 0x7fffa746baec (1)
}
void func3() {
  C obj;  // 内存布局： vbptr c A(a)
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;
  std::cout << (int*)&obj << ": " << *(long*)(*(long*)&obj) << std::endl;
  std::cout << &(obj.c) << " (" << obj.c << ")" << std::endl;
  std::cout << &(obj.a) << " (" << obj.a << ")" << std::endl;

  // 16
  // 0x7fffa746bae0
  // 0x7fffa746bae0: 94109290194992
  // 0x7fffa746bae8 (3)
  // 0x7fffa746baec (1)
}
void func4() {
  D obj;  // 内存布局： vbptrB B(b) vbptrC C(c) d A(a)
  std::cout.setf(std::ios::dec);
  std::cout << sizeof(obj) << std::endl;

  std::cout.setf(std::ios::hex);
  std::cout.setf(std::ios_base::showbase);
  std::cout << &obj << std::endl;
  std::cout << (long*)&obj << std::endl;
  std::cout << "  [0]:" << *(long*)(*(long*)&obj) << std::endl;
  std::cout << "  [1]:" << *(((long*)(*(long*)&obj)) + 1) << std::endl;
  std::cout << &(obj.b) << " (" << obj.b << ")" << std::endl;
  std::cout << (long*)((char*)&obj + sizeof(B)) << ": "
            << *(long*)((char*)&obj + sizeof(B)) << std::endl;
  std::cout << &(obj.c) << " (" << obj.c << ")" << std::endl;
  std::cout << &(obj.d) << " (" << obj.d << ")" << std::endl;
  std::cout << &(obj.a) << " (" << obj.a << ")" << std::endl;

  // 40
  // 0x7ffe37fb6b00
  // 0x7ffe37fb6b00
  //   [0]:16
  //   [1]:-16
  // 0x7ffe37fb6b08 (2)
  // 0x7ffe37fb6b10: 93991712532720
  // 0x7ffe37fb6b18 (3)
  // 0x7ffe37fb6b1c (4)
  // 0x7ffe37fb6b20 (1)
}
}  // namespace test2

namespace test3 {
class A {
 public:
  int dataA;
};

class B : virtual public A {
 public:
  int dataB;
};

class C : virtual public A {
 public:
  int dataC;
};

class D : public B, public C {
 public:
  int dataD;
};

void func() {
  D* d = new D;
  d->dataA = 10;
  d->dataB = 100;
  d->dataC = 1000;
  d->dataD = 10000;

  B* b = d;  // 转化为基类B
  C* c = d;  // 转化为基类C
  A* fromB = (B*)d;
  A* fromC = (C*)d;

  std::cout << "d address    : " << d << std::endl;
  std::cout << "b address    : " << b << std::endl;
  std::cout << "c address    : " << c << std::endl;
  std::cout << "fromB address: " << fromB << std::endl;
  std::cout << "fromC address: " << fromC << std::endl;
  // d address    : 0x562543e392c0
  // b address    : 0x562543e392c0
  // c address    : 0x562543e392d0
  // fromB address: 0x562543e392e0
  // fromC address: 0x562543e392e0

  std::cout << "vbptr address: " << (long*)d << std::endl;
  std::cout << "    [0] => " << *(long*)(*(long*)d) << std::endl;
  std::cout << "    [1] => " << *(((long*)(*(long*)d)) + 1) << std::endl;
  std::cout << "dataB value  : " << (int*)((long*)d + 1) << ","
            << *(int*)((long*)d + 1) << std::endl;
  std::cout << "vbptr address: " << ((long*)d + 2) << std::endl;
  std::cout << "    [0] => " << *(long*)(*((long*)d + 2)) << std::endl;
  std::cout << "    [1] => " << *((long*)(*((long*)d + 2)) + 1) << std::endl;
  std::cout << "dataC value  : " << (int*)((long*)d + 3) << ","
            << *(int*)((long*)d + 3) << std::endl;
  std::cout << "dataD value  : " << (int*)((int*)d + 7) << ","
            << *(int*)((int*)d + 7) << std::endl;
  std::cout << "dataA value  : " << (int*)((int*)d + 8) << ","
            << *(int*)((int*)d + 8) << std::endl;
  // vbptr address: 0x562543e392c0
  //     [0] => 16
  //     [1] => -16
  // dataB value  : 0x562543e392c8,100
  // vbptr address: 0x562543e392d0
  //     [0] => 94718024296536
  //     [1] => 94718024296616
  // dataC value  : 0x562543e392d8,1000
  // dataD value  : 0x562543e392dc,10000
  // dataA value  : 0x562543e392e0,10
}
}  // namespace test3

void testN4() {
  test1::func1();
  test1::func2();
  test1::func3();
  test1::func4();

  test2::func1();
  test2::func2();
  test2::func3();
  test2::func4();

  test3::func();
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
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
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}