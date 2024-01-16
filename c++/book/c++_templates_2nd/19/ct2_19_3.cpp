#include <iostream>
#include <list>
#include <type_traits>
#include <typeinfo>
#include <vector>

// 19.3 类型函数（Type Function）

// 传统上在C和C++里定义的函数可以被更明确的称为值函数value functions：
// 它们接收一些值作为参数并返回一个值作为结果。
// 对于模板，还可以定义类型函数type functions：
// 它们接收一些类型作为参数并返回一个类型或者常量作为结果。

namespace n1 {
// sizeof是很有用的内置类型函数，它返回代表给定类型大小（单位是byte）的常数。
// 类模板依然可以被用作类型函数，此时类型函数的参数是模板参数，
// 其结果被提取为成员类型或者成员常量，比如，sizeof运算符可以被作为如下接口提供：
template <typename T>
struct TypeSize {
  static std::size_t const value = sizeof(T);
};
// 这看上去可能没有那么有用，因为已经有了一个内置的sizeof运算符，
// 但是请注意此处的TypeSize<T>是一个类型，它可以被作为类模板参数传递。
// 或者说，TypeSize是一个模板，也可以被作为模板模板参数传递。

void func() {
  std::cout << TypeSize<int>::value << std::endl;     // 4
  std::cout << TypeSize<float>::value << std::endl;   // 4
  std::cout << TypeSize<double>::value << std::endl;  // 8
}
}  // namespace n1

namespace n2 {
// 19.3.1 元素类型（Element Type）

// 希望得到这样一个类型函数，当给的一个容器类型时，它可以返回相应的元素类型。
// 这可以通过偏特化实现：
template <typename T>
struct ElementT;
template <typename T>
struct ElementT<std::vector<T>> {
  using Type = T;
};
template <typename T>
struct ElementT<std::list<T>> {
  using Type = T;
};
template <typename T, std::size_t N>
struct ElementT<T[N]> {
  using Type = T;
};
template <typename T>
struct ElementT<T[]> {
  using Type = T;
};
// 注意此处应该为所有可能的数组类型提供偏特化。

template <typename T>
void printElementType(T const& c) {
  std::cout << "Container of " << typeid(typename ElementT<T>::Type).name()
            << " elements" << std::endl;
}

void func() {
  std::vector<bool> s;
  printElementType(s);  // Container of b elements

  std::list<float> f;
  printElementType(f);  // Container of f elements

  int arr[42];
  printElementType(arr);  // Container of i elements
}

// 偏特化的使用使得可以在容器类型不知道具体类型函数存在的情况下去实现类型函数。
// 有时类型函数是和其所适用的类型一起被设计的，此时相关实现就可以被简化。
// 比如，如果容器类型定义了value_type成员类型，标准库容器都会这么做：
template <typename C>
struct ElementT {
  using Type = typename C::value_type;
};
// 这个实现可以是默认实现，它不会排除没有定义成员类型value_type的容器的偏特化实现。

// 虽然如此，依然建议为类模板的类型参数提供相应的成员类型定义，
// 这样在泛型代码中就可以更容易的访问它们，和标准库容器的处理方式类似：
// template <typename T1, typename T2, …>
// class X {
//  public:
//   using … = T1;
//   using … = T2;
//   …
// };

// 那么类型函数的作用体现在什么地方呢？
// 它允许根据容器类型参数化一个模板，但是又不需要提供代表了元素类型和其它特性的参数。
// 比如，相比于使用：
template <typename T, typename C>
T sumOfElements(C const& c);
// 这一需要显式指定元素类型的模板sumOfElements<int> list，可以这样定义：
template <typename C>
typename ElementT<C>::Type sumOfElements(C const& c);
// 其元素类型是通过类型函数得到的。

// 注意观察萃取是如何被实现为已有类型的扩充的；
// 也就是说，甚至可以为基本类型和封闭库的类型定义类型函数。

// ElementT被称为萃取类，因为它被用来访问一个已有容器类型的萃取，
// 通常而言，在这样一个类中可以有多个萃取。
// 因此萃取类的功能并不仅限于描述容器参数的特性，而是可以描述任意主参数的特性。

// 为了方便，可以为类型函数创建一个别名模板，比如，可以引入：
template <typename T>
using ElementType = typename ElementT<T>::Type;

// 这可以让sumOfEkements的定义变得更加简单：
template <typename C>
ElementType<C> sumOfElements(C const& c);
}  // namespace n2

