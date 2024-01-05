#include <iostream>
#include <memory>
#include <vector>

// 条款21：优先考虑使用std::make_unique和std::make_shared，而非直接使用new

#if __cplusplus == 201103L  // C++11
// std::make_shared是C++11标准的一部分，但std::make_unique不是。
// 它从C++14开始加入标准库，如果使用C++11，可以简单实现：
template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

// make_unique只是将它的参数完美转发到所要创建的对象的构造函数，
// 从new产生的原始指针里面构造出std::unique_ptr，并返回这个std::unique_ptr。
// 这种形式的函数不支持数组和自定义析构，不要把它放到std命名空间中，因为可能发生冲突。
#elif __cplusplus == 201402L
using std::make_unique;
#endif

// std::make_unique和std::make_shared是三个make函数中的两个：
// 接收任意的多参数集合，完美转发到构造函数去动态分配对象，然后返回这个指向对象的指针。
// 第三个make函数是std::allocate_shared，它行为和std::make_shared一样，
// 只不过第一个参数是用来动态分配内存的allocator对象。

struct Widget {};

namespace n1 {
// make函数更好的第一个原因：使用new的版本重复了类型，但是make函数的版本没有。
// 重复写类型和软件工程里面一个关键原则相冲突：应该避免重复代码。
// 源代码中的重复增加了编译的时间，会导致目标代码冗余，并且通常会让代码库使用更加困难。
// 它经常演变成不一致的代码，而代码库中的不一致常常导致bug。
// 此外，打两次字比一次更费力，而且没人不喜欢少打字吧？

void func() {
  auto upw1(make_unique<Widget>());          // 使用make函数
  std::unique_ptr<Widget> upw2(new Widget);  // 不使用make函数
  auto spw1(std::make_shared<Widget>());     // 使用make函数
  std::shared_ptr<Widget> spw2(new Widget);  // 不使用make函数
}

}  // namespace n1

namespace n2 {
// 第二个使用make函数的原因和异常安全有关。

// 按照某种优先级处理Widget：
void processWidget(std::shared_ptr<Widget> spw, int priority) {
  // 值传递std::shared_ptr可能看起来很可疑，但是条款41解释了，
  // 如果processWidget总是复制std::shared_ptr那么这可能是一个合理的设计选择。
}

// 计算相关的优先级：
int computePriority() { return 5; }

void func1() {
  // 在调用processWidget时使用了new而不是std::make_shared，有潜在的资源泄漏：
  processWidget(std::shared_ptr<Widget>(new Widget), computePriority());

  // 上面的代码可能在new一个Widget时发生泄漏，为何？
  // 调用的代码和被调用的函数都用shared_ptrs，且shared_ptrs就是设计出来防止泄漏的。
  // 它们会在最后一个std::shared_ptr销毁时自动释放所指向的内存。
  // 如果每个人在每个地方都用std::shared_ptrs，这段代码怎么会泄漏呢？

  // 答案和编译器将源码转换为目标代码有关，在运行时，一个函数的实参必须先被计算，
  // 这个函数再被调用，所以在调用processWidget之前，必须执行以下操作：
  // 1.表达式new Widget必须计算，例如，一个Widget对象必须在堆上被创建；
  // 2.负责管理new出来指针的std::shared_ptr<Widget>构造函数必须被执行；
  // 3.computePriority必须运行。

  // 编译器不需要按照执行顺序生成代码，new
  // Widget必须在shared_ptr的构造函数被调用前执行，
  // 因为new出来的结果作为构造函数的实参，但computePriority可能在之前，之后，或之间执行。
  // 也就是说，编译器可能按照这个执行顺序生成代码：
  // 1.执行new Widget；
  // 2.执行computePriority；
  // 3.运行std::shared_ptr构造函数。

  // 如果按照这样生成代码，并且在运行时computePriority产生了异常，
  // 那么第一步动态分配的Widget就会泄漏，因为它永远都不会被第三步的shared_ptr所管理了。
}

void func2() {
  // 使用std::make_shared可以防止这种问题，没有潜在的资源泄漏：
  processWidget(std::make_shared<Widget>(), computePriority());

  // 在运行时，std::make_shared和computePriority其中一个会先被调用。
  // 如果是std::make_shared先被调用，在computePriority调用前，
  // 动态分配Widget的原始指针会安全的保存在作为返回值的std::shared_ptr中。
  // 如果computePriority产生异常，那么shared_ptr析构函数将确保管理的Widget被销毁。
  // 如果首先调用computePriority并产生一个异常，那么std::make_shared将不会被调用，
  // 因此也就不需要担心动态分配Widget（会泄漏）。

  // 如果将std::shared_ptr，std::make_shared替换成std::unique_ptr，
  // std::make_unique，同样的道理也适用，因此，在编写异常安全代码时，
  // 使用std::make_unique而不是new与使用std::make_shared（而不是new）同样重要。
}
}  // namespace n2

