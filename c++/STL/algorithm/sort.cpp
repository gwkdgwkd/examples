#include <algorithm>
#include <iostream>
#include <list>

using namespace std;

// C++ STL标准库提供有很多实用的排序函数，通过调用它们，我们可以很轻松地实现对普通数组或者容器中指定范围内的元素进行排序。
// sort(first, last) 对容器或普通数组中[first, last)范围内的元素进行排序，默认进行升序排序。
// stable_sort(first, last) 和sort()函数功能相似，不同之处在于，对于[first, last)范围内值相同的元素，该函数不会改变它们的相对位置。
// partial_sort(first, middle, last) 从[first,last)范围内，筛选出muddle-first个最小的元素并排序存放在[first，middle)区间中。
// partial_sort_copy(first, last, result_first, result_last) 从[first, last)范围内筛选出result_last-result_first个元素排序
//                                                           并存储到[result_first, result_last)指定的范围中。
// is_sorted(first, last) 检测[first, last)范围内是否已经排好序，默认检测是否按升序排序。
// is_sorted_until(first, last) 和is_sorted()函数功能类似，唯一的区别在于，如果[first, last)范围的元素没有排好序，则该函数会返回一
//                              个指向首个不遵循排序规则的元素的迭代器。
// void nth_element(first, nth, last) 找到[first, last)范围内按照排序规则（默认按照升序排序）应该位于第nth个位置处的元素，并将其放置到
//                                    此位置。同时使该位置左侧的所有元素都比其存放的元素小，该位置右侧的所有元素都比其存放的元素大。

// sort()、stable_sort()、partial_sort()、partial_sort_copy()函数都位于<algorithm>头文件中。

// sort()函数是基于快速排序实现的。
// stable_sort()函数是基于归并排序实现的

// 效率：
// sort() 该函数实现排序的平均时间复杂度为N*log2N（其中N为指定区域[first, last)中last和first的距离）。
// stable_sort() 当可用空间足够的情况下，时间复杂度可达到O(N*log2(N))；反之，时间复杂度为O(N*log2(N)2)，其中N为指定区域[first, last)中last和first的距离。
// partial_sort() 函数实现排序的平均时间复杂度为N*log(M)，其中N指的是[first, last)范围的长度，M指的是[first, middle)范围的长度。
// partial_sort_copy() 函数实现排序的平均时间复杂度为N*log(min(N,M))，其中N指的是[first, last)范围的长度，M指的是[result_first, result_last)范围的长度。

// 一、sort排序函数
// C++ STL标准库中的sort()函数，本质就是一个模板函数。该函数专门用来对容器或普通数组中指定范围内的元素进行排序，排序规则默认以元素值的大小做升序排序，
// 除此之外我们也可以选择标准库提供的其它排序规则（比如std::greater<T>降序排序规则），甚至还可以自定义排序规则。
// 需要注意的是，sort()函数受到底层实现方式的限制，它仅适用于普通数组和部分类型的容器。换句话说，只有普通数组和具备以下条件的容器，才能使用sort()函数：
// 容器支持的迭代器类型必须为随机访问迭代器。这意味着，sort()只对array、vector、deque这3个容器提供支持。
// 如果对容器中指定区域的元素做默认升序排序，则元素类型必须支持<小于运算符；同样，如果选用标准库提供的其它排序规则，元素类型也必须支持该规则底层实现所用的比较运算符；
// sort()函数在实现排序时，需要交换容器中元素的存储位置。这种情况下，如果容器中存储的是自定义的类对象，则该类的内部必须提供移动构造函数和移动赋值运算符。
// 对于指定区域内值相等的元素，sort()函数无法保证它们的相对位置不发生改变。
// sort()函数有2种用法，其语法格式分别为：
// void sort(RandomAccessIterator first, RandomAccessIterator last);  // 对[first, last)区域内的元素做默认的升序排序
// void sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);  // 按照指定的comp排序规则，对[first, last)区域内的元素进行排序
// 其中，first和last都为随机访问迭代器，它们的组合[first, last)用来指定要排序的目标区域；
// 另外在第2种格式中，comp可以是C++STL标准库提供的排序规则（比如std::greater<T>），也可以是自定义的排序规则。
// 和为关联式容器设定排序规则不同，给sort()函数指定排序规则时，需要为其传入一个函数名（mycomp）或者函数对象（std::greater<int>()或者mycomp2()）。

