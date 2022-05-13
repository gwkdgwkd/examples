#include <deque>
#include <iostream>
#include <list>
#include <queue>

// 和stack栈容器适配器不同，queue容器适配器有2个开口，其中一个开口专门用来输入数据，另一个专门用来输出数据。
// 这种存储结构最大的特点是，最先进入queue的元素，也可以最先从queue中出来，
// 即用此容器适配器存储数据具有“先进先出（简称"FIFO"）”的特点，因此queue又称为队列适配器。
// STL queue容器适配器模拟的就是队列这种存储结构，因此对于任何需要用队列进行处理的序列来说，
// 使用queue容器适配器都是好的选择。

// queue容器适配器以模板类queue<T,Container=deque<T>>的形式位于<queue>头文件中，并定义在std命名空间里。
// 其中T为存储元素的类型，Container表示底层容器的类型
//  empty() 	         如果queue中没有元素的话，返回true。
//  size() 	           返回queue中元素的个数。
//  front() 	         返回queue中第一个元素的引用。
//                     如果queue是常量，就返回一个常引用；如果queue为空，返回值是未定义的。
//  back() 	           返回queue中最后一个元素的引用。
//                     如果queue是常量，就返回一个常引用；如果queue为空，返回值是未定义的。
//  push(const T& obj) 在queue的尾部添加一个元素的副本。这是通过调用底层容器的成员函数push_back()来完成的。
//  emplace() 	       在queue的尾部直接添加一个元素。
//  push(T&& obj) 	   以移动的方式在queue的尾部添加元素。
//                     这是通过调用底层容器的具有右值引用参数的成员函数push_back()来完成的。
//  pop() 	           删除queue中的第一个元素。
//  swap(queue<T> &other_queue) 将两个queue容器适配器中的元素进行互换，需要注意的是，
//                     进行互换的2个queue容器适配器中存储的元素类型以及底层采用的基础容器类型，都必须相同。
// 和stack一样，queue也没有迭代器，因此访问元素的唯一方式是遍历容器，通过不断移除访问过的元素，去访问下一个元素。

int main() {
  // 创建queue容器适配器的方式大致可分为以下几种:
  // 1.创建一个空的queue容器适配器，其底层使用的基础容器选择默认的deque容器：
  std::queue<int> q1;

  // 2.可以手动指定queue容器适配器底层采用的基础容器类型：
  std::queue<int, std::list<int>> q2;
  std::queue<int, std::deque<int>> q3;
  // 在手动指定基础容器的类型时，其存储的数据类型必须和queue容器适配器存储的元素类型保持一致。
  // 作为queue容器适配器的基础容器，
  // 其必须提供front()、back()、push_back()、pop_front()、empty()和size()这几个成员函数，
  // 符合条件的序列式容器仅有deque和list。

  // 3.可以用基础容器来初始化queue容器适配器，只要该容器类型和queue底层使用的基础容器类型相同即可：
  std::deque<int> d{1, 2, 3};
  std::queue<int> q4(d);

  // 4.可以直接通过queue容器适配器来初始化另一个queue容器适配器，
  // 只要它们存储的元素类型以及底层采用的基础容器类型相同即可：
  std::queue<int> q5(q1);
  std::queue<int> q6 = q1;

  std::deque<int> values{1, 2, 3};
  std::queue<int> q7(values);
  q7.emplace(4);
  q7.push(5);
  std::cout << q7.back() << std::endl;                          // 5
  std::cout << "size of my_queue: " << q7.size() << std::endl;  // 5
  while (!q7.empty()) {
    std::cout << q7.front() << std::endl;
    q7.pop();
  }
  // 1
  // 2
  // 3
  // 4
  // 5

  return 0;
}