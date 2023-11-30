#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>

// 总是让比较函数在等值情况下返回false

namespace n1 {
void func1() {
  std::set<int, std::less_equal<int>> s;
  s.insert(10);
  // !(10A <= 10B) && !(10B <= 10A)结果为false
  // 任何一个比较函数，如果它对相等的值返回true，则都会导致同样的结果。
  s.insert(10);

  std::copy(s.begin(), s.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 10 10
}

void func2() {
  std::multiset<int, std::less_equal<int>> ms;
  ms.insert(10);
  ms.insert(10);

  std::copy(ms.begin(), ms.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 10 10
}

void func() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {
namespace test1 {
void func() {
  std::set<std::string, std::greater<std::string>> s;
  s.insert("a");
  s.insert("hello");
  s.insert("hello");
  s.insert("z");

  std::copy(s.begin(), s.end(),
            std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << std::endl;  // z hello a
}
}  // namespace test1

namespace test2 {
struct StringGreater
    : public std::binary_function<const std::string, const std::string, bool> {
  bool operator()(const std::string &ps1, const std::string &ps2) const {
    // 把less测试简单取反，这是不对的，得到的是>=，对于相等的值将返回true：
    return !(ps1 < ps2);
  }
};

void func() {
  std::set<std::string, StringGreater> s;
  s.insert("a");
  s.insert("hello");
  s.insert("hello");
  s.insert("z");

  std::copy(s.begin(), s.end(),
            std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << std::endl;  // z hello hello a
}
}  // namespace test2

namespace test3 {
struct StringGreater
    : public std::binary_function<const std::string, const std::string, bool> {
  bool operator()(const std::string ps1, const std::string ps2) const {
    return ps2 < ps1;
  }
};
void func() {
  std::set<std::string, StringGreater> s;
  s.insert("a");
  s.insert("hello");
  s.insert("hello");
  s.insert("z");

  std::copy(s.begin(), s.end(),
            std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << std::endl;  // z hello a
}
}  // namespace test3

void func() {
  test1::func();
  test2::func();
  test3::func();
}
}  // namespace n2

// 对于set和map确实是这样，因为不能包含重复的值，但对于multiset和multimap呢？
// 可以包含重复的值，因此即使容器认为两个等值的对象不等价，又有什么关系呢？
// 可以把这两个值都保存起来，这正是multiset和multimap应该做的，没问题。
// 除非比较函数对相等的值总是返回false，否则会破坏所有的标准关联容器，
// 不管它们是否允许储存重复的值，从技术上来说，用于对关联容器排序的比较函数，
// 必须为它们所比较的对象定义一个严格的弱序化，但是在严格的弱序化的要求中，
// 有一个要求就是：任何一个定义了严格的弱序化的函数必须对相同值的返回false。

int main(int argc, char *argv[]) {
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