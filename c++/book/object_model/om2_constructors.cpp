#include <iostream>

// 构造函数语义学

namespace n1 {
// 2.1 Default Constructor的创建

// 默认构造函数不带任何参数，在需要的时候被编译器产生出来。
// 是编译器需要，不是程序需要，程序需要，是程序员的责任。
// 如果没有任何用户定义的构造函数，那么隐式默认构造函数被声明，但这个函数是无用的。

// 默认构造函数有用的四种情况：
namespace test1 {
// 1.带有Default constructor的对象成员；

// 如果一个类没有任何构造函数，但它内含一个对象成员，
// 而后者有默认构造函数，那么这个类的隐式默认构造函数就是有用的。
// 编译器需要为这个类合成出默认构造函数，不过只有在构造函数真正需要被调用时才会合成。
// 在C++各个不同的编译模块中，编译器如何避免合成出多个默认构造函数呢？
// 解决方法是把合成的默认构造函数、拷贝构造函数、析构函数、赋值操作符都以内联方式完成。
// 一个内联函数有静态链接，不会被模块以外看到。
// 如果函数太复杂，不适合内联，就会合成出一个explicit non-inline static实体。
class A1 {
 public:
  A1() { std::cout << "A1" << std::endl; }
};
struct A2 {
  A2() { std::cout << "A2" << std::endl; }
};
class B {  // 没有定义默认构造函数
 public:
  // 被合成的默认构造函数内会调用a的的默认构造函数，这是编译器的责任，看起来像：
  // inline B::B() {
  //   a1.A1::A1();
  //   a2.A::A2();
  // }
  A1 a1;
  A2 a2;
  // 被合成的默认构造函数内并不会初始化str，初始化str是程序员的责任：
  char *str;
};
class C {  // 定义了默认构造函数
 public:
  C() {
    // 编译器会初始化对象成员a，由于默认构造函数已经被明确定义出来了，
    // 编译没办法合成第二个，所以编译器会扩展已存在的构造函数，
    // 在其中安插一些码，在用户代码被执行之前，先调用对象成员的默认构造函数：
    // a2.A2::A2();
    // a1.A1::A1();
    std::cout << "C" << std::endl;
    str = 0;  // 初始化了str，满足程序的需求
  }
  A2 a2;
  A1 a1;
  char *str;
};

void func() {
  B b;
  // A1
  // A2

  C c;
  // A2
  // A1
  // C

  // 如果有多个对象成员都要求初始化操作，无论是合成的隐式默认构造函数，
  // 还是在已经定义了默认构造函数中安插代码，
  // C++都要求以对象成员在class中的声明次序来调用各个对象成员的构造函数。
}
}  // namespace test1

namespace test2 {
// 2.带有Default constructor的Base Class；

// 类似的道理，如果一个没有任何构造函数的类，派生自一个带有默认构造函数的基类，
// 那么这个派生类的默认构造函数会是有用的，会被合成出来。
// 它将调用上一层基类的默认构造函数（根据它们的声明次序），对一个后继派生的类而言，
// 这个合成的构造函数和一个被明确提供的默认构造函数没什么差异。
class B1 {
 public:
  B1() { std::cout << "B1" << std::endl; }
};
class B2 {
 public:
  B2() { std::cout << "B2" << std::endl; }
};
class D1 : public B2, B1 {};
class D2 : public B1, B2 {
 public:
  // 编译器会扩展每个用户定义的构造函数，当然也包括定义的默认构造函数。
  // 扩展的内容是在用户代码之前按声明的顺序调用基类的默认构造函数：
  D2() { std::cout << "D2()" << std::endl; }
  D2(int) { std::cout << "D2(int)" << std::endl; }
};
class D3 : B2, B1 {
  // 如果同时存在带着默认构造函数的基类和带有默认构造函数的对象成员，
  // 先按声明顺序调用基类默认构造函数，再按声明顺序调用对象成员的默认构造函数。
  test1::A2 a2;
  test1::A1 a1;
};
void func() {
  D1 d1;
  // B2
  // B1

  D2 d21;
  // B1
  // B2
  // D2()

  D2 d22(5);
  // B1
  // B2
  // D2(int)

  D3 d3;
  // B2
  // B1
  // A2
  // A1
}
}  // namespace test2

namespace test3 {
// 3.带有一个Virtual Function的Class；

// 由于缺乏由用户声明的构造函数，编译器会详细记录合成一个默认构造的必要信息。
// 为了使多态生效，编译器必须为每一个基类或其派生类对象的vptr初始化，
// 也就是合适的virtual table地址。
// 对于类所定义每一个构造函数，编译器会安插一些代码来做这样的事情。
// 对于那些未声明任何构造函数的类，编译器会为它们合成一个默认构造函数，
// 以便正确地初始化每一个对象的vptr，编译器的工作：
// 1.一个vtbl被创建出来，存放class的虚函数地址；
// 2.每一个对象中，一个额外的指针成员vptr被编译器创建，指向vtbl的地址。
class B {
 public:
  B() { std::cout << "B" << std::endl; };
  virtual void show() const = 0;
};
class D1 : public B {
 public:
  void show() const { std::cout << "D1::show" << std::endl; }
};
class D2 : public B {
 public:
  void show() const { std::cout << "D2::show" << std::endl; }
};
void show(const B &b) {
  // b.show()的虚拟引发操作会被重新改写，以使用b的vptr和vtbl中的show地址：
  b.show();
  // 转变为：(*b.vptr[1])(&b)
  // 1表示show()在virtual vable中的固定索引；
  // &b代表要交给被调用的某个show()函数实体的this指针。
}
void func() {
  D1 d1;     // B
  D2 d2;     // B
  show(d1);  // D1::show
  show(d2);  // D2::show
}
}  // namespace test3

namespace test4 {
// 4.带有一个Virtual Base Class的Class。

// Virtual Base Class的实现在不同的编译器之间有极大的差异，但共同点在于：
// 必须使Virtual Base Class在其每一个派生类对象中的位置，能够于执行期准备妥当。
class B {
 public:
  B() { std::cout << "B" << std::endl; }
  int i;
};
class D1 : public virtual B {};
class D2 : public virtual B {
 public:
  D2() { std::cout << "D2" << std::endl; }
};
class DD : public D1, public D2 {
 public:
  DD() { std::cout << "DD" << std::endl; }
};
void test(B *pb) {
  pb->i = 1024;
  // 编译器无法确定test中由pb获得的Base::i的实际偏移位置，因为pb类型未知。
  // 编译器必须改变执行存取操作的那些代码，使Base::i可以延迟至执行期才确定。
  // 对于类定义的每个构造函数，编译器会安插允许每个虚基类的执行期存取操作的代码。
  // 如果没有声明任何构造函数，编译器必须生成一个默认构造函数，完成上面的功能。
}
void func() {
  test(new D1);
  // B

  test(new D2);
  // B
  // D2

  test(new DD);
  // B
  // D2
  // DD
}
}  // namespace test4

// 总结：
// 有四种情况，会导致编译器必须为未声明构造函数的类合成一个默认构造函数。
// C++标准把这个合成的默认构造函数称为隐式有用的默认构造函数。
// 被合成出来的构造函数只能满足编译器（而非程序）的需要。
// 它之所以能够完成任务，是借着下面3点完成的：
// 1.调用对象成员；
// 2.基类的默认构造函数;
// 3.为每一个对象初始化其虚函数机制或虚基类机制。

// 至于没有存在那四种情况而又没有声明任何构造函数的类型，
// 它们拥有的是隐式无用默认构造函数，它们实际上并不会被合成出来。
// 在合成的默认构造函数中，只有基类subobjects和对象成员会被初始化。
// 所有其他的非静态数据成员，如整数、整数指针、整数数组等等都不会被初始化。
// 这些初始化操作对程序而言或许有需要，但对编译器则并非必要。
// 如果程序需要一个把某指针设为0的默认构造函数，那么提供它的人应该是程序员。
// C++新手一般有两个常见的误解：
// 1.任何class如果没有定义默认构造函数，就会被合成出一个来；
// 2.编译器合成出来的默认构造函数会明确初始化类内每一个数据成员的默认值。
}  // namespace n1

