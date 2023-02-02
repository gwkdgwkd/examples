#include <iostream>

// 将非尾端类设计为抽象类

namespace n1 {
class Base {
 public:
  Base() { a = 0; }
  Base(int i) : a(i) {}
  virtual ~Base() {}
  Base& operator=(const Base& rhs) {
    std::cout << "Base::operator=" << std::endl;
    a = rhs.a;
  }
  virtual void display() { std::cout << a << std::endl; }

 private:
  int a;
};
class Derived : public Base {
 public:
  Derived() { b = 0; }
  Derived(int i) : b(i), Base(i * 10) {}
  Derived& operator=(const Derived& rhs) {
    std::cout << "Derived::operator=" << std::endl;
    b = rhs.b;
    Base::operator=(rhs);
  }
  virtual void display() {
    Base::display();
    std::cout << b << std::endl;
  }

 private:
  int b;
};
void func() {
  Derived d1(3);
  d1.display();
  // 30
  // 3

  Derived d2(4);
  d2.display();
  // 40
  // 4

  Base* pb1 = &d1;
  Base* pb2 = &d2;
  *pb1 = *pb2;
  d1.display();
  // Base::operator=
  // 40
  // 3

  // 问题：
  // 1.最后一行的赋值运算调用的是Base类的，虽然相关对象的类型是Derived。
  //   结果，只有d1的Base部分被修改，这是部分赋值。
  //   在赋值后，d1的Base成员有了来自于de的值，但其Derived成员部分没被改变。
  // 2.真的有程序员把代码写成这样，用指针来给对象赋值并不少见，
  //   特别是那些对C有丰富经验而转移到C++的程序员。
}
}  // namespace n1

namespace n2 {
// 一个解决方法是将赋值运算申明为虚函数：
class Base {
 public:
  Base() { a = 0; }
  Base(int i) : a(i) {}
  virtual ~Base() {}
  virtual Base& operator=(const Base& rhs) {
    std::cout << "Base::operator=" << std::endl;
    a = rhs.a;
  }
  virtual void display() { std::cout << a << std::endl; }

 private:
  int a;
};

namespace test1 {
class Derived : public Base {
 public:
  Derived() { b = 0; }
  Derived(int i) : b(i), Base(i * 10) {}

  // 基于C++语言最近作出的修改，可以修改返回值的类型，
  // 于是每个都返回正确的类的引用，但C++的规则强迫声明相同的参数类型。
  // 这意味着Derived类的赋值操作，必须准备接受任意类型的Base对象。
  virtual Derived& operator=(const Base& rhs) {
    std::cout << "Derived::operator=" << std::endl;
    // 这个函数只在rhs确实是Derived类型时将它赋给*this。
    // 如果rhs不是Derived类型，dynamic_cast失败时抛的bad_cast异常。
    // 即使不在乎有异常，这个函数看起来也是没必要的复杂和昂贵，
    // dynamic_cast必要引用一个type_info结构。
    // 而且通常情况下都是一个Derived对象赋给另一个。
    b = dynamic_cast<const Derived&>(rhs).b;
    Base::operator=(rhs);
  }
  virtual void display() {
    Base::display();
    std::cout << b << std::endl;
  }

 private:
  int b;
};
void func() {
  Derived d1(3);
  d1.display();
  // 30
  // 3

  Derived d2(4);
  d2.display();
  // 40
  // 4

  Base* pb1 = &d1;
  Base* pb2 = &d2;
  *pb1 = *pb2;
  d1.display();
  // Derived::operator=
  // Base::operator=
  // 40
  // 4

  d1 = d2;
  // Base::operator=
  // ？
}
}  // namespace test1

namespace test2 {
class Derived : public Base {
 public:
  Derived() { b = 0; }
  Derived(int i) : b(i), Base(i * 10) {}

  virtual Derived& operator=(const Base& rhs) {
    std::cout << "Derived::operator=(const Base&)" << std::endl;
    // 现在这个函数试图将rhs转换为一个Derived。
    // 如果转换成功，通常的赋值操作被调用；否则，一个bad_cast异常被抛出。
    return operator=(dynamic_cast<const Derived&>(rhs));
  }
  // 为了处理Derived间的赋值，而无需增加复杂度或花费dynamic_cast，
  // 只要在Derived中增加一个通用形式的赋值操作：
  virtual Derived& operator=(const Derived& rhs) {
    std::cout << "Derived::operator=(const Derived&)" << std::endl;
    b = rhs.b;
    Base::operator=(rhs);
  }
  virtual void display() {
    Base::display();
    std::cout << b << std::endl;
  }

