#include <iostream>
#include <set>

// set容器具有以下几个特性：
// 1.不再以键值对的方式存储数据，因为set容器专门用于存储键和值相等的键值对，
//   因此该容器中真正存储的是各个键值对的值（value）；
// 2.set容器在存储数据时，会根据各元素值的大小对存储的元素进行排序（默认做升序排序）；
// 3.存储到set容器中的元素，虽然其类型没有明确用const修饰，但正常情况下它们的值是无法被修改的；
// 4.set容器存储的元素必须互不相等。

// C++ STL标准库中还提供有一个和set容器相似的关联式容器，即multiset容器。
// 所谓“相似”，是指multiset容器遵循set容器的前3个特性，仅在第4条特性上有差异。
// 和set容器不同的是，multiset容器可以存储多个值相同的元素。
// 也就是说，multiset容器和set容器唯一的差别在于，
// multiset容器允许存储多个值相同的元素，而set容器中只能存储互不相同的元素。

// 和set类模板一样，multiset类模板也定义在<set>头文件，并位于std命名空间中。
// multiset 容器类模板的定义如下所示：
//     template < class T,                        // 存储元素的类型
//                class Compare = less<T>,        // 指定容器内部的排序规则
//                class Alloc = allocator<T> >    // 指定分配器对象的类型
//                > class multiset;
// multiset类模板有3个参数，其中后2个参数自带有默认值。
// 值得一提的是，在实际使用中，我们最多只需要使用前2个参数即可，第3个参数不会用到。

// multiset容器提供的成员方法:
// begin()	        返回指向容器中第一个（是已排好序的第一个）元素的双向迭代器。
//                  如果multiset容器用const限定，则该方法返回的是const类型的双向迭代器。
// end()	          返回指向容器最后一个元素（是已排好序的最后一个）所在位置后一个位置的双向迭代器，通常和begin()结合使用。
//                  如果multiset容器用const限定，则返回const类型的双向迭代器。
// rbegin()         返回指向最后一个（是已排好序的最后一个）元素的反向双向迭代器。
//                  如果multiset容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// rend()	          返回指向第一个（是已排好序的第一个）元素所在位置前一个位置的反向双向迭代器。
//                  如果multiset容器用const限定，则该方法返回的是const类型的反向双向迭代器。
// cbegin()         和begin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的元素值。
// cend()	          和end()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的元素值。
// crbegin()        和rbegin()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的元素值。
// crend()	        和rend()功能相同，只不过在其基础上，增加了const属性，不能用于修改容器内存储的元素值。
// find(val)        在multiset容器中查找值为val的元素，如果成功找到，则返回指向该元素的双向迭代器；
//                  反之，则返回和end()方法一样的迭代器。
//                  另外，如果multiset容器用const限定，则该方法返回的是const类型的双向迭代器。
// lower_bound(val)	返回一个指向当前multiset容器中第一个大于或等于val的元素的双向迭代器。
//                  如果multiset容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(val)	返回一个指向当前multiset容器中第一个大于val的元素的迭代器。
//                  如果multiset容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(val)	该方法返回一个pair对象（包含2个双向迭代器），其中pair.first和lower_bound()方法的返回值等价，
//                  pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含所有值为val的元素。
// empty()          若容器为空，则返回true；否则false。
// size()           返回当前multiset容器中存有元素的个数。
// max_size()       返回multiset容器所能容纳元素的最大个数，不同的操作系统，其返回值亦不相同。
// insert()	        向multiset容器中插入元素。
// erase()	        删除multiset容器中存储的指定元素。
// swap()	          交换2个multiset容器中存储的所有元素。这意味着，操作的2个multiset容器的类型必须相同。
// clear()	        清空multiset容器中所有的元素，即令multiset容器的size()为0。
// emplace()        在当前multiset容器中的指定位置直接构造新元素。其效果和insert()一样，但效率更高。
// emplace_hint()   本质上和emplace()在multiset容器中构造新元素的方式是一样的，不同之处在于，
//                  使用者必须为该方法提供一个指示新元素生成位置的迭代器，并作为该方法的第一个参数。
// count(val)       在当前multiset容器中，查找值为val的元素的个数，并返回。

// multiset容器提供的成员方法，和set容器提供的完全一样
// 虽然multiset容器和set容器拥有的成员方法完全相同，但由于multiset容器允许存储多个值相同的元素，
// 因此诸如count()、find()、lower_bound()、upper_bound()、equal_range()等方法，更常用于multiset容器。

template <typename T>
void print(std::multiset<T>& s) {
  for (auto it = s.begin(); it != s.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "[" << s.size() << "]" << std::endl;
}

int main() {
  std::multiset<int> ms{1, 2, 2, 2, 3, 4, 5};
  std::cout << "count(2) =" << ms.count(2) << std::endl;  // 3
  ms.insert(8);
  int num = ms.erase(2);
  std::cout << "remove " << num << " ge 2" << std::endl;  // remove 3 ge 2
  print(ms);                                              // 1 3 4 5 8 [5]

  ms.insert(4);
  ms.emplace(4);
  ms.emplace_hint(ms.begin(), 4);
  print(ms);  // 1 3 4 4 4 4 5 8 [8]

  std::multiset<int>::iterator it = ms.lower_bound(4);
  for (auto i = ms.begin(); i != it; ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 1 3
  for (; it != ms.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 4 4 4 4 5 8

  it = ms.upper_bound(4);
  for (auto i = ms.begin(); i != it; ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 1 3 4 4 4 4
  for (; it != ms.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 5 8

  std::pair<std::multiset<int>::iterator, std::multiset<int>::iterator> p =
      ms.equal_range(4);
  for (auto i = p.first; i != p.second; ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 4 4 4 4

  return 0;
}