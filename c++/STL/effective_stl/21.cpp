#include <iostream>
#include <set>

// 总是让比较函数在等值情况下返回false

struct StringPtrGreater1
    : public std::binary_function<const std::string *, const std::string *,
                                  bool> {
  bool operator()(const std::string *ps1, const std::string *ps2) const {
    // 把旧的测试简单取反，这是不对的，得到的是>=，对于相等的值将返回true
    return !(*ps1 < *ps2);
  }
};

struct StringPtrGreater2
    : public std::binary_function<const std::string *, const std::string *,
                                  bool> {
  bool operator()(const std::string *ps1, const std::string *ps2) const {
    return *ps2 < *ps1;
  }
};

// 对于set和map确实是这样，因为这些容器不能包含重复的值。
// 但对于multiset和multimap呢？这些容器可以包含重复的值，因此即使容器认为两个等值的对象不等价，又有什么关系呢？
// 可以把这两个值都保存起来，这正是multiset和multimap应该做的，没问题。
// 除非你的比较函数对相等的值总是返回false，否则会破坏所有的标准关联容器，不管它们是否允许储存重复的值。
// 从技术上来说，用于对关联容器排序的比较函数必须为它们所比较的对象定义一个“严格的弱序化”。
// 但是在“严格的弱序化”的要求中，有一个要求就是任何一个定义了“严格的弱序化”的函数必须对相同值的两个拷贝返回false。

int main() {
  std::set<int, std::less_equal<int> > s;
  s.insert(10);
  // !(10A <= 10B) && !(10B <= 10A)结果为false
  // 任何一个比较函数，如果它对相等的值返回true，则都会导致同样的结果。
  s.insert(10);
  for (std::set<int>::iterator i = s.begin(); i != s.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 10 10

  std::multiset<int, std::less_equal<int> > ms;
  ms.insert(10);
  ms.insert(10);
  for (std::set<int>::iterator i = ms.begin(); i != ms.end(); ++i) {
    std::cout << *i << " ";
  }
  std::cout << std::endl;  // 10 10

  return 0;
}