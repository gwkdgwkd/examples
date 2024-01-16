#include <iostream>
#include <typeinfo>

// 第19章 萃取的实现

// 模板允许用多种类型对类和函数进行参数化，如果能够通过引入尽可能多的模板参数，
// 去尽可能多的支持某种类型或者算法的各个方面，那将是一件很吸引人的事情。
// 这样，模板化的代码就可以被针对客户的某种具体需求进行实例化。
// 但是从实际的角度来看，并不希望引入过多的模板参数来实现最大化的参数化。
// 因为让用户在客户代码中指出所有的模板参数是一件很繁杂的事情，
// 而且每增加一个额外的模板参数，都会使模板和客户代码之间的协议变得更加复杂。

// 幸运的是，时间证明大部分被引入的额外的模板参数都有合理的默认值。
// 在一些情况下，额外的模板参数可以完全由很少的、主要的模板参数决定，
// 接下来会看到，这一类额外的模板参数可以被一起省略掉，在大多数情况下，
// 其它一些参数的默认值可以从主模板参数得到，但是针对特殊应用默认值需要偶尔被重载。
// 当然也有一些参数和主模板参数无关：
// 在某种意义上，它们是其自身的主模板参数，只不过它们有适用于大多数情况的默认值。

// 萃取（或者叫萃取模板，traits/traits template）是 C++编程的组件，
// 它们对管理那些在设计工业级应用模板时所需要管理的多余参数很有帮助。

// 19.1 一个例子：对一个序列求和

// 计算一个序列中所有元素的和是一个很常规的任务。
// 也正是这个简单的问题，提供了一个很好的、可以用来介绍各种不同等级的萃取应用的例子。

int num[] = {1, 2, 3, 4, 5};
char name[] = "templates";
int length = sizeof(name) - 1;

