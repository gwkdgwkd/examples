#include <iostream>

// Data语义学

namespace n1 {
// B,D1,D2,DD中没有任何一个类内含明显的数据，其间只表示了继承关系。
class B {
  // 事实上并不是空的，它有一个隐晦的1字节，
  // 那是被编译器安插进取的一个char。
  // 这使得这个类的两个对象得以在内存中配置独一无二的地址。
};
class D1 : public virtual B {
  // 大小和机器有关，也和编译器有关，大小受到三个因素的影响：
  // 1.语言本身所造成的额外负担
  //   当语言支持虚基类时，就会导致一些额外负担。
  //   在派生类中，这个额外负担反映在某种形式的指针身上，
  //   它或者指向virtual base class subobject，或者指向一个表格，
  //   其中存放的若不是virtual base class subobject地址，就是其偏移量。
  // 2.编译器对于特殊情况所提供的优化处理
  //   virtual base class B subobject的1字节大小也出现在D1和D2身上。
  //   传统上它被放在派生类的固定（不变动）部分的尾端。
  //   某些编译器会对empty virtual base class提供特殊支持。
  //   empty virtual base class已经成为C++ OO设计的一个特有术语了。
  //   它提供了一个虚拟接口，没有定义任何数据，
  //   某些新的编译器对此提供了特殊处理。
  //   在这个策略下，empty virtual base class被视为派生类最开头的一部分，
  //   也就是说它并没有花费任何额外的空间。
  //   节省了1字节，只剩下第1点说说的额外负担了。
  // 3.Alignment的限制
  //   D1和D2的大小截止目前为9字节。
  //   大部分机器上，结构体的大小会受到alignment的限制，
  //   是它们能够更有效率地在内存中存取。
};
class D2 : public virtual B {
  // 同D1
};
class DD : public D1, public D2 {
  // 一个virtual base class subobject只会在派生类中存在一份实体，
  // 不管它在类继承体系中出现了多少次。
  // DD的大小由以下几点决定：
  // 1.被大家共享的唯一一个B实体，大小为1字节。
  // 2.基类D1的大小，减去B的大小，结果是8字节，
  //   D2的算法相同，加起来是16字节。
  // 3.DD自己的大小，0字节。
  // 4.DD的alignment数量（如果有的话）。
  //   前三项的和为17，按4字节对齐，补足3字节，结果是20字节。
  // 对empty virtual base class特殊处理的编译器，
  // 拿掉了1字节和额外的3字节，A的大小是16字节。
};
void func() {
  // 所以有人认为每一个类的大小都应该是0，不对的，即使是类B的大小也不为0：
  std::cout << "B sizeof: " << sizeof(B) << std::endl;    // 1
  std::cout << "D1 sizeof: " << sizeof(D1) << std::endl;  // 8
  std::cout << "D2 sizeof: " << sizeof(D2) << std::endl;  // 8
  std::cout << "DD sizeof: " << sizeof(DD) << std::endl;  // 16
}

// C++标准并不强制规定下面的琐碎细节，如：
// 1.base class subobjects的排列次序；
// 2.不同存取层级的数据成员的排列次序。
// 也不规定虚函数或虚基类的实现细节，标准只说：那些细节有各厂商自定。
// C++对象模型尽量以空间优化和存取速度优化的考虑来表现非静态数据成员，
// 并保持和C语言struct数据配置的兼容性。
// 它把数据直接放在每一个对象中，对于继承而来的非静态数据成员也是如此，
// 不管是virtual或non virtual base class。
// 不过并没有强调定义其间的排列顺序。
// 至于静态数据成员，则被放置在程序的一个全局数据段中，不会影响个别的对象大小。
// 在程序中，不管该类被产生出多少个对象（直接产生或间接派生），
// 静态数据成员永远存在一份实体，甚至即使该类没有任何对象实体，
// 其静态数据成员也已存在。
// 但是一个模板类的静态数据成员的行为稍有不同。
// 每一个对象必须有足够的大小以容纳它所有的非静态数据成员，
// 有时候其值比想象的大，因为：
// 1.有编译器自动加上的额外数据成员，
//   用以支持某些语言特性（主要是各种virtual特性）。
// 2.因为alignment的需要。
}  // namespace n1

