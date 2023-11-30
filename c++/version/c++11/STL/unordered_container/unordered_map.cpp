#include <iostream>
#include <unordered_map>
#include <utility>

// unordered_map容器，直译过来就是无序map容器的意思。
// 所谓无序，指的是unordered_map不会像map那样对存储的数据进行排序。
// 换句话说，unordered_map容器和map容器仅有一点不同，
// 即map容器中存储的数据是有序的，而unordered_map容器中是无序的。
// unordered_map容器和map容器一样，以键值对的形式存储数据，
// 存储的各个键值对的键互不相同且不允许被修改。
// 但由于unordered_map底层采用的是哈希表存储结构，不具有对数据的排序功能，
// 所以此容器内部不会自行对存储的键值对进行排序。

// unordered_map定义在<unordered_map>中，并位于std命名空间中。
// unordered_map容器模板的定义如下所示：
// template <
//   class Key,                         键值对中键的类型
//   class T,                           键值对中值的类型
//   class Hash = hash<Key>,            容器内部存储键值对所用的哈希函数
//   class Pred = equal_to<Key>,        判断各个键值对键相同的规则
//   class Alloc = allocator<pair<const Key,T>>  指定分配器的类型
// > class unordered_map;
// 以上5个参数中，必须使用前2个参数，并且除特殊情况外，最多只需要使用前4个参数。
// <key,T> 前2个参数分别用于确定键值对中键和值的类型，也就是存储键值对的类型。
// Hash = hash<Key> 用于指明容器在存储各个键值对时要使用的哈希函数，
//                  默认使用STL标准库提供的hash<key>哈希函数。
//                  注意，默认哈希函数只适用于基本数据类型（包括string类型），
//                  而不适用于自定义的结构体或者类。
// Pred = equal_to<Key> unordered_map容器中存储的各个键值对的键是不能相等的，
//                      而判断是否相等的规则，就由此参数指定。
//                      默认情况下，使用STL标准库中提供的equal_to<key>规则，
//                      该规则仅支持可直接用==运算符做比较的数据类型。
// 当无序容器中存储键值对的键为自定义类型时，默认的hash以及equal_to将不再适用，
// 只能自己设计适用该类型的哈希函数和比较函数，并显式传递给Hash参数和Pred参数。

// unordered_map容器的成员方法:
// begin()           	返回指向容器中第一个键值对的正向迭代器。
// end()	            返回指向容器中最后一个键值对之后位置的正向迭代器。
// cbegin()	          和begin()功能相同，只不过增加了const，不能修改键值对。
// cend()	            和end()功能相同，只不过增加了const，不能修改键值对。
// empty()	          若容器为空，则返回true，否则false。
// size()            	返回当前容器中存有键值对的个数。
// max_size()         返回容器所能容纳键值对的最大个数，不同的系统返回值不相同。
// operator[key]      该模板类中重载了[]运算符，可以像访问数组中元素那样，
//                    只要给定某个键值对的键key，就可以获取该键对应的值。
//                    注意，如果当前容器中没有以key为键的键值对，
//                    则其会使用该键向当前容器中插入一个新键值对。
// at(key)	          返回键key对应的值，如果key不存在则抛出out_of_range。
// find(key)          查找key的键值对，如果找到则返回指向该键值对的正向迭代器；
//                    反之，则返回一个指向容器中最后一个键值对之后位置的迭代器，
//                    与end()方法返回的迭代器相同。
// count(key)         在容器中查找以key键的键值对的个数。
// equal_range(key)	  返回一个pair对象，其包含2个迭代器，
//                    用于表明当前容器中键为key的键值对所在的范围。
// emplace()          向容器中添加新键值对，效率比insert()方法高。
// emplace_hint()     向容器中添加新键值对，效率比insert()方法高。
// insert()	          向容器中添加新键值对。
// erase()	          删除指定键值对。
// clear()	          清空容器，即删除容器中存储的所有键值对。
// swap()	            交换2个unordered_map的键值对，它们的类型完全相等。
// bucket_count()     返回当前容器底层存储键值对时，使用桶的数量。
// max_bucket_count() 返回当前系统中，unordered_map容器底层最多可以使用多少桶。
// bucket_size(n)     返回第n个桶中存储键值对的数量。
// bucket(key)	      返回以key为键的键值对所在桶的编号。
// load_factor()      返回unordered_map容器中当前的负载因子，负载因子指的是：
//                    容器中存储键值对的数量size()和使用桶数bucket_count()的比值，
//                    即load_factor()=size()/bucket_count()。
// max_load_factor()  返回或者设置当前unordered_map容器的负载因子。
// rehash(n)          将当前容器底层使用桶的数量设置为n。
// reserve()          将存储桶的数量bucket_count()设置为，
//                    至少容纳count个元（<最大负载因子）所需的数量，并重新整理容器。
// hash_function()    返回当前容器使用的哈希函数对象。

