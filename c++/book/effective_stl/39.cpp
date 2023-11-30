#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 确保判别式是纯函数

// 一个判别式（predicate）是返回值为bool，或者可以隐式地转换为bool的函数。
// 在STL中，判别式有着广泛的用途，标准关联容器的比较函数就是判别式，
// 对于像find_if以及各种与排序有关的算法，判别式往往也被作为参数来传递。

// 一个纯函数是指返回值仅仅依赖于其参数的函数。
// 在C++中，纯函数所能访问的数据应该局限于参数以及常量。
// 如果一个纯函数需要访问那些可能会在两次调用之间发生变化的数据，
// 那么相同的参数在不同的时刻调用该函数就有可能得到不同的结果，与纯函数的定义相矛盾。

// 判别式类是一个函数子类，它的operator()是判别式，应该返回true或者false。
// STL中凡是能接受判别式的地方，就既可以接受真正的判别式，也可以接受判别式类的对象。

namespace n1 {
// 接受函数子的STL算法可能会先创建函数子的拷贝，然后存放起来待以后再使用这些拷贝，
// 而且，有些STL算法实现也确实利用了这一特性，直接反映就是：要求判别式函数必须是纯函数。
class BadPredicate : public std::unary_function<int, bool> {
 public:
  BadPredicate() : timesCalled(0) {}
  bool operator()(const int&) { return ++timesCalled == 3; }
  // 解决下面问题的办法是把operator()函数声明为const，
  // 这样编译器不会改变任何一个成员：
  // bool operator()(const int&) const {
  //   return ++timesCalled == 3; // 报错
  // }

 private:
  size_t timesCalled;
};

void func() {
  std::vector<int> vw{1, 2, 3, 4, 5, 6};
  std::copy(vw.begin(), vw.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 3 4 5 6

  // 不仅删除了第三个，而且还删除了第六个：
  // remove_if中调用了find_if和remove_copy_if，在find_if中加到了3次，
  // 然后拷贝给remove_copy_if，调用三次后还返回true。
  vw.erase(remove_if(vw.begin(), vw.end(), BadPredicate()), vw.end());
  copy(vw.begin(), vw.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;  // 1 2 4 5
}
}  // namespace n1

namespace n2 {
// 将判别式的operator()函数声明为const，还远远不够。
// 即使是const成员函数，也可以访问mutable成员、非cosnt对象、以及非const的全局对象。
// 一个精心设计的判别式应该保证其operator()函数完全独立所有这些变量。
// 一个行为正常的判别式的operator()肯定是const的，还应该是一个纯函数。
// STL中的判别式函数和判别式类的对象可以互相代替，
// 所以判别式函数也应该和判别式类中的operator()函数一样，应该是纯函数：
bool BadPredicate(const int&, const int&) {
  static int timesCalled = 0;  // 切记不可如此
  return ++timesCalled == 3;   // 应该是纯函数，而纯函数没有状态
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}