#include <iostream>

// 构造、析构、拷贝语义学

namespace n1 {
namespace test1 {
class AB {
 public:
  // 虽然被设计为一个抽象的基类，但它仍然需要一个明确的构造函数以初始化p：
  AB(char* v = 0) : p(v) {}
  virtual ~AB() = 0;
  virtual void func1() const = 0;  // 纯虚函数，类不可能拥有实体
  virtual const char* func2() const { return p; }

 private:
  // 一般而言，类的数据成员应该被初始化，并且只在构造函数或在类的其他成员函数中初始化。
  // 其他任何操作都将破坏封装性，不易维护。
  // 不应该在抽象的基类中声明数据成员，这是强而有力的论点，把接口和实现分类。
  // 但这并不是一直有理，因为将被共享的数据抽取出来放在基类中，是一种正当的设计。
  char* p;
};
class D : public AB {
 public:
  D() {}
  void func1() const;
};
// 竟然可以定义和调用一个纯虚函数：
inline void AB::func1() const { std::cout << "AB::func1()" << std::endl; }
inline void D::func1() const {
  // 不过它只能被静态地调用（类名::这样么？），不能经由虚拟机制调用：
  AB::func1();
}

// 纯虚析构函数一定得定义它，因为每一个派生类析构函数会被编译器扩展，
// 以静态调用的方式调用其每一个虚基类以及上一层基类的析构函数。
// 因此，只要缺乏任何一个基类析构函数的定义，就会导致链接失败。
AB::~AB() { std::cout << "AB::~AB()" << std::endl; }
// 编译器不能阻止纯虚析构函数的调用，因为C++语言的一个保证前提：
// 继承体系中每一个对象的析构函数都会被调用。
// 编译器没有足够的能力来替程序员定义一个纯虚析构函数：
// 1.要么要给出纯虚析构函数的定义；
// 2.要不使用比较好的替代方案，不要把虚析构函数声明为纯虚的。

void func() {
  // 如果抽象基类没有构造函数，其派生类的局部对象p将无法决定初值：
  D* p = new D;
  p->func1();  // AB::func1()
  delete p;    // AB::~AB()
}
}  // namespace test1

namespace test2 {
// test1::AB的函数func2并不与类型有关，因而几乎不会被后继的派生类改写。

// 一般而言，把所有的成员函数都声明为虚函数，
// 然后在靠编译器的优化操作把非必要的virtual去除，并不是好的设计观念。

// 决定一个虚函数是否需要const不那么容易：
// 1.不把函数声明为const，意味着该函数不能够获得一个const引用或const指针；
// 2.声明一个函数为const，然后发现派生类又必须修改某一个数据成员。
// 基于上面两点，可以考虑不再用const了。

class AB {
 public:
  virtual ~AB() {}                                 // 不再是pure
  virtual void func1() = 0;                        // 不再是const
  virtual const char* func2() const { return p; }  // 不再是virtual

 protected:
  AB(char* v = 0) : p(v) {}
  char* p;
};
}  // namespace test2
}  // namespace n1

