#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 理解copy_if算法的正确实现

// 11包含copy的算法：
// copy、copy_backward、replace_copy、reverse_copy、
// replace_copy_if、unique_copy、remove_copy、rotate_copy、
// remove_copy_if、partial_sort_copy、uninitialized_copy
// 但是没有copy_if，简单地复制区间中满足某个判别式的所以元素，需要自己实现。

class Widget {
 public:
  Widget(int i = 0) : i_(i) {}
  friend std::ostream &operator<<(std::ostream &os, const Widget &abook);
  bool isTrue() const { return i_ <= 3; }

 private:
  int i_;
};
std::ostream &operator<<(std::ostream &os, const Widget &pt) {
  os << pt.i_ << std::flush;
  return os;
}
bool isDefective(const Widget &w) { return w.isTrue(); }

namespace n1 {
// STL中没有copy_if，copy_if的价值不大，但正确地实现copy_if算法并不容易。
template <typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end,
                       OutputIterator destBegin, Predicate p) {
  // 虽然STL不允许复制所有使判别式为真的元素，但允许复制所有使判别式条件不为真的元素：
  return remove_copy_if(begin, end, destBegin, std::not1(p));
}

void func() {
  std::vector<Widget> widgets;

  // n1::copy_if(widgets.begin(), widgets.end(),
  //             std::ostream_iterator<Widget>(std::cerr, "\n"), isDefective);
  // 编译不过，试图把not1应用到isDefective上，not1不能被直接用到函数指针上（条款41），
  // 函数指针必须首先用ptr_fun进行转换，为了调用copy_if，传入的不仅是一个函数对象，
  // 而且还应该是一个可配接的函数对象，很容易做到，
  // 但是STL算法从不要求它们的函数子必须是可配接的，copy_if也不应例外。
}
}  // namespace n1

namespace n2 {
// 正确的实现：
template <typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator begin, InputIterator end,
                       OutputIterator destBegin, Predicate p) {
  while (begin != end) {
    if (p(*begin)) {
      *destBegin++ = *begin++;
    }
  }
  return destBegin;
}

void func() {
  std::vector<Widget> widgets{Widget(),  Widget(0), Widget(1),
                              Widget(2), Widget(3), Widget(4)};
  n2::copy_if(widgets.begin(), widgets.end(),
              std::ostream_iterator<Widget>(std::cout, " "), isDefective);
  // 0 0 1 2 3^C
}
}  // namespace n2

int main(int argc, char *argv[]) {
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
