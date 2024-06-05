#include <iostream>

// 条款33：对auto&&类型的形参使用decltype，再经过std::forward转发

void fun(int& t) { std::cout << " lvalue" << std::endl; }
void fun(const int& t) { std::cout << " rvalue" << std::endl; }
int i = 5;

namespace n1 {
// 泛型lambda式是C++14最振奋人心的特性之一，lambda可以在形参规格中使用auto。
// 这个特性的实现十分直截了当：闭包类中的operator()采用模板实现。

void func() {
  auto f = [](auto x) { return fun(x); };
  // 则闭包类的函数调用运算符如下所示：
  // class SomeCompilerGeneratedClassName {
  //  public:
  //   template <typename T>
  //   auto operator()(T x) const { // auto返回值见条款3
  //     return func(fun(x));
  //   }
  // };

  // lambda式对x实施的唯一动作就是将其转发给fun，如果fun区别对待左值和右值，
  // 则可说该lambda式撰写的是有问题的，因为，lambda总会传递左值形参x给fun，
  // 即使传递给lambda式的实参是个右值：
  f(i);             // lvalue
  f(std::move(i));  // lvalue
}
}  // namespace n1

namespace n2 {
// n1中的lambda的正确撰写方式是把x完美转发给fun，这就要求在代码中修改两处：
// 1.x要改成万能引用；
// 2.使用std::forward把x转发给fun。

// 概念上不难理解，这两处的修改都是举手之劳：
// auto f = [](auto&& x) { return func(normalize(std::forward<???> (x)); };
// 通常情况下，在使用完美转发时，是在一个接受类型形参T的模板函数中，
// 所以写std::forward<T>就好，而在泛型lambda式中，却没有可用的类型形参T。
// 在lambda式生成的闭包内的模板化operator()函数中的确有个T，
// 但是在Lambda中无法使用它，所以有也没用。

// 条款28解释过，如果把左值传递给万能引用形参，则该形参的类型会成为左值引用；
// 而如果传递的是右值，则该形参会成为右值引用。
// 那意味着在lambda中，可以通过探查x的类型，来判断传入的实参是左值还是右值。
// decltype提供了实现这一点的一种途径：
// 1.如果传入的是个左值，decltype(x)将会产生左值引用类型；
// 2.如果传入的是个右值，decltype(x)将会产生右值引用类型。

// 条款28还解释了，使用std::forward是惯例是：
// 1.用类型形参为左值引用表明想要返回左值，
// 2.而用非引用类型时来表明想要返回的右值。
// 再看lambda，如果x绑定了左值，decltype(x)将产生左值的引用类型，这符合惯例。
// 不过，如果x绑定的是个右值，decltype(x)将会产生右值引用，不符合惯例的非引用。

// 条款28中的std::forward的C++14实现：
// template <typename T>
// T&& forward(remove_reference_t<T>& param) {
//   return static_cast<T&&>(param);
// }

// 如果客户代码欲完美转发Widget类型的左值，按惯例它应该采用Widget类型，
// 即非引用类型，来实例化std::forward，然后std::forward模板会产生如下函数：
// Widget&& forward(Widget& param) { return static_cast<Widget&&>(param); }

// 如果用户代码想要完美转发Widget的右值，但是这次没有遵从惯例将T指定为非引用类型，
// 而是将T指定为右值引用，这会导致什么结果？
// 这就是需要思考的问题，T指定为Widget&&将会发生什么事情。
// 在std::forward完成初步实例化并实施了std::remove_reference_t之后，
// 但在引用折叠发生之前，std::forward如下所示：
// Widget&& && forward(Widget& param) {
//     return static_cast<Widget&& &&>(param);
// }

// 然后，应用引用折叠规则，右值引用的右值引用结果是单个右值引用，实例化结果为：
// Widget&& forward(Widget& param) { return static_cast<Widget&&>(param); }
// 如果把这个实例化和在T为Widget时调用的forward那个实例化相比较，会发现它们别无二致。
// 那就意味着，实例化forward时，使用右值引用类型和使用非引用类型，会产生相同效果。

void func() {
  // 如果传递给lambda的形参x是个右值，decltype(x)产生的是右值引用类型。
  // 传递左值给的lambda时，decltype(x)会产生传递给forward的符合惯例的类型，
  // 而现在又知道对于右值而言，虽然说decltype(x)产生的类型，
  // 并不符合传给forward的类型形参的惯例，但是产生的结果与符合惯例的类型殊途同归。
  // 所以无论左值还是右值，把decltype(x)传递给std::forward都能给出想要的结果。
  // 是故，完美转发lambda式可以编写如下：
  auto f = [](auto&& param) {
    return fun(std::forward<decltype(param)>(param));
  };

  f(i);             // lvalue
  f(std::move(i));  // rvalue
}
}  // namespace n2

namespace n3 {
void func() {
  // 在此基础上稍加改动，就可以得到可以接受多个形参的完美转发lambda式版本，
  // 因为C++14中的lambda能够接受可变长形参：
  auto f = [](auto&&... param) {
    return fun(std::forward<decltype(param)>(param)...);
  };

  f(i);             // lvalue
  f(std::move(i));  // rvalue
}
}  // namespace n3

// 要记住的东西：
// 1.对auto&&类型的形参使用decltype，再经过std::forward转发。

int main(int argc, char* argv[]) {
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