// C++无序容器的底层实现机制
// 不仅是unordered_map容器，所有无序容器的底层实现都采用的是哈希表存储结构。
// 更准确地说，是用链地址法（又称开链法）解决数据存储位置发生冲突的哈希表。
// 当使用无序容器存储键值对时，会先申请一整块连续的存储空间，
// 但此空间并不用来直接存储键值对，而是存储各个链表的头指针，
// 各键值对真正的存储位置是各个链表的节点。
// STL标准库通常选用vector容器存储各个链表的头指针。
// 在C++ STL标准库中，将各个链表称为桶，每个桶都有自己的编号（从0开始）。
// 当有新键值对存储到无序容器中时，整个存储过程分为如下几步：
// 1.将该键值对中键的值带入设计好的哈希函数，会得到一个哈希值（整数，用H表示）；
// 2.将H和桶的数量n做整除运算（即H%n），该结果表示应将此键值对存储到的桶的编号；
// 3.建立一个新节点存储此键值对，同时将该节点链接到相应编号的桶上。

// 哈希表存储结构还有一个重要的属性，称为负载因子（load factor）。
// 该属性同样适用于无序容器，用于衡量容器存储键值对的空/满程度，
// 即负载因子越大，意味着容器越满，即各链表中挂载着越多的键值对，
// 这无疑会降低容器查找目标键值对的效率，反之，负载因子越小，
// 容器肯定越空，但并不一定各个链表中挂载的键值对就越少。
// 无序容器中，负载因子的计算方法为：负载因子=容器存储的总键值对/桶数，
// 默认情况下，无序容器的最大负载因子为1.0，如果操作无序容器过程中，
// 使得最大复杂因子超过了默认值，则容器会自动增加桶数，
// 并重新进行哈希，以此来减小负载因子的值，需要注意的是，
// 此过程会导致容器迭代器失效，但指向单个键值对的引用或者指针仍然有效。
// 这也解释了为什么在操作无序容器过程中，键值对的存储顺序有时会莫名的变动。

// C++ STL标准库为了方便用户更好地管控无序容器底层使用的哈希表存储结构，
// 各个无序容器的模板类中都提供了以下成员方法：
// bucket_count()     返回当前容器底层存储键值对时，使用桶的数量。
// max_bucket_count() 返回系统中unordered_map容器底层最多可以使用多少个桶。
// bucket_size(n)     返回第n个桶中存储键值对的数量。
// bucket(key)        返回以key为键的键值对所在桶的编号。
// load_factor()      返回unordered_map容器中当前的负载因子。
// max_load_factor()  返回或者设置当前unordered_map容器的最大负载因子。
// rehash(n)          尝试重新调整桶的数量为等于或大于n的值。
//                    如果n大于当前容器使用的桶数，则该方法会是容器重新哈希，
//                    新的桶数将等于或大于n，反之，如果n的值小于当前容器使用的桶数，
//                    则调用此方法可能没有任何作用。
// reserve(n)         将容器使用的桶数设置为，最适合存储n个元素的桶数。
// hash_function()    返回当前容器使用的哈希函数对象。