namespace n3 {
// 19.3.2 转换萃取（Transformation Traits）

// 除了可以被用来访问主参数类型的某些特性，萃取还可以被用来做类型转换，
// 比如为某个类型添加或移除引用、const以及volatile限制符。

namespace test1 {
// 删除引用

// 实现RemoveReferenceT萃取，用它将引用类型转换成其底层对象或者函数的类型，
// 对于非引用类型则保持不变：
template <typename T>
struct RemoveReferenceT {
  using Type = T;
};
template <typename T>
struct RemoveReferenceT<T&> {
  using Type = T;
};
template <typename T>
struct RemoveReferenceT<T&&> {
  using Type = T;
};
// 同样地，引入一个别名模板可以简化上述萃取的使用：
template <typename T>
using RemoveReference = typename RemoveReferenceT<T>::Type;

// 当类型是通过一个有时会产生引用类型的构造器获得的时候，删除引用会很有意义，
// 比如对于在第15.6节介绍的关于函数参数类型T&&的特殊推断规则。
// C++标准库提供了一个相应的std::remove_reference<>萃取。

void func() {
  int i = 1;
  int& r = i;
  std::cout << i << " " << r << std::endl;
  // 1 1

  decltype(i) r1 = i;                   // 变量
  decltype(r) r2 = r;                   // 引用
  RemoveReference<decltype(i)> r3 = i;  // 变量
  RemoveReference<decltype(r)> r4 = r;  // 变量，引用被删除
  std::cout << r1 << " " << r2 << " " << r3 << " " << r4 << std::endl;
  // 1 1 1 1

  // 修改i后，只有r2跟随一起改变：
  i = 2;
  std::cout << i << " " << r << std::endl;
  std::cout << r1 << " " << r2 << " " << r3 << " " << r4 << std::endl;
  // 2 2
  // 1 2 1 1

  // 只有r2的修改会改变i的值：
  r1 = 10;
  r2 = 20;
  r3 = 30;
  r4 = 40;
  std::cout << i << " " << r << std::endl;
  // 20 20
}
}  // namespace test1

namespace test2 {
// 添加引用

// 可以给一个已有类型添加左值或者右值引用：
template <typename T>
struct AddLValueReferenceT {
  using Type = T&;
};
template <typename T>
using AddLValueReference = typename AddLValueReferenceT<T>::Type;

template <typename T>
struct AddRValueReferenceT {
  using Type = T&&;
};
template <typename T>
using AddRValueReference = typename AddRValueReferenceT<T>::Type;

// 引用折叠的规则在这一依然适用（参见15.6节），比如AddLValueRef<int&&>，
// 返回的是int&，因此不需要对它们进行偏特化实现。

// 如果只实现AddLValueReferenceT和AddRValueReferenceT，
// 而又不对它们进行偏特化的话，最方便的别名模板可以被简化成下面这样：
// template <typename T>
// using AddLValueReferenceT = T&;
// template <typename T>
// using AddRValueReferenceT = T&&;
// 此时不通过类模板的实例化就可以对其进行实例化，因此称得上是一个轻量级过程。

// 但是这样做是由风险的，因此依然希望能够针对特殊的情况对这些模板进行特例化。
// 比如，如果适用上述简化实现，那么就不能将其用于void类型。
// 一些显式的特化实现可以被用来处理这些情况：
template <>
struct AddLValueReferenceT<void> {
  using Type = void;
};
template <>
struct AddLValueReferenceT<void const> {
  using Type = void const;
};
template <>
struct AddLValueReferenceT<void volatile> {
  using Type = void volatile;
};
template <>
struct AddLValueReferenceT<void const volatile> {
  using Type = void const volatile;
};
template <>
struct AddRValueReferenceT<void> {
  using Type = void;
};
template <>
struct AddRValueReferenceT<void const> {
  using Type = void const;
};
template <>
struct AddRValueReferenceT<void volatile> {
  using Type = void volatile;
};
template <>
struct AddRValueReferenceT<void const volatile> {
  using Type = void const volatile;
};
// 有了这些偏特化之后，上文中的别名模板必须被实现为类模板的形式，
// 不能适用最简单的那种形式，这样才能保证相应的偏特化在需要的时候被正确选取，
// 因为别名模板不能被特化。

// C++标准库中也提供了与之相应的类型萃取：
// std::add_lvalue_reference<>和std::add_rvalue_reference<>，
// 该标准模板也包含了对void类型的特化。

void f(int&) { std::cout << "left" << std::endl; }
void f(int&&) { std::cout << "right" << std::endl; }
void func() {
  int i;
  f(AddLValueReference<int>(i));  // left
  f(AddRValueReference<int>(i));  // right
}
}  // namespace test2

namespace test3 {
// 移除限制符

// 转换萃取可以分解或者引入任意种类的复合类型，并不仅限于引用。
// 比如，如果一个类型中存在const限制符，可以将其移除：
template <typename T>
struct RemoveConstT {
  using Type = T;
};
template <typename T>
struct RemoveConstT<T const> {
  using Type = T;
};
template <typename T>
using RemoveConst = typename RemoveConstT<T>::Type;

template <typename T>
struct RemoveVolatileT {
  using Type = T;
};
template <typename T>
struct RemoveVolatileT<T volatile> {
  using Type = T;
};
template <typename T>
using RemoveVolatile = typename RemoveVolatileT<T>::Type;

// 而且，转换萃取可以是多功能的，创建可以被用来移除const和volatile的萃取：
template <typename T>
struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type> {};
template <typename T>
using RemoveCV1 = typename RemoveCVT<T>::Type;
// RemoveCVT中有两个需要注意的地方：
// 1.它同时使用了RemoveConstT和相关的RemoveVolitleT，
//   首先移除类型中可能存在的volatile，然后将得到了类型传递给RemoveConstT；
// 2.它没有定义自己的和RemoveConstT中Type类似的成员，
//   而是通过使用元函数转发从RemoveConstT中继承了Type成员，
//   这里元函数转发被用来简单的减少RemoveCVT中的类型成员。

// RemoveCVT的别名模板可以被进一步简化成：
template <typename T>
using RemoveCV2 = RemoveConst<RemoveVolatile<T>>;
// 同样地，这一简化只适用于RemoveCVT没有被特化的情况。
// 但是和AddLValueReference以及AddRValueReference的情况不同的是，
// 没有一种对其进行特化的原因。

// C++标准库也提供了与之对应的std::remove_volatile<>，
// std::remove_const<>，以及std::remove_cv<>。

void func() {
  const int i1 = 5;

  decltype(i1) i2 = 6;
  // i2 = 6;

  decltype(RemoveCV1<int>(i1)) i3 = 7;
  i3 = 8;

  decltype(RemoveCV2<int>(i1)) i4 = 7;
  i4 = 8;
}
}  // namespace test3

namespace test4 {
// 退化（Decay）

// 为了使对转换萃取的讨论变得更完整，实现了模仿按值传递参数时的类型转化行为的萃取。
// 该类型转换继承自C语言，这意味着参数类型会发生退化，数组类型退化成指针类型，
// 函数类型退化成指向函数的指针类型，而且会删除相应的顶层const，
// volatile以及引用限制符，因为在解析函数调用时，会忽略掉参数类型中的顶层限制符。

template <typename T>
void f(T) {}

template <typename A>
void printParameterType(void (*)(A)) {
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << "Parameter type: " << typeid(A).name() << std::endl;
  std::cout << "- is int: " << std::is_same<A, int>::value << std::endl;
  std::cout << "- is const: " << std::is_const<A>::value << std::endl;
  std::cout << "- is pointer: " << std::is_pointer<A>::value << std::endl;
}

void func() {
  printParameterType(&f<int>);
  // Parameter type: i
  // - is int: true
  // - is const: false
  // - is pointer: false

  printParameterType(&f<int const>);
  // Parameter type: i
  // - is int: true
  // - is const: false
  // - is pointer: false

  printParameterType(&f<int[7]>);
  // Parameter type: Pi
  // - is int: false
  // - is const: false
  // - is pointer: true

  printParameterType(&f<int(int)>);
  // Parameter type: PFiiE
  // - is int: false
  // - is const: false
  // - is pointer: true

  // 除了int参数保持不变外，其余int const，int[7]，
  // 以及int(int)参数分别退化成了int，int*，以及int(*)(int)。
}
}  // namespace test4

void func1() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}

