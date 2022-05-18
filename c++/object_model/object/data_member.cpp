#include <assert.h>
#include <cstdio>
#include <iostream>

using namespace std;

// X,Y,Z,A中没有任何一个class内含明显的数据，其间只表示了继承关系。
class X {
  // 事实上并不是空的，它有一个隐晦的1字节，那是被编译器安插进取的一个char。
  // 这使得这个类的两个对象得以在内存中配置独一无二的地址。
};
class Y : public virtual X {
  // 大小和机器有关，也和编译器有关，大小受到三个因素的影响：
  // 1.语言本身所造成的额外负担
  //   当语言支持虚基类时，就会导致一些额外负担。
  //   在派生类中，这个额外负担反映在某种形式的指针身上，它或者指向virtual base class subobject，
  //   或者指向一个表格，表格中存放的若不是virtual base class subobject的地址，就是其偏移量。
  // 2.编译器对于特殊情况所提供的优化处理
  //   virtual base class X subobject的1字节大小也出现在Y和Z身上。
  //   传统上它被放在派生类的固定（不变动）部分的尾端。
  //   某些编译器会对empty virtual base class提供特殊支持。
  //   empty virtual base class已经成为C++ OO设计的一个特有术语了。
  //   它提供了一个虚拟接口，没有定义任何数据，某些新的编译器对此提供了特殊处理。
  //   在这个策略下，empty virtual base class被视为派生类最开头的一部分，也就是说它并没有花费任何额外的空间。
  //   节省了1字节，只剩下第1点说说的额外负担了。
  // 3.Alignment的限制
  //   Y和Z的大小截止目前为5字节。
  //   大部分机器上，结构体的大小会受到alignment的限制，是它们能够更有效率地在内存中存取。
};
class Z : public virtual X {
  // 同Y
};
class A : public Y, public Z {
  // 一个virtual base class subobject只会在派生类中存在一份实体，不管它在类继承体系中出现了多少次。
  // A的大小由以下几点决定（书中说A的大小是12）：
  // 1.被大家共享的唯一一个X实体，大小为1字节。
  // 2.基类Y的大小，减去X的大小，结果是4字节，Z的算法相同，加起来是8字节。
  // 3.A自己的大小，0字节。
  // 4.A的alignment数量（如果有的话）。
  //   前三项的和为9，按4字节对齐，补足3字节，结果是12字节。
  // 对empty virtual base class特殊处理的编译器，拿掉了1字节和额外的3字节，A的大小是8字节。
};
void func1() {
  // 所以有人认为每一个类的大小都应该是0，这是不对的，即使是类X的大小也不为0：
  cout << "X sizeof: " << sizeof(X) << endl;  // 1
  cout << "Y sizeof: " << sizeof(Y) << endl;  // 8
  cout << "Z sizeof: " << sizeof(Z) << endl;  // 8
  cout << "A sizeof: " << sizeof(A) << endl;  // 16
}

// C++标准并不强制规定如base class subobjects的排列次序或不同存取层级的数据成员的排列次序这种琐碎细节。
// 也不规定虚函数或虚基类的实现细节，标准只说：那些细节有各厂商自定。
// C++对象模型尽量以空间优化和存取速度优化的考虑来表现非静态数据成员，并保持和C语言struct数据配置的兼容性。
// 它把数据直接放在每一个对象中，对于继承而来的非静态数据成员（不管是virtual或nonvirtual base class）也是如此。
// 不过并没有强调定义其间的排列顺序。
// 至于静态数据成员，则被放置在程序的一个全局数据段中，不会影响个别的对象大小。
// 在程序中，不管该类被产生出多少个对象（直接产生或间接派生），
// 静态数据成员永远存在一份实体，甚至即使该类没有任何对象实体，其静态数据成员也已存在。
// 但是一个模板类的静态数据成员的行为稍有不同。
// 每一个对象必须有足够的大小以容纳它所有的非静态数据成员，有时候其值比你想象的大，因为：
// 1.有编译器自动加上的额外数据成员，用以支持某些语言特性（主要是各种virtual特性）。
// 2.因为alignment的需要。

// 3.1 数据成员的绑定

extern float x;
class Point3d1 {
 public:
  Point3d1(float, float, float);
  float X() const {
    return x;  // 返回内部的x
  }
  void X(float new_x) { x = new_x; }

