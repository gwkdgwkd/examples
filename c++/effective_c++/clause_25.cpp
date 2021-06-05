#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// 考虑写出一个不抛出异常的swap函数

// swap是个有趣的函数。原本只是STL的一部分，而后成为异常安全性编程的脊柱，以及用来处理自我赋值可能性的一个常见机制。
// 标准库的实现：
// namespace std {
// template <typename T>
// void swap(T& a, T& b) {
//   T temp(a);
//   a = b;
//   b = temp;
// }
// }  // namespace std
// 只要类型T支持copying操作，缺省的swap就可以帮助交换，不需要额外其他工作。

// 对于某些类型（指针指向一个对象，内含真正数据那种类型），默认的swap效率低，三个对象的复制无一是必要的。
class WidgetImpl {
 private:
  int a, b, c;            // 可能有许多数据
  std::vector<double> v;  // 意味着复制时间很长
};
class Widget {
 public:
  Widget(const Widget& rhs) {}
  Widget& operator=(const Widget& rhs) {
    *pImpl = *(rhs.pImpl);
    return *this;
  }
  void swap(Widget& other) {
    using std::swap;
    swap(pImpl, other.pImpl);
  }

 private:
  WidgetImpl* pImpl;
};
// 要置换Widget对象，其实就是要置换pImpl指针，但缺省的swap算法不知道，它不止复制三个Widget，还复制三个WidgetImpl，非常缺乏效率。

// 将std::swap针对Widget特化：
namespace std {  // 通常我们不被允许改变std命名空间内的任何东西，但允许为标准template制造特化版本，使它属于我们自己的class。
template <>      // 表示它是std::swap的一个全特化版本
void swap<Widget>(Widget& a, Widget& b) {
  // 函数名后的<Widget>表示这一特化版本针对”T是Widget“而设计。换句话说，当类型是Widget时，就会使用这个版本。
  a.swap(b);
}
}  // namespace std
// 上面的做法不只能通过编译，还与STL容器有一致性，因为所有STL容器也都提供public swap成员函数和std::swap特化版本。

// 假设WidgetImpl和Widget都是class template而非class
template <typename T>
class WidgetImpl1 {
 private:
  T a, b, c;
  std::vector<T> v;
};
template <typename T>
class Widget1 {
 public:
  Widget1(const Widget1& rhs) {}
  Widget1& operator=(const Widget1& rhs) {
    *pImpl = *(rhs.pImpl);
    return *this;
  }
  void swap(Widget1& other) {
    using std::swap;
    swap(pImpl, other.pImpl);
  }

 private:
  WidgetImpl1<T>* pImpl;
};
// 特化std::swap时遇到问题：
// 企图偏特化一个function template，但C++只允许对class template偏特化，在function template上不能偏特化。
// namespace std {
// template <typename T>
// void swap<Widget1<T>>(Widget1<T>& a, Widget1<T>& b) {  // 错误！不合法
//   a.swap(b);
// }
// }  // namespace std

// 当打算偏特化一个function template时，惯常的做法是简单地为它添加一个重载版本
namespace std {
template <typename T>  // std::swap的一个重载版本
void swap(Widget1<T>& a, Widget1<T>& b) {
  a.swap(b);
}
}  // namespace std
// 一般而言，重载function template没有问题，但std是个特殊的命名空间，其管理规则也比较特殊。
// 客户可以全特化std内的template，但不可以添加新的template（或class或function或其他任何东西）到std里头。
// 虽然是禁止的，但是程序可以编译和执行，但它们的行为没有明确定义。建议不要添加任何新东西到std里头。

// 那该怎么办？总是需要一个办法让其他人调用swap时能够取得我们提供的较高效的template特定版本。
// 声明一个non-member swap，让它调用member swap，但不再将那个non-member swap声明为std::的特化版本或重载版本。
namespace WidgetStuff {
template <typename T>
class WidgetImpl1 {
 private:
  T a, b, c;
  std::vector<T> v;
};
template <typename T>
class Widget1 {
 public:
  Widget1(const Widget1& rhs) {}
  Widget1& operator=(const Widget1& rhs) {
    *pImpl = *(rhs.pImpl);
    return *this;
  }
  void swap(Widget1& other) {
    using std::swap;
    swap(pImpl, other.pImpl);
  }

