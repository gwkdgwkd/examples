#include <complex>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// 19.6 探测成员（Detecting Members）

// 另一种对基于SFINAE的萃取的应用是：
// 创建一个可以判断给定类型T是否含有名为X的成员（类型或者非类型成员）的萃取。

struct S {
  using size_type = std::size_t;
};
class C {
  using size_type = std::size_t;
};
struct CXR {
  using size_type = char &;
};

#if __cplusplus == 201703L
template <typename...>
using VoidT = std::void_t<>;
#else
template <typename...>
using VoidT = void;
#endif

namespace n1 {
// 19.6.1 探测类型成员（Detecting Member Types）

namespace test1 {
// 首先定义一个可以判断给定类型T是否含有类型成员size_type的萃取：
template <typename, typename = VoidT<>>
struct HasSizeTypeT : std::false_type {};
template <typename T>
struct HasSizeTypeT<T, VoidT<typename T::size_type>> : std::true_type {};
// 这里用到了在第19.4.2节介绍的剔除偏特化的方法。

// 和往常已有，对于预测萃取，让一般情况派生自std::false_type，
// 因为某些情况下一个类型是没有size_type成员的，在这种情况下，只需要一个条件：
// typename T::size_type
// 该条件只有在T含有类型成员size_type的时候才有效，这也正是想要做的。
// 如果对于某个类型T，该条件无效，那么SFINAE会使偏特化实现被丢弃，
// 就退回到主模板的情况，否则，偏特化有效并且会被有限选取。

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << HasSizeTypeT<int>::value << std::endl;  // false
  std::cout << HasSizeTypeT<S>::value << std::endl;    // true

  // 如果类型成员size_type是private的，HasSizeTypeT会返回false，
  // 因为并没有访问该类型的权限，因此typename T::size_type是无效的，触发SFINAE。
  // 也就是说，该萃取所做的事情是测试是否能够访问类型成员size_type。
  // std::cout << HasSizeTypeT<C>::value << std::endl;  // 报错

  // 在处理引用类型的时候可能会遇到让人意外的事情：
  std::cout << HasSizeTypeT<CXR>::value << std::endl;     // true
  std::cout << HasSizeTypeT<CXR &>::value << std::endl;   // false
  std::cout << HasSizeTypeT<CXR &&>::value << std::endl;  // false
}
}  // namespace test1

namespace test2 {
// 处理引用类型

// 引用类型确实没有成员，但是当使用引用的时候，结果表达式的类型是引用所指向的类型，
// 因此可能会希望，当传递进来的模板参数是引用类型的时候，依然根据其指向的类型做判断。
// 为了这一目的，可以在HasSizeTypeT的偏特化中使用之前介绍的RemoveReference萃取：
template <typename, typename = VoidT<>>
struct HasSizeTypeT : std::false_type {};
template <typename T>
struct HasSizeTypeT<T,
                    VoidT<typename std::remove_reference<T>::type::size_type>>
    : std::true_type {};

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << HasSizeTypeT<int>::value << std::endl;     // false
  std::cout << HasSizeTypeT<S>::value << std::endl;       // true
  std::cout << HasSizeTypeT<CXR>::value << std::endl;     // true
  std::cout << HasSizeTypeT<CXR &>::value << std::endl;   // true
  std::cout << HasSizeTypeT<CXR &&>::value << std::endl;  // true
}
}  // namespace test2