namespace n2 {
// 2.2 Copy Constructor的创建

namespace test1 {
// 有三种情况，会以一个对象的内容作为另一个对象的初值：
// 1.最明显的一种情况当然就是对一个对象做明确的初始化操作；
// 2.当对象作为参数交给某个函数时；
// 3.以及当函数传回一个对象时。
class A {
 public:
  A() {}
  A(const A &a) { std::cout << "A copy constructor" << std::endl; }
  // 可以是多参数形式，从第二个参数起，提供默认值：A(const A &a, int = 0);
  // 大部分情况下，当一个对象以另一个同类对象作为初值时，会导致拷贝构造函数被调用。
  // 这可能会导致一个临时对象的产生或程序代码的蜕变（或者两者都有）。
};
void func1(A a) {}
A func2() {
  A a;
  return a;
}
void func() {
  A a = func2();
  func1(a);

  // g++ -fno-elide-constructors om2_constructors.cpp
  // A copy constructor
  // A copy constructor
  // A copy constructor

  // g++ om2_constructors.cpp
  // A copy constructor
}
}  // namespace test1

namespace test2 {
// 如果类没有一个显式拷贝构造函数会怎么样？
// 当对象以同类对象作为初值时，是以default memberwise initialization方式完成的。
// 也就是把一个内建的或派生的数据成员的值，从某个对象拷贝一份到另一个对象。
// 并不拷贝对象成员，而是以递归的方式实施memberwise initialization。
// 如果一个类未定义拷贝构造函数，编译器就为它自动产生出一个，这句话不对，
// 而是默认构造函数和拷贝构造函数在必要的时候才由编译器产生出来。

// 一个对象可以从两种方式复制得到：
// 1.被初始化，调用拷贝构造函数；
// 2.被赋值，调用赋值操作符。
// 与默认构造函数一样，如果没有声明一个拷贝构造函数，就会有隐式的声明或隐式的定义出现。
// C++标准把拷贝构造函数区分为有用和无用两种，只有有用的实体才会被合成于程序之中。
// 决定拷贝构造函数是否有用的标准在于：
// 类是否展现出所谓的bitwise copy semantics。

// 什么时候一个类不展现出bitwise copy semantics呢？
// 也就是说需要生成拷贝构造函数，有四种情况：
// 1.当类内含一个声明有拷贝构造函数的对象成员时，不论是被明确声明，还是被编译器合成；
// 2.当类继承自一个基类而后者存在有一个拷贝构造函数时，不论是被明确声明或是被合成而的；
// 3.当类声明了一个或多个虚函数时；
// 4.当类派生自一个继承串链，其中有一个或多个虚基类时。
// 1和2中，编译器必须将成员或基类的拷贝构造函数调用操作，安插到被合成的拷贝构造函数中。
// 对于3，需要重新设定virtual Table的指针，
// 如果编译器对于每一个新产生的对象的vptr不能成功而正确地设好其初值，将导致可怕的后果。
// 因此，当编译器导入一个vptr到class中时，该class就不再展现bitwise semantics了。
// 编译器需要合成出一个拷贝构造函数，以求将vptr适当地初始化。
// 对于4，需要处理Virtual Base Class Subobject，
// 一个对象如果以另一个object作为初值，而后者有Virtual Base Class Subobject，
// 那么bitwise copy semantics失效。
// 编译器对于虚拟继承的支持承诺，都表示必须让派生对象中的Subobject位置在执行期就准备妥当。
// 维护位置的完整性是编译器的责任，bitwise copy semantics可能会破坏这个位置，
// 所以编译器必须在它自己合成出来的拷贝构造函数中做出仲裁。

class B {
 public:
  B() {}
  B(const B &) { std::cout << "B copy" << std::endl; }
};
class A {
  // 在这种情况下，编译器必须合成出一个拷贝构造函数以便调用B成员对象的拷贝构造函数：
  // inline A(cosnt A &rhs) { b = rhs.b; }
  // 被合成出来的拷贝构造函数中，如整数、指针、数组等等的非类对象成员也都会被复制。