 private:
  int b;
};
void func() {
  Derived d1(3);
  Derived d2(4);
  Base* pb1 = &d1;
  Base* pb2 = &d2;

  *pb1 = *pb2;
  // Derived::operator=(const Base&)
  // Derived::operator=(const Derived&)
  // Base::operator=

  d1 = d2;
  // Derived::operator=(const Derived&)
  // Base::operator=
}
}  // namespace test2

namespace test3 {
class Derived1 : public Base {
 public:
  Derived1() {}
  Derived1(int i) : Base(i) {}
  virtual Derived1& operator=(const Base& rhs) {
    std::cout << "Derived1::operator=(const Base&)" << std::endl;
    return operator=(dynamic_cast<const Derived1&>(rhs));
  }
  virtual Derived1& operator=(const Derived1& rhs) {
    std::cout << "Derived1::operator=(const Derived1&)" << std::endl;
    Base::operator=(rhs);
  }
  virtual void display() {}
};
class Derived2 : public Base {
 public:
  Derived2() {}
  Derived2(int i) : Base(i) {}
  virtual Derived2& operator=(const Base& rhs) {
    std::cout << "Derived2::operator=(const Base&)" << std::endl;
    return operator=(dynamic_cast<const Derived2&>(rhs));
  }
  virtual Derived2& operator=(const Derived2& rhs) {
    std::cout << "Derived2::operator=(const Derived2&)" << std::endl;
    Base::operator=(rhs);
  }
  virtual void display() {}
};
void func() {
  // 在运行期使用dynamic_cast进行类型检测，这令人很紧张。
  // 有一件事要注意，一些编译器仍然没有支持dynamic_cast，
  // 所以使用它的代码虽然理论上具有可移植性，实际上不一定。
  // 更重要的是，它要求使用Derived1和Derived2的用户，
  // 必须在每次赋值操作时都准备好捕获bad_cast异常的准备。

  Derived1 d1_1;
  Derived1 d1_2;
  Derived2 d2;

  d1_1 = d1_2;
  // Derived1::operator=(const Derived1&)
  // Base::operator=

  try {
    d1_1 = d2;  // 不同类型的对象，可以直接赋值了
  } catch (const std::bad_cast& e) {
    std::cout << e.what() << std::endl;
  }
  // Derived1::operator=(const Base&)
  // std::bad_cast

  Base* pb1 = &d1_1;
  Base* pb2 = &d2;
  try {
    *pb1 = *pb2;
  } catch (const std::bad_cast& e) {
    std::cout << e.what() << std::endl;
  }
  // Derived1::operator=(const Base&)
  // std::bad_cast

  // 这是一个混合类型赋值：左边是一个Derived1，右边是一个Derived2。
  // 混合类型赋值在C++中通常不是问题，因为C++的强类型原则将评定它们非法。
  // 然而，通过将Base的赋值操作设为虚函数，打开了混合类型操作的门。
  // 这使得处境艰难，应该允许通过指针进行同类型赋值，
  // 而禁止通过同样的指针进行混合类型赋值。
  // 只能在运行期区分它们，因为将*pb2赋给*pb1有时是正确的，有时不是。
  // 于是陷入了基类型运行期错误的黑暗世界中。
  // 尤其是，需要在混合类型赋值时指出在operator=内部发生了错误，
  // 而类型相同时，期望按通常的方式完成赋值。
}
}  // namespace test3
}  // namespace n2

namespace n3 {
namespace test1 {
// 解决上面的问题，最容易的方法是在Base中将operator=置为private。
// 于是，同类型对象可以赋值，部分或混合类型赋值被禁止：
class Base {
 public:
  Base() { a = 0; }
  Base(int i) : a(i) {}
  virtual ~Base() {}
  virtual void display() {}

 private:
  // 它也使得不可能正确实现Derived类的赋值操作，
  // 因为派生类的赋值操作函数有责任调用其基类的赋值操作函数：
  virtual Base& operator=(const Base& rhs) {}
  int a;
};
class Derived : public Base {
 public:
  Derived() { b = 0; }
  Derived(int i) : b(i), Base(i * 10) {}
  virtual Derived& operator=(const Base& rhs) {
    return operator=(dynamic_cast<const Derived&>(rhs));
  }
  virtual Derived& operator=(const Derived& rhs) {
    b = rhs.b;
    // Base的operator=为私有的，下面的语句编译报错：
    // Base::operator=(rhs);
  }
  virtual void display() {}

