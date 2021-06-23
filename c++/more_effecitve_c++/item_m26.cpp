#include <iostream>
#include <memory>
#include <string>

using namespace std;

// 限制某个类所能产生的对象数量

// 每次实例化一个对象时，我们很确切地知道一件事情：“将调用一个构造函数。”事实确实这样，阻止建立某个类的对象，最容易的方法就是把该类的构造函数声明在类的private域：
class CantBeInstantiated {
 private:
  CantBeInstantiated();
  CantBeInstantiated(const CantBeInstantiated&);
};
// 这样做以后，每个人都没有权力建立对象，我们能够有选择性地放松这个限制。
class PrintJob;
class Printer {
 public:
  void submitJob(const PrintJob& job) {}
  void reset() {}
  void performSelfTest() {}
  friend Printer& thePrinter();

 private:
  Printer() {}
  Printer(const Printer& rhs) {}
};
Printer& thePrinter() {
  static Printer p;  // 单个打印机对象
  return p;
}
// 这个设计由三个部分组成:
//  第一、Printer类的构造函数是private。这样能阻止建立对象。
//  第二、全局函数thePrinter被声明为类的友元，让thePrinter避免私有构造函数引起的限制。
//  最后thePrinter包含一个静态Printer对象，这意味着只有一个对象被建立。
// 客户端代码无论何时要与系统的打印机进行交互访问，它都要使用thePrinter函数：
class PrintJob {
 public:
  PrintJob(const string& whatToPrint) {}
};
// thePrinter使用全局命名空间完全是多余的。想把所有与打印有关的功能都放到Printer类里。
// 这很简单，只要在Prihter类中声明thePrinter为静态函数，然后把它放在你想放的位置。就不再需要友元声明了。
class Printer1 {
 public:
  static Printer1& thePrinter();
  void submitJob(const PrintJob& job) {}
  void reset() {}

 private:
  Printer1() {}
  Printer1(const Printer1& rhs) {}
};
Printer1& Printer1::thePrinter() {
  static Printer1 p;
  return p;
}
// 另一种方法是把thePrinter移出全局域，放入namespace（命名空间）。
// 命名空间是C++一个较新的特性。任何能在全局域声明东西也能在命名空间里声明。包括类、结构、函数、变量、对象、typedef等等。
// 把它们放入命名空间并不影响它们的行为特性，不过能够防止在不同命名空间里的实体发生命名冲突。
// 命名空间从句法上来看有些象类，但是它没有public、protected或private域。所有都是public。
namespace PrintingStuff {
class Printer {  // 在命名空间
 public:         // PrintingStuff中的类
  void submitJob(const PrintJob& job) {}
  void reset() { cout << "PrintingStuff::Printer::reset" << endl; }
  void performSelfTest() {}

  friend Printer& thePrinter();

 private:
  Printer() {}
  Printer(const Printer& rhs) {}
};
Printer& thePrinter() {
  static Printer p;
  return p;
}
}

// 在thePrinter的实现上有两个微妙的不引人注目的地方，值得我们看一看：
// 1 唯一的Pritner对象是位于函数里的静态成员而不是在类中的静态成员，这样做是非常重要的。在类中的静态对象实际上总是被构造（和释放），即使不使用该对象。
//   与此相反，只有第一次执行函数时，才会建立函数中的静态对象，所以如果没有调用函数，就不会建立对象。不过你得为此付出代价，每次调用函数时都得检查是否需要建立对象。
//   建立C++一个理论支柱是你不需为你不用的东西而付出，在函数里，把类似于Printer这样的对象定义为静态成员就是坚持这样的理论。你应该尽可能坚持这种理论。
//   与一个函数的静态成员相比，把Printer声明为类中的静态成员还有一个缺点，它的初始化时间不确定。
//   我们能够准确地知道函数的静态成员什么时候被初始化：“在第一次执行定义静态成员的函数时”。而没有定义一个类的静态成员被初始化的时间。
//   C++为一个translation unit（也就是生成一个object文件的源代码的集合）内的静态成员的初始化顺序提供某种保证，
//   但是对于在不同translation unit中的静态成员的初始化顺序则没有这种保证。
//   在实际使用中，这会给我们带来许多麻烦。当函数的静态成员能够满足我们的需要时，我们就能避免这些麻烦。
// 2 另一个细微之处是内联与函数内静态对象的关系。
//   thePrinter的非成员函数形式，除了第一次执行这个函数时（也就是构造p时），其它时候这就是一个一行函数——它由“return p;”一条语句组成。
//   这个函数最适合做为内联函数使用。然而它不能被声明为内联。为什么呢？
//    为什么你要把对象声明为静态呢？通常是因为你只想要该对象的一个拷贝。
//    现在再考虑“内联”意味着什么呢？从概念上讲，它意味着编译器用函数体替代该对函数的每一个调用，不过非成员函数还不只这些。
//    非成员函数还有其它的含义。它还意味着internal linkage（内部链接）。
//   通常情况下，你不需要理解这种语言上令人迷惑的东西，你只需记住一件事：
//   带有内部链接的函数可能在程序内被复制（也就是说程序的目标（object）代码可能包含一个以上的内部链接函数的代码），这种复制也包括函数内的静态对象。
//   结果如何？如果建立一个包含局部静态对象的非成员函数，你可能会使程序的静态对象的拷贝超过一个！所以不要建立包含局部静态数据的非成员函数。