namespace n2 {
// 3.1 数据成员的绑定

float x = 1.0f;
namespace test1 {
class A {
 public:
  float getX() { return x; }
  void setX(float f) { x = f; }

 private:
  float x = 2.0f;
};
void func() {
  A a;
  std::cout << a.getX() << std::endl;  // 2
  a.setX(3.0f);
  std::cout << a.getX() << std::endl;  // 3
}
}  // namespace test1

// n1中类A的getX和setX函数，操作的都是类A的成员变量x，
// 在C++最早的编译器上，两个函数操作的都是向全局的x。
// 这样的绑定结果几乎遍地都是，但不符合大家的预期，
// 并因此导出早期C++的两种防御性程序设计风格：
// 1.把所有的数据成员放在类声明起头处，以确保正确的绑定；
namespace test2 {
class A {
  float x = 2.0f;

 public:
  float getX() { return x; }
  void setX(float f) { x = f; }
};
}  // namespace test2
// 2.把所有的内联函数，不管大小都放在类声明之外。
namespace test3 {
class A {
 public:
  float getX();
  void setX(float f);

 private:
  float x = 2.0f;
};
float A::getX() { return x; }
void A::setX(float f) { x = f; }
}  // namespace test3
}  // namespace n2
namespace test4 {
// n2，n3这些程序设计风格事实上到今天还存在，
// 虽然它们的必要性已经从C++2.0（C++11）之后就消失了。
// 这个古老的语言规则被称为member rewriting rule，
// 大意是一个内联函数实体，在整个类声明未被完全看见之前，是不会被评估求值的。
// C++标准以member scope resolution rules来精炼这个rewriting rule，
// 其效果是，如果一个内联函数在类声明之后立刻被定义的话，那么就还是对其评估求值。
// 然而，对于成员函数的参数列表并不为真，
// 参数列表中的名称还是会在它们第一次遭遇时被适当地决议完成。
// 因此在extern和nested type names之间的非直觉绑定操作还是会发生。
class A {
 public:
  // length必须在本类对它的第一个参数操作之前被看见：
  typedef int length;
  length getX() { return x; }
  void setX(length val) { x = val; }  // length被决议为global

 private:
  // typedef float length; // 如果声明到这里，则报错
  // 对于这种情况，仍然需要某种防御性程序风格，
  // 请始终把nested type声明放在类的起始处。
  length x;
};
}  // namespace test4

namespace n3 {
// 3.2 Data Member的布局

// 非静态数据成员在对象中的排列顺序将和其被声明的顺序一样，
// 任何中间介入的静态数据成员都不会放进对象布局之中。
// C++标准要求，在同一个访问段（private、public、protected）中，
// 成员的排序只需符合较晚出现的成员在对象中有较高的地址这一条即可。
// 也就是说，各个成员并不一定得连续排序。
// 成员的边界调整可能需要填补一些字节，这些位于成员之间。
// 编译器还可能合成一些内部使用的数据成员，以支持整个对象模型，比如vptr。
// 它会放在什么位置呢？
// 传统上被放在所有明确声明的成员的最后，也有一些会放在对象的最前端。
// C++标准秉承先前所说的对于布局持的放任态度，
// 允许编译器把那些内部产生出来的members自由放在任何位置上，
// 甚至那些被程序员声明出来的成员之间。
// C++标准也允许编译器将多个访问段之中的数据成员自由排列，
// 不必在乎他它们出现在类中声明中的次序。
// 各家编译器都是把一个以上的访问段连接在一起，依照声明的次序成为一个连续区块。
// 访问段的多少并不会招来额外负担。
class A {
 public:
  float x;

 public:
  float y;

 public:
  float z;
};
union Cmp {
  float A::*mem;
  long offset;
};
template <class class_type, class data_type1, class data_type2>
const char* asscess_order(data_type1 class_type::*mem1,
                          data_type2 class_type::*mem2) {
  // 接受两个数据成员，然后判断谁先出现在类对象中：
  Cmp cmp1 = {mem1};
  Cmp cmp2 = {mem2};
  return cmp1.offset < cmp2.offset ? "member 1 occurs first"
                                   : "member 2 occurs first";
}
void func() {
  std::cout << asscess_order(&A::x, &A::y) << std::endl;
  // member 1 occurs first

  std::cout << asscess_order(&A::z, &A::y) << std::endl;
  // member 2 occurs first
}
}  // namespace n3

