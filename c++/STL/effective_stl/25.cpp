#include <iostream>
#include <unordered_map>
#include <unordered_set>

// 熟悉非标准的哈希容器

// C++11增加了hash_set、hash_multiset、hash_map和hash_multimap。
// 哈希容器是关联容器，和其他关联容器一样，
// 需要指定存储在容器中对象的类型、用于这种对象的比较函数，
// 已经用于这些对象的分配子。
// 另外，哈希容器也要求指定一个哈希函数。

// SGI的实现：
// template<typename T,
//          typename HashFunction = hash<T>，
//          typename CompareFunction = equal_to<T>,
//          typename Allocator = allocator<T> >
// class hash_set {};
// CompareFunction的默认比较函数是equal_to，
// 与标准关联容器的惯例不同，标准关联容器的默认比较函数是less。
// SGI的哈希容器通过测试两个对象是否相等，
// 而不是是否等价来决定容器中的两个对象是否有相同的值。

// Dinkumware哈希容器的设计采用了一种不同的策略。
// 仍然可以指定对象类型、哈希函数类型、比较函数类型和分配子类型，
// 但是它把默认的哈希函数和比较函数放在：
// 一个单独的类似与traits的hash_compare类中，
// 并把hash_compare作为容器模板的HashingInfo参数的默认实参。

int main() {
  std::unordered_set<int> hs;
  std::unordered_multiset<int> hms;
  std::unordered_map<int, int> hm;
  std::unordered_multimap<int, int> hmm;

  return 0;
}