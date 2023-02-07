#include <iostream>

// 执行期语义学

namespace n1 {
// 6.1 对象的构造和析构
// 一般而言，构造函数和析构函数的安插都如预期一样：
// {
//   Point point;
//   构造函数一般安插在这里
//   ...
//   析构函数被安插在这里
// }
// 如果区段或函数中有一个以上的离开点，情况会很混乱。
// 析构函数必须放在每一个离开点，比如switch和goto。

class A {
 public:
  A(int i) : _i(i) { std::cout << "A " << i << std::endl; }
  A() { std::cout << "A" << std::endl; }
  void print() { std::cout << _i << std::endl; }

 private:
  int _i;
};

// 全局对象
// C++保证，一定会在main()函数中第一次用到全局变量之前，
// 把全局对象构造出来，而在main()结束之前把全局变量摧毁掉。
// 全局对象如果有构造函数和析构函数的话，
// 可以说说它需要静态的初始化操作和内存释放操作。
// C++程序中的所有全局对象都被放置在程序的data segment中。
// 如果明确指定了值，对象将以该值为初值，否则，对象的初值为0。
// 这一点和C不同，C并不自动设置初值，
// 在C语言中一个全局对象只能被一个常量表达式（编译时期求值）设定初值。
// 构造函数并不是常量表达式，
// 虽然对象在编译时期可以被放置于data segment中并切内容为0，
// 但构造函数一直要到程序激活时才会执行。
// 必须对一个放置于program data segment中的对象的初始化表达式做评估，
// 这正是为什么一个对象需要静态初始化的原因。
// munch策略：
// 1.为每一个需要静态初始化的档案产生一个__sti()函数，
//   内带必要的构造函数调用操作或inline expansions。
// 2.类似的，在每一个需要静态的内存释放操作的文件中，产生一个__std()函数，
//   内带必要的析构函数调用，或是其inline expansions。
// 3.提供一组runtime library munch函数：
//   一个_main()函数，调用__sti()函数;
//   以及一个exit函数，调用__std()函数。
// 如何收集一个程序中各个文件的__sti()函数和__std()函数？
// nm命令（导出对象文件的符合表格）输出到munch程序，
// munch搜索以__sti或__std开头的名称，
// 然后把函数名称加到sti函数和std函数的跳离表格中。
// 接下来它把这个表格写到一个小的program text文件中，
// 然后CC命令被重新激活，将这个内含表格的文件加以编译。
// 使用静态初始化的对象有些缺点：
// 1.如果exception handing被支持，那些对象将不能够被放置于try区段之内。
//   这对于被静态调用的构造函数可能是特别无法接受的，因为任何的throw操作，
//   将必然触发exception handing library默认的terminate()函数。
// 2.另一个缺点是为了控制需要跨越模块做静态初始化对象的相依顺序而导致的复杂度。
A g1(1);
A g2;

// 局部静态对象
void test() { static A a(5); }
// 虽然test可能会被调用多次，但是w的构造函数和析构函数只能执行一次。
// 编译器的策略之一是：无条件地在程序起始时构造出对象来。
// 这会导致所有局部静态变量在程序起始时被初始化，即使它们所在的函数从不曾被调用过。
// 因此，只有在test调用时才构造，是比较好的做法，现在的C++标准已经强制要求这一点。
// 新的规则要求编译单位中的静态局部对象必须以构造的相反次序摧毁。
// 由于这些对象是在需要时才被构造，所以编译时无法预期其集合以及顺序。
// 为了支持新规则，可能需要对被产生出来的静态局部对象保存一个执行期链表。

// 对象数组

// 默认构造函数和数组
// 如果想要在程序中取出一个构造函数的地址，这是不可以的。

void func() {
  // A 1
  // A
  std::cout << "enter main" << std::endl;
  test();  // A 5
  test();
  g1.print();  // 1
  g2.print();  // 0
  std::cout << "exit main" << std::endl;
}
}  // namespace n1