  B b;
};
class D : public B {};
void func() {
  A a1;
  A a2 = a1;
  // B copy

  D d1;
  D d2(d1);
  // B copy
}
}  // namespace test2
}  // namespace n2

namespace n3 {
// 2.3 程序转化语意学

class A {};
A func1() {  // 返回对象
  A a;
  return a;
  // 做出以下假设：
  // 1.每次函数被调用，就传回a的值；
  // 2.如果A定义了一个拷贝构造函数，那么当函数被调用时，保证拷贝构造函数也会被调用。
  // 第一个假设的真实性，必须视A如何定义而定。
  // 第二个假设的真实性，虽然也有部分必须视A如何定义而定，
  // 但最主要还是视C++编译器所提供的进取性优化程度而定。
  // 一些高品质的C++编译器中，上述两点对于A的有用的定义都不正确。
}
A func2() {  // 明确的初始化操作
  A a0;
  A a1(a0);
  A a2 = a0;
  A a3 = A(a0);
  // 必要的转换有两个阶段：
  // 1.重写每一个定义，其中的初始化操作会被剥除；
  // 2.类的拷贝构造函数调用操作会被安插进去。
}
void func3(A a) {  // 参数的初始化
  // C++标准说，把一个对象当做参数传给一个函数，
  // 或作为一个函数的返回值，相当于以下形式的初始化操作：
  // A a = arg;

  // 临时对象先以A的拷贝构造函数正确初始化，然后再以bitwise方式拷贝到a0这个局部实体中。
  // 函数的声明因而也必须被转化，形式参数必须从原先的一个对象改变为一个引用：
  // void func3(A & a0);
}
void func4() {
  // 局部实体a以memberwise的方式将xx当做初值。
  // 编译器实现时，导入临时对象，并调用拷贝构造函数将它初始化，然后将临时对象交给函数。
  A a;
  func3(a);
  // 伪码：
  // XX __temp0;
  // __temp0.X::(xx);
  // func3(__temp0);
}
A func5() {  // 返回值的初始化
  A a;
  return a;
  // func5的返回值如何从局部对象a中拷贝过来？一个双阶段转化：
  // 1.首先加个额外参数，类型是对象的引用，这个参数将用来放置被拷贝构建而得的返回值；
  // 2.在return前插入拷贝构造函数的调用，以便将欲返回的对象内容当做新增参数的初值。
  // 伪码：
  //   void func5(X & __result) {
  //     X xx;
  //     xx.X::X();
  //     __result.X::X(xx);
  //     return;
  //   }

  // 调用语句将被转化为：
  //   X x = func5();
  //   X x;  // 不必执行默认构造函数
  //   func5(xx);

  // memfunc()可能被转化为：
  //   func5().memfunc();
  //   X __temp0;
  //   (func5(__temp0),__temp0).memfunc();

  //   X (*pf)();
  //   pf = func5;
  // 转化为：
  //   void (*pf)(X &);
  //   pf =func5
}
A func6() {  // 在编译器层面做优化
  A xx;
  return xx;
  // 所有return指令传回相同的具名数值，因此编译器有可能自己做优化，
  // 方法是以result参数取代named return value。
  //   X A(X & __result) {
  //     __result.X::X();  // 默认构造函数被调用
  //     return;
  //   }
  // 这样的编译器优化操作，有时被称为Named Return Value（NRV）优化。
  // NRV优化如今被视为是标准C++编译器的一个义不容辞的优化操作，
  // 虽然其需求其实超越了正式标准之外。
  // NRV优化提供了重要的效率改善，它还是饱受批评：
  // 1.优化由编译器默默完成，而它是否真的被完成，并不清楚，
  //   因为很少有编译器会说明其实现程度，或是否实现；
  // 2.一旦函数变得比较复杂，优化也就变的比较难以实施，
  //   在cfront中优化实施的前提是，当所有的named return指令句发生于函数的top
  //   level， 如果导入a nested local bolck with a return
  //   statement，cfront会将优化关闭，
  //   许多程序员主张对于这种情况应该以特殊化的构造函数策略取代之；
  // 3.某些程序员真的不喜欢应用程序被优化，以拷贝方式产生出对象时，对称地调用了析构函数。
  //   在此情况下，对称性被优化给打破了，程序虽然比较快，却是错误的。

  // 这样的需求在许多程序中可能会被征以严格的效率税：
  // X xx0(1024);  // 被单一的构造函数设定初值：xx0.X::X(1024);
  // X xx1 = X(1024);
  // X xx2 = (x)1024;
  // 三个初始化操作在语意上相等，但是2和3中，语法明显地提供了两个步骤的初始化操作：
  // 1.将一个临时性的对象设以初值1024；
  // 2.将临时性的对象以拷贝构造的方式作为显示对象的初值。
}

// 这个类的设计者应该提供一个显式的拷贝构造函数么？
// 默认的拷贝构造函数被视为无用的，它既没有任何对象成员或基类对象带有拷贝构造函数，
// 也没有任何虚函数或虚基类，会导致bitwise copy，三个坐标成员是以数值来储存的，
// bitwise copy既不会memory leak，也不会address aliasing，因此既快速又安全。
// 所以类的设计者不应该提供一个拷贝构造函数，因为编译器自动实施了最好的行为。
class Point3d1 {
 public:
  Point3d1(float x, float y, float z) {}

