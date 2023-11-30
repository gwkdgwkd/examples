#include <iostream>
#include <unordered_set>

// 虽然每种无序容器都指定了默认的hash<key>哈希函数和equal_to<key>比较规则，
// 但它们仅适用于存储基本类型（int、double、float、string等）数据的无序容器。
// 换句话说，如果无序容器存储的数据类型为自定义的结构体或类，
// 则STL标准库提供的hash<key>和equal_to<key>将不再适用。
// 无序容器以键值对的方式存储数据，且底层采用哈希表结构存储各个键值对。
// 在此存储结构中，哈希函数的功能是根据各个键值对中键的值，计算出哈希值（一个整数），
// 哈希表可以根据该值判断出该键值对具体的存储位置。
// 简单地理解哈希函数，它可以接收一个元素，并通过内部对该元素做再加工，
// 最终会得出一个整形值并反馈回来，需要注意的是，哈希函数只是一个称谓，
// 其本体并不是普通的函数形式，而是一个函数对象类。
// 因此，如果想自定义个哈希函数，就需要自定义一个函数对象类。

// 默认的hash<key>哈希函数，其底层也是以函数对象类的形式实现的。
// 无论创建哪种无序容器，都需要为其指定一种可比较容器中各个元素是否相等的规则。
// 默认情况下无序容器使用的std::equal_to<key>比较规则，其本质也是函数对象类：
// template <class T>
// class equal_to {
//  public:
//   bool operator()(const T& _Left, const T& _Right) const {
//     return (_Left == _Right);
//   }
// };
// 该规则在底层实现过程中，直接用==运算符比较容器中任意2个元素是否相等，
// 这意味着，如果容器中存储的元素类型，支持直接用==运算符比较是否相等，
// 则该容器可以使用默认的std::equal_to<key>比较规则，反之，就不可以使用。

// 总的来说，当无序容器中存储的是基本类型数据时，
// 自定义哈希函数和比较规则，都只能以函数对象类的方式实现。
// 而当无序容器中存储的是用结构体或类自定义类型的数据时，
// 自定义哈希函数的方式仍只有一种，即使用函数对象类的形式；
// 而自定义比较规则的方式有两种：
// 1.以函数对象类的方式；
// 2.仍使用默认的std::equal_to<key>规则，但前提是必须重载==运算符。

// 也就是说：
//                  自定义哈希函数          自定义比较规则
// 基本类型            函数对象类             函数对象类
// 结构体或类          函数对象类             1.函数对象
//                                2.默认的std::equal_to<key>和重载==

namespace n1 {
class Person {
 public:
  Person(std::string name, int age) : name(name), age(age){};
  std::string getName() const { return this->name; }
  int getAge() const { return this->age; }

 private:
  std::string name;
  int age;
};
class hash_fun {
 public:
  int operator()(const Person &A) const { return A.getAge(); }
};

void test() {
  // 在创建存储Person类对象的unordered_set容器时，
  // 可以将hash_fun作为参数传递给该容器模板类中的Pred参数，
  // 但是，此时创建的myset容器还无法使用，
  // 因为使用的是默认的std::equal_to<key>比较规则，但此规则并不适用。
  std::unordered_set<Person, hash_fun> us;
}
}  // namespace n1

namespace n2 {
class Person {
 public:
  Person(std::string name, int age) : name(name), age(age){};
  std::string getName() const { return this->name; }
  int getAge() const { return this->age; }

 private:
  std::string name;
  int age;
};
// 创建一个可存储Person类对象的unordered_set容器，Person为自定义的类型，
// 因此默认的hash<key>哈希函数不再适用，
// 这时就需要以函数对象类的方式自定义一个哈希函数：
class hash_fun {
 public:
  // 重载()运算符时，其参数必须为const类型，且该方法也必须用const修饰：
  int operator()(const Person &A) const { return A.getAge(); }
};

// Person类对象，不支持直接使用==运算符做比较，有以下2种方式可以解决此问题：
// 1.在Person类中重载==运算符，
//   这会使得std::equal_to<key>比较规则中使用的==运算符变得合法，
//   myset容器就可以继续使用std::equal_to<key>比较规则：
bool operator==(const Person &A, const Person &B) {  // 参数必须用const
  return (A.getAge() == B.getAge());
}
void func1() {
  // 重载==运算符之后，就能以如下方式创建myset容器：
  std::unordered_set<Person, hash_fun> us2{
      {"zhangsan", 40}, {"zhangsan", 40}, {"lisi", 40}, {"lisi", 30}};
  for (auto i = us2.begin(); i != us2.end(); ++i) {
    std::cout << i->getName() << " " << i->getAge() << std::endl;
  }
  // lisi 30
  // zhangsan 40
}

// 2.以函数对象类的方式，自定义一个适用于myset容器的比较规则：
class mycmp {
 public:
  bool operator()(const Person &A, const Person &B) const {
    return (A.getName() == B.getName()) && (A.getAge() == B.getAge());
  }
};
void func2() {
  // 在mycmp规则的基础上，可以像如下这样创建myset容器：
  std::unordered_set<Person, hash_fun, mycmp> us3{
      {"zhangsan", 40}, {"zhangsan", 40}, {"lisi", 40}, {"lisi", 30}};
  for (auto i = us3.begin(); i != us3.end(); ++i) {
    std::cout << i->getName() << " " << i->getAge() << std::endl;
  }
  // lisi 30
  // lisi 40
  // zhangsan 40
}

void test() {
  func1();
  func2();
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test();
      break;
    case 1:
      n2::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}