namespace n1 {
// 19.1.1 固定的萃取（Fixed Traits）

// 先来考虑这样一种情况：待求和的数据存储在一个数组中，
// 然后有一个指向数组中第一个元素的指针，和一个指向最后一个元素的指针。

namespace test1 {
template <typename T>
T accum(T const* beg, T const* end) {
  // 如何创建一个类型正确的零值来作为求和的起始值？
  // total对象要么被其默认值初始化，要么被零（zero）初始化，
  // 零初始化时对应指针是用nullptr初始化，对应bool值是用false初始化：
  T total{};
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}

void func() {
  std::cout << "the average value of the integer values is "
            << accum(num, num + 5) / 5 << std::endl;
  // the average value of the integer values is 3

  std::cout << "the average value of the characters in \"" << name << "\" is "
            << accum(name, name + length) / length << std::endl;
  // the average value of the characters in "templates" is -5

  // 对于templates，结果应该是a到z之间的某一个值，a的ASCII是97，
  // z的ASCII是122，因此应该够得到一个介于97和122之间的返回值。
  // 但模板是被char实例化的，其数值范围即使是被用来存储相对较小的数值的和也是不够的。
}
}  // namespace test1

namespace test2 {
// 为了解决test1的问题应该引入额外的模板参数AccT，并将其用于返回值total的类型：
template <typename AccT, typename T>
AccT accum(T const* beg, T const* end) {
  AccT total{};
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}

void func() {
  std::cout << "the average value of the integer values is "
            << accum<int>(num, num + 5) / 5 << std::endl;
  // the average value of the integer values is 3

  std::cout << "the average value of the characters in \"" << name << "\" is "
            << accum<int>(name, name + length) / length << std::endl;
  // the average value of the characters in "templates" is 108
}
}  // namespace test2

namespace test3 {
// test2的方案会增加模板用户的负担：在调用这一模板的时候，他们必须额外指定一个类型。
// 这并不是一个过于严苛的要求，但是确实是可以避免的。

// 一个可以避免使用额外的模板参数的方式是：
// 在每个被用来实例化accum()的T和与之对应的应该被用来存储返回值的类型之间建立某种联系。
// 这一联系可以被认为是T的某种属性，可以通过模板的偏特化建立这种联系：
template <typename T>
struct AccumulationTraits;
template <>
struct AccumulationTraits<char> {
  using AccT = int;
};
template <>
struct AccumulationTraits<short> {
  using AccT = int;
};
template <>
struct AccumulationTraits<int> {
  using AccT = long;
};
template <>
struct AccumulationTraits<unsigned int> {
  using AccT = unsigned long;
};
template <>
struct AccumulationTraits<float> {
  using AccT = double;
};
// AccumulationTraits模板被称为萃取模板，因为它是提取了其参数类型的特性。
// 通常而言可以有不只一个萃取，也可以有不只一个参数。
// 不对模板进行泛型定义，因为在不了解类型的时候，无法为其求和的类型做出很好的选择。
// 但是，可能有人会辩解说T类型本身就是最好的待选类型，但很显然对于前面的例子不是这样。

template <typename T>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end)
    -> decltype(typename AccumulationTraits<T>::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  using AccT = typename AccumulationTraits<T>::AccT;
  AccT total{};
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}

void func() {
  std::cout << "the average value of the integer values is "
            << accum(num, num + 5) / 5 << std::endl;
  // the average value of the integer values is 3

  std::cout << "the average value of the characters in \"" << name << "\" is "
            << accum(name, name + length) / length << std::endl;
  // the average value of the characters in "templates" is 108
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n1

namespace n2 {
// 19.1.2 值萃取（Value Traits）

// n11中的萃取，代表的都是特定主类型的额外的类型信息。
// 额外的信息并不仅限于类型信息，还可以将常量以及其它数值类和一个类型关联起来。

namespace test1 {
// 在n11的模板中，使用默认构造函数对返回值进行了初始化，希望将其初始化为零。
// 但并不能保证一定会生成一个合适的初始值，因为AccT可能根本就没有默认构造函数。
// 萃取可以再一次被用来救场，可以为AccumulationTraits添加一个新的值萃取：
template <typename T>
struct AccumulationTraits;
template <>
struct AccumulationTraits<char> {
  using AccT = int;
  static AccT const zero = 0;  // 提供了可以在编译期间计算的，const的zero成员
};
template <>
struct AccumulationTraits<short> {
  using AccT = int;
  static AccT const zero = 0;
};
template <>
struct AccumulationTraits<int> {
  using AccT = long;
  static AccT const zero = 0;
};
template <>
struct AccumulationTraits<float> {
  using Acct = float;
  // constexpr的static会稍好些，允许对float以及其它字面值类型进行类内初始化：
  static constexpr float zero = 0.0f;
};

// 但是无论是const还是constexpr都禁止对非字面值类型进行这一类初始化。
// 比如，一个用户定义的任意精度的BigInt类型，可能就不是字面值类型，
// 因为它可能会需要将一部分信息存储在堆上，这会阻碍其成为一个字面值类型，
// 或者是因为所需要的构造函数不是constexpr的：
class BigInt {
 public:
  BigInt(long long) {}
};
template <>
struct AccumulationTraits<BigInt> {
  using AccT = BigInt;
  // 构造函数不是constexpr的，不能编译：
  // static constexpr BigInt zero = BigInt{0};

#if __cplusplus < 201703L
  // 一个比较直接的解决方案是，不再类中定义值萃取，只做声明：
  static BigInt const zero;
#else
// 在C++17中，可以通过使用inline变量来解决这一问题：
// inline static BigInt const zero = BigInt{0};
#endif
};
#if __cplusplus < 201703L
// 然后在源文件中对其进行初始化：
BigInt const AccumulationTraits<BigInt>::zero = BigInt{0};
// 这样虽然可以工作，但是却有些麻烦，必须在两个地方同时修改代码，
// 这样可能还会有些低效，因为编译期通常并不知晓在其它文件中的变量定义。
#endif

template <typename T>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end)
    -> decltype(typename AccumulationTraits<T>::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  using AccT = typename AccumulationTraits<T>::AccT;

  // 存储求和结果的临时变量的初始化依然很直观：
  AccT total = AccumulationTraits<T>::zero;
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}
}  // namespace test1

namespace test2 {
// C++17之前的另一种办法是，对那些不是总是生成整型值的值萃取，使用inline成员函数。
// 同样的，如果成员函数返回的是字面值类型，可以将该函数声明为constexpr的。
template <typename T>
struct AccumulationTraits;
template <>
struct AccumulationTraits<char> {
  using AccT = int;
  static constexpr AccT zero() { return 0; }
};
template <>
struct AccumulationTraits<short> {
  using AccT = int;
  static constexpr AccT zero() { return 0; }
};
template <>
struct AccumulationTraits<int> {
  using AccT = long;
  static constexpr AccT zero() { return 0; }
};
template <>
struct AccumulationTraits<unsigned int> {
  using AccT = unsigned long;
  static constexpr AccT zero() { return 0; }
};
template <>
struct AccumulationTraits<float> {
  using AccT = double;
  static constexpr AccT zero() { return 0; }
};
class BigInt {
 public:
  BigInt(long long) {}
};
template <>
struct AccumulationTraits<BigInt> {
  using AccT = BigInt;
  static BigInt zero() { return BigInt{0}; }
};

template <typename T>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end)
    -> decltype(typename AccumulationTraits<T>::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  using AccT = typename AccumulationTraits<T>::AccT;

  // 唯一的区别是函数的调用语法，不像访问一个static数据成员那么简洁：
  AccT total = AccumulationTraits<T>::zero();
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}

void func() {
  std::cout << "the average value of the integer values is "
            << accum(num, num + 5) / 5 << std::endl;
  // the average value of the integer values is 3

  std::cout << "the average value of the characters in \"" << name << "\" is "
            << accum(name, name + length) / length << std::endl;
  // the average value of the characters in "templates" is 108
}
}  // namespace test2

void func() { test2::func(); }

// 很明显，萃取可以不只是类型。
// 在例子中，萃取可以是一种能够提供所有在调用accum()时所需的调用参数的信息的技术。
// 这是萃取这一概念的关键：
// 萃取为泛型编程提供了一种配置（configure）具体元素（通常是类型）的手段。
}  // namespace n2