// 但是你可能认为建立函数来返回一个隐藏对象的引用，这种限制对象的数量的方法是错误的。
// 也许你认为只需简单地计算对象的数目，一旦需要太多的对象，就抛出异常，这样做也许会更好。
class Printer2 {
 public:
  class TooManyObjects {};  // 当需要的对象过多时就使用这个异常类
  Printer2();
  ~Printer2();

 private:
  static size_t numObjects;
  Printer2(const Printer2& rhs) {}  // 这里只能有一个printer，所以不允许拷贝
};
// 此法的核心思想就是使用numObjects跟踪Pritner对象存在的数量。当构造类时，它的值就增加，释放类时，它的值就减少。
// 如果试图构造过多的Printer对象，就会抛出一个TooManyObjects类型的异常：
size_t Printer2::numObjects = 0;
Printer2::Printer2() {
  if (numObjects >= 1) {
    throw TooManyObjects();
  }
  // 继续运行正常的构造函数;
  ++numObjects;
}
Printer2::~Printer2() {
  // 进行正常的析构函数处理;
  --numObjects;
}
// 这种限制建立对象数目的方法有两个较吸引人的优点：
// 一个是它是直观的，每个人都能理解它的用途。另一个是很容易推广它的用途，可以允许建立对象最多的数量不是一，而是其它大于一的数字。

// 建立对象的环境
// 这种方法也有一个问题。定义基类和派生类对象会产生多少Pritner对象？答案是两个。在运行时，当构造派生类的基类部分时，会抛出TooManyObjects异常。
class ColorPrinter : public Printer2 {};
// 当其它对象包含Printer对象时，会发生同样的问题：
class CPFMachine {
 private:
  Printer2 p;
};
// 问题是Printer对象能存在于三种不同的环境中：只有它们本身；作为其它派生类的基类；被嵌入在更大的对象里。
// 存在这些不同环境极大地混淆了跟踪“存在对象的数目”的含义，因为你心目中的“对象的存在” 的含义与编译器不一致。

// 通常你仅会对允许对象本身存在的情况感兴趣，你希望限制这种实例（instantiation）的数量。
// 如果你使用最初的Printer类示例的方法，就很容易进行这种限制，因为Printer构造函数是private，
// （不存在friend声明）带有private构造函数的类不能作为基类使用，也不能嵌入到其它对象中。
// 你不能从带有private构造函数的类派生出新类，这个事实导致产生了一种阻止派生类的通用方法，这种方法不需要和限制对象实例数量的方法一起使用。

// 假设你允许建立任意数量的对象，但是你想禁止派生出新类。
class FSA {
 public:
  // 伪构造函数
  static FSA* makeFSA();
  static FSA* makeFSA(const FSA& rhs);

 private:
  FSA() {}
  FSA(const FSA& rhs) {}
};
FSA* FSA::makeFSA() { return new FSA(); }
FSA* FSA::makeFSA(const FSA& rhs) { return new FSA(rhs); }
// 不象thePrinter函数总是返回一个对象的引用（引用的对象是固定的），每个makeFSA的伪构造函数则是返回一个指向对象的指针（指向的对象都是惟一的，不相同的）。
// 也就是说允许建立的FSA对象数量没有限制。
// 不过每个伪构造函数都调用new这个事实暗示调用者必须记住调用delete。否则就会发生资源泄漏。
// 如果调用者希望退出生存空间时delete会被自动调用，他可以把makeFSA返回的指针存储在auto_ptr中。

