#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

// 考虑使用函数对象而不是函数作为STL算法的参数

// 使用高级语言写程序的一个缺点是，随着抽象程度的提高，所生成的代码的效率却降低了。
// 对于标题感到惊讶，将函数对象传递给STL算法往往比传递实际的函数更加高效。

namespace n1 {
// 函数指针参数抑制了内联机制，这个事实正好解释了一个长期以来C程序员们都不愿正视的现实：
// C++的sort算法就性能而言总是优于C的qsort。
// 就算C++必须先实例化函数模板和类模板，然后再调用相应的operator()函数，
// 相比之下，C只是进行了一次简单的函数调用，但是C++所有的额外负担都在编译期间消化殆尽。
// 在运行时，sort算法以内联方式调用它的比较函数，而qsort则通过指针来调用它的比较函数。
// 使用包含一百万个double值的vector进行测试，sort比qsort快了670%。

inline bool doubleGreater(double d1, double d2) { return d1 > d2; }

void func() {
  std::vector<double> v;
  sort(v.begin(), v.end(), std::greater<double>());
  sort(v.begin(), v.end(), doubleGreater);
  // 使用greater<double>的sort调用比使用doubleGreater的sort调用快得多，因为函数内联。
  // 如果函数对象的operator()函数被声明为内联的，那么它的函数体将可以直接被编译器使用，
  // 大多数编译器都乐于在被调用算法的模板实例话过程中将该函数内联进去。
  // 最终结果是，sort中不包含函数调用，编译器就可以对这段不包含函数调用的代码进行优化，
  // 而这种优化在正常情况下是很难获得的。

  // 在C/C++中并不能真正地将一个函数作为参数传递给另一个函数，将一个函数作为参数进行传递，
  // 则编译器会隐式地将它转换成一个指向该函数的指针，由于传递的是函数指针，
  // 所有在sort内每次doubleGreater被用到的时候，编译器会产生一个间接的函数调用，
  // 即通过指针发出的调用，大多数编译器不会试图对通过函数指针执行的函数调用进行内联优化。
  // 可能是编译器厂商觉得这种优化不值得去做。
}
}  // namespace n1

namespace n2 {
// 除了效率还有一个理由使用函数对象，必须让程序正确地通过编译。
// 由于种种原因，STL平台可能会拒绝一些完全合法的代码，这样的情形并不罕见。
// 其中的原因可能是编译器的缺陷，也可能是由于STL库的原因，或者两者兼而有之。

// 函数对象优先于函数的第三个理由是，这样做有助于避免一些微妙的、语言本身的缺陷。
// 在偶然的情况下，有些看似合理的代码会被编译器以一些合法但又含糊不清的理由而拒绝。
// 例如，当一个函数模板的实例化名称并不完全等同于一个函数的名称时，就会出现这样的问题：
#if 0
template <typename FPType>
FPType average(FPType val1, FPType val2) {
  return (val1 + val2) / 2;
}
template <typename InputIter1, typename InputIter2>
void writeAverages(InputIter1 begin1, InputIter1 end1,
                   InputIter2 begin2, ostream& s) {
  transform(begin1, end1, begin2,
            ostream_iterator<typename
            iterator_traits<InputIter1>::value_type(
                s, "\n")>,
            average<typename iterator_traits<InputIter1>::value_type>);
}
#endif
// 许多编译接受这段代码，但是C++标准却不认同这样的代码。
// 因为理论上可能存在另一个名为average的函数模板，它也只带一个类型参数，有歧义。

// 使用函数对象类代替函数即可：
template <typename FPType>
struct Average : public std::binary_function<FPType, FPType, FPType> {
  FPType operator()(FPType val1, FPType val2) const {
    return average(val1, val2);
  }
};
template <typename InputIter1, typename InputIter2>
void writeAverages(InputIter1 begin1, InputIter1 end1, InputIter2 begin2,
                   std::ostream& s) {
  std::transform(
      begin1, end1, begin2,
      std::ostream_iterator<
          typename std::iterator_traits<InputIter1>::value_type>(s, "\n"),
      Average<typename std::iterator_traits<InputIter1>::value_type>());
}
// 所有的C++编译器都应该接受这段修正过的代码。
// 而且，transform内部的Average::operator()调用也符合内联优化的条件，
// 而前面代码中的average实例却无法内联优化，因为average是一个函数模板，不是函数对象。

void func() {
  std::set<std::string> s;
  // 代码合法，但是在一个被广泛使用的STL平台却无法通过编译：
  transform(s.begin(), s.end(),
            std::ostream_iterator<std::string::size_type>(std::cout, "\n"),
            std::mem_fun_ref(&std::string::size));
}
}  // namespace n2

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
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}