namespace n2 {
// 5.1 无继承情况下的对象构造

namespace test1 {
// C++标准说下面是一种所谓的Plain Ol' Data声明形式：
typedef struct {
  float x, y, z;
} Point;
// 用C++编译上面的代码，编译器会声明：
// 1.一个无用的默认构造函数；
// 2.一个无用的析构函数；
// 3.一个无用的拷贝构造函数；
// 4.以及一个无用的拷贝赋值操作符。
// 但实际上编译器会分析这个声明，并为它提上Plain Ol' Data标签。
// 当编译器遇到下面的定义，无用的默认构造函数和析构函数都会被调用。
// 构造在程序开始处被调用而析构是在main结束前的exit处被调用，exit由系统产生。
// 实际上那些无用成员，要不是没被定义，就是没被调用，程序的行为与C中表现的一样。
Point global;  // 生命期和整个程序的相同
// 在C中，global被视为一个临时性的定义，因为它没有明确的初始化操作。
// 一个临时性的定义可以在程序中发生多次，那些实例会别链接器折叠起来，只留单独一个实体，
// 被放在程序data segment中一个特别保留给未初始化的全局对象使用的空间，
// 这块空间被称为BSS（Block Started by Symbol）。
// C++并不支持临时性的定义，这是因为类构造行为的隐含应用之故。
// 虽然可以判断一个对象是Plain Ol' Data，但似乎没有必要搞得那么复杂。
// 因此，global在C++中被视为完全定义（它会阻止第二个或更多个定义）。
// C++相比C的一个差异就在于，BSS data segment在C++中相对地不重要。
// C++的所有全局对象都被当做初始化过的数据来对待。
Point func() {
  Point local;  // 生命期到函数结束，既没有被构造也没有被析构

  Point* heap = new Point;  // 生命期从new到delete
  // new返回的对象并没有调用默认构造函数

  *heap = local;  // 如果local被适当的初始化化过，一切就没有问题
  // 观念上，无用的拷贝赋值操作符会被调用，但由于是Plain Ol' Data，
  // 所以赋值操作将只是想C那样的纯粹位搬移操作。

  // 观念上，会调用析构函数，但析构函数要不是没有被产生就是没有被调用：
  delete heap;

  // 观念上，会调用拷贝构造函数，不过实际上只是简单的位拷贝操作，因为是POD：
  return local;
}
}  // namespace test1

namespace test2 {
// 经过封装的Point class，其大小并没有改变，还是连续三个float。
// 不论private、public，或是成员函数声明，都不会占用额外的对象空间。
class Point {
 public:
  Point(float x = 0.0, float y = 0.0, float z = 0.0) : _x(x), _y(y), _z(z) {}
  // 并没有定义拷贝构造函数或赋值操作符，因为默认的位语义已经足够。
  // 也不需要提供析构函数，默认的内存管理方法也已足够。

 private:
  float _x, _y, _z;
};
// global被定义在全局作用域，初始化将延迟到程序激活(startup)才开始：
Point global;  // 调用Point(0.0,0.0,0.0);

// 如果要对类中的所有成员都设置常量初值，比起在构造函数体内设置而言
// 给予一个显示的初始化列表会比较高效，甚至在局部空间内，也是如此。
void func() {
  Point l1 = {1.0, 1.0, 1.0};  // 比下面的方式快
  // 因为activation record被放进程序堆栈时，
  // 初始化列表中的常量就可以被直接放进l1内存中了。
  Point l2;
  // l2._x = 1.0;
  // l2._y = 1.0;
  // l2._z = 1.0;
}
// Explicit initialization list带来三个缺点：
// 1.只有当类成员都是public时，此法才奏效；
// 2.只能指定常量，因为它们在编译期就可以被评估求值；
// 3.由于编译器并没有自动实施，所以初始化行为的失败可能性会比较高一些。
// 手工打造了一些巨大的数据结构如调色盘，或是要把一堆常量数据倾倒给程序，
// 那么Explicit initialization list的效率会比inline构造好的多，特别是对全局对象。
// 在编译器层面，会有一个优化机制来识别inline构造，
// 后缀简单地提供一个member-by-member的常量指定操作。
// 然后编译器会抽取那些值，并且对待它们就好像Explicit initialization list所供应的一样，
// 而不会把构造扩展成为一系列的赋值指令。

// 一般而言，如果代码中有需要许多函数都需要以传值方式传回一个局部对象，
// 那么提供一个拷贝构造函数就比较合理，甚至即使默认的按位拷贝语意已经足够。
// 因为它的出现会触发NRV优化，NRV优化后将不再需要调用拷贝构造函数，
// 因为运算结果已经被直接放置与被传回的对象体内了。
}  // namespace test2
}  // namespace n2

