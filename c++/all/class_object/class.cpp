#include <iostream>
#include <string>

// C++中保留了C语言的struct关键字，并且加以扩充。
// 在C语言中，struct只能包含成员变量，不能包含成员函数。
// 而在C++中，struct类似于class，既可以包含成员变量，又可以包含成员函数。
// C++中的struct和class基本是通用的，唯有几个细节不同：
// 1.使用class时，类中的成员默认都是private属性的，
//   而使用struct时，结构体中的成员默认都是public属性的；
// 2.class继承默认是private继承，而struct继承默认是public继承；
// 3.class可以使用模板，而struct不能。
// C++没有抛弃C语言中的struct关键字，其意义就在于给C程序开发人员归属感，
// 并且能让C++编译器兼容以前用C语言开发出来的项目。
// 使用C++时，强烈建议使用class来定义类，而使用struct来定义结构体，这样做语义更加明确。

// 类是创建对象的模板，一个类可以创建多个对象，每个对象都是类类型的一个变量。
// 创建对象的过程也叫类的实例化，每个对象都是类的一个具体实例，拥有类的成员变量和成员函数。
// 类的成员变量称为类的属性（Property），类的成员函数称为类的方法（Method）。
// 在面向对象的编程语言中，经常把函数（Function）称为方法（Method）。
// 与结构体一样，类只是一种复杂数据类型的声明，不占用内存空间。
// 而对象是类这种数据类型的一个变量，是通过类类型创建出来的一份数据，所以占用内存空间。
// class是C++中新增的关键字，专门用来定义类。

// 类只是一个模板，编译后不占用内存空间，所以在定义类时不能对成员变量进行初始化，
// 因为没有地方存储数据，只有在创建对象以后才会给成员变量分配内存，这个时候就可以赋值了。
// 类可以理解为一种新的数据类型，与char、int、float等不同的是，
// 类是一种复杂数据类型，可以包含基本类型，而且还有很多基本类型中没有的特性。

// 类的声明和成员函数的定义都是类定义的一部分。
// 在实际开发中，通常将类的声明放在头文件中，而将成员函数的定义放在源文件中。
// private关键字的作用在于更好地隐藏类的内部实现，该向外暴露的接口都声明为public，
// 不希望外部知道、或只在类内使用、或对外部没有影响的成员，都应为private。
// 根据C++设计规范，成员变量以及只被成员函数调用的成员函数都应为private，
// 而只将允许通过对象调用的成员函数声明为public。
// 声明protected的成员在类外也不能通过对象访问，但是在它的派生类内可以访问。
// 将成员变量声明为private、将部分成员函数声明为public体现了类的封装性。
// 所谓封装，是指尽量隐藏类的内部实现，只向用户提供有用的成员函数。

// 总结：
// 1.类的成员有成员变量和成员函数两种；
// 2.成员函数之间可以互相调用，成员函数内部可以访问成员变量；
// 3.私有成员只能在类的成员函数内部访问；
// 4.默认情况下，class类的成员是私有的，struct类的成员是公有的。

// 访问对象的成员变量或调用成员函数：
// 1.对象名.成员名
// 2.引用名.成员名
// 3.对象指针->成员名
// 成员函数被调用时，可以用上述三种方法指定函数是作用在哪个对象上的。

// 如果不考虑成员变量对齐问题的话，对象存储空间的大小等于各成员变量所占用空间的大小之和。

// 定义类时，如果一个构造函数都不写，则编译器自动生成默认（无参）构造函数和复制构造函数。
// 如果编写了构造函数，则编译器不自动生成默认构造函数。
// 一个类不一定会有默认构造函数，但一定会有复制构造函数。
// 任何生成对象的语句都要说明对象是用哪个构造函数初始化的。
// 即便定义对象数组，也要对数组中的每个元素如何初始化进行说明。
// 如果不说明，则编译器认为对象是用默认或参数全部可以省略的构造函数初始化。
// 此时如果类没有默认构造函数或参数全部可以省略的构造函数，则编译出错。
// 对象在消亡时会调用析构函数。

// 每个对象各自拥有普通成员变量，但是静态成员变量只有一份，被所有对象所共享。
// 静态成员函数不具体作用于某个对象，即便对象不存在，也可以访问类的静态成员。
// 静态成员函数内部不能访问非静态成员变量，也不能调用非静态成员函数。
// 常量对象不能执行非常量成员函数，只能执行常量成员函数。