namespace n3 {
// 19.1.3 参数化的萃取

// 在n1和n2的accum()里使用的萃取被称为固定的（fixed），
// 这是因为一旦定义了解耦合萃取，在算法中它就不可以被替换。
// 但是在某些情况下，这一类重写（overriding）行为却又是所期望的。
// 比如，可能碰巧知道某一组float数值的和可以被安全地存储在一个float变量中，
// 而这样做可能又会带来一些性能的提升。

using n2::test2::AccumulationTraits;

// 为了解决这一问题，可以为萃取引入一个新的模板参数AT，其默认值由萃取模板决定：
template <typename T, typename AT = AccumulationTraits<T>>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end) -> decltype(typename AT::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  typename AT::AccT total = AT::zero();
  while (beg != end) {
    total += *beg;
    ++beg;
  }
  return total;
}

void func() {
  std::cout << "the average value of the integer values is "
            << accum(num, num + 5) / 5 << std::endl;
  // the average value of the integer values is 3

  std::cout << "the average value of the characters in \"" << name << "\" is "
            << accum(name, name + length) / length << std::endl;
  // the average value of the characters in "templates" is 108

  float fa[] = {1.0f, 2.0f, 3.0f};
  float* p = fa + 3;
  std::cout << accum(fa, p) << std::endl;                                  // 6
  std::cout << accum<float>(fa, p) << std::endl;                           // 6
  std::cout << accum<float, AccumulationTraits<int>>(fa, p) << std::endl;  // 6

  std::cout << typeid(accum(fa, p)).name() << std::endl;         // d
  std::cout << typeid(accum<float>(fa, p)).name() << std::endl;  // d
  std::cout << typeid(accum<float, AccumulationTraits<int>>(fa, p)).name()
            << std::endl;  // l

  // 采用这种方式，一部分用户可以忽略掉额外模板参数，
  // 而对于那些有着特殊需求的用户，他们可以指定一个新的类型来取代默认类型。
  // 但是可以推断，大部分的模板用户永远都不需要显式的提供第二个模板参数，
  // 因为可以为第一个模板参数的每一种（通过推断得到的）类型都配置一个合适的默认值。
}
}  // namespace n3

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