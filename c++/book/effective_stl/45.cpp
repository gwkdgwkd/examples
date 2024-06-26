#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <set>
#include <vector>

// 正确区分count、find、binary_search、lower_bound、upper_bound和equal_range

// 在选择具体的查找策略时，有迭代器指定的区间是否是排序的，这是一个至关重要的决定条件：
// 1.如果是排序的，那么通过binary_search、lower_bound、upper_bound和equal_range，
//   可以获得更快的对数时间查找速度；
// 2.如果未排序，那只能用count、count_if、find以及find_if，这些仅提供线性时间的效率。
// 无论是依赖默认的搜索判别式，还是自己指定的判别式，这都不会影响选择查找算法的决定。
class Widget {
 public:
  bool operator==(const Widget&) const {};
  bool operator<(const Widget&) const {};
};
Widget w;

// 对于标准序列容器可以使用本条款中给出的建议，用begin和end迭代器来指明区间。
// 但对于标准关联容器，用同名的成员函数替换下面的算法：
// count、find、lower_bound、upper_bound和equal_range，
// 只有binary_search是个例外，因为在关联容器中没有与之对应的成员函数。

// 使用算法                使用成员函数                      作用
// 未排序   排序           对set或map   对multiset或multimap
// find    binary_search count        find                特定值存在么？
// find    equal_range   find         find或lower_bound
// 特定值存在么？如果存在，那第一个在哪里？ find_if lower_bound   lower_bound
// lower_bound         第一个不超过特定值的对象在哪里？ find_if upper_bound
// upper_bound  upper_bound         第一个在某个特定值之后的对象在哪里？ count
// equal_range   count        count               具有特定值的对象有多少个？
// find    equla_range   equal_range  equal_range 具有特定值的对象都在哪里？

// 对于排序区间，equla_range出现次数超乎寻常的多。
// 由于在查找过程中使用等价性测试非常重要，所以它的出现频率就很高。
// 使用lower_bound和upper_bound，会太容易就回退到相等性测试了。
// 对于equal_range，仅仅支持等价性测试则是非常自然的事情。
// 第二行针对排序区间，之所以选择equal_range而不是find只有一个理由：
// equal_range按对数时间运行，而find按线性时间运行。
// 对multiset或multimap寻找一个具有特定值的对象时，
// find和lower_bound都能胜任，通常情况下会使用find来完成这项工作。
// 对于multi容器，如果有多个对象具有特定的值，
// 则find并不保证一定标识出第一个具有此值的元素，只是标识出其中一个。
// 如果想找到第一个具有特定值的对象，可以使用lower_bound，
// 然后必须手工执行等价性测试以确定找到了你想找的值。
// 通过使用equal_range可以避免手工的等价性测试，
// 但是调用equal_range的开销比调用lower_bound的更加昂贵。

namespace n1 {
void func() {
  std::list<Widget> lw;

  // 一种很常见的习惯用法，将count用作存在性测试，count返回零或者一个正整数。
  if (count(lw.begin(), lw.end(), w) != 0) {
    // w在lw中
  } else {
    // w不在lw中
  }
  // 依赖隐式转换的做法也很常见：
  if (count(lw.begin(), lw.end(), w)) {
    // w在lw中
  } else {
    // w不在lw中
  }

  // 从存在性测试的角度来看，count的习惯用法相对要容易编码一些。
  // 同时，在搜索成功的情况下，count的效率要差一些。
  // 因为find找到第一个匹配的结果后马上就返回了，而count必须到达区间的末尾，
  // 以便找到所有的匹配，对于大多数程序员，find的在效率上的优势足以弥补稍微复杂的用法。
  if (find(lw.begin(), lw.end(), w) != lw.end()) {
    // w在lw中
  } else {
    // w不在lw中
  }

  // 当不仅仅想知道是否存在，而且也想知道哪一个对象具有这样的值的时候，需要find：
  std::list<Widget>::iterator i = find(lw.begin(), lw.end(), w);
  if (i != lw.end()) {
    // 找到了，i指向第一个满足条件的对象
  } else {
    // 没有找到
  }
}
}  // namespace n1

