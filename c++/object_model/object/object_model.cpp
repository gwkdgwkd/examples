#include <iostream>

using namespace std;

// 三个著名的C++语言扩充性质，它们都会影响C++对象：
// template
// execption handling
// runtime type identification（RTTI）。

// 7.1 Template

// template是最令程序员有挫败感的一个主题。

// Template的实例化行为：
template <class Type>
class Point {
 public:
  enum Status { unallocated, nnormalized };
  Point(Type x = 0.0, Type y = 0.0, Type z = 0.0) {}
  ~Point() {}

  void *operator new(size_t) {}
  void operator delete(void *, size_t) {}
  static Point<Type> *freeList;
  static int chunkSize;

 private:
  Type _x, _y, _z;
};
// 一个对象的定义，不论是有编译器暗中地做，或是有程序员明确地做，
// 都会导致template class的实例化。
// 只有在成员函数被使用的时候，C++标准才要求它们被实例化出来。
// 之所以由使用者来主导实例化规则，有两个主要原因：
// 1.空间和时间效率的考虑。
//   如果类有100个成员函数，但相关类型只使用了2个，
//   那么其他198个函数都实例化将会花费大量的时间和空间。
// 2.尚未实现的机能，并不是一个template实例化出来的所有类型，
//   就一定能够完整支持一组成员函数所需的所有运算符。
//   如果只实例化那些真正用到的成员函数，
//   template就能够支持那些原本可能会造成编译时期错误的类型。
//   例如：Point<float> *p = new Point<float>;
//   只有template的float实例、new运算符、默认构造函数需要被实例化化。
// 函数在什么时候实例化出来呢？有两种策略：
// 1.在编译时候，那么函数将实例化与origin和p存在的哪个文件中。
// 2.在链接时候，那么编译器会别一些辅助工具重新激活。
// template函数实体可能被放在这个文件中，
// 别的文件中，或一个分离的存储位置上。
// 在int和long一致（或double和long double一致）的结构之中，
// 目前所有的编译器都会产生两个实体。
// C++标准并未对此有什么强制规定。

// Template的错误报告

// Template中的名称决议方式
// scope of the template definition
double foo(double) { cout << "double" << endl; }
template <class type>
class ScopeRules {
 public:
  void invariant() { _member = foo(_val); }
  type type_dependent() { return foo(_member); }

 private:
  int _val;
  type _member;
};
// scope of the template instantiantion
int foo(int) { cout << "int" << endl; }

// template之中，对于一个nonmember name的决议结构是根据：
// 这个name的使用是否与用以实例化出该template的参数类型有关而决定的。
// 如果互不相关，那么就以scope of the template definition来决定name。
// 如果互相关联，那么就以scope of the template instantiantion来决定name。
// 此外，函数的决议结果只和函数的原型有关，和函数的返回值没有关联。
// 对于invariant的调用，_member的类型并不会影响哪个foo实体被选中。
// foo()的调用与template参数毫无关联，
// 所以调用操作必须根据scope of the template definition来决议。
// 对于type_dependent的调用，与template参数有关，
// 因为该参数将决定_member的真正类型。
// 所以这一次必须在scope of the template instantiantion中决议，
// 有两个foo()函数声明。
// 由于_member的类型为int，所以应该是int版本的foo()出线。
// 如果ScopeRules是以unsigned int或log类型实例化出来，
// 那么foo调用操作就暧昧不明。
// 如果ScopeRules是以某一个类类型实例化出来，
// 而该类没有针对int或double实现出转换运算符，
// 那么foo()调用操作会被标示为错误。
// 这意味这一个编译器必须保持两个scope contexts：
// 1.scope of the template definition，
//   用以专注与一般的template class。
// 2.scope of the template instantiantion，
//   用以专注与特定的实体。
// 编译器的决议算法必须决定哪一个才是适当的scope，
// 然后在其中搜寻适当的name。

// Member Function的实例化行为
// 对于template的支持，最困难的莫过于template function的实例化，
// 编译器提供了两个策略：
// 1.一个是编译时期策略，程序代码必须在program text file中准备妥当；
// 2.另一个是链接时期策略，有一些工具可以引导编译器的实例化行为。

// 编译器设计者必须回答三个主要问题：
// 1.编译器如何找出函数的定义？
// 2.编译器如何能够只实例化出程序用到的成员函数？
// 3.编译器如何阻止成员定义在多个.o文件中被实例化呢？

// 7.2 异常处理

// 7.3 执行期类型识别

// 在2.0之前，除了析构函数外，
// 唯一不能被重载的函数就是转换运算符，因为它们不使用参数。
// 直到引进了const member functions后，转换操作符才能重载成const。
//  operator char*();
//  operator char*() const;

