#include <algorithm>
#include <iostream>
#include <vector>

// lower_bound
// upper_bound
// equal_range
// binary_search

// find()、find_if()、search()这些函数的底层实现，
// 都采用的是顺序查找（逐个遍历）的方式，在某些场景中的执行效率并不高。
// 例如，当指定区域内的数据处于有序状态时，更推荐使用二分查找的方法。
// C++ STL标准库中还提供有lower_bound()、upper_bound()、
// equal_range()以及binary_search()这4个查找函数，
// 它们的底层实现采用的都是二分查找的方式，它们定义在<algorithm>中。

bool mycomp1(int i, int j) { return i < j; }
class mycomp2 {
 public:
  bool operator()(const int& i, const int& j) { return i < j; }
};

int a[7] = {1, 2, 3, 3, 3, 4, 5};
std::vector<int> v{1, 2, 3, 3, 3, 4, 5};

namespace n1 {
// lower_bound()函数用于在指定区域内查找不小于目标值的第一个元素。
// 也就是说，使用该函数在指定范围内查找某个目标值时，
// 最终查找到的不一定是和目标值相等的元素，还可能是比目标值大的元素。
// 其语法格式有2种，分别为：
// 在[first,last)区域内查找不小于val的元素：
// ForwardIterator lower_bound(ForwardIterator first,
//                             ForwardIterator last, const T& val);
// 在[first,last)区域内查找第一个不符合comp规则的元素：
// ForwardIterator lower_bound(ForwardIterator first,
//                             ForwardIterator last,
//                             const T& val, Compare comp);
// 其中，first和last都为正向迭代器，[first,last)用于指定函数的作用范围；
// val用于指定目标元素；
// comp用于自定义比较规则，此参数可以接收包含2个形参（第二个形参值始终为val），
// 且返回值为bool类型的函数，可以是普通函数，也可以是函数对象。

// 实际上，第一种语法格式也设定有比较规则，只不过此规则无法改变，
// 使用<比较[first,last)区域内元素和val的大小，直至找到一个不小于val的元素。
// 这也意味着，如果使用第一种语法格式，则[first,last)范围的元素必须支持<。
// 此外，该函数还会返回一个正向迭代器，当查找成功时，迭代器指向找到的元素；
// 反之，如果查找失败，迭代器的指向和last迭代器相同。
// 再次强调，该函数仅适用于已排好序的序列。

// lower_bound()函数底层实现的参考代码：
// template <class ForwardIterator, class T>
// ForwardIterator lower_bound(ForwardIterator first,
//                             ForwardIterator last, const T& val) {
//   ForwardIterator it;
//   iterator_traits<ForwardIterator>::difference_type count, step;
//   count = distance(first, last);
//   while (count > 0) {
//     it = first;
//     step = count / 2;
//     advance(it, step);
//     if (*it < val) {  // 或者if(comp(*it,val))，对应第2种语法格式
//       first = ++it;
//       count -= step + 1;
//     } else
//       count = step;
//   }
//   return first;
// }

void func() {
  int* p = std::lower_bound(a, a + 7, 3);
  std::cout << "a[" << p - a << "]=" << *p << std::endl;  // a[2]=3

  p = std::lower_bound(a, a + 7, 4);
  std::cout << "a[" << p - a << "]=" << *p << std::endl;  // a[5]=4

  std::vector<int>::iterator it =
      std::lower_bound(v.begin(), v.end(), 3, mycomp1);
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[2]=3

  it = std::lower_bound(v.begin(), v.end(), 4, mycomp2());
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[5]=4
}
}  // namespace n1

