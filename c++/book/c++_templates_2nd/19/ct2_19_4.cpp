#include <iostream>
#include <type_traits>

// 19.4 基于SFINAE的萃取（SFINAE-Based Traits）

// SFINAE会将在模板参数推断过程中，构造无效类型和表达式的潜在错误，
// 即会导致程序出现的语法错误，转换成简单的推断错误，
// 这样就允许重载解析继续在其它待选项中间做选择。
// 虽然SFINAE最开始是被用来避免与函数模板重载相关的伪错误，
// 也可以用它在编译期间判断特定类型和表达式的有效性。
// 比如可以通过萃取来判断一个类型是否有某个特定的成员，
// 是否支持某个特定的操作，或者该类型本身是不是一个类。

// 基于SFINAE的两个主要技术是：
// 1.用SFINAE排除某些重载函数；
// 2.用SFINAE排除某些偏特化。

struct S1 {
  S1() = delete;
};
struct S2 {
  S2() = default;
};

namespace n1 {
// 19.4.1 用SFINAE排除某些重载函数

namespace test1 {
// 第一个例子是将SFINAE用于函数重载，以判断一个类型是否是默认可构造的，
// 对于可以默认构造的类型，就可以不通过值初始化来创建对象。
// 也就是说，对于类型T，诸如T()的表达式必须是有效的。

// template <bool val>
// struct BoolConstant {
//   using Type = BoolConstant<val>;
//   static constexpr bool value = val;
// };
// using TrueType = BoolConstant<true>;
// using FalseType = BoolConstant<false>;
// template <typename T1, typename T2>
// struct IsSameT : FalseType {};
// template <typename T>
// struct IsSameT<T, T> : TrueType {};

template <typename T>
struct IsDefaultConstructibleT {
 private:
  // 通过函数重载实现一个基于SFINAE的萃取的常规方式是，
  // 声明两个返回值类型不同的同名重载函数模板：

  // 想要测试的条件是被传递进来的类型T是否是可以被默认构造的。
  // 为了实现这一目的，将T传递给U，并增加一个无名的（dummy）模板参数，
  // 该模板参数被一个只有在这一转换有效的情况下才有效的构造函数进行初始化。
  // 使用的是只有当存在隐式或者显式的默认构造函数U()时才有效的表达式。
  // 对U()的结果施加了deltype操作，这样就可以用其结果初始化一个类型参数了。
  // 第二个模板参数不可以被推断，因为不会为之传递任何参数，
  // 而且也不会为之提供显式的模板参数，因此，它只会被替换，如果替换失败，
  // 基于SFINAE，相应的test()会被丢弃掉，因此也就只有应急方案可以匹配相应的调用。
  template <typename U, typename = decltype(U())>
  static char test(void*);  // 只有在所需的检查成功时才会被匹配到
  // 不能在第一个test()声明里直接使用模板参数T：
  // 因为对于任意的T，所有模板参数为T的成员函数都会被执行模板参数替换，
  // 因此对一个不可以默认构造的类型，会遇到编译错误，而不是忽略掉第一个test()。
  // 通过将类模板的模板参数T传递给函数模板的参数U，
  // 就只为第二个test()的重载创建了特定的SFINAE上下文。

  template <typename>
  static long test(...);  // 用来应急的，它会匹配任意调用，但优先级最低

 public:
  //  返回值value的具体值取决于最终选择了哪一个test函数：
  static constexpr bool value =
      std::is_same<decltype(test<T>(nullptr)), char>::value;
  // 如果选择的是第一个test()函数，由于其返回值类型是char，
  // value会被初始化为is_same<char, char>，也就是true。
  // 否则，value会被初始化为is_same<long, char>，也就是false。
};

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << IsDefaultConstructibleT<int>::value << std::endl;  // true
  std::cout << IsDefaultConstructibleT<S1>::value << std::endl;   // false
  std::cout << IsDefaultConstructibleT<S2>::value << std::endl;   // true
}
}  // namespace test1