namespace n3 {
// 5.2 继承体系下的对象构造

// T Object;
// 如果T有一个构造函数（不论是自定义的还是有编译器合成的），它会被调用。
// 这很明显，不明显的是构造函数调用真正伴随了什么？
// 构造函数内有大量的隐藏代码，因为编译器会扩充每个构造函数，扩充程度视类的继承体系而定。
// 一般而言编译器所做的扩充大约如下：
// 1.记录在member initialization list中的数据成员初始化操作，
//   会被放进构造函数本身，并以成员的声明顺序为顺序；
// 2.如果一个成员没有出现在member initialization list中，
//   但它有一个默认构造函数，那么该默认构造函数必须被调用；
// 3.在那之前，如果对象有virtual table pointers，
//   它们必须被设置初值，指向适当的virtual tables；
// 4.在那之前，所有上一层的基类构造函数必须按照基类的声明顺序被调用，
//   与member initialization list中的书写顺序无关：
//   如果基类被列于member initialization list中，
//   那么任何明确指定的参数都应该传递过去。
//   如果基类没有被列于member initialization list中，
//   而它有默认构造函数（或默认memberwise拷贝构造函数），那么就调用之。
//   如果基类是多重继承下的第二或后继的基类，那么this指针必须有所跳转。
// 5.在那之前，所有虚基类构造函数必须被调用，从左到右，从最深到最浅：
//   如果基类被列于member initialization list中，那么明确指定的参数都应该传递过去。
//   若没有列于list中，而class有一个默认构造函数，也应应该调用之。
//   此外，运行时必须能获取类中每个virtual base class subobject的offset。
//   如果对象是最底层的类，其构造函数可能被调用，某些用以支持这个行为的机制必须被放进来。

class Point {
 public:
  Point(float x = 0.0, float y = 0.0) : _x(x), _y(y) {
    std::cout << "Point C" << std::endl;
    size();
  }
  Point(const Point&) {}
  Point& operator=(const Point&) {}
  virtual ~Point() {}
  virtual float z() { return 0.0; }
  virtual void size() { std::cout << "Point::size" << std::endl; }

 private:
  float _x, _y;
};
class Line {
  Point _begin, _end;

 public:
  Line(float = 0.0, float = 0.0, float = 0.0, float = 0.0) {}
  Line(const Point& end, const Point& begin) : _end(end), _begin(begin) {}
  // 会被扩充为：
  // Line(Line* this, const Point& end, const Point& begin) {
  //   this->_begin.Point::Point(begin);
  //   this->_end.Point::Point(end);
  //   return this;
  // }

  // 每一个显示构造函数都会被扩充以调用其两个成员对象的构造函数：
  void draw();

  // 由于Point声明了一个拷贝构造函数、一个赋值操作符，以及一个析构函数，
  // 所以Line的拷贝构造函数、赋值操作符，以及析构函数都将有实际功能。
};
void func1() {
  // Line a;
  // Line隐式析构函数会被合成出来，在其中，它的成员对象的析构函数会被调用。
  // 如果Line派生自Point，那么合成出来的析构函数将会是virtual。
  // 由于Line只是内带Point对象而非继承，所以被合成出来的析构函数是nontrivial。
  // 如果Point析构函数是inline函数，那么每一个调用操作会在调用地点扩展开来。
  // 虽然Point析构函数是virtual，但其调用操作会被静态地决议出来。
  // 隐式拷贝构造函数被合成出来，成为一个inline public member：
  // Line b = a;
  // a = b;
  // 在一个有程序员供应的赋值操作符中忘记检查自我赋值操作，是新手极易陷入的一项错误。
}

// 虚拟继承
class Point3d : public virtual Point {
 public:
  Point3d(float x = 0.0, float y = 0.0, float z = 0.0) : Point(x, y), _z(z) {
    std::cout << "Point3d C" << std::endl;
    size();
  }
  // 传统的构造函数扩充现象并没有用，这是因为虚基类的共享性之故：
  // Point3d* Point3d::Point3d(Point3d* this, bool __most_derived,
  //                           float x, float y, float z) {
  //   if(__most_derived != false)  // 防止基类构造函数被调用多次
  //     this->Point::Point(x, y);
  //   this->__vptr_Point3d = __vptr_Point3d;
  //   this->__vptr_Point3d__Point = __vptr_Point3d__Point;
  //   this->_z = rhs._z;
  //   return this;
  // }
  // 更深层的继承下，调用Point3d时，__most_derived总是设为false。
  Point3d(const Point3d& rhs) : Point(rhs), _z(rhs._z) {}
  ~Point3d() {}
  Point3d& operator=(const Point3d&) {}
  virtual float z() { return _z; }
  virtual void size() { std::cout << "Point3d::size" << std::endl; }