// 通过之前介绍的技术可以实现一个与C++标准库中的std::decay功能类似的萃取DecayT。
// 首先对非数组、非函数的情况进行定义，只需要删除const和volatile限制符即可：
template <typename T>
struct DecayT : test3::RemoveCVT<T> {};
// 然后处理数组到指针的退化，这需要用偏特化来处理所有的数组类型，有界和无界数组：
template <typename T>
struct DecayT<T[]> {
  using Type = T*;
};
template <typename T, std::size_t N>
struct DecayT<T[N]> {
  using Type = T*;
};
// 最后来处理函数到指针的退化，这需要应对所有的函数类型，
// 不管是什么返回类型以及有多数参数，为此，使用了变参模板：
template <typename R, typename... Args>
struct DecayT<R(Args...)> {
  using Type = R (*)(Args...);
};
template <typename R, typename... Args>
struct DecayT<R(Args..., ...)> {
  using Type = R (*)(Args..., ...);
};
// 上面第二个偏特化可以匹配任意使用了C-style可变参数的函数。

template <typename T>
void printDecayedType() {
  std::cout.setf(std::ios_base::boolalpha);
  using A = typename DecayT<T>::Type;
  std::cout << "Parameter type: " << typeid(A).name() << std::endl;
  std::cout << "- is int: " << std::is_same<A, int>::value << std::endl;
  std::cout << "- is const: " << std::is_const<A>::value << std::endl;
  std::cout << "- is pointer: " << std::is_pointer<A>::value << std::endl;
}