 private:
  float _x, _y, _z;
};

void func() {
  func1();
  func2();
  func3((A()));
  func4();
  func5();
  func6();
}

// 如果类需要大量的memberwise初始化操作，例如以传值的返回对象，
// 那么提供一个显式内联拷贝构造函数就非常合理，前提是编译器提供了NRV优化。
class Point3d2 {
 public:
  Point3d2() {}
  Point3d2(float x, float y, float z) {}
  Point3d2(const Point3d2 &rhs) {
    _x = rhs._x;
    _y = rhs._y;
    _z = rhs._z;
    // 上面的实现可以，但是memecpy()会更有效率：
    // memcpy(this, &rhs, sizeof(Point3d2));
    // memcpy和memset都只有在类不含任何有编译器产生的内部成员时才能有效运行。
    // 如果类声明一个或一个以上的虚函数，或内含一个虚基类，
    // 那么使用上述函数将会导致那些被编译器产生的内部成员的初值被改写。
  }

 private:
  float _x, _y, _z;
};

// 拷贝构造函数的应用，迫使编译器多多少少对程序代码做部分转化。
// 尤其是当一个函数以传值的方式传回一个对象，而类有一个拷贝构造函数（显式或隐式）。
// 这将导致深奥的程序转化，不论在函数的定义或使用上。
// 此外编译器也将拷贝构造函数的调用操作优化，以一个额外的第一参数取代NRV。

}  // namespace n3

