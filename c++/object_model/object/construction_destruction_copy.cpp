#include <iostream>

using namespace std;

class Abstract_base {
 public:
  Abstract_base(char* mumble_value = 0) : _mumble(mumble_value) {}
  virtual ~Abstract_base() = 0;
  virtual void interface() const = 0;  // 纯虚函数，类不可能拥有实体
  virtual const char* mumble() const { return _mumble; }

 private:
  char* _mumble;
};
// 虽然被设计为一个抽象的基类，但它仍然需要一个明确的构造函数以初始化_mumble。
class Concrete_derived : public Abstract_base {
 public:
  Concrete_derived() {}
  void interface() const;
};
// 如果没有这个初始化操作，其派生类的局部对象_mumble将无法决定初值：
void foo() { Concrete_derived trouble; }
// 一般而言，类的数据成员应该被初始化，并且只在构造函数或在类的其他成员函数中指定初值。其他任何操作都将破坏封装性，不易维护。
// 不应该在抽象的基类总声明数据成员，这是比较强而有力的论点（把接口和实现分类）。但这并不是一直有理，因为将“被共享的数据”抽取出来放在基类中，是一种正当的设计。

// 竟然可以定义和调用一个纯虚函数。不过它只能被静态地调用（类名:: 这样么？），不能经由虚拟机制调用。
inline void Abstract_base::interface() const {}
inline void Concrete_derived::interface() const { Abstract_base::interface(); }
// 纯虚析构函数一定得定义它。因为每一个派生类析构函数会被编译器扩展，以静态调用的方式调用其“每一个虚基类”以及“上一层基类”的析构函数。
// 因此，只要缺乏任何一个基类析构函数的定义，就会导致链接失败。
Abstract_base::~Abstract_base() {}
// 编译器不能压抑纯虚析构函数的调用，因为C++语言的一个保证前提：继承体系中每一个对象的析构函数都会被调用。所以编译器不能压抑这个调用操作。
// 编译器没有足够的知识来定义一个纯虚析构函数。
// 要么要给出纯虚析构函数的定义，要不使用比较好的替代方案，不要把虚析构函数声明为纯虚的。

// mumble并不与类型有关，因而几乎不会被后继的派生类改写。
// 一般而言，把所有的成员函数都声明为虚函数，然后在靠编译器的优化操作把非必要的virtual去除，并不是好的设计观念。
// 决定一个虚函数是否需要const不那么容易：
//  不把函数声明为const，意味着该函数不能够获得一个const引用或const指针。
//  声明一个函数为const，然后发现派生类又必须修改某一个数据成员。
// 可以考虑不再用const了。

// 重新定义：
class Abstract_base1 {
 public:
  virtual ~Abstract_base1();
  virtual void interface() const = 0;  // 纯虚函数，类不可能拥有实体
  const char* mumble() const { return _mumble; }

 protected:
  Abstract_base1(char* pc = 0);

 private:
  char* _mumble;
};

// 5.1 “无继承”情况下的对象构造

// C++标准说下面是一种所谓的Plain Ol' Data声明形式：
typedef struct { float x, y, z; } Point;
// 用C++编译上面的代码，编译器会声明一个无用的默认构造函数、一个无用额析构函数、一个无用的拷贝构造函数，以及一个无用的拷贝赋值操作符。
// 但实际上编译器会分析这个声明，并为它提上Plain Ol' Data标签。
// 当编译器遇到下面的定义，无用的默认构造函数和析构函数都会别调用。构造是在程序起始出被调用而析构是在exit处被调用（exit有系统产生，放在main结束之前）。
// 实际上那些无用成员，要不是没被定义，就是没被调用，程序的行为与C中表现的一样。
Point global;  // 生命期和整个程序的相同
// 在C中，global被视为一个”临时性的定义“，因为它没有明确的初始化操作。
// 一个“临时性的定义”可以在程序中发生多次。那些实例会别链接器折叠起来，只留单独一个实体，被放在程序data segment中一个“特别保留给未初始化的全局对象使用”
// 的空间，这块空间被称为BSS（Block Started by Symbol）。
// C++并不支持“临时性的定义”，这是因为类构造行为的隐含应用之故。虽然可以判断一个对象是Plain Ol' Data，但似乎没有必要搞得那么复杂。
// 因此，global在C++中被视为完全定义（它会阻止第二个或更多个定义）。
// C和C++的一个差异就在于，BSS data segment在C++中相对地不重要。C++的所有全局对象都被当做“初始化过的数据”来对待。
Point foobar() {
  Point local;  // 生命期到函数结束，既没有被构造也没有被析构
  Point* heap = new Point;  // 生命期从new到delete
  // new返回的对象并没有调用默认构造函数
  *heap = local;  // 如果local被适当的初始化化过，一切就没有问题
  // 观念上，无用的拷贝赋值操作符会被调用，但由于是Plain Ol' Data，所以赋值操作将只是想C那样的纯粹位搬移操作
  delete heap;   // 观念上，会调用析构函数。但析构函数要不是没有被产生就是没有被调用
  return local;  // 观念上，会调用拷贝构造函数，不过实际上只是简单的位拷贝操作，因为是Plain Ol' Data
}

