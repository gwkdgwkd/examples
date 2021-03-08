#include <iostream>
#include <map>

using namespace std;

// multimap容器也用于存储pair<const K, T>类型的键值对（其中K表示键的类型，T表示值的类型），其中各个键值对的键的值不能做修改；
// 并且，该容器也会自行根据键的大小对存储的所有键值对做排序操作。和map容器的区别在于，multimap容器中可以同时存储多（≥2）个键相同的键值对。
// 和map容器一样，实现 multimap容器的类模板也定义在<map>头文件，并位于std命名空间中。
// multimap 容器类模板的定义如下：
// template < class Key,                                   // 指定键（key）的类型
//            class T,                                     // 指定值（value）的类型
//            class Compare = less<Key>,                   // 指定排序规则
//            class Alloc = allocator<pair<const Key,T> >  // 指定分配器对象的类型
//            > class multimap;
// multimap容器模板有4个参数，其中后2个参数都设有默认值。大多数场景中，只需要设定前2个参数的值，有些场景可能会用到第3个参数，但最后一个参数几乎不会用到。

// multimap容器包含的成员方法:
// begin() 	返回指向容器中第一个（是已排好序的第一个）键值对的双向迭代器。如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// end() 	返回指向容器最后一个元素（是已排好序的最后一个）所在位置后一个位置的双向迭代器，通常和begin()结合使用。如果multimap容器用const限定，则返回const类型的双向迭代器。
// rbegin() 返回指向最后一个（是已排好序的最后一个）元素的反向双向迭代器。如果multimap容器用const 限定，则该方法返回的是const类型的反向双向迭代器。
// rend() 	返回指向第一个（是已排好序的第一个）元素所在位置前一个位置的反向双向迭代器。如果multimap容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// cbegin() 和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// cend() 	和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crbegin() 和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crend() 	和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// find(key) 在multimap容器中查找首个键为key的键值对，如果成功找到，则返回指向该键值对的双向迭代器；反之，则返回和end()方法一样的迭代器。
//           另外，如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// lower_bound(key) 返回一个指向当前multimap容器中第一个大于或等于key的键值对的双向迭代器。如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(key) 返回一个指向当前multimap容器中第一个大于key的键值对的迭代器。如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(key) 该方法返回一个pair对象（包含2个双向迭代器），其中pair.first和lower_bound()方法的返回值等价，pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含的键为key的键值对。
// empty()  若容器为空，则返回true；否则false。
// size() 	返回当前multimap容器中存有键值对的个数。
// max_size() 返回multimap容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// insert() 向multimap容器中插入键值对。
// erase() 	删除multimap容器指定位置、指定键（key）值或者指定区域内的键值对。
// swap() 	交换2个multimap容器中存储的键值对，这意味着，操作的2个键值对的类型必须相同。
// clear() 	清空multimap容器中所有的键值对，使multimap容器的size()为0。
// emplace() 在当前multimap容器中的指定位置处构造新键值对。其效果和插入键值对一样，但效率更高。
// emplace_hint() 在本质上和emplace()在multimap容器中构造新键值对的方式是一样的，不同之处在于，使用者必须为该方法提供一个指示键值对生成位置的迭代器，并作为该方法的第一个参数。
// count(key) 在当前multimap容器中，查找键为ke 的键值对的个数并返回。

// 创建一个会返回临时multimap对象的函数
multimap<string, string> dismultimap() {
  multimap<string, string> tempmultimap{
      {"C语言教程", "http://c.biancheng.net/c/"},
      {"Python教程", "http://c.biancheng.net/python/"}};
  return tempmultimap;
}

