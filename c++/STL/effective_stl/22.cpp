#include <iostream>
#include <map>
#include <set>
#include <string>

// 切勿直接修改set或multiset中的键

// set和multiset按照一定的顺序来存放自己的元素，而这些容器的正确行为也是建立在其元素保持有序的基础上的。
// 如果你把关联容器中一个元素的值改变了，那么新的值可能不在正确的位置上，这将会打破容器的有序性。

class Employee {
 public:
  std::string name() const { return std::string(""); }
  void setName(const std::string& name) {}
  std::string title() const { return std::string(""); }
  void setTitle(const std::string& title) {}
  int idNumber() const { return 1; }
};
struct IDNumberLess : public std::binary_function<Employee, Employee, bool> {
  bool operator()(const Employee& lhs, const Employee& rhs) const {
    return lhs.idNumber() < rhs.idNumber();
  }
};

// 如果不关心可移植性，而想改变set和multiset中的元素的值，并且STL的实现允许这样做，那就做。只要不修改键就好了。
// 如果重视可移植性，就要确保set和multiset中的元素不能被修改。至少不能未经过强制类型转换就修改。

// 对于set和multiset，如果直接对容器中的元素做了修改，那么要保证该容器仍然是排序的。

int main() {
  // 对于map和multimap尤其简单，因为如果有程序试图改变这些容器中的键，将不能通过编译(除非使用强制类型转换)：
  std::map<int, std::string> m;
  // m.begin()->first = 10;

  std::multimap<int, std::string> mm;
  // mm.begin()->first = 20;

  // 下面的方法对set和multiset没有任何问题，但对于map和multimap，情况就复杂了。
  // map和multimap中保存的是pair，pair的第一个部分被定义成const，如果试图强制转换掉const属性，结果将可能会改变键部分。
  // 理论上，一个STL实现可以把这样的值写在一个只读的内存区域，这时要是尝试修改，最好的结果将是没有效果。
  // 如果你坚持要遵从C++标准所制定的规则，那就永远都不要试图修改在map和multimap中作为键的对象。

  // 但对set和multiset是可能的。对于set和multiset类型的对象，容器中元素类型是T，而不是const T。
  // 因此，只要你愿意，随时可以改变set或multiset中的元素，而无需任何强制类型转换。
  // 为什么set和multiset中的元素不能是const的？如果是const，那么将不能修改其中对象的非排序部分，这样的需求是合理的。
  typedef std::set<Employee, IDNumberLess> EmpIDSet;
  EmpIDSet se;
  Employee selectedID;
  EmpIDSet::iterator i = se.find(selectedID);
  if (i != se.end()) {
    // 尽管set和multiset的元素不是const，STL也有办法防止它们被修改。
    // 比如set<T>::iterator和operator*返回一个const T&。
    // 也就是说，解除set迭代器的引用之后的结果是一个指向该集合中元素的const引用，无法修改set和multiset的元素。
    // C++在这一点上没有一个统一的说法。不同的实现者以不同的方式做了解释。
    // i->setTitle("Corporate Deity");  // 标准模棱两可，所有试图修改set和multiset中元素的代码是不可移植的

    // 改变set和multiset中元素的非键部分是合理的。正确且可移植的做法是：
    const_cast<Employee&>(*i).setTitle("Corporate Deity");
    // 通过强制类型转换到引用类型，避免了创建新对象。

    // 下面的方法可以通过编译，但是不会修改*i。在这两种情况下，类型转换的结果是一个临时的匿名对象，是*i的一个拷贝：
    static_cast<Employee>(*i).setTitle("Corporate Deity");
    ((Employee)(*i)).setTitle("Corporate Deity");
    // 类似下面的写法：
    Employee tempCopy(*i);
    tempCopy.setTitle("Corporate Deity");

    // 强制类型转换是危险的，执行一次就意味着临时关闭了类型系统的安全性。
    // 大多数强制类型转换都可以避免，包括刚刚考虑过的转换，
    // 以一种总是可行且安全的方式修改set、multiset、map和multimap中的元素，可以分为5个简单步骤：
    // 1.找到想修改的容器的元素。
    // 2.为将要被修改的元素做一份拷贝。
    //   在map和multimap的情况下，不要把该拷贝的第一个部分声明为const，毕竟，你想要改变它。
    Employee e(*i);
    // 3.修改该拷贝，是它具有你期望它在容器中的值。
    e.setTitle("Corporate Deity");
    // 4.把该元素从容器中删除，通常是通过erase来进行。
    se.erase(i++);
    // 5.把新值插入到容器中。
    //   如果按照容器的排序顺序，新元素的位置可能与被删除元素的位置相同或紧邻，则使用“提示”形式的insert，
    //   以便把插入效率从对数时间提高到常数时间。把你从第一步得来的迭代器作为提示信息。
    se.insert(i, e);
  }

  return 0;
}