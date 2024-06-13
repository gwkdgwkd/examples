#include <iostream>
#include <string>

// 条款23：理解std::move和std::forward

// 为了了解move和forward，一种有用的方式是从它们不做什么这个角度来了解它们。
// std::move不移动任何东西，std::forward也不转发任何东西。
// 在运行时，它们不做任何事情，它们不产生任何可执行代码，一字节也没有。

// std::move和std::forward仅仅是执行转换（cast）的函数，事实上是函数模板。
// move无条件的将它的实参转换为右值，而forward只在特定情况满足时下进行转换。
// 它们就是如此，这样的解释带来了一些新的问题，但是从根本上而言，这就是全部内容。

namespace n1 {
#if __cplusplus == 201103L
// C++11的std::move的示例实现，它并不完全满足标准细则，但是它已经非常接近了。
// 该函数返回类型的&&部分表明std::move函数返回的是一个右值引用，
// 但是，正如条款28所说，如果类型T恰好是一个左值引用，那么T&&将会成为一个左值引用。
// 为了避免如此，type trait std::remove_reference应用到了类型T上，
// 因此确保了&&被正确的应用到了一个不是引用的类型上。
// 这保证了std::move返回的真的是右值引用，这很重要，因为函数返回的右值引用是右值。
// 因此，std::move将它的实参转换为一个右值，这就是它的全部作用。
template <typename T>
typename std::remove_reference<T>::type&& move(T&& param) {
  using ReturnType = typename std::remove_reference<T>::type&&;

  // std::move接受对象的通用引用参数T，返回一个指向同对象的引用：
  return static_cast<ReturnType>(param);
}
#elif __cplusplus == 201402L
// 多亏了条款3的函数返回值类型推导和条款9的标准库模板别名std::remove_reference_t，
// std::move可以在C++14中被更简单地实现：
template <typename T>
decltype(auto) move(T&& param) {
  using ReturnType = std::remove_reference_t<T>&&;
  return static_cast<ReturnType>(param);
}
#endif

// 因为move除了转换它的实参到右值以外什么也不做，有人建议叫rvalue_cast可能会更好。
// 虽然可能确实是这样，但是它的名字已经是move，所以记住move做什么和不做什么很重要。
// 它只进行转换，不移动任何东西。
}  // namespace n1

namespace n2 {
class A {
 public:
  A() { std::cout << "A()" << std::endl; }

  // 类B的构造函数的成员初始化列表中，std::move(a)的结果是一个const A的右值。
  // 这个右值不能被传递给A的移动构造函数，因为移动构造函数只接受non-const的A的右值引用。
  // 然而，该右值却可以被传递给A的拷贝构造函数，因为常量左值引用允许被绑定到const右值上。
  // 因此，a_在成员初始化的过程中调用了拷贝构造函数，即使a已经被转换成了右值。
  // 这样是为了确保维持const属性的正确性，从一个对象中移动出某个值通常代表着修改该对象，
  // 所以语言不允许const对象被传递给可以修改他们的函数，例如移动构造函数。
  A(const A& rhs) { std::cout << "A(const A& rhs)" << std::endl; }
  A(A&& rhs) { std::cout << "A(A&& rhs)" << std::endl; }
};

// 右值本来就是移动操作的候选者，所以对对象使用move就是告诉编译器，这个对象很适合被移动。
// 所以这就是为什么std::move叫现在的名字：更容易指定可以被移动的对象。

// 事实上，右值只不过经常是移动操作的候选者，假设有一个类，它用来表示一段注解。
// 这个类的构造函数接受一个包含有注解的A作为形参，然后它复制该形参到数据成员。
// 假设了解条款41，声明一个值传递的形参：
class B {
 public:
  // B类的构造函数仅仅是需要读取text的值，它并不需要修改它，为了历史悠久的传统，
  // 能使用const就使用const保持一致，再加上const，并且为了避免拷贝是用了move：
  explicit B(const A a) : a_(std::move(a)) {}
  // 这段代码可以编译，可以链接，可以运行，这段代码将数据成员a_设置为a的值。
  // 这段代码与期望中的完美实现的唯一区别，是a并不是被移动到a_，而是被拷贝。
  // 诚然，a通过std::move被转换到右值，但是a被声明为const A，所以在转换之前，
  // a是一个左值的const A，而转换的结果是一个右值的const A，const属性一直保留。

 private:
  A a_;
};

void func() {
  const A a;
  B b(a);

  // A()
  // A(const A& rhs)
  // A(const A& rhs)

  // 总结出两点：
  // 1.不要在希望能移动对象的时候，声明为const，对const对象的移动会被转化为拷贝操作；
  // 2.move不仅不移动任何东西，而且它也不保证它执行转换的对象可以被移动，
  //   关于move，能确保的唯一一件事就是将它应用到一个对象上，能够得到一个右值。
}
}  // namespace n2

