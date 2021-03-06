#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

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

// C++ STL标准库共提供了4种排序函数
// sort() 对指定范围内所有的数据进行排序，排序后各个元素的相对位置很可能发生改变。
// stable_sort() 对指定范围内所有的数据进行排序，并确保排序后各个元素的相对位置不发生改变。
// partial_sort() 对指定范围内最大或最小的n个元素进行排序。
// nth_element() 调整指定范围内元素的存储位置，实现位于位置n的元素正好是全排序情况下的第n个元素，并且按照全排序规则排在位置n之前的元素都在该位置之前，
//               按照全排序规则排在位置n之后的元素都在该位置之后。
// 值以上4种排序函数在使用时，都要求传入随机访问迭代器，因此这些函数都只适用于array、vector、deque以及普通数组。
// 当操作对象为list或者forward_list序列式容器时，其容器模板类中都提供有sort()排序方法，借助此方法即可实现对容器内部元素进行排序。
// 其次，对关联式容器（包括哈希容器）进行排序是没有实际意义的，因为这类容器会根据既定的比较函数（和哈希函数）维护内部元素的存储位置。
// 那么，当需要对普通数组或者array、vector或者deque容器中的元素进行排序时，怎样选择最合适（效率最高）的排序函数呢？这里为大家总结了以下几点：
//  如果需要对所有元素进行排序，则选择sort()或者stable_sort()函数；
//  如果需要保持排序后各元素的相对位置不发生改变，就只能选择stable_sort()函数，而另外3个排序函数都无法保证这一点；
//  如果需要对最大（或最小）的n个元素进行排序，则优先选择partial_sort()函数；
//  如果只需要找到最大或最小的n个元素，但不要求对这n个元素进行排序，则优先选择nth_element()函数。
// 除此之外，很多读者都关心这些排序函数的性能。总的来说，函数功能越复杂，做的工作越多，它的性能就越低（主要体现在时间复杂度上）。
// 对于以上4种排序函数，综合考虑它们的时间和空间效率，其性能之间的比较如下所示：
// nth_element() > partial_sort() > sort() > stable_sort()  // 从左到右，性能由高到低
// 建议大家，在实际选择排序函数时，应更多从所需要完成的功能这一角度去考虑，而不是一味地追求函数的性能。
// 换句话说，如果你选择的算法更有利于实现所需要的功能，不仅会使整个代码的逻辑更加清晰，还会达到事半功倍的效果。

// sort()、stable_sort()、partial_sort()、partial_sort_copy()、nth_element()、is_sorted()和is_sorted_until()函数都位于<algorithm>头文件中。

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
// 简单的理解nth_element()函数的功能，当采用默认的升序排序规则（std::less<T>）时，该函数可以从某个序列中找到第n小的元素K，并将K移动到序列中第n的位置处。不仅如此，
// 整个序列经过nth_element()函数处理后，所有位于K之前的元素都比K小，所有位于K之后的元素都比K大。
// 也可以将nth_element()函数的排序规则自定义为降序排序，此时该函数会找到第n大的元素K并将其移动到第n的位置处，同时所有位于K之前的元素都比K大，所有位于K之后的元素都比K小。
// nth_element()函数有以下2种语法格式：
// 排序规则采用默认的升序排序
// void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
//                  RandomAccessIterator last);
// 排序规则为自定义的 comp 排序规则
// void nth_element(RandomAccessIterator first, RandomAccessIterator nth,
//                  RandomAccessIterator last, Compare comp);
// 其中，各个参数的含义如下：
//  first和last：都是随机访问迭代器，[first, last)用于指定该函数的作用范围（即要处理哪些数据）；
//  nth：也是随机访问迭代器，其功能是令函数查找“第nth大”的元素，并将其移动到nth指向的位置；
//  comp：用于自定义排序规则。
// 注意，鉴于nth_element()函数中各个参数的类型，其只能对普通数组或者部分容器进行排序。换句话说，只有普通数组和符合以下全部条件的容器，才能使用使用nth_element()函数：
//  容器支持的迭代器类型必须为随机访问迭代器。这意味着，nth_element()函数只适用于array、vector、deque这3个容器。
//  当选用默认的升序排序规则时，容器中存储的元素类型必须支持<小于运算符；同样，如果选用标准库提供的其它排序规则，元素类型也必须支持该规则底层实现所用的比较运算符；
//  nth_element()函数在实现过程中，需要交换某些元素的存储位置。因此，如果容器中存储的是自定义的类对象，则该类的内部必须提供移动构造函数和移动赋值运算符。

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

