#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

// 在C语音中，“数据”和“处理数据的操作（函数）”是分开声明的，也就是说，语言本身并没有支持“数据和函数”之间的关联性。
// 把这种程序方法称为程序性的，有一组“分布在各个以功能为导向的函数中”的算法所驱动，它们处理的是共同的外部数据。
// 从软件工程的眼光来看，“一个ADT或class hierarchy的数据封装”比“在C程序中程序性地使用全局数据”好。

// 加上封装后，布局成本增加了多少？答案是并没有增加成本。
//  data members直接内含在每一个对象中，就像C的结构体一样。
//  成员函数虽然在class的声明内，却不出现在对象之中。
//  每一个non-inline member function只会诞生一个函数实体。
//  至于每一个“拥有零个或一个定义”的inline function则会在其每一个使用者（模块）身上产生一个函数实体。
// 封装性并未带给程序任何空间或执行期的不良反应。C++在布局以及存取时间上主要的额外负担是由virtual引起，包括：
//  virtual function机制，用以支持一个有效率的“执行期绑定”。
//  virtual base class，用以实现“多次出现在继承体系中的base class，有一个单一而被共享的实体”。
// 此外，还有一些多重继承下的额外负担，发生在“一个derived class和其第二或后继之base class的转换”之间。
// 然后，一般而言，并没有什么天生的理由说C++程序一定比其C兄弟庞大或迟缓。

// 1.1 C++对象模式

// 在C++中有，有两种成员数据：static和nonstatic，依据三种成员函数：static、nonstatic和virtual。
// C++对象模型：
//  非静态成员数据被配置于每一个对象之内，静态成员数据则被存放在所有的对象之外。
//  静态和非静态成员函数也被放在所有的对象之外。
//  virtual函数通过以下两个步骤支持：
//    1 每一个class产生出一堆指向virtual functions的指针，放在表格之中。这个表格被称为virtual vable（vtbl）。
//    2 每一个对象被添加一个指针，指向相关的virtual table。通常这个指针被称为vptr。
//      vptr的设置和重置都由每一个class的构造、析构和拷贝运算符自动完成。
//      每一个class所关联的type_info object（用以支持RTTI）也经由virtual table被指出来，通常放在表格的第一个slot处。
// 这个模型的主要优点在于它的空间和存取时间的效率。
// 主要缺点则是，乳沟应用程序代码本身未曾改变，但所用到的class objects的非静态成员函数有所修改，那么应用程序同样得重新编译。

// C++支持单一继承，也支持多重继承。继承关系也可以指定为虚拟（virtual，也就是共享的意思）。
// 在虚拟继承的情况下，base class不管在继承串中被派生多少此，永远只会存在一个实体。被称为subobject。

// 1.2 关键词所带来的差异

// 如果不是为了努力维护与C直接的兼容性，C++远可以比现在更简单些。
//  如果没有八种整数需要支持的话，overloaded function的解决方式将会简单得多。
//  如果C++丢掉C的声明语法，就不用花脑筋取判断pf是函数调用而不是其声明：
//   int(*pf)(1024);  // 不知道是声明还是定义，看到常量1024才能决定
//   int (*pq)();  // 一个声明
//  如果C++并不需要支持C原有的struct，那么class的观念由关键词class来支持。

// 什么时候应该在C++程序中以struct取代class？当它让一个人感觉比较好的时候。
// 关键词struct本身并不一定要象征其后随之声明的任何东西。可以用struct代替class，但仍声明public、protected、private等待。
// 人们和教科书说到struct时，是指一个数据集合体，没有私有数据，也没有data的相应操作。就是纯C的用法。
// C程序员的巧计有时候却成为C++程序的陷阱。例如把单一元素的数组放在一个struct的尾端，于是每个struct可以拥有可变大小的数组：
struct mumble {
  char pc[1];  // stuff
};
class stumble {
 public:
 protected:
 private:
  char pc[1];  // stuff
};
// C的伎俩或许可以有效运行，或许不能，需要看保护数据成员被放在私有数据成员的前面或后面而定。
// 同样的道理，基类和继生类的数据成员的布局也没有谁先谁后的强制规定，因而也就不能保证前述的C伎俩一定有效。
// 组合，而非继承，才是把C和C++结合在一起的唯一可行方法。
// C struct在C++中的一个合理用途，是当要传递“一个复杂的对象的全部或部分”到某个C函数中时，
// struct声明可以将数据封装起来，并保证拥有与C兼容的空间布局。这项保证只在组合的情况下存在。
// 如果是“继承”而不是“组合”，编译器会决定是否应该有额外的数据成员被安插到base struct subobject中。

// 1.3 对象的差异

// C++程序设计模型直接支持三种程序设计典范：
//  程序模型，就像C一样，C++当然也支持它。
//  抽象数据类型模型（ADT），该模型所谓“抽象”是和一组表达式（public接口）一起提供，而其运算定义仍然隐而未明。
//  面向对象模型，在此模型中有一些彼此相关的类型，通过一个抽象base class（用以提供公共接口）被封装起来。

