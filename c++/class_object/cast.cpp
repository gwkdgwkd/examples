#include <iostream>

// 在C/C++中，不同的数据类型之间可以相互转换：
// 1.无需用户指明如何转换的称为自动类型转换（隐式类型转换）;
// 2.需要用户显式地指明如何转换的称为强制类型转换（显式类型转换）。
// 隐式类型转换利用的是编译器内置的转换规则，
// 或者用户自定义的转换构造函数以及类型转换函数（已知的转换规则），
// 例如从int到double、从派生类到基类、从type *到void *、从double到Complex等。
// type*是一个具体类型的指针，例如int*、double*、Student*等，它们都可以直接赋值给void*指针。
// 而反过来是不行的，必须使用强制类型转换才能将void*转换为type*，
// 例如，malloc()分配内存后返回的就是一个void *指针，我们必须进行强制类型转换后才能赋值给指针变量。
// 当隐式转换不能完成类型转换工作时，就必须使用强制类型转换了。
// 强制类型转换的语法也很简单，只需要在表达式的前面增加新类型的名称，格式为：(new_type) expression

// 类型转换的本质：
// 数据是放在内存中的，变量（以及指针、引用）是给这块内存起的名字，有了变量就可以找到并使用这份数据。
// 内存中的数据有多种解释方式，使用之前必须要确定。
// 这种确定数据的解释方式的工作就是由数据类型（Data Type）来完成的。
// 数据类型用来说明数据的类型，确定了数据的解释方式，让计算机和程序员不会产生歧义。
// 所谓数据类型转换，就是对数据所占用的二进制位做出重新解释。
// 如果有必要，在重新解释的同时还会修改数据，改变它的二进制位。

// 隐式类型转换和显式类型转换最根本的区别是：
// 1.隐式类型转换除了会重新解释数据的二进制位，还会利用已知的转换规则对数据进行恰当地调整；
// 2.而显式类型转换只能简单粗暴地重新解释二进制位，不能对数据进行任何调整。
// 其实，能不能对数据进行调整是显而易见地事情，有转换规则就可以调整，没有转换规则就不能调整，
// 当进行数据类型转换时，编译器明摆着是知道有没有转换规则的。
// 站在这个角度考虑，强制类型转换的语法就是多此一举，编译器完全可以自行判断是否需要调整数据。
// 例如从int*转换到float*，加不加强制类型转换的语法都不能对数据进行调整。
// C/C++之所以增加强制类型转换的语法，是为了提醒程序员这样做存在风险，一定要谨慎小心。
// 说得通俗一点，你现在的类型转换存在风险，你自己一定要知道。

// 为了使潜在风险更加细化，使问题追溯更加方便，使书写格式更加规范，C++对类型转换进行了分类，
// 并新增了四个关键字来予以支持，它们分别是：
// 1.static_cast 	      用于良性转换，一般不会导致意外发生，风险很低。
// 2.const_cast 	      用于const与非const、volatile与非volatile之间的转换。
// 3.dynamic_cast 	    借助RTTI，用于类型安全的向下转型（Downcasting）。
// 4.reinterpret_cast 	高度危险的转换，这种转换仅仅是对二进制位的重新解释，
//                      不会借助已有的转换规则对数据进行调整，但是可以实现最灵活的C++类型转换。

