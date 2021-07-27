#include <assert.h>
#include <iostream>

using namespace std;

// X,Y,Z,A中没有任何一个class内含明显的数据，其间只表示了继承关系。
class X {
  // 事实上并不是空的，它有一个隐晦的1字节，那是被编译器安插进取的一个char。
  // 这使得这个类的两个对象得以在内存中配置独一无二的地址。
};
class Y : public virtual X {
  // 大小和机器有关，也和编译器有关。大小受到三个因素的影响：
  // 1 语言本身所造成的额外负担
  //   当语言支持虚基类时，就会导致一些额外负担。在派生类中，这个额外负担反映在某种形式的指针身上，
  //   它或者指向virtual base class subobject，或者指向一个表格，表格中存放的若不是virtual
  //   base class subobject的地址，就是其偏移量。
  // 2 编译器对于特殊情况所提供的优化处理
  //   virtual base class X subobject的1字节大小也出现在Y和Z身上。
  //   传统上它被放在派生类的固定（不变动）部分的尾端。某些编译器会对empty virtual base class提供特殊支持。
  //   empty virtual base class已经成为C++ OO设计的一个特有术语了。它提供了一个虚拟接口，没有定义任何数据。
  //   某些新的编译器对此提供了特殊处理。在这个策略下，empty virtual base class被视为派生类最开头的一部分，
  //   也就是说它并没有花费任何额外的空间。节省了1字节，只剩下第1点说说的额外负担了。
  // 3 Alignment的限制
  //   Y和Z的大小截止目前为5字节。
  //   大部分机器上，结构体的大小会受到alignment的限制，是它们能够更有效率地在内存中存取。
};
class Z : public virtual X {
  // 同X
};
class A : public Y, public Z {
  // 一个virtual base class subobject只会在派生类中存在一份实体，不管它在类继承体系中出现了多少次。
  // A的大小由以下几点决定（书中说A的大小是12）：
  //  1 被大家共享的唯一一个X实体，大小为1字节。
  //  2 基类Y的大小，减去X的大小，结果是4字节。Z的算法相同，加起来是8字节。
  //  3 A自己的大小，0字节。
  //  4 A的alignment数量（如果有的话）。前三项的和为9，按4字节对齐，补足3字节，结果是12字节。
  // 对empty virtual base class特殊处理的编译器，拿掉了1字节和额外的3字节，A的大小是8字节。
};

// C++标准并不强制规定如“base class subobjects的排列次序”或“不同存取层级的数据成员的排列次序”这种琐碎细节。
// 也不规定虚函数或虚基类的实现细节。标准只说：那些细节有各厂商自定。
// C++对象模型尽量以空间优化和存取速度优化的考虑来表现非静态数据成员，并保持和C语言struct数据配置的兼容性。
// 它把数据直接放在每一个对象中，对于继承而来的非静态数据成员（不管是virtual或nonvirtual base class）也是如此。
// 不过并没有强调定义其间的排列顺序。
// 至于静态数据成员，则被放置在程序的一个全局数据段中，不会影响个别的对象大小。
// 在程序中，不管该类被产生出多少个对象（直接产生或间接派生），静态数据成员永远存在一份实体，甚至即使该类没有任何对象实体，
// 其静态数据成员也已存在。但是一个模板类的静态数据成员的行为稍有不同。
// 每一个对象必须有足够的大小以容纳它所有的非静态数据成员。有时候其值比你想象的大，因为：
//  1 有编译器自动加上的额外数据成员，用以支持某些语言特性（主要是各种virtual特性）。
//  2 因为alignment的需要。

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
// 这样的绑定结果几乎遍地都是。但不在大家预览之中，并因此导出早期C++的两种防御性程序设计风格：
// 1 把所有的数据成员放在类声明起头处，以确保正确的绑定：
class Point3d2 {
  float x, y, z;

 public:
  float X() const { return x; }
};
// 2 把所有的内联函数，不管大小都放在类声明之外：
class Point3d3 {
 public:
  Point3d3(float, float, float);
  float X() const;
  void X(float);