 private:
  float x, y, z;
};
// 在C++最早的编译器上，如果Point3d1::X()的两个函数实例中对x进行参阅（取用）操作，这操作将会指向全局的x。
// 这样的绑定结果几乎遍地都是，但不在大家预览之中，并因此导出早期C++的两种防御性程序设计风格：
// 1.把所有的数据成员放在类声明起头处，以确保正确的绑定：
class Point3d2 {
  float x, y, z;

 public:
  float X() const { return x; }
};
// 2.把所有的内联函数，不管大小都放在类声明之外：
class Point3d3 {
 public:
  Point3d3(float, float, float);
  float X() const;
  void X(float);

 private:
  float x, y, z;
};
inline float Point3d3::X() const { return x; }
// 这些程序设计风格事实上到今天还存在，虽然它们的必要性已经从C++2.0之后就消失了。
// 这个古老的语言规则被称为member rewriting rule，
// 大意是一个内联函数实体，在整个类声明未被完全看见之前，是不会被评估求值的。
// C++标准以member scope resolution rules来精炼这个rewriting rule，其效果是，
// 如果一个内联函数在类声明之后立刻被定义的话，那么就还是对其评估求值。
// 然而，对于成员函数的参数列表并不为真，参数列表中的名称还是会在它们第一次遭遇时被适当地决议完成。
// 因此在extern和nested type names之间的非直觉绑定操作还是会发生。
typedef int length;
class XX {
 public:
  void mumble(length val) { _val = val; }  // length被决议为global
  length mumble() { return _val; }

 private:
  // length必须在本类对它的第一个参数操作之前被看见这样的声明将使先前的参考操作不合法
  // typedef float length;
  // 这种情况，仍然需要某种防御性程序风格：请始终把nested type声明放在类的起始处。
  length _val;
};

// 3.2 Data Member的布局

// 非静态数据成员在对象中的排列顺序将和其被声明的顺序一样，任何中间介入的静态数据成员都不会放进对象布局之中。
// C++标准要求，在同一个访问段（也就是private、public、protected等区段）中，
// 成员的排序只需符合“较晚出现的成员在对象中有较高的地址”这一条即可。
// 也就是说，各个成员并不一定得连续排序。成员的边界调整可能需要填补一些字节，这些位于成员之间。
// 编译器还可能合成一些内部使用的数据成员，以支持整个对象模型，vptr就是这样的东西。它会放在什么位置呢？
// 传统上被放在所有明确声明的成员的最后。也有一些会放在对象的最前端。
// C++标准秉承先前所说的“对于布局持的放任态度”，允许编译器把那些内部产生出来的members自由放在任何位置上，
// 甚至那些被程序员声明出来的成员之间。
// C++标准也允许编译器将多个访问段之中的数据成员自由排列，不必在乎他它们出现在类中声明中的次序。
// 各家编译器都是把一个以上的访问端连接在一起，依照声明的次序，成为一个连续区块。访问段的多少并不会招来额外负担。
class Test {
 public:
  float x;

 public:
  float y;

 public:
  float z;
};
union Cmp {
  float Test::*mem;
  long offset;
};
template <class class_type, class data_type1, class data_type2>
const char* asscess_order(data_type1 class_type::*mem1,
                          data_type2 class_type::*mem2) {
  // 接受两个数据成员，然后判断谁先出现在类对象中
  // assert(mem1 != mem2);
  // return mem1 < mem2 ? "member 1 occurs first" : "member 2 occurs first";
  Cmp cmp1 = {mem1};
  Cmp cmp2 = {mem2};
  return cmp1.offset < cmp2.offset ? "member 1 occurs first"
                                   : "member 2 occurs first";
}
void func2() {
  cout << asscess_order(&Test::z, &Test::y) << endl;  // member 2 occurs first
}

// 3.3 Data Member的存取

// Static Data Members
// 被编译器提出于类之外，并被视为一个全局变量（但只在class生命范围之内可见）。
// 每一个成员的存取许可，以及与class的关联，并不会导致任何空间上或执行时间上的额外负担，
// 不论是在个别对象或是静态数据成员本身。
// 每一个静态数据成员只有一个实体，存放在程序的data segment之中。
// 每次程序取用静态成员，就会被内部转化为对该唯一的extern实体的直接参考操作。

