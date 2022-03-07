#include <algorithm>
#include <functional>
#include <iostream>

// lambda源自希腊字母表中第11位的λ，在计算机科学领域，它则是被用来表示一种匿名函数。
// 所谓匿名函数，简单地理解就是没有名称的函数，又常被称为lambda函数或者lambda表达式。
// 继Python、Java、C#、PHP等众多高级编程语言都支持lambda匿名函数后，C++11标准终于引入了lambda。

// 定义一个lambda匿名函数很简单，可以套用如下的语法格式:
// [外部变量访问方式说明符] (参数) mutable noexcept/throw() -> 返回值类型
// {
//    函数体;
// };
// 其中各部分的含义分别为：
// 1.[外部变量方位方式说明符]
//   []方括号用于向编译器表明当前是一个lambda表达式，其不能被省略。
//   在方括号内部，可以注明当前lambda函数的函数体中可以使用哪些“外部变量”。
//   所谓外部变量，指的是和当前lambda表达式位于同一作用域内的所有局部变量。
// 2.(参数)
//   和普通函数的定义一样，lambda匿名函数也可以接收外部传递的多个参数。
//   和普通函数不同的是，如果不需要传递参数，可以连同()小括号一起省略。
// 3.mutable
//   此关键字可以省略，如果使用则之前的()小括号将不能省略（参数个数可以为0）。
//   默认情况下，对于以值传递方式引入的外部变量，不允许在lambda表达式内部修改它们的值，
//   可以理解为这部分变量都是const常量。而如果想修改它们，就必须使用mutable关键字。
//   注意，对于以值传递方式引入的外部变量，lambda表达式修改的是拷贝的那一份，并不会修改真正的外部变量。
// 4.noexcept/throw()
//   可以省略，如果使用，在之前的()小括号将不能省略（参数个数可以为0）。
//   默认情况下，lambda函数的函数体中可以抛出任何类型的异常。
//   而标注noexcept关键字，则表示函数体内不会抛出任何异常；
//   使用throw()可以指定lambda函数内部可以抛出的异常类型。
//   如果lambda函数标有noexcept而函数体内抛出了异常，又或者使用throw()限定了异常类型
//   而函数体内抛出了非指定类型的异常，这些异常无法使用try-catch捕获，会导致程序执行失败。
// 5.->返回值类型
//   指明lambda匿名函数的返回值类型。
//   如果lambda函数体内只有一个return语句，或者该函数返回void，则编译器可以自行推断出返回值类型，
//   此情况下可以直接省略->返回值类型。如果使用，在之前的()小括号将不能省略（参数个数可以为0）。
// 6.函数体
//   和普通函数一样，lambda匿名函数包含的内部代码都放置在函数体中。
//   该函数体内除了可以使用指定传递进来的参数之外，还可以使用指定的外部变量以及全局范围内的所有全局变量。
//   外部变量会受到以值传递还是以引用传递方式引入的影响，而全局变量则不会。
//   换句话说，在lambda表达式内可以使用任意一个全局变量，必要时还可以直接修改它们的值。

// 比如，如下就定义了一个最简单的lambda匿名函数：
// []{}
// 显然，此lambda匿名函数未引入任何外部变量（[]内为空），也没有传递任何参数，
// 没有指定mutable、noexcept等关键字，没有返回值和函数体。
// 所以，这是一个没有任何功能的lambda匿名函数。

namespace n1 {
// lambda匿名函数中的[外部变量]，外部变量是指和当前lambda表达式位于同一作用域内的所有局部变量:

// []:空方括号表示当前lambda匿名函数中不导入任何外部变量；
void func1() {
  int i = 1;
  [] {
    // std::cout << i << std::endl;  // error: ‘i’ is not captured
  }();
}

// [=]:只有一个=等号，表示以值传递的方式导入所有外部变量；
void func2() {
  int i = 2;
  float f = 2.2;
  std::string s = "func2";
  [=] {
    // i = 3;  // assignment of read-only variable ‘i’
    // f = 3.3;
    // s = "modify func2";
    std::cout << i << std::endl;  // 2
    std::cout << f << std::endl;  // 2.2
    std::cout << s << std::endl;  // func2
  }();
}

// [&]:只有一个&符号，表示以引用传递的方式导入所有外部变量；
void func3() {
  int i = 2;
  float f = 2.2;
  std::string s = "func2";
  std::cout << "before : " << i << std::endl;  // 2
  [&] {
    i = 3;
    std::cout << i << std::endl;  // 3
    f = 3.3;
    std::cout << f << std::endl;  // 3.3
    s = "func3";
    std::cout << s << std::endl;  // func3
  }();

  std::cout << "after : " << i << std::endl;  // 3
}

// [val1,val2,...]:表示以值传递的方式导入val1、val2等外部变量，同时多个变量之间没有先后次序；
void func4() {
  int i = 4;
  float f = 4.4;
  std::string s = "func4";
  [i, f] {
    std::cout << i << std::endl;  // 4
    std::cout << f << std::endl;  // 4.4
    // std::cout << s << std::endl;
  }();
}

// [&val1,&val2,...]:表示以引用传递的方式导入val1、val2等外部变量，多个变量之间没有前后次序；
void func5() {
  int i = 4;
  float f = 5.5;
  std::string s = "func5";
  [&i, &s] {
    std::cout << ++i << std::endl;  // 5
    // std::cout << f << std::endl;
    s += " ext";
    std::cout << s << std::endl;  // func5 ext
  }();
}

// [val,&val2,...]:以上2种方式还可以混合使用，变量之间没有前后次序。
void func6() {
  int i = 5;
  float f = 6.6;
  std::string s = "func6";
  [&i, s, f] {
    std::cout << ++i << std::endl;  // 6
    std::cout << f << std::endl;    // 6.6
    std::cout << s << std::endl;    // func6
  }();
}

// [=,&val1,...]:表示除val1以引用传递的方式导入外，其它外部变量都以值传递的方式导入。
void func7() {
  int i = 6;
  float f = 7.7;
  std::string s = "func7";
  [=, &i] {
    std::cout << ++i << std::endl;  // 7
    std::cout << f << std::endl;    // 7.7
    std::cout << s << std::endl;    // func7
  }();

  [&, i, s] {
    std::cout << i << std::endl;    // 7
    std::cout << ++f << std::endl;  // 8.7
    std::cout << s << std::endl;    // func7
  }();
}

// 注意，单个外部变量不允许以相同的传递方式导入多次。
// 例如[=，val1]中，val1先后被以值传递的方式导入了2次，这是非法的。
void func8() {
  int i = 8;
  // [=, i] {}();
  // [&, &i] {}();
  // warning: explicit by-reference capture of ‘i’ redundant with by-reference capture default
}

// [this]:表示以值传递的方式导入当前的this指针。
class A {
 public:
  A(int i) : i_(i) {}
  void Set(int i) { i_ = i; }
  int Get() { return i_; }