 private:
  float x, y, z;
};
inline float Point3d3::X() const { return x; }
// 这些程序设计风格事实上到今天还存在，虽然它们的必要性已经从C++ 2.0之后就消失了。
// 这个古老的语言规则被称为“member rewriting rule”，大意是“一个内联函数实体，在整个类声明未被完全看见之前，是不会被评估求值的”。
// C++标准以“member scope resolution rules”来精炼这个“rewriting rule”，其效果是，
// 如果一个内联函数在类声明之后立刻被定义的话，那么就还是对其评估求值。
// 然而，对于成员函数的参数列表并不为真。参数列表中的名称还是会在它们第一次遭遇时被适当地决议完成。
// 因此在extern和nested type names之间的非直觉绑定操作还是会发生。
typedef int length;
class XX {
 public:
  void mumble(length val) { _val = val; }  // length被决议为global
  length mumble() { return _val; }

 private:
  // length必须在“本类对它的第一个参数操作”之前被看见这样的声明将使先前的参考操作不合法
  // typedef float length;
  // 这种情况，仍然需要某种防御性程序风格：请始终把“nested type声明”放在类的起始处。
  length _val;
};

// 3.2 Data Member的布局

// 非静态数据成员在对象中的排列顺序将和其被声明的顺序一样，任何中间介入的静态数据成员都不会放进对象布局之中。
// C++标准要求，在同一个访问段（也就是private、public、protected等区段）中，成员的排序只需符合“较晚出现的成员在对象中有较高的地址”这一条即可。
// 也就是说，各个成员并不一定得连续排序。成员的边界调整可能需要填补一些字节，这些位于成员直接。
// 编译器还可能合成一些内部使用的数据成员，以支持整个对象模型，vptr就是这样的东西。它会放在什么位置呢？
// 传统上被放在所有明确声明的成员的最后。也有一些会放在对象的最前端。
// C++标准秉承先前所说的“对于布局持的放任态度”，允许编译器把那些内部产生出来的members自由放在任何位置上，甚至那些被程序员声明出来的成员之间。
// C++标准也允许编译器将多个访问段之中的数据成员自由排列，不必在乎他它们出现在类中声明中的次序。
// 各家编译器都是把一个以上的访问端连接在一起，依照声明的次序，成为一个连续区块。访问段的多少并不会招来额外负担。
template <class class_type, class data_type1, class data_type2>
char* asscess_order(data_type1 class_type::*mem1,
                    data_type2 class_type::*mem2) {
  // 接受两个数据成员，然后判断谁先出现在类对象中
  assert(mem1 != mem2);
  return mem1 < mem2 ? "member 1 occurs first" : "member 2 occurs first";
}
class Test {
 public:
  float x, y, z;
};

// 3.3 Data Member的存取

// Static Data Members
// 被编译器提出于类之外，并被视为一个全局变量（但只在class生命范围之内可见）。
// 每一个成员的存取许可，以及与class的关联，并不会导致任何空间上或执行时间上的额外负担，不论是在个别对象或是静态数据成员本身。
// 每一个静态数据成员只有一个实体，存放在程序的data segment之中。每次程序取用静态成员，就会被内部转化为对该唯一的extern实体的直接参考操作。

// Point3d origin,*pt = &origin;
// origin.chunkSize = 250;  // Point3d::chunkSize = 250;
// pt->chunkSize = 250;     // Point3d::chunkSize = 250;
// 从指令执行的观点来看，这是C++语言中“通过一个指针和通过一个对象来存取member，结论完全相同”的唯一一种情况。
// 这是因为“经由member selection operators（.）对一个静态数据成员进行存取操作”只是语法上的一种便宜行事而已。
// 成员并不在对象之中，因此存取静态成员并不需要通过对象。
// 如果chunkSize是从一个复杂继承关系中继承而来的成员，也是一样的，只有唯一一个实体，而其存取路径仍然是那么直接。