namespace n4 {
// 3.3 Data Member的存取

namespace test1 {
// Static Data Members
// 被编译器提出于类之外，并被视为一个全局变量，但只在class生命范围之内可见。
// 并不会导致空间或运行时间的额外负担，不论是在对象或是静态数据成员本身。
// 每一个静态数据成员只有一个实体，存放在程序的data segment之中。
// 每次程序读静态成员时，就会被内部转化为对该唯一的extern实体的直接操作。
class A {
 public:
  static int s;
  int i;
};
int A::s;
A test() { return A(); }
void func() {
  A a;
  A* p = &a;
  a.s = 6;
  p->s = 7;
  A::s = 8;
  // C++中通过指针和通过对象来存取member，结论完全相同的唯一一种情况。
  // 这是因为经由.对一个静态数据成员进行存取操作只是语法上的一种便于行事而已。
  // 成员并不在对象之中，因此存取静态成员并不需要通过对象。

  // 静态数据成员的存取是经由函数调用（或其他某些语法）而被存取，
  // C++标准明确要求foobar()必须被求值，虽然其结果并无用处：
  test().s = 250;

  // 若取一个静态数据成员的地址，会得到一个指向其数据类型的指针，
  // 而不是一个指向其类成员的指针，因为静态成员并不在对象中：
  int* ptr;
  std::cout << typeid(ptr).name() << std::endl;    // Pi
  std::cout << typeid(&A::s).name() << std::endl;  // Pi
  std::cout << typeid(&A::i).name() << std::endl;  // MN2n41AEi
}
// 如果两个类声明了同名的静态成员，
// 那么当它们放在data segment时，就会导致名称冲突。
// 编译器的解决方法是暗中对每一个静态数据成员编码（name-mangling)，
// 以获得一个独一无二的程序识别代码。
// 每个编译器的name-mangling方法都不同，通常不外乎是表格、语法措辞等等。
// 任何name-mangling做法都要两个要点：
// 1.一种算法，推导出独一无二的名称;
// 2.如果编译系统（或环境工具）必须和程序员交互，
//   那些独一无二的名称可以轻易被推倒回到原来的名称。
}  // namespace test1

namespace test2 {
// Nonstatic Data Members

// Nonstatic Data Members直接存放在每一个对象中。
// 除非经由明确的或暗喻的对象，没有办法直接存取它们。
class A {
 public:
  int get() {
    // 只要程序员在一个成员函数中直接处理一个非静态数据成员，
    // 所谓implicit class object（this指针）就会发生：
    return i;
    // 看上去是对i的直接读取，事实上经由this指针完成：
    return this->i;
  }
  int i;
};

// 欲对一个非静态数据成员进行存取操作，
// 编译器需要把对象的起始地址加上数据成员的偏移量（offset）：
// a.i = 0;  // &a + (&A::i - 1);
// 指向数据成员的指针，其offset总是被加上1，
// 这样可以使编译系统区分出以下两种情况：，
// 1.一个指向data member的指针，用以指出class的第一个member；
// 2.一个指向data member的指针，没有指出任何member。
// 每一个非静态数据成员的偏移量（offset）在编译时期即可获知，
// 因此，存取一个非静态数据成员，
// 其效率和存取一个C结构体或一个nonderived class的member是一样的，
// 哪怕成员派生自单一或多重继承链也是一样的。

// 虚拟继承将经由base class subobject存取class members导入新的间接性：
// A *ptr;
// ptr->x = 0;
// 当x是一个结构体成员、一个类成员、单一继承、多重继承的情况下都完全相同。
// 但如果x是一个virtual base class的member，存取速度会比较慢一点。
// 从对象和指针存取成员有什么重大的差异？
// 当类是一个派生类，而在其继承结构中有一个虚拟基类，
// 并且被存取的成员是一个从虚基类继承而来的成员时，就会有重大的差异。
// 此时不能确定指针指向哪一种类型，
// 因此就不知道编译时期这个成员真正的offset位置。
// 所以这个存取操作必须延迟至执行期，经由一个额外的间接导引，才能解决。
// 如果使用对象，就不会有这些问题，其类型是确定的，
// 所以offset位置也在编译时期就固定了。
// 一个积极进取的编译甚至可以静态地由对象就解决掉对x的存取。
}  // namespace test2
}  // namespace n4