// 和往常一样，也提供了一个很方便的别名模板：
template <typename T>
using Decay = typename DecayT<T>::Type;

// C++标准库也提供了相应的类型萃取std::decay<>。

void func2() {
  printDecayedType<int>();
  // Parameter type: i
  // - is int: true
  // - is const: false
  // - is pointer: false

  printDecayedType<int const>();
  // Parameter type: i
  // - is int: true
  // - is const: false
  // - is pointer: false

  printDecayedType<int[7]>();
  // Parameter type: Pi
  // - is int: false
  // - is const: false
  // - is pointer: true

  printDecayedType<int(int)>();
  // Parameter type: PFiiE
  // - is int: false
  // - is const: false
  // - is pointer: true
}

void func() {
  func1();
  func2();
}
}  // namespace n3

namespace n4 {
// 19.3.3 预测型萃取（Predicate Traits）

// 通过为可能的输出结果true和false提供不同的类型，可以大大的提高对IsSameT的定义。
// 事实上，如果声明一个BoolConstant模板以及两个可能的实例TrueType和FalseType：
template <bool val>
struct BoolConstant {
  using Type = BoolConstant<val>;
  static constexpr bool value = val;
};
using TrueType = BoolConstant<true>;
using FalseType = BoolConstant<false>;
// 就可以基于两个类型是否匹配，让相应的IsSameT分别继承自TrueType和FalseType：
template <typename T1, typename T2>
struct IsSameT : FalseType {};
template <typename T>
struct IsSameT<T, T> : TrueType {};
// 现在IsSameT<T,int>的返回类型会被隐式的转换成其基类TrueType或者FalseType，
// 这样就不仅提供了相应的value成员，
// 还允许在编译期间将相应的需求派发到对应的函数实现或者类模板的偏特化上。
template <typename T>
void fooImpl(T, TrueType) {
  std::cout << "fooImpl(T,true) for int called" << std::endl;
}
template <typename T>
void fooImpl(T, FalseType) {
  std::cout << "fooImpl(T,false) for other type called" << std::endl;
}
template <typename T>
void foo(T t) {
  fooImpl(t, IsSameT<T, int>{});
}
void func() {
  foo(42);   // fooImpl(T,true) for int called
  foo(7.7);  // fooImpl(T,false) for other type called
}

// 这一技术被称为标记派发（tag dispatching）。

// BoolConstant中还有一个Type成员，这样就可以通过它为IsSameT引入一个别名模板：
template <typename T>
using isSame = typename IsSameT<T, T>::Type;
// 这里的别名模板可以和之前的变量模板isSame并存。

// 通常产生bool的萃取都应从诸如TrueType和FalseType的类型进行派生来支持标记派发。
// 但是为了尽可能的进行泛化，应该只有一个类型代表true，
// 也应该只有一个类型代表false，而不是让每一个泛型库都为bool型常量定义它自己的类型。

// 幸运的是，从C++11开始C++标准库在<type_traits>中提供了相应的类型：
// std::true_type和std::false_type。

// 在C++11和C++14中其定义如下：
// namespace std {
// using true_type = integral_constant<bool, true>;
// using false_type = integral_constant<bool, false>;
// }
// 在C++17中，其定义如下：
// namespace std {
// using true_type = bool_constant<true>;
// using false_type = bool_constant<false>;
// }
// 其中bool_constant的定义如下：
// namespace std {
// template<bool B>
// using bool_constant = integral_constant<bool, B>;
// }
}  // namespace n4

