#include <deque>
#include <iostream>
#include <iterator>

// iterator优先于const_iterator、reverse_iterator以及const_reverse_iterator

// STL中的所有标准容器都提供了4中迭代器。对于容器类container<T>而言：
// 1.iterator和reverse_iterator类型的功效相当与T*;
// 2.const_iterator和const_reverse_iterator相当于const T*，或T cosnt*;
// iterator和const_iterator递增从容器的头部一直遍历到尾部。
// reverse_iterator和const_reverse_iterator从尾部反向变量到容器头部。

// vector<T>容器中insert和erase的原型：
//  iterator insert(iterator position, const T& x);
//  iterator erase(iterator position);
//  iterator erase(iterator rangeBegin, iterator rangeEnd);
// 每个标准容器都提供了类似的函数，只不过对于不同的容器类型，返回值有所不同。
// 但这些函数仅接受iterator类型的参数，
// 而不是const_iterator、reverse_iterator以及const_reverse_iterator，
// 总是iterator，iterator与其他的迭代器有所不同。

// 迭代器的转换关系：
// 1.隐士转换
//   iterator -> cosnt_iterator
//   iterator -> reverse_iterator
//   reverse_iterator -> const_reverse_iterator
// 2.通过base成员函数
//   reverse_iterator -> iterator
//   const_reverse_iterator -> cosnt_iterator
// 无法从cosnt_iterator转换到iterator，也无法从const_reverse_iterator得到reverse_iterator。
// 意味着如果得到一个const迭代器，很难将这些迭代器与容器的某些成员函数一起使用。
// 常量迭代器可以用于许多算法，这些算法并不关系面对的是什么类型的迭代器，值关系这些迭代器属于何种类别。
// 容量类的很多成员函数也都可以接受常量迭代器，只有insert和erase的某些形式显得有些吹毛求疵。

// 通常情况下，是可以把cosnt_iterator或const_reverse_iterator转换为一个iterator的。
// 但并不总是可能的，甚至即使可行，其做法也不会非常显然，同时可能还会非常低效。

// 尽可能使用iterator，而避免使用const或者reverse型的迭代器：
//  有些版本的insert和erase函数要求使用iterator。
//  要想隐式的将一个const_iterator转换成iterator是不可能的。
//  从reverse_iterator转换而来的iterator在使用之前可能需要相应的调整。

// iterator与reverse_iterator之间选择需要根据需求的变量方向。
// iterator与const_iterator之间的选择，有足够的理由选择iterator，即便const_iterator也行，
// 即便并不需要使用迭代器做参数来调用容器的任何成员函数。
// 其中一个最赋值的理由涉及了iterator与const_iterator之间的比较：
typedef std::deque<int> IntDeuqe;
typedef IntDeuqe::iterator Iter;
typedef IntDeuqe::const_iterator ConstIter;

int main() {
  IntDeuqe iq;
  Iter i = iq.begin();
  ConstIter c = iq.begin();
  std::cout << std::boolalpha << (i == c) << std::endl;  // true
  std::cout << std::boolalpha << (c == i) << std::endl;  // true

  // iterator在比较之前应该被隐式转换成了const_iterator，真正比较的是两个const_iterator。
  // 对于涉及良好的STL实现而言，情况确实如此。但对于其他一些实现，这段代码甚至无法通过编译。
  // 原因在于，这些STL实现将const_iterator的等于操作符作为一个成员函数而不是一个非成员函数。
  // 解决办法：只要交换两个iterator的位置，就万事大吉了。也是相等?

  // 不仅在进行比较的时候会发生这样的问题，
  // 只要在同一个表达式中混用iterator和const_iterator，这样的问题就会出现：
  if (i - c >= 3) {  // 但这次无法交换i和c的位置了
  }
  if (c + 3 <= i) {
    // 上面的if语句不能编译的时候，这是解决办法。这样的转换并不总是正确的，
    // c+3也许不是一个有效的迭代器，可能会超出容器的有效范围，而变化前的表达式则不存在这样的问题。
  }
  // 避免这种问题的最简单办法是减少混用不同类型的迭代器，尽量使用iterator来代替const_iterator。
  // 从const正确性的角度来看，仅仅为了避免一些可能存在的STL实现缺陷而放弃cosnt_iterator显得有欠公允。
  // 但考虑到在容器类的某些成员函数中指定使用iterator的现状，
  // 得出iterator比const_iterator更为有用的结论就不足为奇了。

  return 0;
}