namespace n5 {
// 3.4 继承与Data Member

// 派生类等于派生类自己的成员加上基类成员的总和。
// 派生类和基类成员的排列次序并未在C++标准中强制指定，
// 理论上编译器可以自由安排。
// 大部分编译器，基类成员总是先出现，虚基类除外。

namespace test1 {
// 只有继承没有多态
class B {
 public:
  void print() {
    std::cout << "&B::x : " << &x << std::endl;
    std::cout << "&B::y : " << &y << std::endl;
  }

 private:
  int x, y;
};
class D : public B {
 public:
  void print() {
    B::print();
    std::cout << "&D::z : " << &z << std::endl;
  }

 private:
  int z;
};

void func1() {
  std::cout << "B size: " << sizeof(B) << std::endl;  // B size: 8
  std::cout << "D size: " << sizeof(D) << std::endl;  // D size: 12

  D d;
  d.print();
  // &B::x : 0x7ffd3fec8edc
  // &B::y : 0x7ffd3fec8ee0
  // &D::z : 0x7ffd3fec8ee4

  dynamic_cast<B*>(&d)->print();
  // &B::x : 0x7ffd3fec8edc
  // &B::y : 0x7ffd3fec8ee0
}
// 这样设计的好处就是可以把管理x和y的程序代码局部化，
// 还可以明显表现出两个类之间的紧密关系。
// 把两个原本独立不相干的类凑成一对，并带有继承关系，会有什么易犯的错误呢？
// 1.经验不足的人可能会重复设计一些相同操作的函数。
// 2.把一个类分解为两层或更多层，有可能会为了表现类体系的抽象化而膨胀所需空间。
//   C++保证出现在派生类中的基类subobject有其完整原样性，这是重点。

class C {
  char c1, c2, c3;
  int val;

 public:
  void print() {
    std::cout << "C c1:" << static_cast<const void*>(&c1) << std::endl;
    std::cout << "C c2:" << static_cast<const void*>(&c2) << std::endl;
    std::cout << "C c3:" << static_cast<const void*>(&c3) << std::endl;
    std::cout << "C val:" << static_cast<const void*>(&val) << std::endl;
  }
};
class C1 {
  char c1;
  int val;

 public:
  void print() {
    std::cout << "C1 c1:" << static_cast<const void*>(&c1) << std::endl;
    std::cout << "C1 val:" << &val << std::endl;
  }
};
class C2 : public C1 {
  char c2;

 public:
  void print() {
    C1::print();
    std::cout << "C2 c2:" << static_cast<const void*>(&c2) << std::endl;
  }
};
class C3 : public C2 {
  char c3;

 public:
  void print() {
    C2::print();
    std::cout << "C3 c3:" << static_cast<const void*>(&c3) << std::endl;
  }
};
void func2() {
  std::cout << sizeof(C) << std::endl;   // 8
  std::cout << sizeof(C3) << std::endl;  // 12
  C c;
  c.print();
  // C c1:0x7fff2bb724b4
  // C c2:0x7fff2bb724b5
  // C c3:0x7fff2bb724b6
  // C val:0x7fff2bb724b8
  C3 c3;
  c3.print();
  // C1 c1:0x7fff2bb724bc
  // C1 val:0x7fff2bb724c0
  // C2 c2:0x7fff2bb724c4
  // C3 c3:0x7fff2bb724c5

  // 如果不保证出现在派生类中的基类subobject有其完整原样性，会怎么样？
  C1 *pc1, *pc2;  // 都可以指向前面的三种对象
  *pc2 = *pc1;    // 执行默认的memberwise复制，复制C1那部分
  // 如果pc1实际指向一个C2或C3对象，则上述操作应该复制内容给C1 subobject。
  // 如果没有保持派生类中的基类subobject的完整原样性，
  // 把c2和c3和C1 subobject捆绑在一起，去填补空间。
  // 那么C2或C3对象的c2或c3会被C1中的补位给覆盖掉。
}
}  // namespace test1

namespace test2 {
// 继承加上多态
class B {
 public:
  B(int i, int j) : x(i), y(j) {}
  virtual int getZ() const { return 0.0; }
  virtual void operator+=(const B& rhs) {
    x += rhs.x;
    y += rhs.y;
  }
  virtual void print() {
    std::cout << "x : " << x << std::endl;
    std::cout << "y : " << y << std::endl;
  }

