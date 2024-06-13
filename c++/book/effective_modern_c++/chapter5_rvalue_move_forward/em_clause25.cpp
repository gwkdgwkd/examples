#include <iostream>
#include <memory>
#include <string>

// 条款25：对右值引用使用std::move，对通用引用使用std::forward

struct SomeDataStructure {};
std::string getWidgetName() { return "nihao"; };

namespace n1 {
class Widget {
 public:
  Widget() = default;

  // 右值引用仅绑定可以移动的对象，如果有一个右值引用形参就知道这个对象可能会被移动：
  Widget(Widget&& rhs) : name(std::move(rhs.name)), p(std::move(rhs.p)) {}
  // rhs是右值引用，希望传递这样的对象给其他函数，允许那些函数利用对象的右值性。
  // 这样做的方法是将绑定到此类对象的形参转换为右值。

  template <typename T>
  void setName(T&& newName) {  // newName是通用引用
    // 条款24，通用引用可能绑定到有资格移动的对象上，
    // 通用引用使用右值初始化时，才将其强制转换为右值：
    // name = std::forward<T>(newName);  // 正确的做法

    // 条款23，可以在右值引用上使用std::forward表现出适当的行为，但是代码较长，
    // 容易出错，所以应该避免在右值引用上使用std::forward。
    // 更糟的是在通用引用上使用std::move，这可能会意外改变左值（比如局部变量）：
    name = std::move(newName);  // 可以编译，但是代码太差了
  }

  // 总而言之，当把右值引用转发给其他函数时：
  // 1.右值引用应该被无条件转换为右值，通过std::move，因为它们总是绑定到右值；
  // 2.当转发通用引用时，通用引用应该有条件地转换为右值，通过std::forward，
  //   因为它们只是有时绑定到右值。

 private:
  std::string name;
  std::shared_ptr<SomeDataStructure> p;
};

void func() {
  Widget w;

  auto n = getWidgetName();  // n是局部变量
  w.setName(n);              // 把n移动进w
  // 现在n的值未知

  // 局部变量n被传递给w.setName，调用方可能认为这是对n的只读操作，这是可以被原谅的。
  // 但是因为setName内部使用move无条件将传递的引用形参转换为右值，n的值被移动进w.name，
  // 调用setName返回时n最终变为未定义的值，这种行为使得调用者蒙圈，还有可能变得狂躁。
}
}  // namespace n1

namespace n2 {
// 可能争辩说setName不应该将其形参声明为通用引用，此类引用如条款24描述，
// 不能使用const，但是setName肯定不应该修改其形参。
// 可能会指出，如果为const左值和为右值分别重载setName可以避免整个问题，比如这样：
class Widget {
 public:
  Widget() = default;

  // 用const左值设置：
  void setName(const std::string& newName) { name = newName; }
  // 用右值设置：
  void setName(std::string&& newName) { name = std::move(newName); }

 private:
  std::string name;
};

void func() {
  // Widget当然可以工作，但是有缺点：
  // 1.编写和维护的代码更多，两个函数而不是单个模板；
  // 2.效率下降。

  // 比如，考虑如下场景：
  Widget w;
  w.setName("Adela Novak");
  // 使用通用引用的版本的setName，字面字符串Adela Novak可以被传递给setName，
  // 再传给w内部std::string的赋值运算符。
  // w的name的数据成员通过字面字符串直接赋值，没有临时std::string对象被创建。
  // 但是，setName重载版本，会有一个临时std::string对象被创建，
  // setName形参绑定到这个对象，然后这个临时std::string移动到w的数据成员中。
  // 一次setName的调用会包括std::string构造函数调用（创建中间对象），
  // std::string赋值运算符调用（移动newName到w.name），
  // std::string析构函数调用（析构中间对象）。
  // 这比调用接受const char*指针的std::string赋值运算符开销昂贵许多。
  // 增加的开销根据实现不同而不同，这些开销是否值得担心也跟应用和库的不同而有所不同，
  // 将通用引用模板替换成对左值引用和右值引用的一对函数重载在某些情况下会导致运行时的开销。
  // 如果把例子泛化，Widget数据成员是任意类型，而不是知道是个std::string，
  // 性能差距可能会变得更大，因为不是所有类型的移动操作都像string开销较小。
}

// 关于对左值和右值的重载函数最重要的问题不是源代码的数量，也不是代码的运行时性能。
// 而是设计的可扩展性差，Widget::setName有一个形参，因此需要两种重载实现，
// 但是对于有更多形参的函数，每个都可能是左值或右值，重载函数的数量几何式增长，
// n个参数的话，就要实现2n种重载，这还不是最坏的。
// 有的函数，实际上是函数模板，接受无限制个数的参数，每个参数都可以是左值或者右值。
// 此类函数的典型代表是std::make_shared，还有对于C++14的std::make_unique。
// template <class T, class... Args>
// shared_ptr<T> make_shared(Args&&... args);
// template <class T, class... Args>
// unique_ptr<T> make_unique(Args&&... args);
// 对于这种函数，对于左值和右值分别重载就不能考虑了，通用引用是仅有的实现方案。
// 对这种函数，肯定使用std::forward传递通用引用形参给其他函数，这也是程序员应该做的。
}  // namespace n2

