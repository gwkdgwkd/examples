#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 了解哪些算法要求使用排序的区间作为参数

// 并非所有的算法都可以应用于任何区间。
// remove算法要求单向迭代器并且要求可以通过这些迭代器向容器中的对象赋值。
// 所以，它不能用于由输入迭代器指定的区间，也不适用与map或multimap，
// 同样不适用于某些set和mutliset的实现。
// 同样地，很多排序算法要求随机访问迭代器，
// 所以对于list的元素不可能调用这些算法。
// 有些算法的前提更微妙，最常见的是，要求区间是排过序的。
// 有写算法的区间，可以排序也可以不排序，但排序过的区间会更加有效。

// 要求排序的STL算法：
// binary_search、lower_bound、upper_bound、equal_range、
// set_union、set_intersection、set_difference、
// set_symmetric_difference、merge、inplace_merge、includes
// 并不一定要求排序的区间，但通常情况下与区间一起使用：
// unique、unique_copy

// binary_search、lower_bound、upper_bound、
// equal_range要求区间排序，因为用二分法查找数据。
// set_union、set_intersection、set_difference、
// set_symmetric_difference提供了线性时间效率的集合操作。
// 如果不满足排序条件，就无法在线性时间内完成工作。
// merge、inplace_merge实现了合并和排序的联合操作。
// 如果不是排序的区间，就不可能在线性时间内完成。
// includes用来判断一个区间中的所有对象是否都在另一个区间中。
// 承诺线性时间的效率，如果区间不排序，运行得更慢。
// unique、unique_copy即使对于未排序的区间也有很好的行为。

// 所有要求排序区间的算法unique、unique_copy除外，
// 均使用等价性来判断两个对象是否相同，这与标准关联容器一致。
// unique、unique_copy默认情况下使用相等来判断两个对象是否相同，
// 当然可以改变这种默认行为。

// 这11个算法之所以要求排序的区间，目的是为了提供更好的性能。
// 只要确保提供给它们的排序的区间，
// 并保证这些算法所用的比较函数与排序所使用的比较函数一致，
// 就可以有效地使用这些与查找、集合操作以及区间合并有关的算法，
// 并且你会惊喜地发现，unique和unique_copy如愿地删除了所有重复的值。

int main() {
  std::vector<int> v{1, 9, 8, 6, 3, 4, 5, 2, 7};
  std::sort(v.begin(), v.end(), std::greater<int>());  // 降序
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  bool a5Exists = binary_search(v.begin(), v.end(), 5);  // 假设区间是升序的
  std::cout << std::boolalpha << a5Exists << std::endl;  // false
  a5Exists = binary_search(v.begin(), v.end(), 5, std::greater<int>());
  std::cout << std::boolalpha << a5Exists << std::endl;  // true

  return 0;
}