#include <algorithm>
#include <iostream>
#include <list>

// 调用empty而不是检查size()是否为0

// 对于任一容器下面的代码本质上是等价的：
// if(c.size() == 0)
// if(c.empty())
// empty通常被实现为内联函数，并且它所做的仅仅是返回size是否为0。
// 应该使用empty形式，理由很简单：
// empty对所有的标准容器都是常数时间操作，而对一些list实现，size耗费线性时间。

// 如果size是常数时间，那么list的每个成员函数都必须更新它们所操作的链表的大小，
// 当然也包括splice，可splice更新它所改变的链表的大小的唯一方式就是遍历链表，
// 而这会使splice不具有所期望的常数时间性能，必须做出让步，
// 不可能让size和splice都是常数时间操作，不同的链表实现通过不同的方式解决，
// 具体方式取决于作者选择把size还是splice实现的最为高效。

// 不管发生了什么，调用empty而不是检查size==0是否成立总是没错的。

int main() {
  // 为什么list不提供常数时间的size呢？因为list所独有的链接操作：
  std::list<int> list1;
  std::list<int> list2;
  list1.splice(list1.end(), list2, std::find(list2.begin(), list2.end(), 5),
               std::find(list2.rbegin(), list2.rend(), 10).base());
  // 有多少个元素被链接来呢？不遍历该区间来数一数，无法知道。

  return 0;
}