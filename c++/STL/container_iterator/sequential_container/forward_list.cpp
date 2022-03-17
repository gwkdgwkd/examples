#include <array>
#include <forward_list>
#include <iostream>
#include <iterator>

using namespace std;

// forward_list是C++11新添加的一类容器，其底层实现和list容器一样，采用的也是链表结构，
// 只不过forward_list使用的是单链表，而list使用的是双向链表。
// 使用链表存储数据最大的特点在于，其并不会将数据进行集中存储，
// 换句话说，链表中数据的存储位置是分散的、随机的，整个链表中数据的线性关系通过指针来维持。
// forward_list容器具有和list容器相同的特性，即擅长在序列的任何位置进行插入元素或删除元素的操作，
// 但对于访问存储的元素，没有其它容器（array、vector）的效率高。
// 由于单链表没有双向链表那样灵活，因此相比list容器，forward_list容器的功能受到了很多限制。
// 比如，由于单链表只能从前向后遍历，而不支持反向遍历，因此forward_list容器只提供前向迭代器，而不是双向迭代器。
// 这意味着，forward_list容器不具有rbegin()、rend()之类的成员函数。
// 既然forward_list容器具有和list容器相同的特性，list容器还可以提供更多的功能函数，forward_list容器有什么存在的必要呢？
// 当然有，forward_list容器底层使用单链表，也不是一无是处。
// 比如，存储相同个数的同类型元素，单链表耗用的内存空间更少，空间利用率更高，并且对于实现某些操作单链表的执行效率也更高。
// 效率高是选用forward_list而弃用list容器最主要的原因，
// 换句话说，只要是list容器和forward_list容器都能实现的操作，应优先选择forward_list容器。

// forward_list容器支持的成员函数:
// before_begin() 	返回一个前向迭代器，其指向容器中第一个元素之前的位置。
// begin() 	        返回一个前向迭代器，其指向容器中第一个元素的位置。
// end() 	          返回一个前向迭代器，其指向容器中最后一个元素之后的位置。
// cbefore_begin() 	和before_begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cbegin() 	      和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// cend() 	        和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改元素。
// empty() 	        判断容器中是否有元素，若无元素，则返回true；反之，返回false。
// max_size() 	    返回容器所能包含元素个数的最大值。这通常是一个很大的值，一般是232-1，所以我们很少会用到这个函数。
// front() 	        返回第一个元素的引用。
// assign() 	      用新元素替换容器中原有内容。
// push_front() 	  在容器头部插入一个元素。
// emplace_front() 	在容器头部生成一个元素。该函数和push_front()的功能相同，但效率更高。
// pop_front() 	    删除容器头部的一个元素。
// emplace_after() 	在指定位置之后插入一个新元素，并返回一个指向新元素的迭代器。和insert_after()的功能相同，但效率更高。
// insert_after() 	在指定位置之后插入一个新元素，并返回一个指向新元素的迭代器。
// erase_after() 	  删除容器中某个指定位置或区域内的所有元素。
// swap() 	        交换两个容器中的元素，必须保证这两个容器中存储的元素类型是相同的。
// resize() 	      调整容器的大小。
// clear() 	        删除容器存储的所有元素。
// splice_after() 	将某个forward_list容器中指定位置或区域内的元素插入到另一个容器的指定位置之后。
// remove(val) 	    删除容器中所有等于val的元素。
// remove_if() 	    删除容器中满足条件的元素。
// unique() 	      删除容器中相邻的重复元素，只保留一个。
// merge() 	        合并两个事先已排好序的forward_list容器，并且合并之后的forward_list容器依然是有序的。
// sort() 	        通过更改容器中元素的位置，将它们进行排序。
// reverse() 	      反转容器中元素的顺序。

// C++11标准库还新增加了begin()和end()这2个函数。
// forward_list容器还有一个std::swap(x,y)非成员函数它和swap()成员函数的功能完全相同，仅使用语法上有差异。

template <typename T>
void print(std::forward_list<T>& fl) {
  std::copy(fl.begin(), fl.end(), std::ostream_iterator<T>(std::cout, " "));
  // forward_list容器中是不提供size()函数的，但如果想要获取forward_list容器中存储元素的个数，
  // 可以使用头文件<iterator>中的distance()函数：
  std::cout << "[" << std::distance(fl.begin(), fl.end()) << "]" << std::endl;
}

void func1() {  // 创建forward_list容器
  // 1.创建一个没有任何元素的空forward_list容器：
  std::forward_list<int> fl1;
  print(fl1);  // [0]

  // 2.创建一个包含n个元素的forward_list容器，包含10个元素，每个元素的值都为相应类型的默认值：
  std::forward_list<int> fl2(10);
  std::forward_list<int> fl3{2};
  print(fl2);  // 0 0 0 0 0 0 0 0 0 0 [10]
  print(fl3);  // 2 [1]

  // 3.创建一个包含n个元素的forward_list容器，并为每个元素指定初始值：
  std::forward_list<int> fl4(10, 5);
  std::forward_list<int> fl5 = {1, 2};
  print(fl4);  // 5 5 5 5 5 5 5 5 5 5 [10]
  print(fl5);  // 1 2 [2]

  // 4.在已有forward_list容器的情况下，通过拷贝该容器可以创建新的forward_list容器：
  std::forward_list<int> fl6(fl5);   // 1 2 [2]
  std::forward_list<int> fl7 = fl4;  // 5 5 5 5 5 5 5 5 5 5 [10]
  print(fl6);
  print(fl7);

  // 5.通过拷贝其他类型容器（或者普通数组）中指定区域内的元素，可以创建新的forward_list容器：
  int a[] = {1, 2, 3, 4, 5};
  std::forward_list<int> fl8(a, a + 5);
  std::array<int, 5> arr{11, 12, 13, 14, 15};
  std::forward_list<int> fl9(arr.begin() + 2, arr.end());
  print(fl8);  // 1 2 3 4 5 [5]
  print(fl9);  // 13 14 15 [3]
}