namespace n1 {
// 对于隐式类型转换，编译器可以根据已知的转换规则来决定是否需要修改数据的二进制位；
// 而对于强制类型转换，由于没有对应的转换规则，所以能做的事情仅仅是重新解释数据的二进制位，
// 但无法对数据的二进制位做出修正。
// 这就是隐式类型转换和强制类型转换最根本的区别。
// 这里说的修改数据并不是修改原有的数据，而是修改它的副本（先将原有数据拷贝到另外一个地方再修改）。
// 修改数据的二进制位非常重要，它能把转换后的数据调整到正确的值，所以这种修改时常会发生，例如：
// 1.整数和浮点数在内存中的存储形式大相径庭，将浮点数f赋值给整数i时，不能原样拷贝f的二进制位，
//   也不能截取部分二进制位，必须先将f的二进制位读取出来，然后直接截掉小数部分，
//   把剩下的整数部分再转换成二进制形式，拷贝到i所在的内存中。
// 2.short一般占用两个字节，int一般占用四个字节，将short类型的s赋值给int类型的i时，
//   如果仅仅是将s的二进制位拷贝给i，那么i最后的两个字节会原样保留，
//   这样会导致赋值结束后i的值并不等于s的值，所以这样做是错误的。
//   正确的做法是，先给s添加16个二进制位（两个字节）并全部置为0，然后再拷贝给i所在的内存。
// 3.当存在多重继承时，如果把派生类指针pd赋值给基类指针pb，就必须考虑基类子对象在派生类对象中的偏移，
//   偏移不为0时就要调整pd的值，让它加上或减去偏移量，这样赋值后才能让pb恰好指向基类子对象。
// 4.Complex占用16个字节，double占用8个字节，将double类型的数据赋值给Complex类型的变量（对象）时，
//   必须调用转换构造函数，否则剩下的8个字节就不知道如何填充了。
// 以上这些都是隐式类型转换，它对数据的调整都是有益的，能够让程序更加安全稳健地运行。
// 隐式类型转换必须使用已知的转换规则，虽然灵活性受到了限制，
// 但是由于能够对数据进行恰当地调整，所以更加安全（几乎没有风险）。
// 强制类型转换能够在更大范围的数据类型之间进行转换，
// 例如不同类型指针（引用）之间的转换、从const到非const的转换、
// 从int到指针的转换（有些编译器也允许反过来）等，这虽然增加了灵活性，
// 但是由于不能恰当地调整数据，所以也充满了风险，要小心使用。

class Base {
 public:
  Base(int a = 0, int b = 0) : m_a(a), m_b(b) {}
  void print() { std::cout << m_a << " " << m_b << std::endl; }

 private:
  int m_a;
  int m_b;
};

void func1() {  // 风险1：破坏类的封装性
  Base *pb = new Base(10, 20);
  int n = *((int *)pb + 1);
  std::cout << n << std::endl;  // 20

  *((int *)pb) = 80;
  *((int *)pb + 1) = 90;
  pb->print();  // 80 90
}

void func2() {  // 风险2：进行无意义的操作
  float f = 56.2;
  int *pi = (int *)&f;
  *pi = -23;
  std::cout << f << std::endl;  // -nan
  // NaN是“not a number”的缩写，意思是“不是一个数字”。
}

void testN1() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 类型转换只能发生在相关类型或者相近类型之间，
// 两个毫不相干的类型不能相互转换，即使使用强制类型转换也不行：
class A {};
class B {};
class Base {};
class Derived : public Base {};
void func1() {
  A a;
  B b;
  Base obj1;
  Derived obj2;
  // a = (A)b;  // Error: 两个没有继承关系的类不能相互转换
  // int n = (int)a;  // Error: 类类型不能向基本类型转换
  // int *p = (int *)b;  // Error: 指针和类类型之间不能相互转换
  // obj2 = (Derived)obj1;  // Error: 基类不能向派生类转换，向下转型
  obj1 = obj2;  // Correct: 向上转型
}

// 隐式类型转换是安全的，显式类型转换是有风险的。
// C语言之所以增加强制类型转换的语法，就是为了强调风险，让程序员意识到自己在做什么。
// 但是，这种强调风险的方式还是比较粗放，粒度比较大，它并没有表明存在什么风险，风险程度如何。
// 再者，C风格的强制类型转换统一使用()，而()在代码中随处可见，
// 所以也不利于使用文本检索工具定位关键代码。
void func2() {
  int i = 97;
  std::cout << i << std::endl;        // 97
  std::cout << (char)i << std::endl;  // a
}

void testN2() {
  func1();
  func2();
}
}  // namespace n2

namespace n3 {
// static_cast关键字：
// static_cast只能用于良性转换，这样的转换风险较低，一般不会发生什么意外，例如：
// 1.原有的自动类型转换，例如short转int、int转double、const转非const、向上转型等；
// 2.void指针和具体类型指针之间的转换，例如void*转int*、char*转void*等；
// 3.有转换构造函数或者类型转换函数的类与其它类型之间的转换，
//   例如double转Complex（调用转换构造函数）、Complex转double（调用类型转换函数）。
// static_cast是“静态转换”的意思，也就是在编译期间转换，转换失败的话会抛出一个编译错误。

class Complex {
 public:
  Complex(double real = 0.0, double imag = 0.0) : m_real(real), m_imag(imag) {}

