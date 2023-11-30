#include <iostream>
#include <map>

// 当效率至关重要时，请在map::operator[]与map::insert之间谨慎做出选择

class Widget {
 public:
  Widget() {}
  Widget(double weight) {}
  Widget& operator=(double weight) { return *this; }
};

namespace n1 {
// map的operator[]函数与众不同，它与vector、deque和string的operator[]函数无关，
// 与用于数组的内置operator[]也没有关系，map::operator[]是为了提供添加和更新的功能。
// m[k]=v;
// 检查键k是否已经在map中了，如果没有，它就被加入，并以v作为响应的值。
// 如果k已经在映射表中了，则与关联的值被更新为v。
// 如果键k已经有了相关联的值，则该值被更新，直接了当。
// 如果还没有在映射表中，那就没有operator[]可以指向的值的对象，
// 在这种情况下，先用值类型的默认构造函数创建一个新的对象，
// 然后operator[]返回指向该新对象的引用，最后这个引用成了被赋值的目标，被用值赋值。
// 现在应该明白为什么这种方式会降低性能了，先默认构造对象，然后立刻赋值。
// 如果直接使用所需要的值构造一个对象，比先默认构造一个对象再赋值效率更高，
// 那么最好把operator[]的使用换成insert的直接调用。

void func() {
  std::map<int, Widget> m;

  // m[1]是m.operator[](1)的缩写，必须返回一个指向Widget的引用：
  m[1] = 1.50;
  m.insert(std::map<int, Widget>::value_type(1, 1.50));
  // 上面两种方式的最终效果相同，只是insert会节省三个函数调用：
  // 1.一个用于创建默认构造的临时对象；
  // 2.一个用于析构该临时对象；
  // 3.一个用于调用对象的赋值操作符。
  // 这些函数调用的代价越高，使用insert代替operator[]节省的开销就越大。
  // 每个标准容器都会提供value_type类型定义，对于map和multimap，
  // 以及hash_map和hash_multimap的元素类型总是某一种pair。
  m[2] = 3.67;
  m[3] = 10.5;
  m[4] = 45.8;
  m[5] = 0.0003;

  // operator[]的设计目的是为了提供添加和更新的功能，对于添加操作时，
  // insert比operator[]效率更高，当操作更新时，形式恰好反过来了。
  m[5] = 0.002;  // 从语法形式本身考虑，倾向于使用operator[]
  m.insert(std::map<int, Widget>::value_type(4, 36.5)).first->second = 36.5;
  // 从效率上讲，insert需要一个：
  // map<int,Widget>::value_type类型的参数，即pair<int,Widget>，
  // 所以当调用insert时，必须构造和析构一个该类型的对象，付出构造和析构的代价。
  // 这有导致对Widget的构造和析构动作，因为pair包含了Wiget对象。
  // operator[]不使用pair对象，所以它不会构造和析构任何pair或Widget。
}
}  // namespace n1

namespace n2 {
// 考虑效率，当向map中添加元素时，要优先选用insert，而不是operator[]；
// 考虑效率和美观，当更新已经在map中的元素的值时，有优先选择operator[]。
// STL没有提供兼具以上两种操作优点的接口，但自己写一个并不太困难：
template <typename MapType, typename KeyArgType, typename ValueArgType>
typename MapType::iterator efficientAddOrUpdate(MapType& m, const KeyArgType& k,
                                                const ValueArgType& v) {
  // 确定k在什么位置或应在什么位置
  typename MapType::iterator lb = m.lower_bound(k);
  if (lb != m.end() && !(m.key_comp()(k, lb->first))) {
    // 如果lb指向pair的键与k等价
    lb->second = v;  // 更新pair的值并返回指向该pair的迭代器
    return lb;
  } else {
    typedef typename MapType::value_type MVT;
    // 把pair<k,v>添加到m中，并返回一个指向该新元素的迭代器
    return m.insert(lb, MVT(k, v));
    // 使用了提示形式的insert，C++标准保证，如果提示是正确的，
    // 则插入操作将耗费常数时间，而不是对数时间。
  }

  // KeyArgType和ValueArgType不必是存储在映射表中的类型。
  // 只要它们能够转换储存在映射表中的类型就可以了。
  // 另一种选择是去掉类型参数KeyArgType和ValueArgType，
  // 而用MapType::key_type和MapType::mapped_type来代替。
  // 然而如果这样做的话，在函数被调用时可能会导致不必要的类型转换。
}

void func() {
  std::map<int, Widget> m;
  // 用MapType::key_type和MapType::mapped_type，
  // 来代替KeyArgType和ValueArgType，会导致调用时把1.5转换成了Widget，
  // 需要为Widget构造和析构付出代价：
  efficientAddOrUpdate(m, 3, 1.5);
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
  return 0;
}