// 经过封装的Point class，其大小并没有改变，还是连续三个float。不论private、public，或是成员函数声明，都不会占用额外的对象空间。
class Point1 {
 public:
  Point1(float x = 0.0, float y = 0.0, float z = 0.0) : _x(x), _y(y), _z(z) {}
  // 并没有定义拷贝构造函数或赋值操作符，因为默认的位语义已经足够。
  // 也不需要提供析构函数，默认的内存管理方法也已足够。

 private:
  float _x, _y, _z;
};
Point1 global1;  // 调用Point1(0.0,0.0,0.0);
// 由于global1被定义在全局作用域，初始化将延迟到程序激活(startup)才开始。

// 如果要对类中的所有成员都设置常量初值，给予一个显示的初始化列表会比较高效（比起在构造函数体内设置而言）。
// 甚至在局部空间内，也是如此。
void mumble() {
  Point1 l1 = {1.0, 1.0, 1.0};
  // 上面比下面会稍微快一些。因为activation record被放进程序堆栈时，初始化列表中的常量就可以被直接放进l1内存中了。
  Point1 l2;
  // l2._x = 1.0;
  // l2._y = 1.0;
  // l2._z = 1.0;
}
// Explicit initialization list带来三项缺点：
//  1 只有当类成员都是public时，此法才奏效。
//  2 只能指定常量，因为它们在编译期就可以被评估求值。
//  3 由于编译器并没有自动实施，所以初始化行为的失败可能性会比较高一些。
// 手工打造了一些巨大的数据结构如调色盘，或是要把一堆常量数据倾倒给程序，那么Explicit initialization list的效率会比inline构造好的多，
// 特别是对全局对象而言。
// 在编译器层面，会有一个优化机制来识别inline构造，后缀简单地提供一个member-by-member的常量指定操作。
// 然后编译器会抽取那些值，并且对待它们就好像Explicit initialization list所供应的一样，而不会把构造扩展成为一系列的赋值指令。

// 一般而言，如果代码中有需要许多函数都需要以传值方式传回一个局部对象，那么提供一个拷贝构造函数就比较合理。甚至即使默认的按位拷贝语意已经足够。
// 因为它的出现会触发NRV优化。NRV优化后将不再需要调用拷贝构造函数，因为运算结果已经被直接放置与“被传回的对象”体内了。

// 5.2 继承体系下的对象构造

// T Object;
// 如果T有一个构造函数（不论是自定义的还是有编译器合成的），它会被调用。这很明显，不明显的是构造函数调用真正伴随了什么？
// 构造函数内有大量的隐藏代码，因为编译器会扩充每一个构造函数，扩充程度视类的继承体系而定。一般而言编译器所做的扩充大约如下：
//  1 记录在member initialization list中的数据成员初始化操作会被放进构造函数本身，并以成员的声明顺序为顺序。
//  2 如果一个成员没有出现在member initialization list中，但它有一个默认构造函数，那么该默认构造函数必须被调用。
//  3 在那之前，如果对象有virtual table pointers，它们必须被设置初值，指向适当的virtual tables。
//  4 在那之前，所有上一层的基类构造函数必须被调用，以基类的声明顺序为顺序（与member initialization list中的顺序没有关联）：
//    如果基类被列于member initialization list中，那么任何明确指定的参数都应该传递过去。
//    如果基类没有被列于member initialization list中，而它有默认构造函数（或默认memberwise拷贝构造函数），那么就调用之。
//    如果基类是多重继承下的第二或后继的基类，那么this指针必须有所跳转。
//  5 在那之前，所有虚基类构造函数必须被调用，从左到右，从最深到最浅：
//    如果基类被列于member initialization list中，那么任何明确指定的参数都应该传递过去。若没有列于list中，而class有一个默认构造函数，也应应该调用之。
//    此外，类中的每一个virtual base class subobject的偏移量（offset）必须在执行期可被存取。
//    如果对象是最底层的类，其构造函数可能被调用；某些用以支持这个行为的机制必须被放进来。
class Point2 {
 public:
  Point2(float x = 0.0, float y = 0.0) : _x(x), _y(y) {
    cout << "Point2 C" << endl;
    size();
  }
  Point2(const Point2&) {}
  Point2& operator=(const Point2&) {}
  virtual ~Point2() {}
  virtual float z() { return 0.0; }
  virtual void size() { cout << "Point2::size" << endl; }

