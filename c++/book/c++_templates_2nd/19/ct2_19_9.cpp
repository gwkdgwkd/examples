#include <iostream>

// 19.9 策略萃取（Policy Traits）

// 到目前为止，例子中的萃取模板被用来判断模板参数的特性：
// 它们代表的是哪一种类型，作用于该类型数值的操作符的返回值的类型，
// 以及其它特性，这一类萃取被称为特性萃取。

// 最为对比，某些萃取定义的是该如何处理某些类型，称之为策略萃取。
// 策略萃取更倾向于是模板参数的某一独有特性，而策略类却通常和其它模板参数无关。

// 特性萃取通常都可以被实现为类型函数，策略萃取却通常将策略包装进成员函数中。

template <bool COND, typename TrueType, typename FalseType>
struct IfThenElseT {
  using Type = TrueType;
};
template <typename TrueType, typename FalseType>
struct IfThenElseT<false, TrueType, FalseType> {
  using Type = FalseType;
};
template <bool COND, typename TrueType, typename FalseType>
using IfThenElse = typename IfThenElseT<COND, TrueType, FalseType>::Type;

namespace n1 {
// 19.9.1 只读参数类型

// 在C++和C中，函数的调用参数默认情况下是按照值传递的。
// 这意味着，调用函数计算出来的参数的值，会被拷贝到由被调用函数控制的位置。
// 大部分程序员都知道，对于比较大的结构体，这一拷贝的成本会非常高，
// 因此对于这一类结构体最好能够将其按照常量引用或者是C中的常量指针进行传递。
// 对于小的结构体，到底该怎样实现目前还没有定论，从性能的角度来看，
// 最好的机制依赖于代码所运行的具体架构，在大多数情况下这并没有那么关键，
// 但是某些情况下，即使是对小的结构体也要仔细应对。

// 当然，有了模板之后事情要变得更加微妙一些：
// 事先并不知道用来替换模板参数的类型将会是多大。
// 而且，事情也并不是仅仅依赖于结构体的大小：
// 即使是比较小的结构体，其拷贝构造函数的成本也可能会很高，应该按常量引用传递。

namespace test1 {
// 这一类问题通常应当用策略萃取模板（一个类型函数）来处理：
// 该函数将预期的参数类型T映射到最佳的参数类型T或者是T const&。
// 作为第一步的近似，主模板会将大小不大于两个指针的类型按值进行传递，
// 对于其它所有类型都按照常量引用进行传递：
template <typename T>
struct RParam {
  using Type =
      typename IfThenElseT<sizeof(T) <= 2 * sizeof(void*), T, T const&>::Type;
};
// 另一方面，对于那些另sizeof运算符返回一个很小的值，
// 但是拷贝构造函数成本却很高的容器类型，可能需要分别对它们进行特化或者偏特化：
// template <typename T>
// struct RParam<Array<T>> {
//   using Type = Array<T> const&;
// };
}  // namespace test1

namespace test2 {
// 这一类类型很常见，如果只将那些拥有简单拷贝以及移动构造函数的类型按值进行传递，
// 当需要考虑性能因素时，再选择性的将其它一些class类型加入按值传递的行列。
// C++包含了std::is_trivially_copy_constructible，
// 还有std::is_trivially_move_constructible类型萃取。
template <typename T>
struct RParam {
  using Type = IfThenElse<(sizeof(T) <= 2 * sizeof(void*) &&
                           std::is_trivially_copy_constructible<T>::value &&
                           std::is_trivially_move_constructible<T>::value),
                          T, T const&>;
};

// 无论采用哪一种方式，现在该策略都可以被集成到萃取模板的定义中，
// 客户也可以用它们去实现更好的效果。
class MyClass1 {
 public:
  MyClass1() {}
  MyClass1(MyClass1 const&) {
    std::cout << "MyClass1 copy constructor called" << std::endl;
  }
};
class MyClass2 {
 public:
  MyClass2() {}
  MyClass2(MyClass2 const&) {
    std::cout << "MyClass2 copy constructor called" << std::endl;
  }
};
template <>
class RParam<MyClass2> {
 public:
  using Type = MyClass2;
};

// 现在，就可以定义将PParam<>用于只读参数的函数了，并对其进行调用：
template <typename T1, typename T2>
void foo(typename RParam<T1>::Type p1, typename RParam<T2>::Type p2) {}

void func1() {
  MyClass1 mc1;
  MyClass2 mc2;
  foo<MyClass1, MyClass2>(mc1, mc2);

  // MyClass2 copy constructor called
}

// 不幸的是，PParam的使用有一些很大的缺点：
// 1.函数的声明很凌乱；
// 2.可能也是更有异议的地方，就是在调用诸如foo()一类的函数时不能使用参数推断，
//   因为模板参数只出现在函数参数的限制符中，因此在调用时必须显式的指明所有的模板参数。

// 一个稍显笨拙的权宜之计是：
// 使用提供了完美转发的inline封装函数，但是需要假设编译器将省略inline函数：
template <typename T1, typename T2>
void foo_wrapper(T1&& p1, T2&& p2) {
  foo<T1, T2>(std::forward<T1>(p1), std::forward<T2>(p2));
}

void func2() {
  MyClass1 mc1;
  MyClass2 mc2;
  foo_wrapper(mc1, mc2);
}
}  // namespace test2

void func() {
  test2::func1();
  test2::func2();
}
}  // namespace n1

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}