namespace test3 {
// 注入类的名字（Injected Class Names）

// 对于注入类的名字（见13.2.3节），上述检测类型成员的萃取也会返回true。
struct size_type {};
struct Sizeable : size_type {};
void func() {
  std::cout << test2::HasSizeTypeT<Sizeable>::value << std::endl;  // true
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n1

namespace n2 {
// 19.6.2 探测任意类型成员

// 在定义了诸如HasSizeTypeT的萃取之后，会很自然的想到该如何将该萃取参数化，
// 以对任意名称的类型成员做探测，不幸的是，目前这一功能只能通过宏来实现，
// 因为还没有语言机制可以被用来描述潜在的名字。
// 当前不使用宏的、与该功能最接近的方法是使用泛型lambda，如19.6.4节介绍的那样。

#define DEFINE_HAS_TYPE(MemType)                  \
  template <typename, typename = VoidT<>>         \
  struct HasTypeT_##MemType : std::false_type {}; \
  template <typename T>                           \
  struct HasTypeT_##MemType<T, VoidT<typename T::MemType>> : std::true_type {}

// 每次对DEFINE_HAS_TYPE(MemberType)的使用都相当于定义了一个新的萃取：
DEFINE_HAS_TYPE(value_type);
DEFINE_HAS_TYPE(char_type);

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << HasTypeT_value_type<int>::value << std::endl;  // false
  std::cout << HasTypeT_value_type<std::vector<int>>::value
            << std::endl;                                               // true
  std::cout << HasTypeT_value_type<std::iostream>::value << std::endl;  // false
  std::cout << HasTypeT_char_type<std::iostream>::value << std::endl;   // true
}
}  // namespace n2

namespace n3 {
// 19.6.3 探测非类型成员

namespace test1 {
// 可以继续修改上述萃取，以让其能够测试数据成员和单个的成员函数：
#define DEFINE_HAS_MEMBER(Member)                            \
  template <typename, typename = VoidT<>>                    \
  struct HasMemberT_##Member : std::false_type {};           \
  template <typename T>                                      \
  struct HasMemberT_##Member<T, VoidT<decltype(&T::Member)>> \
      : std::true_type {}
// 当&::Member无效时，偏特化实现会被SFINAE掉，为了使条件有效，必须满足如下条件：
// 1.Member必须能够被用来没有歧义的识别出T的一个成员，
//   比如，它不能是重载成员函数的名字，也不能是多重继承中名字相同的成员的名字；
// 2.成员必须可以被访问；
// 3.成员必须是非类型成员以及非枚举成员，否则前面的&会无效；
// 4.如果T::Member是static数据成员，那么与其对应的类型，
//   必须没有提供使得&T::Member无效的operator&，比如，将operator&设成不可访问的。

// 修改上面的偏特化实现以排除那些&T::Member不是成员指针的情况并不会很难，
// 比如排除static数据成员，类似地，也可以限制该偏特化仅适用于数据成员或者成员函数。

DEFINE_HAS_MEMBER(size);
DEFINE_HAS_MEMBER(first);
DEFINE_HAS_MEMBER(begin);

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << HasMemberT_size<int>::value << std::endl;               // false
  std::cout << HasMemberT_size<std::vector<int>>::value << std::endl;  // true
  std::cout << HasMemberT_first<std::pair<int, int>>::value
            << std::endl;  // true

  // HasMember萃取只可以被用来测试是否存在唯一一个与特定名称对应的成员。
  // 如果存在两个同名的成员的话，该测试也会失败，当测试重载成员函数是否存在时：
  std::cout << HasMemberT_begin<std::vector<int>>::value << std::endl;  // false
}
}  // namespace test1

namespace test2 {
// 探测成员函数

// SFINAE会确保我们不会在函数模板声明中创建非法的类型和表达式，
// 从而可以使用重载技术进一步测试某个表达式是否是病态的。

// 也就是说，可以很简单地测试能否按照某种形式调用我们所感兴趣的函数，
// 即使该函数被重载了，相关调用可以成功。
// 正如在第19.5节介绍的IsConvertibleT一样，此处的关键是能否构造一个表达式，
// 以测试能否在decltype中调用begin()，并将该表达式用作额外的模板参数的默认值：

template <typename, typename = VoidT<>>
struct HasBeginT : std::false_type {};
template <typename T>
struct HasBeginT<T, VoidT<decltype(std::declval<T>().begin())>>
    : std::true_type {};

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << HasBeginT<std::vector<int>>::value << std::endl;  // true
}
}  // namespace test2

