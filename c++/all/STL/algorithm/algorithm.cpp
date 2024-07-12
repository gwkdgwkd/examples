#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

// find()、merge()、sort()等算法函数都至少要用一对迭代器来指明作用区间，
// 并且为了实现自己的功能，每个函数内部都势必会对指定区域内的数据进行遍历操作。
// 虽然这些算法函数的内部实现不得而知，但无疑都会用到循环结构。
// 可以这么说，STL标准库中几乎所有的算法函数，其底层都是借助循环结构实现的。
// 由于STL使用场景很广，因此很多需要手动编写循环的功能，用STL算法函数就能完成。
// 手动编写循环结构和调用STL算法函数相比，哪种实现方式更好呢？
// 毫无疑问，直接调用算法会更好，理由有以下几个：
// 1.算法函数通常比自己写的循环结构效率更高；
// 2.自己写循环比使用算法函数更容易出错；
// 3.相比自己编写循环结构，直接调用算法函数的代码更加简洁明了；
// 4.使用算法函数编写的程序，可扩展性更强，更容易维护。

// 为什么STL算法效率更高？
// 手动编写的循环代码每一次循环都要执行一次end()方法，
// 事实上该方法并不需要多次调用，因为它的值自始至终都没有发生改变。
// 也就是说，end()只调一次就够啦，for_each()就对这一点进行了优化。
// 通过将end()方法作为参数传入for_each()函数，该方法只执行了1次。
// 当然，这也仅是众多优化中的一处，事实上，STL对每个算法的都进行了优化，
// 使它们的执行效率达到最高，难道自己对循环结构进行优化不行吗？
// 可以，但是其执行效率仍无法和算法函数相提并论。
// 一方面，STL开发者可以根据他们对容器底层的了解，
// 对整个遍历过程进行优化，而这是我们难以做到的。
// 以deque容器为例，该容器底层会将数据存储在多个大小固定的连续空间中。
// 对于这些连续空间的遍历，只有STL开发者才知道这些连续空间的大小，
// 才知道如何控制指针逐个遍历这些连续空间。
// 另一方面，某些STL函数使用了复杂的科学计算方法，并不是普通程序员能驾驭的。
// 例如，在实现对某个序列进行排序时，很难编写出比sort()函数更高效的代码。
// 总之，STL开发者比使用者更了解实现细节，会充分利用这些知识来对算法进行优化。
// 只有熟悉STL标准库提供的函数，才能在实际编程时想到使用它们。
// 作为一个专业的C++程序员，必须熟悉STL中的每个算法，并清楚它们各自的功能。
// C++ STL中包含70多个算法，如果考虑到重载，大约有100多个不同的函数模板。

// 其中有些函数名称和STL容器模板类中提供的成员方法名相同。
// 当某个STL容器提供有和算法同名的成员方法时，应该使用哪一个呢？
// 大多数情况下，应该使用STL容器的成员方法，而不是同名的STL算法，原因包括：
// 1.虽然同名，但它们的底层实现并不完全相同，
//   相比同名的算法，容器的成员方法和自身结合地更加紧密；
// 2.相比同名的算法，STL容器提供的成员方法往往执行效率更高，
//   无论是序列式容器还是关联式容器，成员方法的执行效率要高于同名的STL算法。

// 以find()函数和set容器中的find()成员方法为例。
// find()函数是通过逐个比对来实现查找的，它以线性时间运行；
// set底层采用的是红黑树，所以find()成员方法以对数时间运行，而非线性时间。
// 换句话说，对于含有一百万个元素的set，如果使用find()成员方法查找目标元素，
// 其最差情况下的比对次数也不会超过40次，平均只需要比对20次就可以查找成功；
// 而使用同名的find()查找目标，最差要比对一百万次，平均比对50万次才能查找成功。
// 所谓最差情况，指的是当前set容器中未存储有目标元素。

// 虽然有些容器提供的成员方法和某个STL算法同名，
// 但该容器只能使用自带的成员方法，而不适用同名的STL算法。
// 比如，sort()函数根本不能应用到list容器上，因为该类型容器仅支持双向迭代器，
// 而sort()函数的参数类型要求为随机访问迭代器；
// merge()和list的merge()成员方法行为也不同，即merge()是不允许修改源数据的，
// 而list::merge()成员方法就是对源数据做修改。

// 总之，当需要在STL算法与容器的同名成员方法之间做选择的时候，优先考虑成员方法。
// 几乎可以肯定地讲，成员方法的性能更优越，也更贴合当前要操作的容器。

class Address {
 public:
  Address(std::string url) : url(url){};
  void display() { std::cout << "url:" << this->url << std::endl; }

 private:
  std::string url;
};

class mycomp {
 public:
  bool operator()(const std::string &i, const std::string &j) const {
    return i.length() < j.length();
  }
};

int main() {
  std::vector<Address> adds{Address("ni"), Address("nihao"), Address("nihaoa")};
  for (auto it = adds.begin(); it != adds.end(); ++it) {
    (*it).display();
  }
  // url:ni
  // url:nihao
  // url:nihaoa

  for_each(adds.begin(), adds.end(), std::mem_fun_ref(&Address::display));
  // url:ni
  // url:nihao
  // url:nihaoa

  std::set<std::string, mycomp> s{"123", "1234", "123456"};
  std::set<std::string>::iterator iter = s.find(std::string("abcd"));
  if (iter == s.end()) {
    std::cout << "not found" << std::endl;
  } else {
    std::cout << *iter << std::endl;
  }
  // 1234
  iter = find(s.begin(), s.end(), std::string("abcd"));
  if (iter == s.end()) {
    std::cout << "not found" << std::endl;
  } else {
    std::cout << *iter << std::endl;
  }
  // not found

  // 之所以会这样，是因为find()成员方法和find()函数底层的实现机制不同。
  // 前者会依照mycomp()规则查找和abcd匹配的元素，
  // 而find()底层仅会依据==运算符查找s容器中和abcd相等的元素，所以查找失败。

  return 0;
}