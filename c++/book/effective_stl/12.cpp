#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// 切勿对STL容器的线程安全性有不切实际的依赖

// 标准C++的世界相当狭小和古旧，在这个纯净的世界中，
// 所有的可执行程序都是静态链接的，不存在内存映像或共享内存。
// 没有窗口系统，没有网络，没有数据库，也没有其他进程。
// 对于STL的线程安全性的第一个期望应该是，它会随不同实现而异。

// 对一个STL实现最多只能期望：
// 1.多个线程读是安全，并且在读的过程中，不能对容器有任何写入操作；
// 2.多个线程对不同的容器做写入操作是安全的。
// 这些只是所期望的，而不是所能依赖的，有些实现提供了这些保证，有些则没有。

// 试图实现完全的容器线程安全性时可能采取的方式：
// 1.对容器成员函数的每次调用，都锁住容器直到调用结束；
// 2.在容器所返回的每个迭代器（begin或end）的生存期结束前，都锁住容器；
// 3.对于作用于容器的每个算法，都锁住该容器，直到算法结束，实际上这样做没有意义。

// 需要手工做同步控制。
void getMutexFor(const std::vector<int> &) {}
void releaseMutexFor(const std::vector<int> &) {}

// 使用类（如Lock）来管理资源的生存期的意思通常被称为获得资源时即初始化：
template <typename Container>
class Lock {
 public:
  Lock(const Container &container) : c(container) { getMutexFor(c); }
  ~Lock() { releaseMutexFor(c); }

 private:
  const Container &c;
};

// 当涉及到STL容器和线程安全性时，可以指望一个STL库允许多个线程同时读一个容器，
// 以及多个线程对不同容器的写入操作，不能指望STL库把你从手工同步控制中解脱出来，
// 而且不能依赖于任何线程支持。

namespace n1 {
void func() {
  std::vector<int> v;
  std::vector<int>::iterator it(find(v.begin(), v.end(), 5));
  if (it != v.end()) {
    *it = 0;
  }
}
}  // namespace n1

namespace n2 {
void func() {
  std::vector<int> v;
  getMutexFor(v);
  std::vector<int>::iterator it(find(v.begin(), v.end(), 5));
  if (it != v.end()) {
    *it = 0;
  }
  releaseMutexFor(v);  // 上面有异常，这里不会被执行
}
}  // namespace n2

namespace n3 {
void func() {
  // 希望能n2有异常发生时也是强壮的，可以使用代码块，
  // C++保证如果有异常被抛出，局部对象会被析构。
  std::vector<int> v;
  {  // 创建代码块
    Lock<std::vector<int>> lock(v);
    std::vector<int>::iterator it(find(v.begin(), v.end(), 5));
    if (it != v.end()) {
      *it = 0;
    }
  }
}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    case 2:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}