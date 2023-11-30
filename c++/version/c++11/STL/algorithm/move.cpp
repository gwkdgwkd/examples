#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>

// move
// move_backward

// 在C++11标准中，引入了许多算法和函数，以便更有效地操作和管理对象。
// 其中std::move和std::move_backward是两个非常有用的算法，
// 用于支持右值引用语义，提供高效的资源转移和移动语义，以减少不必要的拷贝操作。

namespace n1 {
// std::move用于将对象从一个位置移动到另一个位置，而不进行不必要的拷贝操作。
// 它实现了右值引用的语义，通过强制将左值转换为右值引用来实现资源的转移和移动语义。
// 这样可以在某些情况下降低资源使用量，提高代码效率。

// move()算法会将它的前两个输入迭代器参数指定的序列，
// 移到第三个参数定义的目的序列的开始位置，第三个参数必须是输出迭代器。
// 这个算法返回的迭代器指向最后一个被移动到目的序列的元素的下一个位置。
// 这是一个移动操作，因此无法保证在进行这个操作之后，输入序列仍然保持不变；
// 源元素仍然会存在，但它们的值可能不再相同了，因此移动后就不应该再用它们。
// 如果源序列可以被替换或破坏，可以用move()，如果不想扰乱源序列，用copy()。

void func1() {
  std::vector<int> source{1, 2, 3, 4, 5};
  std::vector<int> destination;

  // 移动元素
  for (auto&& element : source) {
    destination.push_back(std::move(element));
  }

  std::cout << "Source: ";
  std::copy(source.begin(), source.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // Source: 1 2 3 4 5

  std::cout << "Destination: ";
  std::copy(destination.begin(), destination.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // Destination: 1 2 3 4 5
}

void func2() {
  std::vector<int> v{1, 2, 3, 4};
  std::deque<int> q{5, 6, 7, 8};
  std::move(std::begin(v), std::end(v), std::back_inserter(q));

  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4

  std::copy(q.begin(), q.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 6 7 8 1 2 3 4
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
// 如果一个移动操作的目的地址位于源序列之内：
// move()就无法正常工作，因为是一些元素在移动之前会被重写，
// 这意味着移动需要从序列的右边开始，move_backward()可以正常工作。
// 它的前两个参数指定了被移动的序列，第三个参数是目的地址的结束迭代器。

void func() {
  std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8};
  std::move_backward(std::begin(data) + 2, std::end(data), std::begin(data) + 6);
  data.erase(std::end(data) - 2, std::end(data));
  std::copy(std::begin(data), std::end(data),
            std::ostream_iterator<int>{std::cout, " "});
  std::cout << std::endl;  // 3, 4, 5, 6, 7, 8
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