 private:
  int x, y;
};
class D : public B {
 public:
  D(int i, int j, int k) : B(i, j), z(k) {}
  virtual int getZ() const { return z; }
  virtual void operator+=(const B& rhs) {
    B::operator+=(rhs);
    z += rhs.getZ();
  }
  virtual void print() {
    B::print();
    std::cout << "z : " << z << std::endl;
  }

 private:
  int z;
};
void test(B& b1, B& b2) {
  b1 += b2;
  b1.print();

  // b1和b2可能是B也可能是D，这并不是先前任何设计所能支持的。
  // 这样的弹性，正是面向对象设计的核心。
  // 支持这样的弹性，势必给B类带来空间和存取时间的额外负担：
  // 1.导入一个和B有关的虚函数表，用来存放它所声明的每一个虚函数地址。
  //   这个表的元素数目一般而言是被声明的虚函数的数目，
  //   再加上一个或两个slots（用以支持runtime type identification）。
  // 2.在每一个对象中导入一个vptr，提供执行期的链接，
  //   使每一个对象能够找到对应virtual table。
  // 3.加强构造函数，使它能够为vptr设定初值，
  //   让它指向类所对应的virtual table。
  //   这可能意味着在派生类和每一个基类的构造函数中，重新设定vptr的值。
  //   其情况视编译器的优化的积极性而定。
  // 4.加强析构函数，使它能够抹消指向类的相关virtual table的vptr。
  //   vptr很可能已经在派生类析构函数中被设定为派生类的virtual table地址。
  //   析构函数的调用次序是反向的：从派生类到基类。
  //   一个积极的优化编译器可以压抑那些大量的指定操作。

  // 把vptr放到哪里在最好？
  // 在C++最初问世时，把vptr放在对象的尾端，可以保留基类C struct的对象布局。
  // 因而允许在C程序代码中也能使用。
  // 到了C++2.0，开始支持虚拟继承以及抽象基类，
  // 并且由于面向对象典范的兴起，某些编译器开始把vptr放到对象的开头。
  // 把vptr放在对象的前端，对于在多重继承之下，
  // 通过指向成员的指针调用虚函数，会带来一些帮助。
  // 否则，不仅从对象起点开始量起的offset必须在执行期备妥，
  // 甚至与类vptr之间的offset也必须备妥，也丧失了C语言的兼容性。
}
void func() {
  std::cout << "B size: " << sizeof(B) << std::endl;  // B size: 16
  std::cout << "D size: " << sizeof(D) << std::endl;  // D size: 24

  B b1(1, 1);
  B b2(2, 2);
  test(b1, b2);
  // x : 3
  // y : 3

  D d1(3, 3, 3);
  D d2(4, 4, 4);
  test(d1, d2);
  // x : 7
  // y : 7
  // z : 7
}
}  // namespace test2

namespace test3 {
// 多重继承

// 单一继承提供了一种自然多态形式，是关于类体系中的基类和派生类之间的转换。
// 基类和派生类都是从相同的地址开始，差异只是派生类对象比较大。
// 把派生类对象指给基类（不管继承深度有多深）的指针或引用，
// 该操作并不需要编译器去调停或修改地址。
// 很自然地发生，而且提供了最佳执行效率。
// 把vptr放到对象的起始处，如果基类没有虚函数而派生类有，
// 那么单一继承的自然多态就会被打破。
// 这种情况下，把一个派生类对象转换为其基类类型，
// 就需要编译器的介入，用以调整地址（因为vptr插入的缘故）。
// 在既是多重继承又是虚拟继承的情况下，编译器的介入更有必要。
// 多重继承既不是单一继承，也不容易定义其模型。
// 其复杂度在于派生类和其上一个基类乃至于上上一个基类之间的非自然关系。
class B1 {
 public:
  void print() {
    std::cout << "B1 x   : " << &x << " " << sizeof(x) << std::endl;
    std::cout << "B1 y   : " << &y << " " << sizeof(y) << std::endl;
  }

