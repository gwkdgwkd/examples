#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

// find()函数本质上是一个模板函数，用于在指定范围内查找和目标元素值相等的第一个元素。
// InputIterator find (InputIterator first, InputIterator last, const T& val);
// 其中，first和last为输入迭代器，[first, last)用于指定该函数的查找范围；val为要查找的目标元素。
// 正因为first和last的类型为输入迭代器，因此该函数适用于所有的序列式容器。
// 另外，该函数会返回一个输入迭代器，当find()函数查找成功时，其指向的是在[first, last)区域内查找到的第一个目标元素；如果查找失败，则该迭代器的指向和last相同。
// 值得一提的是，find()函数的底层实现，其实就是用==运算符将val和[first, last)区域内的元素逐个进行比对。这也就意味着，[first, last)区域内的元素必须支持==运算符。
// find()函数除了可以作用于序列式容器，还可以作用于普通数组。
// 对于find()函数的底层实现，C++标准库中给出了参数代码：
// template <class InputIterator, class T>
// InputIterator find(InputIterator first, InputIterator last, const T& val) {
//   while (first != last) {
//     if (*first == val) return first;
//     ++first;
//   }
//   return last;
// }

// 和find()函数相同，find_if()函数也用于在指定区域内执行查找操作。不同的是，前者需要明确指定要查找的元素的值，而后者则允许自定义查找规则。
// 所谓自定义查找规则，实际上指的是有一个形参且返回值类型为bool的函数。值得一提的是，该函数可以是一个普通函数（又称为一元谓词函数），比如：
bool mycomp(int i) { return ((i % 2) == 1); }
// mycomp()就是一个一元谓词函数，其可用来判断一个整数是奇数还是偶数。
// 也可以是一个函数对象，比如：
class mycomp2 {  // 此函数对象的功能和mycomp()函数一样。
 public:
  bool operator()(const int& i) { return ((i % 2) == 1); }
};
// 确切地说，find_if()函数会根据指定的查找规则，在指定区域内查找第一个符合该函数要求（使函数返回true）的元素。
// find_if()函数的语法格式如下：
// InputIterator find_if (InputIterator first, InputIterator last, UnaryPredicate pred);
// 其中，first和last都为输入迭代器，其组合[first, last)用于指定要查找的区域；pred用于自定义查找规则。
// 由于first和last都为输入迭代器，意味着该函数适用于所有的序列式容器。甚至当采用适当的谓词函数时，该函数还适用于所有的关联式容器（包括哈希容器）。
// 同时，该函数会返回一个输入迭代器，当查找成功时，该迭代器指向的是第一个符合查找规则的元素；反之，如果find_if()函数查找失败，则该迭代器的指向和last迭代器相同。
// find_if()函数底层实现的参考代码:
// template <class InputIterator, class UnaryPredicate>
// InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate pred) {
//   while (first != last) {
//     if (pred(*first)) return first;
//     ++first;
//   }
//   return last;
// }

// find_if_not()函数和find_if()函数的功能恰好相反，find_if()函数用于查找符合谓词函数规则的第一个元素，而find_if_not()函数则用于查找第一个不符合谓词函数规则的元素。
// find_if_not()函数的语法规则如下所示：
// InputIterator find_if_not (InputIterator first, InputIterator last, UnaryPredicate pred);
// 其中，first和last都为输入迭代器，[first, last)用于指定查找范围；pred 用于自定义查找规则。
// 和find_if()函数一样，find_if_not()函数也适用于所有的容器，包括所有序列式容器和关联式容器。
// 同样，该函数也会返回一个输入迭代器，当find_if_not()函数查找成功时，该迭代器指向的是查找到的那个元素；反之，如果查找失败，该迭代器的指向和last迭代器相同。
// find_if_not()函数的底层实现和find_if()函数非常类似:
// template <class InputIterator, class UnaryPredicate>
// InputIterator find_if_not(InputIterator first, InputIterator last, UnaryPredicate pred) {
//   while (first != last) {
//     if (!pred(*first)) return first;
//     ++first;
//   }
//   return last;
// }

int main() {
  char stl[] = "http://c.biancheng.net/stl/";
  char* p = find(stl, stl + strlen(stl), 'c');
  if (p != stl + strlen(stl)) {
    cout << p << endl;  // c.biancheng.net/stl/
  }
  std::vector<int> myvector{10, 20, 30, 40, 50};
  std::vector<int>::iterator it;
  it = find(myvector.begin(), myvector.end(), 30);
  if (it != myvector.end())
    cout << "查找成功：" << *it << endl;  // 查找成功：30
  else
    cout << "查找失败" << endl;

  vector<int> myvector1{4, 2, 3, 1, 5};
  vector<int>::iterator it1 =
      find_if(myvector1.begin(), myvector1.end(), mycomp2());
  cout << "*it = " << *it1 << endl;  // *it = 3

  it1 = find_if_not(myvector1.begin(), myvector1.end(), mycomp);
  cout << "*it = " << *it1 << endl;  // *it = 4

  return 0;
}