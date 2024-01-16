#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>

// 19.8 类型分类（Type Classification）

// 在某些情况下，如果能够知道一个模板参数的类型是内置类型，指针类型，
// class类型，或者是其它什么类型，将会很有帮助。

// 通过它们可以判断给定类型的各种特性，这样就可以单独为特定的某些类型编写代码：
// if (IsClassT<T>::value) {}
// 或者是将其用于C++17中引入的编译期if以及某些为了萃取的便利性而引入的特性：
// if constexpr (IsClass<T>) {}
// 或者时将其用于偏特化：
// template<typename T, bool = IsClass<T>>
// class C {};
// template<typename T>
// class C<T, true> {};

// 此外，诸如IsPointerT<T>::value一类的表达式的结果是bool型常量，
// 因此它们也将是有效的非类型模板参数，这样，就可以构造更为高端和强大的模板，
// 这些模板可以被基于它们的类型参数的特性进行特化，C++标准库定义了一些类似的萃取，
// 这些萃取可以判断一个类型的主要种类或者是该类型被复合之后的种类。

namespace n1 {
// 19.8.1 判断基础类型（Determining Fundamental Types）

// 先定义一个可以判断某个类型是不是基础类型的模板。
// 默认情况下，认为类型不是基础类型，而对于基础类型，分别进行了特化。

// 主模板定义了常规情况，通常而言IfFundaT<T>::value会返回false：
template <typename T>
struct IsFundaT : std::false_type {};

// 定义了一个可以扩展成所需代码的宏：
#define MK_FUNDA_TYPE(T) \
  template <>            \
  struct IsFundaT<T> : std::true_type {};

// 对于每一种基础类型，都进行了特化，因此IsFundaT<T>::value也都会返回true：
MK_FUNDA_TYPE(void)
MK_FUNDA_TYPE(bool)
MK_FUNDA_TYPE(char)
MK_FUNDA_TYPE(signed char)
MK_FUNDA_TYPE(unsigned char)
MK_FUNDA_TYPE(wchar_t)
MK_FUNDA_TYPE(char16_t)
MK_FUNDA_TYPE(char32_t)
MK_FUNDA_TYPE(signed short)
MK_FUNDA_TYPE(unsigned short)
MK_FUNDA_TYPE(signed int)
MK_FUNDA_TYPE(unsigned int)
MK_FUNDA_TYPE(signed long)
MK_FUNDA_TYPE(unsigned long)
MK_FUNDA_TYPE(signed long long)
MK_FUNDA_TYPE(unsigned long long)
MK_FUNDA_TYPE(float)
MK_FUNDA_TYPE(double)
MK_FUNDA_TYPE(long double)
MK_FUNDA_TYPE(std::nullptr_t)
#undef MK_FUNDA_TYPE

// 采用同样会的方式，也可以定义类型函数IsIntegralT和IsFloatingT，
// 来区分哪些类型是整形标量类型以及浮点型标量类型。

// C++标准库采用了一种更为细粒度的方法来测试一个类型是不是基础类型。
// 它先定义了主要的类型种类，每一种类型都被匹配到一个相应的种类，
// 然后合成诸如std::is_integral和std::is_fundamental类型种类。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsFundaT<int>::value << std::endl;    // true
  std::cout << IsFundaT<float>::value << std::endl;  // true
  std::cout << IsFundaT<char>::value << std::endl;   // true

  struct S {};
  std::cout << IsFundaT<S>::value << std::endl;            // false
  std::cout << IsFundaT<std::string>::value << std::endl;  // false
  std::cout << IsFundaT<char *>::value << std::endl;       // false
}
}  // namespace n1