// Point3d origin,*pt = &origin;
// origin.chunkSize = 250;  // Point3d::chunkSize = 250;
// pt->chunkSize = 250;     // Point3d::chunkSize = 250;
// 从指令执行的观点来看，这是C++语言中通过一个指针和通过一个对象来存取member，结论完全相同的唯一一种情况。
// 这是因为经由member selection operators（.）对一个静态数据成员进行存取操作只是语法上的一种便于行事而已。
// 成员并不在对象之中，因此存取静态成员并不需要通过对象。
// 如果chunkSize是从一个复杂继承关系中继承而来的成员，也是一样的，只有唯一一个实体，而其存取路径仍然是那么直接。

// 静态数据成员的存取是经由函数调用（或其他某些语法）而被存取：
// foobar().chunkSize = 250;
// C++标准明确要求foobar()必须被求值，虽然其结果并无用处。

// 若取一个静态数据成员的地址，会得到一个指向其数据类型的指针，
// 而不是一个指向其类成员的指针，因为静态成员并不在对象中。
// &Point3d::chunkSize;  // 会得到const int*类型的内存地址

// 如果两个类声明了同名的静态成员，那么当它们放在data segment时，就会导致名称冲突。
// 编译器的解决方法是暗中对每一个静态数据成员编码（name-mangling)，以获得一个独一无二的程序识别代码。
// 每个编译器的name-mangling方法都不同，通常不外乎是表格、语法措辞等等。
// 任何name-mangling做法都要两个要点：
// 1.一种算法，推导出独一无二的名称。
// 2.万一编译系统（或环境工具）必须和使用者交谈，那些独一无二的名称可以轻易被推倒回到原来的名称。

// Nonstatic Data Members
// Nonstatic Data Members直接存放在每一个对象中。
// 除非经由明确的或暗喻的对象，没有办法直接存取它们。
// 只要程序员在一个成员函数中直接处理一个非静态数据成员，所谓implicit class object就会发生。
// Point3d Point3d::translate(const Point3d& pt) {
//   x += pt.x;
//   y += pt.y;
//   z += pt.z;
// }
// 看上去是对于x，y，z的直接存取，事实上是经由一个implicit class object（由this指针表达）完成。
// 事实上这个函数的参数是：
// Point3d Point3d::translate(Point3d *const this, const Point3d& pt) {
//   this->x += pt.x;
//   this->y += pt.y;
//   this->z += pt.z;
// }

// 欲对一个非静态数据成员进行存取操作，编译器需要把对象的起始地址加上数据成员的偏移量（offset）：
// origin._y = 0.0;  // &origin + (&Point3d::_y - 1);
// 指向数据成员的指针，其offset总是被加上1，这样可以使编译系统区分出一个指向数据成员的指针，
// 用以指出类的第一个成员和一个指向数据成员的指针，没有指出任何成员两种情况。
// 每一个非静态数据成员的偏移量（offset）在编译时期即可获知，
// 甚至如果成员属于一个base class subobject（派生自单一或多重继承链）也是一样。
// 因此，存取一个非静态数据成员，其效率和存取一个C结构体或一个nonderived class的member是一样的。

// 虚拟继承将为经由base class subobject存取class members导入一层新的间接性
// Point3d *pt3d;
// pt3d->_x = 0.0;
// 当_x是一个结构体成员、一个类成员、单一继承、多重继承的情况下都完全相同。
// 但如果_x是一个virtual base class的member，存取速度会比较慢一点。
// 从对象和指针存取成员有什么重大的差异？
// 当类是一个派生类，而在其继承结构中有一个虚拟基类，
// 并且被存取的成员是一个从虚基类继承而来的成员时，就会有重大的差异。
// 此时不能确定指针指向哪一种类型，因此就不知道编译时期这个成员真正的offset位置。
// 所以这个存取操作必须延迟至执行期，经由一个额外的间接导引，才能解决。
// 如果使用对象，就不会有这些问题，其类型是确定的，所以offset位置也在编译时期就固定了。
// 一个积极进取的编译甚至可以静态地由对象就解决掉对x的存取。

// 3.4 继承与Data Member

// 派生类等于派生类自己的成员加上基类成员的总和。
// 派生类和基类成员的排列次序并未在C++标准中强制指定，理论上编译器可以自由安排。
// 大部分编译器，基类成员总是先出现，虚基类除外。
struct Point2d_1 {
  float x, y;
} pt2d_1;
struct Point3d_1 {
  float x, y, z;
} pt3d_1;