namespace test2 {
// 另一种基于SFINAE的萃取的实现策略

// 远在1998年发布第一版C++标准之前，基于SFINAE的萃取的实现就已经成为了可能。
// 该方法的核心一致都是实现两个返回值类型不同的重载函数模板：
// template <typename...> static char test(void*);
// template <typename...> static long test(...);

// 但是，在最早的实现技术中，会基于返回值类型的大小来判断使用了哪一个重载函数，
// 也会用到0和enum，因为在当时nullptr和constexpr还没有被引入：
// enum { value = sizeof(test<...>(0)) == 1 };
// 在某些平台上，sizeof(char)的值可能会等于sizeof(long)的值，比如，
// 在数字信号处理器（DSP）或者旧的Cray机器上，所有内部的基础类型的大小都可以相同。
// 比如根据定义sizeof(char)==1，但是在这些机器上，sizeof(long)，
// 甚至是sizeof(long long)的值也都是1。
// 基于此，希望能够确保test()的返回值类型在所有的平台上都有不同的值，比如，在定义了：
using Size1T = char;
using Size2T = struct { char a[2]; };
// 或者：
using Size1 = char (&)[1];
using Size2 = char (&)[2];
// 之后，可以像下面这样定义test()的两个重载版本：
// template <typename...> static Size1T test(void*);
// template <typename...> static Size2T test(...);
// 这样，要么返回Size1T，其大小为1，要么返回Size2T，在所有的平台上其值都至少是2。

// 传递给test()的调用参数的类型并不重要。
// 我们所要保证的是被传递的参数和所期望的类型能够匹配。
// 比如，可以将其定义成能够接受整型常量42的形式：
template <typename...>
static Size1T test(int);
template <typename...>
static Size2T test(...);
enum E { value = sizeof(test<>(42)) == 1 };

void func() {
  std::cout << std::boolalpha << E::value << std::endl;  // 1
}
}  // namespace test2

namespace test3 {
// 将基于SFINAE的萃取变参预测型萃取

// 返回bool值的萃取，应该返回一个继承自std::true_type或者std::false_type的值。
// 使用这一方式，同样可以解决在某些平台上sizeof(char)==sizeof(long)的问题。

// 为了这一目的，我们需要间接定义一个IsDefaultConstructibleT。
// 该萃取本身需要继承自一个辅助类的Type成员，该辅助类会返回所需的基类。
// 幸运的是，我们可以简单地将test()的返回值类型用作对应的基类：
// template <typename...> static std::true_type test(void*);
// template <typename...> static std::false_type test(...);
// 然后可以将基类的Type成员声明为：
// using Type = decltype(test<FROM>(nullptr));
// 此时也不再需要使用is_same萃取了。

template <typename T>
struct IsDefaultConstructibleHelper {
 private:
  template <typename U, typename = decltype(U())>
  static std::true_type test(void*);
  template <typename>
  static std::false_type test(...);

 public:
  using Type = decltype(test<T>(nullptr));
};
template <typename T>
struct IsDefaultConstructibleT : IsDefaultConstructibleHelper<T>::Type {};

// 如果第一个test()函数模板是有效的话，那么它就将是被选择的重载函数，
// 其返回值类型std::true_type，并初始化IsDefaultConstructibleHelper::Type。
// 这样的话IsConvertibleT<>就会继承自std::true_type。

// 如果第一个test()函数模板是无效的话，那么它就会被SFINAE剔除掉，
// IsDefaultConstructibleHelper::Type也就会被应急test()的返回值类型初始化，
// 也就是std::false_type，这样的话IsConvertibleT<>就会继承自std::false_type。

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << IsDefaultConstructibleT<int>::value << std::endl;  // true
  std::cout << IsDefaultConstructibleT<S1>::value << std::endl;   // false
  std::cout << IsDefaultConstructibleT<S2>::value << std::endl;   // true
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n1

namespace n2 {
// 19.4.2 用SFINAE排除偏特化

// 另一种实现基于SFINAE的萃取的方式会用到偏特化。

template <typename...>
using VoidT = void;
template <typename, typename = VoidT<>>
struct IsDefaultConstructibleT : std::false_type {};
template <typename T>
struct IsDefaultConstructibleT<T, VoidT<decltype(T())>> : std::true_type {};
// 和上文中优化之后的IsDefaultConstructibleT预测萃取类似，
// 让适一般情况的版本继承自std::false_type，因为默认情况下类型没有size_type成员。
// 此处一个比较有意思的地方是，第二个模板参数的默认值被设定为一个辅助别名模板VoidT。
// 这使得我们能够定义各种使用了任意数量的编译期类型构造的偏特化。

// 针对例子，只需要一个类型构造：decltype(T())
// 这样就可以检测类型T是否是可以被默认初始化的。
// 如果对于某个特定的类型T，其默认构造函数是无效的，此时SIFINEAE就是使该偏特化被丢弃掉，
// 并最终使用主模板，否则该偏特化就是有效的，并且会被选用。

// 很显然，这一定义类型萃取的方法看上去要比之前介绍的使用了函数模板重载的方法精简的多。
// 但是该方法要求要能够将相应的条件放进模板参数的声明中。
// 而使用了函数重载的类模板则使得我们能够使用额外的辅助函数或者辅助类。

void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << IsDefaultConstructibleT<int>::value << std::endl;  // true
  std::cout << IsDefaultConstructibleT<S1>::value << std::endl;   // false
  std::cout << IsDefaultConstructibleT<S2>::value << std::endl;   // true
}

// 在C++17中，C++标准库引入了与VoidT对应的类型萃取std::void_t<>。
// 在C++17之前，std::void_t是很有用的，甚至可以将其定义在std命名空间里：
// #include <type_traits>
// #ifndef __cpp_lib_void_t
// namespace std {
// template<typename...> using void_t = void;
// }
// #endif
// 从C++14开始，C++标准委员会建议：
// 通过定义预先达成一致的特征宏来标识那些标准库的内容以及被实现了。
// 这并不是标准的强制性要求，但是实现者通常都会遵守这一建议，以为其用户提供方便。
// __cpp_lib_void_t就是被建议用来标识在一个库中是否实现了std::void_t的宏，
// 所以在上面的code中将其用于了条件判断。
}  // namespace n2