namespace n3 {
// 关于forward的故事与move是相似的，但是与move总是无条件的将它的实参为右值不同，
// forward只有在满足一定条件的情况下才执行转换，forward是有条件的转换。

// forward最常见的情景是一个模板函数，接收一个通用引用形参，并将它传递给另外的函数：
void f(const int& lvalArg) { std::cout << "f(const int&)" << std::endl; }
void f(int&& rvalArg) { std::cout << "f(int&&)" << std::endl; }
template <typename T>
void fun(T&& param) {
  // std::forward是怎么知道它的实参是否是被一个右值初始化的。
  // 简短的说，该信息藏在函数fun的模板参数T中。
  // 该参数被传递给了函数std::forward，它解开了含在其中的信息，详见条款28。
  f(std::forward<T>(param));
}

void func() {
  int i;
  fun(i);             // f(const int&)
  fun(std::move(i));  // f(int&&)

  // 在fun函数的内部，形参param被传递给函数f，函数f分别对左值和右值做了重载。
  // 当使用左值来调用fun时，自然期望该左值被当作左值转发给f函数，
  // 而当使用右值来调用fun函数时，期望f函数的右值重载版本被调用。

  // 但是param，正如所有的其他函数形参一样，是一个左值。
  // 每次在函数fun内部对函数f的调用，都会因此调用函数f的左值重载版本。
  // 为防如此，需要一种机制：
  // 当且仅当传递给函数fun的用以初始化param的实参是一个右值时，param会被转换为一个右值。
  // 这就是std::forward做的事情，这就是为什么std::forward是一个有条件的转换：
  // 它的实参用右值初始化时，转换为一个右值。
}
}  // namespace n3

namespace n4 {
// move和forward都是转换，它们唯一的区别就是move总是执行转换，而forward偶尔为之。
// 是否可以用forward替换move？从纯技术的角度可以，forward可以完全胜任，move并非必须。
// 当然，其实两者中没有哪个函数是真的必须的，因为可以直接写转换代码，但这将相当让人恶心。

// std::move的吸引力在于它的便利性：减少了出错的可能性，增加了代码的清晰程度。
// 考虑一个类，希望统计有多少次移动构造函数被调用了。
// 只需要一个static的计数器，它会在移动构造的时候自增，假设在这个类中，
// 唯一一个非静态的数据成员是std::string，一种经典的移动构造函数可以被实现如下：
namespace test1 {
class Widget {
 public:
  Widget() = default;
  Widget(Widget&& rhs) : s(std::move(rhs.s)) { ++moveCtorCalls; }
  static int show() { std::cout << moveCtorCalls << std::endl; }

 private:
  static std::size_t moveCtorCalls;
  std::string s;
};
std::size_t Widget::moveCtorCalls = 0;
void func() {
  Widget w;
  Widget w1 = std::move(w);
  Widget w2(std::move(w1));
  Widget::show();  // 2
}
}  // namespace test1

namespace test2 {
class Widget {
 public:
  // 用std::forward来达成同样的效果：
  // 1.std::move只需要一个函数实参rhs.s，而std::forward不但需要一个函数实参rhs.s，
  //   还需要一个模板类型实参std::string。
  // 2.传递给forward的类型应当是non-reference，因为惯例是传递的实参应该是一个右值，
  //   见条款28，同样，这意味着move比起forward来说需要打更少的字，
  //   并且免去了传递一个表示正在传递一个右值的类型实参，同时，
  //   它杜绝了传递错误类型的可能性，string&可能导致数据成员s被复制而不是被移动构造。
  Widget(Widget&& rhs) : s(std::forward<std::string>(rhs.s)) {
    ++moveCtorCalls;
  }
  // 更重要的是，std::move的使用代表着无条件向右值的转换，
  // 而使用std::forward只对绑定了右值的引用进行到右值转换，这是两种完全不同的动作。
  // 前者是典型地为了移动操作，而后者只是转发对象到其他函数，保留它原有的左值或右值属性。
  // 因为这些动作实在是差异太大，所以拥有两个不同的函数以及函数名来区分这些动作。

  Widget() = default;
  static int show() { std::cout << moveCtorCalls << std::endl; }

 private:
  static std::size_t moveCtorCalls;
  std::string s;
};
std::size_t Widget::moveCtorCalls = 0;
void func() {
  Widget w;
  Widget w1 = std::move(w);
  Widget w2(std::move(w1));
  Widget::show();  // 2
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n4

// 要记住的东西：
// 1.std::move执行到右值的无条件的转换，但就自身而言，它不移动任何东西；
// 2.std::forward只有当它的参数被绑定到一个右值时，才将参数转换为右值；
// 3.std::move和std::forward在运行期什么也不做。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
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