void func3() {
  cout << &pt2d_1.x << endl;
  cout << &pt2d_1.y << endl;
  // 0x601198
  // 0x60119c
  cout << &pt3d_1.x << endl;
  cout << &pt3d_1.y << endl;
  cout << &pt3d_1.z << endl;
  // 0x6011a0
  // 0x6011a4
  // 0x6011a8
}

// 只有继承不要多态
class Point2d_2 {
 public:
  Point2d_2(float x = 0.0, float y = 0.0) : _x(x), _y(y){};
  float x() const { return _x; }
  float y() const { return _y; }
  void x(float newX) { _x = newX; }
  void y(float newY) { _x = newY; }
  void operator+=(const Point2d_2& rhs) {
    _x += rhs.x();
    _y += rhs.y();
  }
  void print() {
    cout << "Point2d_2 x:" << &_x << endl;
    cout << "Point2d_2 y:" << &_y << endl;
  }

 protected:
  float _x, _y;
};
class Point3d_2 : public Point2d_2 {
 public:
  Point3d_2(float x = 0.0, float y = 0.0, float z = 0.0)
      : Point2d_2(x, y), _z(z){};
  float z() const { return _z; }
  void z(float newZ) { _z = newZ; }
  void operator+=(const Point3d_2& rhs) {
    Point2d_2::operator+=(rhs);
    _z += rhs.z();
  }
  void print() {
    Point2d_2::print();
    cout << "Point3d_2 z:" << &_z << endl;
  }

 protected:
  float _z;
};

void func4() {
  Point3d_2 d2(1.0, 2.0, 3.0);
  d2.print();
  // Point2d_2 x:0x7ffe18a04560
  // Point2d_2 y:0x7ffe18a04564
  // Point3d_2 z:0x7ffe18a04568
}
// 这样设计的好处就是可以把管理x和y的坐标的程序代码局部化，还可以明显表现出两个抽象类之间的紧密关系。
// 把两个原本独立不相干的类凑成一对，并带有继承关系，会有什么易犯的错误呢？
// 1.经验不足的人可能会重复设计一些相同操作的函数。
// 2.把一个类分解为两层或更多层，有可能会为了表现类体系的抽象化而膨胀所需空间。
//   C++保证出现在派生类中的基类subobject有其完整原样性，这是重点。

class Concrete {
  char c1, c2, c3;
  int val;

 public:
  void print() {
    cout << "Concrete c1:" << static_cast<const void*>(&c1) << endl;
    cout << "Concrete c2:" << static_cast<const void*>(&c2) << endl;
    cout << "Concrete c3:" << static_cast<const void*>(&c3) << endl;
    cout << "Concrete val:" << static_cast<const void*>(&val) << endl;
  }
};
class Concrete1 {
  char bit1;
  int val;

 public:
  void print() {
    cout << "Concrete1 bit1:" << static_cast<const void*>(&bit1) << endl;
    cout << "Concrete1 val:" << &val << endl;
  }
};
class Concrete2 : public Concrete1 {
  char bit2;

 public:
  void print() {
    Concrete1::print();
    cout << "Concrete2 bit2:" << static_cast<const void*>(&bit2) << endl;
  }
};
class Concrete3 : public Concrete2 {
  char bit3;

 public:
  void print() {
    Concrete2::print();
    cout << "Concrete3 bit3:" << static_cast<const void*>(&bit3) << endl;
  }
};
void func5() {
  cout << sizeof(Concrete) << endl;   // 8
  cout << sizeof(Concrete3) << endl;  // 12
  Concrete c1;
  c1.print();
  // Concrete c1:0x7fffffc24410
  // Concrete c2:0x7fffffc24411
  // Concrete c3:0x7fffffc24412
  // Concrete val:0x7fffffc24414
  Concrete3 c3;
  c3.print();
  // Concrete1 bit1:0x7fffffc24430
  // Concrete1 val:0x7fffffc24434
  // Concrete2 bit2:0x7fffffc24438
  // Concrete3 bit3:0x7fffffc24439
}
// 如果不保证出现在派生类中的基类subobject有其完整原样性，会怎么样？
// Concrete2* pc2;
// Concrete1 *pc1_1, *pc1_2;  // 都可以指向前面的三种对象
// *pc1_2 = *pc1_1;  // 应该执行一个默认的memberwise复制操作，复制Concrete1的那一部分
// 如果pc1_1实际指向一个Concrete2或Concrete3对象，则上述操作应该复制内容给Concrete1 subobject。
// 如果没有保持派生类中的基类subobject的完整原样性，
// 把bit2和bit3和Concrete1 subobject捆绑在一起，去填补空间。
// 那么Concrete2或Concrete3对象的bit2或bit3会被Concrete1中的补位给覆盖掉。

