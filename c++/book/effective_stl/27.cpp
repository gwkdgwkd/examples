#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>

// 使用distance和advance将容器的const_iterator转换成iterator

namespace n1 {
// 把const_iterator强制转换为iterator
typedef std::deque<int> IntDeuqe;
typedef IntDeuqe::iterator Iter;
typedef IntDeuqe::const_iterator ConstIter;

void func() {
  IntDeuqe d{1, 2, 3, 4};
  ConstIter c(d.cend() - 1);

  // 编译错误，没有从const_iterator到iterator的隐式转换：
  // Iter i(c);

  // 编译错误，不能将const_iterator强制转换为iterator：
  // Iter i(const_cast<Iter>(c));

  // 不能编译的原因是，const_iterator和iterator是完全不同的类。
  // 试图将一个种类型转换为另一种类型是毫无意义的。
  // reinterpret_cast、static_cast甚至C语言风格的类型转换也不能胜任。

  // 其他容器得到的结果也是一样的，也许vector或string强制转换能通过编译，
  // 但这非常特殊，大多数STL实现都会利用指针作为vector或string容器的迭代器。
  // vector<T>::iterator和vector<T>::const_iterator则分别被定义为：
  // T*和const T*。
  // string::iterator和string::cosnt_iterator则被定义为：
  // char*和const char*。
  // 从const_iterator到iterator的const_cast转换被最终解释成：
  // 从cosnt T*到T*的转换，因而编译和结果都正确，然而即便在这样的STL实现中，
  // reverse_iterator和const_reverse_iterator仍是真正的类，
  // 所以不能直接将const_reverse_iterator强制转换成reverse_iterator。
  // 这些STL为了便于调试，通常只会在Release下才使用指针表示vector和string的迭代器。
  // 所以即使对于vector和string容器，将cosnt迭代器强制转换成迭代器也是不可取的，
  // 因为这些代码的移植性是一个问题。

  // 安全的、可移植的，而且用不着涉及类型系统的强制转换的得到对应iterator的方法：
  Iter i(d.begin());
  // advance(i, distance(i, c));  // 编译不过，两个参数类型不一样
  // 显式指明distance所使用的类型参数，从而避免编译器来推断该类型的参数，排除二义性。
  std::advance(i, std::distance<ConstIter>(i, c));
  // 这种方法的效率取决与使用的迭代器，对于随机访问的迭代器，是常数时间操作；
  // 对于双向迭代器而言，是一个线性时间的操作。

  std::cout << *c << std::endl;  // 4
  // *c = 5;
  std::cout << *i << std::endl;  // 4

  // 所以应该重新设计，是否一定需要从const_iterator到iterator的转换呢？
  // 参考条款26，尽量用iterator来代替const或reverse型的迭代器。
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