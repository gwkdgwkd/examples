#include <iostream>
#include <iterator>
#include <list>

// distance()用于计算两个迭代器表示的范围内包含元素的个数，语法格式：
// template<class InputIterator>
// typename iterator_traits<InputIterator>::difference_type
// distance(InputIterator first, InputIterator last);
// 其中，first和last都为迭代器，其类型可以是输入、前向、双向以及随机迭代器；
// 该函数会返回[first,last)范围内包含的元素的个数。
// 注意，first和last的迭代器类型，直接决定了distance()函数底层的实现机制：
// 1.当first和last为随机访问迭代器时，其时间复杂度为O(1)常数阶，
//   直接采用last-first求得[first,last)范围内包含元素的个数，
// 2.当first和last为非随机访问迭代器时，时间复杂度为O(n)线性阶，
//   通过不断执行++first（或者first++）直到first==last，
//   由此获取[first,last)内元素的个数。

// distance()函数定义在<iterator>头文件，并位于std命名空间中。

int main() {
  std::list<int> l;
  for (int i = 0; i < 10; i++) {
    l.push_back(i);
  }
  std::list<int>::iterator first = l.begin();
  std::list<int>::iterator last = l.end();
  std::cout << "distance() = " << distance(first, last) << std::endl;
  // distance() = 10

  std::cout << "distance() = " << distance(++l.begin(), --l.end()) << std::endl;
  // distance() = 8

  return 0;
}