 public:
  operator double() const { return m_real; }  // 类型转换函数
 private:
  double m_real;
  double m_imag;
};

void func1() {
  int m = 100;
  long n = static_cast<long>(m);   // 宽转换，没有信息丢失
  char ch = static_cast<char>(m);  // 窄转换，可能会丢失信息

  // 将void指针转换为具体类型指针
  int *p1 = static_cast<int *>(malloc(10 * sizeof(int)));
  void *p2 = static_cast<void *>(p1);  // 将具体类型指针，转换为void指针

  Complex c(12.5, 23.8);
  double real = static_cast<double>(c);  // 调用类型转换函数

  const int i = static_cast<const int>(m);
  std::cout << i << std::endl;  // 100
  // i = 5;
}

// 需要注意的是，static_cast不能用于无关类型之间的转换，因为这些转换都是有风险的，例如：
// 1.两个具体类型指针之间的转换，例如int*转double*、Student*转int*等。
// 2.不同类型的数据存储格式不一样，长度也不一样，
//   用A类型的指针指向B类型的数据后，会按照A类型的方式来处理数据：
//   a.如果是读取操作，可能会得到一堆没有意义的值；
//   b.如果是写入操作，可能会使B类型的数据遭到破坏，
//     当再次以B类型的方式读取数据时会得到一堆没有意义的值。
// 3.int和指针之间的转换，将一个具体的地址赋值给指针变量是非常危险的，
//   因为该地址上的内存可能没有分配，也可能没有读写权限，恰好是可用内存反而是小概率事件。
// static_cast也不能用来去掉表达式的const修饰和volatile修饰。
// 换句话说，不能将const/volatile类型转换为非const/volatile类型。
void func2() {
  int *p;
  // float *fp1 = static_cast<float *>(p);  // 不能在两个具体类型的指针之间进行转换
  // float *fp2 = static_cast<float *>(0X2DF9);  // 不能将整数转换为指针类型

  const int i1 = 5;
  // int *i2 = static_cast<int *>(&i1);
  volatile int i3 = 6;
  // int *i4 = static_cast<int *>(&i3);
}

void testN3() {
  func1();
  func2();
}
}  // namespace n3

namespace n4 {
// const_cast关键字：
// const_cast比较好理解，它用来去掉表达式的const修饰或volatile修饰。
// 换句话说，const_cast就是用来将const/volatile类型转换为非const/volatile类型。
// 使用const_cast进行强制类型转换可以突破C/C++的常数限制，修改常数的值，因此有一定的危险性；
// 但如果这样做的话，基本上会意识到这个问题，因此也还有一定的安全性。

void testN4() {
  const int n = 100;
  int *p = const_cast<int *>(&n);
  *p = 234;
  std::cout << "n = " << n << std::endl;  // n = 100
  // 被替换成了：cout<<"n = "<<100<<endl;
  std::cout << "*p = " << *p << std::endl;  // *p = 234
  // 为什么通过n和*p输出的值不一样呢？
  // 这是因为C++对常量的处理更像是编译时期的#define，是一个值替换的过程，
  // 代码中所有使用n的地方在编译期间就被替换成了100，这样一来，
  // 即使程序在运行期间修改n的值，也不会影响cout语句了。

  volatile int v = 6;
  int *vp = const_cast<int *>(&v);
  std::cout << "*vp = " << *vp << std::endl;  // *vp = 6
}
}  // namespace n4

namespace n5 {
// reinterpret_cast关键字：
// reinterpret是“重新解释”的意思，顾名思义，reinterpret_cast这种转换仅仅是对二进制位的重新解释，
// 不会借助已有的转换规则对数据进行调整，非常简单粗暴，所以风险很高。
// reinterpret_cast可以认为是static_cast的一种补充，
// 一些static_cast不能完成的转换，就可以用reinterpret_cast来完成，
// 例如两个具体类型指针之间的转换、int和指针之间的转换（有些编译器只允许int转指针，不允许反过来）。
class A {
 public:
  A(int a = 0, int b = 0) : m_a(a), m_b(b) {}

 private:
  int m_a;
  int m_b;
};

void testN5() {
  // 将char*转换为float*
  char str[] = "http://c.biancheng.net";
  float *fp = reinterpret_cast<float *>(str);
  std::cout << *fp << std::endl;  // 3.0262e+29

  // 将int转换为int*
  int *p = reinterpret_cast<int *>(100);
  std::cout << p << std::endl;  // 0x64

  // 将A*转换为int*
  p = reinterpret_cast<int *>(new A(25, 96));
  std::cout << *p << std::endl;  // 25
}
}  // namespace n5