// unordered_map容器迭代器的类型为前向迭代器（又称正向迭代器）。
// 假设p是一个前向迭代器，则其只能进行*p、p++、++p操作，
// 且2个前向迭代器之间只能用==和!=运算符做比较。
// equal_range(key)很少用于unordered_map容器，
// 因为该容器中存储的都是键不相等的键值对，
// 即便调用该成员方法，得到的2个迭代器所表示的范围中，最多只包含1个键值对。
// 事实上，该成员方法更适用于unordered_multimap容器。

template <typename K, typename V>
void print(std::unordered_map<K, V>& um) {
  std::cout << "{";
  for (auto it = um.begin(); it != um.end(); ++it) {
    std::cout << it->first << ":" << it->second << ",";
  }
  std::cout << "}[" << um.size() << "]" << std::endl;
}

void func1() {
  // 创建unordered_map容器：

  // 1.通过调用unordered_map的默认构造函数，可以创建空的unordered_map
  std::unordered_map<std::string, std::string> um1;
  print(um1);  // {}[0]

  // 2.在创建unordered_map容器的同时，可以完成初始化操作
  std::unordered_map<int, char> um2{{1, 'a'}, {2, 'b'}, {3, 'c'}};
  print(um2);  // {3:c,2:b,1:a,}[3]

  // 3.调用unordered_map模板中提供的复制（拷贝）构造函数，
  //   将现有unordered_map容器中存储的键值对，复制给新建unordered_map
  std::unordered_map<int, char> um3(um2);
  std::unordered_map<int, char> um4 = um2;
  print(um2);  // {3:c,2:b,1:a,}[3]
  print(um3);  // {3:c,2:b,1:a,}[3]

  // C++11标准中还向unordered_map模板类增加了移动构造函数，
  // 即以右值引用的方式将临时unordered_map中存储的所有键值对，全部移动给新容器。
  // 无论是调用拷贝构造函数还是移动构造函数，必须保证2个容器的类型完全相同。
  auto retUmap = []() -> std::unordered_map<int, char> {
    std::unordered_map<int, char> ret{{4, 'd'}, {5, 'e'}, {6, 'f'}};
    return ret;
  };
  std::unordered_map<int, char> um5(retUmap());
  std::unordered_map<int, char> um6 = retUmap();
  print(um5);  // {6:f,5:e,4:d,}[3]
  print(um6);  // {6:f,5:e,4:d,}[3]

  // 4.如果不想全部拷贝，可以使用unordered_map类模板提供的迭代器，
  //   在现有unordered_map中选择部分区域内的键值对，为新容器初始化
  std::unordered_map<int, char> um7(++um2.begin(), um2.end());
  print(um7);  // {1:a,2:b,}[2]
}

void func2() {
  // unordered_map获取元素的方法：
  std::unordered_map<int, int> um;
  // 1.unordered_map容器类模板中，实现了对[]运算符的重载，
  //   可以像利用下标访问普通数组那样，通过目标键值对的键获取到该键对应的值；
  um[1];      // 添加，key为1，value为int类型的默认值
  um[2] = 3;  // 添加，key为2，value为3
  print(um);  // {2:3,1:0,}[2]
  um[1] = 2;  // 更新
  print(um);  // {2:3,1:2,}[2]

  // 2.unordered_map类模板中，还提供有at()成员方法，和使用[]运算符一样，
  //   at()成员方法也需要根据指定的键，才能从容器中找到该键对应的值；
  //   不同之处在于，如果在当前容器中查找失败，不会新增而是抛出out_of_range。
  std::cout << um.at(2) << std::endl;  // 3
  try {
    std::cout << um.at(8) << std::endl;
  } catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
  }
  // _Map_base::at

  // 3.[]运算符和at()成员方法基本能满足大多数场景的需要。
  //   除此之外，还可以借助unordered_map模板中提供的find()成员方法；
  //   和前面方法不同的是，通过find()方法得到的是一个正向迭代器，
  //   该迭代器的指向分以下2种情况：
  //   当find()成功找到键值对时，其返回的迭代器就指向该键值对；
  //   当find()查找失败时，返回的迭代器和end()方法返回的迭代器一样。
  auto it = um.find(2);
  std::cout << it->first << ":" << it->second << std::endl;  // 2:3
  if (um.find(8) == um.end()) {
    std::cout << "not found" << std::endl;
  }
  // not found

  // 4.除了find()之外，甚至可以借助begin()/end()或者cbegin()/cend()，
  //   通过遍历整个容器中的键值对来找到目标键值对。
  for (auto it = um.cbegin(); it != um.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  // 2:3
  // 1:2
}

