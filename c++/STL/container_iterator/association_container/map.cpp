#include <iostream>
#include <map>
#include <utility>

using namespace std;

// map容器存储的都是pair对象，也就是用pair类模板创建的键值对。其中，各个键值对的键和值可以是任意数据类型，包括C++基本数据类型（int、double等）、使用结构体或类自定义的类型。
// 通常情况下，map容器中存储的各个键值对都选用string字符串作为键的类型。
// 在使用map容器存储多个键值对时，该容器会自动根据各键值对的键的大小，按照既定的规则进行排序。默认情况下，map容器选用std::less<T>排序规则（其中T表示键的数据类型），其会根据
// 键的大小对所有键值对做升序排序。当然，根据实际情况的需要，可以手动指定map容器的排序规则，既可以选用STL中提供的其它排序规则（比如std::greater<T>），也可以自定义排序规则。
// 另外需要注意的是，使用map容器存储的各个键值对，键的值既不能重复也不能被修改。换句话说，map容器中存储的各个键值对不仅键的值独一无二，键的类型也会用const修饰，这意味着只要键
// 值对被存储到map容器中，其键的值将不能再做任何修改。
// map容器存储的都是pair类型的键值对元素，更确切的说，该容器存储的都是pair<const K, T>类型（其中K和T分别表示键和值的数据类型）的键值对元素。
// map容器定义在<map>头文件中，并位于std命名空间中

// template <class Key,                  // 指定键（key）的类型
//           class T,                    // 指定值（value）的类型
//           class Compare = less<Key>,  // 指定排序规则
//           class Alloc = allocator<pair<const Key, T>>  // 指定分配器对象的类型
//           > class map;
// map容器模板有4个参数，其中后2个参数都设有默认值。大多数场景中，我们只需要设定前2个参数的值，有些场景可能会用到第3个参数，但最后一个参数几乎不会用到。

// map容器包含的成员方法:
// begin() 	返回指向容器中第一个（是已排好序的第一个）键值对的双向迭代器。如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// end() 	  返回指向容器最后一个元素（排好序的最后一个）所在位置后一个位置的双向迭代器，通常和begin()结合使用。如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// rbegin() 返回指向最后一个（是已排好序的最后一个）元素的反向双向迭代器。如果map容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// rend() 	返回指向第一个（是已排好序的第一个）元素所在位置前一个位置的反向双向迭代器。如果map容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// cbegin() 和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// cend() 	和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crbegin() 和rbegin()功能相同，只不过在其基础上，增加了 const 属性，不能用于修改容器内存储的键值对。
// crend() 	和end()功能相同，只不过在其基础上，增加了 const 属性，不能用于修改容器内存储的键值对。
// find(key) 在map容器中查找键为key的键值对，如果成功找到，则返回指向该键值对的双向迭代器；反之，则返回和end()方法一样的迭代器。
//           另外，如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// lower_bound(key) 返回一个指向当前map容器中第一个大于或等于key的键值对的双向迭代器。如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(key) 返回一个指向当前map容器中第一个大于key的键值对的迭代器。如果map容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(key) 该方法返回一个pair对象（包含2个双向迭代器），其中pair.first和lower_bound()方法的返回值等价，pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含的键为key的键值对（map容器键值对唯一，因此该范围最多包含一个键值对）。
// empty()  若容器为空，则返回true；否则false。
// size() 	返回当前map容器中存有键值对的个数。
// max_size() 返回map容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// operator[] map容器重载了[]运算符，只要知道map容器中某个键值对的键的值，就可以向获取数组中元素那样，通过键直接获取对应的值。
// at(key) 	找到map容器中key键对应的值，如果找不到，该函数会引发out_of_range异常。
// insert() 向map容器中插入键值对。
// erase() 	删除map容器指定位置、指定键（key）值或者指定区域内的键值对。
// swap() 	交换2个map容器中存储的键值对，这意味着，操作的2个键值对的类型必须相同。
// clear() 	清空map容器中所有的键值对，即使map容器的size()为0。
// emplace() 	在当前map容器中的指定位置处构造新键值对。其效果和插入键值对一样，但效率更高。
// emplace_hint() 在本质上和emplace()在map容器中构造新键值对的方式是一样的，不同之处在于，使用者必须为该方法提供一个指示键值对生成位置的迭代器，并作为该方法的第一个参数。
// count(key) 在当前map容器中，查找键为key的键值对的个数并返回。注意，由于map容器中各键值对的键的值是唯一的，因此该函数的返回值最大为1。

