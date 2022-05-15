#include <algorithm>
#include <iostream>
#include <set>

// 确保less<T>与operator<具有相同的语义

class Widget {
 public:
  size_t weight() const {}
  size_t maxSpeed() const {}
  friend bool operator<(const Widget &, const Widget &);
};
bool operator<(const Widget &lhs, const Widget &rhs) {
  return lhs.weight() < rhs.weight();
}

// multiset<Widget>的默认比较函数是less<Widget>，
// 而less<Widget>在默认情况下会调用operator<来完成它的工作。
// 创建一个按照最大速度进行排序的multiset<Widget>容器，一种显而易见的方式是：
// 特化less<Widget>，切断less<Widget>和operator<之间的关系：
// template <>
// struct std::less<Widget> : public std::binary_function<Widget, Widget, bool> {
//   bool operator()(const Widget &lhs, const Widget &rhs) const {
//     return lhs.maxSpeed() < rhs.maxSpeed();
//   }
// };
// 作为一般性的规则，对std命名空间的组件进行修改确实是被禁止的（会导致未定义行为）。
// 但在某些特定情况下，有效对std名字空间的修补工作仍然是允许的，
// 特别是，针对用户定义的类型，特化std中的模板。
// 大部分情况下应该有比特化std模板更好的选择，但是在偶尔的情形下，这样做也是合理的。
// 例如，智能指针的作者希望它们的类能够像C++内置指针一样进行排序，
// 所以智能指针指针类的std::less特化版本并不少见。

// operator<不仅仅是less的默认实现方式，也是程序员期望less所做的事情。
// 让less不调用operator<而取做别的事，这会无端地违背程序员的意愿，
// 这与“少带给人惊奇”的原则完全背道而驰。
// 这很不好，应该尽量避免这样做。
// 也没有理由这样做。在STL中，凡是使用了less的地方都可以指定另外一个不同的比较类型。
struct MaxSpeedCompare : public std::binary_function<Widget, Widget, bool> {
  bool operator()(const Widget &lhs, const Widget &rhs) const {
    return lhs.maxSpeed() < rhs.maxSpeed();
  }
};

// 尽量避免修改less的行为，可能会误导其他程序员。
// 如果使用了less，无论是显式地或是隐式地，都需要确保它与operator<具有相同的意义。
// 如果需要以一种特殊的方式来排序对象，那么最好创建一个特殊的函数子类，它的名字不能是less，这样做其实很简单。

int main() {
  std::multiset<Widget> widgets1;

  // 避免使用默认的比较类型less<Widget>：
  std::multiset<Widget, MaxSpeedCompare> widgets2;

  return 0;
}