namespace n6 {
// dynamic_cast关键字:
// dynamic_cast用于在类的继承层次之间进行类型转换，
// 它既允许向上转型（Upcasting），也允许向下转型（Downcasting）。
// 向上转型是无条件的，不会进行任何检测，所以都能成功；
// 向下转型的前提必须是安全的，要借助RTTI进行检测，所有只有一部分能成功。

// dynamic_cast与static_cast是相对的，
// dynamic_cast是“动态转换”的意思，static_cast是“静态转换”的意思。
// dynamic_cast会在程序运行期间借助RTTI进行类型转换，这就要求基类必须包含虚函数；
// static_cast在编译期间完成类型转换，能够更加及时地发现错误。

// dynamic_cast的语法格式为：
// dynamic_cast <newType> (expression)
// newType和expression必须同时是指针类型或者引用类型。
// 换句话说，dynamic_cast只能转换指针类型和引用类型，
// 其它类型（int、double、数组、类、结构体等）都不行。
// 对于指针，如果转换失败将返回NULL；
// 对于引用，如果转换失败将抛出std::bad_cast异常。

// 向上转型时，只要待转换的两个类型之间存在继承关系，
// 并且基类包含了虚函数（编译期间就能确定），就一定能转换成功。
// 因为向上转型始终是安全的，所以dynamic_cast不会进行任何运行期间的检查，
// 这时dynamic_cast和static_cast没有区别。
// 向上转型时不执行运行期检测虽然提高了效率，但也留下了安全隐患:
class Base {
 public:
  Base(int a = 0) : m_a(a) {}
  int get_a() const { return m_a; }
  virtual void func() const {}

 protected:
  int m_a;
};
class Derived : public Base {
 public:
  Derived(int a = 0, int b = 0) : Base(a), m_b(b) {}
  int get_b() const { return m_b; }

 private:
  int m_b;
};

void func1() {
  Derived *pd = new Derived(35, 78);
  Base *pb = dynamic_cast<Derived *>(pd);
  std::cout << "pd = " << pd << std::endl;  // pd = 0x55662045beb0
  std::cout << "pb = " << pb << std::endl;  // pb = 0x55662045beb0
  std::cout << pb->get_a() << std::endl;    // 35
  pb->func();
}
void func2() {
  int n = 100;
  Derived *pd = reinterpret_cast<Derived *>(&n);
  Base *pb = dynamic_cast<Base *>(pd);
  std::cout << "pd = " << pd << std::endl;  // pd = 0x7fff01ecfb34
  std::cout << "pb = " << pb << std::endl;  // pb = 0x7fff01ecfb34
  std::cout << pb->get_a() << std::endl;    // 22036，垃圾值
  // pb->func();  // 段错误

  // pd指向的是整型变量n，并没有指向一个Derived类的对象，
  // 在使用dynamic_cast进行类型转换时也没有检查这一点，
  // 而是将pd的值直接赋给了pb（这里并不需要调整偏移量），最终导致pb也指向了n。
  // 因为pb指向的不是一个对象，所以get_a()得不到m_a的值，pb->func()也得不到func()函数的正确地址。
}

// 向下转型是有风险的，dynamic_cast会借助RTTI信息进行检测，确定安全的才能转换成功，否则就转换失败。
// 当使用dynamic_cast对指针进行类型转换时，程序会先找到该指针指向的对象，
// 再根据对象找到当前类（指针指向的对象所属的类）的类型信息，并从此节点开始沿着继承链向上遍历，
// 如果找到了要转化的目标类型，那么说明这种转换是安全的，就能够转换成功，
// 如果没有找到要转换的目标类型，那么说明这种转换存在较大的风险，就不能转换。
// 总起来说，dynamic_cast会在程序运行过程中遍历继承链，如果途中遇到了要转换的目标类型，
// 那么就能够转换成功，如果直到继承链的顶点（最顶层的基类）还没有遇到要转换的目标类型，那么就转换失败。
// 对于同一个指针，它指向的对象不同，会导致遍历继承链的起点不一样，
// 途中能够匹配到的类型也不一样，所以相同的类型转换产生了不同的结果。
class A {
 public:
  virtual void func() const { std::cout << "Class A" << std::endl; }

 private:
  int m_a;
};
class B : public A {
 public:
  virtual void func() const { std::cout << "Class B" << std::endl; }

 private:
  int m_b;
};
class C : public B {
 public:
  virtual void func() const { std::cout << "Class C" << std::endl; }

 private:
  int m_c;
};
class D : public C {
 public:
  virtual void func() const { std::cout << "Class D" << std::endl; }

