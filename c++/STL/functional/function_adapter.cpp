#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

// C++中有三类适配器：
// 1.容器适配器；
// 2.迭代器适配器；
// 3.函数适配器。

// 函数适配器（function adapter），自身也是个function object
// 是指能够将不同的function object（或是和某值或某寻常函数）结合起来的东西。

// STL中定义了大量的函数对象，但是有时候需要对函数返回值进一步的简单计算，
// 或者填上多余的参数，不能直接带入算法。
// 函数适配器实现了这一功能，将一种函数对象转化为另一种符合需求的函数对象。
// 函数适配器分为4大类：
// 1.绑定适配器
//   binder1st,binder2nd
// 2.组合适配器
//   unary_negate,binary_negate
// 3.指针函数适配器
//   pointer_to_unary_function,pointer_to_binary_function
// 4.成员函数适配器
//   mem_fun_t,mem_fun_ref_t,
//   mem_fun1_t,mem_fun1_ref_t,
//   cosnt_mem_fun_t,cosnt_mem_fun_ref_t,
//   cosnt_mem_fun1_t,cosnt_mem_fun1_ref_t

// 直接构造STL中的函数适配器通常会导致冗长的类型声明。
// 为简化函数适配器的构造，C++98 STL还提供了函数适配器辅助函数，
// 借助于泛型自动推导技术，无需显示的类型声明便可实现函数适配器的构造：
// 1.bind1st
// 2.bind2nd
// 3.not1
// 4.not2
// 5.ptr_fun
// 6.mem_fun
// 7.mem_fun_ref
// 这些适配器都需要使用定义于function object中的若干类型。
// 为了定义那些类型，C++标准库为function object提供了特殊的base calss，
// std::unary_function<>和std::binary_function<>，
// 这些class如今都已过时。

// C++98提供的函数适配器特性在C++11的到来全部过时，
// C++11引入了更方便更弹性的adapter：bind、mem_fn、not1、not2。