 protected:
  float x, y;
};
class D : public B1 {
 public:
  void print() {
    B1::print();
    std::cout << "D z    : " << &z << " " << sizeof(z) << std::endl;
  }

 protected:
  float z;
};
class B2 {
 public:
  void print() {
    std::cout << "B2 ptr : " << &ptr << " " << sizeof(ptr) << std::endl;
  }

 protected:
  B2* ptr;
};
class DD : public D, public B2 {
 public:
  void print() {
    D::print();
    B2::print();
    std::cout << "DD f   : " << &f << " " << sizeof(f) << std::endl;
  }

 protected:
  float f;

  // C++标准并未要求D和B2有特定的排列次序。
  // 如果要存取第二个（或后继）基类中的数据成员，需要付出额外的成本么？
  // 不，成员的位置在编译时就固定了，因此存取成员只是一个简单的offset运算，
  // 就像单一继承一样简单，不管是经由指针、引用或是对象来存取。
};
void test(B2& b2) {
  // 多重继承的问题主要发生于派生类对象和其第二或后继的基类对象之间的转换。
  // 不论是直接转换还是经由其所支持的虚函数机制做转换。
  b2.print();
  // 对一个多重派生类对象，将地址指定给最左端（第一个）基类的指针，
  // 情况和单一继承时相同，因为二者都指向相同的起始地址，
  // 付出的成本只有地址的指定操作而已。
  // 至于第二个或后继的基类的地址指定操作，则需要将地址修改：
  // 加上（或减去，如果downcast的话）介于中间的基类subobjcet大小。
}
void func() {
  std::cout << sizeof(float) << std::endl;  // 4
  std::cout << sizeof(B1) << std::endl;     // 8
  std::cout << sizeof(D) << std::endl;      // 12
  std::cout << sizeof(B2) << std::endl;     // 8
  std::cout << sizeof(DD) << std::endl;     // 32
  std::cout << alignof(DD) << std::endl;    // 8

  B2 b2;
  std::cout << "b2 addr: " << &b2 << std::endl;
  test(b2);
  // b2 addr: 0x7ffd88e0f660
  // B2 ptr : 0x7ffd88e0f660 8

  DD dd;
  std::cout << "dd addr: " << &dd << std::endl;
  test(dd);
  // dd addr: 0x7ffd88e0f690
  // B2 ptr : 0x7ffd88e0f6a0 8

  dd.print();
  // B1 x   : 0x7ffd88e0f690 4
  // B1 y   : 0x7ffd88e0f694 4
  // D z    : 0x7ffd88e0f698 4
  // B2 ptr : 0x7ffd88e0f6a0 8
  // DD f   : 0x7ffd88e0f6a8 4

  DD* pdd;
  B2* pb2;
  B1* pb1;
  D* pd;
  pb2 = &dd;
  std::cout << pb2 << std::endl;  // 0x7ffd88e0f6a0
  // 需要这样的内部转化：
  B2* ptr = (B2*)(((char*)&dd) + sizeof(D));  // 没考虑对齐，所以不对
  std::cout << ptr << std::endl;              // 0x7ffd88e0f69c
  pb1 = &dd;                                  // 简单地拷贝地址就行了
  pd = &dd;                                   // 简单地拷贝地址就行了
  pb2 = pdd;
  // 转换为：
  pb2 = pdd ? (B2*)(((char*)&dd) + sizeof(D)) : 0;
}
}  // namespace test3

namespace test4 {
// 虚拟继承

// 多重继承的一个语意上的副作用就是，
// 它必须支持某种形式的share subobject继承。
// 一个典型的例子是iostream library。
// 如果类内含一个或多个虚拟基类subobjects，那么将被分割为两部分：
// 一个不变局部和一个共享局部。
// 不变局部中的数据，不管后继如何变化，总是拥有固定的offset（从对象开头算），
// 所以这一部分的数据可以直接存取。
// 至于共享局部，其位置会因为每次的派生操作而有变化，所以它们只能被间接存取。
// 各家编译器实现技术之间的差异就在于间接存取的方法不同，有三种主流策略。

// 一般的布局策略是先安排好派生类的不变部分，然后再建立其共享部分。
// 然而，存在一个问题：如何能够存取类中的共享部分呢？
// cfront编译器在每一个派生类对象中安插一些指针，每个指针指向一个虚拟基类。
// 通过这个指针间接存取虚基类的成员。
// 这样的实现模型有两个主要的缺点：
// 1.每一个对象必须针对其每一个虚基类背负一个额外的指针，
//   理想上希望对象有固定的负担，不因为虚基类的数目而变化。
// 2.由于虚继承链的加长，导致间接存取层次的增加。
//   理想上希望有固定的存取时间，不因为虚基类的深度而改变。
// MetaWare和其他编译器到今天仍然使用cfront的原始实现模型来解决第二个问题：
// 1.把经由拷贝操作取得得所有的nested virtual base class指针，
//   放到派生类对象中。
// 2.这就解决了固定存取时间的问题，虽然付出了一些空间上的代价。
// 至于第一个问题，一般有两个解决方法：
// 1.microsoft编译器引入所谓的virtual base class table。
//   每一个对象如果有一个或多个虚拟基类，就会有编译器安插一个指针，
//   指向virtual base class table，
//   至于真正的virtual base class指针，就被放在该表格中。
// 2.在virtual function table中放置virtual base class的offset，
//   而不是地址。
// 一般而言，virtual base class最有效的一种运用形式就是：
// 一个抽象的virtual base class，没有任何数据成员。

class B {
 public:
  void print() { std::cout << "B x : " << &x << " " << sizeof(x) << std::endl; }

