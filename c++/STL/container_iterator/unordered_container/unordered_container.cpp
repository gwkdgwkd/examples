#include <iostream>
#include <unordered_map>

using namespace std;

// 一类“特殊”的关联式容器，它们常被称为“无序容器”、“哈希容器”或者“无序关联容器”。
// 无序容器是C++11标准才正式引入到STL标准库中的，这意味着如果要使用该类容器，则必须选择支持C++11标准的编译器。
// 和关联式容器一样，无序容器也使用键值对（pair类型）的方式存储数据。不过它们有本质上的不同：
//  关联式容器的底层实现采用的树存储结构，更确切的说是红黑树结构；
//  无序容器的底层实现采用的是哈希表的存储结构。
// C++ STL底层采用哈希表实现无序容器时，会将所有数据存储到一整块连续的内存空间中，并且当数据存储位置发生冲突时，解决方法选用的是“链地址法”（又称“开链法”）。
// 基于底层实现采用了不同的数据结构，因此和关联式容器相比，无序容器具有以下2个特点：
//  无序容器内部存储的键值对是无序的，各键值对的存储位置取决于该键值对中的键，
//  和关联式容器相比，无序容器擅长通过指定键查找对应的值（平均时间复杂度为O(1)）；但对于使用迭代器遍历容器中存储的元素，无序容器的执行效率则不如关联式容器。

// STL无序容器种类:
// 和关联式容器一样，无序容器只是一类容器的统称，其包含有4个具体容器，分别为unordered_map、unordered_multimap、unordered_set以及unordered_multiset。
// unordered_map  存储键值对<key, value>类型的元素，其中各个键值对键的值不允许重复，且该容器中存储的键值对是无序的。
// unordered_multimap 和unordered_map唯一的区别在于，该容器允许存储多个键相同的键值对。
// unordered_set 不再以键值对的形式存储数据，而是直接存储数据元素本身（当然也可以理解为，该容器存储的全部都是键key和值value相等的键值对，正因为它们相等，因此只存储valu 即可）。
//               另外，该容器存储的元素不能重复，且容器内部存储的元素也是无序的。
// unordered_multiset 和unordered_set唯一的区别在于，该容器允许存储值相同的元素。

// C++11标准的STL中，在已提供有4种关联式容器的基础上，又新增了各自的“unordered”版本（无序版本、哈希版本），提高了查找指定元素的效率。
// 它们仅有一个区别，即是否会对存储的键值对进行排序。
// 实际场景中如果涉及大量遍历容器的操作，建议首选关联式容器；反之，如果更多的操作是通过键获取对应的值，则应首选无序容器。

int main() {
  std::unordered_map<std::string, std::string> my_uMap{
      {"C语言教程", "http://c.biancheng.net/c/"},
      {"Python教程", "http://c.biancheng.net/python/"},
      {"Java教程", "http://c.biancheng.net/java/"}};
  string str = my_uMap.at("C语言教程");
  cout << "str = " << str << endl;
  for (auto iter = my_uMap.begin(); iter != my_uMap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // str = http://c.biancheng.net/c/
  // Java教程 http://c.biancheng.net/java/
  // C语言教程 http://c.biancheng.net/c/
  // Python教程 http://c.biancheng.net/python/

  return 0;
}