// 排序操作是比较耗费时间的，尤其当数据量很大的时候。因此在设计程序时，我们应该有意识的去避免执行一些不必要的排序操作。
// 怎样才能判断一个序列是否为有序序列呢？很简单，使用is_sorted()函数即可，此函数专门用于判断某个序列是否为有序序列。
// is_sorted()函数有2种语法格式，分别是：
// 判断[first, last)区域内的数据是否符合std::less<T>排序规则，即是否为升序序列
// bool is_sorted (ForwardIterator first, ForwardIterator last);
// 判断[first, last)区域内的数据是否符合comp排序规则
// bool is_sorted (ForwardIterator first, ForwardIterator last, Compare comp);
// 其中，first和last都为正向迭代器（这意味着该函数适用于大部分容器），[first, last)用于指定要检测的序列；comp用于指定自定义的排序规则。
// 注意，如果使用默认的升序排序规则，则[first, last)指定区域内的元素必须支持使用<小于运算符做比较；
// 同样，如果指定排序规则为comp，也要保证[first, last)区域内的元素支持该规则内部使用的比较运算符。
// 另外，该函数会返回一个bool类型值，即如果[first, last)范围内的序列符合我们指定的排序规则，则返回true；
// 反之，函数返回false。值得一提得是，如果[first, last)指定范围内只有1个元素，则该函数始终返回true。

// 和is_sorted()函数相比，is_sorted_until()函数不仅能检测出某个序列是否有序，还会返回一个正向迭代器，该迭代器指向的是当前序列中第一个破坏有序状态的元素。
// is_sorted_until()函数有以下2种语法格式：
// ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last);  // 排序规则为默认的升序排序
// ForwardIterator is_sorted_until(ForwardIterator first, ForwardIterator last, Compare comp);  // 排序规则是自定义的comp规则
// 其中，first和last都为正向迭代器（这意味着该函数适用于大部分容器），[first, last)用于指定要检测的序列；comp用于指定自定义的排序规则。
// 注意，如果使用默认的升序排序规则，则[first, last)指定区域内的元素必须支持使用<小于运算符做比较；
// 同样，如果指定排序规则为comp，也要保证[first, last)区域内的元素支持该规则内部使用的比较运算符。
// 该函数会返回一个正向迭代器。对于第一种语法格式来说，该函数返回的是指向序列中第一个破坏升序规则的元素；
// 对于第二种语法格式来说，该函数返回的是指向序列中第一个破坏comp排序规则的元素。
// 注意，如果[first, last)指定的序列完全满足默认排序规则或者comp排序规则的要求，则该函数将返回一个和last迭代器指向相同的正向迭代器。

