#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

// find()、merge()、sort()等算法函数都至少要用一对迭代器来指明作用区间，并且为了实现自己的功能，每个函数内部都势必会对指定区域内的数据进行遍历操作。
// 虽然这些算法函数的内部实现我们不得而知，但无疑都会用到循环结构。可以这么说，STL标准库中几乎所有的算法函数，其底层都是借助循环结构实现的。
// 由于STL标准库使用场景很广，因此很多需要手动编写循环结构实现的功能，用STL算法函数就能完成。
// 手动编写循环结构和调用STL算法函数相比，哪种实现方式更好呢？毫无疑问，直接调用算法会更好，理由有以下几个：
//  算法函数通常比自己写的循环结构效率更高；
//  自己写循环比使用算法函数更容易出错；
//  相比自己编写循环结构，直接调用算法函数的代码更加简洁明了。
//  使用算法函数编写的程序，可扩展性更强，更容易维护；

// 为什么STL算法效率更高?
// 手动编写的循环代码每一次循环都要执行一次end()方法，事实上该方法并不需要多次调用，因为它的值自始至终都没有发生改变。
// 也就是说，end()方法只需要调用一次就够啦，for_each()函数就对这一点进行了优化。
// 通过将end()方法作为参数传入for_each()函数，该方法只执行了1次。当然，这也仅是众多优化中的一处。
// 事实上，STL标准库的开发者对每个算法函数的底层实现代码都多了优化，使它们的执行效率达到最高。
// 难道我们自己对循环结构进行优化不行吗？可以，但是其执行效率仍无法和算法函数相提并论。
// 一方面，STL开发者可以根据他们对容器底层的了解，对整个遍历过程进行优化，而这是我们难以做到的。以deque容器为例，该容器底层会将数据存储在多个大小固定的连续空间中。
// 对于这些连续空间的遍历，只有STL开发者才知道这些连续空间的大小，才知道如何控制指针逐个遍历这些连续空间。
// 另一方面，某些STL函数的底层实现使用了复杂的科学计算方法，并不是普通C++程序员能驾驭的。例如，在实现对某个序列进行排序时，我们很难编写出比sort()函数更高效的代码。
// 总之，STL开发者比使用者更了解内部的实现细节，他们会充分利用这些知识来对算法进行优化。
// 只有熟悉STL标准库提供的函数，才能在实际编程时想到使用它们。作为一个专业的C++程序员，我们必须熟悉STL标准库中的每个算法函数，并清楚它们各自的功能。
// C++ STL标准库中包含70多个算法函数，如果考虑到函数的重载，大约有100多个不同的函数模板。

// 其中有些函数名称和STL容器模板类中提供的成员方法名相同。
// 当某个STL容器提供有和算法同名的成员方法时，应该使用哪一个呢？大多数情况下，我们应该使用STL容器提供的成员方法，而不是同名的STL算法，原因包括：
//  虽然同名，但它们的底层实现并不完全相同。相比同名的算法，容器的成员方法和自身结合地更加紧密。
//  相比同名的算法，STL容器提供的成员方法往往执行效率更高；
// 无论是序列式容器还是关联式容器，成员方法的执行效率要高于同名的STL算法。仍以find()函数和set容器中的find()成员方法为例。要知道，find()函数是通过“逐个比对”来实
// 现查找的，它以线性时间运行；而由于set容器底层存储结构采用的是红黑树，所以find()成员方法以对数时间运行，而非线性时间。
// 换句话说，对于含有一百万个元素的set容器，如果使用find()成员方法查找目标元素，其最差情况下的比对次数也不会超过40次（平均只需要比对20次就可以查找成功）；
// 而使用同名的find()函数查找目标元素，最差情况下要比对一百万次（平均比对50万次才能查找成功）。
// 所谓“最差情况”，指的是当前set容器中未存储有目标元素。
// 虽然有些容器提供的成员方法和某个STL算法同名，但该容器只能使用自带的成员方法，而不适用同名的STL算法。比如，sort()函数根本不能应用到list容器上，因为该类型容器仅
// 支持双向迭代器，而sort()函数的参数类型要求为随机访问迭代器；merge()函数和list容器的merge()成员方法之间也存在行为上的不同，即merge()函数是不允许修改源数据的，
// 而list::merge()成员方法就是对源数据做修改。
// 总之，当读者需要在STL算法与容器提供的同名成员方法之间做选择的时候，应优先考虑成员方法。几乎可以肯定地讲，成员方法的性能更优越，也更贴合当前要操作的容器。

class Address {
 public:
  Address(string url) : url(url){};
  void display() { cout << "url:" << this->url << endl; }

 private:
  string url;
};

class mycomp {
 public:
  bool operator()(const string &i, const string &j) const {
    return i.length() < j.length();
  }
};

int main() {
  vector<Address> adds{Address("http://c.biancheng.net/stl/"),
                       Address("http://c.biancheng.net/java/"),
                       Address("http://c.biancheng.net/python/")};
  cout << "first：\n";
  for (auto it = adds.begin(); it != adds.end(); ++it) {
    (*it).display();
  }
  cout << "second：\n";
  for_each(adds.begin(), adds.end(), mem_fun_ref(&Address::display));

  std::set<string, mycomp> myset{"123", "1234", "123456"};
  set<string>::iterator iter = myset.find(string("abcd"));
  if (iter == myset.end()) {
    cout << "查找失败" << endl;
  } else {
    cout << *iter << endl;  // 1234
  }
  auto iter2 = find(myset.begin(), myset.end(), string("abcd"));
  if (iter2 == myset.end()) {
    cout << "查找失败" << endl;  // 查找失败
  } else {
    cout << *iter << endl;
  }
  // 之所以会这样，是因为find()成员方法和find()函数底层的实现机制不同。前者会依照mycomp()规则查找和"abcd"匹配的元素，而find()函数底层仅会依据"=="运算符
  // 查找myset容器中和"abcd"相等的元素，所以会查找失败。

  return 0;
}