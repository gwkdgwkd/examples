#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

// find
// find_if
// find_if_not
// find_end
// find_first_of
// adjacent_find

namespace n1 {
// find()函数本质上是一个模板函数，
// 用于在指定范围内查找和目标元素值相等的第一个元素。
// InputIterator find(InputIterator first,
//                    InputIterator last, const T& val);
// 其中，first和last为输入迭代器，
// [first,last)用于指定该函数的查找范围；
// val为要查找的目标元素。
// 正因为first和last的类型为输入迭代器，
// 因此该函数适用于所有的序列式容器。
// 另外，该函数会返回一个输入迭代器，当find()函数查找成功时，
// 其指向的是在[first,last)区域内查找到的第一个目标元素；
// 如果查找失败，则该迭代器的指向和last相同。
// 值得一提的是，find()函数的底层实现，
// 其实就是用==运算符将val和[first,last)区域内的元素逐个进行比对。
// 这也就意味着，[first,last)区域内的元素必须支持==运算符。
// find()函数除了可以作用于序列式容器，还可以作用于普通数组。
// 对于find()函数的底层实现，C++标准库中给出了参数代码：
// template <class InputIterator, class T>
// InputIterator find(InputIterator first,
//                    InputIterator last, const T& val) {
//   while (first != last) {
//     if (*first == val) return first;
//     ++first;
//   }
//   return last;
// }
std::vector<int>::iterator it;
std::vector<int> v1{10, 20, 30, 40, 50};
std::vector<int> v2{10, 21, 30, 41, 50};
std::vector<int> v3{11, 21, 31, 41, 51};
auto print = [](std::vector<int>::iterator& it, std::vector<int>& v) {
  if (it == v.end()) {
    std::cout << "not found" << std::endl;
  } else {
    std::cout << "*it = " << *it << std::endl;
  }
};

void func1() {
  it = std::find(v1.begin(), v1.end(), 30);
  print(it, v1);  // found：30
  it = std::find(v1.begin(), v1.end(), 60);
  print(it, v1);  // not found
}

// 和find()函数相同，find_if()函数也用于在指定区域内执行查找操作。
// 不同的是，前者需要明确指定要查找的元素的值，而后者则允许自定义查找规则。
// 所谓自定义查找规则，实际上指的是有一个形参且返回值类型为bool的函数。
// 值得一提的是，该函数可以是一个普通函数（又称为一元谓词函数），比如：
bool mycomp1(int i) { return ((i % 2) == 1); }
bool mycomp3(int i) { return ((i % 2) == 0); }
// mycomp()就是一个一元谓词函数，其可用来判断一个整数是奇数还是偶数。
// 也可以是一个函数对象，比如：
class mycomp2 {  // 此函数对象的功能和mycomp()函数一样
 public:
  bool operator()(const int& i) { return ((i % 2) == 1); }
};
// 确切地说，find_if()函数会根据指定的查找规则，
// 在指定区域内查找第一个符合该函数要求（使函数返回true）的元素。

// find_if()函数的语法格式如下：
// InputIterator find_if(InputIterator first,
//                       InputIterator last, UnaryPredicate pred);
// 其中，first和last都为输入迭代器，
// 其组合[first,last)用于指定要查找的区域；
// pred用于自定义查找规则。
// 由于first和last都为输入迭代器，意味着该函数适用于所有的序列式容器。
// 甚至当采用适当的谓词函数时，该函数还适用于所有的关联式容器（包括哈希容器）。
// 同时，该函数会返回一个输入迭代器，当查找成功时，
// 该迭代器指向的是第一个符合查找规则的元素；
// 反之，如果find_if()函数查找失败，则该迭代器的指向和last迭代器相同。
// find_if()函数底层实现的参考代码:
// template <class InputIterator, class UnaryPredicate>
// InputIterator find_if(InputIterator first,
//                       InputIterator last, UnaryPredicate pred) {
//   while (first != last) {
//     if (pred(*first)) return first;
//     ++first;
//   }
//   return last;
// }

void func2() {
  it = std::find_if(v1.begin(), v1.end(), mycomp1);
  print(it, v1);  // not found
  it = std::find_if(v2.begin(), v2.end(), mycomp2());
  print(it, v2);  // *it = 21
}

// find_if_not()函数和find_if()函数的功能恰好相反，
// find_if()函数用于查找符合谓词函数规则的第一个元素，
// 而find_if_not()函数则用于查找第一个不符合谓词函数规则的元素。
// find_if_not()函数的语法规则如下所示：
// InputIterator find_if_not(InputIterator first,
//                           InputIterator last, UnaryPredicate pred);
// 其中，first和last都为输入迭代器，
// [first,last)用于指定查找范围；
// pred用于自定义查找规则。
// 和find_if()函数一样，find_if_not()函数也适用于所有的容器，
// 包括所有序列式容器和关联式容器。
// 同样，该函数也会返回一个输入迭代器，当find_if_not()函数查找成功时，
// 该迭代器指向的是查找到的那个元素；
// 反之，如果查找失败，该迭代器的指向和last迭代器相同。
// find_if_not()函数的底层实现和find_if()函数非常类似:
// template <class InputIterator, class UnaryPredicate>
// InputIterator find_if_not(InputIterator first,
//                           InputIterator last, UnaryPredicate pred) {
//   while (first != last) {
//     if (!pred(*first)) return first;
//     ++first;
//   }
//   return last;
// }
void func3() {
  it = std::find_if_not(v3.begin(), v3.end(), mycomp1);
  print(it, v3);  // not found
  it = std::find_if(v3.begin(), v3.end(), mycomp3);
  print(it, v3);  // not found
  it = std::find_if_not(v2.begin() + 1, v2.end(), mycomp2());
  print(it, v2);  // *it = 30
  it = std::find_if(v2.begin() + 1, v2.end(), mycomp3);
  print(it, v2);  // *it = 30
}

void test() {
  func1();
  func2();
  func3();
}
}  // namespace n1