// C++被吹毛求疵的一点就是，缺乏一个保证安全的向下转换操作，
// 只有在类型真的可以被适当转换的情况下，才能执行downcast。
// 支持type-safe downcast在对象空间和执行时间上都需要一些额外负担：
// 1.需要额外的空间以存储类型信息，通常是一个指针，指向某个类型信息节点。
// 2.需要额外的时间以决定执行期的类型，因为，正如其名，这需要在执行期才能决定。

// C++的RTTI机制提供一个安全的downcast，但只对展现多态的类型有效。
// 把类相关的RTTI object地址放进virtual table中，通常放在第一个slot中。
// 每一个对象只多花一个指针。
// 这个指针只需被设定一次，它是被编译器静态设定，
// 而不是在执行期有构造函数设定（vptr才这么设定）。

// dynamic_cast运算符可以在执行期决定真正的类型：
// 1.如果downcast是安全的（基类指针指向一个派生类对象），
//   这个运算符会传回被适当转换的指针。
// 2.如果downcast不是安全的，这个运算符会传回0。

// references并不是pointers
// 程序中对一个类指针类型执行dynamic_cast运算符，会获得true或false：
// 1.如果传回真正的地址，表示这个对象的动态类型被确认了，
//   一些与类型有关的操作现在可以施行与其上。
// 2.如果传回0，表示没有指向任何对象，
//   意味这该以另一种逻辑执行这个动态类型为确定的对象身上。
// dynamic_cast运算符也适用于引用身上。
// 然而对于一个non-type-safecast，其结果不会与指针的情况相同。因为：
// 1.一个引用不可以像指针那样把自己设为0便代表了no object；
// 2.若将一个引用设为0，会引起一个临时性对象被产生出来，
//   该临时对象的初值为0，这个引用然后被设定为该临时对象的一个别名。
// 因此当dynamic_cast运算符用在引用时，
// 不能够提供等于指针情况下的那一组true和false。
// 会发生下面的事：
// 1.如果引用真正参考到适当的派生类，downcast会别执行而程序可以继续进行。
// 2.如果引用并不是某一种派生类，那么由于不能传回0，
//   所以丢出一个bad_cast exception。

// Typeid运算符
// 使用typeid运算符，就有可能以一个引用达到相同的执行期替代路线。
// type_info运算符传回一个const reference，类型为type_info。
// RTTI只适用于多态类，type_info对象也适用于内建类型，
// 以及非多态的使用者自定类型。

// 7.4 效率有了，弹性呢？

// 传统的C++对象模型提供了有效率的执行期支持。
// 这份效率，再加上与C之间的兼容性，造成了C++的广泛接受度。
// 然而，在某些方面，像动态共享函数库、共享内存、以及分布式对象方面，
// 这个对象模型的弹性还是不够。

// 动态共享函数库
// 在目前的C++对象模型中，如果库中的对象布局有所改变，应用程序就必须重新编译。
// 因为类的大小及其每一个直接（或继承而来）的成员的偏移量，
// 都在编译时期就已经固定（虚拟继承的成员除外）。
// 这虽然带来了效率，却在二进制层面影响了弹性。

// 共享内存
// 当一个共享库被加载，它在内存中的位置由runtime linker决定，
// 一般而言与执行中的进程无关。
// 然而，在C++对象模型中，当一个动态库支持一个对象，
// 其中含有虚函数，上述说法便不正确。
// 问题并不在于将该对象放置于共享内存中的那个进程，
// 而在于想要经过这个共享对象调用一个虚函数的第二个或更后继的进程。
// 除非共享库放在完全相同的内存位置上，就像当初加载这个共享对象的进程一样，
// 否则虚函数会死的很难看，可能包括段错误或bus error。
// 因为每一个虚函数在virtual table中的位置已经写死了。
// 目前的解决方法是属于程序层面，程序员必须保证让跨进程的共享库有相同的地址。
// 编译系统层面上的解决方法，
// 势必得牺牲原本的virtual table实现模型所带来的高效率。

int main() {
  Point<float>::Status s;  // ok
  // Point::Status s1;  // error

  Point<float>::freeList;  // ok
  // Point::freeList;  // error
  // 使用静态成员，会使其一份实体与Point class的float在程序中产生关联。

  Point<float> *ptr = 0;
  // 程序什么也没发生。因为一个指向对象的指针，本身并不是一个对象，
  // 编译器不需要知道与该类有关的任何成员数据或对象布局数据。
  // 所以将Point的一个float实体实例化也就没有必要。
  // 在C++标准之前，没有强制定义，
  // 编译器可以自行决定要不要将template实例化出来。
  // 如今，C++标准已经禁止编译器实例化了。

  const Point<float> &ref = 0;
  // 换成引用，会实例化出一个Point的float实体来。
  // 因为引用并不是无物（no object）的代名词。
  // 0被视为整数，必须被转换为以下类型的一个对象：Point<float>
  // 如果没有转换的可能，这个定义就是错误的，会在编译时被挑出来。

  ScopeRules<int> sr0;
  sr0.invariant();       // double
  sr0.type_dependent();  // double 怎么不是int？

  return 0;
}