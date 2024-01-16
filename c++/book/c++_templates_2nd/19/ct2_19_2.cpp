#include <iostream>
#include <iterator>

// 19.2 萃取还是策略以及策略类（Traits versus Policiesand Policies Classes）

int num[] = {1, 2, 3, 4, 5};

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

namespace n1 {
// 到目前为止并没有区分累积（accumulation）和求和（summation）。
// 但是也可以相像其它种类的累积，比如，可以对一组数值求积。
// 或者说，如果这些值是字符串的话，可以将它们连接起来。
// 即使是求一个序列中最大值的问题，也可以转化成一个累积问题。
// 在所有这些例子中，唯一需要变得的操作是accum()中的total+=*beg，
// 可以称这一操作为累积操作的一个策略（policy）。

class SumPolicy {  // SumPolicy是一个策略类，也就是一个通过预先商定好的接口
 public:
  template <typename T1, typename T2>
  static void accumulate(T1& total, T2 const& value) {
    total += value;
  }
};
class MultPolicy {  // 提供一个不同的策略对数值进行累积
 public:
  template <typename T1, typename T2>
  static void accumulate(T1& total, T2 const& value) {
    total *= value;
  }
};
template <typename T, typename Policy = SumPolicy,
          typename Traits = AccumulationTraits<T>>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end) -> decltype(typename Traits::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  using AccT = typename Traits::AccT;
  AccT total = Traits::zero();
  while (beg != end) {
    Policy::accumulate(total, *beg);
    ++beg;
  }
  return total;
}

void func() {
  std::cout << accum<int>(num, num + 5) << std::endl;              // 15
  std::cout << accum<int, SumPolicy>(num, num + 5) << std::endl;   // 15
  std::cout << accum<int, MultPolicy>(num, num + 5) << std::endl;  // 0

  // 虽然0能很好的满足求和的需求，但是却不适用于求乘积。
  // 这说明不同的萃取和策略可能会相互影响，也恰好强调了仔细设计模板的重要性。
}
}  // namespace n1

namespace n2 {
// 19.2.1 萃取和策略：有什么区别？（Traits and Policies: What’sthe Difference?）

// 可以设计一个合适的例子来证明策略只是萃取的一个特例。
// 相反地，也可以认为萃取只是编码了一个特定的策略。

// 对策略类这一名词的使用做如下限制：它们应该是编码了某种与其它模板参数大致独立的行为的类。
// 策略和萃取有很多相似之处，只是它们更侧重于行为，而不是类型。

// 引入了萃取技术的Nathan Myers则建议萃取类使用如下更为开放的定义：
// 一个用来代替模板参数的类，作为一个类，它整合了有用的类型和常量；
// 作为一个模板，它为实现一个可以解决所有软件问题的额外的中间层提供了方法。

// 总体而言，更倾向于使用如下稍微模糊的定义：
// 萃取代表的是一个模板参数的本质的、额外的属性；
// 策略代表的是泛型函数和类型（通常都有其常用地默认值）的可以配置的行为。

// 为了进一步阐明两者之间可能的差异，列出了如下和萃取有关的观察结果：
// 1.萃取在被当作固定萃取（fixed traits）的时候会比较有用，
//   比如，当其不是被作为模板参数传递的时候；
// 2.萃取参数通常都有很直观的默认参数，很少被重写，或者简单的说是不能被重写；
// 3.萃取参数倾向于紧密的依赖于一个或者多个主模板参数；
// 3.萃取在大多数情况下会将类型和常量结合在一起，而不是成员函数；
// 5.萃取倾向于被汇集在萃取模板中。

// 对于策略类，有如下观察结果：
// 1.策略类如果不是被作为模板参数传递的话，那么其作用会很微弱；
// 2.策略参数不需要有默认值，它们通常是被显式指定的，有些泛型组件通常会使用默认策略；
// 3.策略参数通常是和其它模板参数无关的；
// 4.策略类通常会包含成员函数；
// 5.策略可以被包含在简单类或者类模板中。

// 但是，两者之间并没有一个清晰的界限。
// 比如，C++标准库中的字符萃取就定义了一些函数行为，比如比较，移动和查找字符。
// 通过替换这些萃取，我们定义一个大小写敏感的字符类型，同时又可以保留相同的字符类型。
// 因此，虽然它们被称为萃取，但是它们的一些属性和策略确实有联系的。
}  // namespace n2