namespace n4 {
// 2.4 成员们的初始化队伍

namespace test1 {
// 自定义构造函数，就有机会设置初值，可以使用成员初始化列表，也可以在构造函数之内。
// 为了让程序能够顺利编译，下列情况中必须使用成员初始化列表：
// 1.当初始化一个引用成员时；
// 2.当初始化一个常量成员时；
// 3.当调用一个基类的构造函数，而它拥有一组参数时；
// 4.当调用一个成员类的构造函数，而它拥有一组参数时。
class B {
 public:
  B(int i) : i_(i) { std::cout << i_ << std::endl; }

 private:
  int i_;
};
B rb(2);
class D1 : public B {
 public:
  // 编译器会一一操作初始化列表，以适当次序在构造函数内安插初始化操作，
  // 并且在任何显式的用户代码之前：
  D1(int i) : cb_(3), b_(4), rb_(rb), B(i) { std::cout << "D1" << std::endl; }

 private:
  B &rb_;
  const B cb_;
  B b_;
};
class D2 : public B {
 public:
  D2(int i) : B(i), b_(4), cb_(3), rb_(rb) { std::cout << "D2" << std::endl; }

 private:
  B &rb_;
  const B cb_;
  B b_;
};
void func() {
  // 2

  // 初始化列表中次序：先所以的基类，然后是所有的成员对象，无论是基类还是成员对象，
  // 当多于一个时，是由成员声明次序决定，不是由初始化列表中的排列次序决定：
  D1 d1(1);
  // 1
  // 3
  // 4
  // D1

  D2 d2(1);
  // 1
  // 3
  // 4
  // D2
}
}  // namespace test1

namespace test2 {
class A {
 public:
  A() { std::cout << "A()" << std::endl; }
  A(const A &a) { std::cout << "A(const A& a)" << std::endl; }
  A &operator=(const A &a) {
    std::cout << "A& operator=(const A& a)" << std::endl;
  }
  ~A() { std::cout << "~A()" << std::endl; }
};
class B1 {
 public:
  // 构造函数中初始化，先生成临时对象，再调用赋值运算符，然后再摧毁临时对象：
  B1(A &a) { a_ = a; }
  A a_;
};
void func1() {
  A a;  // A()
  std::cout << "==========" << std::endl;
  B1 b(a);
  // A()
  // A& operator=(const A& a)
  std::cout << "==========" << std::endl;
  // ~A()
  // ~A()
}
class B2 {
 public:
  // 初始化列表中初始化成员，直接调用拷贝构造函数：
  B2(A &a) : a_(a) {}
  A a_;
};
void func2() {
  A a;  // A()
  std::cout << "==========" << std::endl;
  B2 b(a);
  // A(const A& a)
  std::cout << "==========" << std::endl;
  // ~A()
  // ~A()
}
}  // namespace test2

namespace test3 {
class A1 {
 public:
  // i(j)其实比j(a)更早执行，j开始没有初值，所以导致i获得未定义的值：
  A1(int a) : j(a), i(j) {}
  void print() { std::cout << i << "," << j << std::endl; }

