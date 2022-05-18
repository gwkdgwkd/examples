#include <iostream>

using namespace std;

namespace n1 {
// 2.1 Default Constructor的构建操作

// 默认构造函数在需要（是编译器需要，不是程序需要；程序需要，是程序员的责任）的时候被编译器产生出来。
// 如果没有任何用户定义构造函数，那么会有一个默认构造函数暗中被声明出来，但没啥作用。

// 有用的默认构造函数的四种情况：

namespace n11 {
// 1.带有Default constructor的对象成员
//   如果一个类没有任何构造函数，但它内含一个对象成员，
//   而后者有默认构造函数，那么这个类的隐式默认构造函数就是有用的。
//   编译器需要为这个类合成出一个默认构造函数，
//   不过这个合成操作只有在构造函数真正需要被调用时才会发生。
//   在C++各个不同的编译模块中，编译器如何避免合成出多个默认构造函数呢？
//   解决方法是把合成的默认构造函数、拷贝构造函数、析构函数、赋值操作符都以inline方式完成。
//   一个内联函数有静态链接，不会被模块以外看到。
//   如果函数太复杂，不适合做成内联，就会合成出一个explicit non-inline static实体。
class A {
 public:
  A() { std::cout << "A" << std::endl; }
};
class B {
 public:
  // 被合成的默认构造函数看起来像：inline B::B() { a.A::A(); }
  A a;  // 被合成的默认构造函数内会调用a的的默认构造函数，这是编译器的责任
  char *str;  // 被合成的默认构造函数内并不会初始化str，初始化str是程序员的责任
};
void func1() {
  B b;  // A
}

class C {
 public:
  C() {  // 定义了默认构造函数
    // 编译器会初始化对象成员a，由于默认构造函数已经被明确定义出来了，
    // 编译没办法合成第二个，所以编译器会扩展已存在的构造函数，
    // 在其中安插一些码，使得用户代码在被执行之前，先调用默认构造函数。
    // a.A::A();  // 安插的代码
    std::cout << "C" << std::endl;
    str = 0;  // 初始化了str，现在程序的需求满足了
  }
  A a;
  char *str;
};
void func2() {
  C c;
  // A
  // C
}

struct A1 {
  A1() { std::cout << "A1" << std::endl; }
};
struct A2 {
  A2() { std::cout << "A2" << std::endl; }
};
struct A3 {
  A3() { std::cout << "A3" << std::endl; }
};
class D {
 public:
  // 如果有多个对象成员都要求初始化操作，
  // C++要求以对象成员在class中的声明次序来调用各个构造函数。
  // 这一点由编译器完成，它为每一个构造函数安插代码，这些代码将被安插在explicit user code之前。
  A1 a1;
  A3 a3;
  A2 a2;
};
void func3() {
  D d;
  // A1
  // A3
  // A2
}

class E {
 public:
  E() { std::cout << "E" << std::endl; }
  A2 a2;
  A1 a1;
  A3 a3;
};
void func4() {
  E e;
  // A2
  // A1
  // A3
  // E
}

void func() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n11

namespace n12 {
// 2.带有Default constructor的Base Class
//   类似的道理，如果一个没有任何构造函数的类派生自一个“带有默认构造函数”的base class，
//   那么这个派生类的默认构造函数会是有用的，会被合成出来。
//   它将调用上一层基类的默认构造函数（根据它们的声明次序）。
//   对一个后继派生的类而言，这个合成的构造函数和一个被明确提供的默认构造函数没什么差异。
class BaseA {
 public:
  BaseA() { cout << "BaseA" << endl; }
};
class BaseB {
 public:
  BaseB() { cout << "BaseB" << endl; }
};
class Derived1 : public BaseB, BaseA {};
void func1() {
  Derived1 d;
  // BaseB
  // BaseA
}

// 如果设计这提供多个构造函数，但其中没有默认构造函数，编译器会扩张现有的每一个构造函数，
// 将用以调用所有必要的构造函数的代码加进去。
class Derived2 : public BaseB, BaseA {
 public:
  Derived2(int) { std::cout << "Derived2(int)" << std::endl; }
  Derived2() { std::cout << "Derived2()" << std::endl; }
};
void func2() {
  Derived2 d1;
  // BaseB
  // BaseA
  // Derived2()

  Derived2 d2(5);
  // BaseB
  // BaseA
  // Derived2(int)
}

// 如果同时亦存在着“带有默认构造函数”的对象成员，
// 那些默认构造函数也会被调用，在所有基类构造函数被调用之后。
class Derived3 : public BaseA, BaseB {
 public:
  n11::A1 a1;
  n11::A2 a2;
  n11::A3 a3;
};
void func3() {
  Derived3 d;
  // BaseA
  // BaseB
  // A1
  // A2
  // A3
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n12

namespace n13 {
// 3.带有一个Virtual Function的Class
//   由于缺乏由user声明的构造函数，编译器会详细记录合成一个默认构造的必要信息。
//   为了使多态生效，编译器必须为每一个基类(或其派生类)对象的vptr设定初值，放置适当的virtual table地址。
//   对于类所定义每一个构造函数，编译器会安插一些代码来做这样的事情。
//   对于那些未声明任何构造函数的类，编译器会为它们合成一个默认构造函数，以便正确地初始化每一个对象的vptr。
//   编译器发生：
//   1.一个vtbl被产生出来，内放class的虚函数地址。
//   2.每一个对象中，一个额外的指针成员vptr被编译器合成出来，内含相关的class vtbl的地址。
class A {
 public:
  A() { cout << "A" << endl; };
  virtual void flip() const = 0;
};
class B : public A {
 public:
  void flip() const { std::cout << "B::flip" << std::endl; }
};
class C : public A {
 public:
  void flip() const { std::cout << "C::flip" << std::endl; }
};
void flip(const A &a) {
  // A.flip()的虚拟引发操作会被重新改写，以使用a的vptr和vtbl中的flip条目：
  a.flip();
  // 转变为：(*a.vptrp[1])(&a)
  //  1表示flip()在virtual vable中的固定索引；
  //  &a代表要交给被调用的某个flip()函数实体的this指针。
}
void func() {
  B b;      // A
  C c;      // A
  flip(b);  // B::flip
  flip(c);  // C::flip
}
}  // namespace n13

namespace n14 {
// 4.带有一个Virtual Base Class的Class
//   由于缺乏由user声明的构造函数，编译器会详细记录合成一个默认构造的必要信息。
//   Virtual Base Class的实现在不同的编译器之间有极大的差异。
//   然而，每一种实现法的共通点在于必须使Virtual Base Class在其每一个派生类对象中的位置，
//   能够于执行期准备妥当。
class Base {
 public:
  Base() { cout << "Base" << endl; }
  int i;
};
class A : public virtual Base {
 public:
  int j;
};
class B : public virtual Base {
 public:
  double d;
};
class C : public A, public B {
 public:
  int k;
};
void foo(A *pa) { pa->i = 1024; }  // 无法在编译期决定出pa->Base::i的位置
// 编译器无法固定住foo中“经由pa而存取的Base::i”的实际偏移位置，因为pa的真正类型可以改变。
// 编译器必须改变“执行存取操作”的那些代码，使Base::i可以延迟至执行期才决定下来。
// 对于类所定义的每一个构造函数，
// 编译器会安插那些允许每一个virtual base class的执行期存取操作的代码。
// 如果class没有声明任何构造函数，编译器必须为它合成一个默认构造函数。
void func() {
  foo(new A);  // Base
  foo(new C);  // Base
}
}  // namespace n14

// 总结：
// 有四种情况，会导致编译器必须为未声明构造函数的类合成一个默认构造函数。
// C++标准把那些合成物称为隐式有用的默认构造函数。
// 被合成出来的构造函数只能满足编译器（而非程序）的需要。
// 它之所以能够完成任务，
// 是借着调用对象成员或基类的默认构造函数或是为每一个对象初始化其虚函数机制或虚基类机制而完成。
// 至于没有存在那四种情况而又没有声明任何构造函数的类型，
// 它们拥有的是隐式无用默认构造函数，它们实际上并不会被合成出来。
// 在合成的默认构造函数中，只有基类subobjects和对象成员会被初始化。
// 所有其他的非静态数据成员，如整数、整数指针、整数数组等等都不会被初始化。
// 这些初始化操作对程序而言或许有需要，但对编译器则并非必要。
// 如果程序需要一个把某指针设为0的默认构造函数，那么提供它的人应该是程序员。
// C++新手一般有两个常见的误解（都不是真的）：
// 1.任何class如果没有定义默认构造函数，就会被合成出一个来。
// 2.编译器合成出来的默认构造函数会明确设定类内每一个数据成员的默认值。

void test() {
  n11::func();
  n12::func();
  n13::func();
  n14::func();
}

}  // namespace n1

namespace n2 {
// 2.2 Copy Constructor的构建操作

namespace n21 {
// 有三种情况，会以一个对象的内容作为另一个对象的初值：
// 1.最明显的一种情况当然就是对一个对象做明确的初始化操作；
// 2.当对象作为参数交给某个函数时；
// 3.以及当函数传回一个对象时。
class A {
 public:
  A() {}
  A(const A &x) { cout << "A copy constructor" << endl; }
  // A(const X &x, int = 0);  // 可以是多参数形式，从第二个参数起，提供默认值。
  // 大部分情况下，当一个对象以另一个同类对象作为初值时，会导致拷贝构造函数被调用。
  // 这可能会导致一个临时对象的产生或程序代码的蜕变（或者两者都有）。
};
void foo1(A x) {}
A foo2() {
  A A;
  return A;
}
void func() {
  A a = foo2();
  foo1(a);

  // g++ -fno-elide-constructors constructors.cp
  // A copy constructor
  // A copy constructor
  // A copy constructor

  // g++ constructors.cpp
  // A copy constructor
}
}  // namespace n21

// 如果类没有一个一个显式拷贝构造函数会怎么样？
// 当对象以同类对象作为初值时，其内部是以所谓的default memberwise initialization手法完成的。
// 也就是把一个内建的或派生的数据成员的值，从某个对象拷贝一份到另一个对象。
// 并不拷贝对象成员，而是以递归的方式实施memberwise initialization。
// 如果一个类未定义拷贝构造函数，编译器就为它自动产生出一个，这句话不对，
// 而是默认构造函数和拷贝构造函数在必要的时候才由编译器产生出来。
// 一个对象可以从两种方式复制得到，一种是被初始化（拷贝构造函数），另一种是被赋值（赋值操作符）。
// 与默认构造函数一样，如果没有声明一个拷贝构造函数，就会有隐含的声明或隐含的定义出现。
// C++标准把拷贝构造函数区分为有用和无用两种。
// 只有有用的实体才会被合成于程序之中。
// 决定拷贝构造函数是否有用的标准在于类是否展现出所谓的bitwise copy semantics。

// 什么时候一个类不展现出bitwise copy semantics呢（需要生成拷贝构造函数）？有四种情况：
// 1.当类内含一个对象成员而后者的类声明有一个拷贝构造函数时（不论是被明确声明，还是被编译器合成）；
// 2.当类继承自一个基类而后者存在有一个拷贝构造函数时（不论是被明确声明或是被合成而得）；
// 3.当类声明了一个或多个虚函数时；
// 4.当类派生自一个继承串链，其中有一个或多个虚基类时。
// 前2种情况中，编译器必须将成员或基类的拷贝构造函数调用操作安插到被合成的拷贝构造函数中。
// 对于3，需要重新设定virtual Table的指针，
//  如果编译器对于每一个新产生的对象的vptr不能成功而正确地设好其初值，将导致可怕的后果。
//  因此，当编译器导入一个vptr到class中时，该class就不再展现bitwise semantics了。
//  编译器需要合成出一个拷贝构造函数，以求将vptr适当地初始化。
// 对于4，需要处理Virtual Base Class Subobject，
//  一个对象如果以另一个object作为初值，而后者有一个Virtual Base Class Subobject，
//  那么bitwise copy semantics失效。
//  每一个编译器对于虚拟继承的支持承诺，
//  都表示必须让派生对象中的Virtual Base Class Subobject位置在执行期就准备妥当。
//  维护位置的完整性是编译器的责任。
//  bitwise copy semantics可能会破坏这个位置，所以编译器必须在它自己合成出来的拷贝构造中做出仲裁。

namespace n22 {
class A {
 public:
  A() {}
  A(const char *) {}
  A(const A &) { std::cout << "A copy" << std::endl; }
  ~A() {}
};
class B {
 public:
  B(const A &) {}
  ~B() {}
  // 在这种情况下，编译器必须合成出一个拷贝构造函数以便调用A成员对象的拷贝构造函数：
  // inline B(cosnt B &b) {
  //   str.String::String(b.str);
  //   cnt = b.cnt;
  // }
  // 被合成出来的拷贝构造函数中，如整数、指针、数组等等的非类对象成员也都会被复制，正如期待的一样。

