#include <iostream>
#include <memory>
#include <vector>

// 条款19：对于共享资源使用std::shared_ptr

// 使用带垃圾回收的语言的程序员指着C++程序员笑看他们如何防止资源泄露。
// 他们嘲笑着说：
// 真是原始啊！你们没有从1960年的Lisp那里得到启发吗，
// 机器应该自己管理资源的生命周期而不应该依赖人类。
// C++程序员翻白眼：
// 你们得到的所谓启示就是只有内存算资源，而且资源回收的时间点是不确定的？
// 我们更喜欢通用，可预料的销毁，谢谢你。

// 但我们的虚张声势可能底气不足，因为垃圾回收真的很方便，
// 而且手动管理生命周期真的就像是使用石头小刀和兽皮制作RAM电路。
// 为什么我们不能同时有两个完美的世界：
// 一个自动工作的世界，像是垃圾回收；
// 一个销毁可预测的世界，像是析构。

// C++11中的std::shared_ptr将两者组合了起来。
// 一个通过std::shared_ptr访问的对象其生命周期由指向它的有共享所有权的指针们来管理。
// 没有特定的std::shared_ptr拥有该对象。
// 相反，所有指向它的std::shared_ptr都能相互合作确保在它不再使用的那个点进行析构。
// 当最后一个指向某对象的shared_ptr不再指向那，比如shared_ptr被销毁或指向其他对象，
// std::shared_ptr会销毁它所指向的对象。
// 就垃圾回收来说，客户端不需要关心指向对象的生命周期，而对象的析构是确定性的。

// std::shared_ptr通过引用计数来确保它是否是最后一个指向某种资源的指针，
// 引用计数关联资源并跟踪有多少std::shared_ptr指向该资源。
// shared_ptr构造函数递增引用计数值，析构函数递减值，拷贝赋值运算符做前面这两个工作。
// 如果sp1和sp2是shared_ptr并且指向不同对象，sp1=sp2;会使sp1指向sp2指向的对象。
// 直接效果就是sp1引用计数减一，sp2引用计数加一。
// 如果引用计数在递减后值为零，没有其他shared_ptr指向该资源，它就会销毁资源。

// 引用计数暗示着性能问题：
// 1.std::shared_ptr大小是原始指针的两倍，因为它内部包含一个指向资源的原始指针，
//   还包含一个指向资源的引用计数值的原始指针，这种实现法并不是标准要求的，
//   但是几乎所有标准库都这样实现；
// 2.引用计数的内存必须动态分配，概念上，引用计数与所指对象关联起来，
//   但是实际上被指向的对象不知道这件事情，因此它们没有办法存放一个引用计数值，
//   使用std::make_shared创建std::shared_ptr可以避免引用计数的动态分配，
//   但是还存在一些std::make_shared不能使用的场景，这时候引用计数就会动态分配；
// 3.递增递减引用计数必须是原子性的，因为多个reader、writer可能在不同的线程，
//   比如，指向某种资源的std::shared_ptr可能在一个线程执行析构，
//   于是递减指向的对象的引用计数，在另一个不同的线程，std::shared_ptr指向相同的对象，
//   但是执行的却是拷贝操作，递增了同一个引用计数，原子操作通常比非原子操作要慢，
//   所以即使引用计数通常只有一个word大小，也应该假定读写它们是存在开销的。

// std::shared_ptr构造函数只是通常递增指向对象的引用计数。
// 创建一个指向对象的std::shared_ptr就产生了又一个指向那个对象的std::shared_ptr，
// 为什么不是总是增加引用计数值？
// 从另一个shared_ptr移动构造新shared_ptr会将原来的shared_ptr设置为null，
// 那意味着老的std::shared_ptr不再指向资源，同时新的std::shared_ptr指向资源。
// 这样的结果就是不需要修改引用计数值。
// 因此移动std::shared_ptr会比拷贝它要快：拷贝要求递增引用计数值，移动不需要。
// 移动赋值运算符同理，所以移动构造比拷贝构造快，移动赋值运算符也比拷贝赋值运算符快。

struct Widget {};
auto loggingDel = [](Widget* pw) {
  std::cout << "delete" << std::endl;
  delete pw;
};