int main() {
  auto printMap = [](map<std::string, int> &m) -> void {
    for (auto i = m.begin(); i != m.end(); ++i) {
      cout << i->first << " " << i->second << endl;
    }
  };

  // map容器的模板类中包含多种构造函数，因此创建map容器的方式也有多种:
  // 1) 通过调用map容器类的默认构造函数，可以创建出一个空的map容器
  std::map<std::string, int> myMap1;
  // 通过此方式创建出的myMap容器，初始状态下是空的，即没有存储任何键值对。鉴于空map容器可以根据需要随时添加新的键值对，因此创建空map容器是比较常用的。
  // 2) 当然在创建map容器的同时，也可以进行初始化
  std::map<std::string, int> myMap21{{"C语言教程", 10}, {"STL教程", 20}};
  // myMap容器在初始状态下，就包含有2个键值对。
  // 再次强调，map容器中存储的键值对，其本质都是pair类模板创建的pair对象。因此，下面程序也可以创建出一模一样的myMap容器：
  std::map<std::string, int> myMap22{std::make_pair("C语言教程", 10),
                                     std::make_pair("STL教程", 20)};
  // 3) 除此之外，在某些场景中，可以利用先前已创建好的map容器，再创建一个新的map容器。例如：
  std::map<std::string, int> newMap1(myMap1);
  // 通过调用map容器的拷贝（复制）构造函数，即可成功创建一个和myMap完全一样的newMap容器。
  // C++11标准中，还为map容器增添了移动构造函数。当有临时的map对象作为参数，传递给要初始化的map容器时，此时就会调用移动构造函数。举个例子：
  // 创建一个会返回临时map对象的函数
  auto disMap = []() -> std::map<std::string, int> {
    std::map<std::string, int> tempMap{{"C语言教程", 10}, {"STL教程", 20}};
    return tempMap;
  };
  // 调用map类模板的移动构造函数创建newMap容器
  std::map<std::string, int> newMap(disMap());
  // 注意，无论是调用复制构造函数还是调用拷贝构造函数，都必须保证这2个容器的类型完全一致。
  // 4) map类模板还支持取已建map容器中指定区域内的键值对，创建并初始化新的map容器。
  std::map<std::string, int> myMap3{{"C语言教程", 10}, {"STL教程", 20}};
  std::map<std::string, int> newMap2(++myMap21.begin(), myMap21.end());
  printMap(newMap2);  // STL教程 20
  // 通过调用map容器的双向迭代器，实现了在创建newMap容器的同时，将其初始化为包含一个{"STL教程",20}键值对的容器。
  // 5) 在以上几种创建map容器的基础上，我们都可以手动修改map容器的排序规则。默认情况下，map容器调用 std::less<T>规则，根据容器内各键值对的键的大小，对所有键值对做升序排序。
  // 如下2行创建map容器的方式，其实是等价的：
  std::map<std::string, int> myMap4{{"C语言教程", 10}, {"STL教程", 20}};
  std::map<std::string, int, std::less<std::string>> myMap5{{"C语言教程", 10},
                                                            {"STL教程", 20}};
  printMap(myMap4);
  printMap(myMap5);
  // C语言教程 10
  // STL教程 20
  // 下面程序手动修改了myMap容器的排序规则，令其作降序排序：
  std::map<std::string, int, std::greater<std::string>> myMap6{
      {"C语言教程", 10}, {"STL教程", 20}};
  for (auto i = myMap6.begin(); i != myMap6.end(); ++i) {
    cout << i->first << " " << i->second << endl;
  }
  // STL教程 20
  // C语言教程 10

  std::map<std::string, std::string, std::greater<std::string>> myMap;
  myMap.emplace("C语言教程", "http://c.biancheng.net/c/");
  myMap.emplace("Python教程", "http://c.biancheng.net/python/");
  myMap.emplace("STL教程", "http://c.biancheng.net/stl/");
  cout << "myMap size == " << myMap.size() << endl;
  if (!myMap.empty()) {
    for (auto i = myMap.begin(); i != myMap.end(); ++i) {
      cout << i->first << " " << i->second << endl;
    }
  }
  // myMap size == 3
  // STL教程 http://c.biancheng.net/stl/
  // Python教程 http://c.biancheng.net/python/
  // C语言教程 http://c.biancheng.net/c/

  auto iter = myMap.find("Python教程");
  for (; iter != myMap.end(); ++iter) {
    cout << iter->first << " " << iter->second << endl;
  }
  // Python教程 http://c.biancheng.net/python/
  // C语言教程 http://c.biancheng.net/c/

  // map 类模板中还提供有lower_bound(key)和upper_bound(key)成员方法，它们的功能是类似的，唯一的区别在于：
  // lower_bound(key)返回的是指向第一个键不小于key的键值对的迭代器；
  // upper_bound(key)返回的是指向第一个键大于key的键值对的迭代器；
  map<int, string> m{{1, "一"}, {2, "一"}, {3, "一"},
                     {4, "一"}, {5, "一"}, {6, "一"}};
  auto iter1 = m.lower_bound(2);
  cout << iter1->first << " " << iter1->second << endl;  // 2 一
  iter1 = m.upper_bound(2);
  cout << iter1->first << " " << iter1->second << endl;  // 3 一
  // lower_bound(key)和upper_bound(key)更多用于multimap容器，在map容器中很少用到。

  // equal_range(key)成员方法可以看做是lower_bound(key)和upper_bound(key)的结合体，该方法会返回一个pair对象，其中的2个元素都是迭代器类型，其中pair.first实际上就
  // 是lower_bound(key)的返回值，而pair.second则等同于upper_bound(key)的返回值。
  pair<map<int, string>::iterator, map<int, string>::iterator> myPair =
      m.equal_range(4);
  for (auto iter = myPair.first; iter != myPair.second; ++iter) {
    cout << iter->first << " " << iter->second << endl;  // 4 一
  }
  // 和lower_bound(key)、upper_bound(key)一样，该方法也更常用于multimap容器，因为map容器中各键值对的键的值都是唯一的，
  // 因此通过map容器调用此方法，其返回的范围内最多也只有1个键值对。

  return 0;
}