 private:
  int b;
};
}  // namespace test1

namespace test2 {
// 将Base::operator=声明为protected：
class Base {
 public:
  Base() { a = 0; }
  Base(int i) : a(i) {}
  virtual ~Base() {}

 protected:
  virtual Base& operator=(const Base& rhs) {
    std::cout << "Base::operator=" << std::endl;
    a = rhs.a;
  }

 private:
  int a;
};
class Derived1 : public Base {
 public:
  Derived1() { b = 0; }
  Derived1(int i) : b(i), Base(i * 10) {}
  virtual Derived1& operator=(const Base& rhs) {
    std::cout << "Derived1::operator=Base" << std::endl;
    return operator=(dynamic_cast<const Derived1&>(rhs));
  }
  virtual Derived1& operator=(const Derived1& rhs) {
    std::cout << "Derived1::operator=Derived1" << std::endl;
    b = rhs.b;
    Base::operator=(rhs);
  }

 private:
  int b;
};
class Derived2 : public Base {
 public:
  Derived2() { b = 0; }
  Derived2(int i) : b(i), Base(i * 20) {}
  virtual Derived2& operator=(const Base& rhs) {
    std::cout << "Derived2::operator=Base" << std::endl;
    return operator=(dynamic_cast<const Derived2&>(rhs));
  }
  virtual Derived2& operator=(const Derived2& rhs) {
    std::cout << "Derived2::operator=Derived2" << std::endl;
    b = rhs.b;
    Base::operator=(rhs);
  }

 private:
  int b;
};

void func() {
  Derived1 d1_1;
  Derived1 d1_2;
  d1_1 = d1_2;
  // Derived1::operator=Derived1
  // Base::operator=

  Derived2 d2_1;
  Derived2 d2_2;
  d2_1 = d2_2;
  // Derived2::operator=Derived2
  // Base::operator=

  // 阻止了Base不同派生类对象间通过Base指针进行部分赋值：
  Base* pb1 = &d1_1;
  Base* pb2 = &d2_1;
  // *pb1 = *pb2;

  // 不幸的是，Base也是实体类，同时将Base对象间的赋值评定为非法了：
  Base b1, b2;
  // b1 = b2;
}
}  // namespace test2

void func() { test2::func(); }
}  // namespace n3

namespace n4 {
// 还是不能同时实现：
// 1.允许Base对象间的赋值；
// 2.阻止Base不同派生类对象间通过Base的指针进行部分赋值；
// 程序该怎么办？
// 最容易的事情是排除Base对象间赋值的需求，
// 其最容易的实现方法是将Base设计为抽象类。
// 作为抽象类，Base不能被实例化，所以也就没有Base对象间赋值的需求了。
// 当然，这导致了一个新问题，因为最初的设计表明Base对象是必须的。
// 有一个很容易的解决方法：
// 不用将Base设为抽象类，创建一个新类叫AbstractBase，
// 来包含Base和Derived的共有属性，并把它设为抽象类，
// 然后将每个实体类从AbstractBase继承。

// 这个设计满足所有需要的东西：
// 1.同类型间的赋值被允许，部分赋值或不同类型间的赋值被禁止；
// 2.派生类的赋值操作函数可以调用基类的赋值操作函数。

// 此外，所有涉及Base、Derived1或Derived2类的代码都不需要修改，
// 因为这些类仍然操作，其行为与引入AbstractBase前保持了一致。
// 当然，代码需要重新编译，但这是为获得确保下面的条件付出很小的代价：
// 编译通过的赋值语句的行为是正确的而行为可能不正确的赋值语句不能编译通过。
class AbstractBase {
 protected:
  AbstractBase& operator=(const AbstractBase& rhs) {
    std::cout << "AbstractBase::operator=" << std::endl;
    a = rhs.a;
  }

 public:
  AbstractBase() { a = 0; }
  AbstractBase(int i) : a(i) {}

  // 要实现功能，AbstractBase类必须是抽象类，它必须至少有一个纯虚函数。
  // 大部分情况下，带一个这样的函数是没问题的，但在极少见的情况下，
  // 会发现需要创一个如AbstractBase这样的类，
  // 没有哪个成员函数是自然的纯虚函数。
  // 此时，传统方法是将析构函数申明为纯虚函数。
  virtual ~AbstractBase() = 0;

