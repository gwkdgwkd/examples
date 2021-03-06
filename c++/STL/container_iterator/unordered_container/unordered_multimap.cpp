#include <iostream>
#include <unordered_map>

using namespace std;

// 和unordered_map容器一样，unordered_multimap容器也以键值对的形式存储数据，且底层也采用哈希表结构存储各个键值对。
// 两者唯一的不同之处在于，unordered_multimap容器可以存储多个键相等的键值对，而unordered_map容器不行。
// 无序容器中存储的各个键值对，都会哈希存到各个桶（本质为链表）中。而对于unordered_multimap容器来说，其存储的所有键值对中，键相等的键值对会被哈希到同一个桶中存储。

// STL标准库中实现unordered_multimap容器的模板类并没有定义在以自己名称命名的头文件中，而是和unordered_map容器一样，定义在<unordered_map>头文件，且位于std命名空间中。
// unordered_multimap 容器模板的定义如下所示：
//     template < class Key,      //键（key）的类型
//                class T,        //值（value）的类型
//                class Hash = hash<Key>,  //底层存储键值对时采用的哈希函数
//                class Pred = equal_to<Key>,  //判断各个键值对的键相等的规则
//                class Alloc = allocator< pair<const Key,T> > // 指定分配器对象的类型
//                > class unordered_multimap;
// 以上5个参数中，必须显式给前2个参数传值，且除极个别的情况外，最多只使用前4个参数
// <key,T> 前2个参数分别用于确定键值对中键和值的类型，也就是存储键值对的类型。
// Hash = hash<Key> 用于指明容器在存储各个键值对时要使用的哈希函数，默认使用STL标准库提供的hash<key>哈希函数。
//                  注意，默认哈希函数只适用于基本数据类型（包括string类型），而不适用于自定义的结构体或者类。
// Pred = equal_to<Key> unordered_multimap 容器可以存储多个键相等的键值对，而判断是否相等的规则，由此参数指定。
//                      默认情况下，使用STL标准库中提供的equal_to<key>规则，该规则仅支持可直接用==运算符做比较的数据类型。
// 当unordered_multimap容器中存储键值对的键为自定义类型时，默认的哈希函数hash<key>以及比较函数equal_to<key>将不再适用，
// 这种情况下，需要我们自定义适用的哈希函数和比较函数，并分别显式传递给Hash参数和Pred参数。

// unordered_multimap容器的成员方法:
// begin()	返回指向容器中第一个键值对的正向迭代器。
// end()	返回指向容器中最后一个键值对之后位置的正向迭代器。
// cbegin()	和begin()功能相同，只不过在其基础上增加了const属性，即该方法返回的迭代器不能用于修改容器内存储的键值对。
// cend()	和end()功能相同，只不过在其基础上，增加了const属性，即该方法返回的迭代器不能用于修改容器内存储的键值对。
// empty()	若容器为空，则返回true；否则false。
// size()	返回当前容器中存有键值对的个数。
// max_size()	返回容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// find(key)	查找以key为键的键值对，如果找到，则返回一个指向该键值对的正向迭代器；反之，则返回一个指向容器中最后一个键值对之后位置的迭代器（如果end()方法返回的迭代器）。
// count(key)	在容器中查找以key键的键值对的个数。
// equal_range(key)	返回一个pair对象，其包含2个迭代器，用于表明当前容器中键为key的键值对所在的范围。
// emplace()	向容器中添加新键值对，效率比insert()方法高。
// emplace_hint()	向容器中添加新键值对，效率比insert()方法高。
// insert()	向容器中添加新键值对。
// erase()	删除指定键值对。
// clear()	清空容器，即删除容器中存储的所有键值对。
// swap()	交换2个unordered_multimap容器存储的键值对，前提是必须保证这2个容器的类型完全相等。
// bucket_count()	返回当前容器底层存储键值对时，使用桶（一个线性链表代表一个桶）的数量。
// max_bucket_count()	返回当前系统中，unordered_multimap容器底层最多可以使用多少桶。
// bucket_size(n)	返回第n个桶中存储键值对的数量。
// bucket(key)	返回以key为键的键值对所在桶的编号。
// load_factor()	返回unordered_multimap容器中当前的负载因子。负载因子，指的是的当前容器中存储键值对的数量（size()）和使用桶数（bucket_count()）的比值，即load_factor()=size()/bucket_count()。
// max_load_factor()	返回或者设置当前unordered_multimap容器的负载因子。
// rehash(n)	将当前容器底层使用桶的数量设置为n。
// reserve()	将存储桶的数量（也就是bucket_count()方法的返回值）设置为至少容纳count个元（不超过最大负载因子）所需的数量，并重新整理容器。
// hash_function()	返回当前容器使用的哈希函数对象。
// 和unordered_map容器相比，unordered_multimap容器的类模板中没有重载[]运算符，也没有提供at()成员方法，除此之外它们完全一致。
// 有提供[]运算符和at()成员方法，意味着unordered_multimap容器无法通过指定键获取该键对应的值，因为该容器允许存储多个键相等的键值对，每个指定的键可能对应多个不同的值。
// 对于实现互换2个相同类型unordered_multimap容器的键值对，除了可以调用该容器模板类中提供的swap()成员方法外，STL标准库还提供了同名的swap()非成员函数。

