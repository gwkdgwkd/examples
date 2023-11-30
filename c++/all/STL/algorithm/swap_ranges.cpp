#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// swap
// swap_ranges
// iter_swap

namespace n1 {
// 定义在utility头文件中的swap()算法的重载函数的模板原型为：
// template<typename T1, typename T2>
// void swap(std::pair<T1,T2> left, std::pair<T1,T2> right);
// 除了pair之外，utility文件头中也有可以交换任何类型的容器对象的模板的重载。
// 也就是说，可以交换两个list<T>容器或者两个set<T>容器，
// 但不能是一个list<T>和vector<T>，也不能是一个list<T1>和一个list<T2>。
// 另一个swap()模板的重载可以交换两个相同类型的数组。
// 也有其他swap()的重载，它们可以用来交换其他类型的对象，包含元组和智能指针类型。

void func() {
  int a = 10;
  int b = 20;
  std::cout << "before a = " << a << ", b = " << b << std::endl;
  std::swap(a, b);
  std::cout << "after a = " << a << ", b = " << b << std::endl;
  // before a = 10, b = 20
  // after a = 20, b = 10

  int a2[5] = {10, 20, 30, 40, 50};
  int b2[5] = {15, 25, 35, 45, 55};
  std::vector<int> v1(a2, a2 + 5);
  std::vector<int> v2(b2, b2 + 5);
  std::cout << "before vector1 = ";
  copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // before vector1 = 10 20 30 40 50
  std::cout << "before vector2 = ";
  copy(v2.begin(), v2.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // before vector2 = 15 25 35 45 55

  std::swap(v1, v2);

  std::cout << "after vector1 = ";
  copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // after vector1 = 15 25 35 45 55
  std::cout << "after vector2 = ";
  copy(v2.begin(), v2.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // after vector2 = 10 20 30 40 50

  // std::swap(v1, b);  // 报错
}
}  // namespace n1

namespace n2 {
// 可以用swap_ranges()来交换两个序列，这个算法需要3个正向迭代器作为参数。
// 前两个分别是第一个序列的开始和结束，第三个参数是第二个序列的开始迭代器。
// 显然，这两个序列的长度必须相同。
// 这个算法会返回一个迭代器，它指向第二个序列的最后一个被交换元素的下一个位置。
void func() {
  int a[5] = {1, 2, 3, 4, 5};
  int b[5] = {6, 7, 8, 9, 10};
  std::cout << "before a[5] = ";
  std::copy(a, a + 5, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // before a[5] = 1 2 3 4 5
  std::cout << "before b[5] = ";
  std::copy(b, b + 5, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // before b[5] = 6 7 8 9 10

  std::swap_ranges(a, a + 3, b);

  std::cout << "after a[5] = ";
  std::copy(a, a + 5, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // after a[5] = 6 7 8 4 5
  std::cout << "after b[5] = ";
  std::copy(b, b + 5, std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // after b[5] = 1 2 3 9 10
}
}  // namespace n2

namespace n3 {
// iter_swap()算法有一些不同，它会交换两个正向迭代器所指向的元素。
void func() {
  std::vector<int> v{1, 2, 3, 4, 5, 6};
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 6

  std::vector<int>::iterator l = v.begin();
  std::vector<int>::iterator r = --v.end();
  while (l < r) {
    std::iter_swap(l, r);
    ++l;
    --r;
  }

  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 6 5 4 3 2 1

  l = v.begin();
  r = --v.end();
  while (l < r) {
    std::swap(*l, *r);
    ++l;
    --r;
  }
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 6
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