// 允许对象来去自由
// 把先前使用的对象计数的代码与刚才看到的伪构造函数代码合并在一起：
class Printer3 {
 public:
  class TooManyObjects {};
  // 伪构造函数
  static Printer3* makePrinter();
  ~Printer3();
  void submitJob(const PrintJob& job) {}
  void reset() {}
  void performSelfTest() {}

 private:
  static size_t numObjects;
  Printer3();
  Printer3(const Printer3& rhs);  // 我们不定义这个函数，因为不允进行拷贝
};
size_t Printer3::numObjects = 0;
Printer3::Printer3() {
  if (numObjects >= 1) {
    throw TooManyObjects();
    // 当需要的对象过多时，会抛出异常，如果你认为这种方式给你的感觉是unreasonablyharsh，你可以让伪构造函数返回一个空指针。当然用户在使用之前应该进行检测。
  }
  // 继续运行正常的构造函数;
  ++numObjects;
}
Printer3* Printer3::makePrinter() { return new Printer3; }
// 这种技术很容易推广到限制对象为任何数量上。我们只需把hard-wired常量值1改为根据某个类而确定的数量，然后消除拷贝对象的约束。
class Printer4 {
 public:
  class TooManyObjects {};
  // 伪构造函数
  static Printer4* makePrinter();
  static Printer4* makePrinter(const Printer4& rhs);
  ~Printer4() { --numObjects; }

 private:
  static size_t numObjects;
  static const size_t maxObjects = 10;
  Printer4();
  Printer4(const Printer4& rhs);
};
size_t Printer4::numObjects = 0;
const size_t Printer4::maxObjects;
Printer4::Printer4() {
  if (numObjects >= maxObjects) {
    throw TooManyObjects();
  }
  ++numObjects;
}
Printer4::Printer4(const Printer4& rhs) {
  if (numObjects >= maxObjects) {
    throw TooManyObjects();
  }
}
Printer4* Printer4::makePrinter() { return new Printer4; }
Printer4* Printer4::makePrinter(const Printer4& rhs) {
  return new Printer4(rhs);
}

// 一个具有对象计数功能的基类
// 使用计数类模板可以自动生成适当数量的计数器，因为我们能让计数器成为从模板中生成的类的静态成员：
template <class BeingCounted>
class Counted {
 public:
  class TooManyObjects {};  // 用来抛出异常
  static int objectCount() { return numObjects; }

 protected:
  // 从这个模板生成的类仅仅能被做为基类使用，因此构造函数和析构函数被声明为protected。
  Counted();
  Counted(const Counted& rhs);
  ~Counted() { --numObjects; }

 private:
  static int numObjects;
  static const size_t maxObjects;
  void init();  // private成员函数init用来避免两个Counted构造函数的语句重复。
};
template <class BeingCounted>
int Counted<BeingCounted>::numObjects;  // 定义numObjects
template <class BeingCounted>
Counted<BeingCounted>::Counted() {
  init();
}
template <class BeingCounted>
Counted<BeingCounted>::Counted(const Counted<BeingCounted>&) {
  init();
}
template <class BeingCounted>
void Counted<BeingCounted>::init() {
  if (numObjects >= maxObjects) throw TooManyObjects();
  ++numObjects;
}
// 使用Counted模板
class Printer5 : private Counted<Printer5> {
 public:
  // 伪构造函数
  static Printer5* makePrinter();
  static Printer5* makePrinter(const Printer5& rhs);
  ~Printer5();
  void submitJob(const PrintJob& job);
  void reset();
  void performSelfTest();
  using Counted<Printer5>::objectCount;  // 让这个函数对于Printer是public
  // Counted<Printer5>::objectCount;  // 老式的访问权声明语法：让objectCount在Printer中是public
  using Counted<Printer5>::TooManyObjects;