 private:
  int cnt;
  A a;
};
void func() {
  B b1((A()));
  B b2(b1);  // A copy
}
}  // namespace n22

namespace n23 {
class B : public n22::A {};
void func() {
  B b1;
  B b2(b1);  // A copy
}
}  // namespace n23

void test() {
  n21::func();
  n22::func();
  n23::func();
}
}  // namespace n2

namespace n3 {
// 2.3 程序转化语意学

class A {};
A func1() {
  A a;
  return a;
  // 做出一下假设：
  // 1.每次函数被调用，就传回a的值；
  // 2.如果A定义了一个拷贝构造函数，那么当函数被调用时，保证拷贝构造函数也会被调用。
  // 第一个假设的真实性，必须视A如何定义而定。
  // 第二个假设的真实性，虽然也有部分必须视A如何定义而定，
  // 但最主要还是视你的C++编译器所提供的进取性优化程度而定。
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

void func3(A a) {
  // 参数的初始化
  // C++标准说，把一个对象当做参数传给一个函数（或作为一个函数的返回值），相当于以下形式的初始化操作：
  // A a = arg;

  // 临时对象先以A的拷贝构造函数正确地设定了初值，然后再以bitwise方式拷贝到x0这个局部实体中。
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
  A xx;
  return xx;
  // func5的返回值如何从局部对象xx中拷贝过来？一个双阶段转化：
  // 1.首先加上一个额外参数，类型是对象的一个引用。
  //   这个参数将用来放置被“拷贝构建”而得的返回值。
  // 2.在return指令之前安插一个拷贝构造函数调用操作，
  //   以便将欲返回的对象内容当做上述新增参数的初值。
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

// 在编译器层面做优化
A func6() {
  A xx;
  return xx;
  // 所有return指令传回相同的具名数值，因此编译器有可能自己做优化，
  // 方法是以result参数取代named return value。
  //   X A(X & __result) {
  //     __result.X::X();  // 默认构造函数被调用
  //     return;
  //   }
  // 这样的编译器优化操作，有时被称为Named Return Value（NRV）优化。
  // NRV优化如今被视为是标准C++编译器的一个义不容辞的优化操作，虽然其需求其实超越了正式标准之外。
  // NRV优化提供了重要的效率改善，它还是饱受批评：
  // 1.优化由编译器默默完成，而它是否真的被完成，并不清楚。
  //   因为很少有编译器会说明其实现程度，或是否实现。
  // 2.一旦函数变得比较复杂，优化也就变的比较难以实施。
  //   在cfront中，只有当所有的named return指令句发生于函数的top level时，优化才施行。
  //   如果导入“a nested local bolck with a return statement”，cfront就会静静地将优化关闭。
  //   许多程序员主张对于这种情况应该以“特殊化的构造函数”策略取代之。
  // 3.某些程序员真的不喜欢应用程序被优化，以拷贝方式产生出一个对象时，对称地调用了析构函数。
  //   在此情况下，对称性被优化给打破了。程序虽然比较快，却是错误的。

  // 这样的需求在许多程序中可能会被征以严格的效率税：
  // X xx0(1024);  // 被单一的构造函数设定初值：xx0.X::X(1024);
  // X xx1 = X(1024);
  // X xx2 = (x)1024;
  // 三个初始化操作在语意上相等，但是第二行和第三行中，语法明显地提供了两个步骤的初始化操作：
  // 1.将一个临时性的对象设以初值1024；
  // 2.将临时性的对象以拷贝构造的方式作为显示对象的初值。
}

// Copy Constructor：要还是不要？
class Point3d1 {
 public:
  Point3d1(float x, float y, float z) {}

 private:
  float _x, _y, _z;
};
// 这个类的设计者应该提供一个显式的拷贝构造函数么？
// 默认的拷贝构造函数被视为无用的。
// 它既没有任何对象成员或基类对象带有拷贝构造函数，也没有任何虚函数或虚基类，会导致bitwise copy。
// 三个坐标成员是以数值来储存的，bitwise copy既不会导致memory leak，
// 也不会产生address aliasing，因此既快速又安全。
// 所以类的设计者不应该提供一个拷贝构造函数，因为编译器自动实施了最好的行为。
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
    // 上面的实现可以，但是memecpy()会更有效率
    // memcpy(this, &rhs, sizeof(Point3d2));
    // memcpy和memset都只有在类不含任何有编译器产生的内部成员时才能有效运行。
    // 如果类声明一个或一个以上的虚函数，或内含一个虚基类，
    // 那么使用上述函数将会导致那些被编译器产生的内部成员的初值被改写。
  }

 private:
  float _x, _y, _z;
};
// 拷贝构造函数的应用，迫使编译器多多少少对你的程序代码做部分转化。
// 尤其是当一个函数以传值的方式传回一个对象，而类有一个拷贝构造函数（显式或隐式）。
// 这将导致深奥的程序转化，不论在函数的定义或使用上。
// 此外编译器也将拷贝构造函数的调用操作优化，以一个额外的第一参数取代NRV。

void test() {
  func1();
  func2();
  func3((A()));
  func4();
  func5();
  func6();
}
}  // namespace n3

namespace n4 {
// 2.4 成员们的初始化队伍

// 写一个构造函数，就有机会设定数据成员的初值。
// 可以使用成员初始化列表，也可以在构造函数本身之内。
// 为了让你的程序能够顺利编译，下列情况中必须使用成员初始化列表：
// 1.当初始化一个引用成员时；
// 2.当初始化一个常量成员时；
// 3.当调用一个基类的构造函数，而它拥有一组参数时；
// 4.当调用一个成员类的构造函数，而它拥有一组参数时。

// 构造函数中初始化，会产生临时对象，再以一个赋值运算符，然后再摧毁那个临时对象。
// 编译器会一一操作初始化列表，以适当次序在构造函数内安插初始化操作，并且在任何显式的用户代码之前。
// 初始化列表中次序是由类中的成员声明次序决定，不是由初始化列表中的排列次序决定。

class Y {
  int i;
  int j;

