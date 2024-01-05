#include <atomic>
#include <iostream>
#include <string>
#include <vector>

// 第三章：使用现代C++

// 条款7：创建对象时区分()和{}

namespace n1 {
struct Widget {};
void func() {
  int x(0);   // 使用小括号初始化
  int y = 0;  // 使用=初始化
  int z{0};   // 使用花括号初始化

  // 很多情况下，可以使用=和花括号的组合：
  int i = {0};  // C++通常把它视作只有花括号一样

  // 初始化中使用=可能会误导C++新手，以为发生了赋值运算。
  // 对于内置类型区别不大，但对于自定义类型，区别赋值和初始化非常重要：
  Widget w1;       // 默认构造函数
  Widget w2 = w1;  // 拷贝构造函数
  w1 = w2;         // 拷贝赋值运算符

  // 想直接表示一个存放一些特殊值的STL容器，C++98的初始化方法没有办法
}
}  // namespace n1

namespace n2 {
// C++11用花括号来统一初始化，整合这些混乱且不适于所有情景的初始化语法。

std::vector<int> v{1, 3, 5};  // 1.表达以前表达不出的东西

class Widget {
 private:
  // 2.花括号初始化也能被用于非静态数据成员指定默认初始值：
  int x{0};   // 没问题，初始值为0
  int y = 0;  // C++11允许=初始化不加花括号也可以
  // int z(0);   // 错误
};

void func() {
  // 3.不可拷贝的对象可以使用花括号初始化或者小括号初始化，但不能用=初始化：
  std::atomic<int> ai1{0};
  std::atomic<int> ai2(0);
  // std::atomic<int> ai3 = 0;
}

// 对于1、2、3，除了{}外，没有哪种初始化方式能胜任，所以{}是统一的初始化语法。
}  // namespace n2

namespace n3 {
// 花括号有一个异常的特性，它不允许内置类型间隐式的变窄转换。

void func() {
  double x, y, z;
  int sum1{x + y + z};   // 错误，gcc给出警告
  int sum2(x + y + z);   // 可以，没有警告
  int sum3 = x + y + z;  // 可以，没有警告
}
}  // namespace n3

namespace n4 {
// 花括号表达式对于C++最令人头疼的解析问题有天生的免疫性。

struct Widget {
 public:
  Widget() {}
  Widget(int) {}
};
void func() {
  // C++规定任何能被决议为一个函数声明的东西必须被决议为函数声明。
  // 这个规则的副作用是：当他们想创建一个使用默认构造函数构造的对象，却不小心变成了函数声明。

  // 问题的根源是如果你想使用一个实参调用一个构造函数，你可以这样做：
  Widget w1(10);  // 使用实参10调用Widget的一个构造函数
  // 但是如果你尝试使用相似的语法调用没有参数的Widget构造函数，它就会变成函数声明：
  Widget w2();

  // 函数声明中形参列表不能使用花括号，所以使用花括号初始化表明想调用默认构造函数就没有问题：
  Widget w3{};  // 调用没有参数的构造函数构造对象
}
}  // namespace n4

namespace n5 {
// 花括号初始化、std::initializer_list和构造函数重载决议本来就不清不楚的暧昧关系进一步混乱。
// 例如条款2中说的，auto声明的变量用花括号初始化，被推导为std::initializer_list，
// 所以越喜欢用auto，就越不能用花括号初始化。

namespace test1 {
// 构造函数中只要不包含std::initializer_list形参，那么花括号和小括号效果相同：
class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
};
void func() {
  Widget w1(10, true);  // Widget(int, bool)
  Widget w2{10, true};  // Widget(int, bool)
  Widget w3(10, 5.0);   // Widget(int, double)
  Widget w4{10, 5.0};   // Widget(int, double)
}
}  // namespace test1

