#include <iostream>
#include <unordered_map>
#include <utility>

using namespace std;

// unordered_map容器，直译过来就是"无序map容器"的意思。所谓“无序”，指的是unordered_map容器不会像map容器那样对存储的数据进行排序。
// 换句话说，unordered_map容器和map容器仅有一点不同，即map容器中存储的数据是有序的，而unordered_map容器中是无序的。
// unordered_map容器和map容器一样，以键值对（pair类型）的形式存储数据，存储的各个键值对的键互不相同且不允许被修改。
// 但由于unordered_map容器底层采用的是哈希表存储结构，该结构本身不具有对数据的排序功能，所以此容器内部不会自行对存储的键值对进行排序。

// unordered_map容器在<unordered_map>头文件中，并位于std命名空间中。
// unordered_map容器模板的定义如下所示：
//  template < class Key,                        //键值对中键的类型
//             class T,                          //键值对中值的类型
//             class Hash = hash<Key>,           //容器内部存储键值对所用的哈希函数
//             class Pred = equal_to<Key>,       //判断各个键值对键相同的规则
//             class Alloc = allocator< pair<const Key,T> >  // 指定分配器对象的类型
//             > class unordered_map;
// 以上5个参数中，必须显式给前2个参数传值，并且除特殊情况外，最多只需要使用前4个参数
//  <key,T> 前2个参数分别用于确定键值对中键和值的类型，也就是存储键值对的类型。
//  Hash = hash<Key> 用于指明容器在存储各个键值对时要使用的哈希函数，默认使用STL标准库提供的hash<key>哈希函数。
//                   注意，默认哈希函数只适用于基本数据类型（包括string类型），而不适用于自定义的结构体或者类。
//  Pred = equal_to<Key> 要知道，unordered_map容器中存储的各个键值对的键是不能相等的，而判断是否相等的规则，就由此参数指定。
//                       默认情况下，使用STL标准库中提供的equal_to<key>规则，该规则仅支持可直接用==运算符做比较的数据类型。
// 当无序容器中存储键值对的键为自定义类型时，默认的哈希函数hash以及比较函数equal_to将不再适用，只能自己设计适用该类型的哈希函数和比较函数，并显式传递给Hash参数和Pred参数。

// unordered_map容器的成员方法:
// begin()	返回指向容器中第一个键值对的正向迭代器。
// end()	返回指向容器中最后一个键值对之后位置的正向迭代器。
// cbegin()	和begin()功能相同，只不过在其基础上增加了const属性，即该方法返回的迭代器不能用于修改容器内存储的键值对。
// cend()	和end()功能相同，只不过在其基础上，增加了const属性，即该方法返回的迭代器不能用于修改容器内存储的键值对。
// empty()	若容器为空，则返回true；否则false。
// size()	返回当前容器中存有键值对的个数。
// max_size() 返回容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// operator[key] 该模板类中重载了[]运算符，其功能是可以向访问数组中元素那样，只要给定某个键值对的键key，就可以获取该键对应的值。
//               注意，如果当前容器中没有以key为键的键值对，则其会使用该键向当前容器中插入一个新键值对。
// at(key)	返回容器中存储的键key对应的值，如果key不存在，则会抛出out_of_range异常。
// find(key) 查找以key为键的键值对，如果找到，则返回一个指向该键值对的正向迭代器；反之，则返回一个指向容器中最后一个键值对之后位置的迭代器（如果end()方法返回的迭代器）。
// count(key) 在容器中查找以key键的键值对的个数。
// equal_range(key)	返回一个pair对象，其包含2个迭代器，用于表明当前容器中键为key的键值对所在的范围。
// emplace() 向容器中添加新键值对，效率比insert()方法高。
// emplace_hint() 向容器中添加新键值对，效率比insert()方法高。
// insert()	向容器中添加新键值对。
// erase()	删除指定键值对。
// clear()	清空容器，即删除容器中存储的所有键值对。
// swap()	交换2个unordered_map容器存储的键值对，前提是必须保证这2个容器的类型完全相等。
// bucket_count() 返回当前容器底层存储键值对时，使用桶（一个线性链表代表一个桶）的数量。
// max_bucket_count() 返回当前系统中，unordered_map容器底层最多可以使用多少桶。
// bucket_size(n) 返回第n个桶中存储键值对的数量。
// bucket(key)	返回以key为键的键值对所在桶的编号。
// load_factor() 返回unordered_map容器中当前的负载因子。负载因子，指的是的当前容器中存储键值对的数量（size()）和使用桶数（bucket_count()）的比值，
//               即load_factor()=size()/bucket_count()。
// max_load_factor() 返回或者设置当前unordered_map容器的负载因子。
// rehash(n) 将当前容器底层使用桶的数量设置为n。
// reserve() 将存储桶的数量（也就是bucket_count()方法的返回值）设置为至少容纳count个元（不超过最大负载因子）所需的数量，并重新整理容器。
// hash_function() 返回当前容器使用的哈希函数对象。