void func3() {
  std::unordered_map<std::string, std::string> um;
  std::cout << "umap初始桶数: " << um.bucket_count() << std::endl;         // 1
  std::cout << "umap初始负载因子: " << um.load_factor() << std::endl;      // 0
  std::cout << "umap最大负载因子: " << um.max_load_factor() << std::endl;  // 1

  um.reserve(9);
  std::cout << "umap新桶数: " << um.bucket_count() << std::endl;     // 11
  std::cout << "umap新负载因子: " << um.load_factor() << std::endl;  // 0

  um["1"] = "x";
  um["2"] = "y";
  um["3"] = "z";
  std::cout << "以1为键的键值对，位于桶的编号为:" << um.bucket("1")
            << std::endl;
  // 以1为键的键值对，位于桶的编号为:1
  std::cout << "以2为键的键值对，位于桶的编号为:" << um.bucket("2")
            << std::endl;
  // 以2为键的键值对，位于桶的编号为:5

  auto fn = um.hash_function();
  std::cout << "计算以2为键的键值对，位于桶的编号为："
            << fn("2") % (um.bucket_count()) << std::endl;
  // 计算以2为键的键值对，位于桶的编号为：5
}

void func4() {
  // 在操作unordered_map容器过程（尤其是向容器中添加新键值对）中，
  // 一旦当前容器的负载因子超过最大负载因子（默认值为1.0），
  // 该容器就会适当增加桶的数量（通常是翻一倍），并自动执行rehash()，
  // 重新调整各个键值对的存储位置（重哈希），此过程很可能导致迭代器失效。
  // 所谓迭代器失效，针对的是那些用于表示容器内某个范围的迭代器，
  // 由于重哈希会重新调整每个键值对的存储位置，所以容器重哈希之后，
  // 之前表示特定范围的迭代器很可能无法再正确表示该范围。
  // 但是，重哈希并不会影响那些指向单个键值对元素的迭代器。
  std::unordered_map<int, int> um;
  for (int i = 1; i <= 50; i++) {
    um.emplace(i, i);
  }
  auto pair = um.equal_range(1);
  for (auto iter = pair.first; iter != pair.second; ++iter) {
    std::cout << iter->first << " ";
  }
  std::cout << std::endl;  // 1

  um.max_load_factor(3.0);
  um.rehash(10);
  for (auto iter = pair.first; iter != pair.second; ++iter) {
    std::cout << iter->first << " ";
  }
  std::cout << std::endl;
  // 1 35 19 2 36 20 3 37 21 4 38 22 5 39 23 6 40 24 7 41 25 8 42 26 9 43 27 10
  // 44 28 11 45 29 12 46 13 30 47

  // 用于遍历整个容器的begin()/end()和cbegin()/cend()迭代器对，
  // 重哈希只会影响遍历容器内键值对的顺序，整个遍历的操作仍然可以顺利完成。
}