namespace n1 {
// 类似std::unique_ptr，std::shared_ptr使用delete作为资源的默认销毁机制，
// 但是它也支持自定义的删除器，但这种支持有别于std::unique_ptr。
// 对于std::unique_ptr来说，删除器类型是智能指针类型的一部分。
// 对于std::shared_ptr则不是：

void func1() {
  // 删除器类型是指针类型的一部分：
  std::unique_ptr<Widget, decltype(loggingDel)> upw(new Widget, loggingDel);

  // 删除器类型不是指针类型的一部分：
  std::shared_ptr<Widget> spw(new Widget, loggingDel);
}

void f(std::shared_ptr<Widget>) {}
void func2() {
  // std::shared_ptr的设计更为灵活，有两个std::shared_ptr<Widget>，
  // 每个自带不同的删除器（比如通过lambda表达式自定义删除器）：
  auto customDeleter1 = [](Widget* pw) {};
  auto customDeleter2 = [](Widget* pw) {};
  std::shared_ptr<Widget> pw1(new Widget, customDeleter1);
  std::shared_ptr<Widget> pw2(new Widget, customDeleter2);

  // 因为pw1和pw2有相同的类型，所以：
  // 1.它们都可以放到存放那个类型的对象的容器中：
  std::vector<std::shared_ptr<Widget>> vpw{pw1, pw2};

  // 2.它们也能相互赋值：
  pw1 = pw2;

  // 3.可以传入一个形参为std::shared_ptr<Widget>的函数：
  f(pw1);
  f(pw2);

  // 自定义删除器类型不同的unique_ptr就不行，因为unique_ptr把删除器视作类型的一部分。
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 另一个不同于std::unique_ptr的地方是，指定自定义删除器不会改变shared_ptr对象的大小。
// 不管删除器是什么，一个std::shared_ptr对象都是两个指针大小，这是个好消息，
// 但是它应该让你隐隐约约不安，自定义删除器可以是函数对象，函数对象可以包含任意多的数据，
// 它意味着函数对象是任意大的，shared_ptr怎么能引用一个任意大的删除器而不使用更多的内存？

// 它不能，它必须使用更多的内存，然而，那部分内存不是std::shared_ptr对象的一部分。
// 那部分在堆上面，或者std::shared_ptr创建者利用std::shared_ptr对自定义分配器的支持能力，
// 那部分内存随便在哪都行，前面提到了std::shared_ptr对象包含了所指对象的引用计数的指针。
// 没错，但是有点误导人，因为引用计数是另一个更大的数据结构的一部分，那个数据结构通常叫做控制块。
// 每个std::shared_ptr管理的对象都有个相应的控制块。
// 控制块除了包含引用计数值外还有一个自定义删除器的拷贝，当然前提是存在自定义删除器。
// 如果用户还指定了自定义分配器，控制块也会包含一个分配器的拷贝。
// 控制块可能还包含一些额外的数据，正如条款21提到的，一个次级引用计数，但是目前先忽略它。

// 当指向对象的std::shared_ptr一创建，对象的控制块就建立了，至少期望是如此，通常，
// 对于一个创建指向对象的shared_ptr的函数来说不可能知道是否有其他shared_ptr早已指向那个对象，
// 所以控制块的创建会遵循下面几条规则：
// 1.std::make_shared总是创建一个控制块，它创建一个要指向的新对象，
//   所以可以肯定std::make_shared调用时对象不存在其他控制块；
// 2.当从独占指针（即unique_ptr或者auto_ptr）上构造出std::shared_ptr时会创建控制块，
//   独占指针没有使用控制块，所以指针指向的对象没有关联控制块，
//   作为构造的一部分，shared_ptr侵占独占指针所指向的对象的独占权，所以独占指针被设置为null；
// 3.当从原始指针上构造出std::shared_ptr时会创建控制块，
//   如果想从一个早已存在控制块的对象上创建std::shared_ptr，
//   将假定传递一个std::shared_ptr或者std::weak_ptr作为构造函数实参，而不是原始指针，
//   用shared_ptr或者weak_ptr作为构造函数实参创建shared_ptr不会创建新控制块，
//   因为它可以依赖传递来的智能指针指向控制块。

void func() {
  // 这些规则造成的后果就是从原始指针上构造超过一个shared_ptr就会走上未定义行为的快车道，
  // 因为指向的对象有多个控制块关联，多个控制块意味着多个引用计数值，
  // 多个引用计数值意味着对象将会被销毁多次（每个引用计数一次）。
  // 那意味着像下面的代码是有问题的，很有问题，问题很大：
  auto pw = new Widget;                          // pw是原始指针
  std::shared_ptr<Widget> spw1(pw, loggingDel);  // 为*pw创建控制块
  std::shared_ptr<Widget> spw2(pw, loggingDel);  // 为*pw创建第二个控制块

  // delete
  // delete
  // free(): double free detected in tcache 2
  // 已放弃 (核心已转储)

  // 创建原始指针pw指向动态分配的对象很糟糕，因为它完全背离了这章的建议：
  // 倾向于使用智能指针而不是原始指针。
  // 撇开那个不说，创建pw那一行代码虽然让人厌恶，但是至少不会造成未定义程序行为。
  // 现在，传给spw1的构造函数一个原始指针，它会为指向的对象创建一个控制块（因此有个引用计数值）。
  // 这种情况下，指向的对象是*pw（即pw指向的对象），就其本身而言没什么问题，
  // 但是将同样的原始指针传递给spw2的构造函数会再次为*pw创建一个控制块（所以也有个引用计数值）。
  // 因此*pw有两个引用计数值，每个最后都会变成零，然后导致*pw销毁两次，第二次会产生未定义行为。
}
}  // namespace n2

namespace n3 {
// std::shared_ptr给我们上了两堂课：
// 1.避免传给std::shared_ptr构造函数原始指针，通常替代方案是使用std::make_shared，
//   不过上面例子中，使用了自定义删除器，用std::make_shared就没办法做到；
// 2.如果你必须传给std::shared_ptr构造函数原始指针，直接传new出来的结果，不要传指针变量。

void func() {
  std::shared_ptr<Widget> spw1(new Widget, loggingDel);  // 使用new的结果
  std::shared_ptr<Widget> spw2(spw1);  // spw2使用spw1一样的控制块
}
}  // namespace n3

namespace n4 {
// 一个尤其令人意外的是使用this指针作为shared_ptr构造函数实参的时候可能导致创建多个控制块。
// 假设程序使用shared_ptr管理Widget对象，有一个数据结构用于跟踪已经处理过的Widget对象：

namespace test1 {
class A;
std::vector<std::shared_ptr<A>> processedWidgets;
class A {
 public:
  void process() {
    processedWidgets.emplace_back(this);

    // 上面的代码可以通过编译，我怎么不能编译？
    // 但是向std::shared_ptr的容器传递一个原始指针this，
    // std::shared_ptr会由此为指向的Widget（*this）创建一个控制块，看起来没什么问题，
    // 但如果成员函数外面早已存在指向那个Widget对象的指针，就是未定义行为。
  }
};
}  // namespace test1

namespace test2 {
// shared_ptr的API已有处理test1中情况的设施，它的名字可能是C++标准库中最奇怪的一个：
// std::enable_shared_from_this。

// std::enable_shared_from_this是一个基类模板，它的模板参数总是某个继承自它的类，
// 代码完全合法，而且它背后的设计模式也是没问题的，并且这种设计模式还有个标准名字，
// 奇异递归模板模式The Curiously Recurring Template Pattern，简称CRTP。

class A;
std::vector<std::shared_ptr<A>> processedWidgets;
// 如果想创建一个用shared_ptr管理的类，这个类能够用this指针安全地创建一个shared_ptr，
// std::enable_shared_from_this就可作为基类的模板类：
class A : public std::enable_shared_from_this<A> {
 public:
  void process() {
    // std::enable_shared_from_this定义了一个成员函数shared_from_this，
    // 这个成员函数会创建指向当前对象的std::shared_ptr却不创建多余控制块。
    // 无论在哪当想在成员函数中使用std::shared_ptr指向this所指对象时都请使用它：
    processedWidgets.emplace_back(shared_from_this());
    // shared_from_this查找当前对象控制块，然后创建新的shared_ptr关联这个控制块。
    // 设计的依据是当前对象已经存在一个关联的控制块。
    // 要想符合设计依据的情况，必须已经存在一个指向当前对象的std::shared_ptr，
    // 比如调用shared_from_this的成员函数外面已经存在一个std::shared_ptr。
    // 如果没有std::shared_ptr指向当前对象，即当前对象没有关联控制块，
    // 行为是未定义的，shared_from_this通常会抛出一个异常。
  }
};
}  // namespace test2

namespace test3 {
// 要想防止客户端在创建指向对象的shared_ptr前先调用含有shared_from_this的成员函数，
// 继承自std::enable_shared_from_this的类通常将它们的构造函数声明为private，
// 并且让客户端通过返回std::shared_ptr的工厂函数创建对象。

class A;
std::vector<std::shared_ptr<A>> processedWidgets;
class A : public std::enable_shared_from_this<A> {
 public:
  template <typename... Ts>
  static std::shared_ptr<Widget> create(Ts&&... params) {
    // 完美转发参数给private构造函数的工厂函数：
    return std::make_shared<Widget>(new A(params...));
  }
  void process() { processedWidgets.emplace_back(shared_from_this()); }