namespace n3 {
// std::make_shared与直接使用new相比，效率提升。
// 使用std::make_shared允许编译器生成更小，更快的代码，并使用更简洁的数据结构。

void func() {
  // 这段代码需要进行内存分配，但它实际上执行了两次。
  // 每个std::shared_ptr指向一个控制块，其中包含被指向对象的引用计数，还有其他东西。
  // 这个控制块的内存在std::shared_ptr构造函数中分配。
  // 因此，直接使用new需要为Widget进行一次内存分配，为控制块再进行一次内存分配。
  std::shared_ptr<Widget> spw1(new Widget);

  // std::make_shared只分配一块内存，同时容纳了Widget对象和控制块。
  // 这种优化减少了程序的静态大小，因为代码只包含一个内存分配调用，
  // 并且它提高了可执行代码的速度，因为内存只分配一次。
  // 此外，使用make_shared避免了对控制块中的某些簿记信息的需要，潜在地减少了程序的总内存。
  auto spw2 = std::make_shared<Widget>();

  // 对于std::make_shared的效率分析同样适用于std::allocate_shared，
  // 因此std::make_shared的性能优势也扩展到了该函数。
}
}  // namespace n3

namespace n4 {
// 更倾向于使用make函数而不是直接使用new的争论非常激烈，
// 尽管它们在软件工程、异常安全和效率方面具有优势，但本条款的建议是，
// 更倾向于使用make函数，而不是完全依赖于它们，因为有些情况下它们不能或不应该被使用。

namespace test1 {
// make函数都不允许指定自定义删除器，但是unique_ptr和shared_ptr有构造函数这么做。
void func() {
  auto widgetDeleter = [](Widget* pw) { delete pw; };
  std::unique_ptr<Widget, decltype(widgetDeleter)> upw(new Widget,
                                                       widgetDeleter);
  std::shared_ptr<Widget> spw(new Widget, widgetDeleter);

  // 对于make函数，没有办法做上面的事情。
}
}  // namespace test1

namespace test2 {
// make函数第二个限制来自于其实现中的语法细节，条款7解释了，当构造函数重载，
// 使用std::initializer_list作为参数的重载形式和不用其作为参数的的重载形式，
// 用花括号创建的对象更倾向于使用std::initializer_list作为形参的重载形式，
// 而用小括号创建对象将调用不用std::initializer_list作为参数的的重载形式。
// make函数会将它们的参数完美转发给对象构造函数，但是它们是使用小括号还是花括号？
// 对某些类型，问题的答案会很不相同。

void func() {
  auto upv = make_unique<std::vector<int>>(10, 20);
  auto spv1 = std::make_shared<std::vector<int>>(10, 20);

  // 好消息是这并非不确定：两种调用都创建了10个元素，每个值为20的std::vector。
  // 这意味着在make函数中，完美转发使用小括号，而不是花括号。
  // 坏消息是如果想用花括号初始化指向的对象，必须直接使用new。
  // 使用make函数会需要能够完美转发花括号初始化的能力，花括号初始化无法完美转发。
  // 但是，条款30介绍了一个变通的方法：
  // 使用auto类型推导从花括号初始化创建std::initializer_list对象，
  // 然后将auto创建的对象传递给make函数。

  auto initList = {10, 20};  // 创建std::initializer_list
  // 使用std::initializer_list为形参的构造函数创建std::vector：
  auto spv2 = std::make_shared<std::vector<int>>(initList);
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n4

namespace n5 {
// 对于std::unique_ptr，只有自定义删除器和花括号初始化这两种情景使用make函数有点问题。
// 对于std::shared_ptr和它的make函数，还有2个问题，都属于边缘情况，但是一些开发者常碰到。

// 一些类重载了operator new和operator delete。
// 这些函数的存在意味着对这些类型的对象的全局内存分配和释放是不合常规的。
// 设计这种定制操作往往只会精确的分配、释放对象大小的内存。
// 例如，Widget类的operator new和operator delete只处理sizeof(Widget)大小的内存。
// 这种行为不太适用于shared_ptr对自定义分配（allocate_shared）和释放（自定义删除器）的支持，
// 因为std::allocate_shared需要的内存总大小不等于动态分配的对象大小，还需要再加上控制块大小。
// 因此使用make函数创建重载了new和delete的类对象是个典型的糟糕想法。

// 与直接使用new相比，std::make_shared在大小和速度上的优势源于：
// std::shared_ptr的控制块与指向的对象放在同一块内存中。
// 当对象的引用计数降为0，对象被销毁（即析构函数被调用），但是，
// 因为控制块和对象被放在同一块分配的内存块中，直到控制块的内存也被销毁，对象占用的内存才被释放。

// 控制块除了引用计数，还包含簿记信息，引用计数追踪有多少std::shared_ptrs指向控制块，
// 但控制块还有第二个计数，记录多少个std::weak_ptrs指向控制块，第二个引用计数就是weak数量。
// 实际上，weak count的值不总是等于指向控制块的std::weak_ptr的数目，
// 因为库的实现者找到一些方法在weak count中添加附加信息，促进更好的代码产生。
// 为了本条款的目的，会忽略这一点，假定weak数量的值等于指向控制块的std::weak_ptr的数目。
// 当一个std::weak_ptr检测它是否过期时，它会检测指向的控制块中的引用计数而不是weak数量。
// 如果引用计数是0，std::weak_ptr就已经过期，否则就没过期。

// 只要std::weak_ptrs引用一个控制块（即weak数量大于零），该控制块必须继续存在。
// 只要控制块存在，包含它的内存就必须保持分配，通过std::shared_ptr的make函数分配的内存，
// 直到最后一个std::shared_ptr和最后一个指向它的std::weak_ptr已被销毁，才会释放。

// 如果对象类型非常大，而且销毁最后一个shared_ptr和销毁最后一个weak_ptr之间的时间很长，
// 那么在销毁对象和释放它所占用的内存之间可能会出现延迟。

class ReallyBigType {};

void func1() {
  // 通过std::make_shared创建一个大对象：
  auto pBigObj = std::make_shared<ReallyBigType>();

  // 创建std::shared_ptrs和std::weak_ptrs指向这个对象，使用它们

  // 最后一个std::shared_ptr在这销毁，但std::weak_ptrs还在

  // 在这个阶段，原来分配给大对象的内存还分配着

  // 最后一个std::weak_ptr在这里销毁，控制块和对象的内存被释放
}

void func2() {
  // 通过new创建大对象：
  std::shared_ptr<ReallyBigType> pBigObj(new ReallyBigType);

  // 创建std::shared_ptrs和std::weak_ptrs指向这个对象，使用它们

  // 最后一个std::shared_ptr在这销毁，但std::weak_ptrs还在，对象的内存被释放

  // 在这阶段，只有控制块的内存仍然保持分配

  // 最后一个std::weak_ptr在这里销毁，控制块内存被释放
}
}  // namespace n5

namespace n6 {
// 如果发现自己处于不可能或不合适使用std::make_shared的情况下，
// 将想要保证自己不受我们之前看到的异常安全问题的影响。
// 最好的方法是确保在直接使用new时，在一个不做其他事情的语句中，
// 立即将结果传递到智能指针构造函数，这可以防止：
// 编译器生成的代码在使用new和调用管理new出来对象的智能指针的构造函数之间发生异常。

void processWidget(std::shared_ptr<Widget> spw, int priority) {}
void cusDel(Widget* ptr) { delete ptr; }
int computePriority() { return 5; }

void func1() {
  // 这是非异常安全调用，实参是右值：
  processWidget(std::shared_ptr<Widget>(new Widget, cusDel), computePriority());
  // 如果computePriority在new Widget之后，而在std::shared_ptr构造函数之前调用，
  // 并且如果computePriority产生一个异常，那么动态分配的Widget将会泄漏。
}

void func2() {
  // 这里使用自定义删除排除了对std::make_shared的使用，
  // 因此避免出现问题的方法是将Widget的分配和std::shared_ptr的构造放入它们自己的语句中，
  // 然后使用得到的std::shared_ptr调用processWidget，实参是左值。
  std::shared_ptr<Widget> spw(new Widget, cusDel);
  processWidget(spw, computePriority());  // 正确，但是没优化

  // 因为shared_ptr获取了传递给它的构造函数的原始指针的所有权，即使构造函数产生了一个异常。
  // 此例中，如果spw的构造函数抛出异常（比如无法为控制块动态分配内存），
  // 仍然能够保证cusDel会在new Widget产生的指针上调用。
}

void func3() {
  // 因为processWidget的shared_ptr形参是传值，从右值构造只需移动，而传左值构造需要拷贝。
  // 对shared_ptr而言，这种区别是有意义的，因为拷贝shared_ptr需要对引用计数原子递增，
  // 移动则不需要对引用计数有操作。
  // 为了使异常安全代码达到非异常安全代码的性能水平，需要用std::move将spw转换为右值：
  std::shared_ptr<Widget> spw(new Widget, cusDel);
  processWidget(std::move(spw), computePriority());  // 高效且异常安全

  // 这很有趣，也值得了解，但通常是无关紧要的，因为很少有理由不使用make函数。
  // 除非有令人信服的理由这样做，否则应该使用make函数。
}
}  // namespace n6

// 要记住的东西：
// 1.和直接使用new相比，make函数消除了代码重复，提高了异常安全性，
//   对于std::make_shared和std::allocate_shared，生成的代码更小更快；
// 2.不适合使用make函数的情况包括需要指定自定义删除器和希望用花括号初始化；
// 3.对于std::shared_ptrs，其他不建议使用make函数的情况包括：
//   a.有自定义内存管理的类，重载了operator new和operator delete；
//   b.特别关注内存的系统，非常大的对象，以及weak_ptrs比对应的shared_ptrs活得更久。

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
      n3::func();
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