namespace test2 {
// 如果有一个或者多个构造函数的声明一个std::initializer_list形参，
// 使用花括号初始化语法的调用更倾向于适用std::initializer_list重载函数。

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
  Widget(std::initializer_list<long double> il) {
    std::cout << "std::initializer_list<long double>" << std::endl;
  }
  Widget(const Widget& rhs) {
    std::cout << "Widget(const Widget& rhs)" << std::endl;
  }
  Widget(Widget&& rhs) { std::cout << "Widget(Widget&& rhs)" << std::endl; }
  operator float() const { std::cout << "Widget->float "; }
};

void func() {
  Widget w1(10, true);  // Widget(int, bool)
  Widget w2{10, true};  // std::initializer_list<long double>
  Widget w3(10, 5.0);   // Widget(int, double)
  Widget w4{10, 5.0};   // std::initializer_list<long double>
  // 即使非std::initializer_list构造函数更匹配，但w2和w4还是会使用新添加的构造函数。

  // 拷贝构造函数和移动构造函数都被std::initializer_list构造函数劫持：
  Widget w5(w4);             // Widget(const Widget& rhs)
  Widget w6{w4};             // Widget->float std::initializer_list<long double>
  Widget w7(std::move(w4));  // Widget(Widget&& rhs)
  Widget w8{std::move(w4)};  // Widget->float std::initializer_list<long double>
}
}  // namespace test2

namespace test3 {
// 编译器热衷于把花括号初始化与使用std::initializer_list构造函数匹配了，
// 尽管最佳匹配std::initializer_list构造函数不能被调用也会凑上去。

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
  Widget(std::initializer_list<bool> il) {
    std::cout << "std::initializer_list<bool>" << std::endl;
  }
  // 没有隐式转换函数
};
void func() {
  // Widget w{10, true};  // 编译错误

  // 编译器直接忽略了前两个构造函数，哪怕第二个提供了所有实参的最佳匹配。
  // 然后参数调用第三个构造函数，会把10和5.0转换为bool，产生了变窄转码，
  // 花括号初始化拒绝变窄转换，所以这个调用无效，代码无法通过编译。
}
}  // namespace test3

namespace test4 {
// 只有当没有办法把括号中实参类型转换为std::initializer_list时，
// 编译器才会回到正常的函数决议流程中。

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
  Widget(std::initializer_list<std::string> il) {
    std::cout << "std::initializer_list<std::string>" << std::endl;
  }
};

void func() {
  Widget w1(10, true);  // Widget(int, bool)
  Widget w2{10, true};  // Widget(int, bool)
  Widget w3(10, 5.0);   // Widget(int, double)
  Widget w4{10, 5.0};   // Widget(int, double)
}
}  // namespace test4

namespace test5 {
class Widget {
 public:
  Widget() { std::cout << "Widget()" << std::endl; }
  Widget(std::initializer_list<int> il) {
    std::cout << "std::initializer_list<int>" << std::endl;
  }
};

void func() {
  Widget w1;    // Widget()
  Widget w2();  // 最令人头疼的解析，声明了一个函数

  // {}意味着没有实参，而不是一个空的std::initializer_list：
  Widget w3{};  // Widget()

  // 如果想用空的std::initializer_list调用std::initializer_list构造函数：
  Widget w4({});  // std::initializer_list<int>
  Widget w5{{}};  // std::initializer_list<int>
}
}  // namespace test5

