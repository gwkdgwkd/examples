#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace n1 {
// 早在C++98中，就有两个函数bind1st和bind2nd，他们都只能绑定一个参数，
// 被用于绑定已有变量到可执行函数的第一个参数和第二个参数。

// 绑定适配器binder1st和binder2nd一般不怎么使用，
// 在程序中一般使用函数适配器辅助函数bind1st和bind2nd，
// bind1st调用了binder1st类，bind2nd调用了binder2nd类。

bool less(int a, int b) { return a < b; }
bool less5(int b) { return 5 < b; }
bool greater(int a, int b) { return a > b; }
bool greater5(int a) { return a > 5; }
std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
void func() {
  // count_if的第三个参数是bool(int)类型的：
  std::cout << "less5:" << std::count_if(v.begin(), v.end(), less5)
            << std::endl;
  std::cout << "greater5:" << std::count_if(v.begin(), v.end(), greater5)
            << std::endl;

  // 使用bool(int,int)类型的less作为count_if的第三参数，
  // 使用bind1st把5绑定到less的第一个参数：
  std::cout << "less+bind1st:"
            << std::count_if(
                   v.begin(), v.end(),
                   std::bind1st(std::function<bool(int, int)>(less), 5))
            << std::endl;
  // 使用bool(int,int)类型的greater作为count_if的第三参数，
  // 使用bind1st把5绑定到greater的第二个参数：
  std::cout << "greater+bind2nd:"
            << std::count_if(
                   v.begin(), v.end(),
                   std::bind2nd(std::function<bool(int, int)>(greater), 5))
            << std::endl;

  // 由于bind1st和bind2st局限性太大，所以C++11中推出了std::bind：
  std::cout << "less+bind:"
            << std::count_if(v.begin(), v.end(),
                             std::bind(less, 5, std::placeholders::_1))
            << std::endl;
  std::cout << "greater+bind:"
            << std::count_if(v.begin(), v.end(),
                             std::bind(greater, std::placeholders::_1, 5))
            << std::endl;

  // less5:3
  // greater5:3
  // less+bind1st:3
  // greater+bind2nd:3
  // less+bind:3
  // greater+bind:3
}
}  // namespace n1

