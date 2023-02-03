#include <iostream>

// 关于对象

// 在C语音中，数据和处理数据的操作（函数）是分开声明的，
// 也就是说，语言本身并没有支持数据和函数之间的关联性。
// 把这种由一组分布在各个以功能为导向的函数中的算法所驱动，
// 称为程序性的，它们处理的是共同的外部数据。
// 从软件工程来看，一个ADT或class hierarchy的数据封装，
// 比在C程序中程序性地使用全局数据好。

// 加上封装后，布局成本增加了多少？答案是并没有增加成本。
// 1.data members直接内含在每一个对象中，就像C的结构体一样。
// 2.成员函数虽然在class的声明内，却不出现在对象之中。
// 3.每一个non-inline member function只会诞生一个函数实体。
// 4.至于每一个拥有零个或一个定义的inline function，
//   则会在其每一个使用者（模块）身上产生一个函数实体。
// 封装性并未带给程序任何空间或执行期的不良反应。
// C++在布局以及存取时间上主要的额外负担是由virtual引起，包括：
// 1.virtual function机制，用以支持一个有效率的执行期绑定。
// 2.virtual base class，用以实现多次出现在继承体系中的base class，
//   有一个单一而被共享的实体。
// 此外，还有一些多重继承下的额外负担，
// 发生在一个derived class和其第二或后继之base class的转换之间。
// 一般而言，并没有什么天生的理由说C++程序一定比其C兄弟庞大或迟缓。

namespace n1 {
// 1.1 C++对象模式

// 在C++的类中有：
// 两种成员数据：static和nonstatic；
// 三种成员函数：static、nonstatic和virtual。

// C++对象模型：
// 1.非静态成员数据被配置于每一个对象之内，静态成员数据则被存放在所有的对象之外。
// 2.静态和非静态成员函数也被放在所有的对象之外。
// 3.virtual函数通过以下两个步骤支持：
//   a.每一个class产生出一堆指向virtual functions的指针，放在表格之中。
//     这个表格被称为virtual vable（vtbl）。
//   b.每一个对象被添加一个指针，指向相关的vtbl，通常这个指针被称为vptr。
//     vptr的设置和重置都由每一个class的构造、析构和拷贝运算符自动完成。
//     每一个class所关联的type_info object（用以支持RTTI），
//     也经由virtual table被指出来，通常放在表格的第一个slot处。
// 这个模型的主要优点在于它的空间和存取时间的效率。
// 主要缺点则是，如果应用程序代码本身未曾改变，
// 但所用到的class objects的非静态成员函数有所修改，那么应用程序同样得重新编译。

// C++支持单一继承，也支持多重继承，
// 继承关系也可以指定为虚拟（virtual，也就是共享的意思）。
// 在虚拟继承的情况下，base class不管在继承串中被派生多少次，
// 永远只会存在一个实体，被称为subobject。
}  // namespace n1

namespace n2 {
// 1.2 关键词所带来的差异

// 如果不是为了努力维护与C直接的兼容性，C++远可以比现在更简单些。
// 1.如果没有八种整数需要支持的话，
//   overloaded function的解决方式将会简单得多。
// 2.如果C++丢掉C的声明语法，就不用花脑筋取判断pf是函数调用而不是其声明：
//   int(*pf)(1024);  // 不知道是声明还是定义，看到常量1024才能决定
//   int (*pq)();  // 一个声明
// 3.如果C++并不需要支持C的struct，那么class的观念由关键词class来支持。

// 什么时候应该在C++程序中以struct取代class？
// 当它让一个人感觉比较好的时候。
// 关键词struct本身并不一定要象征其后随之声明的任何东西。
// 可以用struct代替class，但仍声明public、protected、private等待。
// 人们和教科书说到struct时，是指一个数据集合体，
// 没有私有数据，也没有data的相应操作，就是纯C的用法。
// C程序员的技巧有时候却成为C++程序的陷阱。
// 例如把单一元素的数组放在一个struct的尾端，
// 于是每个struct可以拥有可变大小的数组：
struct mumble {
  char pc[1];  // stuff
};
class stumble {
 public:
 protected:
 private:
  char pc[1];  // stuff
};
// C的伎俩或许可以有效运行，或许不能，
// 需要看保护数据成员被放在私有数据成员的前面或后面而定。
// 同样的道理，基类和继生类的数据成员的布局也没有谁先谁后的强制规定，
// 因而也就不能保证前述的C伎俩一定有效。
// 组合，而非继承，才是把C和C++结合在一起的唯一可行方法。
// C struct在C++中的一个合理用途，
// 是当要传递一个复杂的对象的全部或部分到某个C函数中时，
// struct声明可以将数据封装起来，并保证拥有与C兼容的空间布局，
// 这项保证只在组合的情况下存在。
// 如果是继承而不是组合，
// 编译器会决定是否应该有额外的数据成员被安插到base struct subobject中。

void func() {
  // char *str;
  // struct mumble *mpmumb1 =
  // (struct mumble *)malloc(sizeof(struct mumble) + strlen(str) + 1);
  // strcpy(&mumble.pc, str);
}
}  // namespace n2