void func5() {
  // insert：
  // 1.insert()方法可以将pair类型的键值对元素添加到unordered_map容器中
  //   以普通方式传递参数：
  //   pair<iterator,bool> insert(const value_type& val);
  //   以右值引用的方式传递参数：
  //   template <class P> pair<iterator,bool> insert(P&& val);
  //   以上2种格式中，参数val表示要添加到容器中的目标键值对元素；
  //   该方法的返回值为pair类型值，内部包含一个iterator迭代器和bool变量：
  //   当insert()将val成功添加到容器中时，返回的迭代器指向新添加的键值对，
  //   bool值为true，当insert()添加键值对失败时，
  //   意味着当前容器中本就存储有和要添加键值对的键相等的键值对，
  //   返回的迭代器将指向这个导致插入操作失败的迭代器，bool值为false。
  std::unordered_map<char, int> um;
  std::pair<char, int> p('a', 2);
  std::pair<std::unordered_map<char, int>::iterator, bool> ret = um.insert(p);
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // true,a:2
  ret = um.insert(std::make_pair('b', 3));
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // true,b:3
  ret = um.insert(std::make_pair('b', 5));
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // false,b:3

  // 2.insert()方法还可以指定新键值对要添加到容器中的位置
  //   以普通方式传递val参数：
  //   iterator insert(const_iterator hint, const value_type& val);
  //   以右值引用方法传递val参数：
  //   template <class P> iterator insert(const_iterator hint, P&& val);
  //   以上2种语法格式中，hint参数为迭代器，用于指定新键值对要添加到容器中的位置；
  //   val参数指的是要添加容器中的键值对；方法的返回值为迭代器：
  //   如果insert()方法成功添加键值对，该迭代器指向新添加的键值对；
  //   如果insert()方法添加键值对失败，则表示容器中本就包含有相同键的键值对，
  //   该方法返回的迭代器就指向容器中键相同的键值对；
  //   注意，以上2种语法格式中，虽然通过hint参数指定了新键值对添加到容器中的位置，
  //   但真正存储的位置，并不是hint参数说了算，最终的存储位置仍取决于该键值对的键。
  p = std::make_pair('c', 4);
  std::unordered_map<char, int>::iterator it;
  it = um.insert(um.begin(), p);
  std::cout << it->first << ":" << it->second << std::endl;  // c:4
  it = um.insert(um.begin(), std::make_pair('c', 3));
  std::cout << it->first << ":" << it->second << std::endl;  // c:4

  // 3.insert()方法还支持将某一个unordered_map容器中指定区域内的所有键值对，
  //   复制到另一个unordered_map容器中：
  //   template <class InputIterator>
  //   void insert(InputIterator first, InputIterator last);
  //   其中first和last都为迭代器，[first,last)表示其它容器中键值对的区域。
  std::unordered_map<char, int> um1{{'h', 9}, {'i', 8}, {'u', 11}};
  print(um1);  // {u:11,i:8,h:9,}[3]
  um.insert(++um1.begin(), um1.end());
  print(um);  // {h:9,i:8,c:4,b:3,a:2,}[5]

  // 4.insert()方法还支持一次向unordered_map容器添加多个键值对
  //   void insert(initializer_list<value_type> il);
  //   其中，il参数指的是可以用初始化列表的形式指定多个键值对元素。
  um.insert({{'e', 6}, {'f', 7}, {'g', 8}});
  print(um);  // {g:8,f:7,e:6,h:9,i:8,c:4,b:3,a:2,}[8]
}

