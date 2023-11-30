#include <math.h>

#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

// transform
// accumulate

// transform()可以将函数应用到序列的每个元素上，并将返回值保存到另一个序列中，
// 它返回的迭代器指向输出序列所保存的最后一个元素的下一个位置。
// 有一个版本和for_each()相似，将一元函数应用到元素序列上来改变它们的值，
// 但这里有很大的区别，for_each()中使用的函数的返回类型必须为void，
// 而且可以通过这个函数的引用参数来修改输入序列中的值；
// 而transform()的二元函数必须返回一个值，
// 并且也能够将应用函数后得到的结果保存到另一个序列中。
// 不仅如此，输出序列中的元素类型可以和输入序列中的元素类型不同。
// 对于for_each()，函数总是会被应用序列的元素上，但对于transform()，无法保证。

// 应用二元函数的这个版本的transform()含有5个参数：
// 1.前两个参数是第一个输入序列的输入迭代器。
// 2.第3个参数是第二个输入序列的开始迭代器，显然，
//   这个序列必须至少包含和第一个输入序列同样多的元素。
// 3.第4个参数是一个序列的输出迭代器，
//   它所指向的是用来保存应用函数后得到的结果的序列的开始迭代器。
// 4.第5个参数是一个函数对象，它定义了一个接受两个参数的函数，
// 这个函数接受来自两个输入序列中的元素作为参数，返回一个可以保存在输出序列中的值。

namespace n1 {
std::vector<double> deg_C{21.0, 30.5, 0.0, 3.2, 100.0};
void func1() {
  std::vector<double> deg_F(deg_C.size());
  std::transform(std::begin(deg_C), std::end(deg_C), deg_F.rbegin(),
                 [](double temp) { return 32.0 + 9.0 * temp / 5.0; });
  std::copy(deg_F.begin(), deg_F.end(),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 212 37.76 32 86.9 69.8
}

void func2() {
  std::vector<double> deg_F;
  std::transform(std::begin(deg_C), std::end(deg_C), std::back_inserter(deg_F),
                 [](double temp) { return 32.0 + 9.0 * temp / 5.0; });
  copy(deg_F.begin(), deg_F.end(),
       std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 69.8 86.9 32 37.76 212
}

void func3() {
  std::transform(std::begin(deg_C), std::end(deg_C), std::begin(deg_C),
                 [](double temp) { return 32.0 + 9.0 * temp / 5.0; });
  copy(deg_C.begin(), deg_C.end(),
       std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;  // 69.8 86.9 32 37.76 212
}
}  // namespace n1

namespace n2 {
void func() {
  std::vector<std::string> words{"one", "two", "three", "four", "five"};
  std::vector<size_t> hash_values;
  std::transform(std::begin(words), std::end(words),
                 std::back_inserter(hash_values), std::hash<std::string>());
  std::copy(std::begin(hash_values), std::end(hash_values),
            std::ostream_iterator<size_t>{std::cout, " "});
  std::cout << std::endl;
  // 16780311998597636082 4939359993625789802 11764894015389861952
  // 9499269394060476006 12528687628377246665

  // 因为目的序列是由back_insert_iterator对象指定的，transform()会返回一个：
  // back_insert_iterator<vector<size_T>>类型的迭代器，
  // 因此不能在copy()算法中用它作为输入序列的结束迭代器。
  // 为了充分利用transform()返回的迭代器，这段代码可以这样写：
  auto end_iter =
      std::transform(std::begin(words), std::end(words),
                     std::begin(hash_values), std::hash<std::string>());
  std::copy(std::begin(hash_values), end_iter,
            std::ostream_iterator<size_t>{std::cout, " "});
  std::cout << std::endl;
  // 16780311998597636082 4939359993625789802 11764894015389861952
  // 9499269394060476006 12528687628377246665
}
}  // namespace n2

namespace n3 {
void func() {
  // 可以在transform()所运用的函数中为元素序列调用一个算法：
  std::deque<std::string> names{"Stan Laurel", "Oliver Hardy", "Harold Lloyd"};
  std::transform(std::begin(names), std::end(names), std::begin(names),
                 [](std::string& s) {
                   std::transform(std::begin(s), std::end(s), std::begin(s),
                                  ::toupper);
                   return s;
                 });
  std::copy(std::begin(names), std::end(names),
            std::ostream_iterator<std::string>{std::cout, " "});
  std::cout << std::endl;  // STAN LAUREL OLIVER HARDY HAROLD LLOYD
}
}  // namespace n3

namespace n4 {
void func() {
  using Point = std::pair<double, double>;
  std::vector<Point> hexagon{{1, 2}, {2, 1}, {3, 1}, {4, 2},
                             {3, 3}, {2, 3}, {1, 2}};
  std::vector<double> segments;
  std::transform(
      std::begin(hexagon), std::end(hexagon) - 1, std::begin(hexagon) + 1,
      std::back_inserter(segments), [](const Point p1, const Point p2) {
        return std::sqrt((p1.first - p2.first) * (p1.first - p2.first) +
                         (p1.second - p2.second) * (p1.second - p2.second));
      });
  std::cout << "Segment lengths: ";
  std::copy(std::begin(segments), std::end(segments),
            std::ostream_iterator<double>{std::cout, " "});
  std::cout << std::endl;
  // Segment lengths: 1.41421 1 1.41421 1.41421 1 1.41421

  // accumulate()函数可以求出segments中元素值之和，从而得到周长：
  std::cout << "Hexagon perimeter: "
            << std::accumulate(std::begin(segments), std::end(segments), 0.0)
            << std::endl;
  // Hexagon perimeter: 7.65685
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n1::func3();
      break;
    case 3:
      n2::func();
      break;
    case 4:
      n3::func();
      break;
    case 5:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}