namespace n2 {
// upper_bound用于在指定范围内查找大于目标值的第一个元素。
// 该函数的语法格式有2种，分别是：
// 查找[first,last)区域中第一个大于val的元素：
// ForwardIterator upper_bound(ForwardIterator first,
//                             ForwardIterator last, const T& val);
// 查找[first,last)区域中第一个不符合comp规则的元素：
// ForwardIterator upper_bound(ForwardIterator first,
//                             ForwardIterator last,
//                             const T& val, Compare comp);
// 其中，first和last都为正向迭代器，[first,last)用于指定该函数的作用范围；
// val用于执行目标值；
// comp作用自定义查找规则，此参数可接收一个包含2个形参（第一个形参值始终为val），
// 且返回值为bool类型的函数，可以是普通函数，也可以是函数对象。
// 实际上，第一种语法格式也设定有比较规则，
// 即使用<小于号比较[first,last)区域内某些元素和val的大小，
// 直至找到一个大于val的元素，只不过此规则无法改变。
// 这也意味着，如果使用第一种语法格式，则[first,last)范围的元素必须支持<。
// 同时，该函数会返回一个正向迭代器，当查找成功时，迭代器指向找到的元素；
// 反之，如果查找失败，迭代器的指向和last迭代器相同。
// 由于upper_bound()采用的是二分查找法，因此该函数仅适用于已排好序的序列。
// 这里所说的已排好序，并不要求数据完全按照某个排序规则进行升序或降序排序，
// 而仅仅要求[first,last)区域内所有令element<val成立的元素，
// 都位于不成立元素的前面，其中element为指定范围内的元素。

// C++ STL标准库给出了upper_bound()函数底层实现：
// template <class ForwardIterator, class T>
// ForwardIterator upper_bound(ForwardIterator first,
//                             ForwardIterator last, const T& val) {
//   ForwardIterator it;
//   iterator_traits<ForwardIterator>::difference_type count, step;
//   count = std::distance(first, last);
//   while (count > 0) {
//     it = first;
//     step = count / 2;
//     std::advance(it, step);
//     if (!(val < *it)) { // 或者 if(!comp(val,*it)), 对应第二种语法格式
//       first = ++it;
//       count -= step + 1;
//     } else
//       count = step;
//   }
//   return first;
// }
void func() {
  int* p = std::upper_bound(a, a + 7, 3);
  std::cout << "a[" << p - a << "]=" << *p << std::endl;  // a[5]=4

  p = std::upper_bound(a, a + 7, 4);
  std::cout << "a[" << p - a << "]=" << *p << std::endl;  // a[6]=5

  std::vector<int>::iterator it =
      std::upper_bound(v.begin(), v.end(), 3, mycomp1);
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[5]=4

  it = std::upper_bound(v.begin(), v.end(), 4, mycomp2());
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[6]=5
}
}  // namespace n2