 protected:
  float _z;
};

// 按传统的策略（没有判断__most_derived）扩展构造函数时：
class Vertex : virtual public Point {
 public:
  Vertex() {
    std::cout << "Vertex C" << std::endl;
    size();
  }
  virtual void size() { std::cout << "Vertex::size" << std::endl; }
};
class Vertex3d : public Point3d, public Vertex {
 public:
  Vertex3d() {
    std::cout << "Vertex3d C" << std::endl;
    size();
  }
  virtual void size() { std::cout << "Vertex3d::size" << std::endl; }
};
class PVertex : public Vertex3d {
 public:
  PVertex() {
    std::cout << "PVertex C" << std::endl;
    size();
  }
  virtual void size() { std::cout << "PVertex::size" << std::endl; }
};
// Vertex的构造函数必须也调用Point的构造函数。
// 然而，当Point3d和Vertex同为Vertex3d的subobject时，
// 它们对Point构造函数的调用一定不可以发生，取而代之的是，
// 作为一个最底层的类，Vertex3d来负责完成被共享的Point subobject的构造。

void func2() {
  PVertex p;
  // Point C
  // Point::size
  // Point3d C
  // Point3d::size
  // Vertex C
  // Vertex::size
  // Vertex3d C
  // Vertex3d::size
  // PVertex C
  // PVertex::size

  // vptr初始化语意学
  // 当定义一个Pvertex object时，构造函数的调用顺序是：
  // Point(x,y),Point3d(x,y,z),Vertex(x,y,z),Vertex3d(x,y,z),PVertex(x,y,z)
  // 在类的构造函数中，经由构造中的对象来调用一个虚函数，其函数实体应该是在此类中作用的那个。
  // 构造函数的调用顺序是：
  // 由根源到末端，由内而外，当基类构造函数执行时，派生类实体还没有被构造出来。
  // 如果虚函数调用操作限制在构造和析构中直接调用，那么将每一个调用操作以静态方式决定，
  // 千万不要用到虚拟机制，其他情况，使用虚拟机制。
  // 上面的限制操作，需要通过vptr来实现。

  // 只有当一个完整的对象被定义出来时，虚基类构造函数才会被调用；
  // 如果对象只是某个完整对象的subobject它就不会被调用。
  // 某些编译器把每一个构造函数分裂为两个，一个针对完整的对象，另一个针对subobject。
  // 完整版本无条件地调用虚基类构造函数，设定所有vptr。
  // subobject版则不调用虚基类构造函数，也可能不设定vptrs等等。
}

// vptr在构造函数中应该在何时被初始化，有三种选择：
// 1.在任何操作之前；
// 2.在基类构造函数调用操作之后，在程序员定义的代码，
//   或是member initialization list中所列的members初始化操作之前；
// 3.在每一件事情发生之后。
// 答案是2，另外两种没有什么价值。
// 令每一个基类构造函数设定其对象的vptr，使它指向相关的virtual table之后，
// 构造中的对象就可以严格而正确地变成构造过程中所幻化出来的每一个类的对象。
// 也就是说，PVertex对象会先形成一个Point对象、Point3d对象、
// 一个Vertex对象、一个Vertex3d对象，然后才成为一个PVertex对象。
// 在每一个基类构造函数中，对象可以与构造函数的类的完整对象做比较。
// 构造函数执行算法通常如下：
// 1.在派生类构造函数中，所有虚基类及上一层基类的构造函数会被调用。
// 2.上述完成之后，对象的vptrs被初始化，执行相关的virtual tables。
//   vptr必须被设定的两种情况：当一个完整的对象被构造起来时。
//   如果声明一个Point对象，Point构造函数必须设定其vptr。
//   当一个subobject构造调用了一个虚函数时，不论是直接调用或间接调用。
// 3.如果有member initialization list的话，将在构造体内扩展开来。
//   这必须在vptr被设置之后才进行，以免一个虚函数被调用。
// 4.最后，执行程序员所提供的代码。
// 上面方法完美地解决了所说的有关限制虚拟机制的问题。

// Point(float x, float y) : _x(x), _y(y) {}
// Point3d(float x, float y, float z) : Point(x, y), _z(z) {}
// Vertex和Vertex3d构造函数也有类似的定义。
// 声明一个PVertex对象，由于对基构造函数重新定义了，
// 其vptr将不再需要在每一个基类构造函数中被设定。
// 解决方法是把构造函数分裂为一个完整的对象实体和一个subobject实体。
// 在subobject实体中，vptr的设定可以省略（如果可能的话）。

class Base {
 public:
  Base() { std::cout << "Base" << std::endl; }
  Base(int i) : b(test(i)) { std::cout << "Base i" << std::endl; }
  virtual int test(int i) { std::cout << "Derived::test" << std::endl; }