 protected:
  int x;
};
class D1 : public virtual B {
 public:
  void print() {
    B::print();
    std::cout << "D1 y: " << &y << " " << sizeof(y) << std::endl;
  }

 protected:
  int y;
};
class D2 : public virtual B {
 public:
  void print() {
    B::print();
    std::cout << "D2 z: " << &z << " " << sizeof(z) << std::endl;
  }

 protected:
  int z;
};
class DD : public D1, public D2 {
 public:
  void print() {
    D1::print();
    D2::print();
    std::cout << "DD i: " << &i << " " << sizeof(i) << std::endl;
  }

 protected:
  int i;
};

void func() {
  std::cout << sizeof(B) << "," << alignof(B) << std::endl;    // 4,4
  std::cout << sizeof(D1) << "," << alignof(D1) << std::endl;  // 16,8
  std::cout << sizeof(D2) << "," << alignof(D2) << std::endl;  // 16,8
  std::cout << sizeof(DD) << "," << alignof(DD) << std::endl;  // 40,8

  DD dd;
  std::cout << "dd  : " << &dd << std::endl;
  dd.print();
  // dd  : 0x7fffabe06f20
  // B x : 0x7fffabe06f40 4
  // D1 y: 0x7fffabe06f28 4
  // B x : 0x7fffabe06f40 4
  // D2 z: 0x7fffabe06f38 4
  // DD i: 0x7fffabe06f3c 4
}
}  // namespace test4
}  // namespace n5

namespace n6 {
// 3.5 对象成员的效率

// 如果把优化开关打开，封装就不会带来执行期的效率成本，
// 使用inline存取函数也是。
// 如果没有把优化开关打开，就很难猜测一个程序的效率表现，
// 因为程序代码潜在性地受到专家所谓的一种奇行怪癖，
// 也与特定编译器有关的魔咒影响。

// 单一继承应该不会影响测试的效率，因为成员被连续存储与派生类对象中，
// 并且其offset在编译期就已知了。
// 多重继承的情况下应该也是相同的，虚拟继承的效率令人失望。
}  // namespace n6

namespace n7 {
// 3.6 指向Data Members的指针

// 指向data members的指针，是一个有点神秘但颇有用处的语言特性，
// 特别是如果需要详细调查成员的底层布局的话。
// 可以用来决定vptr是放在类的起始处或是尾端，也可以用来决定类中访问段的次序。

namespace test1 {
// 每个对象含有x，y，z，以及一个vptr。
// 静态成员，放在对象之外，唯一随编译器不同而不同的是vptr的位置。
// C++标准允许vptr被放在对象中的任何位置：
// 起始处、尾端，或是在各个成员之间。
// 实际上，所有编译器不是放在对象头部，就是放在对象尾部。
// &A::z;取地址操作将得到z在对象中的偏移量。
// C++要求用一个access level中的成员的排列次序应该和声明次序相同。
class A {
 public:
  virtual ~A() {}