  void test() {
    [this] { this->Set(this->Get() + 5); }();
  }

 private:
  int i_;
};
void func9() {
  A a(0);
  std::cout << a.Get() << std::endl;  // 0
  a.test();
  std::cout << a.Get() << std::endl;  // 5
}

void testN1() {
  func1();
  func2();
  func3();
  func4();
  func5();
  func6();
  func7();
  func8();
  func9();
}
}  // namespace n1

namespace n2 {
void func1() {
  [] {
    std::cout << "no argv" << std::endl;  // no argv
  }();
  [](int i) {
    std::cout << i << std::endl;  // 5
  }(5);
}

void func2() {
  // 虽然lambda匿名函数没有函数名称，但我们仍可以为其手动设置一个名称，比如：
  auto display1 = [](int a, int b) -> void {
    std::cout << a << " " << b << std::endl;
  };
  // 程序中使用auto关键字为lambda匿名函数设定了一个函数名，由此即可在作用域内调用该函数。
  display1(10, 20);  // 10 20

  std::function<void(int, int)> display2 = [](int a, int b) -> void {
    std::cout << a << " " << b << std::endl;
  };
  display2(10, 20);  // 10 20
}

void func3() {
  int i = 3;
  [=]() mutable {
    std::cout << i << std::endl;  // 3
    i = 5;
    std::cout << i << std::endl;  // 5
  }();
  std::cout << i << std::endl;  // 3
}

void func4() {
  std::cout << [] { return 5; }() << std::endl;        // 5
  std::cout << [] { return 2.3; }() << std::endl;      // 2.3
  std::cout << [] { return "nihao"; }() << std::endl;  // nihao

  [] { return; }();
  []() -> void { return; }();
  std::cout << []() -> int { return 6; }() << std::endl;    // 6
  std::cout << []() -> float { return 7; }() << std::endl;  // 7
}

int g = 5;
void func5() {
  std::cout << "g : " << g << std::endl;  // 5
  [] {
    g = 6;
    std::cout << "g : " << g << std::endl;  // 6
  }();
  std::cout << "g : " << g << std::endl;  // 6
}

void testN2() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n2

namespace n3 {
void testN3_1() {
  // 如果不使用noexcept或者throw()，则lambda匿名函数的函数体中允许发生任何类型的异常。
  try {
    []() { throw 10; }();
  } catch (int i) {
    std::cout << "catch int exception " << i << std::endl;
  }
  try {
    []() { throw std::string("ex"); }();
  } catch (std::string s) {
    std::cout << "catch string exception " << s << std::endl;
  }

  // 执行抛出异常类型:
  auto except = []() throw(int) { throw 10; };
  try {
    except();
  } catch (int) {
    std::cout << "catch int exception" << std::endl;
  }
}

void testN3_2() {  // 反例1
  // except匿名函数指定了函数体中不发生任何异常，但函数体中却发生了整形异常；
  auto except = []() noexcept { throw 100; };
  try {
    except();
  } catch (int) {
    std::cout << "catch int exception" << std::endl;
  }
}

void testN3_3() {  // 反例2
  // except匿名函数指定函数体可能会发生字符异常，但函数体中却发生了整形异常。
  auto except = []() throw(char) { throw 10; };
  try {
    except();
  } catch (int) {
    std::cout << "catch int exception" << std::endl;
  }
  // 由于指定异常类型和真正发生的异常类型不匹配，导致try-catch无法捕获，最终程序运行崩溃。
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3_1();
      break;
    case 3:
      n3::testN3_2();
      break;
    case 4:
      n3::testN3_3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}