 private:
  int i;
  int j;
};
class A2 {
 public:
  // 应该这样写：
  A2(int a) : j(a) { i = j; }
  void print() { std::cout << i << "," << j << std::endl; }

 private:
  int i;
  int j;
};
void func() {
  A1 a1(5);
  a1.print();  // 21930,5

  A2 a2(5);
  a2.print();  // 5,5
}
}  // namespace test3

namespace test4 {
class A {
  int i;
  int j;
  int test(int v) { return 5; }

 public:
  // 成员函数的使用是合法的，因为和此对象相关的this指针已经被构建妥当：
  A(int val) : i(test(val)), j(val) {}
  // 建议将test(val)使用存在constructor体内的一个成员，
  // 而不要使用存在于初始化成员列表中的成员，来为另一个成员设定初值。
  // 并不知道test对对象的依赖性有多高，如果把test放在构造函数体内，
  // 那么对于到底是哪一个member在test执行时被设立初值这件事，
  // 就可以确保不会发生模棱两可的情况。
  void print() { std::cout << i << "," << j << std::endl; }
};

// 如果一个派生类成员函数被调用，其返回值被当做基类构造函数的一个参数，将会如何：
class D : public A {
  int val;

 public:
  int fval() { return val; }
  D(int i) : val(i), A(fval()) {  // 导致错误
    // 伪码：
    // A::A(this, this->fval());
    // val = i;
    // 先使用val调用基类构造函数，再对val进行初始化，顺序反了。
  }
};

void func() {
  A a(6);
  a.print();  // 5,6

  D d(4);
  d.print();  // 5,0
}
}  // namespace test4
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 10]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n1::test4::func();
      break;
    case 4:
      n2::test1::func();
      break;
    case 5:
      n2::test2::func();
      break;
    case 6:
      n4::test1::func();
      break;
    case 7:
      n4::test2::func1();
      break;
    case 8:
      n4::test2::func2();
      break;
    case 9:
      n4::test3::func();
      break;
    case 10:
      n4::test4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}