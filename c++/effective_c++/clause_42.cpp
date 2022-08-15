#include <iostream>
#include <iterator>

using namespace std;

// 了解typename的双重意义

template <class T>
class Widget;
template <typename T>
class Widget;
// 从C++的角度来看，在template声明式中，class和typename没有不同。

// 然而C++并不总把class和typename视为等价。
// 有时一定得使用typename。
// template <typename C>
// void print2nd(const C& container) {
//   if (container.size() >= 2) {
//     C::const_iterator iter(container.begin()); // 默认不当做类型
//     ++iter;
//     int value = *iter;
//     cout << value;
//   }
// }
// template内出现的名称如果依赖某个template参数，称之为从属名称。
// iter如果从属名称在class内呈嵌套状，称它为嵌套从属名称。
// C::const_iterator不依赖任何template参数的名称。
// 这是所谓的非从属名称。
// value嵌套从属名称有可能导致解析困难。
// template <typename C>
// void print2nd(const C& container) {
//   C::const_iterator* x;
//   把C::const_iterator当做类型，那么是一个指针。
//   如果C恰好有个static成员变量被命名为const_iterator，
//   或如果x碰巧是个global变量名称呢？
//   那么上述代码就不再是声明一个local变量，而是一个相乘动作。
//   虽然听起来很疯狂，但却是可能的。
//   在知道C是什么之前，没有任何办法知道C::const_iterator是否为一个类型。
//   当编译器开始解析template print2nd时，尚未知C是什么东西。
//   C++有个规则：如果解析器在template中遇到一个嵌套从属名称，
//   便假设这名称不是个类型，除非告诉它是。
//   所以缺省情况下，嵌套从属名称不是类型。
// }

template <typename C>
void print2nd(const C& container) {
  if (container.size() >= 2) {
    // 解决嵌套从属类型默认不是类型的办法，
    // 紧临它之前加上关键字typename即可：
    typename C::const_iterator iter(container.begin());
    // 没有typename编译不过
    ++iter;
    int value = *iter;
    cout << value;
  }
}

// 任何时候当想要在template中涉及一个嵌套从属类型名称，
// 就必须在紧临它的前一个位置放上关键字typename。
// typename只被用来验明嵌套从属类型名称；其他名称不该有它存在。
template <typename C>  // 允许使用typename或class
void f(const C& container,  // 不允许使用typename，并不是嵌套从属类型名称
       typename C::iterator iter);  // 一定要使用typename

// typename必须作为嵌套从属类型名称的前缀词，这一规则的例外是：
// type不可以出现在base class list内的嵌套从属名称之前;
// 也不可以在member initialization list中作为base class修饰符。
// template <typename T>
// class Derived :
//   public Base<T>::Nested {  // base class list不允许typename
//  public:
//   // member initialization list不允许typename：
//   explicit Derived(int x) : Base<T>::Nested(x) {
//     typename Base<T>::Nested temp;  // 嵌套从属类型名称
//   }
// };

template <typename IterT>
void workWithIteraror(IterT iter) {
  // 好长，需要typedef：
  typename iterator_traits<IterT>::value_type temp(*iter);

  // typedef typename看着不和谐，但很快就会习惯的：
  typedef typename iterator_traits<IterT>::value_type vtype;
  vtype temp1(*iter);
}

// typename相关规则在不同的编译器上有不同的实践。
// 意味着typename和嵌套从属类型名称之间的互动，
// 也许会在移植方面带来一些麻烦。

// 请记住：
// 声明template参数时，前缀关键字class和typename可互换。
// 请使用关键字typename标识嵌套从属类型名称；
// 但不得在基类列表或成员初值列表内使用typename。

int main() { return 0; }