namespace n3 {
// 1.3 对象的差异

// C++程序设计模型直接支持三种程序设计典范：
// 1.程序模型，就像C一样，C++当然也支持它；
// 2.抽象数据类型模型（ADT），
//   该模型所谓抽象是和一组表达式（public接口）一起提供，
//   而其运算定义仍然隐而未明；
// 3.面向对象模型，在此模型中有一些彼此相关的类型，
//   通过一个抽象base class（用以提供公共接口）被封装起来。

namespace test1 {
// C++以下列方法支持多态：
// 1.经由一组隐含的转换操作，
//   例如把一个derived class指针转换为一个指向其public base type的指针。
//   shape *ps = new circle();
// 2.经由virtual function机制。
//   ps->rotate();
// 3.经由dynamic_cast和typeid运算符。
//   if(circle *pc = dynamic_cast<circle*>(ps))
class B {
 public:
  virtual void d() const { std::cout << "B::d" << std::endl; }
};
class D1 : public B {
 public:
  virtual void d() const { std::cout << "D1::d" << std::endl; }
};
class D2 : public B {
 public:
  virtual void d() const { std::cout << "D2::d" << std::endl; }
};
void test(B b, const B *p, const B &r) {
  b.d();
  (*p).d();
  r.d();
}
void func() {
  D1 a;
  test(a, &a, a);
  // B::d
  // D1::d
  // D1::d
  D2 b;
  test(b, &b, b);
  // B::d
  // D2::d
  // D2::d
}
}  // namespace test1

namespace test2 {
// 需要多少内存才能够表现一个class object？
// 一般而言有：
// 1.其非静态数据成员的总和大小；
// 2.加上任何由于对齐的需求而填补上去的空间；
// 3.加上为了支持virtual而由内部产生的任何额外负担。

// 一个指针，不管它指向哪一种数据类型，指针本身所需的内存大小是固定的。
// 指向不同类型的指针间的差异，既不在其指针表示法不同，
// 也不在其内容（地址）不同，而是在其所寻址出来的object类型不同。
// 也就是说，指针类型会教导编译器如何解释某个特定地址中的内存内容及其大小。
// 转型（cast）其实是一种编译器指令。
// 大部分情况下它并不改变一个指针所含的真正地址，
// 它只影响被指出的内存的大小和其内容的解释方式。
// 一个指针或一个引用之所以支持多态，
// 是因为它们并不引发内存中任何与类型有关的内存委托操作。
// 改变的只是指向内存的大小和内容解释方式而已。

class B {
 public:
  B() {}
  virtual ~B() {}
  virtual void print() { std::cout << "B::print" << std::endl; }

 protected:
  int i;
  double d;
};
class D : public B {
 public:
  D() {}
  ~D() {}
  void print() { std::cout << "D::print" << std::endl; };
  virtual void show(){};

 protected:
  enum Dances {};
  Dances dances_known;
  int a;
};

void func() {
  B b;
  B *pb1 = &b;
  std::cout << "B pb : " << sizeof(pb1) << std::endl;  // 8
  int i;
  std::cout << "int i : " << sizeof(&i) << std::endl;  // 8

  D d;
  D *pd = &d;
  D &rd = *pd;
  std::cout << "B b : " << sizeof(b) << std::endl;  // 24
  std::cout << "D d : " << sizeof(d) << std::endl;       // 32
  std::cout << "D pd : " << sizeof(pd) << std::endl;     // 8
  std::cout << "D rd : " << sizeof(rd) << std::endl;     // 32

  B *pb2 = &d;
  // pb2和pd每个都指向D对象的第一个byte。
  // 其间的差别是，pd所涵盖的地址包含整个D对象，
  // 而pb2所涵盖的地址只包含D对象中的B subobject。

  pb2->print();  // D::print
  // pb2的类型将在编译时决定以下两点：
  // 1.固定的可用接口，pb2只能调用B的public接口。
  // 2.该接口的access level是B的一个public member。
  // 在每一个执行点，pb2所指的对象类型可以决定print所调用的实体。

  B bb = d;
  bb.print();  // B::print
}
}  // namespace test2
}  // namespace n3

// 当一个基类对象被直接初始化为一个派生类对象时，
// 就会别切割，以塞入较小的base type内存中，
// 派生类没有留下任何蛛丝马迹，多态于是不再呈现。
// 多态是一种威力强大的设计机制，
// 允许你继承一个抽象的public接口之后，封装相关的类型。
// 需要付出的代价就是额外的间接性，不论是在内存的获得或是类型的决断上。
// C++通过类的指针和引用类支持多态，这种程序设计风格就称为面向对象。

// C++也支持具体的ADT程序风格，被称为object_based（OB）。
// 例如string，一种非多态的数据类型，string可以展示封装的非多态形式。
// 提供public接口和private实作品，包括数据和算法，但不支持类型的扩充。

// 一个OB设计可能比一个对等的OO设计速度更快而且空间更紧凑：
// 1.速度快是因为所有的函数引发的操作都在编译时期解析完成，
//   对象构建起来时不需要设置virtual机制；
// 2.空间紧凑则是因为每一个对象不需要负担为了支持virtual机制的额外负荷；
// 3.不过OB设计比较没有弹性，在弹性（OO）和效率（OB）之间常常存在着取与舍。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n3::test1::func();
      break;
    case 1:
      n3::test2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}