 private:
  int b;
};
class Derived : public Base {
 public:
  // 在类的构造函数的member initialization list中调用该类的虚函数，安全吗？
  // 就实际而言，将该函数运行于基类数据成员的初始化中，总是安全的。
  // 这是因为，vptr保证能够在member initialization list被扩展之前，
  // 由编译器正确设置，但是在语意上这可能是不完全的，
  // 因为函数本身可能还得依赖未被设置初值的成员，所以并不推荐这种做法。
  Derived(int i) : a(test(i)) { std::cout << "Derived i" << std::endl; }

  // 何时需要供应参数给一个基类构造函数？
  // 此时在类构造函数的member initialization list中调用该类的虚拟函数，仍然安全么？
  // 不，此时vptr尚未设定好，就是被设置指向错误的类。
  // 更进一步，该函数所存取的任何类的数据成员一定还没被初始化。
  Derived(int i, int j) : Base(test(i)), a(j) {
    std::cout << "Derived i j" << std::endl;
  }

  virtual int test(int i) { std::cout << "Derived::test" << std::endl; }

 private:
  int a;
};
void func3() {
  Derived d1(5);
  // Base
  // Derived::test
  // Derived i

  Derived d2(1, 2);
  // Derived::test
  // Derived::test
  // Base i
  // Derived i j
}
}  // namespace n3

namespace n4 {
// 5.3 对象复制语意学

// 当设计一个class，并赋值时，可以有三种选择：
// 1.什么都不做，因此得以实施默认行为；
// 2.提供一个explicit copy assignment operator；
// 3.明确地拒绝一个对象指定个另一个对象，只要将赋值操作符声明为私有，并不提供定义即可，
//   声明为private，除了成员函数以及类的友元不允许任何地点进行赋值操作，
//   不提供函数定义，因为一旦某个成员函数或友元企图执行拷贝，链接就会失败。

namespace test1 {
// 如果只要支持简单的拷贝操作，那么默认的行为不但足够而且有效率，
// 没有理由再自己提供一个赋值操作符。
// 只有在默认行为所导致的语意不安全或不正确时，才需要设计一个赋值操作符。
class Point {
 public:
  Point(float x = 0.0, float y = 0.0) {}

 protected:
  float _x, _y;
  // 坐标内带数值，所以不会发生别名化或内存泄露，默认的赋值操作符是安全的。
  // 依赖默认的memberwise copy，编译器会产生出一个实体吗？
  // 与拷贝构造函数一样，不会。
  // 已经有了bitwise copy语意，所以implicit赋值操作符毫无用处，不会被合成出来。
};
void func() {
  Point a3, b3;
  a3 = b3;
  // 并没有赋值操作符被调用，从语意或从效率上考虑，这都是所需要的。
  // 可能还是会实现拷贝构造函数，为的是把NRV优化打开。
  // 拷贝构造函数的出现不应该认为一定要提供赋值操作符。
}

// 一个类对于默认的赋值操作符，在以下情况不会表现出bitwise copy语意：
// 1.当类内带一个成员对象，而且对象有一个赋值操作符时；
// 2.当一个类的基类有一个赋值操作符时；
// 3.当一个类声明了任何虚函数（不能直接拷贝vptr地址）时；
// 4.当类继承自一个虚基类（不论此base class有没有赋值操作符）时。
// C++标准上说，赋值操作符并不表示bitwise copy是有用的，
// 实际上，只有有用的赋值操作符才会被合成出来。
}  // namespace test1

namespace test2 {
class Point {
 public:
  Point(float x = 0.0, float y = 0.0) {}
  Point& operator=(const Point& p) {
    std::cout << "Point& operator=" << std::endl;
    _x = p._x;
    _y = p._y;
    return *this;
  }

 protected:
  float _x, _y;
};
class Point3d : virtual public Point {
 public:
  Point3d(float x = 0.0, float y = 0.0, float z = 0.0) {}
  // 如果没有定义赋值操作符，编译器就合成一个，类似：
  // inline Point3d& operator=(Point3d* const this,
  //                            const Point3d& p) {
  //   this->Point::operator=(p);
  //   _z = p._z;
  //   return *this;
  // }
  // 赋值操作符没有member assignment list，不能这样写：
  // inline Point3d& operator=(const Point3d& p) :
  //                            Point(p), z(p._z) {}
  // 调用基类赋值操作符的两种方式：
  // Point::operator=(p);
  // (*(Point*)this) = p;
  // 缺少copy assignment list，看来或许只是一件小事，但如果没有它，
  // 编译器一般而言就没有办法阻止上一层基类的赋值操作符被调用。

