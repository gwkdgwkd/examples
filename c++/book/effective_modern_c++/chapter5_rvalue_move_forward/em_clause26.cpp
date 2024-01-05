#include <iostream>
#include <vector>

// 条款26：避免与万能引用类型进行重载

struct Widget {
  Widget() { std::cout << "Widget()" << std::endl; }
  Widget(int) { std::cout << "Widget(int)" << std::endl; }
  Widget(const char*) { std::cout << "Widget(const char *)" << std::endl; }
  Widget(const Widget&) { std::cout << "Widget(const Widget &)" << std::endl; }
  Widget(Widget&&) { std::cout << "Widget(Widget &&)" << std::endl; }
};
std::vector<Widget> ws;

namespace n1 {
// 这段代码无可厚非，只是效率方面未能尽如意：
void f(const Widget& w) { ws.emplace_back(w); }

void func() {
  ws.reserve(10);  // 如果没有这句，会导致重新分配空间

  // 三种可能的调用方式：

  // 1.形参w绑定到了变量ww，在f内部，w最终被传递给了ws.emplace_back，
  //   由于w是个左值，它是被复制入ws的，没有任何办法避免这个复制操作，
  //   因为传递给f的ww是个左值：
  Widget ww;
  f(ww);  // 传递左值Widget
  // Widget()
  // Widget(const Widget &)

  // 2.形参w绑定到了一个右值，从PersePhone显示构造的Widget类型的临时对象，
  //   w自身是个左值，所以它是被复制入ws的：
  f(Widget("PersePhone"));  // 传递右值Widget
  // Widget(const char *)
  // Widget(const Widget &)

  // 但原则上是可以被移入ws的，所以在这个调用中，付出了一次复制的成本，
  // 但可以用一次移动来达成同样的目标。

  // 3.形参w绑定到了右值，但这次这个Widget的临时对象是从PattyDog隐式构造的，
  //   和第二个调用语句的情况一样，w是被复制入ws的：
  f("PattyDog");  // 传递字符串字面量
  // Widget(const char *)
  // Widget(const Widget &)

  // 但传递给f的实参是个字符串字面量，如果该字符串字面量被直接传递给emplace_back，
  // 那么完全没有必要构造一个Widget类型的临时对象，
  // emplace_back完全可以利用这个字符串字面量在vector内部直接构造出一个Widget对象，
  // 付出了复制Widget对象的成本，但实际上连移动的成本都没有必要付出，更别说复制了。
}
}  // namespace n1

namespace n2 {
// 可以解决n1中第二个和第三个调用语句的效率低下问题，只需要重写f,让它接受一个万能引用，
// 并且根据条款25，对该引用实施std::forward给到emplace_back，重写结果不言自明：
template <typename T>
void f(T&& w) {
  ws.emplace_back(std::forward<T>(w));
}

void func() {
  ws.reserve(10);

  Widget ww;
  f(ww);
  // Widget()
  // Widget(const Widget &)

  f(Widget("PersePhone"));
  // Widget(const char *)
  // Widget(Widget &&)

  f("PattyDog");
  // Widget(const char *)

  // 非常完美，效率达到极致！
}
}  // namespace n2

Widget WidgetFromIdx(int idx) { return Widget(); }
namespace n3 {
template <typename T>
void f(T&& w) {
  std::cout << "void f(T&& w)" << std::endl;
  ws.emplace_back(std::forward<T>(w));
}

// 新需求，需要通过id才能得到Widget：
void f(int idx) {  // 重载版本
  std::cout << "void f(int idx)" << std::endl;
  ws.emplace_back(WidgetFromIdx(idx));
}

void func() {
  ws.reserve(10);

  Widget ww;
  f(ww);
  // Widget()
  // void f(T&& w)
  // Widget(const Widget &)

  f(Widget("PersePhone"));
  // Widget(const char *)
  // void f(T&& w)
  // Widget(Widget &&)

  f("PattyDog");
  // void f(T&& w)
  // Widget(const char *)

  f(22);  // 调用了形参类型为int的重载版本f，符合期望
  // void f(int idx)
  // Widget()
  // Widget(Widget &&)

  short s = 22;
  f(s);
  // void f(T&& w)
  // Widget(int)

  std::string str = "nihao";
  // f(str);  // 报错

  // f有两个重载版本，形参类型为T&&的版本可以将T推导为short，从而产生一个精确匹配。
  // 而形参类型为int的版本却只能在类型提升以后才能匹配到short类型的实参。
  // 按照普适的重载决议规则，精确匹配优先于提升后才能匹配。
  // 所以形参类型为万能引用的版本才是被调用到的版本。
  // 这一切的原因归根结底在于，对于short类型的实参来说，万能引用产生了比int更好的匹配。

  // 形参为万能引用的函数，是C++中最贪婪的。
  // 它们会在具现过程中，和几乎任何实参类型都会产生精确匹配。
  // 这就是为何把重载和万能引用这两者结合起来几乎总是馊主意：
  // 一旦万能引用成为重载候选，它就会吸引走大批的实参类型，远比程序员期望的要多。
}
}  // namespace n3

