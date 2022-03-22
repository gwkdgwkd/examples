#include <algorithm>
#include <iostream>
#include <vector>

// 区间成员函数优先于与之对应的单元素成员函数

// 区间成员函数是指这样一类成员函数，像STL算法一样，使用两个迭代器参数来确定该成员操作执行的区间。
class Widget {};
// assign是一个使用及其方便，却被许多程序员所忽略的成员函数。
// 几乎所有通过利用插入迭代器的方式（inserter、back_inserter或front_inserter)来限定目标区间的copy调用，
// 其实都可以（也应该）被替换为对区间成员函数的调用。

// 通过使用区间成员函数，通常可以少写一些代码。
// 使用区间成员函数通常会得到意图清晰和更加直接的代码。

// 对于标准序列容器，为了取得同样的结果，使用单元素的成员函数比使用区间成员函数需要更多地调用内存分配子，
// 更频繁地拷贝对象，而且或者做冗余操作。
// 相对于区间版本的insert，单元素版本的insert总共在三个方面影响效率：
// 1.不必要的函数调用。调用了多次insert函数。使用区间的成员函数，只调用了一次。
//   使用内联可以避免这样的影响，但实际中不见得会使用内联。
// 2.元素移动的多，每插入一次都要移动后面所以的元素。
//   使用区间的成员函数，只需要每元素只需要移动一次，移到最终的位置。
// 3.使用单元素成员函数，可能会导致多次分配内存。
// 这些问题对string、deque同样适用。
// list使用区间成员函数也有效率的优势。重复调用的论断继续生效。
// 还有新问题，对某些节点的next和prev指针的重复的、多余的赋值操作。

// 对于标准序列容器，在单元素的插入和区间的插入之间做选择所依据的不只是程序风格的考虑。
// 对于关联容器，效率问题更加难以说清楚。

// 哪些函数支持区间？
// 区间创建：所有标准容器都提供了下面的构造函数。
//  container::container(InputIterator Begin, InputIterator end);
// 区间插入：所有标准序列容器都提供了如下形式的inert。
//  void container::insert(iterator position, InputIterator Begin, InputIterator end);
//  void container::insert(InputIterator Begin, InputIterator end);
//  关联容器利用比较函数来决定元素插入位置，不用position
// 区间删除：所有的标准容器都提供了区间形式的删除操作。
//  iterator container::erase(InputIterator Begin, InputIterator end);  // 序列容器
//  void container::erase(InputIterator Begin, InputIterator end);  // 关联容器
// 区间赋值：所有的标准容器都提供了区间形式的assign。
//  void container::assign(InputIterator Begin, InputIterator end);

// 优先选择区间成员函数而不是其对应的单元素成员函数有三条充分的理由：
// 1.区间成员函数写起来更容易；
// 2.更能清楚地表达你的意图；
// 3.更高的效率。

int main() {
  // 使v1的内容和v2的后半部分相同的最简单操作是什么？
  std::vector<Widget> v1, v2;
  v1.clear();
  // 不使用区间成员函数，就得写一个显示的循环：
  for (std::vector<Widget>::const_iterator ci = v2.begin() + v2.size() / 2;
       ci != v2.end(); ++ci) {
    v1.push_back(*ci);
  }

  // 避免循环的方法(代码中没有循环，但copy中肯定有)：
  std::copy(v2.begin() + v2.size() / 2, v2.end(), back_inserter(v1));

  // 被替换为利用区间的insert版本：
  v1.insert(v1.end(), v2.begin() + v2.size() / 2, v2.end());

  // 使用区间成员函数：
  v1.assign(v2.begin() + v2.size() / 2, v2.end());

  static const int numValues = 10;
  int data[numValues];
  std::vector<int> v;
  v.insert(v.begin(), data, data + numValues);
  std::vector<int>::iterator insertLoc(v.begin());
  for (int i = 0; i < numValues; ++i) {
    insertLoc = v.insert(insertLoc, data[i]);
    ++insertLoc;
  }
  // 把循环替换为copy，和显示循环几乎是相同的
  std::copy(data, data + numValues, inserter(v, v.begin()));

  return 0;
}