namespace n2 {
void func() {
  // 要测试一个排序区间中是否存在某一个特定的值，可以使用binary_search。
  // 与标准C/C++函数库中的bsearch不同的是，binary_search仅仅返回一个bool值。
  std::vector<Widget> vw;
  sort(vw.begin(), vw.end());
  if (binary_search(vw.begin(), vw.end(), w)) {
    // w在vw中
  } else {
    // w不在vw中
  }

  // 如果在，在哪里？需要使用equal_range。

  // 先看下lower_bound：
  // 使用lower_bound查找某个特定值的时候，它会返回一个迭代器，
  // 迭代器要么指向该值的第一份拷贝（如果找到了的话），
  // 要么指向一个适合插入该值的位置（如果没有找到的话）。
  std::vector<Widget>::iterator i1 = lower_bound(vw.begin(), vw.end(), w);
  if (i1 != vw.end() && *i1 == w) {
    // 测试条件是相等性测试，但lower_bound是用等价性来搜索的。
    // 大多数情况下，等价性测试和相等性测试的结果是相同的，
    // 但是19条说明了相等性和等价性不相同的情形也是不难发现的，这时是错误的。
    // 确保i指向一个对象并且确保该对象有正确的值；
  } else {
    // 没有找到
  }
  // 上面代码在大多数情况下都能正常工作，但是它并不完全正确，所以正确的做法是：
  // 必须检查lower_bound返回的迭代器所指的对象是否等价于要查找的值。
  // 可以手工实现这一点，但有风险，必须保证使用与lower_bound相同的比较函数。
  // 一般来说，它可能是任意一个函数（或函数对象），如果给lower_bound传递了比较函数，
  // 那么必须确保在手工编写的等价性测试代码中也使用同样的比较函数。
  // 这意味着改变了传给lower_bound的比较函数，那么必须同时也要修改等价性检查的代码。
  // 使比较函数保持同步并不是什么尖端科技，但这是需要时刻记住的事情。

  // 有一种更容易的办法：equal_range，返回一对迭代器，
  // 第一个等于lower_bound返回的迭代器，
  // 第二个等于upper_bound返回的迭代器。
  // equal_range返回的一对迭代器标识了一个子区间，其中的值与查找的值等价。
  // 关于equal_range的返回值有两个需要注意的地方：
  // 1.首先，如果返回的两个迭代器相同，则说明查找所得到的对象区间为空，即没有找到这样的值；
  // 2.其次，equal_range返回的迭代器之间的距离与这个区间中的对象数目是相等的。
  //   对于排序区间，equal_range不仅完成了find的工作，同时也代替了count。
  typedef std::vector<Widget>::iterator VWIter;
  typedef std::pair<VWIter, VWIter> VWIterPair;
  VWIterPair p = equal_range(vw.begin(), vw.end(), w);
  // 使用了等价性，所以总是正确的：
  if (p.first != p.second) {
    // 如果equal_range返回非空区间找到了特定值，
    // p.first指向第一个与w等价的对象，p.second指向最后一个与w等价的对象的下一个位置
  } else {
    // 没有找到特定值，p.first和p.second都指向w的插入位置
  }
  std::cout << distance(p.first, p.second) << " elements in vw equivalent to w"
            << std::endl;
  // 0 elements in vw equivalent to w
}
}  // namespace n2

namespace n3 {
// 从未排序的区间到排序的区间的转变也带来了另一种变化：
// 前者利用相等性来决定两个值是否相同，而后者使用等价性作为判断的依据。
// count和find使用相等性进行搜索，
// 而binary_search、lower_bound、upper_bound和equal_range使用了等价性。
class Timestamp {};
bool operator<(const Timestamp& lhs, const Timestamp& rhs) {}

void func() {
  std::vector<Timestamp> vt;
  sort(vt.begin(), vt.end());
  Timestamp ageLimit;

  // 找到区间中的某一个位置, 从vt中删除所有在ageLimit之前的对象，
  // 这种情况下，并不想找到所有等价的对象，只是想指定一个位置，
  // 第一个不比ageLimit老的对象的位置。
  vt.erase(vt.begin(), lower_bound(vt.begin(), vt.end(), ageLimit));
  // 删除那些至少与ageLimit一样老的对象：
  vt.erase(vt.begin(), upper_bound(vt.begin(), vt.end(), ageLimit));
}
}  // namespace n3

namespace n4 {
class Person {
 public:
  const std::string& name() const {}
};
struct PersonNameLess : public std::binary_function<Person, Person, bool> {
  bool operator()(const Person& lhs, const Person& rhs) const {
    return lhs.name() < rhs.name();
  }
};
void func() {
  // 在一个排序区间插入一些对象，并且希望等价的对象仍然保持它们在插入时的顺序：
  std::list<Person> lp;
  lp.sort(PersonNameLess());
  Person newPerson;
  lp.insert(upper_bound(lp.begin(), lp.end(), newPerson, PersonNameLess()),
            newPerson);
  // 只指向了对数次的比较操作，但查找过程仍需要线性时间
}
}  // namespace n4

namespace n5 {
void func() {
  // 在set或者map中测试一个值是否存在，可以按照习惯用法使用count：
  std::set<Widget> s;
  if (s.count(w)) {
    // 存在与w等价的值
  } else {
    // 不存在这样的值
  }
  // 如果要在multiset和multimap中测试一个值是否存在，则一般情况下用find比用count好，
  // 因为find只要找到第一个期望的值就返回了，而count在最坏的情形下必须检查每个对象。
  // 对于set和map而言，这不是问题，因为set不允许有重复的值，而map不允许有重复的键。
  // 要在关联容器中统计个数，使用count是非常安全的，特别是，它比先调用equal_range，
  // 再在结果迭代器上调用distance的做法要好得多：
  // 1.首先，用法非常清晰，count的含义就是计数；
  // 2.其次，count简单，没有必要先创建一对迭代器，再将两个迭代器传给distance；
  // 3.第三，速度更快一些。
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}

