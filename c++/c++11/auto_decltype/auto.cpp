#include <iostream>
#include <vector>

// C++11之前，在98和03中，定义变量或声明变量都必须指明类型，比如int、char等。
// 但是在一些比较灵活的语言中，比如C#、JavaScript、PHP、Python等，
// 定义时可以不指明类型，而是让编译器去推导，这就让代码的编写更加方便。
// C++11为了顺应这种趋势也开始支持自动类型推导了，使用auto关键字来支持自动类型推导。

// 在C++11以前，auto用来指明变量的存储类型，它和static关键字是相对的。
// auto表示变量是自动存储的，这也是默认规则，所以使得auto变得非常鸡肋。
// C++11赋予auto关键字新的含义，使用它来做自动类型推导。

// auto关键字基本的使用语法如下：
// auto name = value;
// name是变量的名字，value是变量的初始值。
// auto仅仅是一个占位符，在编译器期间它会被真正的类型所替代。
// C++中的变量必须是有明确类型的，只是这个类型是由编译器自己推导出来的。

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

  int b = 20;
  // 可以连续定义多个变量，但不能有歧义：
  auto *q = &b, m = 99;  // 根据第一个子表达式推导出int，后面的m也只能是int
  // auto *q = &b, m = 12.5;  // 报错，12.5是double，与int有歧义
}

void func2() {
  // auto可以和某些具体类型混合使用，表示半个类型，而不是完整的类型：
  int x = 0;
  auto *p1 = &x;  // p1为int*，auto推导为int
  auto p2 = &x;   // p2为int*，auto推导为int*
  auto &r1 = x;   // r1为int&，auto推导为int

  // 当=右边是引用类型时，auto会把引用抛弃，直接推导出它的原始类型：
  auto r2 = r1;  // r2为int，auto推导为int

  std::cout << typeid(p1).name() << std::endl;  // Pi
  std::cout << typeid(p2).name() << std::endl;  // Pi
  std::cout << typeid(r1).name() << std::endl;  // i
  std::cout << typeid(r2).name() << std::endl;  // i
}

void func3() {
  // auto和const的结合：
  // 1.当类型不为引用时，auto的推导结果将不保留表达式的const属性；
  // 2.当类型为引用时，auto的推导结果将保留表达式的const属性。

  int x = 0;
  const auto n = x;  // n为const int，auto被推导为int
  auto f = n;        // n为const int且不是引用，去掉const，f为int
  f = 5;

  const auto &r1 = x;  // r1为const int&类型，auto被推导为int
  auto &r2 = r1;  // r1为const int&类型，auto被推导为const int类型
  // r2 = 5;         // assignment of read-only reference ‘r2’

  std::cout << typeid(n).name() << std::endl;   // i
  std::cout << typeid(f).name() << std::endl;   // i
  std::cout << typeid(r1).name() << std::endl;  // i
  std::cout << typeid(r2).name() << std::endl;  // i
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// auto的限制：

// 2.auto不能在函数的参数中使用，在定义函数的时候只是对参数进行了声明，
//   指明了参数的类型，但并没有给它赋值，只有在实际调用时才会给参数赋值，
//   而auto要求必须对变量进行初始化，所以这是矛盾的：
// void func1(auto i) {}

template <typename T>
class A {};

void func() {
  // 1.使用auto推导的变量必须初始化，因为auto只是占位符，并非如int一样类型声明：
  // auto i;

  // 3.auto关键字不能定义数组：
  // auto str[] = url; // ‘str’ declared as array of ‘auto’

  // 4.auto不能作用于模板参数：
  A<int> C1;
  // A<auto> C2 = C1;  // invalid use of ‘auto’
}
}  // namespace n2

namespace n3 {
// auto的应用场景：

// 1.auto的典型应用场景是用来定义STL的迭代器：
void func1() {
  std::vector<std::vector<int>> v;
  std::vector<std::vector<int>>::iterator i1 = v.begin();
  // auto可以根据表达式v.begin()的类型来推导出变量i2的类型：
  auto i2 = v.begin();
  if (i1 == i2) {
    std::cout << "same" << std::endl;
  }
  // same
}

// 2.当不知道变量是什么类型，或者不希望指明具体类型的时候，
//   比如泛型编程中，可以使用auto：
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
// 不使用auto，需要额外增加一个模板参数T2：
template <typename T1, typename T2>
void func(void) {
  T2 val = T1::get();
  std::cout << val << std::endl;
}
void func2() {
  // func使用了auto，只需要一个参数：
  func<A>();  // 100
  func<B>();  // http://c.biancheng.net/cplus/

  // func不使用auto，调用时需要手动传递参数类型：
  func<A, int>();           // 100
  func<B, const char *>();  // http://c.biancheng.net/cplus
}

void func() {
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
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}