namespace n2 {
// find_end()函数定义在<algorithm>头文件中，
// 常用于在序列A中查找序列B最后一次出现的位置。
// find_end()函数的语法格式有2种：
// 查找序列[first1,last1)中最后一个子序列[first2,last2)：
// ForwardIterator find_end(ForwardIterator first1,
//                          ForwardIterator last1,
//                          ForwardIterator first2,
//                          ForwardIterator last2);
// 查找序列[first2,last2)中，
// 和[first2,last2)序列满足pred规则的最后一个子序列：
// ForwardIterator find_end(ForwardIterator first1,
//                          ForwardIterator last1,
//                          ForwardIterator first2,
//                          ForwardIterator last2,
//                          BinaryPredicate pred);
// 其中，各个参数的含义如下：
// first1、last1：都为正向迭代器，其组合[first1,last1)用于指定查找范围；
// first2、last2：都为正向迭代器，其组合[first2,last2)用于指定要查找的序列；
// pred：用于自定义查找规则。
// 该规则实际上是一个包含2个参数且返回值类型为bool的函数，
// 第一个参数接收[first1,last1)范围内的元素，
// 第二个参数接收[first2,last2)范围内的元素。
// 函数定义的形式可以是普通函数，也可以是函数对象。
// 实际上，第一种语法格式也可以看做是包含一个默认的pred参数，
// 该参数指定的是一种相等规则：
// 在[first1,last1)内查找和[first2,last2)中各个元素对应相等的子序列；
// 而借助第二种语法格式，可以自定义一个当前场景需要的匹配规则。
// 同时，find_end()函数会返回一个正向迭代器，
// 当函数查找成功时，该迭代器指向查找到的子序列中的第一个元素；
// 反之，如果查找失败，则该迭代器的指向和last1迭代器相同。
bool mycomp1(int i, int j) { return (i % j == 0); }
class mycomp2 {
 public:
  bool operator()(const int& i, const int& j) { return (i % j == 0); }
};
// find_end()函数的第一种语法格式，其底层是借助==运算符实现的。
// 这意味着，[first1,last1]和[first2,last2]区域内的元素，
// 如果为自定义的类对象或结构体变量时，使用该函数之前需要对==运算符进行重载。
// find_end()函数底层实现的参考代码
// template <class ForwardIterator1, class ForwardIterator2>
// ForwardIterator1 find_end(ForwardIterator1 first1,
//                           ForwardIterator1 last1,
//                           ForwardIterator2 first2,
//                           ForwardIterator2 last2) {
//   if (first2 == last2) return last1;  // specified in C++11
//   ForwardIterator1 ret = last1;
//   while (first1 != last1) {
//     ForwardIterator1 it1 = first1;
//     ForwardIterator2 it2 = first2;
//     while (*it1 == *it2) {  // or: while (pred(*it1,*it2)) for version (2)
//       ++it1;
//       ++it2;
//       if (it2 == last2) {
//         ret = first1;
//         break;
//       }
//       if (it1 == last1) return ret;
//     }
//     ++first1;
//   }
//   return ret;
// }
// C++ STL标准库还提供了和find_end()功能恰恰相反的search()。

void test() {
  std::vector<int> v{1, 2, 3, 4, 8, 12, 18, 1, 2, 3};
  int a[] = {1, 2, 3, 2, 4, 6};
  std::vector<int>::iterator it = std::find_end(v.begin(), v.end(), a, a + 3);
  if (it != v.end()) {
    std::cout << "最后一个{1,2,3}的起始位置为：" << it - v.begin()
              << ",*it = " << *it << std::endl;
  }
  // 最后一个{1,2,3}的起始位置为：7,*it = 1

  it = std::find_end(v.begin(), v.end(), a, a + 7);
  if (it == v.end()) {
    std::cout << "not found" << std::endl;
  }
  // not found

  it = std::find_end(v.begin(), v.end(), a + 4, a + 7, mycomp1);
  // it = std::find_end(v.begin(), v.end(), a + 4, a + 7, mycomp2());
  if (it != v.end()) {
    std::cout << "最后一个{2,4,6}的起始位置为：" << it - v.begin()
              << ",*it = " << *it << std::endl;
  }
  // 最后一个{2,3,4}的起始位置为：4,*it = 8
}
}  // namespace n2

