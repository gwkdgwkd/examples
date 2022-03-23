#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

// 为包含指针的关联容器指定比较类型

struct StringPtrLess : public std::binary_function<const std::string *,
                                                   const std::string *, bool> {
  bool operator()(const std::string *ps1, const std::string *ps2) const {
    return *ps1 < *ps2;
  }
};
typedef std::set<std::string *, StringPtrLess> StringPtrSet;
void print(const std::string *ps) { std::cout << *ps << std::endl; }
struct Dereferenc {
  template <typename T>
  const T &operator()(const T *ptr) const {
    return *ptr;
  }
};

// 每当要创建包含指针的关联容器时，一定要记住，容器将会按照指针的值进行排序。
// 所以几乎肯定要创建自己的函数子类作为容器的比较类型。
// 为啥是函数子类，而不是简单地写一个比较函数？因为编译不过
bool stringPtrLess1(const std::string *ps1, const std::string *ps2) {
  return *ps1 < *ps2;
}

// 每当要创建包含指针的关联容器时，同时也要指定容器的比较类型。
// 大多数情况下，只是解引用，比对所指对象进行比较。模板如下：
struct DereferenceLess {
  template <typename PtrType>
  bool operator()(PtrType pT1, PtrType pT2) const {
    return *pT1 < *pT2;
  }
};

// 本条款除了关联容器，也适用于其他容器，这些容器中包含的对象与指针的行为相似，比如智能指针和迭代器。
// 如果有一个包含智能指针或迭代器的容器，那么也要考虑为它指定一个比较类型。
// 对指针的解决方案也适用于那些类似指针的对象。
// 对于包含迭代器和指针的情形，DereferenceLess也同样可用作比较类型。

int main() {
  std::set<std::string *> ssp;
  // set<string*,less<string*>,allocator<string*> ssp;  // 上面的定义的展开
  ssp.insert(new std::string("Anteater"));
  ssp.insert(new std::string("Wombat"));
  ssp.insert(new std::string("Lemur"));
  ssp.insert(new std::string("Penguin"));
  for (std::set<std::string *>::const_iterator i = ssp.begin(); i != ssp.end();
       ++i) {
    std::cout << *i << " : " << **i << std::endl;
  }
  // 0x18d0c20 : Anteater
  // 0x18d0c80 : Wombat
  // 0x18d0ce0 : Lemur
  // 0x18d0d40 : Penguin
  // 没有按照顺序输出

  // copy(ssp.begin(), ssp.end(), std::ostream_iterator<std::string>(std::cout, "\n"));  // 编译不过

  StringPtrSet sps;
  sps.insert(new std::string("Anteater"));
  sps.insert(new std::string("Wombat"));
  sps.insert(new std::string("Lemur"));
  sps.insert(new std::string("Penguin"));
  for (std::set<std::string *>::const_iterator i = sps.begin(); i != sps.end();
       ++i) {
    std::cout << *i << " : " << **i << std::endl;
  }
  // 0x18fa1b0 : Anteater
  // 0x18fa270 : Lemur
  // 0x18fa2d0 : Penguin
  // 0x18fa210 : Wombat
  std::for_each(sps.begin(), sps.end(), print);
  // Anteater
  // Lemur
  // Penguin
  // Wombat
  std::transform(sps.begin(), sps.end(),
                 std::ostream_iterator<std::string>(std::cout, "\n"),
                 Dereferenc());
  // Anteater
  // Lemur
  // Penguin
  // Wombat

  // 编译失败
  // std::copy(sps.begin(), sps.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

  // set<string, stringPtrLess1> sps1;  // 编译不过

  return 0;
}