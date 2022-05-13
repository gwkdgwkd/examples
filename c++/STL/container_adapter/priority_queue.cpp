#include <array>
#include <iostream>
#include <queue>

// priority_queue容器适配器模拟的也是队列这种存储结构，即使用此容器适配器存储元素只
// 能“从一端进（称为队尾），从另一端出（称为队头）”，且每次只能访问priority_queue中位于队头的元素。
// 但是，priority_queue容器适配器中元素的存和取，遵循的并不是“First in,First out”（先入先出）原则，
// 而是“First in，Largest out”原则。
// 直白的翻译，指的就是先进队列的元素并不一定先出队列，而是优先级最大的元素最先出队列。
// priority_queue容器适配器中存储的元素，优先级是如何评定的呢？
// 很简单，每个priority_queue容器适配器在创建时，都制定了一种排序规则。
// 根据此规则，该容器适配器中存储的元素就有了优先级高低之分。
// priority_queue容器适配器为了保证每次从队头移除的都是当前优先级最高的元素，每当有新元素进入，
// 它都会根据既定的排序规则找到优先级最高的元素，并将其移动到队列的队头；
// 同样，当priority_queue从队头移除出一个元素之后，它也会再找到当前优先级最高的元素，并将其移动到队头。
// 基于priority_queue的这种特性，该容器适配器有被称为优先级队列。

// priority_queue 容器适配器的定义如下：
//     template <typename T,
//             typename Container=std::vector<T>,
//             typename Compare=std::less<T> >
//     class priority_queue{
//         //......
//     }
// priority_queue容器适配器模板类最多可以传入3个参数，它们各自的含义如下：
//  typename T：指定存储元素的具体类型；
//  typename Container：指定priority_queue底层使用的基础容器，默认使用vector容器。
//  typename Compare：指定容器中评定元素优先级所遵循的排序规则，
//                    默认使用std::less<T>按照元素值从大到小进行排序，
//                    还可以使用std::greater<T>按照元素值从小到大排序，但更多情况下是使用自定义的排序规则。
//  其中，std::less<T>和std::greater<T>都是以函数对象的方式定义在<function>头文件中。

// priority_queue容器适配器模板位于<queue>头文件中，并定义在std命名空间里，提供的成员函数:
// empty() 	               如果priority_queue为空的话，返回true；反之，返回false。
// size() 	               返回priority_queue中存储元素的个数。
// top() 	                 返回priority_queue中第一个元素的引用形式。
// push(const T& obj) 	   根据既定的排序规则，将元素obj的副本存储到priority_queue中适当的位置。
// push(T&& obj) 	         根据既定的排序规则，将元素obj移动存储到priority_queue中适当的位置。
// emplace(Args&&... args) Args&&... args 表示构造一个存储类型的元素所需要的数据。
//                         此函数的功能是根据既定的排序规则，在容器适配器适当的位置直接生成该新元素。
// pop()                   移除priority_queue容器适配器中第一个元素。
// swap(priority_queue<T>& other) 将两个priority_queue容器适配器中的元素进行互换，需要注意的是，
//       进行互换的2个priority_queue容器适配器中存储的元素类型以及底层采用的基础容器类型，都必须相同。
// 和queue一样，priority_queue也没有迭代器，因此访问元素的唯一方式是遍历容器，
// 通过不断移除访问过的元素，去访问下一个元素。

// priority_queue容器的底层实现:
// priority_queue优先级队列之所以总能保证优先级最高的元素位于队头，
// 最重要的原因是其底层采用堆数据结构存储结构。
// priority_queue底层不是采用vector或deque容器存储数据吗，
// 这里又说使用堆结构存储数据，它们之间不冲突吗？
// 显然，它们之间是不冲突的。
// 首先，vector和deque是用来存储元素的容器，而堆是一种数据结构，其本身无法存储数据，
// 只能依附于某个存储介质，辅助其组织数据存储的先后次序。
// 其次，priority_queue底层采用vector或者deque作为基础容器，这毋庸置疑。
// 但由于vector或deque容器并没有提供实现priority_queue容器适配器“First in,Largest out”特性的功能，
// 因此STL选择使用堆来重新组织vector或deque容器中存储的数据，从而实现该特性。
// 虽然不使用堆结构，通过编写算法调整vector或者deque容器中存储元素的次序，
// 也能使其具备“First in,Largest out”的特性，但执行效率通常没有使用堆结构高。
// 堆是完全二叉树的基础上，要求树中所有的父节点和子节点之间，都要满足既定的排序规则：
// 1.如果排序规则为从大到小排序，则表示堆的完全二叉树中，
//   每个父节点的值都要不小于子节点的值，这种堆通常称为大顶堆；
// 2.如果排序规则为从小到大排序，则表示堆的完全二叉树中，
//   每个父节点的值都要不大于子节点的值，这种堆通常称为小顶堆；
// 无论是通过大顶堆或者小顶堆，总可以筛选出最大或最小的那个元素（优先级最大），并将其移至序列的开头，
// 此功能也正是priority_queue容器适配器所需要的。