 public:
  static A a;
  int x, y, z;
};
A A::a;
void func() {
  // 取一个非静态数据成员的地址，将会得到它在类中的offset：
  std::printf("A::x offset :%d\n", &A::x);  // 8
  std::printf("A::y offset :%d\n", &A::y);  // 12
  std::printf("A::z offset :%d\n", &A::z);  // 16

  // 取一个对象的数据成员的地址，将会得到该成员在内存中的真正地址：
  A a;
  std::printf("a addr      :%p\n", &a);    // 0x7fff6bb9a780
  std::printf("A::x addr   :%p\n", &a.x);  // 0x7fff6bb9a788
  std::printf("A::y addr   :%p\n", &a.y);  // 0x7fff6bb9a78c
  std::printf("A::z addr   :%p\n", &a.z);  // 0x7fff6bb9a790

  // 为啥不行？
  std::cout << "A x: " << &A::x << std::endl;  // 1
  std::cout << "A y: " << &A::y << std::endl;  // 1
  std::cout << "A z: " << &A::z << std::endl;  // 1
}
}  // namespace test1

namespace test2 {
class A {
 public:
  int x;
};
void func() {
  // 如何区分一个没有指向任何数据成员的指针，和一个指向第一个数据成员的指针？
  int A::*p1 = 0;  // nullptr
  // 每个真正成员offset都被加上1，编译器和使用者都必须在使用前先减掉1：
  int A::*p2 = &A::x;  // 没有加1，是不是编译器优化了？
  std::printf("A::x offset :%d\n", p2);  // 0
  if (p1 == p2) {
    std::cout << "p1 == p2" << std::endl;
  } else {
    std::cout << "p1 != p2" << std::endl;
  }
  // p1 != p2
}
}  // namespace test2

namespace test3 {
// 在多重继承下，如果要将第二个（或后继）base class的指针，
// 和一个与派生类对象绑定的成员结合起来，
// 那么将会因为需要加入offset值而变得相当复杂。
struct B1 {
  int v1;
};
struct B2 {
  int v2;
};
struct D : B1, B2 {};
void func1(int D::*dmp, D* pd) {
  // 期望第一个参数得到的是一个指向派生类成员的指针。
  // 如果传入一个指向基类成员的指针会怎么样呢？
  pd->*dmp;
}
void func2(D* pd) {
  int B2::*bmp = &B2::v2; // bmp将成为1，但在D中，v2为5
  // 当bmp被作为func1的第一个参数时，
  // 它的值就必须因介入的B1 class的大小而调整，
  // 否则func1中将取到B1::v1，而非程序员所期望的B2::v2。
  // 要解决这个问题，必须经由编译器内部转换：
  // func1(bmp ? bmp + sizeof(B1):0,pd);
  func1(bmp, pd);
}
void func() {
  printf("&B1::v1 = %d\n", &B1::v1);  // 0
  printf("&B2::v2 = %d\n", &B2::v2);  // 0
  printf("&D1::v1 = %d\n", &D::v1);   // 0
  printf("&D2::v2 = %d\n", &D::v2);   // 0
  // 为什么都是0？
}
}  // namespace test3
}  // namespace n7

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 11]" << std::endl;
    return 0;
  }
  int tD1pe = atoi(argv[1]);
  switch (tD1pe) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::test1::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::test1::func();
      break;
    case 4:
      n5::test1::func1();
      break;
    case 5:
      n5::test1::func2();
      break;
    case 6:
      n5::test2::func();
      break;
    case 7:
      n5::test3::func();
      break;
    case 8:
      n5::test4::func();
      break;
    case 9:
      n7::test1::func();
      break;
    case 10:
      n7::test2::func();
      break;
    case 11:
      n7::test3::func();
      break;
    default:
      std::cout << "invalid tD1pe" << std::endl;
      break;
  }

  return 0;
}