 private:
  float _x, _y;
};
class Line {
  Point2 _begin, _end;

 public:
  Line(float = 0.0, float = 0.0, float = 0.0, float = 0.0) {}
  Line(const Point2& end, const Point2& begin) : _end(end), _begin(begin) {}
  // 会被扩充为：
  // Line(Line* this, const Point2& end, const Point2& begin) {
  //   this->_begin.Point2::Point2(begin);
  //   this->_end.Point2::Point2(end);
  //   return this;
  // }

  // 每一个显示构造函数都会被扩充以调用其两个成员对象的构造函数
  void draw();

  // 由于Point2声明了一个拷贝构造函数、一个赋值操作符，以及一个析构函数，所以Line的implicit copy constructor、
  // copy operator和destructor都将有实际功能。
};
// Line a;
// Line隐式析构函数会被合成出来。在其中，它的成员对象的析构函数会被调用（以其构造的相反顺序）。
// 如果Line派生自Point，那么合成出来的析构函数将会是virtual。由于Line只是内带Point对象而非继承，所以被合成出来的析构函数是nontrivial。
// 如果Point析构函数是inline函数，那么每一个调用操作会在调用地点扩展开来。
// 虽然Point析构函数是virtual，但其调用操作会被静态地决议出来。
// Line b = a;  // 隐式拷贝构造函数被合成出来，成为一个inline public member。
// a = b;  // 隐式赋值操作符会别合成出来，成为一个inline public member。
// 在一个有程序员供应的赋值操作符中忘记检查自我赋值操作，是新手极易陷入的一项错误。

// 虚拟继承
class Point3d : public virtual Point2 {
 public:
  Point3d(float x = 0.0, float y = 0.0, float z = 0.0) : Point2(x, y), _z(z) {
    cout << "Point3d C" << endl;
    size();
  }
  // 传统的构造函数扩充现象并没有用，这是因为虚基类的“共享性”之故：
  // Point3d* Point3d::Point3d(Point3d* this, bool __most_derived, float x, float y, float z) {
  //   if(__most_derived != false)  // 防止基类构造函数被调用多次
  //     this->Point::Point(x, y);
  //   this->__vptr_Point3d = __vptr_Point3d;
  //   this->__vptr_Point3d__Point = __vptr_Point3d__Point;
  //   this->_z = rhs._z;
  //   return this;
  // }
  // 更深层的继承下，调用Point3d时，__most_derived总是设为false。
  Point3d(const Point3d& rhs) : Point2(rhs), _z(rhs._z) {}
  ~Point3d() {}
  Point3d& operator=(const Point3d&) {}
  virtual float z() { return _z; }
  virtual void size() { cout << "Point3d::size" << endl; }

 protected:
  float _z;
};
// 按传统的策略（没有判断__most_derived）扩展构造函数时，考虑下面的继承体系：
class Vertex : virtual public Point2 {
 public:
  Vertex() {
    cout << "Vertex C" << endl;
    size();
  }
  virtual void size() { cout << "Vertex::size" << endl; }
};
class Vertex3d : public Point3d, public Vertex {
 public:
  Vertex3d() {
    cout << "Vertex3d C" << endl;
    size();
  }
  virtual void size() { cout << "Vertex3d::size" << endl; }
};
class PVertex : public Vertex3d {
 public:
  PVertex() {
    cout << "PVertex C" << endl;
    size();
  }
  virtual void size() { cout << "PVertex::size" << endl; }
};
// Vertex的构造函数必须也调用Point的构造函数。然后，当Point3d和Vertex同为Vertex3d的subobject时，它们对Point2
// 构造函数的调用一定不可以发生，取而代之的是，作为一个最底层的类，Vertex3d来负责完成“被共享的Point subobject”的构造。

// 只有当一个完整的对象被定义出来时，虚基类构造函数才会被调用；如果对象只是某个完整对象的subobject它就不会被调用。
// 某些编译器把每一个构造函数分裂为两个，一个针对完整的对象，另一个针对subobject。
// 完整版本无条件地调用虚基类构造函数，设定所有vptr。subobject版则不调用虚基类构造函数，也可能不设定vptrs等等。

