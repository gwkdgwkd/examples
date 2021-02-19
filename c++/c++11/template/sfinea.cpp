#include <iostream>

using namespace std;

// SFINAE(substitution failure is not a error) 主要用于模板函数，它是指编译器在使用具体类型来替换模板类型参数，对模板进行实例化（展开模板）时，
// 如果发生替换失败，那么并不会直接引发编译错误（Error），而只是简单地把这个模板从重载候选者中去除掉。
struct Test {
  typedef int foo;
};

template <typename T>
void f(typename T::foo) {
  cout << "#1" << endl;
};  // 第一个模板定义

template <typename T>
void f(T) {
  cout << "#2" << endl;
};  // 第二个模板定义

// 对于普通函数的重载而言，由于这些函数的所有信息都已经完备，在发生调用之前，编译器已经可以完成对这些函数的编译，这些函数也不可能再被增加任何新的信息，
// 可以直接产生执行代码。在函数的调用点上，编译器只需要根据参数信息选择一个合适函数的地址就可以了。
int max(int a, int b) { return a > b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }

// 对于模板函数重载，情况就不一样了。在真正发生调用（应该说真正需要被展开）之前，模板函数中的信息是不完备的，编译器无法为这些模板函数生成真正的执行代码，
// 而只是进行一些很基本、简单的检查。所有的模板都不是“真正的代码”，它们是编译器用来生成代码的工具。在需要展开的时候，编译器从合适的候选者中选出优先级最高
// 的一个来进行实例化（展开）。在展开后的代码如果不能正确被编译（像上面例子中 int::* 这种情况），编译器只是简单地放弃这次展开，转而寻找其它的模板。试想，
// 如果编译器在展开失败后，直接产生一个编译错误的话，其它的函数就没有机会了，这是非常不合理的，因为：
//  1 本次展开失败并不意味着被展开的模板代码就有问题，因为用其它类型的话还是有可能展开成功的。
//  2 本次展开失败并不代表用于展开的类型无法找到合适的模板，其它模板可能合用。
// SFINEA的意义就是：编译器在每个调用点上，只为当前需要实例化的类型寻找一个合适的模板进行展开，而不会为某一次实例化而展开所有可能合适的重载模板（函数）。
// 这是编译器“智能”选择模板的表现。普通函数重载则不一样，无论是否被调用，或是无论调用点需要的是什么类型的重载，编译器会将所有参与了重载的函数一个不落的全
// 部编译。如果对模板也采用同样的方式，那么模板将受到巨大的局限而失去意义。

// 有了SFINEA ，当我们在写模板代码的时候，就不需要担心这些模板在使用某些类型进行展开的时候会失败，从而造成程序编译错误，因为我们知道编译器只会在能展开的
// 情况展开它们，展开失败的情况下，这些代码并不会真正进入你的程序中。

// SFINEA“知名”的一个例子：
template <typename T>
class is_class {
  typedef char one;
  typedef struct { char a[2]; } two;

  template <typename C>
  static one test(int C::*);

  template <typename C>
  static two test(...);

 public:
  enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

// ==========

// 在C++模板中，SFINEA规则是指”Substitution failure is not an error“(匹配失败不是错误)。
// 具体来说，就是当重载的模板参数展开时，如果展开导致一些类型不匹配，编译器并不报错。

// 使用这个规则来判断类是否存在某个成员函数:
template <typename T>
struct has_member_foo {
  // 定义两个重载函数Check，由于模板实例化的过程中会优先选择匹配程度最高的重载函数，在模板实例化的过程中检查类型是否存在foo函数，
  // 如果存在则返回std::true_type，否则返回std::false_type.巧妙的利用了C++的SFINAE特性.
 private:
  // 最关键的一句代码: 配合decltype来获取函数foo的返回类型，如果获取成功则表明存在foo函数，否则就会替换失败，而选择默认的Check函数(不会报错,SFINAE)。
  // decltype在这里还有另外一个妙用，它可以通过逗号表达式连续推断多个函数的返回类型.
  template <typename U>
  static auto Check(int) -> decltype(std::declval<U>().foo(), std::true_type());
  template <typename U>
  static std::false_type Check(...);

 public:
  enum { value = std::is_same<decltype(Check<T>(0)), std::true_type>::value };
};  // 这个has_member_foo的作用就是检查类型是否存在非静态成员foo函数

struct myStruct {
  void foo() { std::cout << "hello" << std::endl; }
};

struct another {
  void test() { std::cout << "test" << std::endl; }
};

int main() {
  f<Test>(10);  // 调用#1
  f<int>(10);   // 调用#2但因为SFINEA，尽管int::foo并不存在，也不会发生编译错误。

  if (has_member_foo<myStruct>::value)
    std::cout << "myStruct has foo funciton" << std::endl;
  else
    std::cout << "myStruct does't have foo funciton" << std::endl;

  if (has_member_foo<another>::value)
    std::cout << "another has foo function" << std::endl;
  else
    std::cout << "another does't have foo function" << std::endl;

  // myStruct has foo funciton
  // another does't have foo function

  return 0;
}