#include <iostream>
#include <iterator>
#include <list>

using namespace std;

// distance()函数用于计算两个迭代器表示的范围内包含元素的个数，其语法格式如下：
// template<class InputIterator> typename iterator_traits<InputIterator>::difference_type distance (InputIterator first, InputIterator last);
// 其中，first和last都为迭代器，其类型可以是输入迭代器、前向迭代器、双向迭代器以及随机访问迭代器；该函数会返回[first,last)范围内包含的元素的个数。
// 注意，first和last的迭代器类型，直接决定了distance()函数底层的实现机制：
//  当first和last为随机访问迭代器时，distance()底层直接采用last-first求得[first, last)范围内包含元素的个数，其时间复杂度为O(1)常数阶；
//  当first和last为非随机访问迭代器时，distance()底层通过不断执行++first（或者first++）直到first==last，由此获取[first,last)内元素的个数，时间复杂度为O(n)线性阶。

// distance()函数定义在<iterator>头文件，并位于std命名空间中

int main() {
  list<int> mylist;
  for (int i = 0; i < 10; i++) {
    mylist.push_back(i);
  }
  list<int>::iterator first = mylist.begin();
  list<int>::iterator last = mylist.end();
  cout << "distance() = " << distance(first, last) << endl;  // distance() = 10

  return 0;
}