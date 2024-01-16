#include <climits>
#include <iostream>
#include <type_traits>

// 19.7 其它的萃取技术

namespace n1 {
// 19.7.1 If-Then-Else

// 19.4的PlusResultT的定义采用了完全不同的方法，依赖另一个萃取HasPlusT的结果。
// 可以用一个特殊的类型模板IfThenElse来表达这一if-then-else的行为，
// 它接受一个bool型的模板参数，并根据该参数从另外两个类型参数中间做选择：
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

namespace test1 {
#if __cplusplus == 201703L
// 定义了一个可以为给定数值选择最合适的整形类型的函数：
template <auto N>
struct SmallestIntT {
  using Type = typename IfThenElseT<
      N <= std::numeric_limits<char>::max(), char,
      typename IfThenElseT<
          N <= std::numeric_limits<short>::max(), short,
          typename IfThenElseT<
              N <= std::numeric_limits<int>::max(), int,
              typename IfThenElseT<
                  N <= std::numeric_limits<long>::max(), long,
                  typename IfThenElseT<
                      N <= std::numeric_limits<long long>::max(), long long,
                      void>::Type>::Type>::Type>::Type>::Type;
};

// 和C++的if-else语句不同，在最终做选择之前，then和else中的模板参数都会被计算，
// 因此两个分支中的代码都不能有问题，否则整个程序就会有问题。

void func() {
  std::cout << sizeof(SmallestIntT<42>::Type) << std::endl;          // 1
  std::cout << sizeof(SmallestIntT<1000000000>::Type) << std::endl;  // 4
  std::cout << sizeof(SmallestIntT<std::numeric_limits<long long>::max()>::Type)
            << std::endl;  // 8
}
#endif
}  // namespace test1

namespace test2 {
// 一个可以为给定的有符号类型生成与之对应的无符号类型的萃取。

// 已经有一个标准萃取（std::make_unsigned）可以做这件事情，
// 但是它要求传递进来的类型是有符号的整形，而且不能是bool类型，否则行为未定义。
// 这一萃取不够安全，因此最好能够实现一个这样的萃取，当可能的时候，
// 它就正常返回相应的无符号类型，否则就原样返回被传递进来的类型，
// 这样，当传递进来的类型不合适时，也能避免触发未定义行为。
// 下面这个简单的实现是不行的：
// template <typename T>
// struct UnsignedT {
//   using Type =
//       IfThenElse<std::is_integral<T>::value && !std::is_same<T, bool>::value,
//                  typename std::make_unsigned<T>::type, T>;
// };
// 因为在实例化UnsingedT<bool>的时候，行为依然是未定义的，
// 编译期依然会试图从下面的代码中生成返回类型：
// typename std::make_unsigned<T>::type

// 为了解决这一问题，需要再引入一层额外的间接层，
// 从而让IfThenElse的参数本身用类型函数去封装结果：
template <typename T>
struct IdentityT {
  using Type = T;
};

template <typename T>
struct MakeUnsignedT {
  using Type = typename std::make_unsigned<T>::type;
};
template <typename T>
struct UnsignedT {
  using Type = typename IfThenElse<std::is_integral<T>::value &&
                                       !std::is_same<T, bool>::value,
                                   MakeUnsignedT<T>, IdentityT<T>>::Type;
};
// 在这一版UnsignedT的定义中，IfThenElse的类型参数本身也都是类型函数的实例。
// 只不过在最终IfThenElse做出选择之前，类型函数不会真正被计算。
// 而是由IfThenElse选择合适的类型实例，MakeUnsignedT或者IdentityT。
// 最后由::Type对被选择的类型函数实例进行计算，并生成结果Type。

// 之所以能够这样做，是因为IfThenElse中未被选择的封装类型永远不会被完全实例化。
// 下面的代码也不能正常工作：
// template <typename T>
// struct UnsignedT {
//   using Type = typename IfThenElse<std::is_integral<T>::value &&
//                                        !std::is_same<T, bool>::value,
//                                    MakeUnsignedT<T>::Type, T>::Type;
// };
// 必须要延后对MakeUnsignedT<T>使用::Type，也就是意味着，
// 同样需要为else分支中的T引入IdentyT辅助模板，并同样延后对其使用::Type。

// 同样不能在当前语境中使用如下代码：
template <typename T>
using Identity = typename IdentityT<T>::Type;
// 当然可以定义这样一个别名模板，在其它地方它可能也很有用，
// 但是唯独不能将其用于IfThenElse的定义中，
// 因为任意对Identity<T>的使用都会立即触发对IdentityT<T>的完全实例化，
// 不然无法获取其Type成员。

#if __cplusplus == 201703L
// 在C++标准库中有与IfThenElseT模板对应的模板std::conditional<>。
// 使用这一标准库模板实现的UnsignedT萃取如下：
template <typename T>
struct UnsignedT1 {
  using Type =
      typename std::conditional_t<std::is_integral<T>::value &&
                                      !std::is_same<T, bool>::value,
                                  MakeUnsignedT<T>, IdentityT<T>>::Type;
};
#endif

void func() {
  std::cout << typeid(UnsignedT<int>).name() << std::endl;
  // N2n15test29UnsignedTIiEE
}
}  // namespace test2

void func() {
#if __cplusplus == 201703L
  test1::func();
#endif
  test2::func();
}
}  // namespace n1

