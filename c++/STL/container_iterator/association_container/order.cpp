#include <iostream>
#include <set>

// 无论关联式容器中存储的是基础类型（如int、double、float等）数据，
// 还是自定义的结构体变量或类对象（包括string类），
// 都可以使用函数对象的方式为该容器自定义排序规则。

// 在STL标准库中，包含几个可供关联式容器使用的排序规则：
// 1.std::less<T>
//   底层采用<运算符实现升序排序，各关联式容器默认采用的排序规则。
// 2.std::greater<T>
//   底层采用>运算符实现降序排序，同样适用于各个关联式容器。
// 3.std::less_equal<T>
//   底层采用<=运算符实现升序排序，多用于multimap和multiset容器。
// 4.std::greater_equal<T>
//   底层采用>=运算符实现降序排序，多用于multimap和multiset容器。
// 排序规则底层也是采用函数对象的方式实现的，
// 以std::less<T>为例，其底层实现为：
// template <typename T>
// struct less {
//   bool operator()(const T &_lhs, const T &_rhs) const{
//     return _lhs < _rhs;
//   }
// }

namespace n1 {
// 定义一个函数对象类，并在其重载()运算符的方法中自定义了新的排序规则，
// 按照字符串的长度做升序排序：
class cmp {  // 函数对象类cmp也可以使用struct关键字创建
 public:
  // 重载()运算符：
  bool operator()(const std::string &a, const std::string &b) {
    // 按照字符串的长度，做升序排序：
    return (a.length() < b.length());
  }
};

void test() {
  // 通过将函数对象类的类名cmp通过set类模板的第2个参数传递给myset容器，
  // 该容器内部排序数据的规则，就改为了以字符串的长度为标准做升序排序。
  std::set<std::string, cmp> s{"A", "BB", "FFF", "EEEE", "DDDD", "CCCCC"};
  for (auto iter = s.begin(); iter != s.end(); ++iter) {
    std::cout << *iter << std::endl;
  }
  // A
  // BB
  // FFF
  // EEEE
  // CCCCC

  // 由于是以字符串的长度为准进行排序，
  // 因此其无法存储相同长度的多个字符串，没有DDDD
}
}  // namespace n1

namespace n2 {
// 在定义函数对象类时，也可以将其定义为模板类：
template <typename T>
class cmp {
 public:
  bool operator()(const T &a, const T &b) { return a <= b; }
};
// 此方式必须保证T类型元素可以直接使用关系运算符做比较。
// 当关联式容器中存储的元素类型为结构体指针变量或者类的指针对象时，
// 只能使用函数对象的方式自定义排序规则，此方法不再适用。

void test() {
  std::set<std::string, cmp<std::string>> s{"A",    "BB",   "FFF",
                                            "EEEE", "DDDD", "CCCCC"};
  for (auto iter = s.begin(); iter != s.end(); ++iter) {
    std::cout << *iter << std::endl;
  }
  // A
  // BB
  // CCCCC
  // DDDD
  // EEEE
  // FFF
}
}  // namespace n2

namespace n3 {
// 当关联式容器中存储的数据类型为自定义的结构体变量或者类对象时，
// 通过对现有排序规则中所用的关系运算符进行重载，也能实现自定义排序规则的目的。
class myString {
 public:
  myString(std::string tempStr) : str(tempStr){};
  std::string getStr() const;

 private:
  std::string str;
};
std::string myString::getStr() const { return this->str; }
bool operator<(const myString &stra, const myString &strb) {
  return stra.getStr().length() > strb.getStr().length();
}

void test() {
  std::set<myString> s;
  s.emplace("A");
  s.emplace("AA");
  s.emplace("AAA");
  s.emplace("AAAA");
  for (auto iter = s.begin(); iter != s.end(); ++iter) {
    myString mystr = *iter;
    std::cout << mystr.getStr() << std::endl;
  }

  // AAAA
  // AAA
  // AA
  // A
}
}  // namespace n3

namespace n4 {
// n3以全局函数的形式实现对<运算符的重载，
// 还可以使用成员函数或者友元函数的形式实现。
// 其中，当以成员函数的方式重载<运算符时，
// 该成员函数必须声明为const类型，且参数也必须为const类型：
class myString {
 public:
  myString(std::string tempStr) : str(tempStr){};
  std::string getStr() const;
  // 至于参数的传值方式是采用按引用传递还是按值传递，
  // 都可以，建议采用按引用传递，效率更高。
  bool operator<(const myString &tempStr) const {
    return this->str.length() < tempStr.str.length();
  }

 private:
  std::string str;
};
std::string myString::getStr() const { return this->str; }

void test() {
  std::set<myString> s;
  s.emplace("A");
  s.emplace("AA");
  s.emplace("AAA");
  s.emplace("AAAA");
  for (auto iter = s.begin(); iter != s.end(); ++iter) {
    myString mystr = *iter;
    std::cout << mystr.getStr() << std::endl;
  }

  // A
  // AA
  // AAA
  // AAAA
}
}  // namespace n4

namespace n5 {
// 如果以友元函数的方式重载<运算符时，要求参数必须使用const修饰：
class myString {
 public:
  myString(std::string tempStr) : str(tempStr){};
  std::string getStr() const;
  friend bool operator<(const myString &a, const myString &b);

 private:
  std::string str;
};
std::string myString::getStr() const { return this->str; }
bool operator<(const myString &stra, const myString &strb) {
  return stra.str.length() == strb.str.length();
}

void test() {
  std::set<myString> s;
  s.emplace("AAAAA");
  s.emplace("AABCD");
  s.emplace("A");
  s.emplace("ABCDE");
  s.emplace("ABCDEFG");
  for (auto iter = s.begin(); iter != s.end(); ++iter) {
    myString mystr = *iter;
    std::cout << mystr.getStr() << std::endl;
  }

  // ABCDE
  // AABCD
  // AAAAA
}
}  // namespace n5

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
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
    case 2:
      n3::test();
      break;
    case 3:
      n4::test();
      break;
    case 4:
      n5::test();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}