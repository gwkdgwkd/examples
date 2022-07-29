#include <iostream>
#include <vector>

// 在C++11之前的版本（C++98和C++03）中，
// 定义变量或者声明变量之前都必须指明它的类型，比如int、char等；
// 但是在一些比较灵活的语言中，比如C#、JavaScript、PHP、Python等，
// 程序员在定义变量时可以不指明具体的类型，
// 而是让编译器（或者解释器）自己去推导，这就让代码的编写更加方便。
// C++11为了顺应这种趋势也开始支持自动类型推导了！
// C++11使用auto关键字来支持自动类型推导。

// 在之前的C++版本中，auto关键字用来指明变量的存储类型，它和static关键字是相对的。
// auto表示变量是自动存储的，这也是编译器的默认规则，所以写不写都一样，
// 一般也不写，这使得auto关键字的存在变得非常鸡肋。
// C++11赋予auto关键字新的含义，使用它来做自动类型推导。
// 也就是说，使用了auto关键字以后，编译器会在编译期间自动推导出变量的类型，
// 这样就不用手动指明变量的数据类型了。

// auto关键字基本的使用语法如下(name是变量的名字，value是变量的初始值)：
// auto name = value;
// auto仅仅是一个占位符，在编译器期间它会被真正的类型所替代。
// 或者说，C++中的变量必须是有明确类型的，只是这个类型是由编译器自己推导出来的。

namespace n1 {
void func1() {
  auto n = 10;
  auto l = 10l;
  auto f = 12.8f;
  auto d = 12.8;
  auto p = &n;
  auto url = "http://c.biancheng.net/cplus/";

  std::cout << typeid(n).name() << std::endl;    // i
  std::cout << typeid(l).name() << std::endl;    // l
  std::cout << typeid(f).name() << std::endl;    // f
  std::cout << typeid(d).name() << std::endl;    // d
  std::cout << typeid(p).name() << std::endl;    // Pi
  std::cout << typeid(url).name() << std::endl;  // PKc

  // 也可以连续定义多个变量:
  int b = 20;
  auto *q = &b, m = 99;
  // 编译器根据第一个子表达式已经推导出auto为int，那后面的m也只能是int，
  // 如果写作m=12.5是错误的，因为12.5是double，和int是冲突的。
  // auto *q = &b, m = 12.5;  // 推导的时候不能有二义性。
}

void func2() {
  // auto还可以和某些具体类型混合使用，这样auto表示的就是半个类型，而不是完整的类型:
  int x = 0;
  auto *p1 = &x;  // p1为int*，auto推导为int
  auto p2 = &x;   // p2为int*，auto推导为int*
  auto &r1 = x;   // r1为int&，auto推导为int
  // r1是int&类型，当=右边的表达式是一个引用类型时，auto会把引用抛弃，直接推导出它的原始类型。
  auto r2 = r1;  // r2为int，auto推导为int。

  std::cout << typeid(p1).name() << std::endl;  // Pi
  std::cout << typeid(p2).name() << std::endl;  // Pi
  std::cout << typeid(r1).name() << std::endl;  // i
  std::cout << typeid(r2).name() << std::endl;  // i
}

void func3() {
  // auto和const的结合:
  // 当类型不为引用时，auto的推导结果将不保留表达式的const属性；
  // 当类型为引用时，auto的推导结果将保留表达式的const属性。

  int x = 0;
  // 当=右边的表达式带有const属性时，auto不会使用const属性，而是直接推导出non-const类型。
  const auto n = x;  // n为const int，auto被推导为int
  auto f = n;        // n为const int，auto被推导为int
  f = 5;

  // 当const和引用结合时，auto的推导将保留表达式的const类型。
  const auto &r1 = x;  // r1为const int&类型，auto被推导为int
  auto &r2 = r1;  // r1为const int&类型，auto被推导为const int类型
  // r2 = 5;         // assignment of read-only reference ‘r2’

  std::cout << typeid(n).name() << std::endl;   // i
  std::cout << typeid(f).name() << std::endl;   // i
  std::cout << typeid(r1).name() << std::endl;  // i
  std::cout << typeid(r2).name() << std::endl;  // i
}

void testN1() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// auto的限制:

// 2.auto不能在函数的参数中使用
// 在定义函数的时候只是对参数进行了声明，指明了参数的类型，但并没有给它赋值，
// 只有在实际调用函数的时候才会给参数赋值；
// 而auto要求必须对变量进行初始化，所以这是矛盾的。
// void func1(auto i) {}

template <typename T>
class A {};

void testN2() {
  // 1.使用auto类型推导的变量必须马上初始化，因为auto在C++11中只是占位符，
  //   并非如int一样的真正的类型声明
  // auto i;

  // 3.auto关键字不能定义数组
  // auto str[] = url; // ‘str’ declared as array of ‘auto’

  // 4.auto不能作用于模板参数
  A<int> C1;
  // A<auto> C2 = C1;  // invalid use of ‘auto’
}
}  // namespace n2

namespace n3 {
// auto的应用场景:

// 1.auto的典型应用场景是用来定义STL的迭代器
void func1() {
  std::vector<std::vector<int>> v;
  std::vector<std::vector<int>>::iterator i1 = v.begin();
  // 使用auto代替具体的类型，auto可以根据表达式v.begin()的类型来推导出变量i的类型。
  auto i2 = v.begin();
  if (i1 == i2) {
    std::cout << "same" << std::endl;
  }
  // same
}

// 2.当不知道变量是什么类型，或者不希望指明具体类型的时候，比如泛型编程中，可以使用auto
class A {
 public:
  static int get(void) { return 100; }
};
class B {
 public:
  static const char *get(void) { return "http://c.biancheng.net/cplus/"; }
};
template <typename T>
void func(void) {
  auto val = T::get();
  std::cout << val << std::endl;
}
template <typename T1, typename T2>  // 不使用auto，需要额外增加一个模板参数T2
void func(void) {
  T2 val = T1::get();
  std::cout << val << std::endl;
}
void func2() {
  // func使用了auto，只需要一个参数
  func<A>();  // 100
  func<B>();  // http://c.biancheng.net/cplus/

  // func不使用auto，调用时需要手动传递参数类型
  func<A, int>();           // 100
  func<B, const char *>();  // http://c.biancheng.net/cplus
}

void testN3() {
  func1();
  func2();
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
      n3::testN3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}