// vptr初始化语意学
// 当定义一个Pvertex object时，构造函数的调用顺序是：
// Point2(x,y),Point3d(x,y,z),Vertex(x,y,z),Vertex3d(x,y,z),PVertex(x,y,z)
// 在一个类的构造函数中，经由构造中的对象来调用一个虚函数，其函数实体应该是在此类中作用的那个。
// 构造函数的调用顺序是：有根源到末端，有内而外，当基类构造函数执行时，派生类实体还没有被构造出来。
// 如果虚函数调用操作限制在构造和析构中直接调用，那么将每一个调用操作以静态方式决定，千万不要用到虚拟机制。其他情况，使用虚拟机制。
// 上面的限制操作，需要通过vptr来实现。

// vptr在构造函数中“应该在何时被初始化”，有三种选择：
//  1 在任何操作之前。
//  2 在基类构造函数调用操作之后，但是在程序员定义的代码或是member initialization list中所列的members初始化操作之前。
//  3 在每一件事情发生之后。
// 答案是2。另外两种没有什么价值。
// 令每一个基类构造函数设定其对象的vptr，使它指向相关的virtual table之后，构造中的对象就可以严格而正确地变成”构造过程中所幻化出来的每一个类“的对象。
// 也就是说，PVertex对象会先形成一个Point2对象、Point3d对象、一个Vertex对象、一个Vertex3d对象，然后才成为一个PVertex对象。
// 在每一个基类构造函数中，对象可以与构造函数的类的完整对象做比较。构造函数执行算法通常如下：
//  1 在派生类构造函数中，“所有虚基类”及“上一层基类”的构造函数会被调用。
//  2 上述完成之后，对象的vptrs被初始化，执行相关的virtual tables。
//    vptr必须被设定的两种情况：
//    当一个完整的对象被构造起来时。如果我们声明一个Point2对象，Point2构造函数必须设定其vptr。
//    当一个subobject构造调用了一个虚函数时，不论是直接调用或间接调用。
//  3 如果有member initialization list的话，将在构造体内扩展开来。这必须在vptr被设置之后才进行，以免一个虚函数被调用。
//  4 最后，执行程序员所提供的代码。
// 上面方法完美地解决了所说的有关限制虚拟机制的问题。

// Point2(float x, float y) : _x(x), _y(y) {}
// Point3d(float x, float y, float z) : Point2(x, y), _z(z) {}
// Vertex和Vertex3d构造函数也有类似的定义。
// 声明一个PVertex对象，由于对基构造函数重新定义了，其vptr将不再需要在每一个基类构造函数中被设定。
// 解决方法是把构造函数分裂为一个完整的对象实体和一个subobject实体。
// 在subobject实体中，vptr的设定可以省略（如果可能的话）。

// 在类的构造函数的member initialization list中调用该类的一个虚函数，安全吗？
// 就实际而言，将该函数运行于其类数据成员的初始化中，总是安全的。
// 这是因为，vptr保证能够在member initialization list被扩展之前，有编译器正确设定好。
// 但是在语意上这可能是不完全的，因为函数本身可能还得依赖未被设置初值的成员。所以并不推荐这种做法。
// 何时需要供应参数给一个基类构造函数？这种情况下在“类的构造函数的member initialization list”中调用该类的虚拟函数，仍然安全么？
// 不，此时vptr尚未设定好，就是被设定指向错误的类。更进一步，该函数所存取的任何类的数据成员一定还没被初始化。
class Base {
 public:
  Base() { cout << "Base" << endl; }
  Base(int i) : b(test(i)) { cout << "Base i" << endl; }
  virtual int test(int i) { cout << "Derived::test" << endl; }

 private:
  int b;
};
class Derived : public Base {
 public:
  Derived(int i) : a(test(i)) { cout << "Derived i" << endl; }
  Derived(int i, int j) : Base(i), a(j) { cout << "Derived i j" << endl; }
  virtual int test(int i) { cout << "Derived::test" << endl; }

 private:
  int a;
};

// 5.3 对象复制语意学

int main() {
  PVertex p;
  // Point2 C
  // Point2::size
  // Point3d C
  // Point3d::size
  // Vertex C
  // Vertex::size
  // Vertex3d C
  // Vertex3d::size
  // PVertex C
  // PVertex::size

  Derived a(5);
  // Base
  // Derived::test
  // Derived i

  Derived b(1, 2);
  // Derived::test
  // Base i
  // Derived i j

  return 0;
}