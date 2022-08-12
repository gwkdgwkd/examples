#include <iostream>

// 理解相等（equality）和等价（equivalence）的区别

// find对相同的定义是相等，是以operator==为基础的。
// set::insert对相同的定义是等价，是以operator<为基础的。
// 在实际操作中，相等的概念是基于operator==的。
// 等价关系是以在已排序的区间中对象值的相对顺序为基础的。
// 如果按照关联容器的排列顺序，每个都不在另一个的前面，
// 那么称这两个对象按照c的排列顺序有等价的值。
// 一般情形下，一个关联容器的比较函数并不是operator<，
// 甚至也不是less，它是用户定义的判别式。
// 每个标准关联容器都通过key_comp成员函数使排序判别式可被外部使用，
// 所以下面表达式为true，这x和y有等价的值：
// ！c.key_comp()(x,y) && !c.key_comp()(y,x)
// struct CIStringCompare {
//  public:
//   // binary_function<string, string, bool>
//   bool operator()(const std::string &lhs, const std::string &rhs) const {
//     return ciStringCompare(lhs, rhs);
//   }
// };

// 标准关联容器总是保持排列顺序的，
// 所以每个容器必须有一个比较函数（默认是less）来决定保持怎样的顺序。
// 等价的定义正是通过该比较函数而确定的。
// 标准关联容器的使用者要为所使用的每个容器指定一个比较函数。
// 如果使用相等来决定两个对象是否有相同的值，
// 那么要需提供决定相等的函数（默认是equal_to)。
// 但equal_to从来没有被用作STL的默认比较函数。
// STL中需要判断相等时，一般的惯例是直接调用operator==，
// 比如非成员的find算法。

// 使用单一的比较函数，并把等价关系作为判定两个元素是否相同的依据，
// 使得标准关联容器避免了一大堆若使用两个比较函数将带来的问题。
// 一旦离开了排序的关联容器，情况就发生了变化。

int main() {
  // 不区分大小写的字符串集：
  // set<string, CIStringCompare> ciss;

  // 新元素被插入到集合中：
  // ciss.insert("Persephone");

  // 没有新元素被插入到集合中：
  // ciss.insert("persephone");

  // 使用set的find，查找会成功：
  // if(ciss.find("persephone") != ciss.end())

  // 使用非成员的find，查找将失败：
  // if(find(ciss.begin(),ciss.end(),"persephone") != ciss.end())
  // 这是因为persephone和Persephone等价，但并不相等

  return 0;
}