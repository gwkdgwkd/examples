#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 了解各种与排序有关的选择

class Widget {};
bool qualityCompare(const Widget &lhs, const Widget &rhs) { return false; }
// sort是一个非常不错的算法，但也并非在任何场合下都是完美无缺的。
// 只需排序出前20好的，可以使用partial_sort。
// 选出最好的几个，可以使用nth_element。
// partial_sort和nth_element效果基本相同，不同的是partial_sort排序了，nth_element没有排序。

// 对于等价的元素，partial_sort和nth_element有自己的做法，无法控制它们的行为。
// partial_sort、nth_element和sort都属于非稳定的排序算法，stable_sort可以提供稳定排序。
// partial_sort和nth_element没有对应的稳定排序。

// partition算法可以把满足某个特定条件的元素放在区间的前部。
// 如果相对位置非常重要，那么可以使用stable_partition。
bool hasAcceptableQuality(const Widget &w) { return false; }

// partial_sort、nth_element、sort和stable_sort算法都要求随机访问迭代器，
// 所以这些算法只能被用于vector、string、deque和数组。
// 对于关联容器进行排序并没有实际意义。list是唯一需要排序却无法使用这些排序算法的容器。
// 为此，list特别提供了sort成员函数（稳定排序）。
// 如果需要在list中使用partial_sort、nth_element，只能使用间接的方法：
// 1.将list中的元素拷贝到一个提供随机访问的迭代器容器中，然后对该容器执行期望的算法；
// 2.先创建一个list::iterator的容器，在对容器执行相应的算法，然后通过其中的迭代器访问list的元素；
// 3.利用一个包含迭代器的有序容器中的信息，通过反复地调用splice成员函数，将list中的元素跳转到期望的目标位置。

// 与partial_sort、nth_element、sort和stable_sort不同的是，
// partition和stable_partition只要求双向迭代器就能完成工作。
// 所以对标准序列容器，都可以使用partition和stable_partition。

// 总结：
// 1.如果需要对vector、string、deque或数组进行完全排序，那么可以使用sort或者stable_sort。
// 2.如果有一个vector、string、deque或数组，并且只需要对等价性最前面的n个元素进行排序，
//   那么可以使用partial_sort。
// 3.如果有一个vector、string、deque或数组，并且需要找到第n个位置上的元素，
//   或者需要找到等价性最前面的n个元素但有不必对这n个元素进行排序，那使用nth_element。
// 4.如果需要将一个标准序列容器中的元素按照是否满足某个特定的条件区分开来，
//   那么partition和stable_partition可以做到。
// 5.如果数据在list中，可以直接调用partition和stable_partition，可以用list::sort来代替sort和stable_sort算法。
//   partial_sort、nth_element功能需要间接实现。
// 6.可以通过使用关联容器来保证元素始终保存特定的顺序。
//   也可以考虑使用标准的非STL容器priority_queue，它总是保持其元素的顺序关系。

// 稳定的排序算法要比那些忽略稳定性的算法更为耗时。按消耗资源排序，较少的算法排在前面：
// partition、stable_partition、nth_element、partial_sort、sort、stable_sort

int main() {
  std::vector<Widget> widgets;
  std::partial_sort(widgets.begin(), widgets.begin() + 20, widgets.end(),
                    qualityCompare);

  std::nth_element(widgets.begin(), widgets.begin() + 19, widgets.end(),
                   qualityCompare);

  std::vector<int> v{9, 4, 8, 2, 7, 3, 6, 1, 5};
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 9 4 8 2 7 3 6 1 5
  std::partial_sort(v.begin(), v.begin() + 5, v.end());
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 9 8 7 6

  std::vector<int> v1{9, 4, 8, 2, 7, 3, 6, 1, 5};
  std::copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 9 4 8 2 7 3 6 1 5
  std::nth_element(v1.begin(), v1.begin() + 5, v1.end());
  std::copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 4 1 2 3 6 7 8 9

  // nth_element除了可以找出排名前n个元素，还有其他功能，
  // 比如可以用来找到一个区间的中间值，或者找到某个特定百分比上的值：
  std::vector<Widget>::iterator begin(widgets.begin());
  std::vector<Widget>::iterator end(widgets.end());
  // 找出具有中间质量级别的Widget
  std::vector<Widget>::iterator goalPosition;
  goalPosition = begin + widgets.size() / 2;
  std::nth_element(begin, goalPosition, end, qualityCompare);
  // 找出具有75%质量的元素
  std::vector<Widget>::size_type goalOffset = 0.25 * widgets.size();
  std::nth_element(begin, begin + goalOffset, end, qualityCompare);

  std::vector<int> v2{9, 4, 8, 2, 7, 3, 6, 1, 5};
  std::vector<int>::iterator begin1(v2.begin());
  std::vector<int>::iterator end1(v2.end());
  std::vector<int>::iterator goalPosition1;
  goalPosition1 = begin1 + v2.size() / 2;
  std::nth_element(begin1, goalPosition1, end1);
  std::copy(v1.begin(), v1.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 4 1 2 3 6 7 8 9

  std::vector<int> v3{9, 4, 8, 2, 7, 3, 6, 1, 5};
  std::vector<int>::iterator begin2(v3.begin());
  std::vector<int>::iterator end2(v3.end());
  std::vector<int>::size_type goalOffset2 = 0.25 * v3.size();
  std::nth_element(begin2, begin2 + goalOffset2, end2);
  std::copy(v3.begin(), v3.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 7 6 8 9

  // vector<Widget>::iterator goodEnd = partition(widgets.begin(), widgets.end(), hasAcceptableQuality);
  // 调用partition后，所有满足要求的都被放在了begin和goodEnd之间的区间中。其他大在goodEnd到end之间的区间中。

  return 0;
}