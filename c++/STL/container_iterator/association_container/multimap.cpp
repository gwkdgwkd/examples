#include <iostream>
#include <map>

// multimap容器也用于存储pair<const K, T>类型的键值对，其中各个键值对的键的值不能做修改；
// 并且，该容器也会自行根据键的大小对存储的所有键值对做排序操作。
// 和map容器的区别在于，multimap容器中可以同时存储多（≥2）个键相同的键值对。
// 和map容器一样，实现multimap容器的类模板也定义在<map>头文件，并位于std命名空间中。
// multimap 容器类模板的定义如下：
// template < class Key,                                   // 指定键（key）的类型
//            class T,                                     // 指定值（value）的类型
//            class Compare = less<Key>,                   // 指定排序规则
//            class Alloc = allocator<pair<const Key,T> >  // 指定分配器对象的类型
//            > class multimap;
// multimap容器模板有4个参数，其中后2个参数都设有默认值。
// 大多数场景中，只需要设定前2个参数的值，有些场景可能会用到第3个参数，但最后一个参数几乎不会用到。

// multimap容器包含的成员方法:
// begin() 	        返回指向容器中第一个（是已排好序的第一个）键值对的双向迭代器。
//                  如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// end() 	          返回指向容器最后一个元素（是已排好序的最后一个）所在位置后一个位置的双向迭代器，通常和begin()结合使用。
//                  如果multimap容器用const限定，则返回const类型的双向迭代器。
// rbegin()         返回指向最后一个（是已排好序的最后一个）元素的反向双向迭代器。
//                  如果multimap容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// rend() 	        返回指向第一个（是已排好序的第一个）元素所在位置前一个位置的反向双向迭代器。
//                  如果multimap容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// cbegin()         和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// cend() 	        和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crbegin()        和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// crend() 	        和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的键值对。
// find(key)        在multimap容器中查找首个键为key的键值对，如果成功找到，则返回指向该键值对的双向迭代器；
//                  反之，则返回和end()方法一样的迭代器。
//                  另外，如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// lower_bound(key) 返回一个指向当前multimap容器中第一个大于或等于key的键值对的双向迭代器。
//                  如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(key) 返回一个指向当前multimap容器中第一个大于key的键值对的迭代器。
//                  如果multimap容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(key) 该方法返回一个pair对象（包含2个双向迭代器），其中pair.first和lower_bound()方法的返回值等价，
//                  pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含的键为key的键值对。
// empty()          若容器为空，则返回true；否则false。
// size() 	        返回当前multimap容器中存有键值对的个数。
// max_size()       返回multimap容器所能容纳键值对的最大个数，不同的操作系统，其返回值亦不相同。
// insert()         向multimap容器中插入键值对。
// erase() 	        删除multimap容器指定位置、指定键（key）值或者指定区域内的键值对。
// swap() 	        交换2个multimap容器中存储的键值对，这意味着，操作的2个键值对的类型必须相同。
// clear() 	        清空multimap容器中所有的键值对，使multimap容器的size()为0。
// emplace()        在当前multimap容器中的指定位置处构造新键值对。其效果和插入键值对一样，但效率更高。
// emplace_hint()   在本质上和emplace()在multimap容器中构造新键值对的方式是一样的，不同之处在于，
//                  使用者必须为该方法提供一个指示键值对生成位置的迭代器，并作为该方法的第一个参数。
// count(key)       在当前multimap容器中，查找键为ke 的键值对的个数并返回。

void func1() {
  // 和map容器相比，multimap未提供at()成员方法，也没有重载[]运算符。
  // 这意味着，map容器中通过指定键获取指定指定键值对的方式，将不再适用于multimap容器。
  // 其实这很好理解，因为multimap容器中指定的键可能对应多个键值对，而不再是1个。
  // 只要是multimap容器提供的成员方法，map容器都提供，并且它们的用法是相同的。
  // 由于multimap容器可存储多个具有相同键的键值对，
  // 因此lower_bound()、upper_bound()、equal_range()以及count()成员方法会经常用到。
  std::multimap<char, int> mymultimap{
      {'a', 10}, {'b', 20}, {'b', 15}, {'b', 10}, {'c', 30}};
  std::cout << mymultimap.size() << std::endl;      // 5
  std::cout << mymultimap.count('b') << std::endl;  // 3

  for (auto iter = mymultimap.begin(); iter != mymultimap.end(); ++iter) {
    std::cout << iter->first << " " << iter->second << std::endl;
  }
  // a 10
  // b 20
  // b 15
  // b 10
  // c 30

  std::pair<std::map<char, int>::iterator, std::map<char, int>::iterator>
      myPair1 = mymultimap.equal_range('b');
  for (auto iter = myPair1.first; iter != myPair1.second; ++iter) {
    std::cout << iter->first << " " << iter->second << std::endl;
  }
  // b 20
  // b 15
  // b 10

  std::map<char, int>::iterator it = mymultimap.lower_bound('b');
  for (auto iter = mymultimap.begin(); iter != it; ++iter) {
    std::cout << iter->first << " " << iter->second << std::endl;
  }
  // a 10

  it = mymultimap.lower_bound('b');
  for (auto iter = it; iter != mymultimap.end(); ++iter) {
    std::cout << iter->first << " " << iter->second << std::endl;
  }
  // b 20
  // b 15
  // b 10
  // c 30
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}