std::unordered_multimap<std::string, std::string> retUmmap() {
  std::unordered_multimap<std::string, std::string> tempummap{
      {"Python教程", "http://c.biancheng.net/python/"},
      {"Java教程", "http://c.biancheng.net/java/"},
      {"Linux教程", "http://c.biancheng.net/linux/"}};
  return tempummap;
}

int main() {
  // 创建C++ unordered_multimap容器的方法
  // 1 利用unordered_multimap容器类模板中的默认构造函数，可以创建空的unordered_multimap容器
  std::unordered_multimap<std::string, std::string> myummap1;
  // 2 在创建空unordered_multimap容器的基础上，可以完成初始化操作
  unordered_multimap<string, string> myummap2{
      {"Python教程", "http://c.biancheng.net/python/"},
      {"Java教程", "http://c.biancheng.net/java/"},
      {"Linux教程", "http://c.biancheng.net/linux/"}};
  // 3 unordered_multimap模板中还提供有复制（拷贝）构造函数，可以实现在创建unordered_multimap容器的基础上，用另一unordered_multimap容器中的键值对为其初始化
  unordered_multimap<string, string> myummap31(myummap2);
  // C++11标准中还向unordered_multimap模板类增加了移动构造函数，即以右值引用的方式将临时unordered_multimap容器中存储的所有键值对，全部复制给新建容器
  std::unordered_multimap<std::string, std::string> myummap32(retUmmap());
  // 无论是调用复制构造函数还是拷贝构造函数，必须保证2个容器的类型完全相同。
  // 4 如果不想全部拷贝，可以使用unordered_multimap类模板提供的迭代器，在现有unordered_multimap容器中选择部分区域内的键值对，为新建unordered_multimap容器初始化
  unordered_multimap<string, string> m4(++myummap2.begin(), myummap2.end());

  std::unordered_multimap<std::string, std::string> myummap;
  myummap.emplace("Python教程", "http://c.biancheng.net/python/");
  myummap.emplace("STL教程", "http://c.biancheng.net/stl/");
  myummap.emplace("Java教程", "http://c.biancheng.net/java/");
  myummap.emplace("C教程", "http://c.biancheng.net");
  myummap.emplace("C教程", "http://c.biancheng.net/c/");
  cout << "myummmap size = " << myummap.size() << endl;  // myummmap size = 5
  for (auto iter = myummap.begin(); iter != myummap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // C教程 http://c.biancheng.net/c/
  // C教程 http://c.biancheng.net
  // Java教程 http://c.biancheng.net/java/
  // STL教程 http://c.biancheng.net/stl/
  // Python教程 http://c.biancheng.net/python/

  // unordered_multimap模板提供的所有成员方法的用法，都和unordered_map提供的同名成员方法的用法完全相同（仅是调用者发生了改变）

  return 0;
}