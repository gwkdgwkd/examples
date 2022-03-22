#include <algorithm>
#include <iostream>
#include <vector>

// partition可直译为“分组”，partition()函数可根据用户自定义的筛选规则，重新排列指定区域内存储的数据，
// 使其分为2组，第一组为符合筛选条件的数据，另一组为不符合筛选条件的数据。
// partition()函数只会根据筛选条件将数据进行分组，并不关心分组后各个元素具体的存储位置。
// 如果想在分组之后仍不改变各元素之间的相对位置，可以选用stable_partition()函数。
// partition()、stable_partition()、partition_copy()和partition_point()函数定义于<algorithm>头文件中。

bool mycomp1(int i) { return (i % 2) == 0; }
class mycomp2 {
 public:
  bool operator()(const int& i) { return (i % 2 == 1); }
};

// partition()函数的语法格式：
// ForwardIterator partition(ForwardIterator first, ForwardIterator last, UnaryPredicate pred);
// 其中，first和last都为正向迭代器，其组合[first,last)用于指定该函数的作用范围；pred用于指定筛选规则。
// 所谓筛选规则，其本质就是一个可接收1个参数且返回值类型为bool的函数，可以是普通函数，也可以是一个函数对象。
// 同时，partition()函数还会返回一个正向迭代器，其指向的是两部分数据的分界位置，更确切地说，指向的是第二组数据中的第1个元素。
void func1() {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int>::iterator bound =
      std::partition(v.begin(), v.end(), mycomp1);
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;                          // 8 2 6 4 5 3 7 1 9
  std::cout << "bound = " << *bound << std::endl;  // bound = 5

  bound = std::partition(v.begin(), v.end(), mycomp2());
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;                          // 9 1 7 3 5 4 6 2 8
  std::cout << "bound = " << *bound << std::endl;  // bound = 4
}

// stable_partition()函数可以保证对指定区域内数据完成分组的同时，不改变各组内元素的相对位置。
// stable_partition()函数语法格式如下：
// BidirectionalIterator stable_partition (BidirectionalIterator first,
//  BidirectionalIterator last, UnaryPredicate pred);
// 其中，first和last都为双向迭代器，其组合[first,last)用于指定该函数的作用范围；pred用于指定筛选规则。
// 同时，stable_partition()函数还会返回一个双向迭代器，其指向的是两部分数据的分界位置，
// 更确切地说，指向的是第二组数据中的第1个元素。
void func2() {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int>::iterator bound =
      std::stable_partition(v.begin(), v.end(), mycomp1);
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;                          // 2 4 6 8 1 3 5 7 9
  std::cout << "bound = " << *bound << std::endl;  // bound = 1

  bound = std::stable_partition(v.begin(), v.end(), mycomp2());
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;                          // 1 3 5 7 9 2 4 6 8
  std::cout << "bound = " << *bound << std::endl;  // bound = 2
}

// 需要类似partition()或者stable_partition()函数“分组”的功能，但并不想对原序列做任何修改。
// 这种情况下，就可以考虑使用partition_copy()函数。
// 和stable_partition()一样，partition_copy()函数也能按照某个筛选规则对指定区域内的数据进行“分组”，
// 并且分组后不会改变各个元素的相对位置。
// 更重要的是，partition_copy()函数不会对原序列做修改，而是以复制的方式将序列中各个元组“分组”到其它的指定位置存储。
// pair<OutputIterator1,OutputIterator2> partition_copy (
//                     InputIterator first, InputIterator last,
//                     OutputIterator1 result_true, OutputIterator2 result_false,
//                     UnaryPredicate pred);
// 其中，各个参数的含义为：
//  first、last：都为输入迭代器，其组合[first,last)用于指定该函数处理的数据区域；
//  result_true：为输出迭代器，其用于指定某个存储区域，以存储满足筛选条件的数据；
//  result_false：为输出迭代器，其用于指定某个存储区域，以存储满足筛选条件的数据；
//  pred：用于指定筛选规则，其本质就是接收一个具有1个参数且返回值类型为bool的函数。
//       注意，该函数既可以是普通函数，还可以是一个函数对象。
// 除此之外，该函数还会返回一个pair类型值，其包含2个迭代器，
//  第一个迭代器指向的是result_true区域内最后一个元素之后的位置；
//  第二个迭代器指向的是 result_false区域内最后一个元素之后的位置
// partition_copy()函数底层实现的参考代码
// template <class InputIterator, class OutputIterator1, class OutputIterator2,
//           class UnaryPredicate pred>
// pair<OutputIterator1, OutputIterator2> partition_copy(
//     InputIterator first, InputIterator last, OutputIterator1 result_true,
//     OutputIterator2 result_false, UnaryPredicate pred) {
//   while (first != last) {
//     if (pred(*first)) {
//       *result_true = *first;
//       ++result_true;
//     } else {
//       *result_false = *first;
//       ++result_false;
//     }
//     ++first;
//   }
//   return std::make_pair(result_true, result_false);
// }
void func3() {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
  int b[10] = {0}, c[10] = {0};
  std::pair<int*, int*> result =
      std::partition_copy(v.begin(), v.end(), b, c, mycomp1);
  std::cout << "b[10]：";
  for (int* p = b; p < result.first; p++) {
    std::cout << *p << " ";
  }
  std::cout << std::endl;  // b[10]：2 4 6 8
  std::cout << "c[10]：";
  for (int* p = c; p < result.second; p++) {
    std::cout << *p << " ";
  }
  std::cout << std::endl;  // c[10]：1 3 5 7 9
}

// 对于如何在已分好组的数据中找到分界位置，C++11标准库提供了专门解决此问题的函数，即partition_point()函数。
// ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, UnaryPredicate pred);
// 其中，first和last为正向迭代器，[first,last)用于指定该函数的作用范围；pred用于指定数据的筛选规则。
// 所谓筛选规则，其实就是包含1个参数且返回值类型为bool的函数，此函数可以是一个普通函数，也可以是一个函数对象。
// 同时，该函数会返回一个正向迭代器，该迭代器指向的是[first,last]范围内第一个不符合pred筛选规则的元素。
// partition_point()返回了一个指向元素1的迭代器，而该元素为容器中第一个不符合规则的元素，同时其也可以第二组数据中第一个元素。
// C++11标准库中给出了partition_point()函数底层实现：
// template <class ForwardIterator, class UnaryPredicate>
// ForwardIterator partition_point(ForwardIterator first, ForwardIterator last, UnaryPredicate pred) {
//   auto n = distance(first, last);
//   while (n > 0) {
//     ForwardIterator it = first;
//     auto step = n / 2;
//     std::advance(it, step);
//     if (pred(*it)) {
//       first = ++it;
//       n -= step + 1;
//     } else
//       n = step;
//   }
//   return first;
// }
void func4() {
  std::vector<int> v{1, 7, 9, 3, 5, 8, 2, 6, 4};
  std::vector<int>::iterator iter =
      std::partition_point(v.begin(), v.end(), mycomp2());
  std::cout << "*iter = " << *iter << std::endl;
  // *iter = 8
  for (auto it = v.begin(); it != iter; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 1 7 9 3 5
  for (auto it = iter; it != v.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 8 2 6 4
}

int main() {
  func1();
  func2();
  func3();
  func4();
  return 0;
}