 private:
  Printer5() {}
  Printer5(const Printer5& rhs) {}
};
// 不对maxObject进行初始化。而是让此类的客户端提供合适的初始化。Printer的作者必须把这条语句加入到一个实现文件里：
template <>
const size_t Counted<Printer5>::maxObjects = 5;
Printer5* Printer5::makePrinter() { return new Printer5; }
Printer5* Printer5::makePrinter(const Printer5& rhs) {
  return new Printer5(rhs);
}
// Printer使用了Counter模板来跟踪存在多少Printer对象，坦率地说，除了Printer的编写者，没有人关心这个事实。
// 它的实现细节最好是private，这就是为什么这里使用private继承的原因。
// 另一种方法是在Printer和counted<Printer>之间使用public继承，但是我们必须给Counted类一个虚拟析构函数。
// 否则如果有人通过 Counted<Printer>*指针删除一个Printer 对象，我们就有导致对象行为不正确的风险。
// 在Counted中存在虚函数，几乎肯定影响从Counted继承下来的对象的大小和布局。我们不想引入这些额外的负担，所以使用private继承来避免这些负担。
// Counted所做的大部分工作对于Printer的用户来说都是隐藏的，但是这些用户可能很想知道有当前多少Printer对象存在。
// Counted模板提供了objectCount函数，用来提供这种信息，但是因为我们使用private继承，这个函数在Printer类中成为了private。
// 当Printer继承Counted<Printer>时，它可以忘记有关对象计数的事情。
// 编写Printer类时根本不用考虑对象计数，就好像有其他人会为它计数一样。Printer的构造函数可以是这样的：
// Printer5::Printer5() {
//   // 进行正常的构造函数运行
// }
// 这里有趣的不是你所见到的东西，而是你看不到的东西。不检测对象的数量就好像限制将被超过，执行完构造函数后也不增加存在对象的数目。
// 所有这些现在都由Counted<Printer>的构造函数来处理，因为Counted<Printer>是Printer的基类，我们知道Counted<Printer>的构造函数总在Printer的前面被调用。

int main() {
  // CantBeInstantiated c;

  string buffer;
  thePrinter().reset();
  thePrinter().submitJob(buffer);

  Printer1::thePrinter().reset();
  Printer1::thePrinter().submitJob(buffer);

  PrintingStuff::thePrinter().reset();  // PrintingStuff::Printer::reset
  PrintingStuff::thePrinter().submitJob(buffer);
  // 也可以使用using声明，以简化键盘输入：
  using PrintingStuff::thePrinter;
  thePrinter().reset();  // 现在可以象使用局部命名，覆盖了全局的thePrinter
  thePrinter().submitJob(buffer);  // 一样，使用thePrinter
  // PrintingStuff::Printer::reset

  // Printer2 p;
  // ColorPrinter cp;
  // terminate called after throwing an instance of 'Printer2::TooManyObjects'

  // CPFMachine m1;  // 运行正常
  // CPFMachine m2;  // 抛出TooManyObjects异常
  // terminate called after throwing an instance of 'Printer2::TooManyObjects'

  shared_ptr<FSA> pfsa1(FSA::makeFSA());
  shared_ptr<FSA> pfsa2(FSA::makeFSA(*pfsa1));

  // 除了用户必须调用伪构造函数，而不是真正的构造函数之外，它们使用Printer类就象使用其他类一样：
  // Printer3 p1;  // 错误! 缺省构造函数是private
  Printer3* p2 = Printer3::makePrinter();  // 正确, 间接调用缺省构造函数
  // Printer3 p3 = *p2;      // 错误!拷贝构造函数是private
  p2->performSelfTest();  // 所有其它的函数都可以正常调用
  p2->reset();
  // delete p2;  // 避免内存泄漏，如果p2是一个auto_ptr，就不需要这步。

  // Printer3* p3 = Printer3::makePrinter();
  // terminate called after throwing an instance of 'Printer3::TooManyObjects'

  Printer4* p41 = Printer4::makePrinter();
  Printer4* p42 = Printer4::makePrinter();
  Printer4* p43 = Printer4::makePrinter();
  Printer4* p44 = Printer4::makePrinter();
  Printer4* p45 = Printer4::makePrinter();
  Printer4* p46 = Printer4::makePrinter();
  Printer4* p47 = Printer4::makePrinter();
  Printer4* p48 = Printer4::makePrinter();
  Printer4* p49 = Printer4::makePrinter();
  Printer4* p4a = Printer4::makePrinter();
  // Printer4* p4b = Printer4::makePrinter();
  // terminate called after throwing an instance of 'Printer4::TooManyObjects'

  Printer5* p51 = Printer5::makePrinter();
  Printer5* p52 = Printer5::makePrinter();
  Printer5* p53 = Printer5::makePrinter();
  Printer5* p54 = Printer5::makePrinter();
  Printer5* p55 = Printer5::makePrinter();
  // Printer5* p56 = Printer5::makePrinter();
  // terminate called after throwing an instance of 'Counted<Printer5>::TooManyObjects'

  return 0;
}