 public:
  Y(int val) : j(val), i(j) {}
  // i(j)其实比j(val)更早执行。
  // j一开始没有初值，所以会导致i获得未定义的值，建议这样写：
  // Y(int val) : j(val) { i = j; }
};

// 是否可以调用一个成员函数来设定一个成员的初值？
class Z {
  int i;
  int j;
  int xfoo(int v) {}

 public:
  // 成员函数的使用是合法的，因为和此对象相关的this指针已经被构建妥当：
  Z(int val) : i(xfoo(val)), j(val) {}
  // 建议使用存在于constructor体内的一个成员，
  // 而不要使用存在于初始化成员列表中的成员，来为另一个成员设定初值。
  // 并不知道xfoo对对象的依赖性有多高，如果把xfoo()放在构造函数体内，
  // 那么对于到底是哪一个member在xfoo()执行时被设立初值这件事，就可以确保不会发生模棱两可的情况。
};

// 如果一个派生类成员函数被调用，其返回值被当做基类构造函数的一个参数，将会如何：
class FooBar : public Z {
  int _fval;

 public:
  int fval() { return _fval; }
  FooBar(int val) : _fval(val), Z(fval()) {  // 不是一个好主意
    // 伪码
    // Z::Z(this, this->fval());
    // _fval = val;
  }
};

void test() {}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    case 2:
      n3::test();
      break;
    case 3:
      n4::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}