// 为了验证priority_queue底层确实采用堆存储结构实现的，
// 我们可以尝试用堆结合基础容器vector或deque实现priority_queue。
// 值得庆幸的是，STL已经为我们封装好了可以使用堆存储结构的方法，它们都位于<algorithm>头文件中。
// 常用的几个和堆存储结构相关的方法:
// make_heap(first,last,comp) 	  选择位于[first,last)区域内的数据，并根据comp排序规则建立堆，
//                                其中fist和last可以是指针或者迭代器，默认是建立大顶堆。
// push_heap(first,last,comp) 	  当向数组或容器中添加数据之后，此数据可能会破坏堆结构，该函数的功能是重建堆。
// pop_heap(first,last,comp) 	    将位于序列头部的元素（优先级最高）移动序列尾部，
//                                并使[first,last-1]区域内的元素满足堆存储结构。
// sort_heap(first,last,comp) 	  对[first,last)区域内的元素进行堆排序，将其变成一个有序序列。
// is_heap_until(first,last,comp) 发现[first,last)区域内的最大堆。
// is_heap(first,last,comp) 	    检查[first,last)区域内的元素，是否为堆结构。

int main() {
  // 创建priority_queue容器适配器的方法，大致有以下几种:
  // 1.创建一个空的priority_queue容器适配器，第底层采用默认的vector容器，
  //   排序方式也采用默认的std::less<T>方法：
  std::priority_queue<int> pq1;

  // 2.可以使用普通数组或其它容器中指定范围内的数据，对priority_queue容器适配器进行初始化：
  int a1[]{4, 1, 3, 2};
  std::priority_queue<int> pq2(a1, a1 + 4);
  std::array<int, 4> a2{4, 1, 3, 2};
  std::priority_queue<int> pq3(a2.begin(), a2.end());
  // 以上2种方式必须保证数组或容器中存储的元素类型和priority_queue指定的存储类型相同。
  // 另外，用来初始化的数组或容器中的数据不需要有序，priority_queue会自动对它们进行排序。

  // 3.还可以手动指定priority_queue使用的底层容器以及排序规则：
  std::priority_queue<int, std::deque<int>, std::greater<int>> pq4(a1, a1 + 4);

  std::priority_queue<int> pq5;
  pq5.push(3);  // {3}
  pq5.push(1);  // {3,1}
  pq5.push(4);  // {4,1,3}
  pq5.push(2);  // {4,2,3,1}
  pq5.emplace(5);
  while (!pq5.empty()) {
    std::cout << pq5.top() << " ";
    pq5.pop();
  }
  std::cout << std::endl;  // 5 4 3 2 1

  // 结合vector容器提供的成员函数，模拟了priority_queue容器适配器部分成员函数的底层实现：
  auto display = [](std::vector<int>& val) -> void {
    for (auto v : val) {
      std::cout << v << " ";
    }
    std::cout << std::endl;
  };
  std::vector<int> vv{2, 1, 3, 4};
  make_heap(vv.begin(), vv.end());
  display(vv);  // 4 2 3 1
  vv.push_back(5);
  display(vv);  // 4 2 3 1 5
  push_heap(vv.begin(), vv.end());
  display(vv);  // 5 4 3 1 2
  pop_heap(vv.begin(), vv.end());
  display(vv);  // 4 2 3 1 5
  vv.pop_back();
  display(vv);  // 4 2 3 1

  // 创建优先级队列
  std::vector<int> v{2, 1, 3, 4};
  display(v);  // 2 1 3 4
  std::priority_queue<int> pq6(v.begin(), v.end());
  pq6.push(5);
  display(v);  // 2 1 3 4
  pq6.pop();
  display(v);  // 2 1 3 4
  while (!pq6.empty()) {
    std::cout << pq6.top() << " ";
    pq6.pop();
  }
  std::cout << std::endl;  // 4 3 2 1

  return 0;
}