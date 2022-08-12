#include <algorithm>
#include <iostream>
#include <vector>

// 考虑用排序的vector代替关联容器

// 许多STL程序员，当需要一个可提供快速查找功能的数据结构时，
// 刻会想到关联容器，没有问题，只要合适就行。
// 但它们并不总是合适的，如果查找速度真的很重要，
// 那么考虑非标准的哈希容器几乎总是值得的。
// 通过适当的哈希函数，几乎能提供常数时间的查找能力。
// 对于许多应用，哈希容器可能提供的常数时间查找能力优于set、
// multiset、map和multimap的确定的对数时间查找能力。
// 即使确定的对数时间查找能力正是想要的，标准关联容器可能也不是最好的选择。
// 与直觉相反，标准关联容器的效率比vector还低的情况并不少见。

// 标准关联容器通常被实现为平衡的二叉查找树。
// 二叉查找树对输入、删除和查找的混合操作做了优化，
// 总的来说，没办法预测出下一个操作是啥。
// 很多程序使用数据时并不这样混乱，一般为：
// 1.设置阶段：
//   创建数据结构，并插入大量数据。
//   这个阶段，几乎所有的操作都是插入和删除，很少或几乎没有查找操作。
// 2.查找阶段：
//   这个阶段几乎所有的操作都是查找操作，很少或几乎没有插入和删除操作。
// 3.重组阶段：
//   改变数据的内容，或许是删除所有数据，再插入新的数据。
//   行为上，与阶段1类似，这个阶段结束后，有回到阶段2。
// 对于这样使用数据来说，vector可能比关联容器提供了更好的性能，
// 无论是时间还是空间，但必须是排序的vector才可以。
// 因为只有对排序的容器才能够正确地使用查找算法binary_search、
// lower_bound和equal_range等。

// 为什么通过（排序的）vector执行的二分搜索，
// 比通过二叉查找树执行的二分搜索具有更好的性能呢？
// 1.大小的问题，关联容器存储一个对象所伴随的空间开销至少是3个指针；
//   vector中存储对象，不会有任何额外开销，
// 2.但vector末尾可能会有空闲的空间，不过平均到每个对象上，
//   这些开销通常是微不足道的，而且如果有必要，可以用swap技巧去除。
// 3.绝大多数STL使用了自定义的内存管理器，如果STL实现没有采用这样的措施，
//   那么这些节点会散布在全部地址空间，会导致更多的页面错误。
//   在排序的vector中存储数据可能比在标准关联容器中存储同样的数据，
//   要耗费更少的内存，而考虑到页面错误的因素，
//   通过二分搜索法来查找一个排序vector比查找一个关联容器要快些。
// 对于排序vector，最不利的地方在于必须保持有序。
// 插入时，新元素之后的所有元素都必须向后移动一个位置，删除也是需要移动。
// 当数据结构的使用方式是：
// 查找操作几乎从不跟插入和删除操作混在一起时，
// 再考虑使用排序的vector而不是关联容器才是合理的。
struct Widget {
  bool operator<(const Widget& rhs) const { return false; }
};

// 如果决定用一个vector来替换map或multimap，
// 那么存储在vector中的数据必须是pair<K,V>，
// 而不是像map或multimap中那样保存pair<const K,V>。
// 因为当对vector进行排序时，它的元素的值将通过赋值操作被移动，
// 这意味着pair的两个部分都必须是可以被赋值的。
// map或multimap总是保持自己的元素是排序的，但它在排序时，
// 只看元素的键的部分，当对vector做排序时，也必须这么做。
// 需要自己写一个自定义的比较函数，
// 因为pair的operator<对pair的两部分都要检查。
// 需要另一个比较函数来执行查找过程。
// 用来做排序的比较函数需要两个pair对象作为参数，
// 但是查找的时候只需要一个键值。
// 所以查找的比较函数必须带一个与键同类型的对象（将被查找的值），
// 和一个pair对象（存储在vector中的对象）作为参数，两个参数的类型不相同。
// 另外，不知道传进来的第一个参数是键还是pair，
// 所以实际上需要两个用于查找的比较函数：
// 一个假定键部分作为第一个参数传入，另一假定pair先传入。
typedef std::pair<std::string, int> Data;
class DataCompare {
 public:
  bool operator()(const Data& lhs, const Data& rhs) const {
    return keyLess(lhs.first, rhs.first);
  }
  bool operator()(const Data& lhs, const Data::first_type& k) const {
    return keyLess(lhs.first, k);
  }
  bool operator()(const Data::first_type& k, const Data& rhs) const {
    return keyLess(k, rhs.first);
  }

 private:
  bool keyLess(const Data::first_type& k1, const Data::first_type& k2) const {
    return k1 < k2;
  }
};

// 写好了DataCompare,则一切都进入了轨道。
// 相比使用map的设计，它们通常会运行的更快而且使用更少的内存。
// 前提是：只用使用数据结构的方式符合本条款的三阶段模式。
// 否则使用排序的vector而不是标准的关联容器几乎肯定是在浪费时间。

int main() {
  // 使用排序的vector的代码骨架：
  std::vector<Widget> vw;
  std::sort(vw.begin(), vw.end());
  Widget w;
  if (binary_search(vw.begin(), vw.end(), w)) {
  }
  std::vector<Widget>::iterator i = std::lower_bound(vw.begin(), vw.end(), w);
  if (i != vw.end() && !(w < *i)) {
  }
  std::pair<std::vector<Widget>::iterator, std::vector<Widget>::iterator>
      range = equal_range(vw.begin(), vw.end(), w);
  if (range.first != range.second) {
  }
  std::sort(vw.begin(), vw.end());  // 开始新的查找

  // 假定排序的vector在模仿map<string,int>：
  std::vector<Data> vd;
  std::sort(vd.begin(), vd.end());
  std::string s;
  if (binary_search(vd.begin(), vd.end(), s, DataCompare())) {
  }
  std::vector<Data>::iterator i1 =
      lower_bound(vd.begin(), vd.end(), s, DataCompare());
  if (i1 != vd.end() && !DataCompare()(s, *i1)) {
  }
  std::pair<std::vector<Data>::iterator, std::vector<Data>::iterator> range1 =
      equal_range(vd.begin(), vd.end(), s, DataCompare());
  if (range1.first != range1.second) {
  }
  sort(vd.begin(), vd.end(), DataCompare());

  return 0;
}