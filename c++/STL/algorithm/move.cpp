#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

// move
// move_backward

// move()算法会将它的前两个输入迭代器参数指定的序列，
// 移到第三个参数定义的目的序列的开始位置，第三个参数必须是输出迭代器。
// 这个算法返回的迭代器指向最后一个被移动到目的序列的元素的下一个位置。
// 这是一个移动操作，因此无法保证在进行这个操作之后，输入序列仍然保持不变；
// 源元素仍然会存在，但它们的值可能不再相同了，
// 因此在移动之后，就不应该再使用它们。
// 如果源序列可以被替换或破坏，就可以选择使用move()算法。
// 如果不想扰乱源序列，可以使用copy()算法。

// 如果一个移动操作的目的地址位于源序列之内，
// move()就无法正常工作，这意味着移动需要从序列的右边开始。
// 原因是一些元素在移动之前会被重写，但move_backward()可以正常工作。
// 它的前两个参数指定了被移动的序列，第三个参数是目的地址的结束迭代器。

int main() {
  std::vector<int> srce{1, 2, 3, 4};
  std::deque<int> dest{5, 6, 7, 8};
  std::move(std::begin(srce), std::end(srce), std::back_inserter(dest));
  for (auto i = srce.begin(); i != srce.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;  // 1 2 3 4
  for (auto i = dest.begin(); i != dest.end(); ++i) {
    std::cout << *i << ' ';
  }
  std::cout << std::endl;  // 5 6 7 8 1 2 3 4

  std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8};
  std::move(std::begin(data) + 2, std::end(data), std::begin(data));
  data.erase(std::end(data) - 2, std::end(data));
  std::copy(std::begin(data), std::end(data),
            std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 3, 4, 5, 6, 7, 8

  return 0;
}