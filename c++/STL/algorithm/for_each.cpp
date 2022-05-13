#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

// for_each()函数是C++ STL中的一个遍历函数，函数原型如下：
// for_each(InputIterator first, InputIterator last, Function functor);

// 一般情况下使用的时候都是把first作为容器遍历的起始点指针，last是容器的末尾。
// 重点提到的是functor(仿函数)。
// 仿函数是一种在模板类编程时的封装手法，本质上的仿函数是一种类(class)。
// 但是实际起到的是一种函数的作用，在这个所谓的class中通常都是一个operator()的重载，
// 这个重载是根据programmer的需求产生的，通过调用类的构造函数就使得这个类有了函数的行为。
// 进一步的解释就是，在写代码的时候，总有一部分代码的重用率非常高，
// 通常的情况下我们会把这段代码写成一个顶层函数放在类外，任何的成员函数都可以去调用或访问，
// 但是在编写代码的同时也会产生大量的全局变量，很难维护。
// 为了提高代码的内聚度，我们选择将这部分代码封装到一个类中，
// 通过调用类的含参或默认构造函数来执行这段代码，我们将这种做法成为仿函数。
// 仿函数的机制为编写代码提供了很好的有利于资源管理的手段，如果得到恰当应用的话会写出质量很高的代码。

// for_each()具有如下的特性：
// 1.for_each()对当前[begin, end)范围内的所有成员进行访问；
// 2.仿函数对每个成员进行相应操作；
// 3.for_each()返回functor的一个副本；
// 4.for_each()忽略functor的任何返回值；
// 5.算法时间复杂度为O(n)，n为容器中的变量数目。
// 个人的思路是for_each()的返回值需要用户使用一个相同类型的变量进行记录，
// 函数本身忽略仿函数对当前参量的任何处理，也就是说无法记录参量的最终状态。

void myfunction(int i) { std::cout << ' ' << i; }
struct myclass {
  void operator()(int i) { std::cout << ' ' << i; }
} myobject;

int add(int &i) { return i++; }
struct addclass {
  void operator()(int &i) { i += 5; }
};

int main() {
  std::vector<int> myvector;
  myvector.push_back(10);
  myvector.push_back(20);
  myvector.push_back(30);

  std::cout << "myvector contains:";
  for_each(myvector.begin(), myvector.end(), myfunction);
  std::cout << std::endl;
  // myvector contains: 10 20 30

  std::cout << "myvector contains:";
  for_each(myvector.begin(), myvector.end(), myobject);
  std::cout << std::endl;
  // myvector contains: 10 20 30

  // 修改元素
  for_each(myvector.begin(), myvector.end(), add);
  std::copy(myvector.begin(), myvector.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // 11 21 31

  // 修改元素
  for_each(myvector.begin(), myvector.end(), addclass());
  std::copy(myvector.begin(), myvector.end(),
            std::ostream_iterator<int>(std::cout, " "));
  std::cout << std::endl;
  // 16 26 36

  return 0;
}
