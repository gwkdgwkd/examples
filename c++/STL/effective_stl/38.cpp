#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>

using namespace std;

// 遵守按值传递的原则来设计函数子类

// 函数和类似于函数的对象（即函数子，functor）遍布在STL的每个角落。
//  关联容器利用它们为其元素进行排序；
//  find_if这样的STL算法使用它们来控制算法的行为；
//  如果没有函数子，那么for_each和transform这样的功能组件就形同虚设；
//  not1和bind2nd这样的配接器则可以动态生成函数子。

// 无法是C还是C++，都不允许将一个函数作为参数传递给另一个函数，相反，必须传递函数指针。
// void qsort(void *base, size_t nmemb, size_t size, int(*cmpfcn)(const void*, const void*));
// C和C++标准库函数都遵循的规则：函数指针是按值传递的。

// STL函数对象是函数指针的一种抽象和建模形式，所以STL中，函数对象在函数之间来回传递的时候也是按值传递的。
// template <class InputIterator, class Function>
// Function for_each(InputIterator first, InputIterator last, Function f);
// 按值传递的行为并非不可改变，for_each的调用者在调用点上可以显式地指明模板参数的类型。迫使for_each按引用方式传递参数和返回值：
class DoSomething : public unary_function<int, void> {
 public:
  void operator()(int x) {}
};
typedef deque<int>::iterator DequeIntIter;

// 由于函数对象往往会按值传递和返回，所以，你必须确保编写的函数对象在经过了传递之后还能正常工作。意味这两件事：
//  首先，你的函数对象必须尽可能地小，否则拷贝的开销会非常昂贵；
//  其次，函数对象必须是单态的（不是多态的），也就是说，不能使用虚函数。
//       如果参数的类型是基类类型，而实参是派生类对象，那么在传递过程中会产生剥离问题：对象拷贝过程中，派生部分可能会被去掉。
// 当然，效率很重要，而避免剥离问题同样也很重要，但并不是所有的函数子都非常小巧，而且，也不是所有的函数子都是单态的。
// 与普通函数相比，函数对象的一大优点是，函数子可以包含所需要的状态信息。
// 有些函数对象生来就显得非常繁重，所以要能够像传递普通的函数指针那样方便地将这样的函数子传递给STL算法。
// 试图禁止多态的函数子同样也是不切实际的。所以必须找到一个两全其美的办法，既允许函数对象可以很大并且保留多态性，
// 有可以与STL所采用的按值传递函数子的习惯保持一致。
// 方法是：将数据和虚函数从函数子中分类出来，放到一个新的类中；然后在函数子类中包含指向这个新类的指针。
struct Widget {};
template <typename T>
class BPFC1 : public unary_function<T, void> {
 private:
  Widget w;
  int x;
  // 很多数据

 public:
  virtual void operator()(const T& val) const;
};
// 上面的应该分成下面的两个类：
template <typename T>
class BPFC;
template <typename T>
class BPFCImpl : public unary_function<T, void> {
 private:
  Widget w;
  int x;
  virtual ~BPFCImpl();
  virtual void operator()(const T& val) const;
  friend class BPFC<T>;
};
template <typename T>
class BPFC : public unary_function<T, void> {
  // 小巧而且是单态的，但却可以访问大量状态信息且行为上具备多态特性。
 private:
  BPFCImpl<T>* pImpl;

 public:
  void operator()(const T& val) const { pImpl->operator()(val); }
  // 必须以某种合理的方式来支持拷贝动作。
  // 确保BPFC的拷贝构造函数正确地处理了所指向的BPFCImpl对象。简单合理的做法是使用引用计数。
};

// 需要谨慎处理BPFC的拷贝构造函数，因为函数对象在STL中作为参数传递或者返回的时候总是按值方式传递拷贝的。
// 意味着两件事情：第一，使他们小巧；第二，使它们成为单态的。

int main() {
  deque<int> di;
  DoSomething d;
  // for_each<DequeIntIter, DoSomething&>(di.begin(), di.end(), d);
  // 不能通过编译，因为有些STL算法的某些实现甚至根本不能通过编译

  return 0;
}