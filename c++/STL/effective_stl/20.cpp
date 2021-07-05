#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <string>

using namespace std;

// 为包含指针的关联容器指定比较类型

struct StringPtrLess
    : public binary_function<const string *, const string *, bool> {
  bool operator()(const string *ps1, const string *ps2) const {
    return *ps1 < *ps2;
  }
};
typedef set<string *, StringPtrLess> StringPtrSet;
void print(const string *ps) { cout << *ps << endl; }
struct Dereferenc {
  template <typename T>
  const T &operator()(const T *ptr) const {
    return *ptr;
  }
};

// 每当要创建包含指针的关联容器时，一定要记住，容器将会按照指针的值进行排序。
// 所以几乎肯定要创建自己的函数子类作为容器的比较类型。为啥是函数子类，而不是简单地写一个比较函数？因为编译不过
bool stringPtrLess1(const string *ps1, const string *ps2) {
  return *ps1 < *ps2;
}
// 每当要创建包含指针的关联容器时，同时也要指定容器的比较类型。大多数情况下，只是解引用，比对所指对象进行比较。模板如下：
struct DereferenceLess {
  template <typename PtrType>
  bool operator()(PtrType pT1, PtrType pT2) const {
    return *pT1 < *pT2;
  }
};

// 本条款除了关联容器，也适用于其他容器，这些容器中包含的对象与指针的行为相似，比如智能指针和迭代器。
// 如果有一个包含智能指针或迭代器的容器，那么也要考虑为它指定一个比较类型。对指针的解决方案也适用于那些类似指针的对象。
// 对于包含迭代器和指针的情形，DereferenceLess也同样可用作比较类型。

int main() {
  set<string *> ssp;
  // set<string*,less<string*>,allocator<string*> ssp;  // 上面的定义的展开
  ssp.insert(new string("Anteater"));
  ssp.insert(new string("Wombat"));
  ssp.insert(new string("Lemur"));
  ssp.insert(new string("Penguin"));
  for (set<string *>::const_iterator i = ssp.begin(); i != ssp.end(); ++i) {
    cout << *i << " : " << **i << endl;
  }
  // 0x18d0c20 : Anteater
  // 0x18d0c80 : Wombat
  // 0x18d0ce0 : Lemur
  // 0x18d0d40 : Penguin
  // 没有按照顺序输出

  // copy(ssp.begin(), ssp.end(), ostream_iterator<string>(cout, "\n")); // 编译不过

  StringPtrSet sps;
  sps.insert(new string("Anteater"));
  sps.insert(new string("Wombat"));
  sps.insert(new string("Lemur"));
  sps.insert(new string("Penguin"));
  for (set<string *>::const_iterator i = sps.begin(); i != sps.end(); ++i) {
    cout << *i << " : " << **i << endl;
  }
  // 0x18fa1b0 : Anteater
  // 0x18fa270 : Lemur
  // 0x18fa2d0 : Penguin
  // 0x18fa210 : Wombat
  for_each(sps.begin(), sps.end(), print);
  // Anteater
  // Lemur
  // Penguin
  // Wombat
  transform(sps.begin(), sps.end(), ostream_iterator<string>(cout, "\n"),
            Dereferenc());
  // Anteater
  // Lemur
  // Penguin
  // Wombat

  // set<string, stringPtrLess1> sps1;  // 编译不过

  return 0;
}