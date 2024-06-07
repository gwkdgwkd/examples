#include <iostream>

// Function语义学

namespace n1 {
// 4.1 Member的各种调用方式

// Nonstatic Member Functions（非静态成员函数）
// C++的设计准则之一就是：非静态成员函数至少必须和普通函数有相同的效率。
// 选择成员函数没有额外负担，这是因为编译器内部已将成员函数转换为对等的普通函数实体。
// 转换步骤为：
// 1.改写函数签名，以安插一个额外的参数到成员函数中，用以提供一个存取管道，
//   是对象可以调用该函数，额外参数被称为this指针；
// 2.将每一个对nonstatic data member的存取操作改为经由this指针来存取；
// 3.将成员函数重新写成外部函数，对函数名称进行mangling处理，使它成为独一无二的名称。

// 名称的特殊处理（Name Mangling）
// 一般而言，member的名称会被加上class名称，形成独一无二的命名。
// 为了重载，名称还要加上参数链表，目前C++编译器对name mangling的做法还没统一。

// Virtual Member Function(虚拟成员函数)
// ptr->normalize();
// 改为：
// (*ptr->vptr[1])(ptr);
// 1.vptr表示由编译器产生的指针，指向vitual table，
//   它被安插在每一个声明有（或继承自）一个或多个虚函数的对象中，
//   事实上vptr也会被mangled，因为在复杂的派生体系中，可能存在多个vptr。
// 2.1是virtual table slot的索引值，关联到normalize()函数；
// 3.第二个ptr表示this指针。

// 在normalize中调用虚拟函数magnitude：
// 由于normaliz已经有虚拟机制而决议妥当，所以可以明确调用的实体会比较有效率，
// 并因此压制虚拟机而产生不必要的重复调用操作：
// register float mag = Point3d::magnitude();不是(*this->vptr[2])(this);
// 如果magnitude声明为inline函数会更有效率。
// 通过对象调用虚函数这种操作，总是被编译器像对待一般的非静态成员函数一样地加以决议。

// Static Member Functions（静态成员函数）
// 无论是通过指针还是通过对象调用静态成员函数，都将被转换为一般的nonmember函数调用。
// Static Member Functions特性：
// 1.最主要的特性是没有this指针；
// 2.不能够直接存取其class中nonstatic members；
// 3.不能够被声明为const、volatile或virtual；
// 4.不需要经由对象才能被调用，虽然大部分时候它是这样被调用的。
// 对一个静态成员函数取地址，获得的是内存中的位置。
// 由于没有this指针，所以地址的类型不是指向类成员函数的指针，而是一个非成员函数指针。
// 静态成员函数差不多等同于普通函数，因此提供了意想不到的好处：
// 成为一个callback函数，使得以将C++和C系统结合，也可以成功地应用在线程函数身上。
}  // namespace n1

namespace n2 {
// 4.2 Virtual Member Functions（虚拟成员函数）

// 在C++中，多态表示以public base class的指针（或引用），引用派生类对象的意思。
// 判断一个类是否支持多态，唯一适当的方法就是看看它是否有任何虚函数。
// 只要有虚函数，它就需要额外的执行期信息：
// 1.一个表格，包含了程序的虚函数的执行期地址；
// 2.为了找到表格，每一个对象被安插上一个由编译器内部产生的指针，指向该表格；
// 3.为了找到函数地址，每一个虚函数被指派一个表格索引值。
// 上面的工作都由编译器完成。
// 执行期间要做的，只是在特定的virtual table slot中调用virtual function。

namespace test1 {
// 一个类只会有一个virtual table。
// 每一个table内含其对应的对象中所有active virtual functions函数实体的地址。
// 可能包括：
// 1.这个类所定义的函数实体，它会改写一个可能存在的基类虚函数实体；
// 2.继承自基类的函数实体，这是在派生类决定不改写虚函数时才会出现的情况；
// 3.一个pure_virtual_called()函数实体，
//   它既可以扮演pure virtual function的空间保卫者角色，
//   也可以当做执行期异常处理函数；
// 4.每个虚函数都被分配固定的索引，这个索引在整个继承体系中保持与特定的虚函数的关联。

class Point {
 public:
  virtual ~Point();                // 被赋值slot 1
  virtual Point &mult(float) = 0;  // slot 2，如果被调用，通常是结束程序
  float x() const { return _x; }         // 没有slot，因为是非虚函数
  virtual float y() const { return 0; }  // slot 3
  virtual float z() const { return 0; }  // slot 4

