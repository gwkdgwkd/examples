#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <numeric>
#include <set>
#include <vector>

// 使用accumulate或者for_each进行区间统计

// count获取一个区间有多少个元素；
// count_if统计出满足某个判别式的元素个数；
// 区间中的最小值和最大值可以由min_element、max_element来获得。
// 有时候需要按照某种自定义的方式对区间进行统计处理，需要比上面更灵活的算法。
// accumulate就是这样的算法，
// 它和inner_product、adjacent_difference和partial_sum三个数值算法一样，位于<numeric>中。

std::string::size_type stringLengthSum(std::string::size_type sumSoFar,
                                       const std::string& s) {
  return sumSoFar + s.size();
}

struct Point {
  Point(double X, double Y) : x(X), y(Y) {}
  double x, y;
};
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

// for_each是另一个可被用来统计区间的算法，而且它不受accumulate的那些限制。
// 与accumulate一样，for_each也带两个参数：一个是区间，一个是函数（通常是函数对象）。
// 区间中的每个元素都要调用这个函数，但是，传给for_each的这个函数只接收一个实参（即当前的区间元素）。
// for_each执行完毕后会返回它的函数（返回的是一份拷贝），
// 重要的是，传给for_each的函数（及后来返回的函数）可以有副作用。
// 抛开副作用，for_each和accumulate在两个方面有所不同：
// 首先，accumulate暗示这个算法会计算出一个区间的统计信息。
// 而for_each听起来是对一个区间的每个元素做一个操作（主要应用）。
// 用for_each统计某个区间是合法的，但是不如accumulate来得清晰。
// 其次，accumulate直接返回我们要的统计结果，而for_each取返回一个函数对象，
// 必须从这个函数对象中提取出我们所要统计的信息。
// 在C++中，这意味着我们必须在函数子类中加入一个成员函数，以便获得我们想要的统计信息。
class PointAverage1 : public std::binary_function<Point, Point, Point> {
 public:
  PointAverage1() : xSum(0), ySum(0), numPoints(0) {}
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

// for_each的函数允许有副作用，而accumulate的函数参数却不允许呢？这是一个深层次的问题。

int main() {
  // accumulate有两种形式：
  // 第一种形式有两个迭代器和一个初始值，返回该初始值加上迭代器标识的区间中的值的总和：
  std::list<double> ld;
  double sum = accumulate(ld.begin(), ld.end(), 0.0);  // 初始值为0.0，不能是0
  // accumulate只要求输入迭代器，甚至可以使用istream_iterator和istreambuf_iterator:
  std::cout << "sum : "
            << std::accumulate(std::istream_iterator<int>(std::cin),
                               std::istream_iterator<int>(), 0)
            << std::endl;
  // 正是由于accumulate的这种默认行为，才使它被归为数值算法一类。

  // 第二种形式（一个初始值和一个任意的统计函数），更加通用：
  // 计算容器中字符串的长度总和：
  std::set<std::string> ss;
  std::string::size_type lengthSum =
      accumulate(ss.begin(), ss.end(), static_cast<std::string::size_type>(0),
                 stringLengthSum);
  // 计算区间中数值的乘积更加容易：
  std::vector<float> vf;
  float product =
      accumulate(vf.begin(), vf.end(), 1.0f, std::multiplies<float>());
  // 计算出一个区间中所有点的平均值：
  std::list<Point> lp;
  Point avg = accumulate(lp.begin(), lp.end(), Point(0, 0), PointAverage());
  // 上面的代码可以工作，但是标准中描述，传递给accumulate的函数不允许有副作用。
  // 而修改numPoints、xSum、ySum的值会带来副作用，所以从技术上说，很多专家不允许这么做。

  Point avg1 = for_each(lp.begin(), lp.end(), PointAverage1()).result();

  return 0;
}