// 静态数据成员的存取是经由函数调用（或其他某些语法）而被存取：
// foobar().chunkSize = 250;
// C++标准明确要求foobar()必须被求值，虽然其结果并无用处。

// 若取一个静态数据成员的地址，会得到一个指向其数据类型的指针，而不是一个指向其类成员的指针，因为静态成员并不在对象中。
// &Point3d::chunkSize;  // 会得到const int*类型的内存地址

// 如果两个类声明了同名的静态成员，那么当它们放在data segment时，就会导致名称冲突。
// 编译器的解决方法是暗中对每一个静态数据成员编码（name-mangling)，以获得一个独一无二的程序识别代码。
// 每个编译器的name-mangling方法都不同，通常不外乎是表格、语法措辞等等。任何name-mangling做法都要两个要点：
//  1 一种算法，推导出独一无二的名称。
//  2 万一编译系统（或环境工具）必须和使用者交谈，那些独一无二的名称可以轻易被推倒回到原来的名称。

// Nonstatic Data Members
// Nonstatic Data Members直接存放在每一个对象中。除非经由明确的或暗喻的对象，没有办法直接存取它们。
// 只要程序员在一个成员函数中直接处理一个非静态数据成员，所谓“implicit class object”就会发生。
// Point3d Point3d::translate(const Point3d& pt) {
//   x += pt.x;
//   y += pt.y;
//   z += pt.z;
// }
// 看上去是对于x，y，z的直接存取，事实上是经由一个“implicit class object”（由this指针表达）完成。事实上这个函数的参数是：
// Point3d Point3d::translate(Point3d *const this, const Point3d& pt) {
//   this->x += pt.x;
//   this->y += pt.y;
//   this->z += pt.z;
// }

// 欲对一个非静态数据成员进行存取操作，编译器需要把对象的起始地址加上数据成员的偏移量（offset）：
// origin._y = 0.0;  // &origin + (&Point3d::_y - 1);
// 指向数据成员的指针，其offset总是被加上1，这样可以使编译系统区分出“一个指向数据成员的指针，用以指出类的第一个成员”和“一个
// 指向数据成员的指针，没有指出任何成员”两种情况。
// 每一个非静态数据成员的偏移量（offset）在编译时期即可获知，甚至如果成员属于一个base class subobject（派生自单一或多重继承链）也是一样。
// 因此，存取一个非静态数据成员，其效率和存取一个C结构体或一个nonderived class的member是一样的。

// 虚拟继承将为“经由base class subobject存取class members”导入一层新的间接性
// Point3d *pt3d;
// pt3d->_x = 0.0;
// 当_x是一个结构体成员、一个类成员、单一继承、多重继承的情况下都完全相同。但如果_x是一个virtual base class的member，存取速度会比较慢一点。
// 从对象和指针存取成员有什么重大的差异？
// 当类是一个派生类，而在其继承结构中有一个虚拟基类，并且被存取的成员是一个从虚基类继承而来的成员时，就会有重大的差异。
// 此时不能确定指针指向哪一种类型，因此就不知道编译时期这个成员真正的offset位置。所以这个存取操作必须延迟至执行期，经由一个额外的间接导引，才能解决。
// 如果使用对象，就不会有这些问题，其类型是确定的，所以offset位置也在编译时期就固定了。一个积极进取的编译甚至可以静态地由对象就解决掉对x的存取。

// 3.4 “继承”与Data Member

int main() {
  // 所以有人认为每一个类的大小都应该是0。这是不对的，即使是类X的大小也不为0：
  cout << "X sizeof: " << sizeof(X) << endl;  // 1
  cout << "Y sizeof: " << sizeof(Y) << endl;  // 8
  cout << "Z sizeof: " << sizeof(Z) << endl;  // 8
  cout << "A sizeof: " << sizeof(A) << endl;  // 16

  // asscess_order(&Test::z, &Test::y);

  return 0;
}