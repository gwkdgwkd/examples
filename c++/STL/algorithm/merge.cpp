#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// 需要将2个有序序列合并为1个有序序列，这时就可以借助merge()或者inplace_merge()函数实现。
// merge()和inplace_merge()函数都定义在<algorithm>头文件中
// merge()函数用于将2个有序序列合并为1个有序序列，前提是这2个有序序列的排序规则相同（要么都是升序，要么都是降序）。
// 并且最终借助该函数获得的新有序序列，其排序规则也和这2个有序序列相同。
// merge()函数设计了以下2种语法格式：
// 以默认的升序排序作为排序规则
// OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
//                      InputIterator2 first2, InputIterator2 last2,
//                      OutputIterator result);
// 以自定义的 comp 规则作为排序规则
// OutputIterator merge(InputIterator1 first1, InputIterator1 last1,
//                      InputIterator2 first2, InputIterator2 last2,
//                      OutputIterator result, Compare comp);
// first1、last1、first2以及last2都为输入迭代器，[first1, last1)和[first2, last2)各用来指定一个有序序列；
// result为输出迭代器，用于为最终生成的新有序序列指定存储位置；
// comp用于自定义排序规则。同时，该函数会返回一个输出迭代器，其指向的是新有序序列中最后一个元素之后的位置。
// 注意，当采用第一种语法格式时，[first1, last1)和[first2, last2)指定区域内的元素必须支持<小于运算符；
// 同样当采用第二种语法格式时，[first1, last1)和[first2, last2)指定区域内的元素必须支持comp排序规则内的比较运算符。

// 当2个有序序列存储在同一个数组或容器中时，如果想将它们合并为 1 个有序序列，除了使用merge()函数，更推荐使用inplace_merge()函数。
// 和merge()函数相比，inplace_merge()函数的语法格式要简单很多：
// 默认采用升序的排序规则
// void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
//                     BidirectionalIterator last);
// 采用自定义的comp排序规则
// void inplace_merge (BidirectionalIterator first, BidirectionalIterator middle,
//                     BidirectionalIterator last, Compare comp);
// first、middle和last都为双向迭代器，[first, middle)和[middle, last)各表示一个有序序列。
// 和merge()函数一样，inplace_merge()函数也要求[first, middle)和[middle, last)指定的这2个序列必须遵循相同的排序规则，且当采用第一种语法格式时，
// 这2个序列中的元素必须支持 < 小于运算符；同样，当采用第二种语法格式时，这2个序列中的元素必须支持comp排序规则内部的比较运算符。
// 不同之处在于，merge()函数会将最终合并的有序序列存储在其它数组或容器中，而inplace_merge()函数则将最终合并的有序序列存储在[first, last)区域中。

int main() {
  int first[] = {5, 10, 15, 20, 25};
  int second[] = {7, 17, 27, 37, 47, 57};
  vector<int> myvector(11);
  merge(first, first + 5, second, second + 6, myvector.begin());
  for (vector<int>::iterator it = myvector.begin(); it != myvector.end();
       ++it) {
    cout << *it << ' ';
  }
  cout << endl;  // 5 7 10 15 17 20 25 27 37 47 57
  // merge()函数底层是通过拷贝的方式实现合并操作的。换句话说，上面程序在采用merge()函数实现合并操作的同时，并不会对first和second数组有任何影响。

  // 对于2个有序序列是各自存储（像first和second这样）还是存储到一起，merge()函数并不关心，只需要给它传入恰当的迭代器（或指针），该函数就可以正常工作。
  int first1[] = {5, 10, 15, 20, 25, 7, 17, 27, 37, 47, 57};
  vector<int> myvector1(11);
  merge(first1, first1 + 5, first1 + 5, first1 + 11, myvector1.begin());
  for (vector<int>::iterator it = myvector1.begin(); it != myvector1.end();
       ++it) {
    cout << *it << ' ';
  }
  cout << endl;  // 5 7 10 15 17 20 25 27 37 47 57

  int first2[] = {5, 10, 15, 20, 25, 7, 17, 27, 37, 47, 57};
  inplace_merge(first2, first2 + 5, first2 + 11);
  for (int i = 0; i < 11; i++) {
    cout << first2[i] << " ";
  }
  cout << endl;  // 5 7 10 15 17 20 25 27 37 47 57

  return 0;
}