 private:
  int a;
};
AbstractBase::~AbstractBase() {
  // 为了支持多态，基类总需要虚析构函数，
  // 将它再多设为纯虚的唯一麻烦就是必须在类的定义之外实现它。

  // 声明一个函数为纯虚函数并不意味着它没有实现，它意味着：
  // 1.当前类是抽象类。
  // 2.任何从此类派生的实体类必须将此函数申明为一个普通的虚函数，
  //   也就是说，不能带=0。
  // 是的，绝大部分纯虚函数都没有实现，但纯虚析构函数是个特例。
  // 它们必须被实现，因为它们在派生类析构函数被调用时也将被调用。
  // 而且，它们经常执行有用的任务，诸如释放资源或纪录消息。
  // 实现纯虚函数一般不常见，但对纯虚析构函数，它不只是常见，它是必须。
}
class Base : AbstractBase {
 public:
  Base() {}
  virtual ~Base() {}

  Base& operator=(const Base& rhs) {
    std::cout << "Base::operator=" << std::endl;
    AbstractBase::operator=(rhs);
  }
};
class Derived1 : public AbstractBase {
 public:
  Derived1() { b = 0; }
  Derived1(int i) : b(i), AbstractBase(i * 10) {}

  Derived1& operator=(const Derived1& rhs) {
    std::cout << "Derived1::operator=" << std::endl;
    b = rhs.b;
    AbstractBase::operator=(rhs);
  }

 private:
  int b;
};
class Derived2 : public AbstractBase {
 public:
  Derived2() { c = 0; }
  Derived2(int i) : c(i), AbstractBase(i * 20) {}
  Derived2& operator=(const Derived2& rhs) {
    std::cout << "Derived2::operator=" << std::endl;
    c = rhs.c;
    AbstractBase::operator=(rhs);
  }

 private:
  int c;
};
void func() {
  Derived1 d1_1, d1_2;
  d1_1 = d1_2;  // 同类型间的赋值被允许
  // Derived1::operator=
  // AbstractBase::operator=

  Derived2 d2_1, d2_2;
  d2_1 = d2_2;  // 同类型间的赋值被允许
  // Derived2::operator=
  // AbstractBase::operator=

  AbstractBase* pab1 = &d1_1;
  AbstractBase* pab2 = &d1_2;
  AbstractBase* pab3 = &d2_1;
  // *pab1 = *pab2;  // 报错，通过基类指针部分赋值被禁止
  // *pab1 = *pab3;  // 报错，不同类型间的赋值被禁止

  Base b1, b2;
  b1 = b2;  // 同类型间的赋值被允许
  // Base::operator=
  // AbstractBase::operator=
}
}  // namespace n4

