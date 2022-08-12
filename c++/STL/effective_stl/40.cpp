#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>

// 若一个类是函数子，则应使它可配接

struct Widget {};
bool isInteresting(const Widget* pw) {}

// STL的4个标准的函数配接器：
// not1、not2、bind1st和bind2nd，都要求一些特殊的类型定义，
// 那些非标准的、与STL兼容的配接器通常也是如此。
// 提供了这些必要的类型定义的函数对象被称为可配接的函数对象，
// 反之，如果函数对象缺少这些类型定义，则称为不可配接的。
// 可配接的函数对象能够与其他STL组件更为默契地协同工作，
// 它们能够应用于更多的上下文环境中，因此应当尽可能地使你编写的函数对象可以配接。
// 这些类型是：
// argument_type、first_argument_type、second_argument_type已经result_type。
// 不同种类的函数子类所提供的定义也不尽相同，是上面类型的不同子集。
// 提供这些类型定义最简单的办法是让函数子从特定的基类继承：
// 1.如果函数子类的operator()只有一个实参，
//   那么它应该从std::unary_function继承；
// 2.如果函数子类的operator()有两个实参，
//   那么它应该从std::binary_function继承。
// std::unary_function和std::binary_function是模板，
// 不能直接继承，必须继承它们所产生的结构，就是要指定某些类型实参。
// 对于std::unary_function必须指定函数子类operator()的参数的类型和返回类型。
// 而对于std::binary_function必须指定三个类型，
// operator()的第一个和第二个参数的类型，以及operator()的返回类型。
// std::unary_function和std::binary_function operator()的返回类型，
// 都是最后一个参数。
template <typename T>
class MeetsThreshold : public std::unary_function<Widget, bool> {
 private:
  // 使用class是因为包含了状态信息，数据成员：
  const T threshold;

 public:
  MeetsThreshold(const T& t) : threshold(t) {}
  bool operator()(const Widget&) const {}
};

// 虽然operator()的参数类型都是const Widget&，
// 但我们传递给binary_function的类型却是Widget。
// 对于std::unary_function和std::binary_function，
// 一般情况下传递给的非指针类型需要去掉const和引用(&)部分：
struct WidgetNameCompare : public std::binary_function<Widget, Widget, bool> {
  // 不包含状态信息，因而不需要任何私有成员。
  // 如果一个函数子的所有成员都是公有的，那么通常会将其声明为结构而不是类。
  // STL中所有无状态的函数子类一般都被定义成结构。
  bool operator()(const Widget& lhs, const Widget& rhs) const {}
};

// 如果operator()带有指针参数，规则又不同了：
struct PtrWidgetNameCompare
    : public std::binary_function<const Widget*, const Widget*, bool> {
  bool operator()(const Widget* lhs, const Widget* rhs) const {}
};
// 对于以指针作为参数或返回类型的函数子类，一般的规则是，
// 传给std::unary_function和std::binary_function的类型，
// 与operator()的参数和返回类型完全相同。

// STL函数对象是C++函数的一种抽象和建摸形式，
// 而每个C++函数只有一组确定的参数类型和一个返回类型。
// 所以STL总是假设每个函数子类只有一个operator()成员函数，
// 并且其参数和返回类型，
// 应该吻合std::unary_function和std::binary_function的模板参数。
// 这也就意味着，虽然只要创建一个包含两个operator函数的结构，
// 就可以把WidgetNameCompare和PtrWidgetNameCompare的功能，
// 合并到一个函数子结构中，但最好不要这样做。
// 如果这样做了，那么这样的函数子至多只有一种调用形式是可配接的。
// 一个只有一半配接能力的函数子恐怕并不比完全不可配接的函数子强多少。
// 但有时确实需要函数子类具有多种不同的调用形式，
// 也就意味这放弃了其可配接能力。

int main() {
  std::list<Widget*> widgetPtrs;

  // 找到第一个满足条件的：
  std::list<Widget*>::iterator i =
      find_if(widgetPtrs.begin(), widgetPtrs.end(), isInteresting);
  if (i != widgetPtrs.end()) {
  }

  // 找到第一个不满足条件的：
  // find_if(widgetPtrs.begin(), widgetPtrs.end(),
  //         not1(isInteresting));  // 编译失败
  // isInteresting作为一个基本的函数指针，它缺少not1所需要的类型定义。

  // 正确的做法是：
  find_if(widgetPtrs.begin(), widgetPtrs.end(),
          std::not1(std::ptr_fun(isInteresting)));
  // ptr_fun只是完成了一些类型定义而已，这些类型定义是not1所必须的。

  std::list<Widget> widgets;
  Widget a;
  // 找到最后一个不符合的：
  std::list<Widget>::reverse_iterator i1 =
      find_if(widgets.rbegin(), widgets.rend(), not1(MeetsThreshold<int>(10)));
  // 找到按WidgetNameCompare定义的规则排序时，在a之前的第一个Widget对象：
  std::list<Widget>::reverse_iterator i2 = find_if(
      widgets.rbegin(), widgets.rend(), bind2nd(WidgetNameCompare(), a));
  // 如果MeetsThreshold不是从unary_function继承而来的，
  // WidgetNameCompare不是从binary_function继承而来的，
  // 上面的例子就无法通过编译，因为not1和bind2nd只能用于可配接的函数对象。

  return 0;
}