namespace n4 {
// 解决n3的简单办法，是撰写带完美转发的构造函数，考虑Person类，它的构造函数有相同的功能：
class Person {
 public:
  // 完美转发构造函数：
  template <typename T>
  explicit Person(T&& n) : w_(std::forward<T>(n)) {
    std::cout << "Person(T&& n)" << std::endl;
  }

  // 形参为int的构造函数：
  explicit Person(int idx) : w_(WidgetFromIdx(idx)) {
    std::cout << "Person(int idx)" << std::endl;
  }

  Person(const Person&) { std::cout << "Person(const Person &)" << std::endl; }

 private:
  Widget w_;
};

void func() {
  Person p("Nancy");
  // auto cloneOfP(p);  // 编译错误

  // 在这里尝试了从一个Person创建另一个Person，看起来再明显不过会是调用复制构造的情况，
  // p是个左值，这就足以打消一切会将复制通过移动来完成的想法。
  // 在这段代码竟没有调用复制构造函数，而是调用了完美转发构造函数。
  // 该函数是在尝试从一个Person对象p出发来初始化另一个Person对象的string类型的数据成员。
  // 而std::string类型却并不具备接受Person类型形参的构造函数，编译器只能悲愤地举手投降，
  // 也许会丢出一堆冗长且无法理解错误信息作为惩罚和发泄。

  // 可能感觉莫名其妙，这是怎么回事？怎么会调用的是完美转发构造函数而不是复制构造函数呢？
  // 这不是明明在用一个Person类型的对象初始化另一个Person类型的对象吗？
  // 确实是在做这件事，但是编译器是宣誓效忠于C++规则的，而在这里的规则是调用重载函数的决议。

  // 编译器的推理过程如下：cloneOfP被非常量左值p初始化，
  // 那意味着模板构造函数可以实例化来接受Person类型的非常量左值形参。
  // 如此实例化后，Person的代码应该变换成下面这样：
  // class Person {
  //  public:
  //   explicit Person(Person& n) : w_(std::forward<Person&>(n)) {}
  //   explicit Person(int idx) {}
  //   Person(const Person& rhs) {}
  //  private:
  //   Widget w_;
  // };
  // 在语句auto
  // clineOfP(p);中，p既可以传递给复制构造函数，也可以传递给实例化了的模板。
  // 但是，调用复制构造的话就要先对p添加const饰词才能匹配复制构造函数的形参类型，
  // 而调用了实例化了的模板却不要求添加任何饰词，因此模板生成的重载版本是更佳匹配，
  // 所以编译器的做法完全符合设计：它调用了符合更佳匹配原则的函数。

  const Person cp("Nancy");  // 对象成为常量了
  auto cloneOfP(cp);         // 这回调用的是复制构造函数了
  // Widget(const char *)
  // Person(T&& n)
  // Widget()
  // Person(const Person &)

  // 因为欲复制的对象是个常量，就形成了对复制构造函数形参的精确匹配。
  // 另一方面，那个模板化的构造函数可以经由实例化得到同样的签名：
  // class Person {
  //  public:
  //   explicit Person(const Person& n);  // 从模板出发实例化而得到的构造函数
  //   Person(const Person& rhs);  // 复制构造函数
  // };
  // 但不要紧，因为C++重载决议规则中有这么一条：
  // 若在函数调用时，一个模板实例化函数和一个非函数模板（普通函数）具备相等的匹配程度，
  // 则优先选用常规函数，根据这一条，在签名相同时，复制构造函数是普通函数，
  // 就会压过实例化了的函数模板。
}

// 完美转发构造函数与编译器生成的复制和移动操作之间的那些错综复杂的关系，
// 再加上继承以后就变得更让人眉头紧锁，特别的，
// 派生类的复制和移动操作的平凡实现会表现出让人大跌眼镜的行为。
class SpecialPerson : public Person {
 public:
  // 复制构造函数，调用的是基类的完美转发构造函数：
  // SpecialPerson(const SpecialPerson& rhs) : Person(rhs) {}

  // 移动构造函数，调用的是基类的完美转发构造函数：
  // SpecialPerson(SpecialPerson&& rhs) : Person(std::move(rhs)) {}
};

// 派生类的复制和移动构造函数并未调用到基类的复制和移动构造函数，
// 调用的是基类的完美转发构造函数，派生类函数把类型为SpecialPerson的实参传递给了基类，
// 然后Person类的构造函数中完成模板实例化和重载决议，最终代码无法通过编译，
// 因为Widget的构造函数中没有任何一个会接受SpecialPerson类型的形参。

// 现在已经说服了你去尽可能避免以把万能引用类型作为重载函数的形参选项，不过，
// 如果使用万能引用进行重载是个糟糕的思路，而又需要针对绝大多数的实参类型实施转发，
// 只针对某些实参类型实施特殊的处理，这时该怎么办呢？
// 解决之道多种多样，方法实在太多，专门定制条款27进行讲述。
}  // namespace n4

// 要记住的东西：
// 1.把万能引用作为重载候选类型，几乎总会让该重载版本在始料未及的情况下被调用到；
// 2.完美转发构造函数的问题尤其严重，因为对于非常量的左值类型而言，
//   他们一般都会形成相对于复制构造函数的更佳匹配，
//   并且它们还会劫持派生类中对基类的复制和移动构造函数的调用。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}