namespace n2 {
// 6.2 new和delete运算符

// new由两个步骤完成：
// 1.通过适当的new运算符函数实体，配置所需的内存；
// 2.给配置得来的对象设立初值，初始化操作应该在内存分配成功后才执行。
// delete运算符的情况类似，如果操作数是0，C++要求delete运算符不要有操作。
// new操作符实际上总是以标准的C malloc()完成，delete总是以C free()完成。
// 标准并没有规定一定得这么做不可。

// 针对数组的new语意
// 主要功能是把默认构造函数执行于对象所组成的数组的每一个元素身上：
// int *array = new int[5];
// 最好避免以一个基类指针指向一个派生类对象所组成的数组。

// Placement Operator new的语意
// Point *ptw = new(arena) Point;
// arena指向内存中的一块区块，用以放置新产生出来的Point对象。
// 除了将获得的指针所指的地址传回外，
// 还要在arena所值得地址上执行Point的构造函数。
// 然而有一个轻微的不良行为：
// 在原已存在的一个对象上构造新的对象，
// 而该现有的对象有一个析构函数，这个析构函数并不会被调用。
// 如何知道所指的区域是否需要先析构呢？这个问题在语言层面上并没有解答。
// 一个习惯是，令执行new的这一端也要负责执行析构的责任。
// C++标准说，arena要不指向相同的类，要不就是一块新内存，足够容纳该对象。
// 1.新的存储空间：char *arena = new char[sizeof(Point2w)];
// 2.相同类型的对象：Point2w *arena = new Point2w;
// 一般而言，placement new operator并不支持多态。
// 被交给new的指针，应该适当地指向一块预先配置好的内存。
// 如果派生类比其基类大，派生类的构造函数将会导致严重的破坏。
class AA {
 public:
  AA() { std::cout << "AA" << std::endl; }
  ~AA() { std::cout << "~AA" << std::endl; }
};
void func1() {
  void *addr = malloc(50);
  AA *p = new (addr) AA;
  p = new (addr) AA;
  delete addr;
  // AA
  // AA
}
void func2() {
  void *addr = malloc(50);
  AA *p = new (addr) AA;  // AA

  // 会释放所指的内存，不是期望的：
  // delete p;  // ~AA

  // 明确地调用析构函数并保留存储空间：
  p->~AA();           // ~AA
  p = new (addr) AA;  // AA
  p->~AA();           // AA
}

// placement new operator最隐晦的问题：
struct Base {
  int j;
  virtual void f() { std::cout << "Base::f" << std::endl; }
};
struct Derived : Base {
  void f() { std::cout << "Derived::f" << std::endl; }
};
// Base和Derived有相同的大小，
// 故把派生类对象放在为基类配置的内存中是安全的。
// 然而，要支持这一点，
// 或许必须放弃对于经由对象静态调用所有虚函数通常会有的优化处理。
void func3() {
  Base b;
  b.f();  // Base::f
  b.~Base();

  // 下面的程序行为未定义，不确定调用哪个f，
  // 大部分使用者以为会调用Derived::f()，
  // 但大部分编译器调用的却是Base::f()：
  new (&b) Derived;
  b.f();  // Base::f
}
}  // namespace n2

namespace n3 {
// 6.3 临时性对象

struct T {
  T(int i) : _i(i) { std::cout << "T(int i) " << std::endl; }
  T() : _i(0) { std::cout << "T()" << std::endl; }
  T(const T &r) {
    _i = r._i;
    std::cout << "T copy" << std::endl;
  };
  int _i;
};
T operator+(const T &rhs, const T &lhs) { return T(rhs._i + lhs._i); }
// C++标准允许编译对于临时性对象的产生有完全的自由度。
// 但实际上，由于市场竞争，几乎保证：
// 表达式T c = a + b;实现时根本不产生一个临时对象。
// 以一连串的析构和拷贝构造来取代赋值操作符，
// 一般而言是不安全的，而且会产生临时对象。
// 所以初始化操作：T c = a + b;
// 总是比赋值操作：c = a + b;更有效率地被编译器优化。
// 没有出现目标对象a + b有必要产生一个临时对象，
// 以放在运算后的结果，这种情况在子表达式中十分普遍。

// 临时对象的生命期，在标准C++之前，
// 临时对象的生命并没有明确指定，而是由编译厂商自行决定。
// 在C++标准下，临时对象被摧毁，
// 应该是对完整表达式（整个表达式）求值过程中的最后一个步骤。
// 临时对象的生命期规则有两个例外：
// 1.发生在表达式被用来初始化一个对象时。
//   String s = b ? a + b;
//   产生临时对象存放加法结果，临时对象必须根据b的测试结果来有条件地析构。
//   在临时对象的生命规则下，它应该在完整的?:表达式结束评估后尽快被摧毁。
//   如果s的初始化需要调用一个拷贝构造函数，那么临时对象的析构不是所期望的。
//   C++标准说：
//   凡是表达式执行结果的临时性对象，应该保留到对象的初始化操作完成为止。
// 2.当一个临时性对象被一个引用绑定时。
//   const string &space = " ";
//   如果被销毁，那么引用也就没什么用了。
//   如果一个临时性对象被引用绑定，对象将残留，直到被初始化的引用的生命结束，
//   或直到临时对象的生命周期结束，视哪一种情况先到达而定。

void func() {
  T x, y;
  // T()
  // T()

  x + y;  // T(int i)
  // 可能会导致一个临时对象，以放置传回的对象。
  // 是否生成临时对象，视编译器的进取性以及上述操作发生时的程序上下文而定。
  // 此外，视operator+()的定义而定，NRV优化也可能实施起来。

  T z = x + y;  // T(int i)  初始化操作
  // 编译器会产生一个临时性对象，放置x+y的结果，
  // 然后再使用T的拷贝构造函数，把临时性对象当做z的初始值。
  // 比较更可能的转换是直接以拷贝构造函数的方式，将x+y的值放到z中，
  // 于是就不需要临时性对象，以及构造和析构的调用了。
  // NRV优化后，将导致直接在z对象中求表达式结果，
  // 避免执行拷贝构造函数和具名对象的析构函数。
  // 三种方式所获得的z对象，结果都一样，其差异在于初始化的成本。
  z = x + y;  // T(int i) 0  z.operator=(x + y);
  // 赋值语句，不能忽略临时性对象。

  // g++ -fno-elide-constructors runtime.cpp
  // T()
  // T()

  // T(int i)
  // T copy

  // T(int i)
  // T copy
  // T copy

  // T(int i)
  // T copy
}

// 临时性对象的迷思（神话、传说）
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int tD1pe = atoi(argv[1]);
  switch (tD1pe) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    case 4:
      n3::func();
      break;
    default:
      std::cout << "invalid tD1pe" << std::endl;
      break;
  }

  return 0;
}