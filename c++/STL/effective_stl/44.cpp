#include <algorithm>
#include <iostream>
#include <map>
#include <set>

// 容器的成员函数优先于同名的算法

// 有些STL容器提供了一些与算法同名的成员函数。比如：
// 1.关联容器提供了count、find、lower_bound、upper_bound和equal_range；
// 2.list提供了remove、remove_if、unique、sort、merge和reverse。
// 大多数情况下，应该使用这些成员函数，而不是相应的STL算法。有两个理由：
// 第一，成员函数往往速度快；
// 第二，成员函数通常与容器（特别是关联容器）结合的更加紧密，这是算法所不能的。
//      这是因为算法和成员函数虽然有同样的名称，但是它们所做的事情往往不完全相同。

// find成员函数所要求的比较次数与关联容器的具体类型有关，大多数STL中关联容器的底层都会选用红黑树结构。
// 搜索一个包含1000000个元素值的集合所需的最大比较次数为38，但对于大多数搜索，比较次数不会超过22.
// 基于完全平衡树的实现永远不会超过21次比较，但在实践中，完全平衡树的总体性能略逊色于红黑树的性能，
// 这也是为什么大多数STL实现采用了红黑树而不是平衡树的原因。

// 效率并不是find成员函数和find算法之间的唯一差别。
// STL算法以相等性来判断两个对象是否具有相同的值，而关联容器则使用等价性来进行它们的“相同性”测试。
// 因此find算法查找的是在容器中是否存在与727等值的元素，而find成员函数则是搜索容器中是否有等价与727的元素。
// 由于使用的准则不同，有可能导致截然不同的搜索结果。
// 因此，在使用关联容器的时候，优先考虑成员函数形式的find、count以及lower_bound等，而不是算法。

// 对于map和multimap容器而言，成员函数与STL算法的行为差异更是至关重要，
// 因为包含的是pair，成员函数只检查每个pair对象的键部分，
// count成员函数只统计与特定的键相匹配的pair对象的个数，其值部分被忽略；
// find、lower_bound、upper_bound和equal_randge也有类似的表现。
// count算法会使用相等性测试具有相同键和值的pair对象的个数，find、lower_bound等算法也与此类似。

// 对于标准关联容器，选择成员函数而不选择对应的同名算法，这可以带来几方面的好处：
// 第一，可以获得对数时间的性能，而不是线性时间的性能。
// 第二，可以使用等价性来确定两个值是否“相同”，而等价性是关联容器的一个本质定义。
// 第三，在使用map和multimap的时候，将很自然地只考虑元素的键部分，而不是完整的key，value对。

// 对于list中那些具有同名的STL算法，性能几乎成了全部考虑的因素。
// remove、remove_if、unique、sort、merge和reverse这些算法无一例外地需要拷贝list容器中的对象，
// 而list容器量身定做的成员函数则无需任何对象拷贝，它们只是简单地维护好那些将list节点连接起来的指针。
// 这些算法的时间复杂度并没有改变，但多数情况下维护指针的开销比拷贝对象要低的多，所以list成员函数提供更好的性能。

// list成员函数的行为不同于其同名的算法的行为：
// 1.如果真的要从一个容器中删除对象的话，在调用了remove、remove_if或者unique算法后，必须紧接着再调用erase；
// 2.而list的remove、remove_if或者unique成员函数则实实在在地删除了元素，所以无需再调用erase了。

// 在sort算法与list的sort函数之间有一个很重要的区别是，前者根本不能被应用到list容器上，
// 这是因为，list的迭代器只是双向迭代器，而sort算法要求随机访问迭代器。
// 在merge算法和list::merge函数之间也存在行为上的隔阂：
// merge算法是不允许修改其源区间的，而list::merge则总是在修改它所操作的链表。

// 因此当需要在STL算法与容器的同名成员函数之间做出选择的时候，应该优先考虑成员函数。
// 几乎可以肯定地讲，成员函数的性能更为优越，而且它们更能够与容器的一贯行为保持一致。

int main() {
  // set<int>容器，包含了一百万个整数值。
  // 现在想知道727这个整数是否包含其中，如果在的话，第一次出现是在哪里。
  std::set<int> s;
  std::set<int>::iterator i1 = s.find(727);
  if (i1 != s.end()) {
    // 无论容器中是否存在727，set::find的比较次数不会超过40次，通常大约20次比较操作就可以了
  }
  std::set<int>::iterator i2 = find(s.begin(), s.end(), 727);
  if (i2 != s.end()) {
    // find算法以线性时间运行，如果容器中不存在727的话，必须执行1000000次比较操作。
    // 即使包含了727，仍需平均500000此比较才能找到
  }

  std::map<int, char> m{{1, 'a'}, {2, 'b'}};
  std::cout << m.count(1) << std::endl;  // 1
  std::pair<int, char> p{1, 'a'};
  // std::count(m.begin(), m.end(), p);  // 编译报错

  return 0;
}