namespace test6 {
void func() {
  // 括号初始化，std::initializer_list和构造函数重载在日常编程中到底占多大比例？
  // 可能比想象的要多，因为std::vector作为其中之一会直接受到影响。
  // std::vector有个非std::initializer_list构造函数允许指定大小和所有元素的初值。
  // 但它也有一个std::initializer_list构造函数允许你使用花括号里面的值初始化容器。
  // 如果创建一个数值类型的std::vector，有两个实参，放到小括号和花括号中大不相同：
  std::vector<int> v1(10, 20);  // 非std::initializer_list构造函数，10个20
  std::vector<int> v2{10, 20};  // std::initializer_list构造函数，2个元素10和20
}

// 有两个重要结论：
// 1.作为类库作者，需要意识到如果有一个或者多个含有std::initializer_list形参的构造函数，
//   用户代码如果使用了花括号初始化，可能只会看std::initializer_list版本的重载的构造函数。
//   因此，最好把你的构造函数设计为不管用户是小括号还是使用花括号进行初始化都不会有什么影响。
//   换句话说，现在看到std::vector设计的缺点以后设计的时候避免它。
//   一个类没有std::initializer_list构造函数，然后添加一个，用户代码中如果使用花括号初始化，
//   可能会发现过去被决议为非std::initializer_list构造函数而现在被决议为新的函数。
//   这种事情也可能发生在添加重载函数的时候：过去被决议为旧的重载函数而现在调用了新的函数。
//   std::initializer_list重载不会和其他重载函数比较，它直接盖过了其它重载函数，
//   其它重载函数几乎不会被考虑，所以如果要加入std::initializer_list构造函数，请三思而后行。
// 2.作为类库使用者，必须认真的在花括号和小括号之间选择一个来创建对象。
//   大多数开发者都使用其中一种作为默认情况，只有当他们不能使用这种的时候才会考虑另一种。
//   如果默认使用花括号初始化，会得到大范围适用面的好处，禁止变窄转换，免疫C++最令人头疼的解析。
//   他们知道在一些情况下（比如给一个容器大小和一个值创建std::vector）要使用小括号。
//   如果默认使用小括号，能和C++98语法保持一致，避开了自动推导std::initializer_list的问题，
//   也不会不经意间就调用了std::initializer_list构造函数。
//   关于花括号和小括号的使用没有一个一致的观点，所以建议是选择一个方法并遵守它。
}  // namespace test6

namespace test7 {
// 如果是模板的作者，花括号和小括号创建对象就更麻烦了，通常不能知晓哪个会被使用。
// 举个例子，假如想创建一个接受任意数量的参数来创建的对象，
// 使用可变参数模板（variadic template）可以非常简单的解决：

template <typename T, typename... Ts>
void doSomeWork(Ts&&... params) {
  // 在现实中有两种方式实现这个伪代码：
  T localObject1(std::forward<Ts>(params)...);  // 使用小括号
  T localObject2{std::forward<Ts>(params)...};  // 使用花括号
}

void func() {
  std::vector<int> v;
  doSomeWork<std::vector<int>>(10, 20);

  // 如果doSomeWork创建localObject时使用的是小括号，std::vector就会包含10个元素。
  // 如果doSomeWork创建localObject时使用的是花括号，std::vector就会包含2个元素。
  // 哪个是正确的？doSomeWork的作者不知道，只有调用者知道。

  // 这正是标准库函数std::make_unique和std::make_shared面对的问题。
  // 它们的解决方案是使用小括号，并被记录在文档中作为接口的一部分。
  // 注：更灵活的设计，允许调用者决定从模板来的函数应该使用小括号还是花括号是有可能的。
}
}  // namespace test7
}  // namespace n5

// 要记住的东西：
// 1.花括号初始化是最广泛的初始化语法，它防止变窄转换，并对C++最令人头疼的解析有天生的免疫性；
// 2.在构造函数重载决议中，花括号初始化尽最大可能与std::initializer_list参数匹配，
//   即便其他构造函数看起来是更好的选择；
// 3.对于数值类型的std::vector来说使用花括号初始化和小括号初始化会造成巨大的不同；
// 4.在模板类选择使用小括号初始化或使用花括号初始化创建对象是一个挑战。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 7]" << std::endl;
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
      n5::test1::func();
      break;
    case 5:
      n5::test2::func();
      break;
    case 6:
      n5::test4::func();
      break;
    case 7:
      n5::test5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}