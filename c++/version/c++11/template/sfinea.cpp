#include <iostream>

// SFINEA是Substitution failure is not an error，匹配失败不是错误。
// 主要用于模板函数，它是指编译器在使用具体类型来替换模板类型参数，
// 进行实例化时，如果发生替换失败，那么并不会直接引发编译错误，
// 而只是简单地把这个模板从重载候选者中去除掉。

namespace n1 {
struct A {
  typedef int foo;
};
template <typename T>
void func(typename T::foo) {
  std::cout << "#1" << std::endl;
};

template <typename T>
void func(T) {
  std::cout << "#2" << std::endl;
};

void func() {
  func<A>(10);    // #1
  func<int>(10);  // #2，尽管int::foo并不存在，也不会发生编译错误
}
}  // namespace n1

namespace n2 {
// 对于普通函数的重载而言，由于这些函数的所有信息都已经完备，
// 在发生调用之前，编译器已经可以完成对这些函数的编译，
// 这些函数也不可能再被增加任何新的信息，可以直接产生执行代码。
// 在函数的调用点上，编译器只需要根据参数信息选择合适函数的地址就可以了。
// 对于模板函数重载，情况就不一样了。
// 在真正发生调用（应该说真正需要被展开）之前，模板函数中的信息是不完备的，
// 编译器无法为模板函数生成真正的执行代码，而只是进行些基本、简单的检查。
// 所有的模板都不是真正的代码，它们是编译器用来生成代码的工具。
// 在需要展开的时候，编译器从合适的候选者中选出优先级最高的进行实例化。
// 在展开后如果不能正确被编译，只是简单地放弃这次展开，转而寻找其它模板。
// 如果编译器在展开失败后，直接产生编译错误的话，其它的函数就没有机会了，
// 这非常不合理，因为：
// 1.本次展开失败并不意味着被展开的模板代码就有问题，其它类型还有可能成功；
// 2.本次展开失败并不代表用于展开的类型无法找到合适的模板，其它模板可能可用。

// SFINEA的意义就是：
// 编译器在每个调用点上，只为当前需要实例化的类型寻找合适的模板进行展开，
// 而不会实例化而展开所有可能合适的重载模板（函数），这是智能选择模板的表现。
// 普通函数重载则不一样，无论是否被调用，或是无论调用需要的是什么类型的重载，
// 编译器会将所有参与了重载的函数一个不落的全部编译。
// 如果对模板也采用同样的方式，那么模板将受到巨大的局限而失去意义。

// 有了SFINEA，当写模板代码的时候，就不需要担心某些类型展开的时候会失败，
// 从而造成程序编译错误，因为编译器只会在能展开的情况展开它们，
// 展开失败的情况下，这些代码并不会真正进入程序中。

struct A {};
namespace test1 {
template <typename T>
bool is_class(int T::*) {
  return true;
}
template <typename T>
bool is_class(...) {
  return false;
}

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << is_class<A>(0) << std::endl;    // true
  std::cout << is_class<int>(0) << std::endl;  // false
}
}  // namespace test1

namespace test2 {
// SFINEA知名的一个例子：
template <typename T>
class is_class {
  typedef char one;
  typedef struct {
    char a[2];
  } two;

  template <typename C>
  static one test(int C::*);

  template <typename C>
  static two test(...);

 public:
  // 编译期的bool值，ture或是false是在运行期才得到的结果：
  enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

void func() {
  std::cout << is_class<A>::value << std::endl;    // 1
  std::cout << is_class<int>::value << std::endl;  // 0
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

namespace n3 {
// 使用这个规则来判断类是否存在某个成员函数：
template <typename T>
struct has_member_foo {  // 检查类型是否存在非静态成员foo函数
  // 定义两个重载函数Check，模板实例化会优先选择匹配程度最高的重载函数，
  // 检查类型是否存在foo函数，如果存在则返回std::true_type，
  // 否则返回std::false_type，巧妙的利用了C++的SFINAE特性。
 private:
  // 最关键的一句代码：
  // 配合decltype来获取函数foo的返回类型，如果获取成功则表明存在foo，
  // 否则就会替换失败，而选择默认的Check函数（不会报错,SFINAE）。
  // decltype另外一个妙用，它可以通过逗号表达式连续推断多个函数的返回类型。
  template <typename U>
  static auto Check(int) -> decltype(std::declval<U>().foo(), std::true_type());
  template <typename U>
  static std::false_type Check(...);

 public:
  enum { value = std::is_same<decltype(Check<T>(0)), std::true_type>::value };
};

struct A {
  void foo() { std::cout << "hello" << std::endl; }
};
struct B {
  void test() { std::cout << "test" << std::endl; }
};

void func() {
  if (has_member_foo<A>::value) {
    std::cout << "A has foo funciton" << std::endl;
  } else {
    std::cout << "A does't have foo funciton" << std::endl;
  }

  if (has_member_foo<B>::value) {
    std::cout << "B has foo function" << std::endl;
  } else {
    std::cout << "B does't have foo function" << std::endl;
  }

  // A has foo funciton
  // B does't have foo function
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
}