// 二、stable_sort排序函数
// 当指定范围内包含多个相等的元素时，sort()排序函数无法保证不改变它们的相对位置。那么，如果既要完成排序又要保证相等元素的相对位置，该怎么办呢？可以使用stable_sort()函数。
// stable_sort()函数完全可以看作是sort()函数在功能方面的升级版。换句话说，stable_sort()和sort()具有相同的使用场景，就连语法格式也是相同的，只不过前者在功能上除了可以
// 实现排序，还可以保证不改变相等元素的相对位置。
// table_sort()函数的用法也有2种，其语法格式和sort()函数完全相同（仅函数名不同）：
// void stable_sort(RandomAccessIterator first, RandomAccessIterator last);  // 对[first, last)区域内的元素做默认的升序排序
// void stable_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp);   //按照指定的comp排序规则，对[first, last)区域内的元素进行排序
// 其中，first和last都为随机访问迭代器，它们的组合[first, last)用来指定要排序的目标区域；
// 另外在第2种格式中，comp可以是C++ STL标准库提供的排序规则（比如 std::greater<T>），也可以是自定义的排序规则。

// 有一个存有100万个元素的容器，但我们只想从中提取出值最小的10个元素，该如何实现呢？使用sort()或者stable_sort()排序函数，需要全部排序，效率非常低。
// 对于解决类似的问题，C++ STL标准库提供了更高效的解决方案，即使用partial_sort()或者partial_sort_copy()函数。
// 三、partial_sort排序函数
// partial_sort()函数，partial sort可直译为“部分排序”。partial_sort()函数的功能确是如此，即该函数可以从指定区域中提取出部分数据，并对它们进行排序。
// partial_sort()函数有2种用法，其语法格式分别为：
// 按照默认的升序排序规则，对 [first, last) 范围的数据进行筛选并排序
// void partial_sort (RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last);
// 按照comp排序规则，对[first, last)范围的数据进行筛选并排序
// void partial_sort (RandomAccessIterator first, RandomAccessIterator middle, RandomAccessIterator last, Compare comp);
// 其中，first、middle和last都是随机访问迭代器，comp参数用于自定义排序规则。
// partial_sort()函数会以交换元素存储位置的方式实现部分排序的。具体来说，partial_sort()会将[first, last)范围内最小（或最大）的middle-first个元
// 素移动到[first, middle)区域中，并对这部分元素做升序（或降序）排序。
// 需要注意的是，partial_sort()函数受到底层实现方式的限制，它仅适用于普通数组和部分类型的容器。换句话说，只有普通数组和具备以下条件的容器，才能使用partial_sort()函数：
//  容器支持的迭代器类型必须为随机访问迭代器。这意味着，partial_sort()函数只适用于array、vector、deque这3个容器。
//  当选用默认的升序排序规则时，容器中存储的元素类型必须支持<小于运算符；同样，如果选用标准库提供的其它排序规则，元素类型也必须支持该规则底层实现所用的比较运算符；
//  partial_sort()函数在实现过程中，需要交换某些元素的存储位置。因此，如果容器中存储的是自定义的类对象，则该类的内部必须提供移动构造函数和移动赋值运算符。