 private:
  int m_d;
};

void func3() {  // 向下转型失败
  A *pa = new A();
  B *pb;
  C *pc;
  pb = dynamic_cast<B *>(pa);
  if (pb == nullptr) {
    std::cout << "Downcasting failed: A* to B*" << std::endl;
  } else {
    std::cout << "Downcasting successfully: A* to B*" << std::endl;
    pb->func();
  }
  pc = dynamic_cast<C *>(pa);
  if (pc == nullptr) {
    std::cout << "Downcasting failed: A* to C*" << std::endl;
  } else {
    std::cout << "Downcasting successfully: A* to C*" << std::endl;
    pc->func();
  }
  // Downcasting failed: A* to B*
  // Downcasting failed: A* to C*

  A a;
  try {
    B &b = dynamic_cast<B &>(a);
  } catch (std::bad_cast e) {
    std::cout << "catch exception" << e.what() << std::endl;
  }
  try {
    C &c = dynamic_cast<C &>(a);
  } catch (std::bad_cast e) {
    std::cout << "catch exception" << e.what() << std::endl;
  }
  // catch exceptionstd::bad_cast
  // catch exceptionstd::bad_cast
}

void func4() {                    // 向下转型成功
  A *pa = new D();                // 向上转型都是允许的
  pa->func();                     // Class D
  B *pb = dynamic_cast<B *>(pa);  // 向下转型成功
  if (pb == NULL) {
    std::cout << "Downcasting failed: A* to B*" << std::endl;
  } else {
    std::cout << "Downcasting successfully: A* to B*" << std::endl;
    pb->func();
  }
  C *pc = dynamic_cast<C *>(pa);  // 向下转型成功
  if (pc == NULL) {
    std::cout << "Downcasting failed: A* to C*" << std::endl;
  } else {
    std::cout << "Downcasting successfully: A* to C*" << std::endl;
    pc->func();
  }
  // Downcasting successfully: A* to B*
  // Class D
  // Downcasting successfully: A* to C*
  // Class D

  D d;
  A &a = d;  // 向上转型都是允许的
  a.func();  // Class D
  try {
    B &b = dynamic_cast<B &>(a);
    b.func();
  } catch (std::bad_cast e) {
    std::cout << "catch exception" << e.what() << std::endl;
  }
  try {
    C &c = dynamic_cast<C &>(a);
    c.func();
  } catch (std::bad_cast e) {
    std::cout << "catch exception" << e.what() << std::endl;
  }
  // Class D
  // Class D
}

// 总结：
// pa是A*类型的指针，当pa指向A类型的对象时，向下转型失败，pa不能转换为B*或C*类型。
// 当pa指向D类型的对象时，向下转型成功，pa可以转换为B*或C*类型。
// 同样都是向下转型，为什么pa指向的对象不同，转换的结果就大相径庭呢？
// pa指向A类对象，根据该对象找到的就是A的类型信息，当程序从这个节点开始向上遍历时，
// 发现A的上方没有要转换的B类型或C类型（实际上A的上方没有任何类型了），所以就转换败了。
// pa指向D类对象，根据该对象找到的就是D的类型信息，程序从这个节点向上遍历的过程中，
// 发现了C类型和B类型，所以就转换成功了。
// 从表面上看起来dynamic_cast确实能够向下转型，成功地将pa从A类型指针转换成了B和C类型指针。
// 但是从本质上讲，dynamic_cast还是只允许向上转型，因为它只会向上遍历继承链。
// 造成这种假象的根本原因在于，派生类对象可以用任何一个基类的指针指向它，这样做始终是安全的。
// pa指向的对象是D类型的，pa、pb、pc都是D的基类的指针，所以它们都可以指向D类型的对象，
// dynamic_cast只是让不同的基类指针指向同一个派生类对象罢了。

void testN6() {
  func1();
  func2();
  func3();
  func4();
}
}  // namespace n6

namespace n7 {
// dynamic_cast用于不同接口之间的转换
struct interfaceA {
  virtual void funcA() = 0;
};
struct interfaceB {
  virtual void funcB() = 0;
};
class C : public interfaceA, public interfaceB {
  virtual void funcA() override { std::cout << "funcA" << std::endl; }
  virtual void funcB() override { std::cout << "funcB" << std::endl; }
};

void testN7() {
  C *c = new C();
  interfaceA *a = dynamic_cast<interfaceA *>(c);
  a->funcA();  // funcA
  interfaceB *b = dynamic_cast<interfaceB *>(c);
  b->funcB();  // funcB
}

}  // namespace n7

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    case 4:
      n5::testN5();
      break;
    case 5:
      n6::testN6();
      break;
    case 6:
      n7::testN7();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}