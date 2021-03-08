#include <iostream>
#include <set>

using namespace std;

// 无论关联式容器中存储的是基础类型（如int、double、float等）数据，还是自定义的结构体变量或类对象（包括string类），都可以使用函数对象的方式为该容器自定义排序规则。

// 在STL标准库中，包含几个可供关联式容器使用的排序规则:
//  std::less<T>    	    底层采用<运算符实现升序排序，各关联式容器默认采用的排序规则。
//  std::greater<T> 	    底层采用>运算符实现降序排序，同样适用于各个关联式容器。
//  std::less_equal<T> 	    底层采用<=运算符实现升序排序，多用于multimap和multiset容器。
//  std::greater_equal<T> 	底层采用>=运算符实现降序排序，多用于multimap和multiset容器
// 底层也是采用函数对象的方式实现的。以std::less<T>为例，其底层实现为：
// template <typename T>
// struct less {
//   bool operator()(const T &_lhs, const T &_rhs) const { return _lhs < _rhs; }
// }

// 其定义了一个函数对象类，并在其重载()运算符的方法中自定义了新的排序规则，即按照字符串的长度做升序排序。
class cmp1 {  // 函数对象类cmp也可以使用struct关键字创建
 public:
  // 重载()运算符
  bool operator()(const string &a, const string &b) {
    // 按照字符串的长度，做升序排序(即存储的字符串从短到长)
    return (a.length() < b.length());
  }
};
// 在定义函数对象类时，也可以将其定义为模板类
template <typename T>
class cmp2 {
 public:
  bool operator()(const T &a, const T &b) { return a <= b; }
};
// 此方式必须保证T类型元素可以直接使用关系运算符（比如这里的<运算符）做比较。

// 当关联式容器中存储的元素类型为结构体指针变量或者类的指针对象时，只能使用函数对象的方式自定义排序规则，此方法不再适用。
// 当关联式容器中存储的数据类型为自定义的结构体变量或者类对象时，通过对现有排序规则中所用的关系运算符进行重载，也能实现自定义排序规则的目的。
class myString1 {
 public:
  myString1(string tempStr) : str(tempStr){};
  string getStr() const;

 private:
  string str;
};
string myString1::getStr() const { return this->str; }
bool operator<(const myString1 &stra, const myString1 &strb) {
  return stra.getStr().length() > strb.getStr().length();
}

// 上面程序以全局函数的形式实现对<运算符的重载，还可以使用成员函数或者友元函数的形式实现。
// 其中，当以成员函数的方式重载<运算符时，该成员函数必须声明为const类型，且参数也必须为const类型：
class myString2 {
 public:
  myString2(string tempStr) : str(tempStr){};
  string getStr() const;
  // 至于参数的传值方式是采用按引用传递还是按值传递，都可以（建议采用按引用传递，效率更高）。
  bool operator<(const myString2 &tempStr) const {
    return this->str.length() < tempStr.str.length();
  }

 private:
  string str;
};
string myString2::getStr() const { return this->str; }

// 如果以友元函数的方式重载<运算符时，要求参数必须使用const修饰：
class myString3 {
 public:
  myString3(string tempStr) : str(tempStr){};
  string getStr() const;
  friend bool operator<(const myString3 &a, const myString3 &b);

 private:
  string str;
};
string myString3::getStr() const { return this->str; }
bool operator<(const myString3 &stra, const myString3 &strb) {
  return stra.str.length() == strb.str.length();
}

// 自定义排序规则的方法并不仅仅适用于set容器，其它关联式容器（map、multimap、multiset）也同样适用

int main() {
  // 通过将函数对象类的类名cmp通过set类模板的第2个参数传递给myset容器，该容器内部排序数据的规则，就改为了以字符串的长度为标准做升序排序。
  std::set<string, cmp1> myset1{"A", "BB", "CCC", "EEEE", "DDDD", "FFFFF"};
  for (auto iter = myset1.begin(); iter != myset1.end(); ++iter) {
    cout << *iter << endl;
  }
  // A
  // BB
  // CCC
  // EEEE
  // FFFFF
  // 此程序中创建的myset容器，由于是以字符串的长度为准进行排序，因此其无法存储相同长度的多个字符串。
  std::set<string, cmp2<string>> myset2{"A",    "BB",   "CCC",
                                        "EEEE", "DDDD", "FFFFF"};
  for (auto iter = myset2.begin(); iter != myset2.end(); ++iter) {
    cout << *iter << endl;
  }
  // A
  // BB
  // CCC
  // DDDD
  // EEEE
  // FFFFF

  std::set<myString1> myset3;
  myset3.emplace("A");
  myset3.emplace("AA");
  myset3.emplace("AAA");
  myset3.emplace("AAAA");
  for (auto iter = myset3.begin(); iter != myset3.end(); ++iter) {
    myString1 mystr = *iter;
    cout << mystr.getStr() << endl;
  }
  // AAAA
  // AAA
  // AA
  // A

  std::set<myString2> myset4;
  myset4.emplace("A");
  myset4.emplace("AA");
  myset4.emplace("AAA");
  myset4.emplace("AAAA");
  for (auto iter = myset4.begin(); iter != myset4.end(); ++iter) {
    myString2 mystr = *iter;
    cout << mystr.getStr() << endl;
  }
  // A
  // AA
  // AAA
  // AAAA

  std::set<myString3> myset5;
  myset5.emplace("AAAAA");
  myset5.emplace("AABCD");
  myset5.emplace("A");
  myset5.emplace("ABCDE");
  myset5.emplace("ABCDEFG");
  for (auto iter = myset5.begin(); iter != myset5.end(); ++iter) {
    myString3 mystr = *iter;
    cout << mystr.getStr() << endl;
  }
  // ABCDE
  // AABCD
  // AAAAA

  return 0;
}