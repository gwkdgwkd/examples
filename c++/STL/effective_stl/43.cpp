#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>

// 算法调用优先于手写的循环

// 算法的内部都是循环，由于STL算法涉及面很广，
// 所以意味着本该编写循环来完成的任务也可以用STL算法来完成。
class Widget {
 public:
  void redraw() const {};
};

// 调用算法通常是更好的选择，它往往优先于任何一个手写循环。为什么呢？
// 1.效率：算法通常比程序员自己写的循环效率更高。
// 2.正确性：自己写的循环比使用算法更容易出错。
// 3.可维护性：使用算法的代码通常比手写循环的代码更加简洁明了。

// 从效率方面来说，算法有三个理由比显式循环更好，
// 其中两个是主要的，一个是次要的：
// 1.次要的是可以减少冗余的计算，每次迭代都需要检查i是否等于lw.end()。
//   这意味着每次循环时，函数list::end()都要被调用。
//   但并不需要每次都调用它，因为我们并没有改变链表，这个函数调用一次就够了。
//   for(list<Widget>::iterator i = lw.begin(); i != lw.end(); ++i)
//   STL实现者很清楚，begin、end（类似的size）都是被频繁使用的函数，
//   几乎肯定会使用inline来编译，并努力改善这些函数的代码，
//   尽可能让大多数编译器都能将循环中的计算提到外面来，比避免重复计算。
//   然而，实践表明，实现者并不是每次都能成功，
//   不成功时，使用算法就比手写循环值得了。
// 2.最主要的是，类库的实现者可以根据对容器实现的了解程度对遍历过程进行优化，
//   这是库的使用者很难做到的。
//   例如deque中的对象通常被存放在一个或多个固定大小的数组中。
//   对于这些数组，基于指针的遍历比基于迭代器的遍历要快的多。
//   但是只有库的实现者才可以使用基于指针的遍历，
//   因为只有他们才知道内部数组的大小，才知道如何从一个数组转移到另一个数组。
//   有些STL包含的算法实现考虑到deque的内部数据结构，
//   这些算法实现比普通的算法实现了快20%多。
//   STL算法的实现未必一定都针对deque进行了优化，
//   但无论如何实现者肯定比你更了解内部的实现细节，可以在算法中充分利用这些知识。
// 3.其次的性能增益在于，除了一些不太重要的算法以外，
//   其他几乎所有的STL算法都使用了复杂的计算科学算法，
//   有些算法非常复杂，并非一般C++程序员能够达到。
//   例如你很难在算法级别上写出比STL的sort或类似的算法更有效的代码。

// 除了效率的原因，正确性也许更有说服力。
// 当编写循环代码时，最要紧的莫过于要保证所使用的迭代器都是：
// 1.都是有效的；
// 2.并且指向你所希望的地方。
size_t fillArray(double *pArray, size_t arraySize) { return arraySize; }

// 使用算法的关键是要熟知算法的名称。
// 在STL中有70个算法名称，如果考虑重载的情形，大约有100个不同的函数模板。
// 每一位专业C++程序员都应该知道（或者会查找）每一个算法所做的事情。
// 算法的名称比普通的循环更有意义，这已是不争的事实。
// 想要表明在一次迭代中该完成什么工作，则使用循环比算法更为清晰。

// 使用了STL的精巧的C++程序比不用STL的程序所包含的循环要少的多。
// 任何时候都应该尽量用较高层次的insert、
// find和for_each来替换较低层次的for、while和do，
// 这样提高了抽象层次，更易于编写，更易于文档化，也更易于扩展和维护。

int main() {
  std::list<Widget> lw;
  for (std::list<Widget>::iterator i = lw.begin(); i != lw.end(); ++i) {
    i->redraw();
  }
  for_each(lw.begin(), lw.end(), std::mem_fun_ref(&Widget::redraw));

  double data[10];
  std::deque<double> d;
  size_t numDoubles = fillArray(data, 10);
  // 新插入的数据与data中相应的数据排列顺序相反，
  // 最后插入的元素跑到了deque的最前面：
  for (size_t i = 0; i < numDoubles; ++i) {
    d.insert(d.begin(), data[i] + 41);
  }

  // 产生未定义的行为，每次insert被调用的时候，
  // 都会使deque中的所有迭代器无效，也包括insertLocation：
  std::deque<double>::iterator insertLocation = d.begin();
  for (size_t i = 0; i < numDoubles; ++i) {
    d.insert(insertLocation++, data[i] + 41);
  }

  // 这个例子就是手写循环难以保证其正确性的一个典型实例。
  // 这段代码是真正想要的，因为要一刻不停地为迭代器的有效性担心：
  std::deque<double>::iterator insertLocation1 = d.begin();
  for (size_t i = 0; i < numDoubles; ++i) {
    insertLocation1 = d.insert(insertLocation1, data[i] + 41);
    ++insertLocation1;
  }

  transform(data, data + numDoubles, inserter(d, d.begin()),
            std::bind2nd(std::plus<double>(), 41));

  return 0;
}