namespace n2 {
// 19.8.2 判断复合类型

// 复合类型是由其它类型构建出来的类型。
// 简单的复合类型包含指针类型，左值以及右值引用类型，指向成员的指针类型，
// 和数组类型，它们是由一种或者两种底层类型构造的。
// Class类型以及函数类型同样也是复合类型，但是它们可能是由任意数量的类型组成的。
// 在这一分类方法中，枚举类型同样被认为是复杂的符合类型，
// 虽然它们不是由多种底层类型构成的，简单的复合类型可以通过偏特化来区分。

namespace test1 {
// 指针

// 主模板会捕获所有的非指针类型，和往常一样，
// 其值为fase的value成员是通过基类std::false_type提供的，表明该类型不是指针。
template <typename T>
struct IsPointerT : std::false_type {};

// 偏特化实现会捕获所有的指针类型（T*），其为true的成员value表明该类型是一个指针。
// 偏特化实现还额外提供了类型成员BaseT，描述了指针所指向的类型。
// 注意该类型成员只有在原始类型是指针的时候才有，从其使其变成SFINAE友好的类型萃取。
template <typename T>
struct IsPointerT<T *> : std::true_type {
  using BaseT = T;
};
// C++标准库也提供了相对应的萃取std::is_pointer<>，
// 但是没有提供一个成员类型来描述指针所指向的类型。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsPointerT<char>::value << " " << std::is_pointer<char>::value
            << std::endl;  // false false

  std::cout << IsPointerT<char *>::value << " "
            << std::is_pointer<char *>::value << std::endl;  // true true
}
}  // namespace test1
namespace test2 {
// 引用

// 相同的方法也可以被用来识别左值引用：
template <typename T>
struct IsLValueReferenceT : std::false_type {};
template <typename T>
struct IsLValueReferenceT<T &> : std::true_type {
  using BaseT = T;
};
// 以及右值引用：
template <typename T>
struct IsRValueReferenceT : std::false_type {};
template <typename T>
struct IsRValueReferenceT<T &&> : std::true_type {
  using BaseT = T;
};
// 它俩又可以被组合成IsReferenceT<>萃取：
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
template <typename T>
class IsReferenceT
    : public IfThenElseT<IsLValueReferenceT<T>::value, IsLValueReferenceT<T>,
                         IsRValueReferenceT<T>>::Type {};
// 用IfThenElseT从ISLvalueReference<T>和IsRValueReferenceT<T>中选择基类，
// 还用到了元函数转发，如果T是左值引用，会从IsLReference<T>做继承，
// 并通过继承得到相应的value和BaseT成员，否则，就从IsRValueReference<T>做继承，
// 它会判断一个类型是不是右值引用，并未相应的情况提供对应的成员。
// C++标准库提供了std::is_lvalue_reference<>和std::is_rvalue_reference<>萃取，
// 还有std::is_reference<>，同样的，这些萃取也没有提供代表其所引用的类型的类型成员。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsLValueReferenceT<int>::value << " "
            << IsRValueReferenceT<int>::value << std::endl;  // false false

  std::cout << IsLValueReferenceT<int &>::value << " "
            << std::is_lvalue_reference<int &>::value
            << std::endl;  // true true

  std::cout << IsRValueReferenceT<int &&>::value << " "
            << std::is_rvalue_reference<int &&>::value
            << std::endl;  // true true

  std::cout << IsReferenceT<int &>::value << " "
            << std::is_reference<int &>::value << std::endl;  // true true

  std::cout << IsReferenceT<int &&>::value << " "
            << std::is_reference<int &&>::value << std::endl;  // true true
}
}  // namespace test2