namespace n3 {
// 需要在A序列中查找和B序列中任意元素相匹配的第一个元素，
// 这时就可以使用find_first_of()函数。
// find_first_of()函数定义于<algorithm>头文件中，
// find_first_of()函数有2种语法格式，分别是：
// 以判断两者相等作为匹配规则：
// InputIterator find_first_of(InputIterator first1,
//                             InputIterator last1,
//                             ForwardIterator first2,
//                             ForwardIterator last2);
// 以pred作为匹配规则：
// InputIterator find_first_of(InputIterator first1,
//                             InputIterator last1,
//                             ForwardIterator first2,
//                             ForwardIterator last2,
//                             BinaryPredicate pred);
// 其中，各个参数的含义如下：
// first1、last1：都为输入迭代器，
// 它们的组合[first1,last1)用于指定该函数要查找的范围；
// first2、last2：都为正向迭代器，
// 它们的组合[first2,last2)用于指定要进行匹配的元素所在的范围；
// pred：可接收一个包含2个形参且返回值类型为bool的函数，
//      该函数可以是普通函数（又称为二元谓词函数），也可以是函数对象。
// find_first_of()函数用于在[first1,last1)范围内，
// 查找和[first2,last2)中任何元素相匹配的第一个元素。
// 如果匹配成功，该函数会返回一个指向该元素的输入迭代器；
// 反之，则返回一个和last1迭代器指向相同的输入迭代器。
// 值得一提的是，不同语法格式的匹配规则也是不同的：
// 第1种语法格式：逐个取[first1,last1)范围内的元素（假设为A），
// 和[first2,last2)中的每个元素（假设为B）做A==B运算，如果成立则匹配成功；
// 第2种语法格式：逐个取[first1,last1)范围内的元素（假设为A），
// 和[first2,last2)中的每个元素（假设为B）一起带入pred(A,B)谓词函数，
// 如果函数返回true则匹配成功。
// 注意，当采用第一种语法格式时，
// [first1,last1)或者[first2,last2)范围内的元素，
// 如果类型为自定义的类对象或者结构体变量，
// 此时应对==运算符进行重载，使其适用于当前场景。
// 自定义二元谓词函数，作为find_first_of()函数的匹配规则：
bool mycomp1(int c1, int c2) { return (c2 % c1 == 0); }
// 以函数对象的形式定义一个find_first_of()函数的匹配规则
class mycomp2 {
 public:
  bool operator()(const int& c1, const int& c2) { return (c2 % c1 == 0); }
};
// find_first_of()函数底层实现的参考代码：
// template <class InputIt, class ForwardIt, class BinaryPredicate>
// InputIt find_first_of(InputIt first, InputIt last,
//                       ForwardIt s_first,
//                       ForwardIt s_last,
//                       BinaryPredicate p) {
//   for (; first != last; ++first) {
//     for (ForwardIt it = s_first; it != s_last; ++it) {
//       // 第二种语法格式换成if(p(*first, *it))
//       if (p(*first, *it)) {
//         return first;
//       }
//     }
//   }
//   return last;
// }

void test() {
  char str[] = "abctefstl/";
  char ch[] = "stlopq";
  // 调用第一种语法格式，找到str1中和stl任一字符相同的第一个字符：
  char* p = std::find_first_of(str, str + strlen(str), ch, ch + 4);
  if (p != str + strlen(str)) {
    std::cout << "p[" << p - str << "] = " << *p << std::endl;
  }
  // p[3] = t

  p = std::find_first_of(str, str + strlen(str), ch + 3, ch + 7);
  if (p == str + strlen(str)) {
    std::cout << "not found form " << ch + 3 << std::endl;
  }
  // not found form opq

  std::vector<int> v{5, 7, 3, 9};
  int inter[] = {4, 6, 8};
  // 调用第二种语法格式，
  // 找到v容器中和3、5、7任一元素有c2%c1=0关系的第一个元素：
  std::vector<int>::iterator it =
      find_first_of(v.begin(), v.end(), inter, inter + 3, mycomp2());
  if (it != v.end()) {
    std::cout << "*it = " << *it << std::endl;
  }
  // *it = 3
}
}  // namespace n3