 private:
  A() = default;
};
}  // namespace test3

void func() {}
}  // namespace n4

// 控制块通常只占几个word大小，自定义删除器和分配器可能会让它变大一点。
// 控制块的实现比想的更复杂，它使用继承，甚至还有一个用来确保指向的对象被正确销毁的虚函数。
// 这意味着使用std::shared_ptr还会招致控制块使用虚函数带来的成本。

// 了解了动态分配控制块，任意大小的删除器和分配器，虚函数机制，
// 原子性的引用计数修改，对于std::shared_ptr的热情可能有点消退。
// 可以理解，对每个资源管理问题来说都没有最佳的解决方案。
// 但就它提供的功能来说，std::shared_ptr的开销是非常合理的。
// 在通常情况下，使用默认删除器和默认分配器，使用std::make_shared创建std::shared_ptr，
// 产生的控制块只需三个word大小，它的分配基本上是无开销的。
// 对std::shared_ptr解引用的开销不会比原始指针高。
// 执行需要原子引用计数修改的操作需要承担一两个原子操作开销，这些操作通常都会映射到机器指令上，
// 所以即使对比非原子指令来说，原子指令开销较大，但是它们仍然只是单个指令上的。
// 对于每个被shared_ptr指向的对象来说，控制块中的虚函数开销通常只有一次，即对象销毁的时候。

// 作为这些轻微开销的交换，得到了动态分配的资源的生命周期自动管理的好处。
// 大多数时候，比起手动管理，使用std::shared_ptr管理共享性资源都是非常合适的。
// 如果还在犹豫是否能承受std::shared_ptr带来的开销，那就再想想你是否需要共享所有权。
// 如果独占资源可行或者可能可行，用std::unique_ptr是一个更好的选择。
// 它的性能表现更接近于原始指针，并且从std::unique_ptr升级到std::shared_ptr也很容易，
// 因为std::shared_ptr可以从std::unique_ptr上创建。

// 反之不行，当资源由std::shared_ptr管理，现在又想修改资源生命周期管理方式是没有办法的。
// 即使引用计数为1，也不能重新修改资源所有权，改用std::unique_ptr管理它。
// 资源和指向它的std::shared_ptr的签订的所有权协议是除非死亡否则永不分开。
// 不能分离，不能废除，没有特许。

// std::shared_ptr不能处理的另一个东西是数组，和std::unique_ptr不同的是，
// std::shared_ptr的API设计之初就是针对单个对象的，没有办法std::shared_ptr<T[]>。
// 一次又一次，聪明的程序员踌躇于是否该使用std::shared_ptr<T>指向数组，
// 然后传入自定义删除器来删除数组，即delete[]，这可以通过编译，但是个糟糕的主意。
// 一方面，std::shared_ptr没有提供operator[]，所以数组索引操作需要借助怪异的指针算术。
// 另一方面，std::shared_ptr支持转换为指向基类的指针，这对于单个对象来说有效，
// 但是当用于数组类型时相当于在类型系统上开洞。
// 出于这个原因，std::unique_ptr<T[]> API禁止这种转换。
// 更重要的是，C++11已经提供了很多内置数组的候选方案，比如array，vector，string。
// 声明一个指向傻瓜数组的智能指针几乎总是表示着糟糕的设计。

// 要记住的东西：
// 1.std::shared_ptr为有共享所有权的任意资源提供一种自动垃圾回收的便捷方式；
// 2.较之于std::unique_ptr，std::shared_ptr对象通常大两倍，
//   控制块会产生开销，需要原子性的引用计数修改操作；
// 3.默认资源销毁是通过delete，但是也支持自定义删除器，
//   删除器的类型是什么对于std::shared_ptr的类型没有影响。
// 4.避免从原始指针变量上创建std::shared_ptr。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}