namespace n1 {
// 可以将bind函数看作是一个通用的函数适配器，它接受一个可调用对象，
// 生成一个新的可调用对象来适应原对象的参数列表。
// 调用bind的一般形式：
// auto newCallable = bind(callable,arg_list);
// 其中，
// newCallable本身是一个可调用对象，
// arg_list是一个逗号分隔的参数列表，对应给定的callable的参数。
// 即，当调用newCallable时，newCallable会调用callable，
// 并传给它arg_list中的参数。
// arg_list中的参数可能包含形如_n的名字，其中n是一个整数，
// 这些参数是占位符，表示newCallable的参数，
// 它们占据了传递给newCallable的参数的位置。
// 数值n表示生成的可调用对象中参数的位置：
// _1为newCallable的第一个参数，_2为第二个参数，以此类推。

// bind()是最重要的函数适配器，它允许：
// 1.在既有的或预定义的函数对象之外另配接及合成新的函数对象；
// 2.调用全局函数；
// 3.针对对象、对象指针、对象智能指针调用成员函数；

namespace test1 {
int plus(int a, int b) { return a + b; }
void func() {
  auto f1 = std::bind(plus, 1, 2);
  std::cout << f1() << std::endl;  // 3

  auto f2 = std::bind(plus, std::placeholders::_1, 2);
  std::cout << f2(3) << std::endl;  // 5

  auto f3 = std::bind(plus, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(3, 4) << std::endl;  // 7
}
}  // namespace test1

namespace test2 {
class Plus {
 public:
  int plus(int a, int b) { return a + b; }
};
void func() {
  Plus p;
  auto f1 = std::bind(&Plus::plus, p, 1, 2);
  std::cout << f1() << std::endl;  // 3
  auto f2 = std::bind(&Plus::plus, p, std::placeholders::_1, 2);
  std::cout << f2(3) << std::endl;  // 5
  auto f3 =
      std::bind(&Plus::plus, p, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(3, 4) << std::endl;  // 7
}
}  // namespace test2

namespace test3 {
class Plus {
 public:
  static int plus(int a, int b) { return a + b; }
};
void func() {
  Plus p;
  auto f1 = std::bind(&Plus::plus, 1, 2);
  std::cout << f1() << std::endl;  // 3
  auto f2 = std::bind(Plus::plus, std::placeholders::_1, 2);
  std::cout << f2(3) << std::endl;  // 5
  auto f3 =
      std::bind(&Plus::plus, std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(3, 4) << std::endl;  // 7
}
}  // namespace test3

namespace test4 {
void func() {
  auto f1 = std::bind(std::plus<int>(), 1, 2);
  std::cout << f1() << std::endl;  // 3
  auto f2 = std::bind(std::plus<int>(), std::placeholders::_1, 2);
  std::cout << f2(3) << std::endl;  // 5
  auto f3 =
      std::bind(std::plus<int>(), std::placeholders::_1, std::placeholders::_2);
  std::cout << f3(3, 4) << std::endl;  // 7
}
}  // namespace test4

void test() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n1

namespace n2 {
// bind()默认是以by value的方式传递参数值，
// 如果想要以引用的方式传递参数，可以使用ref()或者cref()。
// std::ref和std::cref是函数模板，
// 返回类模板std::reference_wrapper的对象，
// std::reference_wrapper内部可以用指针指向需要绑定到的变量上，
// 通过指针访问变量和修改变量的值，就实现了reference的作用。

void f(int &n1, int &n2, const int &n3) {
  std::cout << "f:" << n1 << "," << n2 << "," << n3 << std::endl;
  ++n1;
  ++n2;
  const_cast<int &>(n3)++;
}

void test() {
  int n1 = 1;
  int n2 = 2;
  int n3 = 3;
  std::function<void()> bound = std::bind(f, n1, std::ref(n2), std::cref(n3));

  n1 = 11;
  n2 = 12;
  n3 = 13;
  std::cout << "b:" << n1 << "," << n2 << "," << n3 << std::endl;
  bound();
  std::cout << "a:" << n1 << "," << n2 << "," << n3 << std::endl;

  // b:11,12,13
  // f:1,12,13
  // a:11,13,14
}
}  // namespace n2

namespace n3 {
// binder1st和binder2nd一般不怎么使用，
// 在程序中一般使用bind1st和bind2nd。
// bind1st调用了binder1st类，bind2nd调用了binder2nd类。

std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
void func1() {
  std::cout << ">5 count(bind1st + less):"
            << std::count_if(v.begin(), v.end(),
                             std::bind1st(std::less<int>(), 5))
            << std::endl;
  std::cout << ">5 count(bind2nd + greater):"
            << std::count_if(v.begin(), v.end(),
                             std::bind2nd(std::greater<int>(), 5))
            << std::endl;

  // >5 count(bind1st + less):3
  // >5 count(bind2nd + greater):3
}
void func2() {
  std::cout << ">5 count(bind + less):"
            << std::count_if(
                   v.begin(), v.end(),
                   std::bind(std::less<int>(), 5, std::placeholders::_1))
            << std::endl;
  std::cout << ">5 count(bind + greater):"
            << std::count_if(
                   v.begin(), v.end(),
                   std::bind(std::greater<int>(), std::placeholders::_1, 5))
            << std::endl;

  // >5 count(bind + less):3
  // >5 count(bind + greater):3
}
void test() {
  func1();
  func2();
}
}  // namespace n3

namespace n4 {
// not1是构造一个与谓词结果相反的一元函数对象。
// not2是构造一个与谓词结果相反的二元函数对象。

std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
namespace test1 {
struct IsOdd {
  bool operator()(const int &i) const { return i % 2 == 1; }
  typedef int argument_type;  // 这个类型not1需要
};
void func() {
  std::cout << "Odd count:" << std::count_if(v.begin(), v.end(), IsOdd())
            << std::endl;
  std::cout << "Even count:"
            << std::count_if(v.begin(), v.end(), std::not1(IsOdd()))
            << std::endl;

  // Odd count:5
  // Even count:4
}
}  // namespace test1

namespace test2 {
// unary_funciton可以作为一个一元函数对象（又叫仿函数）的基类，
// 它只定义了参数和返回值的类型，本身并不重载()操作符，
// 这个任务交给派生类去完成。
struct IsOdd : public std::unary_function<int, bool> {
  bool operator()(const int &i) const { return i % 2 == 1; }
};
void func() {
  std::cout << "Odd count:" << std::count_if(v.begin(), v.end(), IsOdd())
            << std::endl;
  std::cout << "Even count:"
            << std::count_if(v.begin(), v.end(), std::not1(IsOdd()))
            << std::endl;

  // Odd count:5
  // Even count:4
}
}  // namespace test2

std::vector<int> v1{10, 20, 30, 40, 50};
std::vector<int> v2{0, 15, 30, 45, 60};
namespace test3 {
void func() {
  auto it =
      std::mismatch(v1.begin(), v1.end(), v2.begin(), std::equal_to<int>());
  std::cout << "First mismatch:" << *it.first << " " << *it.second << std::endl;
  it = std::mismatch(v1.begin(), v1.end(), v2.begin(),
                     std::not2(std::equal_to<int>()));
  std::cout << "First match:" << *it.first << " " << *it.second << std::endl;

  // First mismatch:10 0
  // First match:30 30
}
}  // namespace test3

namespace test4 {
// binary_function可以作为一个二元函数对象的基类，
// 它只定义了参数和返回值的类型，本身并不重载()操作符，
// 这个任务交给派生类去完成。
class Equal : public std::binary_function<int, int, bool> {
 public:
  bool operator()(const int &a, const int &b) const { return a == b; }
};
void func() {
  auto it = std::mismatch(v1.begin(), v1.end(), v2.begin(), Equal());
  std::cout << "First mismatch:" << *it.first << " " << *it.second << std::endl;
  it = std::mismatch(v1.begin(), v1.end(), v2.begin(), std::not2(Equal()));
  std::cout << "First match:" << *it.first << " " << *it.second << std::endl;

  // First mismatch:10 0
  // First match:30 30
}
}  // namespace test4

void test() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n4

namespace n5 {
class C {
 public:
  void display() { std::cout << "Hello,world" << std::endl; }
  void display_num(int i) { std::cout << "num:" << i << std::endl; }
  int data = 7;
};
void func1() {
  // 当调用成员函数或成员变量时，如果不需要绑定参数，
  // 可以使用std::mem_fn，相比于bind()，就不需要占位符来表示调用者了：
  C c;
  auto greet = std::mem_fn(&C::display);
  greet(c);
  auto display = std::mem_fn(&C::display_num);
  display(c, 5);
  auto d = std::mem_fn(&C::data);  // 成员变量也可以
  std::cout << "data:" << d(c) << std::endl;

  // Hello,world
  // num:5
  // data:7
}

void func2() {
  // std::mem_fn都可以用std::bind替换：
  C c;
  auto greet = std::bind(&C::display, std::placeholders::_1);
  greet(c);
  auto display =
      std::bind(&C::display_num, std::placeholders::_1, std::placeholders::_2);
  display(c, 5);
  auto d = std::bind(&C::data, std::placeholders::_1);
  std::cout << "data:" << d(c) << std::endl;

  // Hello,world
  // num:5
  // data:7
}

void test() {
  func1();
  func2();
}
}  // namespace n5

namespace n6 {
// 如果有一个函数f和一个对象x，现在希望在x上调用f，而在x的成员函数之外，
// 为了执行这个调用操作，C++提供了3种不同的语法：
//  f(x);     // f是一个non-member-func
//  x.f();    // f是一个member-func,x是一个对象or对象的引用
//  px->f();  // f是一个member-func,且px是一个指向对象x的指针
// 由于存在这3种不同的语法，因此在使用STL的一些组件时，不免会产生一些语法上的冲突。
// 而ptr_func、mem_fun、mem_func_ref这三个函数，
// 就是用来掩盖C++语言中的一个内在语法不一致的问题！
class C {
  int i_;