namespace n4 {
// adjacent_find()函数用于在指定范围内查找2个连续相等的元素。
// 该函数的语法格式为：
// 查找2个连续相等的元素：
// ForwardIterator adjacent_find(ForwardIterator first,
//                               ForwardIterator last);
// 查找2个连续满足pred规则的元素：
// ForwardIterator adjacent_find(ForwardIterator first,
//                               ForwardIterator last,
//                               BinaryPredicate pred);
// 其中，first和last都为正向迭代器，
// 其组合[first,last)用于指定该函数的查找范围；
// pred用于接收一个包含2个参数且返回值类型为bool的函数，
// 以实现自定义查找规则。
// 值得一提的是，pred参数接收的函数既可以定义为普通函数，
// 也可以用函数对象的形式定义。
// 另外，该函数会返回一个正向迭代器，当函数查找成功时，
// 该迭代器指向的是连续相等元素的第1个元素；
// 而如果查找失败，该迭代器的指向和last迭代器相同。
// 以创建普通函数的形式定义一个查找规则
bool mycomp1(int i, int j) { return (i == j); }
// 以函数对象的形式定义一个查找规则
class mycomp2 {
 public:
  bool operator()(const int& _Left, const int& _Right) {
    return (_Left == _Right);
  }
};
// adjacent_find()函数底层实现的参考代码：
// template <class ForwardIterator>
// ForwardIterator adjacent_find(ForwardIterator first,
//                               ForwardIterator last) {
//   if (first != last) {
//     ForwardIterator next = first;
//     ++next;
//     while (next != last) {
//       if (*first == *next)  // 或者if(pred(*first,*next)), 对应第二种语法格式
//         return first;
//       ++first;
//       ++next;
//     }
//   }
//   return last;
// }

void test() {
  std::vector<int> v{5, 20, 5, 30, 30, 20, 10, 10, 20};
  std::vector<int>::iterator it = adjacent_find(v.begin(), v.end());
  if (it != v.end()) {
    std::cout << "one : " << *it << std::endl;
  }
  it = adjacent_find(++it, v.end(), mycomp2());
  if (it != v.end()) {
    std::cout << "two : " << *it << std::endl;
  }
  // one : 30
  // two : 10
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    case 2:
      n3::test();
      break;
    case 3:
      n4::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}