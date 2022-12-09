#include <iostream>
#include <unordered_map>

// 和unordered_map容器一样，
// unordered_multimap容器也以键值对的形式存储数据，
// 且底层也采用哈希表结构存储各个键值对。
// 两者唯一的不同之处在于，
// unordered_multimap容器可以存储多个键相等的键值对，
// 而unordered_map容器不行。
// 无序容器中存储的各个键值对，都会哈希存到各个桶（本质为链表）中。
// 而对于unordered_multimap容器来说，其存储的所有键值对中，
// 键相等的键值对会被哈希到同一个桶中存储。

// STL标准库中实现unordered_multimap容器和unordered_map容器一样，
// 定义在<unordered_map>头文件，且位于std命名空间中。
// unordered_multimap 容器模板的定义如下所示：
// template <
//   class Key,                         // 键（key）的类型
//   class T,                           // 值（value）的类型
//   class Hash = hash<Key>,            // 底层存储键值对时采用的哈希函数
//   class Pred = equal_to<Key>,        // 判断各个键值对的键相等的规则
//   class Alloc = allocator< pair<const Key,T> > // 指定分配器的类型
// > class unordered_multimap;
// 以上5个参数中，必须显式给前2个参数传值，
// 且除极个别的情况外，最多只使用前4个参数。
// <key,T>前2个参数分别用于确定键值对中键和值的类型，也就是存储键值对的类型。
// Hash = hash<Key>用于指明容器在存储各个键值对时要使用的哈希函数，
//                 默认使用STL标准库提供的hash<key>哈希函数。
//                 注意，默认哈希函数只适用于基本数据类型（包括string类型），
//                 而不适用于自定义的结构体或者类。
// Pred = equal_to<Key>unordered_multimap 容器可以存储多个键相等的键值对，
//                     而判断是否相等的规则，由此参数指定。
//                     默认情况下，使用STL标准库中提供的equal_to<key>规则，
//                     该规则仅支持可直接用==运算符做比较的数据类型。
// 当unordered_multimap容器中存储键值对的键为自定义类型时，
// 默认的哈希函数hash<key>以及比较函数equal_to<key>将不再适用，
// 这种情况下，需要我们自定义适用的哈希函数和比较函数，
// 并分别显式传递给Hash参数和Pred参数。

// unordered_multimap容器的成员方法：
// begin()	          返回指向容器中第一个键值对的正向迭代器。
// end()	            返回指向容器中最后一个键值对之后位置的正向迭代器。
// cbegin()	          和begin()功能相同，只不过在其基础上增加了const属性，
//                    即该方法返回的迭代器不能用于修改容器内存储的键值对。
// cend()	            和end()功能相同，只不过在其基础上，增加了const属性，
//                    即该方法返回的迭代器不能用于修改容器内存储的键值对。
// empty()	          若容器为空，则返回true；否则false。
// size()	            返回当前容器中存有键值对的个数。
// max_size()       	返回容器所能容纳键值对的最大个数，
//                    不同的操作系统，其返回值亦不相同。
// find(key)	        查找以key为键的键值对，
//                    如果找到，则返回一个指向该键值对的正向迭代器；
//                    反之，则返回一个指向容器中最后一个键值对之后位置的迭代器，
//                    与end()方法返回的迭代器相同。
// count(key)	        在容器中查找以key键的键值对的个数。
// equal_range(key)	  返回一个pair对象，其包含2个迭代器，
//                    用于表明当前容器中键为key的键值对所在的范围。
// emplace()	        向容器中添加新键值对，效率比insert()方法高。
// emplace_hint()	    向容器中添加新键值对，效率比insert()方法高。
// insert()         	向容器中添加新键值对。
// erase()           	删除指定键值对。
// clear()	          清空容器，即删除容器中存储的所有键值对。
// swap()	            交换2个unordered_multimap容器存储的键值对，
//                    前提是必须保证这2个容器的类型完全相等。
// bucket_count()	    返回当前容器底层存储键值对时，
//                    使用桶（一个线性链表代表一个桶）的数量。
// max_bucket_count()	返回当前系统中，
//                    unordered_multimap容器底层最多可以使用多少桶。
// bucket_size(n)	    返回第n个桶中存储键值对的数量。
// bucket(key)	      返回以key为键的键值对所在桶的编号。
// load_factor()	    返回unordered_multimap容器中当前的负载因子。
//                    负载因子指的是：
//                    当前容器中存储键值对的size()和使用桶数bucket_count()的比值，
//                    即load_factor()=size()/bucket_count()。
// max_load_factor()	返回或者设置当前unordered_multimap容器的负载因子。
// rehash(n)	        将当前容器底层使用桶的数量设置为n。
// reserve()	        将存储桶的数量（bucket_count()）设置为：
//                    至少容纳count个元（不超过最大负载因子）所需的数量，
//                    并重新整理容器。
// hash_function()  	返回当前容器使用的哈希函数对象。

// 和unordered_map容器相比，
// unordered_multimap容器的类模板中没有重载[]运算符，
// 也没有提供at()成员方法，除此之外它们完全一致。
// 有提供[]运算符和at()成员方法，
// 意味着unordered_multimap容器无法通过指定键获取该键对应的值，
// 因为该容器允许存储多个键相等的键值对，每个指定的键可能对应多个不同的值。
// unordered_multimap模板提供的所有成员方法的用法，
// 都和unordered_map提供的同名成员方法的用法完全相同。
// 对于实现互换2个相同类型unordered_multimap容器的键值对，
// 除了可以调用该容器模板类中提供的swap()成员方法外，
// STL标准库还提供了同名的swap()非成员函数。

template <typename K, typename V>
void print(std::unordered_multimap<K, V>& um) {
  std::cout << "{";
  for (auto it = um.begin(); it != um.end(); ++it) {
    std::cout << it->first << ":" << it->second << ",";
  }
  std::cout << "}[" << um.size() << "]" << std::endl;
}

void func1() {
  std::unordered_multimap<char, int> umm{
      {'a', 9}, {'a', 1}, {'b', 2}, {'c', 3}};
  print(umm);  // {c:3,b:2,a:1,a:9,}[4]

  auto ret = umm.insert(std::make_pair('a', 100));
  print(umm);  // {c:3,b:2,a:100,a:1,a:9,}[5]

  std::cout << umm.count('b') << std::endl;  // 1
  std::cout << umm.count('a') << std::endl;  // 3

  std::pair<std::unordered_multimap<char, int>::iterator,
            std::unordered_multimap<char, int>::iterator>
      p = umm.equal_range('a');
  for (auto it = p.first; it != p.second; ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  // a:100
  // a:1
  // a:9
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}