// 四、partial_sort_copy排序函数
// partial_sort_copy()功能和partial_sort()类似，唯一的区别是前者不会对原有数据做任何变动，而是先将选定的部分元素拷贝到另外指定的数组或容器中，然后再对这部分元素进行排序。
// partial_sort_copy()函数也有2种语法格式，分别为：
// RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last, RandomAccessIterator result_first,
//                                        RandomAccessIterator result_last);
// RandomAccessIterator partial_sort_copy(InputIterator first, InputIterator last, RandomAccessIterator result_first,
//                                        RandomAccessIterator result_last, Compare comp);
// 其中，first和last为输入迭代器；result_first和result_last为随机访问迭代器；comp用于自定义排序规则。
// partial_sort_copy()函数会将[first, last)范围内最小（或最大）的result_last-result_first个元素复制到[result_first, result_last)区域中，
// 并对该区域的元素做升序（或降序）排序。
// 值得一提的是，[first, last] 中的这2个迭代器类型仅限定为输入迭代器，这意味着相比partial_sort()函数，partial_sort_copy()函数放宽了对存储原有数据的容器类型的限制。
// 换句话说，partial_sort_copy()函数还支持对list容器或者forward_list容器中存储的元素进行“部分排序”，而partial_sort()函数不行。
// 但是，介于result_first和result_last仍为随机访问迭代器，因此[result_first, result_last)指定的区域仍仅限于普通数组和部分类型的容器，
// 这和partial_sort()函数对容器的要求是一样的。

// 五、nth_element排序函数
// 在有序序列中，可以称第n个元素为整个序列中“第n大”的元素。

bool mycomp(int i, int j) { return (i < j); }
class mycomp2 {
 public:
  bool operator()(int i, int j) { return (i < j); }
};
bool mycomp3(int i, int j) { return (i > j); }
class mycomp4 {
 public:
  bool operator()(int i, int j) { return (i > j); }
};

int main() {
  auto printVector = [](vector<int> &v) -> void {
    for (auto i = v.begin(); i != v.end(); ++i) {
      cout << *i << ' ';
    }
    cout << endl;
  };
  std::vector<int> myvector{32, 71, 12, 45, 26, 80, 53, 33};
  printVector(myvector);  // 32 71 12 45 26 80 53 33
  std::sort(myvector.begin(), myvector.begin() + 4);
  printVector(myvector);  // 12 32 45 71 26 80 53 33
  std::sort(myvector.begin(), myvector.begin() + 4, std::greater<int>());
  printVector(myvector);  // 71 45 32 12 26 80 53 33
  std::sort(myvector.begin(), myvector.end(), mycomp2());
  printVector(myvector);  // 12 26 32 33 45 53 71 80

  std::vector<int> my1{32, 71, 12, 45, 26, 80, 53, 33};
  printVector(my1);  // 32 71 12 45 26 80 53 33
  std::stable_sort(my1.begin(), my1.begin() + 4);
  printVector(my1);  // 12 32 45 71 26 80 53 33
  std::stable_sort(my1.begin(), my1.begin() + 4, std::greater<int>());
  printVector(my1);  // 71 45 32 12 26 80 53 33
  std::stable_sort(my1.begin(), my1.end(), mycomp);
  printVector(my1);  // 12 26 32 33 45 53 71 80

  std::vector<int> my2{3, 2, 5, 4, 1, 6, 9, 7};
  printVector(my2);  // 3 2 5 4 1 6 9 7
  std::partial_sort(my2.begin(), my2.begin() + 4, my2.end());
  printVector(my2);  // 1 2 3 4 5 6 9 7
  std::partial_sort(my2.begin(), my2.begin() + 4, my2.end(), mycomp4());
  printVector(my2);  // 9 7 6 5 1 2 3 4

  auto print = [](int a[]) -> void {
    for (int i = 0; i < 5; ++i) {
      cout << a[i] << ' ';
    }
    cout << endl;
  };
  int mys[5] = {0};
  std::list<int> ml{3, 2, 5, 4, 1, 6, 9, 7};
  print(mys);  // 0 0 0 0 0
  std::partial_sort_copy(ml.begin(), ml.end(), mys, mys + 5);
  print(mys);  // 1 2 3 4 5
  std::partial_sort_copy(ml.begin(), ml.end(), mys, mys + 5, mycomp4());
  print(mys);  // 9 7 6 5 4

  return 0;
}