namespace n5 {
// 19.3.4 返回结果类型萃取（Result Type Traits）

namespace test1 {
// 另一个可以被用来处理多个类型的类型函数的例子是返回值类型萃取。
// 在编写操作符模板的时候它们会很有用。
// 为了引出这一概念，来写一个可以对两个Array容器求和的函数模板：
// template<typename T>
// Array<T> operator+ (Array<T> const&, Array<T> const&);

// 这看上去很好，但是由于语言本身允许对一个char型数值和一个整形数值求和，
// 自然也很希望能够对Array也执行这种混合类型（mixed-type）的操作。
// 这样就要处理该如何决定相关模板的返回值的问题：
// template<typename T1, typename T2>
// Array<???> operator+ (Array<T1> const&, Array<T2> const&);

// 除了在1.3介绍的各种方法外，另一个可以解决上述问题的方式就是返回值类型模板：
// template <typename T1, typename T2>
// Array<typename PlusResultT<T1, T2>::Type> operator+(Array<T1> const&,
//                                                     Array<T2> const&);
// 如果有便捷别名模板可用的话，还可以将其写称这样：
// template <typename T1, typename T2>
// Array<PlusResult<T1, T2>> operator+(Array<T1> const&, Array<T2> const&);

// 其中的PlusResultT萃取会自行判断：
// 通过+操作符对两种类型（可能是不同类型）的数值求和所得到的类型。
template <typename T1, typename T2>
struct PlusResultT {
  using Type = decltype(T1() + T2());
};
template <typename T1, typename T2>
using PlusResult = typename PlusResultT<T1, T2>::Type;
// 这一萃取模板通过使用decltype来计算表达式T1()+T2()的类型，
// 将决定结果类型这一艰巨的工作留给了编译器，
// 包括处理类型增进规则（promotion rules）和运算符重载。

// 但是对于例子而言， decltype却保留了过多的信息。
// 比如，PlusResultT可能会返回引用类型，但Array模板却很可能不是为引用类型设计的。
// 更为实际的例子是，重载的operator+可能会返回一个const类型的数值：
class Integer {};
Integer const operator+(Integer const&, Integer const&);
// 对两个Array<Integer>的值进行求和却得到了一个存储Integer const的Array，
// 不符合期望，事实上所期望的是将返回值类型中的引用和限制符移除之后所得到的类型：
// template<typename T1, typename T2>
// Array<RemoveCV<RemoveReference<PlusResult<T1, T2>>>>
// operator+ (Array<T1> const&, Array<T2> const&);

// 这一萃取的嵌套形式在模板库中很常见，在元编程中也经常被用到。
// 便捷别名模板在这一类多层级嵌套中会很有用。
// 如果没有它的话，就必须为每一级嵌套都增加一个typename和一个::Type。

// 到目前为止，数组的求和运算符可以正确地计算出：
// 对两个元素类型可能不同的Array进行求和的结果类型。
// 但是上述形式的PlusResultT却对元素类型T1和T2施加了一个不期望的限制：
// 由于表达式T1()+T2()试图对类型T1和T2的数值进行值初始化，
// 这两个类型必须要有可访问的、未被删除的默认构造函数或者是非class类型。
// Array本身可能并没有要求其元素可以被进行值初始化，这是额外的、不必要的限制。
}  // namespace test1

namespace test2 {
// 好在可以很简单的在不需要构造函数的情况下计算+表达式的值，
// 方法就是使用一个可以为一个给定类型T生成数值的函数。
// 为了这一目的，C++标准提供了std::declval<>，见11.2.3节。
// 在<utility>中其定义如下：
// namespace std {
// template <typename T>
// add_rvalue_reference_t<T> declval() noexcept;
// }
// declval<>可以在不需要使用默认构造函数或者其它任意操作的情况下为类型T生成一个值。

// 该函数模板被故意设计成未定义的状态，因为只希望它被用于decltype，
// sizeof或者其它不需要相关定义的上下文中，它有两个很有意思的属性：
// 1.对于可引用的类型，其返回类型总是相关类型的右值引用，
//   这能够使declval适用于那些不能够正常从函数返回的类型，
//   比如抽象类的类型（包含纯虚函数的类型）或者数组类型，
//   因此当被用作表达式时，从类型T到T&&的转换对declval<T>()的行为是没有影响的：
//   其结果都是右值（如果T是对象类型的话），对于右值引用，
//   其结果之所以不会变是因为存在引用塌缩；
// 2.在noexcept异常规则中提到，一个表达式不会因为使用了declval而被认成是会抛出异常的，
//   当declval被用在noexcept运算符上下文中时，这一特性会很有帮助。

// 有了declval，就可以不用在PlusResultT中使用值初始化了：
template <typename T1, typename T2>
struct PlusResultT {
  using Type = decltype(std::declval<T1>() + std::declval<T2>());
};
template <typename T1, typename T2>
using PlusResult = typename PlusResultT<T1, T2>::Type;
}  // namespace test2

void func() {}

// 返回值类型萃取提供了一种从特定操作中获取准确的返回值类型的方式，
// 在确定函数模板的返回值的类型的时候，它会很有用。
}  // namespace n5

int main(int argc, char* argv[]) {
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