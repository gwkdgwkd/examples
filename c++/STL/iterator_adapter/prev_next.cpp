#include <iostream>
#include <iterator>
#include <list>
#include <vector>

using namespace std;

// prev原意为“上一个”，但prev()的功能远比它的本意大得多，该函数可用来获取一个距离指定迭代器n个元素的迭代器。
// prev() 函数的语法格式如下：
// template <class BidirectionalIterator>
//     BidirectionalIterator prev (BidirectionalIterator it, typename iterator_traits<BidirectionalIterator>::difference_type n = 1);
// 其中，it为源迭代器，其类型只能为双向迭代器或者随机访问迭代器；n为指定新迭代器距离it的距离，默认值为1。该函数会返回一个距离it 迭代器n个元素的新迭代器。
// 注意，当n为正数时，其返回的迭代器将位于it左侧；反之，当n为负数时，其返回的迭代器位于it右侧。
// prev()函数自身不会检验新迭代器的指向是否合理，需要我们自己来保证其合理性。

// 和prev相反，next 原意为“下一个”，但其功能和prev()函数类似，即用来获取一个距离指定迭代器n个元素的迭代器。
// next()函数的语法格式如下：
// template <class ForwardIterator>
//     ForwardIterator next (ForwardIterator it, typename iterator_traits<ForwardIterator>::difference_type n = 1);
// 其中it为源迭代器，其类似可以为前向迭代器、双向迭代器以及随机访问迭代器；n为指定新迭代器距离it的距离，默认值为1。该函数会返回一个距离it迭代器n个元素的新迭代器。
// 需要注意的是，当it为前向迭代器时，n只能为正数，该函数最终得到的新迭代器位于it右侧；当it为双向迭代器或者随机访问迭代器时，若n为正数，则得到的新迭代器位于it右侧，
// 反之位于it左侧。
// 注意，和prev()函数一样，next()函数自身也不会检查新迭代器指向的有效性，需要我们自己来保证。

int main() {
  // advance()函数移动的是源迭代器, advance()函数没有任何返回值，其移动的是it迭代器本身。
  vector<int> myvector{1, 2, 3, 4};
  vector<int>::iterator it = myvector.begin();
  cout << "移动前的 *it = " << *it << endl;  // 1
  advance(it, 2);
  cout << "移动后的 *it = " << *it << endl;  // 2
  // 若不想移动it迭代器本身，而仅仅是想在it迭代器的基础上，得到一个移动指定位置的新迭代器，显然advance()函数是不合适的，这时就可以使用C++ STL标准库提供的另
  // 外2个函数，即prev()和next()函数。

  std::list<int> mylist{1, 2, 3, 4, 5};
  std::list<int>::iterator it1 = mylist.end();
  auto newit1 = prev(it1, 2);
  cout << "prev(it, 2) = " << *newit1 << endl;  // 4
  it1 = mylist.begin();
  newit1 = prev(it1, -2);
  cout << "prev(it, -2) = " << *newit1 << endl;  // 3

  std::list<int>::iterator it2 = mylist.begin();
  auto newit2 = next(it2, 2);
  cout << "next(it, 2) = " << *newit2 << endl;  // 3
  it2 = mylist.end();
  newit2 = next(it2, -2);
  cout << "next(it, -2) = " << *newit2 << endl;  // 4

  return 0;
}