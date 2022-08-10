#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>

// C++ STL标准库中还提供有一些辅助函数：
// advance(it,n)        it表示某个迭代器，n为整数。
//                      该函数的功能是将it迭代器前进或后退n个位置。
// distance(first,last) first和last都是迭代器，
//                      该函数的功能是计算first和last之间的距离。
// begin(cont)          返回一个指向cont容器中第一个元素的迭代器。
// end(cont)            返回一个指向cont容器中最后一个元素之后位置的迭代器。
// prev(it)             it为指定的迭代器，默认返回一个指向上一个位置处的迭代器。
//                      注意，it至少为双向迭代器。
// next(it)             it为指定的迭代器，默认返回一个指向下一个位置处的迭代器。
//                      注意，it最少为前向迭代器。

// advance()函数用于将迭代器前进（或者后退）指定长度的距离，
// 其语法格式如下：
// template <class InputIterator, class Distance>
// void advance(InputIterator& it, Distance n);
// 其中it指的是目标迭代器，n通常为一个整数。
// 需要注意的是，如果it为输入迭代器或者前向迭代器，
// 则n必须为一个正数，即表示将it右移（前进）n个位置；
// 反之，如果it为双向迭代器或者随机访问迭代器，
// 则n为正数时表示将it右移n个位置，n为负数时表示将it左移n个位置。
// 另外，根据it类型是否为随机访问迭代器，
// advance()函数底层采用了不同的实现机制：
// 1.当it为随机访问迭代器时，
//   由于该类型迭代器支持p+n或者p-n（其中p就是一个随机访问迭代器）运算，
//   advance()函数底层采用的就是it+n操作实现的；
// 2.当it为其他类型迭代器时，它们仅支持进行++或者--运算，这种情况下，
//   advance()函数底层是通过重复执行n个++或者--操作实现的。

// advance()函数定义在<iterator>头文件，并位于std命名空间中。
// advance()函数本身不会检测it迭代器移动n个位置的可行性，
// 如果it迭代器的移动位置超出了合理范围，
// it迭代器的指向将无法保证，此时使用*it将会导致程序崩溃。

void func1() {
  std::forward_list<int> l{1, 2, 3, 4};
  std::forward_list<int>::iterator it = l.begin();
  advance(it, 2);
  std::cout << "*it = " << *it << std::endl;  // *it = 3
}

void func2() {
  std::vector<int> v{1, 2, 3, 4};
  std::vector<int>::iterator it = v.begin();
  advance(it, 2);
  std::cout << "begin+2 : *it = " << *it << std::endl;
  // begin+2 : *it = 3

  it = v.end();
  advance(it, -3);
  std::cout << "end-3 : *it = " << *it << std::endl;
  // end-3 : *it = 2
}

int main() {
  func1();
  func2();
  return 0;
}