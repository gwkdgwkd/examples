#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// 避免产生“直写型”（write-only）的代码

// 当编写代码的时候，看似非常直接和简捷，因为它是由某些基本想法自然而形成的。
// 然而，阅读代码的人却很难将最终的语句还原成它所依据的思路，这就是“直写型的代码”。
// 很容易编写，但是难以阅读和理解。一段代码是否是“直写型”取决于读者的知识水平。

// 在软件工程领域有这样一条真理：代码被阅读的次数远远大于它被编写的次数。
// 一个等价的说法是：软件的维护过程通常比开发过程需要消耗更多的时间。
// 如果无法正确地阅读和理解软件的含义，则自然也谈不上对软件的维护；一个无法被维护的软件恐怕也就不具备任何价值了。

// 请使用STL，并且用好STL。还要有效地使用STL。但是，一定要避免“直写型”的代码。从长远来看，这样的代码绝对算不算有效。

int main() {
  vector<int> v;
  int x, y;
  // 删除所有其值小于x的元素，但是，在最后一个其值不小于y的元素之前的所有元素都应该保留下来。
  v.erase(
      remove_if(find_if(v.rbegin(), v.rend(), bind2nd(greater_equal<int>(), y))
                    .base(),
                v.end(), bind2nd(less<int>(), x)),
      v.end());
  // 任何一条包含了对10个不同函数的12次调用的语句都会超出绝大多数C++软件开发人员的接受范围。
  // 若要理解这条语句，必须有很强的STL背景才行。

  // 分解的做法：
  typedef vector<int>::iterator VecIntIter;
  VecIntIter rangeBegin =
      find_if(v.rbegin(), v.rend(), bind2nd(greater_equal<int>(), y)).base();
  v.erase(remove_if(rangeBegin, v.end(), bind2nd(less<int>(), x)), v.end());

  return 0;
}