// 友元分为友元函数和友元类，友元关系不能传递。

// 成员函数中出现的this指针，就是指向成员函数所作用的对象的指针。
// 因此，静态成员函数内部不能出现this指针。
// 成员函数实际上的参数个数比表面上看到的多一个this指针。

namespace n1 {
// A是类的名称；类名的首字母一般大写，以和其他的标识符区分开。
// 类可以看做是一种数据类型，它类似于普通的数据类型，但是又有区别。
// 类这种数据类型是一个包含成员变量和成员函数的集合。
class A {
  // C++通过public、protected、private来控制成员的访问权限，
  // 与Java、C#不同，C++中访问控制只能修饰类的成员，不能修饰类，类没有共有私有之分。
  // 在类的内部，无论成员被声明为public、protected还是private，
  // 都是可以互相访问的，没有访问权限的限制。
  // 在类的外部，只能通过对象访问成员，并且只能访问public属性的成员，
  // 不能访问private、protected属性的成员。
 public:
  // 类的成员变量和普通变量一样，也有数据类型和名称，占用固定长度的内存。
  // 但是，在定义类的时候不能对成员变量赋值（C++11可以了），
  // 因为类是数据类型或者说是模板，本身不占用内存空间，而变量的值则需要内存来存储。
  char a = 'h';

 protected:
  int b;

  // 声明为private的成员和声明为public的成员的次序任意，
  // 既可以先出现private部分，也可以先出现public部分。
 private:
  int c;

  // 在一个类体中，private和public可以分别出现多次。
  // 每个部分的有效范围到出现另一个访问限定符或类体结束时为止。
  // 但是为了使程序清晰，应该养成每一种成员访问限定符只出现一次的习惯。
 public:
  // 类的成员函数也和普通函数一样，都有返回值和参数列表，区别是：
  // 1.成员函数是一个类的成员，出现在类体中，它的作用范围由类来决定；
  // 2.而普通函数是独立的，作用范围是全局的，或位于某个命名空间内。
  // 在类体中直接定义函数时，不需要在函数名前加类名：
  void print() { std::cout << a << " " << b << " " << c << std::endl; }

  // 可以在类体中定义成员函数，也可以只在类体中声明，而将定义放在类外面：
  int getC();

  // 在类体中和类体外定义成员函数是有区别的：
  // 在类体中定义的成员函数会自动成为内联函数，在类体外定义的不会。
  // 当然，在类体内部定义的函数也可以加inline关键字，但这是多余的，
  // 因为类体内部定义的函数默认就是内联函数。
  // 内联函数一般不是所期望的，它会将函数调用处用函数体替代，
  // 所以建议在类体内部对成员函数作声明，而在类体外部进行定义，
  // 这是良好的编程习惯，实际开发中也是这样做的。
  // 除非函数比较短小，希望定义为内联函数，那也没有什么不妥的。
  // 如果既希望将函数定义在类体外部，又希望它是内联函数，那么可以在定义函数时加inline关键字。
  // 当然也可以在函数声明处加inline，不过这样做没有效果，编译器会忽略函数声明处的inline。
  // 在类体外定义inline函数的方式，必须将类的定义和成员函数的定义都放在同一个头或源文件中，
  // 否则编译时无法进行嵌入（将函数代码的嵌入到函数调用出）。
  // 虽然C++支持将内联函数定义在类的外部，但强烈建议将函数定义在类的内部，
  // 这样它会自动成为内联函数，何必费力不讨好地定义在外部呢，并没有任何优势。

  void testThis() {
    // 成员函数最终被编译成与对象无关的全局函数，如果函数体中没有成员变量，
    // 就很简单，不用做任何处理，直接调用即可，如果成员函数中使用到了成员变量该怎么办呢？
    // 成员变量的作用域不是全局，不经任何处理就无法在函数内部访问。
    // C++规定，编译成员函数时要额外添加一个参数，把当前对象的指针传递进去，
    // 通过指针来访问成员变量，这样通过传递对象指针就完成了成员函数和成员变量的关联。
    std::cout << static_cast<void *>(this) << " " << sizeof(*this) << " "
              << this->a << std::endl;
  }