// 加上多态
class Point2d_3 {
 public:
  Point2d_3(float x = 0.0, float y = 0.0) : _x(x), _y(y){};
  float x() const { return _x; }
  float y() const { return _y; }
  virtual float z() const { return 0.0; }
  void x(float newX) { _x = newX; }
  void y(float newY) { _x = newY; }
  virtual void z(float) {}
  virtual void operator+=(const Point2d_3& rhs) {
    _x += rhs.x();
    _y += rhs.y();
  }
  void print() {
    cout << "Point2d_3 x:" << &_x << endl;
    cout << "Point2d_3 y:" << &_y << endl;
  }

 protected:
  float _x, _y;
};
class Point3d_3 : public Point2d_3 {
 public:
  Point3d_3(float x = 0.0, float y = 0.0, float z = 0.0)
      : Point2d_3(x, y), _z(z){};
  float z() const { return _z; }
  void z(float newZ) { _z = newZ; }
  void operator+=(const Point2d_3& rhs) {
    Point2d_3::operator+=(rhs);
    _z += rhs.z();
  }
  void print() {
    Point2d_3::print();
    cout << "Point3d_3 z:" << &_z << endl;
  }

 protected:
  float _z;
};
void foo(Point2d_3& p1, Point2d_3& p2) {
  p1 += p2;
  // p1和p2可能是2d也可能是3d坐标点，这并不是先前任何设计所能支持的。
  // 这样的弹性，正是面向对象设计的中心。
  // 支持这样的弹性，势必给2d类带来空间和存取时间的额外负担：
  // 1.导入一个二和2d有关的虚函数表，用来存放它所声明的每一个虚函数地址。
  //   这个表的元素数目一般而言是被声明的虚函数的数目，
  //   再加上一个或两个slots（用以支持runtime type identification）。
  // 2.在每一个对象中导入一个vptr，提供执行期的链接，使每一个对象能够找到对应virtual table。
  // 3.加强构造函数，使它能够为vptr设定初值，让它指向类所对应的virtual table。
  //   这可能意味着在派生类和每一个基类的构造函数中，重新设定vptr的值。
  //   其情况视编译器的优化的积极性而定。
  // 4.加强构造函数，使它能够抹消“指向类的相关virtual table”的vptr。
  //   要知道，vptr很可能已经在派生类析构函数中被设定为派生类的virtual table地址。
  //   析构函数的调用次序是反向的：从派生类到基类。
  //   一个积极的优化编译器可以压抑那些大量的指定操作。

  // 把vptr放到哪里在最好？
  // 在C++最初问世时，把vptr放在对象的尾端，可以保留基类C struct的对象布局。
  // 因而允许在C程序代码中也能使用。
  // 到了C++2.0，开始支持虚拟继承以及抽象基类，并且由于面向对象典范的兴起，某些编译器开始把vptr放到对象的开头。
  // 把vptr放在对象的前端，对于在多重继承之下，通过指向成员的指针调研虚函数，会带来一些帮助。
  // 否则，不仅从对象起点开始量起的offset必须在执行期备妥，甚至与类vptr之间的offset也必须备妥。
  // 也丧失了C语言的兼容性。
}
void func6() {
  Point2d_3 p2d3(2.1, 2.2);
  Point3d_3 p3d3(3.1, 3.2, 3.3);
  p3d3 += p2d3;
  foo(p3d3, p2d3);
}