namespace n3 {
// 19.2.2 成员模板还是模板模板参数？
// Member Templates versus Template Template Parameters

// 为了实现累积策略（accumulation policy），
// 选择将SumPolicy和MultPolicy实现为有成员模板的常规类。
// 另一种使用类模板设计策略类接口的方式，此时就可以被当作模板模板参数使用。
// 比如，可以将SumPolicy重写为如下模板：
template <typename T1, typename T2>
class SumPolicy {
 public:
  static void accumulate(T1& total, T2 const& value) { total += value; }
};
template <typename T1, typename T2>
class MultPolicy {
 public:
  static void accumulate(T1& total, T2 const& value) { total *= value; }
};
template <typename T, template <typename, typename> class Policy = SumPolicy,
          typename Traits = AccumulationTraits<T>>
#if __cplusplus == 201103L
auto accum(T const* beg, T const* end) -> decltype(typename Traits::AccT()) {
#else
auto accum(T const* beg, T const* end) {
#endif
  using AccT = typename Traits::AccT;
  AccT total = Traits::zero();
  while (beg != end) {
    Policy<AccT, T>::accumulate(total, *beg);
    ++beg;
  }
  return total;
}

void func() {
  std::cout << accum<int>(num, num + 5) << std::endl;              // 15
  std::cout << accum<int, SumPolicy>(num, num + 5) << std::endl;   // 15
  std::cout << accum<int, MultPolicy>(num, num + 5) << std::endl;  // 0
}

// 相同的转化也可以被用于萃取参数。
// 这一主题的其它变体也是有可能的，比如，相比于显式的将Acct的类型传递给策略类型，
// 传递累积萃取并通过策略从萃取参数中获取类型，可能会更有优势一些。

// 通过模板模板参数访问策略类的主要优势是，
// 让一个策略类通过一个依赖于模板参数的类型携带一些状态信息会更容易一些，
// 比如static数据成员，在第一个方法中，static数据成员必须要被嵌入到一个成员类模板中。

// 但是，模板模板参数方法的一个缺点是，策略类必须被实现为模板，
// 而且模板参数必须和我们的接口所定义的参数一样。
// 这可能会使萃取本身的表达相比于非模板类变得更繁琐，也更不自然。
}  // namespace n3

namespace n4 {
// 19.2.3 结合多个策略以及/或者萃取（Combining Multiple Policies and/or Traits）

// 正如实现所展现的那样，萃取以及策略并不会完全摒除多模板参数的情况。
// 但是，它们确实将萃取和模板的数量降低到了易于管理的水平。
// 然后就有了一个很有意思的问题，该如何给这些模板参数排序？

// 一个简单的策略是，根据参数默认值被选择的可能型进行递增排序，也就是说，
// 越是有可能使用一个参数的默认值，就将其排的越靠后。
// 比如说，萃取参数通常要在策略参数后面，因为在客户代码中，策略更可能被重写。

// 如果不介意增加代码的复杂性的话，还有可以按照任意顺序指定非默认参数的方法，见第21.4。
}  // namespace n4

namespace n5 {
// 19.2.4 通过普通迭代器实现累积（Accumulation with General Iterators）\

// 另一个版本的accum()的实现，在该实现中添加了处理泛化迭代器的能力，
// 不再只是简单的指针，这是为了支持工业级的泛型组件。
// 有意思的是，依然可以用指针来调用这一实现，因为C++标准库提供了迭代器萃取。
// 此时就可以像下面这样定义最初版本的accum()了：
template <typename Iter>
auto accum(Iter start, Iter end) {
  using VT = typename std::iterator_traits<Iter>::value_type;
  VT total{};
  while (start != end) {
    total += *start;
  }
}

// 这里的std::iterator_traits包含了所有迭代器相关的属性。
// 由于存在一个针对指针的偏特化，这些萃取可以很方便的被用于任意常规的指针类型。
// 标准库对这一特性的支持可能会像下面这样：
// namespace std {
// template <typename T>
// struct iterator_traits<T*> {
//   using difference_type = ptrdiff_t;
//   using value_type = T;
//   using pointer = T*;
//   using reference = T&;
//   using iterator_category = random_access_iterator_tag;
// };
// }  // namespace std
// 但是，此时并没有一个适用于迭代器所指向的数值的累积的类型，
// 因此依然需要设计自己的AccumulationTraits。
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}