namespace n2 {
// 可以将bind函数看作是一个通用的函数适配器，它接受一个可调用对象，
// 生成一个新的可调用对象来适应原对象的参数列表，调用bind的一般形式：
// auto newCallable = bind(callable,arg_list);
// 其中，newCallable本身是一个可调用对象，arg_list是一个逗号分隔的参数列表，
// 对应给定的callable的参数，当调用newCallable时，
// newCallable会调用callable，并传给它arg_list中的参数。
// arg_list中的参数可能包含形如_n的名字，其中n是一个整数，这些参数是占位符，
// 表示newCallable的参数，它们占据了传递给newCallable的参数的位置。
// 数值n表示生成的可调用对象中参数的位置：
// _1为newCallable的第一个参数，_2为第二个参数，以此类推。

// bind()是最重要的函数适配器，它允许绑定：
// 1.普通函数、静态函数、模板函数；
// 2.类成员函数、类静态成员函数、类成员模板函数、类成员变量、类内部绑定；
// 3.仿函数、Lambda表达式。

namespace test1 {
// 普通函数
int plus(int a, int b) { return a + b; }
void func() {
  auto f1 = std::bind(plus, 1, 2);
  std::cout << f1() << std::endl;  // 3

  auto f2 = std::bind(plus, std::placeholders::_1, 2);
  std::cout << f2(1) << std::endl;  // 3

  auto f3 = std::bind(plus, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(1, 2) << std::endl;  // 3
}
}  // namespace test1

namespace test2 {
// 静态函数
static int plus(int a, int b) { return a + b; }
void func() {
  auto f1 = std::bind(plus, 2, 3);
  std::cout << f1() << std::endl;  // 5

  auto f2 = std::bind(plus, 2, std::placeholders::_1);
  std::cout << f2(3) << std::endl;  // 5

  auto f3 = std::bind(plus, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(2, 3) << std::endl;  // 5
}
}  // namespace test2

namespace test3 {
// 模板函数
template <typename T1 = int, typename T2 = int>
int plus(T1 a, T2 b) {
  return a + b;
}
void func() {
  auto f1 = std::bind(plus<int, int>, 3, 4);
  std::cout << f1() << std::endl;  // 7

  auto f2 = std::bind(plus<int>, std::placeholders::_1, 4);
  std::cout << f2(3) << std::endl;  // 7

  auto f3 = std::bind(plus<>, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(3, 4) << std::endl;  // 7
}
}  // namespace test3

namespace test4 {
// 类成员函数
class Plus {
 public:
  int plus(int a, int b) { return a + b; }
};
void func() {
  Plus p;
  auto f1 = std::bind(&Plus::plus, p, 4, 5);
  std::cout << f1() << std::endl;  // 9
  auto f2 = std::bind(&Plus::plus, p, 4, std ::placeholders::_1);
  std::cout << f2(5) << std::endl;  // 9
  auto f3 =
      std::bind(&Plus::plus, p, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(4, 5) << std::endl;  // 9
}
}  // namespace test4

namespace test5 {
// 类静态成员函数
class Plus {
 public:
  static int plus(int a, int b) { return a + b; }
};
void func() {
  auto f1 = std::bind(&Plus::plus, 5, 6);
  std::cout << f1() << std::endl;  // 11
  auto f2 = std::bind(Plus::plus, std::placeholders::_1, 6);
  std::cout << f2(5) << std::endl;  // 11
  auto f3 =
      std::bind(&Plus::plus, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(5, 6) << std::endl;  // 11
}
}  // namespace test5

namespace test6 {
// 类成员模板函数
class Plus {
 public:
  template <typename T1 = int, typename T2 = int>
  int plus(T1 a, T2 b) {
    return a + b;
  }
};
void func() {
  Plus p;
  auto f1 = std::bind(&Plus::plus<int, int>, p, 6, 7);
  std::cout << f1() << std::endl;  // 13

  auto f2 = std::bind(&Plus::plus<int>, p, std::placeholders::_1, 7);
  std::cout << f2(6) << std::endl;  // 13

  auto f3 =
      std::bind(&Plus::plus<>, p, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(6, 7) << std::endl;  // 13
}
}  // namespace test6

namespace test7 {
// 类成员变量
class A {
 public:
  int a_ = 10;
};
void func() {
  A a;
  auto ma = std::bind(&A::a_, &a);  // 传入类对象指针
  auto mb = std::bind(&A::a_, a);   // 传入对象也行

  std::cout << ma() << ',' << mb() << std::endl;  // 10,10
  a.a_ = 5;
  std::cout << ma() << ',' << mb() << std::endl;  // 5,10

  // 修改a_后，ma跟着改变了，但是mb没有变，是因为传入的是对象，产生了复制么？
}
}  // namespace test7

namespace test8 {
// 类内部绑定
class Plus {
 public:
  int plus(int a, int b) { return a + b; }
  int run() {
    auto p = std::bind(&Plus::plus, this, 7, 8);
    return p();
  };
};
void func() {
  Plus p;
  std::cout << p.run() << std::endl;  // 15
}
}  // namespace test8

namespace test9 {
// 仿函数
class Plus {
 public:
  int operator()(int a, int b) { return a + b; }
};
void func() {
  auto f1 = std::bind(Plus(), 8, 9);
  std::cout << f1() << std::endl;  // 17
  auto f2 = std::bind(Plus(), std::placeholders::_1, 9);
  std::cout << f2(8) << std::endl;  // 17
  auto f3 = std::bind(Plus(), std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(8, 9) << std::endl;  // 17
}
}  // namespace test9

namespace test10 {
// lambda表达式
void func() {
  auto f1 = std::bind([](int a, int b) { return a + b; }, 9, 10);
  std::cout << f1() << std::endl;  // 19
  auto f2 =
      std::bind([](int a, int b) { return a + b; }, 9, std::placeholders::_1);
  std::cout << f2(10) << std::endl;  // 19
  auto f3 = std::bind([](int a, int b) { return a + b; }, std::placeholders::_1,
                      std::placeholders::_2);
  std::cout << f3(9, 10) << std::endl;  // 19
}
}  // namespace test10

namespace test11 {
// 标准库定义的函数对象
void func() {
  auto f1 = std::bind(std::plus<int>(), 10, 11);
  std::cout << f1() << std::endl;  // 21
  auto f2 = std::bind(std::plus<int>(), std::placeholders::_1, 11);
  std::cout << f2(10) << std::endl;  // 21
  auto f3 =
      std::bind(std::plus<int>(), std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(10, 11) << std::endl;  // 21
}
}  // namespace test11

void func() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
  test5::func();
  test6::func();
  test7::func();
  test8::func();
  test9::func();
  test10::func();
  test11::func();
}
}  // namespace n2

namespace n3 {
// std::placeholders::_1是占位符，其中_1，是数字对象，用于function的bind中。
// _1用于代替回调函数（给bind函数传入的函数对象）中的第一个参数，
// _2用于代替回调函数中的第二个参数，以此类推。

void f(std::string arg1, std::string arg2) {
  std::cout << "arg1:" << arg1 << " , "
            << "arg2:" << arg2 << std::endl;
}
void func() {
  f("hello", "world");  // arg1:hello , arg2:world

  using namespace std::placeholders;
  // 改变参数顺序：
  auto order = std::bind(f, _2, _1);
  order("hello", "world");  // arg1:world , arg2:hello

  // std::is_placeholder用于判断T是否为占位符，它有一个成员变量value。
  // 如果T是placeholder类型，value的值为1代表_1，2代表_2；如果不是，则value为0。
  int _6;
  std::cout << std::is_placeholder<decltype(_1)>::value << std::endl;  // 1
  std::cout << std::is_placeholder<decltype(_6)>::value << std::endl;  // 0
  std::cout << std::is_placeholder<decltype(_9)>::value << std::endl;  // 9
  std::cout << std::is_placeholder<int>::value << std::endl;           // 0

  std::cout << std::boolalpha;
  std::cout << std::is_bind_expression<decltype(order)>::value
            << std::endl;  // true
  std::cout << std::is_bind_expression<decltype(f)>::value
            << std::endl;  // false
}
}  // namespace n3

namespace n4 {
// bind()默认是以by value的方式传递参数值，如果想要以引用的方式传递参数，
// 可以使用ref()或者cref()，std::ref和std::cref是函数模板，
// 返回类模板std::reference_wrapper的对象，
// std::reference_wrapper内部可以用指针指向需要绑定到的变量上，
// 通过指针访问变量和修改变量的值，就实现了reference的作用。

void f(int &n1, int &n2, const int &n3) {
  std::cout << "step 2:" << n1 << "," << n2 << "," << n3 << std::endl;
  ++n1;
  ++n2;
  const_cast<int &>(n3)++;
}

void func() {
  int n1 = 1;
  int n2 = 2;
  int n3 = 3;

  std::cout << "step 1:" << n1 << "," << n2 << "," << n3 << std::endl;
  std::bind(f, n1, std::ref(n2), std::cref(n3))();
  std::cout << "step 3:" << n1 << "," << n2 << "," << n3 << std::endl;

  // 1:1,2,3
  // 2:1,2,3
  // 3:1,3,4
}
}  // namespace n4

int main(int argc, char *argv[]) {
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