namespace n3 {
// 19.4.3 将泛型Lambdas用于SFINAE（Using Generic Lambdasfor SFINAE）

// 无论使用哪一种技术，在定义萃取的时候总是需要用到一些样板代码：
// 重载并调用两个test()成员函数，或者实现多个偏特化。
// 在C++17中通过指定一个泛型lambda来做条件测试，将样板代码的数量最小化。

#if __cplusplus == 201703L
// 先介绍一个用两个嵌套泛型lambda表达式构造的工具：
template <typename F, typename... Args,
          typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
std::true_type isValidImpl(void*);
template <typename F, typename... Args>
std::false_type isValidImpl(...);

// isValid是一个类型为lambda闭包的constexpr变量。
// 声明中必须要使用一个占位类型（auto），因为C++没有办法直接表达一个闭包类型。
// 在C++17之前，lambda表达式不能出现在const表达式中。
// 因为isValid是闭包类型的，因此它可以被调用，
// 但是它被调用之后返回的依然是一个闭包类型，返回结果由内部的lambda表达式生成。
inline constexpr auto isValid = [](auto f) {
  return [](auto&&... args) {
    return decltype(isValidImpl<decltype(f), decltype(args)&&...>(nullptr)){};
  };
};

// 辅助变量模板type允许用一个值代表一个类型。
// 对于通过这种方式获得的数值x，可以通过使用decltype(valueT(x))得到其原始类型，
// 这也正是上面被传递给isValid的lambda所做的事情，如果提取的类型不可以被默认构造，
// 要么会得到一个编译错误，要么相关联的声明就会被SFINAE掉。
template <typename T>
struct TypeT {
  using Type = T;
};
template <typename T>
constexpr auto type = TypeT<T>{};

template <typename T>
T valueT(TypeT<T>);

constexpr auto isDefaultConstructible =
    isValid([](auto x) -> decltype((void)decltype(valueT(x))()) {});
// 已经知道isDefaultConstructible的类型是闭包类型，而且正如其名字所暗示的那样，
// 它是一个可以被用来测试某个类型是不是可以被默认构造的函数对象。
// 也就是说，isValid是一个萃取工厂（traits factory）：
// 它会为其参数生成萃取，并用生成的萃取对对象进行测试。

// 当isValid的参数f被绑定到isDefaultConstructible的泛型lambda参数时，
// isValid内部的lambda表达式会变成什么样子。
// 通过对isValid的定义进行替换，得到如下等价代码：
// constexpr auto isDefaultConstructible = [](auto&&... args) {
//   return decltype(isValidImpl<decltype([](auto x) ->
//          decltype((void)decltype(valueT(x))())),
//          decltype(args)&&...> (nullptr)){};
// };
#endif

void func() {
#if __cplusplus == 201703L
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << isDefaultConstructible(type<int>) << std::endl;   // true
  std::cout << isDefaultConstructible(type<int&>) << std::endl;  // false
  std::cout << isDefaultConstructible(type<S1>) << std::endl;    // false
  std::cout << isDefaultConstructible(type<S2>) << std::endl;    // true
#endif

  // 如果回头看看第一个isValidImpl()的定义，会发现它还有一个如下形式的默认模板参数：
  // decltype(std::declval<F>()(std::declval<Args&&>()...))>
  // 它试图对第一个模板参数的值进行调用，
  // 而这第一个参数正是isDefaultConstructible定义中的lambda的闭包类型，
  // 调用参数为传递给isDefaultConstructible的(decltype(args)&&...)类型的值。
  // 由于lambda中只有一个参数x，因此args就需要扩展成一个参数；
  // 在上面的static_assert例子中，参数类型为TypeT<int>或者TypeT<int&>。
  // 对于TypeT<int&>的情况，decltype(valueT(x))的结果是int&，
  // 此时decltype(valueT(x))()是无效的，
  // 因此在第一个isValidImpl()的声明中默认模板参数的替换会失败，
  // 从而该isValidImpl()声明会被SFINAE掉。
  // 这样就只有第二个声明可用，且其返回值类型为std::false_type。

  // 整体而言，传递type<int&>时，isDefaultConstructible会返回false_type。
  // 而如果传递的是type<int>的话，替换不会失败，
  // 因此第一个sValidImpl()的声明会被选择，返回结果也就是true_type类型的值。

  // 回忆一下，为了使SFINAE能够工作，替换必须发生在被替换模板的立即上下文中。
  // 在这个例子中，被替换的模板是isValidImpl的第一个声明，
  // 而且泛型lambda的调用运算符被传递给了isValid。
  // 因此，被测试的内容必须出现在lambda的返回类型中，而不是函数体中。

  // isDefaultConstructible萃取和之前的萃取在实现上有一些不同，
  // 主要体现在它需要执行函数形式的调用，而不是指定模板参数。
  // 这可能是一种更为刻度的方式，但是也可以按照之前的方式实现：
  // template<typename T>using IsDefaultConstructibleT
  // = decltype(isDefaultConstructible(std::declval<T>()));

  // 虽然这是传统的模板声明方式，但是它只能出现在namespace作用域内，
  // 然而isDefaultConstructible的定义却很可能被在一个块作用域内引入。
}

// 到目前为止，这一技术看上去好像并没有那么有竞争力，
// 因为无论是实现中涉及的表达式还是其使用方式都要比之前的技术复杂得多，但是，
// 一旦有了isValid，并且对其进行了很好的理解，有很多萃取都可以只用一个声明实现。
// 比如，对是否能够访问名为first的成员进行测试，就非常简洁：
// constexpr auto hasFirst
//   = isValid([](auto x) -> decltype((void)valueT(x).first) {});
}  // namespace n3