namespace test3 {
// 探测其它的表达式

// 相同的技术还可以被用于其它的表达式，甚至是多个表达式的组合。
// 比如，可以测试对类型为T1和T2的对象，是否有合适的<运算符可用：
template <typename, typename, typename = VoidT<>>
struct HasLessT : std::false_type {};
template <typename T1, typename T2>
struct HasLessT<T1, T2,
                VoidT<decltype(std::declval<T1>() < std::declval<T2>())>>
    : std::true_type {};
// 和往常一样，问题的难点在于该如果为所要测试的条件定义一个有效的表达式，
// 并通过使用decltype将其放入SFINAE的上下文中，在该表达式无效的时候，
// SFINAE机制会最终选择主模板：decltype(std::declval<T1>() < std::declval<T2>())
// 采用这种方式探测表达式有效性的萃取是很稳健的：
// 如果表达式没有问题，它会返回true，
// 而如果<运算符有歧义，被删除，或者不可访问的话，它也可以准确的返回false。

void func() {
  std::cout << HasLessT<int, char>::value << std::endl;                 // true
  std::cout << HasLessT<std::string, std::string>::value << std::endl;  // true
  std::cout << HasLessT<std::string, int>::value << std::endl;          // false
  std::cout << HasLessT<std::string, char *>::value << std::endl;       // true
  std::cout << HasLessT<std::complex<double>, std::complex<double>>::value
            << std::endl;  // false
}

// 正如在2.3.1节介绍的那样，也可以通过使用该萃取去要求模板参数T必须要支持<运算符：
template <typename T>
class C {
  // static_assert(HasLessT<T>::value, "Class C requires comparable elements");
};

// 值得注意的是，基于 std::void_t的特性，可以将多个限制条件放在同一个萃取中：
template <typename, typename = VoidT<>>
struct HasVariousT : std::false_type {};
template <typename T>
struct HasVariousT<T, VoidT<decltype(std::declval<T>().begin()),
                            typename T::difference_type, typename T::iterator>>
    : std::true_type {};
// 能够测试某一语法特性有效性的萃取是很有用的，基于有或者没有某一特定操作，
// 可以用该萃取去客制化模板的行为，这一类萃取既可以被用于SFINAE友好的萃取的一部分，
// 也可以为基于类型特性的重载提供帮助。
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n3

namespace n4 {
// 19.6.4 用泛型Lambda探测成员

#if __cplusplus == 201703L
template <typename F, typename... Args,
          typename = decltype(std::declval<F>()(std::declval<Args &&>()...))>
std::true_type isValidImpl(void *);
template <typename F, typename... Args>
std::false_type isValidImpl(...);

inline constexpr auto isValid = [](auto f) {
  return [](auto &&... args) {
    return decltype(isValidImpl<decltype(f), decltype(args) &&...>(nullptr)){};
  };
};

template <typename T>
struct TypeT {
  using Type = T;
};
template <typename T>
constexpr auto type = TypeT<T>{};

template <typename T>
T valueT(TypeT<T>);

// 在19.4.3介绍的isValid lambda，提供了一种定义可以被用来测试成员的技术，
// 其主要的好处是在处理名称任意的成员时，不需要使用宏，而且更为紧凑。

namespace test1 {
// 展示了定义可以检测数据或者类型成员是否存在，比如first或者size_type，
// 或者有没有为两个不同类型的对象定义operator<的萃取的方式：
constexpr auto hasFirst =
    isValid([](auto x) -> decltype((void)valueT(x).first) {});
constexpr auto hasSizeType =
    isValid([](auto x) -> typename decltype(valueT(x))::size_type {});
constexpr auto hasLess =
    isValid([](auto x, auto y) -> decltype(valueT(x) < valueT(y)) {});

void func1() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << hasFirst(type<std::pair<int, int>>) << std::endl;  // true

  std::cout << hasSizeType(type<S>) << std::endl;    // true
  std::cout << hasSizeType(type<int>) << std::endl;  // false

  std::cout << hasLess(42, type<char>) << std::endl;         // false ?
  std::cout << hasLess(type<int>, type<char>) << std::endl;  // true
  std::cout << hasLess(type<std::string>, type<std::string>)
            << std::endl;                                           // true
  std::cout << hasLess(type<std::string>, type<int>) << std::endl;  // false
  std::cout << hasLess(type<std::string>, "hello") << std::endl;    // false ?
  std::cout << hasLess(type<std::string>, type<char *>) << std::endl;  // true
}
}  // namespace test1

namespace test2 {
// 为了能够使用统一的泛型语法（将类型用于模板参数），可以继续定义额外的辅助工具：
constexpr auto hasFirst = isValid([](auto &&x) -> decltype((void)&x.first) {});
template <typename T>
using HasFirstT = decltype(hasFirst(std::declval<T>()));

// hasSizeType通过std::decay将参数的引用删除了，因为不能访问引用中的类型成员。
// 如果不这么做，该萃取对于引用类型会始终返回false，第二个isValidImpl<>被使用。
constexpr auto hasSizeType =
    isValid([](auto &&x) -> typename std::decay_t<decltype(x)>::size_type {});
template <typename T>
using HasSizeTypeT = decltype(hasSizeType(std::declval<T>()));

constexpr auto hasLess = isValid([](auto &&x, auto &&y) -> decltype(x < y) {});
template <typename T1, typename T2>
using HasLessT = decltype(hasLess(std::declval<T1>(), std::declval<T2>()));

void func2() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << HasFirstT<std::pair<int, int>>::value << std::endl;  // true

  std::cout << HasSizeTypeT<S>::value << std::endl;    // true
  std::cout << HasSizeTypeT<int>::value << std::endl;  // false

  std::cout << HasLessT<int, char>::value << std::endl;                 // true
  std::cout << HasLessT<std::string, std::string>::value << std::endl;  // true
  std::cout << HasLessT<std::string, int>::value << std::endl;          // false
  std::cout << HasLessT<std::string, char *>::value << std::endl;       // true
}
}  // namespace test2
#endif

void func() {
#if __cplusplus == 201703L
  test1::func1();
  test2::func2();
#endif
}

}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}