// 多重继承
// 单一继承提供了一种自然多态形式，是关于类体系中的基类和派生类之间的转换。
// 基类和派生类都是从相同的地址开始，差异只是派生类对象比较大。
// 把派生类对象指给基类（不管继承深度有多深）的指针或引用，该操作并不需要编译器去调停或修改地址。
// 很自然地发生，而且提供了最佳执行效率。
// 把vptr放到对象的起始处，如果基类没有虚函数而派生类有，那么单一继承的自然多态就会被打破。
// 这种情况下，把一个派生类对象转换为其基类类型，就需要编译器的介入，用以调整地址（因为vptr插入的缘故）。
// 在既是多重继承又是虚拟继承的情况下，编译器的介入更有必要。
// 多重继承既不是单一继承，也不容易定义其模型。
// 其复杂度在于派生类和其上一个基类乃至于上上一个基类之间的非自然关系。
class Point2d_4 {
 public:
  void print() {
    cout << "Point2d_4 x: " << &x << " " << sizeof(x) << endl;
    cout << "Point2d_4 y: " << &y << " " << sizeof(y) << endl;
  }

 protected:
  float x, y;
};
class Point3d_4 : public Point2d_4 {
 public:
  void print() {
    Point2d_4::print();
    cout << "Point3d_4 z: " << &z << " " << sizeof(z) << endl;
  }

 protected:
  float z;
};
class Vertex {
 public:
  void print() {
    cout << "Vertex next: " << &next << " " << sizeof(next) << endl;
  }

 protected:
  Vertex* next;
};
class Vertex3d : public Point3d_4, public Vertex {
 public:
  void print() {
    Point3d_4::print();
    Vertex::print();
    cout << "V3d mumbles: " << &mumbles << " " << sizeof(mumbles) << endl;
  }

 protected:
  float mumbles;

  // C++标准并未要求Point3d_4和Vertex有特定的排列次序。
  // 如果要存取第二个（或后继）基类中的数据成员，需要付出额外的成本么？
  // 不，成员的位置在编译时就固定了，因此存取成员只是一个简单的offset运算，
  // 就像单一继承一样简单，不管是经由指针、引用或是对象来存取。
};
// 多重继承的问题主要发生于派生类对象和其第二或后继的基类对象之间的转换。
// 不论是直接转换还是经由其所支持的虚函数机制做转换。
void mumble(const Vertex&) {}
// 对一个多重派生类对象，将地址指定给最左端（第一个）基类的指针，
// 情况和单一继承时相同，因为二者都指向相同的起始地址，付出的成本只有地址的指定操作而已。
// 至于第二个或后继的基类的地址指定操作，则需要将地址修改：
// 加上（或减去，如果downcast的话）介于中间的基类subobjcet大小。
void func7() {
  Vertex3d v;
  mumble(v);

  cout << sizeof(float) << endl;      // 4
  cout << sizeof(Point2d_4) << endl;  // 8
  cout << sizeof(Point3d_4) << endl;  // 12
  cout << sizeof(Vertex) << endl;     // 8
  cout << sizeof(Vertex3d) << endl;   // 32
  Vertex3d v3d;
  cout << "v3d addr   : " << &v3d << endl;
  v3d.print();
  // v3d addr   : 0x7fff432f7430
  // Point2d_4 x: 0x7fff432f7430 4
  // Point2d_4 y: 0x7fff432f7434 4
  // Point3d_4 z: 0x7fff432f7438 4
  // Vertex next: 0x7fff432f7440 8
  // V3d mumbles: 0x7fff432f7448 4

  Vertex3d* pv3d;
  Vertex* pv;
  Point2d_4* p2d;
  Point3d_4* p3d;
  pv = &v3d;
  cout << pv << endl;  // 0x7fff432f7440
  // 需要这样的内部转化：
  Vertex* pv1 = (Vertex*)(((char*)&v3d) + sizeof(Point3d_4));  // 转换的不对啊!
  cout << pv1 << endl;                                         // 0x7fff432f743c
  p2d = &v3d;  // 简单地拷贝地址就行了
  p3d = &v3d;  // 简单地拷贝地址就行了
  pv = pv3d;
  // 转换为(对于引用，则不需要针对可能的0值做防卫，因为引用不可能参考到无物)：
  pv = pv3d ? (Vertex*)(((char*)&v3d) + sizeof(Point3d_4)) : 0;
}