 protected:
  Point(float x = 0.0);
  float _x;

  // 内存布局：
  //  __vptr__Point
  //  _x
  // Virtual Table Point
  //  type_info for Point
  //  Point::~Point()
  //  pure_virtual_called()
  //  Point::y()
  //  Point::z()
};
class Point2d : public Point {
 public:
  Point2d(float x = 0.0, float y = 0.0) : Point(x), _y(y) {}
  ~Point2d();
  Point2d &mult(float);
  float y() const { return _y; }

 protected:
  float _y;

  // 内存布局：
  //  __vptr__Point
  //  _x
  //  _y
  // Virtual Table Point2d
  //  type_info for Point2d
  //  Point2d::~Point2d()
  //  Point2d::mult()
  //  Point2d::y()
  //  Point::z()

  // 当一个类派生时，有三种可能：
  // 1.可以继承基类所声明的虚函数实体，正确地说，
  //   是该函数实体会被拷贝到派生类的virtual table相对应的slot中；
  // 2.可以使用自己的函数实体，这表示它自己的函数实体地址必须放在对应的slot中；
  // 3.可以加入一个新的virtual function，这时候virtual table的尺寸会增加一个，
  //   而新的函数实体被放进该slot中。
};
class Point3d : public Point2d {
 public:
  Point3d(float x = 0.0, float y = 0.0, float z = 0.0) : Point2d(x, y), _z(z) {}
  ~Point3d();
  Point3d &mult(float);
  float z() const { return _z; }

 protected:
  float _z;

  // 内存布局：
  //  __vptr__Point
  //  _x
  //  _y
  //  _z
  // Virtual Table Point3d
  //  type_info for Point3d
  //  Point3d::~Point3d()
  //  Point3d::mult()
  //  Point2d::y()
  //  Point3d::z()
};
void func() {
  Point *ptr;
  ptr->z();
  // 一般而言，并不知道ptr所指对象的真正类型，但运行时可以调用正确的函数：
  // 经由ptr可以存取到该对象的virtual table。
  // 虽然不知道哪一个z()函数实体被调用，但知道每个z()函数地址都放在slot 4。
  // 这些信息使得编译可以将调用转换为：
  // (*ptr->vptr[4])(ptr);
  // 唯一不在编译期知道的是：slot 4所指的到底是哪一个z()函数实体。
}
}  // namespace test1

namespace test2 {
// 多重继承下的Virtual Functions

// 在一个单一继承体系中，虚函数机制十分良好，不但有效率而且很容被塑造出来。
// 但是多重继承和虚拟继承中，对虚函数的支持就没那么好了。
// 在多重继承中支持virtual functions，
// 其复杂度围绕在第二个及后续的基类身上，以及必须在执行期调整this指针这一点。
class Base1 {
 public:
  Base1() {}
  virtual ~Base1() {}
  virtual void speakClearly() {}
  virtual Base1 *clone() const {}

 protected:
  float data_Base1;

  // 内存布局：
  //  __vptr__Base1
  //  data_Base1
  // Virtual Table Base1
  //  type_info for Base1
  //  Base1::~Base1()
  //  Base1::SpeakClearly()
  //  Base1::clone()
};
class Base2 {
 public:
  Base2() {}
  virtual ~Base2() {}
  virtual void mumble() {}
  virtual Base2 *clone() const {}

 protected:
  float data_Base2;

  // 内存布局：
  //  __vptr__Base2
  //  data_Base2
  // Virtual Table Base2
  //  type_info for Base2
  //  Base2::~Base2()
  //  Base2::mumble()
  //  Base2::clone()
};
class Derived : public Base1, public Base2 {
 public:
  Derived() {}
  virtual ~Derived() {}
  virtual Derived *clone() const {}

 protected:
  float data_Derived;