// 作为一门面向对象的编程语言，使用C++编写程序有一个缺点，即随着代码面向对象程度的提高，其执行效率反而会降低。
// 事实上，对于STL标准库中的每个算法，只要用户需要自定义规则，该算法都会提供有带comp参数的语法格式。
// comp参数用于接收用户自定义的函数，其定义的方式有2种，既可以是普通函数，也可以是函数对象。
inline bool mycomp5(int i, int j) { return (i < j); }
class mycomp6 {
 public:
  bool operator()(int i, int j) { return (i < j); }
};
// 为了提高执行效率，其函数都定义为内联函数（在类内部定义的函数本身就是内联函数）。
// 函数对象可以理解为伪装成函数的对象，根据以往的认知，函数对象的执行效率应该不如普通函数。但事实恰恰相反，即便将普通函数定义为更高效的内联函数，其执行效率也无法和函数对象相比。
// 通过在4个不同的STL平台上，对包含100万个double类型数据的vector容器进行排序，最差情况下使用函数对象的执行效率要比普通内联函数高50%，最好情况下则高160%。
// 那么，是什么原因导致了它们执行效率上的差异呢？以mycomp6()函数对象为例，其mycomp6::operator()也是一个内联函数，编译器在对sort()函数进行实例化时会将该函数直接展开，这也
// 就意味着，展开后的sort()函数内部不包含任何函数调用。
// 而如果使用mycomp5作为参数来调用sort()函数，情形则大不相同。要知道，C++并不能真正地将一个函数作为参数传递给另一个函数，换句话说，如果我们试图将一个函数作为参数进行传递，编
// 译器会隐式地将它转换成一个指向该函数的指针，并将该指针传递过去。
// 当sort()函数被实例化时，编译器生成的函数声明如下所示：
// std::sort(vector<int>::iterator first, vector<int>::iterator last, bool (*comp)(int, int));
// 可以看到，参数comp只是一个指向函数的指针，所以sort()函数内部每次调用comp时，编译器都会通过指针产生一个间接的函数调用。
// 也正是基于这个原因，C++ sort()函数要比C语言qsort()函数的执行效率更高。可能会问，程序中comp()函数也是内联函数，为什么C++不像函数对象那样去处理呢？具体原因我们无从得知，事
// 实上也没必要关心，也许是编译器开发者觉得这种优化不值得去做。
// 除了效率上的优势之外，相比普通函数，以函数对象的方式自定义规则还有很多隐藏的优势。例如在某些特殊情况下，以普通函数的形式编写的代码看似非常合理，但就是无法通过编译，这也许是由
// 于STL标准库的原因，也许是编译器缺陷所至，甚至两者都有可能。而使用函数对象的方式，则可以有效避开这些“坑”，而且还大大提升的代码的执行效率。
// 总之，以函数对象的方式为STL算法自定义规则，具有效率在内的诸多优势。当调用带有comp参数的STL算法时，除非调用STL标准库自带的比较函数，否则应优先以函数对象的方式自定义规则。

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

  std::vector<int> mv{3, 1, 2, 5, 4};
  printVector(mv);  // 3 1 2 5 4
  std::nth_element(mv.begin(), mv.begin() + 2, mv.end());
  printVector(mv);  // 2 1 3 4 5
  std::nth_element(mv.begin(), mv.begin() + 3, mv.end(), mycomp3);
  printVector(mv);  // 3 5 4 2 1

  vector<int> myvector3{3, 1, 2, 4};
  if (!is_sorted(myvector.begin(), myvector.end(), mycomp4())) {
    sort(myvector3.begin(), myvector3.end(), mycomp4());
    printVector(myvector3);  // 4 3 2 1
  }
  vector<int> myvector4{1, 2, 3, 4};
  // if (!is_sorted(myvector4.begin(), myvector4.end(), mycomp4())) {
  if (!is_sorted(myvector4.begin(), myvector4.end())) {
    sort(myvector4.begin(), myvector4.end());
    printVector(myvector4);
  } else {
    cout << "sort" << endl;  // sort
  }

  vector<int> m4{1, 2, 3, 4};
  if (is_sorted_until(m4.begin(), m4.end(), mycomp4()) != m4.end()) {
    sort(m4.begin(), m4.end(), mycomp4());
    printVector(m4);  // 4 3 2 1
  }
  list<int> mylist{1, 2, 3, 4};
  if (is_sorted_until(mylist.begin(), mylist.end()) == mylist.end()) {
    cout << "sort" << endl;  // sort
  }
  vector<int> m5{2, 3, 4, 1};
  cout << *(is_sorted_until(m5.begin(), m5.end())) << endl;  // 1

  return 0;
}