// C++无序容器的底层实现机制
// C++ STL标准库中，不仅是unordered_map容器，所有无序容器的底层实现都采用的是哈希表存储结构。更准确地说，是用“链地址法”（又称“开链法”）解决数据存储位置发生冲突的哈希表
// 当使用无序容器存储键值对时，会先申请一整块连续的存储空间，但此空间并不用来直接存储键值对，而是存储各个链表的头指针，各键值对真正的存储位置是各个链表的节点。
// STL标准库通常选用vector容器存储各个链表的头指针。
// 在C++ STL标准库中，将各个链表称为桶（bucket），每个桶都有自己的编号（从0开始）。当有新键值对存储到无序容器中时，整个存储过程分为如下几步：
//  将该键值对中键的值带入设计好的哈希函数，会得到一个哈希值（一个整数，用H表示）；
//  将H和无序容器拥有桶的数量n做整除运算（即H%n），该结果即表示应将此键值对存储到的桶的编号；
//  建立一个新节点存储此键值对，同时将该节点链接到相应编号的桶上。
// 哈希表存储结构还有一个重要的属性，称为负载因子（load factor）。该属性同样适用于无序容器，用于衡量容器存储键值对的空/满程序，即负载因子越大，意味着容器越满，即各链表中
// 挂载着越多的键值对，这无疑会降低容器查找目标键值对的效率；反之，负载因子越小，容器肯定越空，但并不一定各个链表中挂载的键值对就越少。
// 无序容器中，负载因子的计算方法为：负载因子=容器存储的总键值对/桶数
// 默认情况下，无序容器的最大负载因子为1.0。如果操作无序容器过程中，使得最大复杂因子超过了默认值，则容器会自动增加桶数，并重新进行哈希，以此来减小负载因子的值。
// 需要注意的是，此过程会导致容器迭代器失效，但指向单个键值对的引用或者指针仍然有效。这也就解释了，为什么我们在操作无序容器过程中，键值对的存储顺序有时会“莫名”的发生变动。
// C++ STL标准库为了方便用户更好地管控无序容器底层使用的哈希表存储结构，各个无序容器的模板类中都提供了以下成员方法：
// bucket_count() 返回当前容器底层存储键值对时，使用桶的数量。
// max_bucket_count() 返回当前系统中，unordered_map容器底层最多可以使用多少个桶。
// bucket_size(n) 返回第n个桶中存储键值对的数量。
// bucket(key) 返回以key为键的键值对所在桶的编号。
// load_factor() 返回unordered_map容器中当前的负载因子。
// max_load_factor() 返回或者设置当前unordered_map容器的最大负载因子。
// rehash(n) 尝试重新调整桶的数量为等于或大于n的值。如果n大于当前容器使用的桶数，则该方法会是容器重新哈希，该容器新的桶数将等于或大于n。
//           反之，如果n的值小于当前容器使用的桶数，则调用此方法可能没有任何作用。
// reserve(n) 将容器使用的桶数（bucket_count()方法的返回值）设置为最适合存储n个元素的桶数。
// hash_function() 返回当前容器使用的哈希函数对象。

// unordered_map容器迭代器的类型为前向迭代器（又称正向迭代器）。假设p是一个前向迭代器，则其只能进行*p、p++、++p操作，且2个前向迭代器之间只能用==和!=运算符做比较。
// equal_range(key)很少用于unordered_map容器，因为该容器中存储的都是键不相等的键值对，即便调用该成员方法，得到的2个迭代器所表示的范围中，最多只包含1个键值对。
// 事实上，该成员方法更适用于unordered_multimap容器