namespace n5 {
// 可能已经注意到前面讨论的通过基类指针进行赋值的问题，
// 是基于假设实体类（如Base）有数据成员。
// 如果它们没有数据成员，可能指出，那么就不会有问题，
// 从一个无数据的实体类派生新的实体类是安全的。

// 无数据而可以成为实体类的基类会两种可能，
// 在将来，或者它可能有数据成员，或者它仍然没有：
// 1.如果它将来可能有数据成员，
//   现在做的只是推迟问题的发生直到数据成员被加入，在用短利换长痛。
// 2.如果这个基类真的不会有数据成员，
//   那么它现在就该是抽象类，没有数据的实体类有什么用处？

// 用如AbstractBase这样的抽象基类替换如Base这样的实体基类，
// 其好处远比简单地使得operator=的行为易于了解。
// 它也减少了试图对数组使用多态的可能，这种行为的令人不愉快的后果。
// 然而，这个技巧最大的好处发生在设计的层次上，
// 因为这种替换强迫你明确地认可有用处的抽象行为的实体。
// 也就是说，它使得为有用的原型（concept）创造了新的抽象类，
// 即使并不知道这个有用的原型的存在。
// 如果有两个实体类C1和C2并且喜欢C2公有继承自C1，
// 应该将两个类的继承层次改为三个类的继承层次，
// 通过创造一个新的抽象类A并将C1和C2都从它继承。
// 这种修改的重要价值是强迫确定抽象类A。

// 这一切导致了一些令人不安的思考。
// 毕竟，每个类都完成了某些类型的抽象，
// 不应该在此继承体系中创造两个类来针对每个原型吗？
// 1.一个是抽象类来表示其抽象部分；
// 2.一个是实体类来表示对象生成部分。
// 不应该，如果这么做了，将使得继承体系中有太多的类。
// 这样的继承体系是难以理解的，难以维护的，
// 编译的代价很昂贵，这不是面向对象设计的目的。
// 其目的是：
// 确认有用的抽象，并强迫它们（并且只有它们）放入如抽象类这样的实体。
// 但：
// 1.怎么确认有用的抽象？
// 2.谁知道什么抽象在将来被证明有用？
// 3.谁能预知他将来要从什么进行继承？
// 在一个原型第一次被需要时，无法证明同时创造一个抽象类（为了这个原型），
// 和一个实体类（为了原型对应的对象）是正确的，
// 但第二次需要时，就能够这么做是正确的。
// 描述过的修改简单地实现了这个过程，
// 并且在这么做的过程中强迫设计着和程序员明确表达那些有用的抽象，
// 即使不知道那些有用的原型，这也碰巧使得构建正确的赋值行为很容易。

// 假设处理的只有一种传输协议，也只有一种包。
// 也许听说了其它协议和数据包类型的存在，
// 但还从未支持它们，也没有任何计划以在未来支持它们。
// 会为数据包既设计一个抽象类吗，又设计一个实际使用的实体类？
// 如果这么做了，可以在以后增加新的数据包而不用改变基类。
// 这使得增加新的数据包类型时程序不用重新编译。
// 但这种设计需要两个类，而现在只需要一个，针对于现在使用的特殊数据包类型。
// 这值得吗，增加设计的复杂度以允许扩充特性，而这种扩充可能从不发生？
// 这儿没有肯定正确的选择，但经验显示：
// 为还不完全了解的原型设计优秀的类几乎是不可能的。
// 如果为数据包设计了抽象类，怎么保证它正确，
// 尤其是在经验只局限于这唯一的数据包类型时？
// 记住，只有在设计出的类能被将来的类从它继承而不需要它作任何修改时，
// 才能从数据包的抽象类中获得好处。
// 如果它需要被修改，不得不重新编译所有使用数据包类的代码，那没得到任何好处。
// 看起来不太能够设计出一个领人满意的抽象设计包类，
// 除非精通各种数据包的区别以及它们相应的使用环境。
// 鉴于有限的经验，建议不要定义抽象类，等到以后需要从实体类继承时再加。

// 所说的转换方法是一个判断是否需要抽象类的方法，但不是唯一的方法。
// 还有很多其它的好方法，讲述面向对象分析的书籍上满是这类方法。
// 当发现需求从一个实体类派生出另外一个实体类时，
// 这也不是唯一需要引入抽象类的地方。
// 不管怎么说啦，需要通过公有继承将两个实体类联系起来，
// 通常表示需要一个新的抽象类，这种情况是如此常见，所以引起了深思。
// 第三方的C++类库越来越多，当发现需要从类库中的实体类派生出一个新的实体类，
// 而这个库你只有只读权时，要怎么做？
// 不能修改类库以加入一个新的抽象类，所以选择将很有限、很无趣：
// 1.从已存在的实体类派生出实体类，并容忍在开始时说到的赋值问题，
//   还要注意在Item M3中说过的数组问题；
// 2.试图在类库的继承树的更高处找到一个完成了所需的大部分功能的抽象类，
//   从它进行继承，当然，可能没有合适的类，即使有，
//   可能不得不重复很多已经在实体类中实现了的东西；
// 3.用包容试图继承的类的方法来实现新类。
//   例如，将一个类库中的类的对象为数据成员，并在类中重实现它的接口，
//   这种方法需要你在类库每次升级时也要更新自己的类。
//   它还需要放弃重定义类库中的类的虚函数的能力，因为用的不是继承；
class Window {
 public:
  virtual void resize(int newWidth, int newHeight) {}
  virtual void repaint() const {}
  int width() const {}
  int height() const {}
};
class SpecialWindow {
 public:
  int width() const { return w.width(); }
  int height() const { return w.height(); }
  virtual void resize(int newWidth, int newHeight);
  virtual void repaint() const;

 private:
  Window w;
};
// 4.使用类库中的类，而将自己的程序修改得那个类适用。
//   用非成员函数来提供扩展功能（那些想加入那个类而没有做到的）。
//   结果，程序将不如期望中的清晰、高效、可维护、可扩展，
//   但至少它完成了所需要的功能。
// 这些选择都不怎么吸引人，所以不得不作出判断并选择最轻的毒药。
// 这不怎么有趣，但生活有时就是这样。
// 想让事情在以后对自己（和我们其它人）容易些，将问题反馈给类库生产商。
// 靠运气（以及大量的用户反馈），随时间的流逝，那些设计可能被改进。

// 最后，一般的规则是：非尾端类应该是抽象类；
// 在处理外来的类库时，可能需要违背这个规则；
// 对于能控制的代码，遵守它可以提高程序的可靠性、健壮性、可读性、可扩展性。
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    case 4:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}