  // 由{}包围起来的部分有时也称为类体，和函数体的概念类似。
  // 类定义最后的分号;是类定义的一部分，表示类定义结束了，不能省略。
};

// 当成员函数定义在类外时，就必须在函数名前面加上类名予以限定。
// ::被称为域解析符，也称作用域运算符或作用域限定符，
// 用来连接类名和函数名，指明当前函数属于哪个类。
// 成员函数必须先在类中声明，然后在类外定义，类定义应在函数定义之前：
int A::getC() { return c; }

// 打印成员函数地址的两种方法：
template <typename dst_type, typename src_type>
dst_type pointer_cast(src_type src) {
  return *static_cast<dst_type *>(static_cast<void *>(&src));
}
// 这种方法是最常规，也是最好理解的一种方法了，巧妙地利用了联合体的优点：
template <typename dst_type, typename src_type>
dst_type union_cast(src_type src) {
  union {
    src_type s;
    dst_type d;
  } u;
  u.s = src;
  return u.d;
}

void func1() {
  // 编译器会将成员变量和成员函数分开存储：
  // 分别为每个对象的成员变量分配内存，但是所有对象都共享同一段函数代码。
  // 成员变量在堆区或栈区分配内存，成员函数在代码区分配内存。

  // 创建对象时，class可写可不写，但出于习惯通常会省略掉class关键字：
  A a1;
  class A a2;
  std::cout << (void *)&a1.a << std::endl;                    // 0x7ffdfa75b300
  std::cout << (void *)&a2.a << std::endl;                    // 0x7ffdfa75b30c
  std::cout << pointer_cast<void *>(&A::print) << std::endl;  // 0x55d5ca315504
  std::cout << union_cast<void *>(&A::print) << std::endl;    // 0x55d5ca315504

  // 类可以看做是一种复杂的数据类型，也可以使用sizeof求得该类型的大小。
  // 在计算类型的大小时，只计算了成员变量的大小，并没有把成员函数也包含在内。
  // 和结构体非常类似，对象也会有内存对齐的问题。
  std::cout << sizeof(A) << std::endl;   // 12
  std::cout << sizeof(a1) << std::endl;  // 12

  // 通过对象调用成员函数时，其实不是通过对象找函数，而是通过函数找对象。
  // 这一切都是隐式完成的，对程序员来说完全透明，就好像这个额外的参数不存在一样。
  a1.a = 'l';
  a1.testThis();                                       // 0x7fff32b77f90 12 l
  std::cout << static_cast<void *>(&a1) << std::endl;  // 0x7fff32b77f90
}

void func2() {
  A a;
  // 创建对象以后，可以使用点号.来访问成员变量和成员函数：
  a.testThis();  // 0x7ffc6f952c3c 12 h

  A *p = new A();
  // 有了对象指针后，可以通过箭头->来访问对象的成员变量和成员函数：
  p->testThis();  // 0x557a97c872c0 12 h
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 构造函数的名字和类名相同，没有返回值，不需要（也不能）显式调用，而是在创建对象时自动执行。
// 构造函数没有返回值，因为没有变量来接收返回值，这意味着：
// 1.不管是声明还是定义，函数名前面都不能出现类型，void也不允许；
// 2.函数体中不能有return语句。

// 创建对象时系统会自动调用构造函数进行初始化工作，同样，销毁对象时也会自动调用析构函数，
// 进行一些清理工作，例如释放分配的内存、关闭打开的文件等。
// 析构函数也是一种特殊的成员函数，没有返回值，不需要程序员显式调用（也没法显式调用），
// 而是在销毁对象时自动执行，构造函数的名字和类名相同，而析构函数的名字是在类名前面加一个~符号。
// 析构函数没有参数，不能被重载，因此一个类只能有一个析构函数。
// 如果用户没有定义，编译器会自动生成一个默认的析构函数。
// 函数命名中不允许出现~符号，析构函数是特殊情况，目的是为了和构造函数的名字加以对比和区分。

namespace test1 {
class A {
 private:
  A() {}

 public:
  static A *create() { return new A(); }
  void test() { std::cout << "this is A" << std::endl; }
};
void func() {
  // 构造函数必须是public属性的，否则创建对象时无法调用：
  // A a;             // 不能在栈上创建对象
  // A *p = new A();  // 不能在堆上创建对象
  // static A sa;     // 不能创建静态对象

  // 把构造函数私有化，通过静态函数在类内部创建对象，可以调用构造函数：
  A *p = A::create();
  p->test();  // this is A
}
}  // namespace test1

namespace test2 {
// 一个类必须有构造函数，要么用户自己定义，要么编译器自动生成。
// 用户自己没有定义构造函数，那么编译器会自动生成一个默认的构造函数，
// 只是这个构造函数的函数体是空的，也没有形参，也不执行任何操作。
// 实际上编译器只有在必要的时候才会生成默认构造函数，而且它的函数体一般不为空。
// 默认构造函数的目的是帮助编译器做初始化工作，而不是帮助程序员。
class A {};
void func() {
  A a1();  // 函数声明
  A a2;    // 调用没有参数的构造函数也可以省略括号

  A *p1 = new A();
  A *p2 = new A;  // 调用没有参数的构造函数也可以省略括号
}
}  // namespace test2

namespace test3 {
class A {
 public:
  A(int i) {}
};
void func() {
  A a1(5);
  // 一旦定义了构造函数，不管有几个，也不管形参如何，编译器都不再自动默认构造函数生成。
  // A a2;  // 调用默认构造函数
}
}  // namespace test3

namespace test4 {
// 和普通成员函数一样，构造函数是允许重载的。
// 一个类可以有多个重载的构造函数，创建对象时根据传递的实参来判断调用哪一个构造函数。
// 构造函数的调用是强制性的，一旦定义了，那么创建对象时就一定要调用，不调用是错的。
// 反过来说，创建对象时只有一个构造函数会被调用。
class A {
 public:
  A() : a(6) { std::cout << "A()" << std::endl; }
  A(int) { std::cout << "A(int)" << std::endl; }
  A(int, float) { std::cout << "A(int,float)" << std::endl; }
  int a;
};
void func() {
  A a1();        // 这是个函数声明
  A a2;          // A()
  A a3(5);       // A(int)
  A a4(7, 2.3);  // A(int,float)

  A *p1 = new A();  // A()
  A *p2 = new A;    // A()
}
}  // namespace test4

void func() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n2

namespace n3 {
// C++中的new和delete分别用来分配和释放内存，与malloc()、free()最大的一个不同在于：
// 用new分配内存时会调用构造函数，用delete释放内存时会调用析构函数。
// 构造函数和析构函数对于类来说是不可或缺的，所以在C++中鼓励使用new和delete。
// 析构函数的执行时机：
// 1.析构函数在对象被销毁时调用，而对象的销毁时机与它所在的内存区域有关；
// 2.在所有函数之外创建的对象是全局对象，它和全局变量类似，
//   位于内存分区中的全局数据区，程序在结束执行时会调用这些对象的析构函数；
// 3.在函数内部创建的对象是局部对象，它和局部变量类似，
//   位于栈区，函数执行结束时会调用这些对象的析构函数；
// 4.new创建的对象位于堆区，通过delete删除时才会调用析构函数，
//   如果没有delete，析构函数就不会被执行。

class A {
 public:
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }
};

A g;
void func() {
  A a;
  A *p = new A();
  delete p;
  p = (A *)malloc(sizeof(A));  // 不会调用构造函数
  free(p);                     // 不会调用析构函数
  // A
  // A
  // A
  // ~A
  // ~A
  // ~A
}
}  // namespace n3

namespace n4 {
// C++允许数组的每个元素都是对象，这样的数组称为对象数组：
class A {
 public:
  A() { std::cout << "A()" << std::endl; }
  A(int n) { std::cout << "A(int n)" << std::endl; }
};

void func1() {
  // 对象数组中的每个元素都需要用构造函数初始化。
  // 具体哪些元素用哪些构造函数初始化，取决于定义数组时的写法：
  A a1[2];
  // A()
  // A()

  A a2[2] = {4, 5};
  // A(int n)
  // A(int n)

  A a3[2] = {3};
  // A(int n)
  // A()

  A *a4 = new A[2];
  // A()
  // A()
  delete[] a4;
}

// 在构造函数有多个参数时，数组的初始化列表中要显式地包含对构造函数的调用：
class B {
 public:
  B(int n) { std::cout << "B(int n)" << std::endl; }
  B(int n, int m) { std::cout << "B(int n, int m)" << std::endl; }
  B() { std::cout << "B()" << std::endl; }
};

void func2() {
  B b1[3] = {1, B(1, 2)};
  // B(int n)
  // B(int n, int m)
  // B()

  B b2[3] = {B(2, 3), B(1, 2), 1};
  // B(int n, int m)
  // B(int n, int m)
  // B(int n)

  B *b3[3] = {new B(4), new B(1, 2)};
  // B(int n)
  // B(int n, int m)
}

void func() {
  func1();
  func2();
}
}  // namespace n4

namespace n5 {
// this是C++中的一个关键字，也是一个const指针，它指向当前对象。
// this只能在成员函数内部使用，用在其他地方没有意义，也是非法的。
// this虽然用在类的内部，但是只有在对象被创建以后才会给this赋值，
// 并且这个赋值的过程是编译器自动完成的，用户不需要也不能显式地给this赋值。

// this实际上是成员函数的一个形参，在调用成员函数时将对象的地址作为实参传递给this。
// 不过this这个形参是隐式的，它并不出现在代码中，
// 而是在编译阶段由编译器默默地将它添加到参数列表中。
// this为隐式形参，本质是成员函数的局部变量，所以只能用在成员函数内部，
// 并且只有在通过对象调用成员函数时才给this赋值。

// 成员函数最终被编译成与对象无关的普通函数，除了成员变量，
// 会丢失所有信息，所以编译时要在成员函数中添加一个额外的参数，
// 把当前对象的首地址传入，以此来关联成员函数和成员变量。
// 这个额外的参数，实际上就是this，它是成员函数和成员变量关联的桥梁。

class A {
 public:
  int a = 1;
  void testThis() {
    std::cout << "this :" << static_cast<void *>(this) << std::endl;

    // this只能用在类的内部，通过this可以访问所有private、protected、public成员：
    this->a = 5;
    std::cout << this->a << " " << (*this).b << " " << this->getC()
              << std::endl;

    // this是const指针，它的值是不能被修改的，
    // 一切企图修改该指针的操作，如赋值、递增、递减等都是不允许的：
    // this++;
  }

