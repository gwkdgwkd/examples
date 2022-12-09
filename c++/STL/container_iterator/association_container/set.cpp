#include <iostream>
#include <set>

// 和map、multimap容器不同，使用set容器存储的各个键值对，
// 要求键key和值value必须相等。
// 基于set容器的这种特性，当使用set容器存储键值对时，
// 只需要为其提供各键值对中的value值即可。
// map、multimap容器都会自行根据键的大小对存储的键值对进行排序，
// set容器也会如此，
// 只不过set容器中各键值对的键key和值value是相等的，
// 根据key排序，也就等价为根据value排序。
// 使用set容器存储的各个元素的值必须各不相同。
// 从语法上讲set容器并没有强制对存储元素的类型做const修饰，
// 即set容器中存储的元素的值是可以修改的。
// 但是，C++标准为了防止用户修改容器中元素的值，
// 对所有可能会实现此操作的行为做了限制，
// 使得在正常情况下，用户是无法做到修改set容器中元素的值的。
// 切勿尝试直接修改set容器中已存储元素的值，
// 这很有可能破坏set容器中元素的有序性，
// 最正确的修改set容器中元素值的做法是：
// 先删除该元素，然后再添加一个修改后的元素。

// set容器定义于<set>头文件，并位于std命名空间中。
// set容器的类模板定义如下：
// template < 
//   class T,                    // 键key和值value的类型
//   class Compare = less<T>,    // 指定set容器内部的排序规则
//   class Alloc = allocator<T>  // 指定分配器对象的类型
// > class set;
// 由于set容器存储的各个键值对，其键和值完全相同，
// 也就意味着它们的类型相同，因此set容器类模板的定义中，
// 仅有第1个参数用于设定存储数据的类型。
// 对于set类模板中的3个参数，后2个参数自带默认值，
// 且几乎所有场景中只需使用前2个参数，第3个参数不会用到。

// set容器包含的成员方法:
// begin()	        返回指向容器中排序后的第一个元素的双向迭代器。
//                  如果set容器用const限定，
//                  则该方法返回的是const类型的双向迭代器。
// end()	          返回指向容器排序后最后一个元素所在位置后一个位置的双向迭代器，
//                  通常和begin()结合使用。
//                  如果set容器用const限定，则返回const类型的双向迭代器。
// rbegin()         返回指向排序后最后一个元素的反向双向迭代器。
//                  如果set容器用const限定，
//                  则该方法返回的是const类型的反向双向迭代器。
// rend()	          返回指向排序后的第一个元素所在位置前一个位置的反向双向迭代器。
//                  如果set容器用const限定，
//                  则该方法返回的是const类型的反向双向迭代器。
// cbegin()         和begin()功能相同，只不过在其基础上，
//                  增加了const属性，不能用于修改容器内存储的元素值。
// cend()	          和end()功能相同，只不过在其基础上，
//                  增加了const属性，不能用于修改容器内存储的元素值。
// crbegin()        和rbegin()功能相同，只不过在其基础上，
//                  增加了const属性，不能用于修改容器内存储的元素值。
// crend()	        和rend()功能相同，只不过在其基础上，
//                  增加了const属性，不能用于修改容器内存储的元素值。
// find(val)        在set容器中查找值为val的元素，
//                  如果成功找到，则返回指向该元素的双向迭代器；
//                  反之，则返回和end()方法一样的迭代器。
//                  另外，如果set容器用const限定，
//                  则该方法返回的是const类型的双向迭代器。
// lower_bound(val) 返回一个指向当前set容器中第一个大于或等于val的元素的双向迭代器。
//                  如果set容器用const限定，则该方法返回的是const类型的双向迭代器。
// upper_bound(val) 返回一个指向当前set容器中第一个大于val的元素的迭代器。
//                  如果set容器用const限定，则该方法返回的是const类型的双向迭代器。
// equal_range(val) 该方法返回一个pair对象（包含2个双向迭代器），
//                  其中pair.first和lower_bound()方法的返回值等价，
//                  pair.second和upper_bound()方法的返回值等价。
//                  也就是说，该方法将返回一个范围，该范围中包含的值为val的元素，
//                  set元素是唯一的，因此最多包含一个元素。
// empty()	        若容器为空，则返回true；否则false。
// size()	          返回当前set容器中存有元素的个数。
// max_size()       返回set容器所能容纳元素的最大个数，
//                  不同的操作系统，其返回值亦不相同。
// insert()         向set容器中插入元素。
// erase()	        删除set容器中存储的元素。
// swap()	          交换2个set容器中存储的所有元素。
//                  这意味着，操作的2个set容器的类型必须相同。
// clear()	        清空set容器中所有的元素，即令set容器的size()为0。
// emplace()        在当前set容器中的指定位置直接构造新元素。
//                  其效果和insert()一样，但效率更高。
// emplace_hint()   在本质上和emplace()在set容器中构造新元素的方式是一样的，
//                  不同之处在于，
//                  使用者必须为该方法提供一个指示新元素生成位置的迭代器，
//                  并作为该方法的第一个参数。
// count(val)       在当前set容器中，查找值为val的元素的个数，并返回。
//                  注意，由于set容器中各元素的值是唯一的，
//                  因此该函数的返回值最大为1。