namespace n3 {
// equel_range用于在指定范围内查找等于目标值的所有元素。
// 当指定范围内的数据支持用<小于运算符直接做比较时，
// 可以使用如下格式的equel_range()函数：
// pair<ForwardIterator,ForwardIterator> equal_range(ForwardIterator first,
//                                                   ForwardIterator last,
//                                                   const T& val);
// 如果指定范围内的数据为自定义的类型（用结构体或类），就需要自定义比较规则，
// 这种情况下可以使用如下格式的equel_range()函数：
// pair<ForwardIterator,ForwardIterator> equal_range(ForwardIterator first,
//                                                   ForwardIterator last,
//                                                   const T& val, Compare
//                                                   comp);
// 以上2种格式中：
// first和last都为正向迭代器，[first, last)用于指定该函数的作用范围；
// val用于指定目标值；
// comp用于指定比较规则，此参数可接收一个包含2个形参（第二个形参值始终为val），
// 且返回值为bool类型的函数，可以是普通函数，也可以是函数对象。
// 该函数会返回一个pair类型值，其包含2个正向迭代器。当查找成功时：
// 第1个迭代器指向的是[first,last)区域中第一个等于val的元素；
// 第2个迭代器指向的是[first,last)区域中第一个大于val的元素。
// 反之如果查找失败，则这2个迭代器要么都指向大于val的第一个元素（如果有），
// 要么都和last迭代器指向相同。
// 由于equel_range()采用的是二分查找法，因此该函数仅适用于已排好序的序列。
// 所谓已排好序，并不是要求[first,last)区域内的数据，
// 严格按照某个排序规则进行升序或降序排序，只要满足：
// 所有令element<val成立的元素都位于不成立元素的前面即可，
// 其中element为指定范围内的元素。
// 实际上，equel_range()函数的功能完全可以看做是：
// lower_bound()和upper_bound()函数的合体。

// C++ STL标准库给出了equel_range()函数底层实现的参考代码：
// template <class ForwardIterator, class T>
// pair<ForwardIterator,ForwardIterator> equal_range (ForwardIterator first,
//                                                    ForwardIterator last,
//                                                    const T& val) {
//     ForwardIterator it = std::lower_bound (first,last,val);
//     return std::make_pair ( it, std::upper_bound(it,last,val) );
// }
// template<class ForwardIterator, class T, class Compare>
// std::pair<ForwardIt,ForwardIt> equal_range(ForwardIterator first,
//                                            ForwardIterator last,
//                                            const T& val, Compare comp) {
//     ForwardIterator it = std::lower_bound (first,last,val,comp);
//     return std::make_pair ( it, std::upper_bound(it,last,val,comp) );
// }

void func() {
  std::pair<int*, int*> range1 = std::equal_range(a, a + 7, 3);
  for (int* p = range1.first; p < range1.second; ++p) {
    std::cout << *p << " ";
  }
  std::cout << std::endl;  // 3 3 3

  range1 = std::equal_range(a, a + 7, 4);
  for (int* p = range1.first; p < range1.second; ++p) {
    std::cout << *p << " ";
  }
  std::cout << std::endl;  // 4

  std::pair<std::vector<int>::iterator, std::vector<int>::iterator> range2;
  range2 = std::equal_range(v.begin(), v.end(), 3, mycomp1);
  for (auto it = range2.first; it != range2.second; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 3 3 3

  range2 = std::equal_range(v.begin(), v.end(), 4, mycomp2());
  for (auto it = range2.first; it != range2.second; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 4
}
}  // namespace n3

namespace n4 {
// binary_search用于查找指定区域内是否包含某个目标元素。
// 该函数有2种语法格式，分别为：
// 查找[first,last)区域内是否包含val：
// bool binary_search(ForwardIterator first,
//                    ForwardIterator last, const T& val);
// 根据comp指定的规则，查找[first,last)区域内是否包含val
// bool binary_search(ForwardIterator first,
//                    ForwardIterator last,
//                    const T& val, Compare comp);
// 其中，first和last都为正向迭代器，[first,last)用于指定该函数的作用范围；
// val用于指定要查找的目标值；
// comp用于自定义查找规则，此参数可接收一个包含2个形参（第一个形参值为val），
// 且返回值为bool类型的函数，可以是普通函数，也可以是函数对象。
// 同时，如果binary_search()函数在[first,last)区域内成功找到和val相等的元素，
// 则返回true；反之则返回false。
// 由于binary_search()采用的是二分查找法，因此该函数仅适用于已排好序的序列。
// 所谓已排好序，并不是要求[first,last)区域内的数据，
// 严格按照某个排序规则进行升序或降序排序，只要满足：
// 所有令element<val成立的元素都位于不成立元素的前面即可，
// 其中element为指定范围内的元素。

// C++ STL标准库给出了binary_search()函数底层实现的参考代码：
// template <class ForwardIterator, class T>
// bool binary_search (ForwardIterator first,
//                     ForwardIterator last, const T& val) {
//     first = std::lower_bound(first,last,val);
//     return (first!=last && !(val<*first));
// }
// template<class ForwardIt, class T, class Compare>
// bool binary_search(ForwardIt first, ForwardIt last,
//                    const T& val, Compare comp) {
//     first = std::lower_bound(first, last, val, comp);
//     return (!(first == last) && !(comp(val, *first)));
// }

void func() {
  std::cout.setf(std::ios::boolalpha);

  bool haselem = std::binary_search(a, a + 7, 4);
  std::cout << haselem << std::endl;  // true

  haselem = std::binary_search(a, a + 7, 8);
  std::cout << haselem << std::endl;  // false

  haselem = std::binary_search(v.begin(), v.end(), 3, mycomp1);
  std::cout << haselem << std::endl;  // true

  haselem = std::binary_search(v.begin(), v.end(), 8, mycomp2());
  std::cout << haselem << std::endl;  // false
}
}  // namespace n4

namespace n5 {
bool mycomp(int i, int j) { return i > j; }  // 降序
void func() {
  std::vector<int> v{5, 4, 3, 3, 3, 2, 1};

  std::vector<int>::iterator it =
      std::lower_bound(v.begin(), v.end(), 3, mycomp);
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[2]=3
  it = std::upper_bound(v.begin(), v.end(), 3, mycomp);
  std::cout << "it[" << std::distance(v.begin(), it) << "]=" << *it
            << std::endl;  // it[5]=2

  std::pair<std::vector<int>::iterator, std::vector<int>::iterator> range;
  range = std::equal_range(v.begin(), v.end(), 3, mycomp);
  for (auto it = range.first; it != range.second; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;  // 3 3 3
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
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
    case 4:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}