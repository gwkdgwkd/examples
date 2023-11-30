#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// merge
// inplace_merge

// 需要将2个有序序列合并为1个有序序列，可以借助merge()或inplace_merge()实现。
// merge()和inplace_merge()函数都定义在<algorithm>头文件中
// merge()函数用于将2个有序序列合并为1个有序序列，
// 前提是这2个有序序列的排序规则相同（要么都是升序，要么都是降序）。
// 并且最终借助该函数获得的新有序序列，其排序规则也和这2个有序序列相同。
// merge()函数设计了以下2种语法格式：
// 以默认的升序排序作为排序规则
// OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
//                      InputIterator2 first2, InputIterator2 last2,
//                      OutputIterator result);
// 以自定义的comp规则作为排序规则
// OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
//                      InputIterator2 first2, InputIterator2 last2,
//                      OutputIterator result, Compare comp);
// first1、last1、first2以及last2都为输入迭代器，
// [first1,last1)和[first2,last2)各用来指定一个有序序列；
// result为输出迭代器，用于为最终生成的新有序序列指定存储位置；
// comp用于自定义排序规则。
// 同时，该函数会返回一个输出迭代器，其指向的是新有序序列中最后一个元素之后的位置。

// 第一种格式，[first1,last1)和[first2,last2)指定区域内的元素必须支持<小于运算符；
// 第二种格式，[first1,last1)和[first2,last2)区域的元素必须支持comp内的比较运算符。

namespace n1 {
// merge()函数底层是通过拷贝的方式实现合并操作的。
// 换句话说，采用merge()合并的同时，并不会对first和second数组有任何影响。

void func() {
  int first[] = {5, 10, 15, 20, 25};
  int second[] = {7, 17, 27, 37, 47, 57};
  std::vector<int> v(11);
  std::merge(first, first + 5, second, second + 6, v.begin());

  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 7 10 15 17 20 25 27 37 47 57

  std::copy(std::begin(first), std::end(first),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 10 15 20 25

  std::copy(std::begin(second), std::end(second),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 7 17 27 37 47 57
}
}  // namespace n1

namespace n2 {
// 对于2个有序序列是各自存储还是存储到一起，merge()函数并不关心，
// 只需要给它传入恰当的迭代器（或指针），该函数就可以正常工作。
void func() {
  int first[] = {5, 10, 15, 20, 25, 7, 17, 27, 37, 47, 57};
  std::vector<int> v(11);  // 必须有足够的空间，不然段错误
  std::merge(first, first + 5, first + 5, first + 11, v.begin());
  for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << ' ';
  }
  std::cout << std::endl;  // 5 7 10 15 17 20 25 27 37 47 57
}
}  // namespace n2

namespace n3 {
bool comp1(int i, int j) { return i % 10 < j % 10; }
class comp2 {
 public:
  bool operator()(const int& i, const int& j) { return (i / 10 < j / 10); }
};
void func() {
  int first[] = {11, 32, 54, 76};
  int second[] = {23, 45, 67, 88};
  std::vector<int> v(8);

  std::merge(first, first + sizeof(first) / sizeof(int), second,
             second + sizeof(second) / sizeof(int), v.begin());
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 11 23 32 45 54 67 76 88

  std::merge(first, first + sizeof(first) / sizeof(int), second,
             second + sizeof(second) / sizeof(int), v.begin(), comp1);
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 11 32 23 54 45 76 67 88

  std::merge(first, first + sizeof(first) / sizeof(int), second,
             second + sizeof(second) / sizeof(int), v.begin(), comp2());
  std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 11 23 32 45 54 67 76 88
}
}  // namespace n3

namespace n4 {
// 当2个有序序列存储在同一个数组或容器中时，如果想将它们合并为1个有序序列，
// 除了使用merge()函数，更推荐使用inplace_merge()函数。
// 和merge()函数相比，inplace_merge()函数的语法格式要简单很多：
// 默认采用升序的排序规则：
// void inplace_merge(BidirectionalIterator first,
//                    BidirectionalIterator middle,
//                    BidirectionalIterator last);
// 采用自定义的comp排序规则：
// void inplace_merge(BidirectionalIterator first,
//                    BidirectionalIterator middle,
//                    BidirectionalIterator last, Compare comp);
// first、middle和last都为双向迭代器，
// [first,middle)和[middle,last)各表示一个有序序列。
// 和merge()一样，inplace_merge()也要求[first,middle)和[middle,last)这2个序列，
// 必须遵循相同的排序规则，且当采用第一种语法格式时，这2个序列元素必须支持<小于运算符；
// 同样，当采用第二种语法格式时，这2个序列中的元素必须支持comp排序规则内部的比较运算符。
// 不同之处在于，merge()函数会将最终合并的有序序列存储在其它数组或容器中，
// 而inplace_merge()函数则将最终合并的有序序列存储在[first,last)区域中。
void func() {
  int v[] = {5, 10, 15, 20, 25, 7, 17, 27, 37, 47, 57};
  std::inplace_merge(v, v + 5, v + 11);

  std::copy(std::begin(v), std::end(v),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 5 7 10 15 17 20 25 27 37 47 57
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
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}