int main() {
  // multimap类模板内部提供有多个构造函数，总的来说，创建multimap容器的方式可归为以下5种:
  // 1 通过调用multimap类模板的默认构造函数，可以创建一个空的multimap容器：
  std::multimap<std::string, std::string> mm1;
  // 2 在创建multimap容器的同时，还可以进行初始化操作:
  multimap<string, string> mm2{
      {"C语言教程", "c"}, {"Python教程", "python"}, {"STL教程", "stl"}};
  // 使用此方式初始化multimap容器时，其底层会先将每一个{key, value}创建成pair类型的键值对，然后再用已建好的各个键值对初始化multimap容器。
  // 完全可以先手动创建好键值对，然后再用其初始化multimap容器。下面2种方式创建pair类型键值对，再用其初始化multimap容器，它们是完全等价的：
  // 借助pair类模板的构造函数来生成各个pair类型的键值对
  multimap<string, string> mm21{pair<string, string>{"C语言教程", "c"},
                                pair<string, string>{"Python教程", "python"},
                                pair<string, string>{"STL教程", "stl"}};
  // 调用make_pair()函数，生成键值对元素创建并初始化multimap容器
  multimap<string, string> mm22{make_pair("C语言教程", "c"),
                                make_pair("Python教程", "python"),
                                make_pair("STL教程", "stl")};
  // 3 通过调用multimap类模板的拷贝（复制）构造函数，也可以初始化新的multimap容器
  multimap<string, string> mm3(mm2);
  // 在C++11标准中，还为multimap类增添了移动构造函数。即当有临时的multimap容器作为参数初始化新multimap容器时，其底层就会调用移动构造函数来实现初始化操作。
  // 调用multimap类模板的移动构造函数创建newMultimap容器
  multimap<string, string> mm31(dismultimap());
  // 由于dismultimap()函数返回的tempmultimap容器是一个临时对象，因此在实现初始化newmultimap容器时，底层调用的是multimap容器的移动构造函数，而不再是拷贝构造函数。
  // 注意，无论是调用复制构造函数还是调用拷贝构造函数，都必须保证这2个容器的类型完全一致。
  // 4 multimap类模板还支持从已有multimap容器中，选定某块区域内的所有键值对，用作初始化新multimap容器时使用
  multimap<string, string> mm4(++mm2.begin(), mm2.end());
  // multimap容器迭代器，和map容器迭代器的用法完全相同
  for (auto i = mm4.begin(); i != mm4.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // Python教程 python
  // STL教程 stl
  // 5 multimap类模板共可以接收4个参数，其中第3个参数可用来修改multimap容器内部的排序规则。默认情况下，此参数的值为std::less<T>，以下2种创建multimap容器的方式是等价的：
  multimap<char, int> mm51{{'a', 1}, {'b', 2}};
  for (auto i = mm51.begin(); i != mm51.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // a 1
  // b 2
  multimap<char, int, std::less<char>> mm52{{'a', 1}, {'b', 2}};
  for (auto i = mm52.begin(); i != mm52.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // a 1
  // b 2
  // 利用了STL模板库提供的std::greater<T>排序函数，实现令multimap容器对存储的键值对做降序排序：
  multimap<char, int, std::greater<char>> mm53{{'a', 1}, {'b', 2}};
  for (auto i = mm53.begin(); i != mm53.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // b 2
  // a 1
  // 在某些特定场景中，我们还可以为multimap容器自定义排序规则

  // 和map容器相比，multimap未提供at()成员方法，也没有重载[]运算符。这意味着，map容器中通过指定键获取指定指定键值对的方式，将不再适用于multimap容器。
  // 其实这很好理解，因为multimap容器中指定的键可能对应多个键值对，而不再是1个。
  // 只要是multimap容器提供的成员方法，map容器都提供，并且它们的用法是相同的。
  // 由于multimap容器可存储多个具有相同键的键值对，因此lower_bound()、upper_bound()、equal_range()以及count()成员方法会经常用到。
  multimap<char, int> mymultimap{
      {'a', 10}, {'b', 20}, {'b', 15}, {'b', 10}, {'c', 30}};
  cout << mymultimap.size() << endl;      // 5
  cout << mymultimap.count('b') << endl;  // 3
  for (auto iter = mymultimap.begin(); iter != mymultimap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // a 10
  // b 20
  // b 15
  // b 10
  // c 30
  pair<map<char, int>::iterator, map<char, int>::iterator> myPair1 =
      mymultimap.equal_range('b');
  for (auto iter = myPair1.first; iter != myPair1.second; ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // b 20
  // b 15
  // b 10
  map<char, int>::iterator ii1 = mymultimap.lower_bound('b');
  for (auto iter = mymultimap.begin(); iter != ii1; ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // a 10
  map<char, int>::iterator ii3 = mymultimap.lower_bound('b');
  for (auto iter = ii3; iter != mymultimap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // b 20
  // b 15
  // b 10
  // c 30

  return 0;
}