 public:
  C(int i) : i_(i) {}
  void testOdd() {
    std::cout << i_ << ":" << (i_ % 2 ? "true" : "false") << std::endl;
  }
};
void testOdd(C &c) { c.testOdd(); }

void func1() {
  std::vector<C> v{C(1), C(2), C(3)};

  std::for_each(v.begin(), v.end(), testOdd);
  // 1:true
  // 2:false
  // 3:true

  // 将一个非成员函数传递给一个STL组件时，多数时候是一个STL算法，
  // 为了避免你为此感到困惑（不加应该也行），要在函数前加一个ptr_fun：
  std::for_each(v.begin(), v.end(), std::ptr_fun(testOdd));
  // 1:true
  // 2:false
  // 3:true
  // 如果你不知道什么时候该使用ptr_fun，什么时候不该使用，
  // 那么大可以再每一次将函数传递给一个STL组件时总是使用它，
  // STL不会在意，且这么做也不会带来代码运行时性能上的损失！

  // 将一个成员函数传递给一个STL组件时，多数时候是一个STL算法，
  // 并且对应之STL容器中存储的是类的对象或引用时，
  // 你要在函数前加一个mem_fun_ref：
  std::for_each(v.begin(), v.end(), std::mem_fun_ref(&C::testOdd));
  // 1:true
  // 2:false
  // 3:true

  // 函数模板mem_fn生成指向成员的指针的包装对象，
  // 该对象可以存储，复制和调用指向成员的指针。
  // mem_fn既可以用于指针（包括智能指针）、引用，还可以用于对象本身，
  // 因此在C++11中使用mem_fn可以替代mem_fun和mem_fun_ref：
  std::for_each(v.begin(), v.end(), std::mem_fn(&C::testOdd));  // 对象
  // 1:true
  // 2:false
  // 3:true
}

void func2() {
  std::vector<C *> v{new C(1), new C(2), new C(3)};

  // 将一个成员函数传递给一个STL组件时，多数时候是一个STL算法，
  // 并且对应之STL容器中存储的是指向类对象的指针时，
  // 你要在函数前加一个mem_fun：
  std::for_each(v.begin(), v.end(), std::mem_fun(&C::testOdd));
  // 1:true
  // 2:false
  // 3:true

  std::for_each(v.begin(), v.end(), std::mem_fn(&C::testOdd));  // 指针
  // 1:true
  // 2:false
  // 3:true
}

void func3() {
  std::vector<std::shared_ptr<C>> v{
      std::make_shared<C>(1), std::make_shared<C>(2), std::make_shared<C>(3)};

  // std::for_each(v.begin(), v.end(), std::mem_fun(&C::testOdd)); // 编译不过
  std::for_each(v.begin(), v.end(), std::mem_fn(&C::testOdd));  // 智能指针
  // 1:true
  // 2:false
  // 3:true
}

void test() {
  func1();
  func2();
}
}  // namespace n6

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    case 2:
      n3::test();
      break;
    case 3:
      n4::test();
      break;
    case 4:
      n5::test();
      break;
    case 5:
      n6::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}