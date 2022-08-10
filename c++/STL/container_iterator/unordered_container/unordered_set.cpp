#include <iostream>
#include <unordered_set>

// unordered_set容器，可直译为无序set容器，
// 即unordered_set容器和set容器很像，
// 唯一的区别就在于set容器会自行对存储的数据进行排序，
// 而unordered_set容器不会。
// 总的来说，unordered_set容器具有以下几个特性：
// 1.不再以键值对的形式存储数据，而是直接存储数据的值；
// 2.容器内部存储的各个元素的值都互不相等，且不能被修改。
// 3.不会对内部存储的数据进行排序
// 对于unordered_set容器不以键值对的形式存储数据，
// 即unordered_set存储的都是键和值相等的键值对，
// 为了节省存储空间，该类容器在实际存储时选择只存储每个键值对的值。

// 实现unordered_set容器的模板类定义在<unordered_set>头文件，
// 并位于std命名空间中。
// unordered_set容器的类模板定义如下：
// template <class Key,                     // 容器中存储元素的类型
//           class Hash = hash<Key>,        // 确定元素存储位置所用的哈希函数
//           class Pred = equal_to<Key>,    // 判断各个元素是否相等所用的函数
//           class Alloc = allocator<Key>   // 指定分配器对象的类型
//          >class unordered_set;
// 以上4个参数中，只有第一个参数没有默认值，
// 这意味着如果想创建一个unordered_set容器，至少需要手动传递1个参数。
// 事实上，在99%的实际场景中最多只需要使用前3个参数，最后一个参数保持默认值即可。
// Key 确定容器存储元素的类型，
// 如果将unordered_set看做是存储键和值相同的键值对的容器，
// 则此参数则用于确定各个键值对的键和值的类型，
// 因为它们是完全相同的，因此一定是同一数据类型的数据。
// Hash = hash<Key> 指定unordered_set容器底层存储各个元素时，所使用的哈希函数。
//                  需要注意的是，
//                  默认哈希函数hash<Key>只适用于基本数据类型（包括string类型），
//                  而不适用于自定义的结构体或者类。
// Pred = equal_to<Key> unordered_set容器内部不能存储相等的元素，
//                      而衡量2个元素是否相等的标准，取决于该参数指定的函数。
//                      默认情况下，使用STL标准库中提供的equal_to<key>规则，
//                      该规则仅支持可直接用==运算符做比较的数据类型。
// 如果unordered_set容器中存储的元素为自定义的数据类型，
// 则默认的哈希函数hash<key>以及比较函数equal_to<key>将不再适用，
// 只能自己设计适用该类型的哈希函数和比较函数，并显式传递给Hash参数和Pred参数。

// C++ unordered_set容器的成员方法：
// begin()	          返回指向容器中第一个元素的正向迭代器。
// end()              返回指向容器中最后一个元素之后位置的正向迭代器。
// cbegin()	          和begin()功能相同，只不过其返回的是const类型的正向迭代器。
// cend()	            和end()功能相同，只不过其返回的是const类型的正向迭代器。
// empty()            若容器为空，则返回true；否则false。
// size()             返回当前容器中存有元素的个数。
// max_size()         返回容器所能容纳元素的最大个数，不同的操作系统，其返回值亦不相同。
// find(key)	        查找以值为key的元素，如果找到，则返回一个指向该元素的正向迭代器；
//                    反之，则返回一个指向容器中最后一个元素之后位置的迭代器，
//                    与end()方法返回的迭代器相同。
// count(key)	        在容器中查找值为key的元素的个数。
// equal_range(key)   返回一个pair对象，其包含2个迭代器，
//                    用于表明当前容器中值为key的元素所在的范围。
// emplace()	        向容器中添加新元素，效率比insert()方法高。
// emplace_hint()	    向容器中添加新元素，效率比insert()方法高。
// insert()	          向容器中添加新元素。
// erase()	          删除指定元素。
// clear()	          清空容器，即删除容器中存储的所有元素。
// swap()	            交换2个unordered_map容器存储的元素，
//                    前提是必须保证这2个容器的类型完全相等。
// bucket_count()	    返回当前容器底层存储元素时，使用桶（一个线性链表代表一个桶）的数量。
// max_bucket_count()	返回当前系统中，unordered_map容器底层最多可以使用多少桶。
// bucket_size(n)	    返回第n个桶中存储元素的数量。
// bucket(key)	      返回值为key的元素所在桶的编号。
// load_factor()      返回unordered_map容器中当前的负载因子。
//                    负载因子指的是：
//                    当前容器中存储元素的size()和使用桶数bucket_count()的比值，
//                    即load_factor()=size()/bucket_count()。
// max_load_factor()  返回或者设置当前unordered_map容器的负载因子。
// rehash(n)          将当前容器底层使用桶的数量设置为n。
// reserve()          将存储桶的数量（bucket_count()）设置为：
//                    至少容纳count个元（不超过最大负载因子）所需的数量，
//                    并重新整理容器。
// hash_function()	  返回当前容器使用的哈希函数对象。

// 此容器模板类中没有重载[]运算符，也没有提供at()成员方法。
// 不仅如此，由于unordered_set容器内部存储的元素值不能被修改，
// 因此无论使用那个迭代器方法获得的迭代器，都不能用于修改容器中元素的值。
// 对于实现互换2个相同类型unordered_set容器的所有元素，
// 除了调用swap()成员方法外，
// 还可以使用STL标准库提供的swap()非成员函数，它们具有相同的名称，
// 用法也相同（都只需要传入2个参数即可），仅是调用方式上有差别。

template <typename K>
void print(std::unordered_set<K>& us) {
  for (auto it = us.begin(); it != us.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "[" << us.size() << "]" << std::endl;
}

std::unordered_set<std::string> retuset() {
  std::unordered_set<std::string> tempuset{"hello1", "world1", "nihao1"};
  return tempuset;
}

void func1() {
  // 创建C++ unordered_set容器：
  // 1.过调用unordered_set模板类的默认构造函数，
  //   可以创建空的unordered_set容器，
  //   该容器底层采用默认的哈希函数hash<Key>和比较函数equal_to<Key>：
  std::unordered_set<std::string> us1;
  print(us1);  // [0]

  // 2.创建unordered_set容器的同时，可以完成初始化操作：
  std::unordered_set<std::string> us2{"hello", "world", "nihao"};
  print(us2);  // nihao world hello [3]

  // 3.调用unordered_set模板中提供的复制（拷贝）构造函数，
  //   将现有unordered_set容器中存储的元素，
  //   全部用于为新建unordered_set容器初始化：
  std::unordered_set<std::string> us3(us2);
  std::unordered_set<std::string> us4 = us2;
  print(us3);  // nihao world hello [3]
  print(us4);  // nihao world hello [3]
  // C++11标准中还向unordered_set模板类增加了移动构造函数，
  // 即以右值引用的方式，
  // 利用临时unordered_set容器中存储的所有元素，给新建容器初始化：
  std::unordered_set<std::string> us5(retuset());
  std::unordered_set<std::string> us6 = retuset();
  print(us5);  // world1 nihao1 hello1 [3]
  print(us6);  // world1 nihao1 hello1 [3]
  // 无论是调用复制构造函数还是拷贝构造函数，必须保证2个容器的类型完全相同。

  // 4.如果不想全部拷贝，可以使用unordered_set类模板提供的迭代器，
  //   在现有unordered_set容器中选择部分区域内的元素，
  //   为新建unordered_set容器初始化：
  std::unordered_set<std::string> us7(++us2.begin(), us2.end());
  print(us7);  // hello world [2]
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