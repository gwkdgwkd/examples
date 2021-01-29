#include <iostream>
#include <vector>

// 在C++11之前的版本（C++98和C++03）中，定义变量或者声明变量之前都必须指明它的类型，比如int、char等；但是在一些比较灵活的语言中，比如C#、JavaScript、PHP、Python等，
// 程序员在定义变量时可以不指明具体的类型，而是让编译器（或者解释器）自己去推导，这就让代码的编写更加方便。
// C++11为了顺应这种趋势也开始支持自动类型推导了！C++11使用auto关键字来支持自动类型推导。

// 在之前的C++版本中，auto关键字用来指明变量的存储类型，它和static关键字是相对的。auto表示变量是自动存储的，这也是编译器的默认规则，所以写不写都一样，一般我们也不写，这使得
// auto关键字的存在变得非常鸡肋。
// C++11赋予auto关键字新的含义，使用它来做自动类型推导。也就是说，使用了auto关键字以后，编译器会在编译期间自动推导出变量的类型，这样我们就不用手动指明变量的数据类型了。

template <typename T> class A {
  // TODO:
};

class A1 {
public:
  static int get(void) { return 100; }
};
class B1 {
public:
  static const char *get(void) { return "http://c.biancheng.net/cplus/"; }
};
template <typename T> void func(void) {
  auto val = T::get();
  std::cout << val << std::endl;
}

class A2 {
public:
  static int get(void) { return 100; }
};
class B2 {
public:
  static const char *get(void) { return "http://c.biancheng.net/cplus/"; }
};
template <typename T1, typename T2> //额外增加一个模板参数 T2
void func(void) {
  T2 val = T1::get();
  std::cout << val << std::endl;
}

int main() {
  // auto 关键字基本的使用语法如下(name是变量的名字，value是变量的初始值)：
  // auto name = value;
  // 注意：auto仅仅是一个占位符，在编译器期间它会被真正的类型所替代。或者说，C++中的变量必须是有明确类型的，只是这个类型是由编译器自己推导出来的。
  auto n = 10;
  auto f = 12.8;
  auto p = &n;
  auto url = "http://c.biancheng.net/cplus/";

  // 也可以连续定义多个变量:
  int b = 20;
  auto *q = &b, m = 99;
  // 编译器根据第一个子表达式已经推导出auto为int，那后面的m也只能是int，如果写作m=12.5是错误的，因为12.5是double，和int是冲突的。
  // auto *q = &b, m = 12.5; // 推导的时候不能有二义性。
  // error: inconsistent deduction for ‘auto’: ‘int’ and then ‘double’

  // auto除了可以独立使用，还可以和某些具体类型混合使用，这样auto表示的就是“半个”类型，而不是完整的类型:
  int x = 0;
  auto *p1 = &x; // p1为int*，auto推导为int
  auto p2 = &x;  // p2为int*，auto推导为int*
  auto &r1 = x;  // r1为int&，auto推导为int
  // r1本来是int&类型，当=右边的表达式是一个引用类型时，auto会把引用抛弃，直接推导出它的原始类型。
  auto r2 = r1; // r2为int，auto推导为int。

  // auto和const的结合:
  int x1 = 0;
  const auto n1 = x1; // n1为const int，auto被推导为int
  // 当=右边的表达式带有const属性时，auto不会使用const属性，而是直接推导出non-const类型。
  auto f1 = n1;        // n1为const int，auto被推导为int
  const auto &r3 = x1; // r3为const int&类型，auto被推导为int
  // 当const和引用结合时，auto的推导将保留表达式的const类型。
  auto &r4 = r3; // r4为const int&类型，auto被推导为const int类型，

  // 总结一下 auto 与 const 结合的用法：
  // 当类型不为引用时，auto的推导结果将不保留表达式的const属性；
  // 当类型为引用时，auto的推导结果将保留表达式的const属性。

  // auto的限制:
  // 1.使用auto类型推导的变量必须马上初始化，这个很容易理解，因为auto在C++11中只是“占位符”，并非如int一样的真正的类型声明。
  // 2.auto不能在函数的参数中使用。
  // 在定义函数的时候只是对参数进行了声明，指明了参数的类型，但并没有给它赋值，只有在实际调用函数的时候才会给参数赋值；而auto要求必须对变量进行初始化，所以这是矛盾的。
  // 3.auto关键字不能定义数组。
  // auto str[] = url; // ‘str’ declared as array of ‘auto’
  // 4.auto 不能作用于模板参数。
  A<int> C1;
  // A<auto> C2 = C1; // invalid use of ‘auto’

  // auto的应用:
  // 1.auto的一个典型应用场景是用来定义stl的迭代器。
  std::vector<std::vector<int>> vv;
  std::vector<std::vector<int>>::iterator ii = vv.begin();
  // 使用auto代替具体的类型，auto可以根据表达式v.begin()的类型（begin()函数的返回值类型）来推导出变量iii的类型。
  auto iii = vv.begin();
  if (iii == ii) {
    std::cout << "same" << std::endl;
  }

  // 2.auto的另一个应用就是当我们不知道变量是什么类型，或者不希望指明具体类型的时候，比如泛型编程中。
  func<A1>(); // 100
  func<B1>(); // http://c.biancheng.net/cplus/

  // 不使用auto，调用时也要手动给模板参数赋值
  func<A2, int>();          // 100
  func<B2, const char *>(); // http://c.biancheng.net/cplus

  return 0;
}