void func2() {  // 遍历
  std::forward_list<int> fl{1, 2, 3, 4, 5};

  for (auto it = fl.begin(); it != fl.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5

  for (auto& i : fl) {
    std::cout << i << " ";
  }
  std::cout << std::endl;  // 1 2 3 4 5

  auto it = fl.begin();
  while (it != fl.end()) {
    std::cout << *it << " ";
    std::advance(it, 1);  // ++it
  }
  std::cout << std::endl;  // 1 2 3 4 5
}

void func3() {
  std::forward_list<int> fl{1, 2, 3, 4, 5};
  auto it = fl.before_begin();  // 不能访问it，行为未定义

  fl.emplace_after(it, 0);
  print(fl);  // 0 1 2 3 4 5 [6]

  fl.insert_after(it, -1);
  print(fl);  // -1 0 1 2 3 4 5 [7]

  fl.erase_after(it);
  print(fl);  // 0 1 2 3 4 5 [6]
}

void func4() {
  std::forward_list<int> fl{1, 2, 3, 4, 5};
  fl.push_front(-1);
  print(fl);  // -1 1 2 3 4 5 [6]

  fl.emplace_front(-2);
  print(fl);  // -2 -1 1 2 3 4 5 [7]

  auto it = fl.begin();
  std::advance(it, 6);
  fl.emplace_after(it, 6);
  print(fl);  // -2 -1 1 2 3 4 5 6 [8]

  ++it;
  fl.insert_after(it, 2, 7);
  print(fl);  // -2 -1 1 2 3 4 5 6 7 7 [10]

  fl.pop_front();
  print(fl);  // -1 1 2 3 4 5 6 7 7 [9]

  it = fl.before_begin();
  fl.erase_after(it);
  print(fl);  // 1 2 3 4 5 6 7 7 [8]

  fl.remove(7);
  print(fl);  // 1 2 3 4 5 6 [6]

  fl.remove_if([](int i) { return i >= 3 && i <= 5; });
  print(fl);  // 1 2 6 [3]

  fl.resize(5);
  print(fl);  // 1 2 6 0 0 [5]

  fl.reverse();
  print(fl);  // 0 0 6 2 1 [5]

  fl.sort();
  print(fl);  // 0 0 1 2 6 [5]

  fl.unique();
  print(fl);  // 0 1 2 6 [4]

  fl.front() = 8;
  print(fl);  // 8 1 2 6 [4]

  fl.clear();
  print(fl);  // [0]

  std::cout << std::boolalpha << fl.empty() << std::endl;  // true
  std::cout << fl.max_size() << std::endl;                 // 576460752303423487
}

void func5() {
  std::forward_list<int> fl1{1, 2, 3, 4, 5};
  std::forward_list<int> fl2{6, 7, 8, 9, 10};

  print(fl1);  // 1 2 3 4 5 [5]
  print(fl2);  // 6 7 8 9 10 [5]
  fl1.swap(fl2);
  print(fl1);  // 6 7 8 9 10 [5]
  print(fl2);  // 1 2 3 4 5 [5]
  std::swap(fl1, fl2);
  print(fl1);  // 1 2 3 4 5 [5]
  print(fl2);  // 6 7 8 9 10 [5]

  // auto it = fl1.before_begin();
  // fl1.splice_after(it, fl2);
  fl2.splice_after(fl2.before_begin(), fl1, fl1.before_begin(), fl1.end());
  print(fl1);  // [0]
  print(fl2);  // 1 2 3 4 5 6 7 8 9 10 [10]

  fl1.splice_after(fl1.before_begin(), fl2, fl2.begin());
  print(fl1);  // 2 [1]
  print(fl2);  // 1 3 4 5 6 7 8 9 10 [9]

  auto it = fl2.begin();
  std::advance(it, 4);
  fl1.splice_after(fl1.before_begin(), fl2, it);
  std::advance(it, 1);
  fl1.splice_after(fl1.before_begin(), fl2, it);
  print(fl1);  // 9 7 2 [3]
  print(fl2);  // 1 3 4 5 6 8 10 [7]

  fl1.splice_after(fl1.begin(), fl2);
  print(fl1);  // 9 1 3 4 5 6 8 10 7 2 [10]
  print(fl2);  // [0]

  it = fl1.begin();
  advance(it, 1);
  fl2.splice_after(fl2.before_begin(), fl1, it);
  advance(it, 1);
  fl2.splice_after(fl2.begin(), fl1, it);
  advance(it, 1);
  fl2.splice_after(fl2.begin(), fl1, it);
  print(fl1);  // 9 1 4 6 10 7 2 [7]
  print(fl2);  // 3 8 5 [3]

  fl1.merge(fl2);
  print(fl1);  // 3 8 5 9 1 4 6 10 7 2 [10]
  print(fl2);  // [0]
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 8]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    case 3:
      func4();
      break;
    case 4:
      func5();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}