namespace test3 {
// 数组

// 在定义可以判断数组的萃取时，意外的是偏特化实现中的模板参数数量要比主模板多：
template <typename T>
struct IsArrayT : std::false_type {};
template <typename T, std::size_t N>
struct IsArrayT<T[N]> : std::true_type {
  // 多个额外的成员被用来描述被用来分类的数组的信息：
  using BaseT = T;                        // 数组的基本类型
  static constexpr std::size_t size = N;  // 数组大小
};
template <typename T>
struct IsArrayT<T[]> : std::true_type {
  using BaseT = T;
  static constexpr std::size_t size = 0;  // 未知大小的数组
};
// C++标准库提供了相应的std::is_array<>来判断一个类型是不是数组，除此之外，
// std::rank<>和std::extent<>等萃取还允许查询数组的维度以及某个维度的大小。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsArrayT<int>::value << std::endl;  // false
  std::cout << IsArrayT<int[5]>::value << " " << IsArrayT<int[5]>::size
            << std::endl;  // true 5
  std::cout << IsArrayT<int[]>::value << " " << IsArrayT<int[]>::size
            << std::endl;  // true 0
}
}  // namespace test3
namespace test4 {
// 指向成员的指针（Pointers to Members）

// 也可以用相同的方式处理指向成员的指针：
template <typename T>
struct IsPointerToMemberT : std::false_type {};
template <typename T, typename C>
struct IsPointerToMemberT<T C::*> : std::true_type {
  // 额外的成员提供了与成员的类型以及class的类型相关的信息：
  using MemberT = T;
  using ClassT = C;
};

// C++标准库提供了更为具体的萃取：
// std::is_member_object_pointer<>和std::is_member_function_pointer<>，
// 还有std::is_member_pointer<>。

struct S {
  int i;
  void f() {}
};

void func() {
  std::cout << IsPointerToMemberT<int *>::value << std::endl;          // false
  std::cout << IsPointerToMemberT<int S::*>::value << std::endl;       // true
  std::cout << IsPointerToMemberT<void (S::*)()>::value << std::endl;  // true
  std::cout << std::is_member_object_pointer<int S::*>::value
            << std::endl;  // true
  std::cout << std::is_member_object_pointer<void (S::*)()>::value
            << std::endl;  // false
  std::cout << std::is_member_function_pointer<int S::*>::value
            << std::endl;  // false
  std::cout << std::is_member_function_pointer<void (S::*)()>::value
            << std::endl;                                             // true
  std::cout << std::is_member_pointer<int S::*>::value << std::endl;  // true
  std::cout << std::is_member_pointer<void (S::*)()>::value
            << std::endl;  // true
}
}  // namespace test4

void func() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n2

namespace n3 {
// 19.8.3 识别函数类型（Identifying Function Types）

template <typename... Elements>
class Typelist {};  // 第24章

// 函数类型比较有意思，因为它们除了返回类型，还可能会有任意数量的参数。
// 因此，在匹配一个函数类型的偏特化实现中，用一个参数包来捕获所有的参数类型，
// 就如同在19.3.2节中对DecayT所做的那样：
template <typename T>
struct IsFunctionT : std::false_type {};
template <typename R, typename... Params>
struct IsFunctionT<R(Params...)> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = false;
};
// 上述实现中函数类型的每一部分都被暴露了出来：
// 返回类型被Type标识，所有的参数都被作为ParamsT捕获进了一个typelist中，
// 而可变参数...表示的是当前函数类型使用的是不是C风格的可变参数。

// 不幸的是，这一形式的IsFunctionT并不能处理所有的函数类型，
// 因为函数类型还可以包含const和volatile修饰符，以及左值或者右值引用修饰符，
// 在C++17之后，还有noexcept修饰符：
// using MyFuncType = void (int&) const;

// 这一类函数类型只有在被用于非static成员函数的时候才有意义，
// 但是不管怎样都算得上是函数类型。
// 而且，被标记为const的函数类型并不是真正意义上的const类型，
// 因此RemoveConst并不能将const从函数类型中移除。
// 因此，为了识别有限制符的函数类型，需要引入一大批额外的偏特化实现，
// 来覆盖所有可能的限制符组合，每一个实现都需要包含C风格和非C风格的可变参数情况。

// 只展示所有偏特化实现中的5中情况：
template <typename R, typename... Params>
struct IsFunctionT<R(Params...) const> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = false;
};
template <typename R, typename... Params>
struct IsFunctionT<R(Params..., ...) volatile> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = true;
};
template <typename R, typename... Params>
struct IsFunctionT<R(Params..., ...) const volatile> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = true;
};
template <typename R, typename... Params>
struct IsFunctionT<R(Params..., ...) &> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = true;
};
template <typename R, typename... Params>
struct IsFunctionT<R(Params..., ...) const &> : std::true_type {
  using Type = R;
  using ParamsT = Typelist<Params...>;
  static constexpr bool variadic = true;
};
// 当所有这些都准备完毕之后，就可以识别除class类型和枚举类型之外的所有类型了。
// C++标准库也提供了相应的std::is_function<>萃取。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsFunctionT<int>::value << std::endl;  // false
  std::cout << IsFunctionT<std::function<void(int)>>::value
            << std::endl;                                       // false ?
  std::cout << IsFunctionT<void (*)(int)>::value << std::endl;  // false ?
}
}  // namespace n3

