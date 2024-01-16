#include <iostream>
#include <utility>

// 19.5 IsConvertibleT

// 基于SIFINAE萃取的常规方法在实际中会变得更加复杂，为了展示这一复杂性，
// 将定义一个能够判断一种类型是否可以被转化成另外一种类型的萃取，
// 比如当期望某个基类或者其某一个子类作为参数的时候。
// IsConvertibleT就可以判断其第一个类型参数是否可以被转换成第二个类型参数。

namespace n1 {
// 这里使用了在第19.4.1节介绍的函数重载的方法。
// 也就是说，在一个辅助类中定义了两个名为test()的返回值类型不同的重载函数，
// 并为该辅助类声明了Type成员类型：
template <typename FROM, typename TO>
struct IsConvertibleHelper {
 private:
  static void aux(TO);

  // 和往常一样，第一个test()只有在所需的检查成功的时候才会被匹配到，
  // 第二个test()则是应急方案。
  // 因此问题的关键就是让第一个test()只有在类型FROM 可以被转换成TO的时候才有效。
  // 为了实现这一目的，再次给第一个test()分配了一个dummy（并且无名）的模板参数，
  // 并将其初始化成只有当转换又消失才有效的内容。
  // 该模板参数不可以被推断，也不会为之提供显式的模板参数。
  // 因此它会被替换，而且当替换失败之后，该test()声明会被丢弃掉。
  template <typename F, typename T, typename = decltype(aux(std::declval<F>()))>
  static std::true_type test(void*);

  template <typename, typename>
  static std::false_type test(...);

  // 这种声明是不可以的：
  // template<typename = decltype(aux(std::declval<FROM>()))>
  // static char test(void*);
  // 这样当成员函数模板被解析的时候，FROM和TO都已经完全确定了，
  // 因此对一组不适合做相应转换的类型，在调用test()之前就会立即触发错误。

  // 由于这一原因，引入了作为成员函数模板参数的F：
  // template<typename F, typename = decltype(aux(std::declval<F>()))>
  // static char test(void*);

  // 并在value的初始化中将FROM类型用作调用test()时的显式模板参数：
  // static constexpr bool value =
  //     std::is_same<decltype(test<FROM>(nullptr)), char>;

  // 请注意这里是如何在不调用任何构造函数的情况下，
  // 通过使用在第19.3.4节介绍的std::declval生成一个类型的值的。
  // 如果这个值可以被转换成TO，对aux()的调用就是有效的，相应的test()调用也就会被匹配到。
  // 否则，会触发SFINAE错误，导致应急test()被调用。

  // 然后，就可以像下面这样使用该萃取了：
  // IsConvertibleT<int, int>::value // true
  // IsConvertibleT<int, std::string>::value // false
  // IsConvertibleT<char const*, std::string>::value // true
  // IsConvertibleT<std::string, char const*>::value // false

 public:
  using Type = decltype(test<FROM>(nullptr));
};
template <typename FROM, typename TO>
struct IsConvertibleT : IsConvertibleHelper<FROM, TO>::Type {};

template <typename FROM, typename TO>
using IsConvertible = typename IsConvertibleT<FROM, TO>::Type;

template <typename FROM, typename TO>
constexpr bool isConvertible = IsConvertibleT<FROM, TO>::value;

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  // std::cout << IsConvertibleT<int, int>::value << std::endl;
  // std::cout << IsConvertibleT<int, std::string>::value << std::endl;
  // std::cout << IsConvertibleT<char const*, std::string>::value << std::endl;
  // std::cout << IsConvertibleT<std::string, char const*>::value << std::endl;
}
}  // namespace n1

namespace n2 {
// 处理特殊情况

// 下面3种情况还不能被上面的IsConvertibleT正确处理：
// 1.向数组类型的转换要始终返回false，但是在上面的代码中，
//   aux()声明中的类型为TO的参数会退化成指针类型，
//   因此对于某些FROM类型，它会返回true；
// 2.向指针类型的转换也应该始终返回false，但是和1中的情况一样，
//   上述实现只会将它们当作退化后的类型；
// 3.向（被const/volatile修饰）的void类型的转换需要返回true，
//   但是不幸的是，在TO是void的时候，上述实现甚至不能被正确实例化，
//   因为参数类型不能包含void类型（而且aux()的定义也用到了这一参数）。

// 对于这几种情况，需要对它们进行额外的偏特化。
// 但是，为所有可能的与const以及volatile的组合情况都分别进行偏特化是很不明智的。
// 相反，为辅助类模板引入了一个额外的模板参数：
// template <typename FROM, typename TO,
//           bool = IsVoidT<TO>::value || IsArrayT<TO>::value ||
//                  IsFunctionT<TO>::value>
// struct IsConvertibleHelper {
//   using Type =
//       std::integral_constant<bool, IsVoidT<TO>::value &&
//       IsVoidT<FROM>::value>;
// };
// template <typename FROM, typename TO>
// struct IsConvertibleHelper<FROM, TO, false> {};

// 额外的bool型模板参数能够保证，对于上面的所有特殊情况，
// 都会最终使用主辅助萃取（而不是偏特化版本）。
// 如果试图将FROM转换为数组或者函数，或者FROM是void而TO不是，
// 都会得到false_type的结果，不过对于FROM和TO都是false_type的情况，
// 它也会返回false_type。其它所有的情况，都会使第三个模板参数为false，
// 从而选择偏特化版本的实现（对应于我们之前介绍的实现）。

// C++标准库中也提供了与之对应的std::is_convertible<>。
}  // namespace n2

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