  // 内存布局：
  //  __vptr__Base1
  //  data_Base1
  //  __vptr__Base2
  //  data_Base2
  //  data_Derived
  // Virtual Table Derived(shared with Base1)
  //  type_info for Derived
  //  Derived::~Derived()
  //  Base1::SpeakClearly()
  //  Derived::clone()
  //  Base2::mumble() * // *表示需要调整this指针
  // Virtual Table Derived/Base2
  //  type_info for Derived
  //  Derived::~Derived() *
  //  Base2::mumble()
  //  Derived::clone() *

  // Derived支持virtual functions的困难度，统统落在Base2 subobjcet身上。
  // 有三个问题需要解决：
  // 1.virtual destructor;
  // 2.被继承下来的Base2::mumble;
  // 3.一组clone函数实体。

  // 有两个virtual tables被编译器产生出来：
  // 1.主要实体，与Base1（最左端base class）共享。
  // 2.次要实体，与Base2（第二个base clase）有关。
  // 针对每个virtual tables，派生类中有对应的vptr，vptrs在构造函数中初始化。
  // 用以支持一个类拥有多个virtual tables的传统方法是，
  // 将每一个表格以外部对象的形式产生出来，并给予独一无二的名称，比如：
  // vtbl__Derived;  // 主要表格
  // vtbl__Base2__Derived;  // 次要表格
  // 当将一个派生类对象地址指定给一个Base1指针或Derived指针时，使用主要表格，
  // 而当将一个派生类对象地址指定给一个Base2指针时，使用次要表格。
};

// Base2 *pbase2 = new Derived;
// 新的Derived对象的地址必须调整，以指向其Base2 subobject。
// 编译期会产生以下代码：
// Derived *temp = new Derived;
// Base2 *pbase2 = temp ? temp + size(Base1) : 0;
// 如果没有这样的调整，指针的任何非多态运用都将失败：
// pbase2->data_Base2;
// 当程序员要删除pbase2所指的对象时：delete pbase2;
// 指针必须被再一次调整，以求再一次指向Dervied对象的起始处。
// 然后上述的offset加法却不能够在编译期直接设定，
// 因为pbase2所指的真正对象只有在执行期才能确定。
// 一般规则是，经由指向第二或后继的基类的指针（或引用）来调用派生类虚函数。
// 调用操作所连带的必要的this指针调整操作，必须在执行期完成。
// 也就是说，offset的大小，以及把offset加到this指针上头的那一小段代码，
// 必须有编译器插入到某个地方，问题是在哪个地方？
// 比较有效率的解决方法是理由所谓的thunk。
// 是一小段assembly码，用来以适当的offset值调整this指针和跳到virtual
// function去。 pbbase2_dtor_thunk:
//  this += sizeof(base1);
//  Derived::~Derived(this);
// thunk允许virtual table
// slot内含一个简单的指针，因此多重继承不需要空间上的额外负担。
// slot中的地址可以指向virtual function，
// 也可以指向thunk（如果需要调整this指针的话）。
// 因此对于那些不需要调整this指针的virtual
// function而言，也就不需承载效率上的额外负担。
// 调整this指针的第二个额外负担就是，由于两种不同的可能：
// 1.经由派生类（或第一个基类）调用；
// 2.经由第二个（或其后继）基类调用，同一函数在virtual
// table中可能需要多个对应的slots，
//   Base1 *pbase1 = new Derived;
//   Base2 *pbase2 = new Derived;
//   delete pbase1;
//   delete pbase2;
//   虽然两个delete操作导致调用相同的派生类析构函数，
//   但它们需要两个不同的vitrual table slots:
//   pbase1不需要调整this，因为Base1是最左端的base
//   class，已经指向了Derived的起始处。 vitrual table
//   slot放置的是真正的destructor地址。 pbase2需要调整this指针，其vitrual table
//   slot需要相关的thunk地址。
// 在多重继承下，一个派生类内含n-1个额外的virtual tables，
// n表示上一层次基类的数目，单一继承不会有额外的virtual tables。

void func() {
  // 有三种情况，第二或后继的base class会影响对virtual functiron的支持：

  // 1.通过一个指向第二个基类的指针，调用派生类虚函数：
  Base2 *ptr = new Derived;
  // ptr指向Derived对象中的Base2 subobject，
  // 为了能够正确执行，ptr必须调整指向Derived对象的起始处。
  // 调用Derived::~Derived，ptr必须被向后调整sizeof(Base1)个btyes：
  delete ptr;

  // 2.通过一个指向派生类的指针，调用第二个基类中一个继承而来的虚函数，
  //   派生类指针必须再次调整，以指向第二个base subobject：
  Derived *pder = new Derived;
  pder->mumble();  // pder必须调整sizeof（Base1）个btyes

  // 3.发生于一个语言扩充性质之下：
  //   允许虚函数返回值类型有所变化，可能是基类类型，
  //   也可能是publicly derived type。
  //   Derived::clone()函数返回一个Derived class指针，
  //   默默改写了两个基类函数实体。
  Base2 *pb1 = new Derived;
  //   当通过指向第二个基类的指针来调用clone()时，this指针的offset问题于是诞生：
  Base2 *pb2 = pb1->clone();
  //   调Derived::clone，调整返回值指向Base2 subobject。
  //   当调用clone时，pb1会被调整到派生类对象的真实地址，于是clone的Derived会被调用；
  //   会返回一个指针，指向一个新的Derived对象，该对象的地址在被指给pb2之前，
  //   必须先经过调整，以指向Base2 subobject。
}
}  // namespace test2

namespace test3 {
// 虚拟继承下的virtual Function
class Point2d {
 public:
  Point2d(float = 0.0, float = 0.0) {}
  virtual ~Point2d() {}
  virtual void mumble() {}
  virtual float z() {}