 private:
  WidgetImpl1<T>* pImpl;
};
template <typename T>                      // non-member swap函数
void swap(Widget1<T>& a, Widget1<T>& b) {  // 不属于std命名空间
  a.swap(b);
}
}  // namespace WidgetStuff
// 任何地点任何代码如果打算交换Widget1对象，因而调用swap，C++根据名称查找规则，会找到WidgetStuff内的Widget1专属版本，就是我们想要的。

// 假设客户正在写一个function template，需要用到swap。
// 应该调用那个swap？std一般版本？还是某个可能存在的特化版本？抑或是一个可能存在的T专属版本而且可能栖身于某个命名空间内？
// 希望调用T的专属版本，并在该版本不存在的情况下调用std内的一般化版本。
template <typename T>
void doSomething(T& obj1, T& obj2) {
  using std::swap;   // 令std::swap在此函数内可用
  swap(obj1, obj2);  // 为T型对象调用最佳swap版本
  // std::swap(obj1, obj2);  // 错误的swap调用方式
  // 强迫编译器只认std内的swap（包括任何template特化），不能再调用一个定义于它处的比较适当的T专属版本。
  // 这正是”你的class对std::swap进行全特化“的重要原因。使得类型专属的swap实现版本也让可以被上面的“迷途代码”所用。
}
// 一旦编译器看到对swap的调用，便查找适当的swap并调用。
// C++的查找法则确保将找到global作用域或T所在命名空间内的任何T专属的swap。
// 如果T是Widget1，编译器会使用”实参数取决之查找规则“找出WidgetStuff内的swap。如果没有T专属的swap存在，编译器就使用std内的swap，
// 这要感谢using声明让std::swap在函数内可用。
// 尽管如此，编译器还是比较喜欢std::swap的T专属特化版本，而非一般化的那个template，所以如果已经针对T将std::swap特化，特化版本会被编译器选中。

// 总结：
// 涉及了default swap、member swap、non-member swap、std::swap特化版本、以及对swap的调用。
// 首先，如果swap的缺省实现对class或class template提供可接受的效率，那么不需要做任何事。
// 其次，如果swap的缺省版本效率不足（意味着class或class template使用了某种pimpl手法），试着做以下事情：
//  1 提供public swap成员函数，让它高效地交换，这个函数绝不该抛出异常。
//  2 在class或template所在的命名空间内提供一个non-member swap，并令它调用上述swap成员函数。
//  3 如果在编写一个class（非class template），为你的class特化std::swap，并令它调用你的swap成员函数。
// 最后，如果你调用swap，确定包含一个using声明，以便让std::swap在你的函数内可见，染红不加任何namespace修饰符，赤裸裸地调用swap。

// 成员版swap绝不可抛出异常。因为swap的一个最好的应用是帮助class（和class template）提供强烈的异常安全性保障。
// 上面的约束只针对成员版，不针对非成员版，因为swap缺省版本是以copy构造函数和copy assignment操作符为基础，而一般情况下两者都允许抛出异常。
// 自定义版本的swap，往往提供的不只是高效交换对象值的办法，而且不抛出异常。
// 上面的两个特性是连在一起的，因为高效的swap几乎总是基于对内置类型的操作，而内置类型上的操作绝不会抛出异常。

// 请记住：
// 当std::swap对你的类型效率不高时，提供一个swap成员函数，并确定这个函数不抛出异常。
// 如果提供一个member swap，也该提供一个non-member swap用来调用前者。对于class（非template），也请特化std::swap。
// 调用swap时应该针对std::swap使用using声明式，然后调用swap并且不带任何“命名空间资格修饰”。
// 为“用户定义类型”进行std template全特化是好的，但千万不要尝试在std内加入某些对std而言全新的东西。

int main() { return 0; }