  static void print() {
    // 只有当对象被创建后this才有意义，因此不能在static成员函数中使用：
    // std::cout << this->a << " " << this->b << " " << this->c << std::endl;
  }

 protected:
  int b = 2;

 private:
  int getC() { return c; }
  int c = 3;
};

void func() {
  A a;
  std::cout << "addr :" << &a << std::endl;
  a.testThis();
  // addr :0x7ffd28ba489c
  // this :0x7ffd28ba489c
  // 5 2 3
}
}  // namespace n5

namespace n6 {
// 类其实也是一种作用域，每个类都会定义它自己的作用域。
// 在类的作用域之外，普通的成员只能通过对象来访问，可以是对象本身，
// 也可以是对象指针或对象引用，静态成员既可以通过对象访问，
// 又可以通过类访问，而typedef定义的类型只能通过类来访问。
// 一个类就是一个作用域的事实能够很好的解释：
// 为什么在类的外部定义成员函数时必须同时提供类名和函数名。
// 在类的外部，类内部成员的名字是不可见的。
class A {
 public:
  typedef int INT;
  static void show();
  INT work();

 private:
  int n;
};
void A::show() { std::cout << "show()" << std::endl; }

// 函数的返回值类型出现在函数名之前，当成员函数定义在类的外部时，
// 返回值类型中使用的名字都位于类的作用域之外，此时必须指明该名字的类名。
A::INT A::work() {
  std::cout << "work()" << std::endl;
  // 一旦遇到类名，定义的剩余部分就在类的作用域之内了，
  // 这里的剩余部分包括参数列表和函数体。
  // 结果就是，可以直接使用类的其他成员而无需再次授权了。
  n = 10;
  return n;
}

void func() {
  A a;
  a.work();       // 通过对象访问普通成员
  a.show();       // 通过对象访问静态成员
  A::show();      // 通过类访问静态成员
  A::INT n = 10;  // 通过类访问typedef定义的类型
}
}  // namespace n6

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    case 4:
      n5::func();
      break;
    case 5:
      n6::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}