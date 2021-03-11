#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// 无论是序列式容器还是关联式容器（包括哈希容器），不仅模板类内部提供有begin()和end()成员方法，C++ STL标准库中还提供有同名且具有相同功能的begin()和end()函数。
// begin()和end()是以函数模板的形式定义的，但它们的模板并没有位于某一个头文件中，而是很多头文件中都有它们的定义。
// C++ STL标准库中，包含begin()和end()函数模板的头文件包括：<iterator>, <array>, <deque>, <forward_list>, <list>, <map>, <regex>（正则表达式的头文
// 件）, <set>, <string>, <unordered_map>, <unordered_set>以及<vector>。begin()和end()都位于std命名空间中。

// 在实际的使用场景中，begin()和end()函数往往会一起使用的。根据作用对象的不同，begin()和end()函数可细分为以下2个功能。
// 1 begin()和end()参数为容器
// 当将某个具体容器（cont）作为参数分别传给begin()和end()函数时，其中begin()底层会执行cont.begin()语句，而end()底层会执行cont.end()语句，它们最终会将得到的
// 迭代器作为函数的返回值反馈回来。
// 当作用对象为容器时，end()和begin()函数的语法格式是完全一样的，这里以begin()函数为例，有以下2种格式：
//  1 非const修改的容器作为参数，begin()函数返回的为非const类型的迭代器template <class Container> auto begin (Container& cont)
//  2 传入const修饰的容器，begin()函数返回的为const类型的迭代器template <class Container> auto begin (const Container& cont)
// 其中，cont表示指定的容器；同时，函数会返回一个有特定指向的迭代器，且此迭代器的类型也取决于cont容器。
// 以上2种格式的区别仅在与传入的容器是否有const修饰，即如果有，则通过该函数获得的迭代器也有const修饰（不能用于修改容器中存储的数据）；反之就没有。
// 2 begin()和end()参数为数组
// 除了可以将指定容器作为参数传给begin()和end()之外，还可以指定数组作为参数传给它们。
// 将指定数组传给begin()函数，其会返回一个指向该数组首个元素的指针；将指定数组传给end()函数，其会返回一个指向数组中最后一个元素之后位置的指针。
// 同样，数组作为参数时，end()函数的语法格式和begin()函数也完全一样，这里仅给出了begin()函数的语法格式：
// template <class T, size_t N> T* begin (T(&arr)[N]);
// 其中T为数组中存储元素的类型，N为数组的长度；(&arr)[N]表示以引用的方式传递数组作为参数。

int main() {
  std::vector<int> myvector{1, 2, 3, 4, 5};
  // begin(myvector)等同于执行myvector.begin()，而end(myvector)也等同于执行myvector.end()。
  for (auto it = begin(myvector); it != end(myvector); ++it) {
    cout << *it << ' ';
  }
  cout << endl;  // 1 2 3 4 5

  int arr[] = {1, 2, 3, 4, 5};
  vector<int> myvector1;
  for (int *it = begin(arr); it != end(arr); ++it) myvector1.push_back(*it);
  for (auto it = myvector1.begin(); it != myvector1.end(); ++it) {
    cout << *it << ' ';
  }
  cout << endl;  // 1 2 3 4 5

  return 0;
}