// 和map容器不同，C++ STL中的set容器类模板中未提供at()成员函数，
// 也未对[]运算符进行重载。
// 因此，要想访问set容器中存储的元素，只能借助set容器的迭代器。
// set容器配置的迭代器类型为双向迭代器。
// 假设p为此类型的迭代器，则其只能进行++p、p++、--p、p--、*p操作，
// 并且2个双向迭代器之间做比较，也只能使用==或者!=运算符。
// 在set容器类模板提供的返回的迭代器的成员函数，
// 指向的只是set容器中存储的元素，而不再是键值对。
// 无论是const类型还是非const类型，都不能用于修改set容器中的值。
// 因为iter迭代器指向的是set容器存储的某个元素，而不是键值对，
// 因此通过*iter可以直接获取该迭代器指向的元素的值。

template <typename T>
void print(std::set<T>& s) {
  for (auto it = s.begin(); it != s.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "[" << s.size() << "]" << std::endl;
}

std::set<char> retSet() {
  std::set<char> s{'h', 'd', 'i'};
  return s;
}
void func1() {
  // 1.调用默认构造函数，创建空的set容器：
  std::set<char> s1;
  // 创建了set容器，该容器采用默认的std::less<T>规则，
  // 会对存储的string类型元素做升序排序。
  // 由于set容器支持随时向内部添加新的元素，
  // 因此创建空set容器的方法是经常使用的。
  print(s1);  // [0]

  // 2.set类模板还支持在创建set容器的同时，
  //   对其进行初始化(其采用默认的std::less<T>规则)：
  std::set<char> s2{'z', 'c', 'a', 'b'};
  print(s2);  // a b c z [4]

  // 3.set类模板中还提供了拷贝（复制）构造函数，
  //   可以实现在创建新set容器的同时，
  //   将已有set容器中存储的所有元素全部复制到新set容器中：
  std::set<char> s3(s2);
  print(s3);  // a b c z [4]
  std::set<char> s4 = s2;
  print(s4);  // a b c z [4]
  // C++11标准还为set类模板新增了移动构造函数，
  // 其功能是实现创建新set容器的同时，利用临时的set容器为其初始化：
  std::set<char> s5(retSet());
  print(s5);  // d h i [3]
  std::set<char> s6 = retSet();
  print(s6);  // d h i [3]
  // 由于retSet()函数的返回值是一个临时set容器，
  // 因此在初始化copyset容器时，
  // 其内部调用的是set类模板中的移动构造函数，而非拷贝构造函数。
  // 无论是调用复制构造函数还是调用拷贝构造函数，
  // 都必须保证这2个容器的类型完全一致。

  // 4.在第3种方式的基础上，set类模板还支持取已有set容器中的部分元素，
  //   来初始化新set容器：
  std::set<char> s7(++s2.begin(), s2.end());
  print(s7);  // b c z [3]

  // 5.借助set类模板定义中第2个参数，
  //   完全可以手动修改set容器中的排序规则：
  std::set<char, std::greater<char> > s8{'a', 'b', 'c'};
  for (auto i = s8.begin(); i != s8.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << "[" << s8.size() << "]" << std::endl;  // c b a [3]
}

void func2() {  // insert
  // 1.只要给定目标元素的值，insert()方法即可将该元素添加到set容器中：
  //   普通引用方式传参：
  //   pair<iterator,bool> insert(const value_type& val);
  //   右值引用方式传参：
  //   pair<iterator,bool> insert(value_type&& val);
  //   以上2种语法格式的insert()方法，返回的都是pair类型的值，
  //   其包含2个数据，一个迭代器和一个bool值：
  //   当向set容器添加元素成功时，该迭代器指向set容器新添加的元素，
  //   bool类型的值为true；
  //   如果添加失败，即证明原已存有相同的元素，
  //   此时返回的迭代器就指向容器中相同的此元素，同时bool类型的值为false。
  std::set<char> s;
  std::pair<std::set<char>::iterator, bool> retpair;
  char ch = 'a';  // 采用普通引用传值方式
  retpair = s.insert(ch);
  std::cout << std::boolalpha << retpair.second << "," << *retpair.first
            << std::endl;   // true,a
  retpair = s.insert('b');  // 采用右值引用传值方式
  std::cout << std::boolalpha << retpair.second << "," << *retpair.first
            << std::endl;  // true,b
  retpair = s.insert('b');
  std::cout << std::boolalpha << retpair.second << "," << *retpair.first
            << std::endl;  // false,b
  print(s);                // a b [2]

  // 2.insert()还可以指定将新元素插入到set容器中的具体位置，其语法格式如下：
  //   以普通引用的方式传递val值：
  //   iterator insert(const_iterator position, const value_type& val);
  //   以右值引用的方式传递val值：
  //   iterator insert(const_iterator position, value_type&& val);
  //   以上2种语法格式中，insert()函数的返回值为迭代器：
  //   当向set容器添加元素成功时，该迭代器指向容器中新添加的元素；
  //   当添加失败时，证明原set容器中已有相同的元素，
  //   该迭代器就指向set容器中相同的这个元素。
  ch = 'c';  // 采用普通引用传值方式
  auto it = s.insert(s.begin(), ch);
  std::cout << *it << std::endl;  // c
  it = s.insert(s.end(), 'd');    // 采用右值引用传值方式
  std::cout << *it << std::endl;  // d
  print(s);                       // a b c d [4]
  // 使用insert()方法将目标元素插入到set容器指定位置后，
  // 如果该元素破坏了容器内部的有序状态，
  // set容器还会自行对新元素的位置做进一步调整。
  // 也就是说，insert()方法中指定新元素插入的位置，
  // 并不一定就是该元素最终所处的位置。

  // 3.insert()支持向set容器中插入其它set容器指定区域内的所有元素，
  //   只要这2个set容器存储的元素类型相同即可：
  //   template <class InputIterator>
  //   void insert(InputIterator first, InputIterator last);
  //   其中first和last都是迭代器，
  //   它们的组合[first,last)可以表示另一set容器中的一块区域。
  std::set<char> s1{'o', 'p', 'q'};
  s.insert(++s1.begin(), s1.end());
  print(s);  // a b c d p q [6]

  // 4.采用如下格式的insert()方法，可实现一次向set容器中添加多个元素：
  //   void insert({E1, E2,...,En});
  s.insert({'j', 'i', 'h'});
  print(s);  // a b c d h i j p q [9]
}

void func3() {
  // emplace()和emplace_hint()是C++11标准加入到set类模板中的，
  // 相比具有同样功能的insert()方法，完成同样的任务，
  // emplace()和emplace_hint()的效率会更高。
  // emplace()方法的语法格式如下：
  // template <class... Args>
  // pair<iterator,bool> emplace (Args&&... args);
  // 该方法的返回值类型为pair类型，其包含2个元素，一个迭代器和一个bool值：
  // 当该方法将目标元素成功添加到set容器中时，
  // 其返回的迭代器指向新插入的元素，同时bool值为true；
  // 当添加失败时，则表明原set容器中已存在相同值的元素，
  // 此时返回的迭代器指向容器中具有相同键的这个元素，同时bool值为false。
  std::set<int> s;
  std::pair<std::set<int>::iterator, bool> r = s.emplace(7);
  std::cout << std::boolalpha << r.second << "," << *r.first
            << std::endl;  // true,7
  r = s.emplace(7);
  std::cout << std::boolalpha << r.second << "," << *r.first
            << std::endl;  // false,7
  print(s);                // 7 [1]

  // emplace_hint()方法的功能和emplace()类似，其语法格式如下：
  // template <class... Args>
  // iterator emplace_hint (const_iterator position, Args&&... args);
  // 和emplace()方法相比，有以下2点不同：
  // 该方法需要额外传入一个迭代器，用来指明新元素添加到set容器的具体位置，
  // 新元素会添加到该迭代器指向元素的前面；
  // 返回值是一个迭代器，而不再是pair对象。
  // 当成功添加元素时，返回的迭代器指向新添加的元素；
  // 反之，如果添加失败，则迭代器就指向set容器和要添加元素的值相同的元素。
  std::set<int>::iterator it = s.emplace_hint(s.begin(), 9);
  std::cout << *it << std::endl;  // 9
  it = s.emplace_hint(s.begin(), 9);
  std::cout << *it << std::endl;  // 9
  print(s);                       // 7 9 [2]

  // 和insert()方法一样，虽然emplace_hint()方法中指定了添加新元素的位置，
  // 但set容器为了保持数据的有序状态，可能会移动其位置。
  // emplace()和emplace_hint()比insert()执行效率高的原因，
  // 和map容器emplace()和emplace_hint()比insert()效率高的原因一样。
}

void func4() {
  // 想删除set容器存储的元素，可以选择用erase()或者clear()成员方法：
  // set类模板中，erase()方法有3种语法格式，分别如下：
  // 删除set容器中值为val的元素：
  // size_type erase(const value_type& val);
  // 删除position迭代器指向的元素：
  // iterator erase(const_iterator position);
  // 删除[first,last)区间内的所有元素：
  // iterator erase(const_iterator first, const_iterator last);
  // 第1种格式的erase()方法，其返回值为一个整数，表示成功删除的元素个数；
  // 后2种格式的erase()方法，返回值都是迭代器，
  // 其指向的是set容器中删除元素之后的第一个元素。
  // 如果要删除的元素就是set容器最后一个元素，
  // 则erase()方法返回的迭代器器和end()方法返回的迭代器相同。
  std::set<int> s{1, 2, 3, 4, 5};
  print(s);  // 1 2 3 4 5 [5]

  int num = s.erase(4);
  std::cout << num << std::endl;  // 1
  std::set<int>::iterator it = s.erase(s.begin());
  std::cout << *it << std::endl;  // 2
  print(s);                       // 2 3 5 [3]

  it = s.erase(++s.begin(), --s.end());
  std::cout << *it << std::endl;  // 5
  print(s);                       // 2 5 [2]

  // 如果需要删除set容器中存储的所有元素，可以使用clear()成员方法。
  // 该方法的语法格式如下：
  // void clear();
  // 该方法不需要传入任何参数，也没有任何返回值。
  s.clear();
  print(s);  // [0]
}

void func5() {
  // 如果只想遍历set容器中指定区域内的部分数据，
  // 则可以借助find()、lower_bound()以及upper_bound()实现。
  // equal_range(val)函数的返回值是一个pair类型数据，其包含2个迭代器，
  // 表示set容器中和指定参数val相等的元素所在的区域，
  // 但由于set容器中存储的元素各不相等，
  // 因此该函数返回的这2个迭代器所表示的范围中，最多只会包含1个元素。
  // 所以它们更适用于multiset容器，几乎不会用于操作set容器。

  std::set<int> s{1, 3, 4, 5};
  std::set<int>::iterator it = s.find(4);
  for (; it != s.end(); ++it) {
    std::cout << *it << std::endl;
  }
  // 4

  if (s.find(6) == s.end()) {
    std::cout << "not found" << std::endl;
  }
  // not found

  if (s.count(4) < 1) {
    std::cout << "no value" << std::endl;
  } else {
    std::cout << "find value" << std::endl;
  }
  // find value

  if (s.count(6) < 1) {
    std::cout << "no value" << std::endl;
  }
  // no value
}

void func6() {
  std::set<int> s{1, 2, 3};
  for (auto it = s.begin(); it != s.end(); ++it) {
    if (*it == 2) {
      // *it = 5;  // 非const的set的元素不能被修改
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}