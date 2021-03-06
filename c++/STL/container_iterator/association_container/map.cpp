#include <iostream>
#include <map>

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

int main() {
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
  // 创建一个会返回临时 map 对象的函数
  std::map<std::string, int> disMap() {
    std::map<std::string, int> tempMap{{"C语言教程", 10}, {"STL教程", 20}};
    return tempMap;
  }
  // 调用map类模板的移动构造函数创建newMap容器
  std::map<std::string, int> newMap(disMap());
  // 注意，无论是调用复制构造函数还是调用拷贝构造函数，都必须保证这2个容器的类型完全一致。
  // 4) map类模板还支持取已建map容器中指定区域内的键值对，创建并初始化新的map容器。
  std::map<std::string, int> myMap3{{"C语言教程", 10}, {"STL教程", 20}};
  std::map<std::string, int> newMap2(++myMap21.begin(), myMap21.end());
  // 通过调用map容器的双向迭代器，实现了在创建newMap容器的同时，将其初始化为包含一个{"STL教程",20}键值对的容器。
  // 5) 在以上几种创建map容器的基础上，我们都可以手动修改map容器的排序规则。默认情况下，map容器调用 std::less<T>规则，根据容器内各键值对的键的大小，对所有键值对做升序排序。
  // 如下2行创建map容器的方式，其实是等价的：
  std::map<std::string, int> myMap4{{"C语言教程", 10}, {"STL教程", 20}};
  std::map<std::string, int, std::less<std::string> > myMap5{{"C语言教程", 10},
                                                             {"STL教程", 20}};
  // 以上2中创建方式生成的myMap容器，其内部键值对排列的顺序为：
  // <"C语言教程", 10>
  // <"STL教程", 20>
  // 下面程序手动修改了 myMap 容器的排序规则，令其作降序排序：
  std::map<std::string, int, std::greater<std::string> > myMap{
      {"C语言教程", 10}, {"STL教程", 20}};
  // 此时，myMap 容器内部键值对排列的顺序为：
  // <"STL教程", 20>
  // <"C语言教程", 10>

  return 0;
}