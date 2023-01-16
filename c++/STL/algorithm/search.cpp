#include <algorithm>
#include <iostream>
#include <vector>

// search
// search_n

// find_end()函数用于在序列中查找序列B最后一次出现的位置。
// 那么，如果想知道序列B在序列中第一次出现的位置，该如何实现呢？
// 可以借助search()函数。
// search()函数定义在<algorithm>头文件中，
// 其功能恰好和find_end()函数相反，用于在序列A中查找序列B第一次出现的位置。
// 和find_end()相同，search()函数也提供有以下2种语法格式：
// 查找[first1,last1)范围内第一个[first2,last2)子序列：
// ForwardIterator search(ForwardIterator first1,
//                        ForwardIterator last1,
//                        ForwardIterator first2,
//                        ForwardIterator last2);
// 查找[first1,last1)范围内，
// 和[first2,last2)序列满足pred规则的第一个子序列：
// ForwardIterator search(ForwardIterator first1,
//                        ForwardIterator last1,
//                        ForwardIterator first2,
//                        ForwardIterator last2,
//                        BinaryPredicate pred);
// 其中，各个参数的含义分别为：
// first1、last1：都为正向迭代器，其组合[first1,last1)用于指定查找范围；
// first2、last2：都为正向迭代器，其组合[first2,last2)用于指定要查找的序列；
//  pred：用于自定义查找规则。
//       该规则实际上是一个包含2个参数且返回值类型为bool的函数。
//       第一个参数接收[first1, last1)范围内的元素，
//       第二个参数接收[first2,last2)范围内的元素。
//       函数定义的形式可以是普通函数，也可以是函数对象。
// 实际上，第一种语法格式也可以看做是包含一个默认的pred参数，
// 该参数指定的是一种相等规则，
// 即在[first1,last1)范围内查找和[first2,last2)中各个元素对应相等的子序列；
// 而借助第二种语法格式，可以自定义一个当前场景需要的匹配规则。
// 同时，search()函数会返回一个正向迭代器，
// 当函数查找成功时，该迭代器指向查找到的子序列中的第一个元素；
// 反之，如果查找失败，则该迭代器的指向和last1迭代器相同。
// search()函数的第一种语法格式，其底层是借助==运算符实现的。
// 这意味着，[first1,last1]和[first2,last2]区域内的元素，
// 如果为自定义的类对象或结构体变量时，使用该函数之前需要对==运算符进行重载。
bool mycomp1(int i, int j) { return (i % j == 0); }
class mycomp2 {
 public:
  bool operator()(const int& i, const int& j) { return (i % j == 0); }
};
// search()函数底层实现的参考代码：
// template <class ForwardIterator1, class ForwardIterator2>
// ForwardIterator1 search(ForwardIterator1 first1,
//                         ForwardIterator1 last1,
//                         ForwardIterator2 first2,
//                         ForwardIterator2 last2) {
//   if (first2 == last2) return first1;
//   while (first1 != last1) {
//     ForwardIterator1 it1 = first1;
//     ForwardIterator2 it2 = first2;
//     while (*it1 == *it2) {  // 或者 while (pred(*it1,*it2)) 对应第二种语法格式
//       if (it2 == last2) return first1;
//       if (it1 == last1) return last1;
//       ++it1;
//       ++it2;
//     }
//     ++first1;
//   }
//   return last1;
// }
void func1() {
  std::vector<int> v{1, 2, 3, 4, 8, 12, 18, 1, 2, 3};
  int a1[] = {1, 2, 3};
  std::vector<int>::iterator it = search(v.begin(), v.end(), a1, a1 + 3);
  if (it != v.end()) {
    std::cout << "第一个{1,2,3}的起始位置为：" << it - v.begin()
              << ",*it = " << *it << std::endl;
  }
  // 第一个{1,2,3}的起始位置为：0,*it = 1

  int a2[] = {2, 4, 6};
  it = search(v.begin(), v.end(), a2, a2 + 3, mycomp2());
  if (it != v.end()) {
    std::cout << "第一个{2,4,6}的起始位置为：" << it - v.begin()
              << ",*it = " << *it << std::endl;
  }
  // 第一个{2,4,6}的起始位置为：3,*it = 4
}

// 和search()一样，search_n()函数也定义在<algorithm>头文件中，
// 用于在指定区域内查找第一个符合要求的子序列。
// 不同之处在于，前者查找的子序列中可包含多个不同的元素，
// 而后者查找的只能是包含多个相同元素的子序列。
// search_n()函数的语法格式如下：
// 在[first,last]中查找count个val第一次连续出现的位置：
// ForwardIterator search_n(ForwardIterator first,
//                          ForwardIterator last,
//                          Size count, const T& val);
// 在[first,last]中查找第一个序列，
// 该序列和count个val满足pred匹配规则：
// ForwardIterator search_n(ForwardIterator first,
//                          ForwardIterator last,
//                          Size count, const T& val,
//                          BinaryPredicate pred);
// 其中，各个参数的含义分别为：
// first、last：都为正向迭代器，其组合[first,last)用于指定查找范围；
// count、val：指定要查找的元素个数和元素值；
// pred：用于自定义查找规则。
//       该规则实际上是一个包含2个参数且返回值类型为bool的函数。
//       第一个参数接收[first, last)范围内的元素，第二个参数接收val。
//       函数定义的形式可以是普通函数，也可以是函数对象。
// 实际上，第一种语法格式也可以看做是包含一个默认的pred参数，
// 该参数指定的是一种相等规则，
// 即在[first,last)范围内查找和count个val相等的子序列；
// 而借助第二种语法格式，可以自定义一个当前场景需要的匹配规则。
// 同时，search_n()函数会返回一个正向迭代器，
// 当函数查找成功时，该迭代器指向查找到的子序列中的第一个元素；
// 反之，如果查找失败，则该迭代器的指向和last迭代器相同。
// search_n()函数的第一种语法格式，其底层是借助==运算符实现的。
// 这意味着，如果[first,last]区域内的元素为自定义的类对象或结构体变量时，
// 使用此格式的search_n()函数之前，需要对==运算符进行重载。

// C++ STL标准库官方给出了search_n()函数底层实现的参考代码：
// template <class ForwardIterator, class Size, class T>
// ForwardIterator search_n(ForwardIterator first,
//                          ForwardIterator last,
//                          Size count, const T& val) {
//   ForwardIterator it, limit;
//   Size i;
//   limit = first;
//   std::advance(limit, std::distance(first, last) - count);
//   while (first != limit) {
//     it = first;
//     i = 0;
//     while (*it == val)  // 或者while(pred(*it,val))，对应第二种格式
//     {
//       ++it;
//       if (++i == count) return first;
//     }
//     ++first;
//   }
//   return last;
// }
void func2() {
  int a[] = {1, 2, 3, 4, 4, 4, 1, 2, 3, 4, 4, 4};
  int* it = std::search_n(a, a + 12, 3, 4);
  if (it != a + 12) {
    std::cout << "one：" << it - a << ",*it = " << *it << std::endl;
  }
  // one：3,*it = 4

  std::vector<int> v{1, 2, 4, 8, 3, 4, 6, 8};
  std::vector<int>::iterator iter =
      std::search_n(v.begin(), v.end(), 3, 2, mycomp1);
  if (iter != v.end()) {
    std::cout << "two：" << iter - v.begin() << ",*iter = " << *iter
              << std::endl;
  }
  // two：1,*iter = 2
}

int main() {
  func1();
  func2();
  return 0;
}