namespace n3 {
// 在某些情况，可能需要在一个函数中多次使用绑定到右值引用或者通用引用的对象，
// 并且确保在完成其他操作前，这个对象不会被移动，这时，只想在最后一次使用时，
// 使用std::move（对右值引用）或者std::forward（对通用引用），比如：
template <typename T>
void setSignText(T&& text) {  // text是通用引用
  // sign.setText(text);                           // 使用text但是不改变它
  // auto now = std::chrono::system_clock::now();  // 获取现在的时间
  // signHistory.add(now, std::forward<T>(text));

  // 想要确保text的值不会被sign.setText改变，
  // 因为想要在signHistory.add中继续使用，因此std::forward只在最后使用。

  // 对于std::move，同样的思路，即最后一次用右值引用的时候再调用std::move，
  // 但是需要注意，在有些稀少的情况下，需要调用条款14说描述的move_if_noexcept代替move。
}
}  // namespace n3

namespace n4 {
// 如果在按值返回的函数中，返回值绑定到右值引用或者通用引用上，
// 需要对返回的引用使用std::move或者std::forward。

struct Widget {
  Widget() = default;
  Widget(const Widget&) { std::cout << "Widget(const Widget&) " << std::endl; }
  Widget(Widget&&) { std::cout << "Widget(Widget&&) " << std::endl; }
};

namespace test1 {
template <typename T>
Widget f1(T&& w) {
  // 在return语句中通过move将w转换为右值，w可以移动到返回值的内存位置：
  return std::move(w);
}

template <typename T>
Widget f2(T&& w) {
  // w是个左值，会强制编译器拷贝它到返回值的内存空间：
  return w;
}

void func() {
  Widget w1;
  Widget w2 = f1(w1);             // Widget(Widget&&)
  Widget w3 = f1(std::move(w2));  // Widget(Widget&&)

  Widget w4;
  Widget w5 = f2(w4);             // Widget(const Widget&)
  Widget w6 = f2(std::move(w5));  // Widget(const Widget&)
}

// 假定Widget支持移动，并且比拷贝操作效率更高，在return语句中使用move的代码效率更高。
// 如果Widget不支持移动，将其转换为右值不会变差，因为右值可以直接被拷贝构造函数拷贝。
// 如果Widget随后支持了移动操作，f1将在下一次编译时受益，就是这种情况，
// 通过将move应用到按值返回的函数中要返回的右值引用上，不会损失什么，还可能获得收益。
}  // namespace test1

namespace test2 {
// 使用通用引用和std::forward的情况类似：
template <typename T>
Widget f1(T&& w) {
  // 如果原始对象是右值，可以将其移动到返回值中，但是如果原始对象是左值，必须创建副本：
  return std::forward<T>(w);
}

template <typename T>
Widget f2(T&& w) {
  // std::forward被忽略，w就被无条件复制到f2的返回值内存空间：
  return w;
}

void func() {
  Widget w1;
  Widget w2 = f1(w1);             // Widget(const Widget&)
  Widget w3 = f1(std::move(w2));  // Widget(Widget&&)

  Widget w4;
  Widget w5 = f2(w4);             // Widget(const Widget&)
  Widget w6 = f2(std::move(w5));  // Widget(const Widget&)
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n4

namespace n5 {
struct Widget {
  Widget() { std::cout << "Widget() " << std::endl; };
  Widget(const Widget&) { std::cout << "Widget(const Widget&) " << std::endl; }
  Widget(Widget&&) { std::cout << "Widget(Widget&&) " << std::endl; }
};

namespace test1 {
// 如果对要被拷贝到返回值的右值引用形参使用std::move，
// 会把拷贝构造变为移动构造，也可以对要返回的局部对象应用同样的优化：
Widget makeWidget() {
  Widget w;
  return w;  // 拷贝w到返回值中

  // 这里满足返回值优化的两个条件，每个合适的C++编译器都会应用RVO来避免拷贝w。
  // 那意味着makeWidget的拷贝版本实际上不拷贝任何东西。
}

void func() {
  Widget w = makeWidget();  // Widget()
}
}  // namespace test1

namespace test2 {
// 想要优化代码，把拷贝变为移动：
Widget makeWidget() {
  Widget w;

  // 这是错的，因为对于这种优化，标准化委员会远领先于开发者。
  // 早就为人认识到的是，makeWidget的拷贝版本可以避免复制局部变量w的需要，
  // 通过在分配给函数返回值的内存中构造w来实现，这在C++标准中已经实现了，
  // 就是所谓的返回值优化，return value optimization，RVO。

  // 说出这种优化反倒是一件麻烦事，因为只想在不影响软件行为的地方，省去那些拷贝。
  // 剖析一下标准中这个合法的优化，它表达的是，如果满足：
  // 1.局部对象的类型与返回值类型相同；
  // 2.返回的就是那个局部对象，编译器可能省去按值返回局部对象时的拷贝或移动。
  // 这种局部对象包括大多数的局部变量，如makeWidget中的w，
  // 以及return语句中的临时对象，函数参数不属于这种。
  // 有些人对RVO还区分命名和未命名（即临时的）局部对象：
  // 1.未命名对象叫RVO；
  // 2.命名对象就叫NRVO，named return value optimization。

  // 移动版本的makeWidget行为与其名称一样，将w的内容移动到makeWidget的返回值位置。
  // 但是为什么编译器不使用RVO消除这种移动，而是在分配给函数返回值的内存中再次构造w呢？
  // 答案很简单，因为它们不能，条件2中规定，仅当返回值为局部对象时，才进行RVO，
  // 但是makeWidget的移动版本不满足这条件，再次看一下返回语句：
  return std::move(w);  // 移动w到返回值中，不要这样做
  // 返回的已经不是局部对象w，而是w的引用，std::move(w)的结果。
  // 返回局部对象的引用不满足RVO的第二个条件，所以编译器必须移动w到函数返回值的位置。
  // 开发者试图对要返回的局部变量用std::move帮助编译器优化，反而限制了编译器的优化选项。
}
void func() {
  Widget w = makeWidget();
  // Widget()
  // Widget(Widget&&)
}
}  // namespace test2

namespace test3 {
// RVO的确是一种优化，但编译器没有要求一定会省去拷贝和移动操作，即使允许那样做。
// 也许会怀疑，并且担心编译器会用拷贝操作惩罚你，仅仅因为它们可以做到。
// 或者，也许研究足够深入，会认识到存在某些情况，RVO是很难实现的，
// 例如，当一个函数中有不同的控制路径返回不同的局部变量，
// 编译器必须要在返回值内存中构造局部变量，但编译器怎么知道要构造哪个合适呢？
// 如果是这样，可能更愿意执行移动而不是拷贝，还是会想用std::move，那样就不拷贝了。

// 那种情况下，使用std::move依然是一个糟糕的想法，标准中RVO部分还提到，
// 如果RVO条件满足，但编译器选择不省去拷贝，返回的对象必须是右值。
// 实现上，标准要求，当RVO满足条件时，要么就选择省去拷贝，要么就隐性使用std::move。
// 所以在拷贝版本的makeWidget中，编译器要么消除w的拷贝，要么把函数看成像下面一样：
Widget makeWidget() {
  Widget w;
  return std::move(w);  // 把w看成右值，因为不执行拷贝消除
}
}  // namespace test3

namespace test4 {
// 函数传值参数也类似，由于参数的内存已经分配好，它们不适用于返回值省去拷贝的那种策略，
// 如果返回它们，编译器必须按右值处理，因此，如果源代码是这样的：
Widget makeWidget(Widget w) {  // 传值形参，与函数返回的类型相同
  return w;
}
// 编译器必须看成像下面这样写的代码：
// Widget makeWidget(Widget w) { return std::move(w); }

// 这意味着，如果对从按值返回的函数返回来的局部对象使用std::move，并不能帮助编译器，
// 如果不能实行拷贝消除的话，他们必须把局部对象看做右值，而是阻碍其执行优化选项RVO。
// 在某些情况下，将move应用于局部变量可能是一件合理的事，即把一个变量传给函数，
// 并且知道不会再用这个变量，但是满足RVO的return语句或者返回一个传值形参并不在此列。
}  // namespace test4

void func() {
  test1::func();
  test2::func();
}
}  // namespace n5

// 要记住的东西：
// 1.最后一次使用时，在右值引用上使用move，在通用引用上使用forward；
// 2.对按值返回的函数，返回右值引用或通用引用时，也同样要分别引用move和forward；
// 3.如果局部对象可以被返回值优化消除，就绝不使用move或者forward。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n4::func();
      break;
    case 1:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}