namespace n4 {
// 19.4.4 SFINAE友好的萃取

// 类型萃取应该可以在不使程序出现问题的情况下回答特定的问题。
// 基于SFINAE的萃去解决这一难题的方式是：
// 小心地将潜在的问题捕获进一个SFINAE上下文中，将可能出现的错误转变成相反的结果。

template <typename T1, typename T2>
struct PlusResultT {  // 在应对错误的时候表现的并不是那么好
  using Type = decltype(std::declval<T1>() + std::declval<T2>());
};
template <typename T1, typename T2>
using PlusResult = typename PlusResultT<T1, T2>::Type;
// 用到+的上下文并没有被SFINAE保护。
// 因此，如果程序试着对不支持+运算符的类型执行PlusResultT的话，
// 那么PlusResultT计算本身就会使成勋遇到错误。
template <typename T>
class Array {};
class A {};
class B {};

namespace test1 {
template <typename T1, typename T2>
Array<typename PlusResultT<T1, T2>::Type> operator+(Array<T1> const&,
                                                    Array<T2> const&);
// 很显然，如果没有为数组元素定义合适的+运算符的话，使用PlusResultT<>就会遇到错误。

void addAB(Array<A> arrayA, Array<B> arrayB) {
  // auto sum = arrayA + arrayB;
  // 这里的问题并不是错误会发生在代码明显有问题的地方，没办法对A和B的数组进行求和，
  // 而错误会发生在对operator+模板参数推断的时候，在PlusResultT<A,B>的实例化中。
}
}  // namespace test1

namespace test2 {
// 这会导致一个很值得注意的结果：
// 即使为A和B的数组重载一个求和函数，程序依然可能会遇到编译错误。
template <typename T1, typename T2>
Array<typename PlusResultT<T1, T2>::Type> operator+(Array<T1> const&,
                                                    Array<T2> const&);
Array<A> operator+(Array<A> const& arrayA, Array<B> const& arrayB);
void addAB(Array<A> const& arrayA, Array<B> const& arrayB) {
  // auto sum = arrayA + arrayB;
  // 如果编译器可以在不对第一个operator+模板声明进行推断和替换的情况下，
  // 就能够判断出第二个operator+声明会更加匹配的话，上述代码也不会有问题。
}
}  // namespace test2

namespace test3 {
template <typename T1, typename T2>
struct PlusResultT {
  using Type = decltype(std::declval<T1>() + std::declval<T2>());
};
Array<A> operator+(Array<A> const& arrayA, Array<B> const& arrayB) {}
void addAB(Array<A> const& arrayA, Array<B> const& arrayB) {
  auto sum = arrayA + arrayB;
}
}  // namespace test3

namespace test4 {
// 解决test3的问题，要将PlusResultT变成SFINAR友好的，需要提供更恰当的定义，
// 以使其即使会在decltype中遇到错误，也不会诱发编译错误。

// 定义一个HasPlusT萃取，来判断给定的类型是有一个可用的+运算符：
#if __cplusplus == 201703L
template <typename...>
using VoidT = std::void_t<>;
#else
template <typename...>
using VoidT = void;
#endif
template <typename, typename, typename = n2::VoidT<>>
struct HasPlusT : std::false_type {};
template <typename T1, typename T2>
struct HasPlusT<T1, T2,
                VoidT<decltype(std::declval<T1>() + std::declval<T2>())>>
    : std::true_type {};

// 如果其返回结果为true，PlusResultT就可以使用现有的实现。
// 否则，PlusResultT就需要一个安全的默认实现。
// 对于一个萃取，如果对某一组模板参数它不能生成有意义的结果，
// 那么最好的默认行为就是不为其提供Type成员。
// 这样，如果萃取被用于SFINAE上下文中，比如之前array的operator+的返回值类型，
// 缺少Type成员会导致模板参数推断出错，这也正是所期望的array的operator+的行为。

template <typename T1, typename T2, bool = HasPlusT<T1, T2>::value>
struct PlusResultT {
  using Type = decltype(std::declval<T1>() + std::declval<T2>());
};
template <typename T1, typename T2>
struct PlusResultT<T1, T2, false> {};
// 有默认值的模板参数，会使用HasPlusT来判断前面的两个模板参数是否支持求和操作。
// 然后对于第三个模板参数的值为false的情况进行了偏特化，
// 而且在该偏特化中没有任何成员，从而避免了我们所描述过的问题。
// 对与支持求和操作的情况，第三个模板参数的值是true，
// 因此会选用主模板，也就是定义了Type成员的那个模板。
// 这样就保证了只有对支持+操作的类型，PlusResultT才会提供返回类型。
// 注意，被进行求和的模板参数不应该被显式的指定参数。

// 再次考虑Array<A>和Array<B>的求和：
// 如果使用最新的PlusResultT实现，那么PlusResultT<A,B>的实例没有Type成员，
// 因为不能够对A和B进行求和。
// 因此对应的operator+模板的返回值类型是无效的，该函数模板也就会被SFINAE掉。
// 这样就会去选择专门为Array<A>和Array<B>指定的operator+的重载版本。

// 作为一般的设计原则，在给定了合理的模板参数的情况下，
// 萃取模板永远不应该在实例化阶段出错，实先方式通常是执行两次相关的检查：
// 1.一次是检查相关操作是否有效；
// 2.一次是计算其结果。

// 在PlusResultT中已经见证了这一原则，
// 通过调用HasPlusT<>来判断PlusResultImpl<>中对operator+的调用是否有效。
}  // namespace test4

namespace test5 {
// 将test4的原则用于19.3.1节的ElementT：它从一个容器类型生成该容器的元素类型。
// 同样的，由于其结果依赖于该容器类型所包含的成员类型value_type，
// 因此主模板应该只有在容器类型包含value_type成员的时候，才去定义成员类型Type：
// template <typename C, bool = HasMemberT_value_type<C>::value>
// struct ElementT {
//   using Type = typename C::value_type;
// };
// template <typename C>
// struct ElementT<C, false> {};
}  // namespace test5

namespace test6 {
// 第三个能够让萃取变得SFINAE友好的例子将在第19.7.2节介绍，
// 在那里对于IsNothrowMoveConstructibleT需要首先检测是否存在游动构造函数，
// 然后检测它是否被声明为noexcept的。
}
}  // namespace n4

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