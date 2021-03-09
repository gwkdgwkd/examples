#include <array>
#include <iostream>
#include <queue>

using namespace std;

// priority_queue容器适配器模拟的也是队列这种存储结构，即使用此容器适配器存储元素只能“从一端进（称为队尾），从另一端出（称为队头）”，且每次只能访问priority_queue中位于队头的元素。
// 但是，priority_queue容器适配器中元素的存和取，遵循的并不是“First in,First out”（先入先出）原则，而是“First in，Largest out”原则。
// 直白的翻译，指的就是先进队列的元素并不一定先出队列，而是优先级最大的元素最先出队列。
// priority_queue容器适配器中存储的元素，优先级是如何评定的呢？
// 很简单，每个priority_queue容器适配器在创建时，都制定了一种排序规则。根据此规则，该容器适配器中存储的元素就有了优先级高低之分。
// priority_queue容器适配器为了保证每次从队头移除的都是当前优先级最高的元素，每当有新元素进入，它都会根据既定的排序规则找到优先级最高的元素，并将其移动到队列的队头；
// 同样，当priority_queue从队头移除出一个元素之后，它也会再找到当前优先级最高的元素，并将其移动到队头。基于priority_queue的这种特性，该容器适配器有被称为优先级队列。

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
//  typename Compare：指定容器中评定元素优先级所遵循的排序规则，默认使用std::less<T>按照元素值从大到小进行排序，还可以使用std::greater<T>按照元素值从小到大排序，
//                    但更多情况下是使用自定义的排序规则。
//  其中，std::less<T>和std::greater<T>都是以函数对象的方式定义在<function>头文件中。
// 作为priority_queue容器适配器的底层容器，其必须包含empty()、size()、front()、push_back()、pop_back()这几个成员函数，STL序列式容器中只有vector和deque容器符合条件。

// priority_queue容器适配器模板位于<queue>头文件中，并定义在std命名空间里
// priority_queue提供的成员函数:
// empty() 	如果priority_queue为空的话，返回true；反之，返回false。
// size() 	返回priority_queue中存储元素的个数。
// top() 	返回priority_queue中第一个元素的引用形式。
// push(const T& obj) 	根据既定的排序规则，将元素obj的副本存储到priority_queue中适当的位置。
// push(T&& obj) 	根据既定的排序规则，将元素obj移动存储到priority_queue中适当的位置。
// emplace(Args&&... args) 	Args&&... args 表示构造一个存储类型的元素所需要的数据（对于类对象来说，可能需要多个数据构造出一个对象）。
//                          此函数的功能是根据既定的排序规则，在容器适配器适当的位置直接生成该新元素。
// pop() 	移除 priority_queue 容器适配器中第一个元素。
// swap(priority_queue<T>& other) 	将两个priority_queue容器适配器中的元素进行互换，需要注意的是，进行互换的2个priority_queue容器适配器中存储的元素类型以
//                                  及底层采用的基础容器类型，都必须相同。
// 和queue一样，priority_queue也没有迭代器，因此访问元素的唯一方式是遍历容器，通过不断移除访问过的元素，去访问下一个元素。

int main() {
  // 创建priority_queue容器适配器的方法，大致有以下几种:
  // 1 创建一个空的priority_queue容器适配器，第底层采用默认的vector容器，排序方式也采用默认的std::less<T>方法
  std::priority_queue<int> values1;
  // 2 可以使用普通数组或其它容器中指定范围内的数据，对priority_queue容器适配器进行初始化
  int a1[]{4, 1, 3, 2};
  std::priority_queue<int> copy_values1(a1, a1 + 4);
  std::array<int, 4> a2{4, 1, 3, 2};
  std::priority_queue<int> copy_values2(a2.begin(), a2.end());
  // 以上2种方式必须保证数组或容器中存储的元素类型和priority_queue指定的存储类型相同。另外，用来初始化的数组或容器中的数据不需要有序，priority_queue会自动对它们进行排序。
  // 3 还可以手动指定priority_queue使用的底层容器以及排序规则
  std::priority_queue<int, std::deque<int>, std::greater<int> > copy_values3(
      a1, a1 + 4);

  std::priority_queue<int> values;
  values.push(3);  // {3}
  values.push(1);  // {3,1}
  values.push(4);  // {4,1,3}
  values.push(2);  // {4,2,3,1}
  while (!values.empty()) {
    std::cout << values.top() << " ";
    values.pop();
  }
  cout << endl;  // 4 3 2 1

  return 0;
}