#if __cplusplus == 201703L
template <typename...>
using VoidT = std::void_t<>;
#else
template <typename...>
using VoidT = void;
#endif

namespace n4 {
// 19.8.4 判断class类型（Determining Class Types）

// 和到目前为止已经处理的各种复合类型不同，没有相应的偏特化模式来专门匹配class类型。
// 也不能像基础类型一样列举所有的class类型，需要用一种间接的方法来识别class类型，
// 为此需要找出一些适用于所有class类型的类型或者表达式，但是不能适用于其它类型。
// 有着这样的类型或者表达式之后，就可以使用在第19.4节介绍的SFINAE萃取技术了。

// class中可以被用来识别class类型的最为方便的特性是：
// 只有class类型可以被用于指向成员的指针类型的基础。
// 也就是说，对于X Y::*一类的类型结构，Y只能是class类型。
// 下面的IsClassT<>就利用了这一特性：
template <typename T, typename = VoidT<>>
struct IsClassT : std::false_type {};
template <typename T>
struct IsClassT<T, VoidT<int T::*>> : std::true_type {};

// C++标准库提供了std::is_class<>和std::is_union萃取。
// 但是，这些萃取需要编译期进行专门的支持，
// 因为目前还不能通过任何核心的语言技术将class和struct从union类型中分辨出来。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  std::cout << IsClassT<int>::value << std::endl;  // false

  struct S {};
  std::cout << IsClassT<S>::value << std::endl;  // true

  auto l = [] {};
  std::cout << IsClassT<decltype(l)>::value << std::endl;  // true
  // C++语言规则指出，lambda表达式的类型是唯一的，未命名的，非枚举class类型。

  union U {};
  std::cout << IsClassT<U>::value << std::endl;  // true
  // 根据C++标准，枚举类型也是class类型。

  enum E {};
  std::cout << IsClassT<E>::value << std::endl;  // false
}
}  // namespace n4

namespace n5 {
// 19.8.5 识别枚举类型（Determining Enumeration Types）

// 目前通过已有的萃取技术还唯一不能识别的类型是枚举类型。
// 可以通过编写基于SFINAE的萃取来实现这一功能，
// 这里首先需要测试是否可以像整形类型进行显式转换，然后依次排除基础类型，
// class类型，引用类型，指针类型，还有指向成员的指针类型，
// 这些类型都可以被转换成整形类型，但是都不是枚举类型。
// 但是也有更简单的方法，因为发现所有不属于其它任何一种类型的类型就是枚举类型，
// 这样就可以像下面这样实现该萃取：
template <typename T>
struct IsEnumT {
  static constexpr bool value =
      !n1::IsFundaT<T>::value && !n2::test1::IsPointerT<T>::value &&
      !n2::test2::IsReferenceT<T>::value && !n2::test3::IsArrayT<T>::value &&
      !n2::test4::IsPointerToMemberT<T>::value && !n3::IsFunctionT<T>::value &&
      !n4::IsClassT<T>::value;
};

// C++标准库提供了相对应的std::is_enum<>萃取，通常，为了提高编译性能，
// 编译期会直接提供这一类萃取，而不是将其实现为其它的样子。

void func() {
  std::cout.setf(std::ios_base::boolalpha);

  struct S {};
  std::cout << IsEnumT<S>::value << std::endl;  // false

  union U {};
  std::cout << IsEnumT<U>::value << std::endl;  // false

  enum E {};
  std::cout << IsEnumT<E>::value << std::endl;  // true
}
}  // namespace n5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
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
    case 4:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}