// 虚拟继承
// 多重继承的一个语意上的副作用就是，它必须支持某种形式的share subobject继承。
// 一个典型的例子是iostream library。
// 如果类内含一个或多个虚拟基类subobjects，那么将被分割为两部分：一个不变局部和一个共享局部。
// 不变局部中的数据，不管后继如何变化，总是拥有固定的offset（从对象的开头算起），
// 所以这一部分的数据可以直接存取。
// 至于共享局部，其位置会因为每次的派生操作而有变化，所以它们只能被间接存取。
// 各家编译器实现技术之间的差异就在于间接存取的方法不同，有三种主流策略。
class Point2d_5 {
 public:
  void print() {
    cout << "Point2d_5 x: " << &x << " " << sizeof(x) << endl;
    cout << "Point2d_5 y: " << &y << " " << sizeof(y) << endl;
  }

 protected:
  float x, y;
};
class Point3d_5 : public virtual Point2d_5 {
 public:
  void print() {
    Point2d_5::print();
    cout << "Point3d_5 z: " << &z << " " << sizeof(z) << endl;
  }

 protected:
  float z;
};
class Vertex1 : public virtual Point2d_5 {
 public:
  void print() {
    Point2d_5::print();
    cout << "Vertex next: " << &next << " " << sizeof(next) << endl;
  }

 protected:
  Vertex1* next;
};
class Vertex3d1 : public Point3d_5, public Vertex1 {
 public:
  void print() {
    Point3d_5::print();
    Vertex1::print();
    cout << "Vertex3d mumbles: " << &mumbles << " " << sizeof(x) << endl;
  }

 protected:
  float mumbles;
};
// 一般的布局策略是先安排好派生类的不变部分，然后再建立其共享部分，然而，存在一个问题：
// 如何能够存取类中的共享部分呢？
// cfront编译器在每一个派生类对象中安插一些指针，每个指针指向一个虚拟基类。
// 通过这个指针间接存取虚基类的成员。
// 这样的实现模型有两个主要的缺点：
// 1.每一个对象必须针对其每一个虚基类背负一个额外的指针，
//   理想上我们希望对象有固定的负担，不因为虚基类的数目而变化。
// 2.由于虚继承链的加长，导致间接存取层次的增加。
//   理想上希望有固定的存取时间，不因为虚基类的深度而改变。
// MetaWare和其他编译器到今天仍然使用cfront的原始实现模型来解决第二个问题：
// 1.把经由拷贝操作取得得所有的nested virtual base class指针，放到派生类对象中。
// 2.这就解决了固定存取时间的问题，虽然付出了一些空间上的代价。
// 至于第一个问题，一般有两个解决方法：
// 1.microsoft编译器引入所谓的virtual base class table。
//   每一个对象如果有一个或多个虚拟基类，就会有编译器安插一个指针，
//   指向virtual base class table，至于真正的virtual base class指针，就被放在该表格中。
// 2.在virtual function table中放置virtual base class的offset，而不是地址。
// 一般而言，virtual base class最有效的一种运用形式就是：
// 一个抽象的virtual base class，没有任何数据成员。

void func8() {
  cout << sizeof(Point2d_5) << endl;  // 8
  cout << sizeof(Point3d_5) << endl;  // 24
  cout << sizeof(Vertex1) << endl;    // 24
  cout << sizeof(Vertex3d1) << endl;  // 48
  Vertex3d1 vv;
  cout << &vv << endl;  // 0x7ffdef10f300
  vv.print();
  // Point2d_5 x: 0x7ffdef10f324 4
  // Point2d_5 y: 0x7ffdef10f328 4
  // Point3d_5 z: 0x7ffdef10f308 4
  // Point2d_5 x: 0x7ffdef10f324 4
  // Point2d_5 y: 0x7ffdef10f328 4
  // Vertex next: 0x7ffdef10f318 8
  // Vertex3d mumbles: 0x7ffdef10f320 4
}

// 3.5 对象成员的效率

// 如果把优化开关打开，封装就不会带来执行期的效率成本，使用inline存取函数也是。
// 如果没有把优化开关打开，就很难猜测一个程序的效率表现，
// 因为程序代码潜在性地受到专家所谓的一种奇行怪癖...与也特定编译器有关的魔咒影响。

// 单一继承应该不会影响测试的效率，因为成员被连续存储与派生类对象中，并且其offset在编译期就已知了。
// 多重继承的情况下应该也是相同的。
// 虚拟继承的效率令人失望。

// 3.6 指向Data Members的指针