namespace n2 {
// 19.7.2 探测不抛出异常的操作

namespace test1 {
// 可能偶尔会需要判断某一个操作会不会抛出异常。
// 在可能的情况下，移动构造函数应当被标记成noexcept的，意思是它不会抛出异常。
// 但是，某一特定class的move constructor是否会抛出异常，
// 通常决定于其成员或者基类的移动构造函数会不会抛出异常。
// 比如对于下面这个简单类模板（Pair）的移动构造函数：
template <typename T1, typename T2>
class Pair {
  T1 first;
  T2 second;

 public:
  // 当T1或者T2的移动操作会抛出异常时，Pair的移动构造函数也会抛出异常：
  Pair(Pair&& other)
      : first(std::forward<T1>(other.first)),
        second(std::forward<T2>(other.second)) {}
};
}  // namespace test1

namespace test2 {
// 如果有一个叫做IsNothrowMoveConstructibleT的萃取，
// 就可以在Pair的移动构造函数中通过使用noexcept将这一异常的依赖关系表达出来。

#if __cplusplus == 201703L
// 可以直接用noexcept萃取，这样就可以判断表达式是否被进行nothrow修饰了：
template <typename T>
struct IsNothrowMoveConstructibleT
    : std::bool_constant<noexcept(T(std::declval<T>()))> {};
// 这里使用了运算符版本的noexcept，它会判断一个表达式是否会抛出异常。
// 由于其结果是bool型的，可以直接将它用于std::bool_constant<>基类的定义，
// std::bool_constant也被用来定义std::true_type和std::false_type。

template <typename T1, typename T2>
class Pair {
  T1 first;
  T2 second;

 public:
  Pair(Pair&& other) noexcept(IsNothrowMoveConstructibleT<T1>::value&&
                                  IsNothrowMoveConstructibleT<T2>::value)
      : first(std::forward<T1>(other.first)),
        second(std::forward<T2>(other.second)) {}
};

// 但是该实现还应该被继续优化，因为它不是SFINAE友好的：
// 如果它被一个没有可用移动或者拷贝构造函数的类型实例化，
// 这样表达式T(std::declval<T&&>())就是无效的，整个程序就会遇到问题：
class E {
 public:
  E(E&&) = delete;
};

void func() {
  // std::cout << IsNothrowMoveConstructibleT<E>::value;  // 编译错误
}
#endif
}  // namespace test2

namespace test3 {
// 在test2中，所期望的并不是让整个程序奔溃，而是获得一个false类型的值。

#if __cplusplus == 201703L
// 就像在第19.4.4节介绍的那样，在真正做计算之前，
// 必须先对被用来计算结果的表达式的有效性进行判断。
// 在这里，要在检查移动构造函数是不是noexcept之前，先对其有效性进行判断。
// 因此，要重写之前的萃取实现，给其增加一个默认值是void的模板参数，
// 并根据移动构造函数是否可用对其进行偏特化：
template <typename T, typename = std::void_t<>>
struct IsNothrowMoveConstructibleT : std::false_type {};

template <typename T>
struct IsNothrowMoveConstructibleT<T,
                                   std::void_t<decltype(T(std::declval<T>()))>>
    : std::bool_constant<noexcept(T(std::declval<T>()))> {};

// 如果在偏特化中对std::void_t<...>的替换有效，那么就会选择该偏特化实现，
// 在其父类中的noexcept(...)表达式也可以被安全的计算出来。
// 否则，偏特化实现会被丢弃，也不会对其进行实例化，
// 被实例化的也将是主模板，产生一个std::false_type的返回值。

// 值得注意的是，除非真正能够调用移动构造函数，
// 否则无法判断移动构造函数是不是会抛出异常。
// 也就是说，移动构造函数仅仅是public和未被标识为delete的还不够，
// 还要求对应的类型不能是抽象类（但是抽象类的指针或者引用却可以）。
// 因此，该类型萃取被命为IsNothrowMoveConstructible，
// 而不是HasNothrowMoveConstructor，对于其它所有的情况，都需要编译期支持。
#endif

// C++标准库提供了与之对应的萃取std::is_move_constructible<>
}  // namespace test3
}  // namespace n2

