#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

// fill
// fill_n

// fill()和fill_n()算法提供了一种为元素序列填入给定值的简单方式，
// fill()会填充整个序列；
// fill_n()则以给定的迭代器为起始位置，为指定个数的元素设置值。
// fill的前两个参数是定义序列的正向迭代器，第三个参数是赋给每个元素的值。
// 当然这个序列并不一定要代表容器的全部元素。
// fill_n()的参数分别是指向被修改序列的第一个元素的正向迭代器、
// 被修改元素的个数以及要被设置的值。

int main() {
  std::vector<std::string> data{6};
  std::fill(std::begin(data), std::end(data), "none");
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // none none none none none none

  std::deque<int> values(13);
  int n{2};
  const int step{7};
  const size_t count{3};
  auto iter = std::begin(values);
  while (true) {
    auto to_end = std::distance(iter, std::end(values));
    if (to_end < count) {
      std::fill(iter, iter + to_end, n);
      break;
    } else {
      std::fill(iter, std::end(values), n);
    }
    iter = std::next(iter, count);
    n += step;
  }
  std::copy(values.begin(), values.end(),
            std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 2 2 2 9 9 9 16 16 16 23 23 23 30

  std::fill_n(std::begin(data), 4, "one");
  std::copy(data.begin(), data.end(),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // one one one one none none

  return 0;
}