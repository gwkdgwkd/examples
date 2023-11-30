#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <set>
#include <vector>

// 使用accumulate或者for_each进行区间统计

// count：获取一个区间有多少个元素；
// count_if：统计出满足某个判别式的元素个数；
// min_element：获取区间中的最小值；
// max_element：获取区间中的最大值；
// accumulate：按照某种自定义的方式对区间进行统计处理。

// accumulate、inner_product、adjacent_difference和partial_sum都位于<numeric>。

struct Point {
  Point(double X, double Y) : x(X), y(Y) {}
  double x, y;
};

namespace n1 {
// accumulate有两种形式：

namespace test1 {
// 1.第一种形式有两个迭代器和一个初始值，返回该初始值加上迭代器标识的区间中的值的总和；
void func() {
  std::list<double> ld{1.4, 2.6};
  double sum = accumulate(ld.begin(), ld.end(), 0.0);  // 初始值为0.0，不能是0
  std::cout << sum << std::endl;                       // 4

  // accumulate只要求输入迭代器，甚至是istream_iterator和istreambuf_iterator：
  std::cout << "sum : "
            << std::accumulate(std::istream_iterator<int>(std::cin),
                               std::istream_iterator<int>(), 0)
            << std::endl;
  // sum : 1
  // 2
  // 3
  // 4
  // g
  // 10

  // 正是由于accumulate的这种默认行为，才使它被归为数值算法一类。
}
}  // namespace test1

namespace test2 {
// 2.第二种形式（一个初始值和一个任意的统计函数），更加通用。
std::string::size_type stringLengthSum(std::string::size_type sumSoFar,
                                       const std::string& s) {
  return sumSoFar + s.size();
}

void func1() {
  // 计算容器中字符串的长度总和：
  std::set<std::string> ss{"1", "22", "333", "4444"};
  std::string::size_type lengthSum =
      accumulate(ss.begin(), ss.end(), static_cast<std::string::size_type>(0),
                 stringLengthSum);
  std::cout << lengthSum << std::endl;  // 10

  // 计算区间中数值的乘积更加容易：
  std::vector<float> vf{1.2, 0.5};
  float product =
      accumulate(vf.begin(), vf.end(), 1.0f, std::multiplies<float>());
  std::cout << product << std::endl;  // 0.6
}

class PointAverage : public std::binary_function<Point, Point, Point> {
 public:
  PointAverage() : xSum(0), ySum(0), numPoints(0) {}
  const Point operator()(const Point& avgSoFar, const Point& p) {
    ++numPoints;
    xSum += p.x;
    ySum += p.y;
    return Point(xSum / numPoints, ySum / numPoints);
  }

 private:
  size_t numPoints;
  double xSum;
  double ySum;
};

void func2() {
  // 计算出一个区间中所有点的平均值：
  std::list<Point> lp{Point(1, 1), Point(3, 3)};
  Point avg = accumulate(lp.begin(), lp.end(), Point(0, 0), PointAverage());
  std::cout << avg.x << "," << avg.y << std::endl;  // 2,2
  // 上面的代码可以工作，但是标准中描述，传递给accumulate的函数不允许有副作用。
  // 而修改numPoints、xSum、ySum的会带来副作用，所以从技术上说，很多专家不允许这么做。
}
}  // namespace test2
}  // namespace n1

namespace n2 {
// for_each是另一个可被用来统计区间的算法，而且它不受accumulate的那些限制。
// 与accumulate一样，for_each也带两个参数，区间和函数（通常是函数对象），
// 区间中的每个元素都要调用这个函数，函数参数只接收一个实参（即当前的区间元素）。
// for_each执行完毕后会返回它的函数（返回的是一份拷贝），
// 重要的是，传给for_each的函数（及后来返回的函数）可以有副作用。

// 抛开副作用，for_each和accumulate在两个方面有所不同：
// 首先，accumulate暗示这个算法会计算出一个区间的统计信息；
// 而for_each听起来是对一个区间的每个元素做一个操作（主要应用）。

// 用for_each统计某个区间是合法的，但是不如accumulate来得清晰。
// 其次，accumulate直接返回统计结果，而for_each取返回一个函数对象，
// 必须从这个函数对象中提取出所要统计的信息。
// 在C++中，这意味着必须在函数子类中加入一个成员函数，以便获得我们想要的统计信息。
class PointAverage : public std::binary_function<Point, Point, Point> {
 public:
  PointAverage() : xSum(0), ySum(0), numPoints(0) {}
  void operator()(const Point& p) {
    ++numPoints;
    xSum += p.x;
    ySum += p.y;
  }

  Point result() const { return Point(xSum / numPoints, ySum / numPoints); }

 private:
  size_t numPoints;
  double xSum;
  double ySum;
};

void func() {
  std::list<Point> lp{Point(1, 1), Point(3, 3)};
  Point avg = for_each(lp.begin(), lp.end(), PointAverage()).result();
  std::cout << avg.x << "," << avg.y << std::endl;  // 2,2
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func1();
      break;
    case 2:
      n1::test2::func2();
      break;
    case 3:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}