// C++以下列方法支持多态：
//  1 经由一组隐含的转换操作。例如把一个derived class指针转换为一个指向其public base type的指针。
//    shape *ps = new circle();
//  2 经由virtual function机制。
//    ps->rotate();
//  3 经由dynamic_cast和typeid运算符。
//    if(circle *pc = dynamic_cast<circle*>(ps))
class Base {
 public:
  virtual void d() const { cout << "Base::d" << endl; }
};
class DerivedA : public Base {
 public:
  virtual void d() const { cout << "DerivedA::d" << endl; }
};
class DerivedB : public Base {
 public:
  virtual void d() const { cout << "DerivedB::d" << endl; }
};
void test(Base b, const Base *p, const Base &r) {
  b.d();
  (*p).d();
  r.d();
}

// 需要多少内存才能够表现一个class object？一般而言有：
//  其非静态数据成员的总和大小；
//  加上任何由于对齐的需求而填补上去的空间；
//  加上为了支持virtual而由内部产生的任何额外负担。

// 一个指针，不管它指向哪一种数据类型，指针本身所需的内存大小是固定的。
class ZooAnimal {
 public:
  ZooAnimal() {}
  virtual ~ZooAnimal() {}
  virtual void rotate() { cout << "ZooAnimal::rotate" << endl; }

 protected:
  int loc;
  string name;
};
// 指向不同类型的指针间的差异，既不在其指针表示法不同，也不在其内容（地址）不同，而是在其所寻址出来的object类型不同。
// 也就是说，“指针类型”会教导编译器如何解释某个特定地址中的内存内容及其大小。
// 转型（cast）其实是一种编译器指令。大部分情况下它并不改变一个指针所含的真正地址，它只影响“被指出的内存的大小和其内容”的解释方式。
// 一个指针或一个引用之所以支持多态，是因为它们并不引发内存中任何“与类型有关的内存委托操作”。改变的只是指向内存的“大小和内容解释方式”而已。

class Bear : public ZooAnimal {
 public:
  Bear() {}
  ~Bear() {}
  void rotate() { cout << "Bear::rotate" << endl; };
  virtual void dance(){};

 protected:
  enum Dances {};
  Dances dances_known;
  int cell_block;
};

// 当一个基类对象被直接初始化为一个派生类对象时，就会别切割，以塞入较小的base type内存中，派生类没有留下任何蛛丝马迹。多态于是不再呈现。
// 多态是一种威力强大的设计机制，允许你继承一个抽象的public接口之后，封装相关的类型。
// 需要付出的代价就是额外的间接性，不论是在“内存的获得”或是“类型的决断”上。
// C++通过类的指针和引用类支持多态，这种程序设计风格就称为“面向对象”。

// C++也支持具体的ADT程序风格，被称为object_based（OB）。例如string，一种非多态的数据类型。
// string可以展示封装的非多态形式。提供public接口和private实作品，包括数据和算法，但不支持类型的扩充。

// 一个OB设计可能比一个对等的OO设计速度更快而且空间更紧凑：
//  速度快是因为所有的函数引发的操作都在编译时期解析完成，对象构建起来时不需要设置virtual机制；
//  空间紧凑则是因为每一个对象不需要负担传统上为了支持virtual机制而需要的额外负荷。
//  不过OB设计比较没有弹性。在弹性（OO）和效率（OB）之间常常存在着取与舍。

int main() {
  // char *str;
  // struct mumble *mpmumb1 = (struct mumble *)malloc(sizeof(struct mumble) + strlen(str) + 1);
  // strcpy(&mumble.pc, str);

  ZooAnimal za;
  ZooAnimal *pza = &za;
  cout << "ZooAnimal pza : " << sizeof(pza) << endl;  // 8
  int aa;
  cout << "int aa : " << sizeof(&aa) << endl;  // 8

  DerivedA a;
  test(a, &a, a);
  // Base::d
  // DerivedA::d
  // DerivedA::d
  DerivedB b;
  test(b, &b, b);
  // Base::d
  // DerivedB::d
  // DerivedB::d

  Bear bb;
  // 内存如下：
  //  int loc
  //  String name
  //  __vptr__ZooAnimal
  //  Dances dances_known
  //  int cell_block
  Bear *pbb = &bb;
  Bear &rbb = *pbb;
  cout << "ZooAnimal za : " << sizeof(za) << endl;  // 48
  cout << "Bear bb : " << sizeof(bb) << endl;       // 56
  cout << "Bear pbb : " << sizeof(pbb) << endl;     // 8
  cout << "Bear rbb : " << sizeof(rbb) << endl;     // 56

  ZooAnimal *pzz = &bb;
  // pzz和pbb每个都指向Bear对象的第一个byte。
  // 其间的差别是，pbb所涵盖的地址包含整个Bear对象，而pzz所涵盖的地址只包含Bear对象中的ZooAnimal subobject.
  // pzz->cell_block;  // 不合法：cell_block不是ZooAnimal的一个member
  // ((Bear *)pzz)->cell_block;  // ok，经过一个downcast操作就没有问题，但是保护成员编译不过
  // if(Bear * pb2 = dynamic_cast<Bear*>(pzz)) pb2->cell_block;  // 更好，但是成本较高
  // pbb->cell_block;  // ok

  pzz->rotate();  // Bear::rotate
  // pzz的类型将在编译时决定以下两点：
  //  固定的可用接口，pzz只能调用ZooAnimal的public接口。
  //  该接口的access level是ZooAnimal的一个public member。
  // 在每一个执行点，pzz所指的object类型可以决定rotate()所调用的实体。

  ZooAnimal zaa = bb;
  zaa.rotate();  // ZooAnimal::rotate

  return 0;
}