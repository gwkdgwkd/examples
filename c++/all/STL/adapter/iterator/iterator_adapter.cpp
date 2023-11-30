#include <iostream>
#include <list>

// 可以将迭代器适配器视为普通迭代器，之所以称为迭代器适配器，
// 是因为这些迭代器是在输入迭代器、输出迭代器、正向迭代器、
// 双向迭代器或者随机访问迭代器这些基础迭代器的基础上实现的。
// 也就是说，使用迭代器适配器的过程中，其本质就是在操作某种基础迭代器。
// 无论是序列式容器还是关联式容器（包括哈希容器），
// 要想遍历容器中存储的数据，就只能用使用该容器模板类中提供的迭代器。
// C++ STL标准库中迭代器大致分为5种类型：
// 1.输入迭代器；
// 2.输出迭代器；
// 3.正向（前向）迭代器；
// 4.双向迭代器；
// 5.随机访问迭代器。
// 这5种是STL标准库提供的最基础的迭代器，很多场景遍历容器的需求，它们并不适合。

// 迭代器适配器本质也是模板类，只不过该模板类是借助以上5种基础迭代器实现的。
// 本质上讲，迭代器适配器仍属于迭代器，可以理解为是基础迭代器的翻新版或者升级版。
// 同时，xxx迭代器适配器通常直接称为xxx迭代器。
// 5种迭代器适配器：
// 1.反向迭代器适配器；
// 2.插入型迭代器适配器；
// 3.流迭代器适配器；
// 4.流缓冲区迭代器适配器；
// 5.移动迭代器适配器。

// STL迭代器适配器种类：
// 1.反向（逆向）迭代器（reverse_iterator） 
//   其内部重新定义了++和--，专门用来实现对容器的逆序遍历；
// 2.插入型迭代器（inserter或者insert_iterator）
//   通常用于在容器的任何位置添加新的元素，但不能用在array等固定个数的容器上；
// 3.流迭代器（istream_iterator/ostream_iterator）
//   输入流迭代器用于从文件或键盘读数据，输出流迭代器用于将数据输出到文件或屏幕。
// 4.流缓冲区迭代器（istreambuf_iterator/ostreambuf_iterator）
//   输入流缓冲区迭代器用于从输入缓冲区中逐个读取数据，
//   输出流缓冲区迭代器用于将数据逐个写入输出流缓冲区；
// 5.移动迭代器（move_iterator）
//   是C++11新添加的，可以将某个范围的类对象移动到目标范围，而不通过拷贝去移动。

std::list<int> l{1, 2, 3, 4, 5};
void func1() {
  // list容器模板类提供的是双向迭代器，如果使用该类型迭代器实现逆序操作：
  std::list<int>::iterator begin = --l.end();
  std::list<int>::iterator end = --l.begin();
  while (begin != end) {
    std::cout << *begin << " ";
    --begin;
  }
  std::cout << std::endl;  // 5 4 3 2 1
}
void func2() {
  // 用反向迭代器适配器遍历list容器的实现过程：
  std::reverse_iterator<std::list<int>::iterator> begin = l.rbegin();
  std::reverse_iterator<std::list<int>::iterator> end = l.rend();
  while (begin != end) {
    std::cout << *begin << " ";
    ++begin;
  }
  std::cout << std::endl;  // 5 4 3 2 1

  // 程序中通过调用list容器模板类提供的rbegin()和rend()，
  // 可以获得反向迭代器，从而轻松实现逆序输出容器中存储的所有数据。
}

int main() {
  func1();
  func2();
  return 0;
}