 protected:
  float _x, _y;

  // 内存布局：
  //  __vptr__P2d
  //  _x
  //  _y
  // Virtual Table Point2d
  //  type_info for Point2d
  //  Point2d::~Point2d()
  //  Point2d::mumble()
  //  Point2d::z()
};
class Point3d : public virtual Point2d {
 public:
  Point3d(float = 0.0, float = 0.0, float = 0.0) {}
  virtual ~Point3d() {}
  float z() {}

 protected:
  float _z;

  // 内存布局：
  //  __vptr__Point3d
  //  _z
  //  __vptr__Point3d
  //  _x
  //  _y
  // Virtual Table Point2d subobject of Point3d
  //  type_info for Point3d
  //  Point3d::~Point3d()
  //  Point3d::mumble()  P2d？
  //  Point3d::z()
  // Virtual Table Point3d
  //  offset to virtual base
  //  type_info for Point3d
  //  Point3d::~Point3d()
  //  Point3d::mumble()  P2d？
  //  Point3d::z()
};
// 虽然Point3d有唯一一个base class，
// 但Point3d和Point2d的起始部分并不像非虚拟的单一继承情况那样一致。
// 由于Point2d和Point3d的对象不再相符，两者之间的转换也就需要调整this指针。
// 建议：不要在一个virtual base class中声明nostatic data members。
// 否则，距离复杂的深渊越来越近。
}  // namespace test3
}  // namespace n2

namespace n3 {
// 4.3 函数的效能
}