int main() {
  auto printUm = [](std::unordered_map<std::string, std::string> &um) -> void {
    for (auto i = um.begin(); i != um.end(); ++i) {
      cout << i->first << " " << i->second << endl;
    }
  };

  // 创建C++ unordered_map容器的方法:
  // 1 通过调用unordered_map模板类的默认构造函数，可以创建空的unordered_map容器
  std::unordered_map<std::string, std::string> umap1;
  // 2 在创建unordered_map容器的同时，可以完成初始化操作
  std::unordered_map<std::string, std::string> umap2{
      {"Python教程", "python"}, {"Java教程", "java"}, {"Linux教程", "linux"}};
  printUm(umap2);
  // Linux教程 linux
  // Python教程 python
  // Java教程 java
  // 3 调用unordered_map模板中提供的复制（拷贝）构造函数，将现有unordered_map容器中存储的键值对，复制给新建unordered_map容器
  std::unordered_map<std::string, std::string> umap31(umap2);
  // C++11标准中还向unordered_map模板类增加了移动构造函数，即以右值引用的方式将临时unordered_map容器中存储的所有键值对，全部复制给新建容器
  auto retUmap = []() -> std::unordered_map<std::string, std::string> {
    std::unordered_map<std::string, std::string> tempUmap{
        {"Python教程", "python"}, {"Java教程", "java"}, {"Linux教程", "linux"}};
    return tempUmap;
  };
  std::unordered_map<std::string, std::string> umap32(retUmap());
  // 无论是调用复制构造函数还是拷贝构造函数，必须保证2个容器的类型完全相同。
  // 4 如果不想全部拷贝，可以使用unordered_map类模板提供的迭代器，在现有unordered_map容器中选择部分区域内的键值对，为新建unordered_map容器初始化
  unordered_map<string, string> umap4(++umap2.begin(), umap2.end());
  printUm(umap4);
  // Java教程 java
  // Python教程 python

  unordered_map<string, string> umap;
  umap.emplace("Python教程", "http://c.biancheng.net/python/");
  umap.emplace("Java教程", "http://c.biancheng.net/java/");
  umap.emplace("Linux教程", "http://c.biancheng.net/linux/");
  cout << "umap size = " << umap.size() << endl;
  for (auto iter = umap.begin(); iter != umap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // Linux教程 http://c.biancheng.net/linux/
  // Java教程 http://c.biancheng.net/java/
  // Python教程 http://c.biancheng.net/python/

  unordered_map<string, string> umap5;
  cout << "umap初始桶数: " << umap5.bucket_count() << endl;         // 1
  cout << "umap初始负载因子: " << umap5.load_factor() << endl;      // 0
  cout << "umap最大负载因子: " << umap5.max_load_factor() << endl;  // 1
  umap5.reserve(9);
  cout << "umap新桶数: " << umap5.bucket_count() << endl;     // 11
  cout << "umap新负载因子: " << umap5.load_factor() << endl;  // 0
  umap5["Python教程"] = "http://c.biancheng.net/python/";
  umap5["Java教程"] = "http://c.biancheng.net/java/";
  umap5["Linux教程"] = "http://c.biancheng.net/linux/";
  cout << "以\"Python教程\"为键的键值对，位于桶的编号为:"
       << umap5.bucket("Python教程") << endl;
  // 以"Python教程"为键的键值对，位于桶的编号为:6
  auto fn = umap5.hash_function();
  cout << "计算以\"Python教程\"为键的键值对，位于桶的编号为："
       << fn("Python教程") % (umap5.bucket_count()) << endl;
  // 计算以"Python教程"为键的键值对，位于桶的编号为：6

  unordered_map<string, string>::iterator iter = umap2.find("Java教程");
  cout << "umap.find(\"Java教程\") = "
       << "<" << iter->first << ", " << iter->second << ">" << endl;
  // umap.find("Java教程") = <Java教程, java>

  // 在操作unordered_map容器过程（尤其是向容器中添加新键值对）中，一旦当前容器的负载因子超过最大负载因子（默认值为1.0），该容器就会适当增加桶的数量（通常是翻一倍），
  // 并自动执行rehash()成员方法，重新调整各个键值对的存储位置（此过程又称“重哈希”），此过程很可能导致之前创建的迭代器失效。
  // 所谓迭代器失效，针对的是那些用于表示容器内某个范围的迭代器，由于重哈希会重新调整每个键值对的存储位置，所以容器重哈希之后，之前表示特定范围的迭代器很可能无法再正确表
  // 示该范围。但是，重哈希并不会影响那些指向单个键值对元素的迭代器。
  unordered_map<int, int> umap6;
  for (int i = 1; i <= 50; i++) {
    umap6.emplace(i, i);
  }
  auto pair = umap6.equal_range(1);
  for (auto iter = pair.first; iter != pair.second; ++iter) {
    cout << iter->first << " ";
  }
  cout << endl;  // 1
  umap6.max_load_factor(3.0);
  umap6.rehash(10);
  for (auto iter = pair.first; iter != pair.second; ++iter) {
    cout << iter->first << " ";
  }
  cout << endl;  // 1 18 36 2 19 37 3 20 38 4 21 39
  // 用于遍历整个容器的begin()/end()和cbegin()/cend()迭代器对，重哈希只会影响遍历容器内键值对的顺序，整个遍历的操作仍然可以顺利完成。

  // unordered_map获取元素的4种方法:
  // 1 unordered_map容器类模板中，实现了对[]运算符的重载，使得我们可以像“利用下标访问普通数组中元素”那样，通过目标键值对的键获取到该键对应的值
  // 当使用[]运算符向unordered_map容器中添加键值对时，分为2种情况：
  //  当[]运算符位于赋值号（=）右侧时，则新添加键值对的键为[]运算符内的元素，其值为键值对要求的值类型的默认值（string类型默认值为空字符串）；
  //  当[]运算符位于赋值号（=）左侧时，则新添加键值对的键为[]运算符内的元素，其值为赋值号右侧的元素。
  cout << umap2["Java教程"] << endl;  // java
  // 如果当前容器中并没有存储以[]运算符内指定的元素作为键的键值对，则此时[]运算符的功能将转变为：向当前容器中添加以目标元素为键的键值对
  string str = umap2["STL教程"];
  umap2["C教程"] = "http://c.biancheng.net/c/";
  printUm(umap2);
  // C教程 http://c.biancheng.net/c/
  // STL教程
  // Linux教程 linux
  // Python教程 python
  // Java教程 java
  // 2 unordered_map类模板中，还提供有at()成员方法，和使用[]运算符一样，at()成员方法也需要根据指定的键，才能从容器中找到该键对应的值；
  // 不同之处在于，如果在当前容器中查找失败，该方法不会向容器中添加新的键值对，而是直接抛出out_of_range异常。
  cout << umap2.at("C教程") << endl;  // http://c.biancheng.net/c/
  // cout << umap.at("GO教程");  // std::out_of_range
  // 3 []运算符和at()成员方法基本能满足大多数场景的需要。除此之外，还可以借助unordered_map模板中提供的find()成员方法。
  // 和前面方法不同的是，通过find()方法得到的是一个正向迭代器，该迭代器的指向分以下2种情况：
  //  当find()方法成功找到以指定元素作为键的键值对时，其返回的迭代器就指向该键值对；
  //  当find()方法查找失败时，其返回的迭代器和end()方法返回的迭代器一样，指向容器中最后一个键值对之后的位置。
  unordered_map<string, string>::iterator iter11 = umap2.find("Python教程");
  cout << iter11->first << " " << iter11->second << endl;
  unordered_map<string, string>::iterator iter22 = umap2.find("GO教程");
  if (iter22 == umap2.end()) {
    cout << "没有\"GO教程\"的键" << endl;  // 没有"GO教程"的键
  }
  // 4 除了find()成员方法之外，甚至可以借助begin()/end()或者 cbegin()/cend()，通过遍历整个容器中的键值对来找到目标键值对。
  for (auto iter = umap2.begin(); iter != umap2.end(); ++iter) {
    if (!iter->first.compare("Java教程")) {
      cout << iter->second << endl;  // java
      break;
    }
  }
  // 以上4种方法中，前2种方法基本能满足多数场景的需要，建议初学者首选at()成员方法！

  // unordered_map模板类中，提供了多种语法格式的insert()方法，根据功能的不同，可划分为以下几种用法
  // 1 insert()方法可以将pair类型的键值对元素添加到unordered_map容器中，其语法格式有2种：
  // pair<iterator,bool> insert (const value_type& val); 以普通方式传递参数
  // template <class P> pair<iterator,bool> insert (P&& val); 以右值引用的方式传递参数
  // 以上2种格式中，参数val表示要添加到容器中的目标键值对元素；该方法的返回值为pair类型值，内部包含一个iterator迭代器和bool变量：
  //  当insert()将val成功添加到容器中时，返回的迭代器指向新添加的键值对，bool值为True；
  //  当insert()添加键值对失败时，意味着当前容器中本就存储有和要添加键值对的键相等的键值对，返回的迭代器将指向这个导致插入操作失败的迭代器，bool值为False。
  unordered_map<char, int> um7;
  std::pair<char, int> pp('a', 2);
  std::pair<unordered_map<char, int>::iterator, bool> ret = um7.insert(pp);
  cout << ret.first->first << " " << ret.first->second << " " << ret.second
       << endl;  // a 2 1
  ret = um7.insert(make_pair('b', 3));
  cout << ret.first->first << " " << ret.first->second << " " << ret.second
       << endl;  // b 3 1
  // 2 insert()方法还可以指定新键值对要添加到容器中的位置，其语法格式如下：
  // iterator insert (const_iterator hint, const value_type& val);  // 以普通方式传递val参数
  // template <class P> iterator insert (const_iterator hint, P&& val);  // 以右值引用方法传递val参数
  // 以上2种语法格式中，hint参数为迭代器，用于指定新键值对要添加到容器中的位置；val参数指的是要添加容器中的键值对；方法的返回值为迭代器：
  //  如果insert()方法成功添加键值对，该迭代器指向新添加的键值对；
  //  如果insert()方法添加键值对失败，则表示容器中本就包含有相同键的键值对，该方法返回的迭代器就指向容器中键相同的键值对；
  // 注意，以上2种语法格式中，虽然通过hint参数指定了新键值对添加到容器中的位置，但该键值对真正存储的位置，并不是hint参数说了算，最终的存储位置仍取决于该键值对的键的值。
  std::pair<char, int> mypair8('c', 4);
  unordered_map<char, int>::iterator iter8;
  iter8 = um7.insert(um7.begin(), mypair8);
  cout << iter8->first << " " << iter8->second << endl;  // c 4
  iter8 = um7.insert(um7.begin(), std::make_pair('d', 5));
  cout << iter8->first << " " << iter8->second << endl;  // d 5
  // 3 insert()方法还支持将某一个unordered_map容器中指定区域内的所有键值对，复制到另一个unordered_map容器中
  // template <class InputIterator> void insert (InputIterator first, InputIterator last);
  // 其中first和last都为迭代器，[first, last)表示复制其它unordered_map容器中键值对的区域。
  unordered_map<char, int> um8;
  um8.insert(++um7.begin(), um7.end());
  for (auto i = um8.begin(); i != um8.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // a 2
  // b 3
  // c 4
  // 4 insert()方法还支持一次向unordered_map容器添加多个键值对，其语法格式如下：
  // void insert (initializer_list<value_type> il); 其中，il参数指的是可以用初始化列表的形式指定多个键值对元素。
  um7.insert({{'e', 6}, {'f', 7}, {'g', 8}});
  for (auto i = um7.begin(); i != um7.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // g 8
  // f 7
  // e 6
  // a 2
  // b 3
  // c 4
  // d 5

  // C++11标准为unordered_map容器新增了emplace()和emplace_hint()成员方法
  // 向已有unordered_map容器中添加新键值对，可以通过调用insert()方法，但其实还有更好的方法，即使用emplace()或者emplace_hint()方法，
  // 它们完成“向容器中添加新键值对”的效率，要比insert()方法高。
  // emplace()方法的用法很简单，其语法格式如下：template <class... Args> pair<iterator, bool> emplace (Args&&... args);
  // 其中，参数args表示可直接向该方法传递创建新键值对所需要的2个元素的值，其中第一个元素将作为键值对的键，另一个作为键值对的值。
  // 也就是说，该方法无需我们手动创建键值对，其内部会自行完成此工作。
  // 另外需要注意的是，该方法的返回值为pair类型值，其包含一个迭代器和一个bool类型值：
  //  当emplace()成功添加新键值对时，返回的迭代器指向新添加的键值对，bool值为True；
  //  当emplace()添加新键值对失败时，说明容器中本就包含一个键相等的键值对，此时返回的迭代器指向的就是容器中键相同的这个键值对，bool值为False。
  std::pair<unordered_map<char, int>::iterator, bool> rr = um7.emplace('z', 9);
  cout << rr.first->first << " " << rr.first->second << " " << rr.second
       << endl;  // z 9 1
  rr = um7.emplace('f', 9);
  cout << rr.first->first << " " << rr.first->second << " " << rr.second
       << endl;  // f 7 0
  // emplace_hint()方法的语法格式如下：template <class... Args> iterator emplace_hint (const_iterator position, Args&&... args);
  // 和emplace()方法相同，emplace_hint()方法内部会自行构造新键值对，因此我们只需向其传递构建该键值对所需的2个元素（第一个作为键，另一个作为值）即可。
  // 不同之处在于：
  //  emplace_hint()方法的返回值仅是一个迭代器，而不再是pair类型变量。当该方法将新键值对成功添加到容器中时，返回的迭代器指向新添加的键值对；
  //                反之，如果添加失败，该迭代器指向的是容器中和要添加键值对键相同的那个键值对。
  //  emplace_hint()方法还需要传递一个迭代器作为第一个参数，该迭代器表明将新键值对添加到容器中的位置。需要注意的是，新键值对添加到容器中的位置，
  //                并不是此迭代器说了算，最终仍取决于该键值对的键的值。
  // 可以这样理解，emplace_hint()方法中传入的迭代器，仅是给unordered_map容器提供一个建议，并不一定会被容器采纳。
  unordered_map<char, int>::iterator xx =
      um7.emplace_hint(um7.begin(), 'k', 15);
  cout << xx->first << " " << xx->second << endl;  // k 15

  for (auto i = um7.begin(); i != um7.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // k 15
  // g 8
  // f 7
  // e 6
  // a 2
  // b 3
  // c 4
  // z 9
  // d 5

  // unordered_map删除元素：erase()和clear()
  //  erase()：删除unordered_map容器中指定的键值对；
  //  clear()：删除unordered_map容器中所有的键值对，即清空容器。
  // unordered_map 3种语法格式的erase()方法:
  // 1 erase()方法可以接受一个正向迭代器，并删除该迭代器指向的键值对。该方法的语法格式如下：
  // iterator erase(const_iterator position); // 其中position为指向容器中某个键值对的迭代器，该方法会返回一个指向被删除键值对之后位置的迭代器。
  xx = um7.erase(um7.begin());
  cout << xx->first << " " << xx->second << endl;  // g 8
  for (auto i = um7.begin(); i != um7.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // g 8
  // f 7
  // e 6
  // a 2
  // b 3
  // c 4
  // z 9
  // d 5
  // 通过给erase()方法传入指向容器中第一个键值对的迭代器，该方法可以将容器中第一个键值对删除，同时返回一个指向被删除键值对之后位置的迭代器。
  // 如果erase()方法删除的是容器存储的最后一个键值对，则该方法返回的迭代器，将指向容器中最后一个键值对之后的位置（等同于end()方法返回的迭代器）。
  // 2 还可以直接将要删除键值对的键作为参数直接传给erase()方法，该方法会自行去unordered_map容器中找和给定键相同的键值对，将其删除。
  // erase()方法的语法格式如下：size_type erase ( const key_type& k );其中，k表示目标键值对的键的值；该方法会返回一个整数，其表示成功删除的键值对的数量。
  cout << "befor size = " << um7.size() << endl;  // befor size = 8
  int count = um7.erase('g');
  cout << count << ", size = " << um7.size() << endl;  // 1, size = 7
  // 3 除了支持删除unordered_map容器中指定的某个键值对，erase()方法还支持一次删除指定范围内的所有键值对
  // 其语法格式如下：iterator erase (const_iterator first, const_iterator last);
  // 其中first和last都是正向迭代器，[first, last)范围内的所有键值对都会被erase()方法删除；同时，该方法会返回一个指向被删除的最后一个键值对之后一个位置的迭代器。
  xx = um7.erase(++um7.begin(), ++(++(++um7.begin())));
  cout << xx->first << " " << xx->second << endl;  // b 3
  for (auto i = um7.begin(); i != um7.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // b 3
  // f 7
  // b 3
  // c 4
  // z 9
  // d 5
  // 需要一次性删除unordered_map容器中存储的所有键值对，可以使用clear()方法，其语法格式如下：void clear()
  um7.clear();
  cout << "size: " << um7.size() << endl;  // size: 0
  // 虽然使用erase()方法的第3种语法格式，可能实现删除unordered_map容器内所有的键值对，但更推荐使用clear()方法。

  return 0;
}