namespace n3 {
// 19.7.3 萃取的便捷性（Traits Convenience）

// 对萃取的普遍不满是它们相对而言有些繁琐，因为对类型萃取的使用通需要提供::Type尾缀，
// 而且在依赖上下文中，还需要一个typename前缀，两者几成范式。
// 当同时使用多个类型萃取时，会让代码形式变得很笨拙，
// 如果想正确的对其进行实现，需要确保不会返回const或者引用类型：
// template <typename T1, typename T2>
// Array<typename RemoveCVT<
//     typename RemoveReferenceT<typename PlusResultT<T1,
//     T2>::Type>::Type>::Type>
// operator+(Array<T1> const&, Array<T2> const&);
// 通过使用别名模板（alias templates）和变量模板（variable templates），
// 可以让对产生类型或者数值的萃取的使用变得很方便。
// 但是也需要注意，在某些情况下这一简便方式并不使用，依然要使用最原始的类模板。

namespace test1 {
// 别名模板和萃取（Alias Templates and Traits）

// 别名模板为降低代码繁琐性提供了一种方法。
// 相比于将类型萃取表达成一个包含了Type类型成员的类模板，可以直接使用别名模板。
// template <typename T>
// using RemoveCV = typename RemoveCVT<T>::Type;
// template <typename T>
// using RemoveReference = typename RemoveReferenceT<T>::Type;
// template <typename T1, typename T2>
// using PlusResult = typename PlusResultT<T1, T2>::Type;

// 有了这些别名模板，可以将operator+的声明简化成：
// template <typename T1, typename T2>
// Array<RemoveCV<RemoveReference<PlusResultT<T1, T2>>>> operator+(
//     Array<T1> const&, Array<T2> const&);
// 这一版本的实现明显更简洁，也让人更容易分辨其组成。
// 这一特性使得别名模板非常适用于某些类型萃取。

// 但是，将别名模板用于类型萃取也有一些缺点：
// 1.别名模板不能够被进行特化，但是由于很多编写萃取的技术都依赖于特化，
//   别名模板最终可能还是需要被重新导向到类模板；
// 2.有些萃取是需要由用户进行特化的，比如描述了一个求和运算符是否是可交换的萃取，
//   此时在很多使用都用到了别名模板的情况下，对类模板进行特换会很让人困惑；
// 3.对别名模板的使用最会让该类型被实例化，比如，底层类模板的特化，
//   这样对于给定类型我们就很难避免对其进行无意义的实例化，
//   另外一种表述方式是，别名模板不可以和元函数转发一起使用。

// 由于将别名模板用于类型萃取既有优点也有缺点，建议像本节以及C++标准库中那样使用它：
// 1.同时提供根据遵守特定命名管理的类模板，选择使用T后缀以及Type类型成员；
// 2.遵守了稍微不同命名惯例的别名模板，丢弃了T尾缀，
//   而且让每一个别名模板都基于底层的类模板进行定义。
// 这样，在别名模板能够使代码变得更简洁的地方就是用别名模板，
// 否则，对于更为高阶的用户就让他们使用类模板。

// 注意，由于某些历史原因，C++标准库选择了不同的命名惯例。
// 其类型萃取会包含一个type类型成员，但是不会有特定的后缀，
// 在C++11中为某些类型萃取引入了后缀，从C++14开始，为之引入了相应的别名模板，
// 直接生成type，该别名模板会有一个_t后缀，因为没有后缀的名字已经被标准化了。
}  // namespace test1

namespace test2 {
// 变量模板和萃取（Variable Templates and Traits）

// 对于返回数值的萃取需要使用一个::value或者类似的成员来生成萃取的结果。
// 在这种情况下，constexpr修饰的变量模板提供了一种简化代码的方法。

// 比如，下面的变量模板封装了IsSameT萃取和IsConvertibleT萃取：
// template <typename T1, typename T2>
// constexpr bool IsSame = IsSameT<T1, T2>::value;
// template <typename FROM, typename TO>
// constexpr bool IsConvertible = IsConvertibleT<FROM, TO>::value;

// 此时我们可以将这一类代码：
// if (IsSameT<T,int>::value || IsConvertibleT<T,char>::value)
// 简化成：
// if (IsSame<T,int> || IsConvertible<T,char>)

// 同样由于历史原因，C++标准库也采用了不同的命名惯例。
// 产生result结果的萃取类模板并没有特殊的后缀，
// 而且它们中的一些在C++11中就已经被引入进来了。
// 在C++17中引入的与之对应的变量模板则有一个_v后缀。
}  // namespace test2
}  // namespace n3

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