namespace n4 {
// 4.4 指向Member Functions的指针

// 取非静态成员函数地址，如果该函数不是虚拟的，则得到的结果是它在内存中真正的地址。
// 这个值也需要被绑定于某个对象的地址上，才能通过它调用该函数。
// 所有的非静态成员函数都需要对象的地址，以参数this指出。
class A {
 public:
  virtual ~A() {}
  double x() { std::cout << "A::x" << std::endl; }
  double y() { std::cout << "A::y" << std::endl; }
  virtual float z() { std::cout << "A::z" << std::endl; }
};
// 使用一个成员函数指针，如果不使用虚函数、多重继承、
// 虚拟继承等（调用太过复杂），并不会比使用一个非成员函数指针的成本更高。
void func() {
  A a;
  double (A::*coord)() = &A::x;
  (a.*coord)();  // A::x   (coord)(&p);
  coord = &A::y;
  A *p = &a;
  (p->*coord)();  // A::y  (coord)(pp);

  float (A::*pmf)() = &A::z;
  A *pa = new A;
  pa->z();                          // A::z
  (pa->*pmf)();                     // A::z  (*pa->vptr[(int)pmf])(pa);
  std::cout << &A::z << std::endl;  // 1
  std::cout << &A::x << std::endl;  // 1
  std::cout << &A::y << std::endl;  // 1
  printf("%d\n", &A::z);            // 17
  printf("%p\n", &A::x);            // 0x555c097b29a0
  printf("%p\n", &A::y);            // 0x555c097b29d8
}

// 支持指向virtual member Functions的指针
// 虚函数地址在编译期是未知的，只知道虚函数在virtual table中的索引值。
// 对虚函数取地址，能获得的只是一个索引值。
// 对于一个指向成员函数的指针评估求值，会因为该值有两种意义而复杂化：
// 1.能够寻址出nonvirtual x()代表的内存地址；
// 2.virtual z()代表的virtual table中的索引值。
// 编译器必须能够识别指向成员函数的指针含有两种数值，
// 更重要的是其数值可以被区别代表内存地址或是virtual table中的索引值。

// 多重继承下，指向Member Function的指针
// 为了支持多重继承和虚拟继承，Stroustrup设计了下面的结构体：
// struct __mptr {
//   int delta;  // this指针的offset值
//   int index;
//   union {
//     ptrofunc faddr;
//     int v_offset;  // 不指向virtual table时，会被设为-1
//   };
// };
// (ptr->*pmf)(); 转换为：
// (pmf.index < 0) ? (*pmf.faddr)(ptr) : (*ptr->vptr[pmf.index](ptr));
// 这种方法收到的批评是，每一个调用操作都得付出上述成本，
// 微软把这个检查拿掉，导入一个所谓的vcall thunk。
// faddr要不就是成员函数地址，要不是就是vcall thunk的地址。
// 上面结构的另一个副作用是，当传递不变值的指针给memver
// function时，会产生临时对象。
// 许多编译器在自身内部根据不同的类特性提供多种指向member functions的指针形式。
}  // namespace n4

namespace n5 {
// 4.5 Inline Functions

// 并不能够强迫任何函数变成inline，关键字inline只是一项请求。
// 如果被编译器接受，就必须认为它可以用一个表达式合理地将这个函数扩展开来。
// 一般而言，处理一个inline函数，有两个阶段：
// 1.分析函数定义，以决定函数的本质的能力，如果函数因其复杂度或因其构建问题，
//   被判为不可成为inline，它会被转为static函数，并在编译模块内产生对应的函数定义。
// 2.真正的inline函数扩展是在调用位置，这会带来参数的求值操作以及临时性对象的管理。

// 在inline扩展期间，到底发生了什么事情？
// 每一个形参都会被实参取代。
// 副作用是不可以只是简单地一一替换形参，因为这将导致对于实际参数的多次求值操作。
// 一般而言，面对会带来副作用的实际参数，通常都需要引入临时性对象。
// 换句话说，如果实际参数是一个常量表达式，可以在替换之前完成其求值操作，
// 后继的inline替换，就可以把常量直接绑上去，如果既不是各个常量表达式，
// 也不是个带有副作用的表达式，那么就直接替换之。

// 一般而言，inline函数中的每一个局部变量，
// 都必须被放在函数调用的一个封闭区段中，拥有一个独一无二的名称。
// 如果inline函数以单一表达式扩展多次，那么每次扩张都需要自己的一组局部变量。
// 如果inline函数以分离的多个式子被扩展多次，那只需要一组局部变量，就可以重复使用。
// inline函数中的局部变量，再加上有副作用的参数，可能会导致大量临时对象的产生。
// 特别是如果它以单一表达式被扩展多次的话。

// inline函数对于封装提供了一种必要的支持，可以有效存取封装于类中的nonpublic数据。
// 它也是C中大量使用的#define的一个安全替代品，特别是宏中的参数有副作用的话。
// 然而一个inline函数如果别调用太多次的话，会产生大量的扩展码，是程序的大小暴涨。

// 参数带有副作用，或是以一个单一表达式做多重调用，或是在inline函数总有多个局部变量，
// 都会产生临时对象，编译器也许能把它们移除，此外，inline中再有inline，
// 可能会使一个表面上看起来平凡的inline却因其连锁复杂度而没办法扩展开来。
// 这种情况可能发生于复杂类体系下的构造函数，
// 或是对象体系中一些表面上并不正确的内联调用所组成的串链。
// 对于既要安全有要效率的程序，inline函数提供了一个强而有力的工具。
// 然而，与non-inline函数比起来，它们需要更加小心地处理。
}  // namespace n5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n2::test2::func();
      break;
    case 1:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}