void func6() {
  // C++11标准为unordered_map容器新增了emplace()和emplace_hint()，
  // 向已有unordered_map容器中添加新键值对，可以通过调用insert()方法，
  // 但其实还有更好的方法，即使用emplace()或者emplace_hint()方法，
  // 它们完成向容器中添加新键值对的效率，要比insert()方法高。

  // emplace()方法的用法很简单，其语法格式如下：
  // template <class... Args>
  // pair<iterator, bool> emplace(Args&&... args);
  // 其中，参数args表示可直接向该方法传递创建新键值对所需要的2个元素的值，
  // 其中第一个元素将作为键值对的键，另一个作为键值对的值。
  // 也就是说，该方法无需我们手动创建键值对，其内部会自行完成此工作。
  // 另外需要注意的是，该方法的返回值为pair类型值，
  // 其包含一个迭代器和一个bool类型值：
  // 当emplace()成功添加新键值对时，返回的迭代器指向新添加的键值对，
  // bool值为true，当emplace()添加新键值对失败时，
  // 说明容器中本就包含一个键相等的键值对，
  // 此时返回的迭代器指向的就是容器中键相同的这个键值对，bool值为false。
  std::unordered_map<char, int> um;
  std::pair<char, int> p('a', 2);
  std::pair<std::unordered_map<char, int>::iterator, bool> ret = um.emplace(p);
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // true,a:2
  ret = um.emplace(std::make_pair('b', 3));
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // true,b:3
  ret = um.emplace(std::make_pair('b', 5));
  std::cout << std::boolalpha << ret.second << "," << ret.first->first << ":"
            << ret.first->second << std::endl;  // false,b:3

  // emplace_hint()方法的语法格式如下：
  // template <class... Args>
  // iterator emplace_hint(const_iterator position, Args&&... args);
  // 和emplace()方法相同，emplace_hint()方法内部会自行构造新键值对，
  // 因此只需向其传递构建该键值对所需的2个元素（第一个作为键，另一个作为值）即可。
  // 不同之处在于：
  // emplace_hint()方法的返回值仅是一个迭代器，而不再是pair类型变量。
  //               当该方法将新键值对成功添加到容器中时，
  //               返回的迭代器指向新添加的键值对，反之，如果添加失败，
  //               该迭代器指向的是容器中和要添加键值对键相同的那个键值对。
  // emplace_hint()方法还需要传递一个迭代器作为第一个参数，
  //               该迭代器表明将新键值对添加到容器中的位置。
  //               需要注意的是，新键值对添加到容器中的位置，
  //               并不是此迭代器说了算，最终仍取决于该键值对的键的值。
  // 可以这样理解，emplace_hint()方法中传入的迭代器，
  // 仅是给unordered_map容器提供一个建议，并不一定会被容器采纳。
  std::unordered_map<char, int>::iterator it =
      um.emplace_hint(um.begin(), 'k', 15);
  std::cout << it->first << ":" << it->second << std::endl;  // k:15
  it = um.emplace_hint(um.begin(), 'k', 15);
  std::cout << it->first << ":" << it->second << std::endl;  // k:15
}

void func7() {
  std::unordered_map<char, int> um{{'h', 9}, {'i', 8}, {'u', 11},
                                   {'a', 1}, {'b', 2}, {'c', 3}};
  print(um);  // {c:3,u:11,b:2,i:8,a:1,h:9,}[6]

  // unordered_map删除元素：erase()和clear()
  // erase()：删除unordered_map容器中指定的键值对；
  // clear()：删除unordered_map容器中所有的键值对，即清空容器。
  // unordered_map 3种语法格式的erase()方法:
  // 1.erase()方法可以接受一个正向迭代器，并删除该迭代器指向的键值对。
  //   iterator erase(const_iterator position);
  //   其中position为指向容器中某个键值对的迭代器，
  //   该方法会返回一个指向被删除键值对之后位置的迭代器。
  //   如果erase()方法删除的是容器存储的最后一个键值对，则该方法返回的迭代器，
  //   将指向容器中最后一个键值对之后的位置（等同于end()方法返回的迭代器）。
  auto it = um.erase(um.begin());
  std::cout << it->first << ":" << it->second << std::endl;  // u:11
  print(um);  // {u:11,b:2,i:8,a:1,h:9,}[5]

  // 2.还可以直接将要删除键值对的键作为参数直接传给erase()方法，
  //   该方法会自行去unordered_map中找和给定键相同的键值对，将其删除。
  //   size_type erase(const key_type& k);
  //   其中，k表示目标键值对的键的值；
  //   该方法会返回一个整数，其表示成功删除的键值对的数量。
  std::cout << "befor size = " << um.size() << std::endl;  // befor size = 5
  int count = um.erase('a');
  std::cout << count << ", size = " << um.size() << std::endl;  // 1, size = 4
  print(um);  // {u:11,b:2,i:8,h:9,}[4]

  um.clear();
  print(um);  // {}[0]
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
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
    case 5:
      func6();
      break;
    case 6:
      func7();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}