// 指向data members的指针，是一个有点神秘但颇有用处的语言特性，
// 特比是如果需要详细调查成员的底层布局的话。
// 可以用来决定vptr是放在类的起始处或是尾端，也可以用来决定类中访问段的次序。
class Point3d_6 {
 public:
  virtual ~Point3d_6() {}
  void fn() {
    // 取一个非静态数据成员的地址，将会得到它在类中的offset；
    // 取一个绑定于真正对象身上的数据成员的地址，将会得到该成员在内存中的真正地址。
    // 过去的编译器会在每个偏移值上加1么？
    std::printf("%p, %p\n", &Point3d_6::x, &(Point3d_6::x));
    std::printf("%p, %p\n", &Point3d_6::y, &(Point3d_6::y));
    std::printf("%p, %p\n", &Point3d_6::z, &(Point3d_6::z));
    std::printf("%p, %p\n", &origin.x, &(origin.x));
    std::printf("%p, %p\n", &origin.y, &(origin.y));
    std::printf("%p, %p\n", &origin.z, &(origin.z));

    // 为啥不行？
    cout << "Point3d_6 x: " << &Point3d_6::x << endl;  // 1
    cout << "Point3d_6 y: " << &Point3d_6::y << endl;  // 1
    cout << "Point3d_6 z: " << &Point3d_6::z << endl;  // 1
  }

 protected:
  static Point3d_6 origin;

 public:
  float x, y, z;
};
Point3d_6 Point3d_6::origin;
// 每个对象含有三个坐标，依次为x，y，z，以及一个vptr。
// 静态成员，放在对象之外，唯一随编译器不同而不同的是vptr的位置。
// C++标准允许vptr被放在对象中的任何位置：起始处、尾端，或是在各个成员之间。
// 实际上，所有编译器不是放在对象头部，就是放在对象尾部。
// &Point3d_6::z;
// 上面的取地址操作将得到z坐标在对象中的偏移量。
// 最少也是x和y的大小的总和，因为C++要求用一个access level中的成员的排列次序应该和声明次序相同。
void func9() {
  Point3d_6 dd6;
  dd6.fn();
  // 0x8, 0x7fff7ac65498
  // 0xc, 0x7fff7ac6549c
  // 0x10, 0x7fff7ac654a0
  // 0x6032f8, 0x6032f8
  // 0x6032fc, 0x6032fc
  // 0x603300, 0x603300
  // Point3d_6 x: 1
  // Point3d_6 y: 1
  // Point3d_6 z: 1

  // 如何区分一个没有指向任何数据成员的指针和一个指向第一个数据成员的指针？
  float Point3d_6::*p1 = 0;  // Point3d_6::*是指向Point3d_6数据成员的指针类型
  float Point3d_6::*p2 = &Point3d_6::x;
  if (p1 == p2) {
    // 每一个真正的成员offset值都被加上1，编译器和使用者都必须在使用前，先减掉1。
  }
}

// 在多重继承下，如果要将第二个（或后继）base class的指针和一个与派生类对象绑定的成员结合起来，
// 那么将会因为“需要加入offset值”而变得相当复杂。
struct Base1 {
  int val1;
};
struct Base2 {
  int val2;
};
struct Derived : Base1, Base2 {};
void func1(int Derived::*dmp, Derived* pd) {
  // 期望第一个参数得到的是一个指向派生类成员的指针。
  // 如果传入一个指向基类成员的指针会怎么样呢？
  pd->*dmp;
}
void func2(Derived* pd) {
  int Base2::*bmp = &Base2::val2;  // bmp将成为1，但是在派生类中，val2==5
  // 当bmp被作为func1的第一个参数时，它的值就必须因介入的Base1 class的大小而调整，
  // 否则func1中将取到Base1::val1，而非程序员所期望的Base::val2。
  // 要解决这个问题，必须经由编译器内部转换：
  // func1(bmp ? bmp + sizeof(Base1):0,pd);
  func1(bmp, pd);
}
void func10() {
  printf("&Base1::val1 = %p\n", &Base1::val1);
  printf("&Base2::val2 = %p\n", &Base2::val2);
  printf("&Derived1::val1 = %p\n", &Derived::val1);
  printf("&Derived2::val1 = %p\n", &Derived::val2);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 9]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    case 3:
      func4();
      break;
    case 4:
      func5();
      break;
    case 5:
      func6();
      break;
    case 6:
      func7();
      break;
    case 7:
      func8();
      break;
    case 8:
      func9();
      break;
    case 9:
      func10();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