 protected:
  float _z;
};
class Vertex : virtual public Point {
 public:
  Vertex& operator=(const Vertex& v) {
    this->Point::operator=(v);
    _next = v._next;
    std::cout << "Vertex& operator=" << std::endl;
    return *this;
  }

 protected:
  char* _next;
};
class Vertex3d : public Point3d, public Vertex {
 public:
  Vertex3d& operator=(const Vertex3d& v) {
    this->Point::operator=(v);
    this->Point3d::operator=(v);
    this->Vertex::operator=(v);
    std::cout << "Vertex3d& operator=" << std::endl;
  }
  // 如何能够在Point3d和Vertex的赋值操作符中阻止Point的赋值操作符呢？
  // 编译器不能够重复传统的构造函数解决方法：附加上额外的参数。
  // 这是因为，和构造函数以及析构函数不同的是，取赋值操作符地址的操作是合法的。
  // 下面的代码是毫无瑕疵的合法程序代码：
  // typedef Point3d& (Point3d::*pmfPoint3d)(const Point3d&);
  // pmfPoint3d pmf = &Point3d::operator=;
  // (x.*pmf)(x);
  // 然而无法支持它，仍需根据其独特的继承体系，安插任何可能数目的参数给赋值操作符。
  // 另一个方法是，编译器可能为赋值操作符产生分化函数，
  // 以支持这个类成为most_derived class或成为中间的基类。
  // 如果赋值操作符别编译器产生的话，那么split function解决方案可说是定义明确。
  // 但如果它是被类设计者完成的，那么就不算是定义明确。
};

void func() {
  Vertex3d v1, v2;
  v1 = v2;
  // Point& operator=
  // Point& operator=
  // Point& operator=
  // Vertex& operator=
  // Vertex3d& operator=
}
// 事实上，赋值操作符在虚拟继承情况下行为不佳，需要小心的设计和说明。
// 建议尽可能不要允许一个虚基类的拷贝操作，甚至不要在任何虚基类中声明数据。
}  // namespace test2
}  // namespace n4

namespace n5 {
// 5.4 对象的功能
}

namespace n6 {
// 5.5 析构语意学

// 如果类没有定义析构函数，那么只有在类内带成员对象或基类拥有析构函数的情况下，
// 编译器才会自动合成出一个来，否则，析构函数会别视为不需要，
// 也就不需要被合成，当然更不需要被调用。
// 应该因为需要而非感觉来提供析构函数，更不要因为不确定是否需要就提供析构函数。

// 一个自定义的析构函数被扩展的方式类似构造函数别扩展的方式，但顺序相反：
// 1.如果对象内带一个vptr，那么首先重设相关的virtual table；
// 2.析构函数本身现在被执行，也就是说vptr会在程序员的执行代码前被重设；
// 3.如果类拥有有析构函数的成员对象，那么它们会以其声明顺序的相反顺序被调用；
// 4.如果有任何直接的（上一层）非虚基类拥有析构函数，它们会以其声明顺序的相反顺序被调用；
// 5.如果有任何虚基类拥有析构函数，而当前这个类是最尾端（most-derived)的类，
//   那么它们会以其原来的构造顺序的相反顺序被调用。

// 就像构造函数一样，目前对于析构函数的一种最佳实现策略就是维护两份析构函数实体：
// 1.一个complete object实体，总是设定好vptrs，
//   并调用virtual base class destructors；
// 2.一个base class subobject实体，除非在析构函数中调用虚函数，
//   否则它绝不会调用虚基类析构函数并设定vptr。

// 一个对象的生命结束于其析构函数开始执行时。
// 由于每一个基类析构函数都轮番被调用，所以派生类对象实际上变成了一个不完整的对象。
// PVertex对象归还其内存之前，会依次变成vertex3d对象、Vertex对象、Point3d对象，
// 对象的蜕变会因为vptr的重新设置而受到影响，vptr的重新设置在每一个析构函数中，
// 在程序员提供的代码执行之前。
}  // namespace n6

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int tD1pe = atoi(argv[1]);
  switch (tD1pe) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n3::func2();
      break;
    case 2:
      n3::func3();
      break;
    case 3:
      n4::test2::func();
      break;
    default:
      std::cout << "invalid tD1pe" << std::endl;
      break;
  }

  return 0;
}