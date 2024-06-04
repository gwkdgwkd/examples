#include <iostream>
#include <string>
#include <vector>

// decltype是C++11新增的一个关键字，它和auto的功能一样，
// 都用来在编译时期进行自动类型推导，是declare type的缩写。
// 既然已经有了auto关键字，为什么还需要decltype关键字呢？
// 因为auto并不适用于所有的自动类型推导场景，在某些情况下auto非常不方便，
// 甚至压根无法使用，所以decltype关键字也被引入到C++11中。

// auto和decltype关键字都可以自动推导出变量的类型，但它们是有区别的：
// auto varname = value;
// decltype(exp) varname = value;
// 其中，varname表示变量名，value表示赋给变量的值，exp表示一个表达式。

// decltype推导过程是在编译期完成的，并且不会真正计算表达式的值。

#include <cxxabi.h>
const std::string GetClearName(const char *name) {
  int status = -1;
  char *clearName = abi::__cxa_demangle(name, NULL, NULL, &status);
  const char *const demangledName = (status == 0) ? clearName : name;
  std::string ret_val(demangledName);
  free(clearName);
  return ret_val;
}

template <typename T>
void printType(T val) {
  std::cout << typeid(T).name() << " " << GetClearName(typeid(T).name())
            << std::endl;
}

namespace n1 {
// 当使用decltype(exp)获取类型时，根据以下三条规则得出结果：
// 1.如果exp是以下3个中的一个：
//   a.不被括号()包围的表达式；
//   b.一个类成员访问表达式；
//   c.一个单独的变量；
//   那么decltype(exp)的类型就和exp一致，这是最普遍最常见的情况；
// 2.如果exp是函数调用，那么decltype(exp)的类型就和函数返回值的类型一致；
// 3.如果exp是左值，或者被括号()包围，那么decltype(exp)的类型是exp的引用。

namespace test1 {
// decltype能够根据变量、字面量、带有运算符的表达式推导出变量的类型

void func1() {
  int i1 = 0;
  decltype(i1) i2 = 1;      // i2被推导成了int
  decltype(10.8) d1 = 5.5;  // d1被推导成了double

  decltype(d1 + 100) d2;  // d2被推导成了double，没有初始化

  printType(i2);  // i int
  printType(d1);  // d double
  printType(d2);  // d double
}

// exp是一个普通表达式：
class Student {
 public:
  static int total;
  std::string name;
  int age;
  float scores;
};
int Student::total = 0;
void func2() {
  int i1 = 0;
  const int &r1 = i1;

  decltype(i1) i2 = i1;  // i1为int类型，i2被推导为int类型
  i2 = 5;
  decltype(r1) r2 = i1;  // r1为const int&类型，r2被推导为const int&类型
  // r2 = 5;  // assignment of read-only reference ‘r2’

  Student stu;
  decltype(Student::total) i3 = 0;  // i3被推导为int类型
  decltype(stu.name) url = "http";  // url被推导为string

  printType(r2);  // i int
  printType(i2);  // i int
  printType(i3);  // i int
  printType(url);
  // NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
  // std::__cxx11::basic_string<char,
  // std::char_traits<char>, std::allocator<char> >
}

void func() {
  func1();
  func2();
}
}  // namespace test1

namespace test2 {
// exp为函数调用

void test();                           // 返回值为void
int &func_int_r(int, char);            // 返回值为int&
int &&func_int_rr(void);               // 返回值为int&&
int func_int(double);                  // 返回值为int
const int &fun_cint_r(int, int, int);  // 返回值为const int&
const int &&func_cint_rr(void);        // 返回值为const int&&
void func() {
  int i = 100;
  decltype(func_int_r(100, 'A')) i1 = i;  // i1的类型为int&
  decltype(func_int_rr()) i2 = 0;         // i2的类型为int&&
  decltype(func_int(10.5)) i3 = 0;        // i3的类型为int
  decltype(fun_cint_r(1, 2, 3)) i4 = i;   // i4的类型为const int&
  decltype(func_cint_rr()) i5 = 0;        // i5的类型为const int&&

  // 原则上讲，exp就是一个普通的表达式，它可以是任意复杂的形式，
  // 但是必须要保证exp的结果是有类型的，不能是void；
  // 当exp调用一个返回值类型为void的函数时，exp的结果也是void类型，编译错误。
  // decltype(test());
  // declaration does not declare anything [-fpermissive]
}
}  // namespace test2

namespace test3 {
// exp是左值，或者被()包围

class Base {
 public:
  Base(){};
  int x;
};
void func() {
  const Base obj;

  // 带括号的表达式：
  int i = 5;
  // obj.x为类的成员访问表达式，符合推导规则一，i1为int：
  decltype(obj.x) i1 = i;
  // obj.x带有括号，符合推导规则三，i2的类型为int&：
  decltype((obj.x)) i2 = i;
  std::cout << i1 << "," << i2 << std::endl;  // 5,5
  i = 6;
  std::cout << i1 << "," << i2 << std::endl;  // 5,6

  // 加法表达式：
  int n = 0, m = 0;
  // n+m得到一个右值，符合规则一，所以推导结果为int：
  decltype(n + m) i3 = i;
  // n=n+m得到一个左值，符号规则三，结果为int&：
  decltype(n = n + m) i4 = i;
  // 单独的变量，不算是左值，符合规则一，推断结果为int：
  decltype(n) i5 = i;
  std::cout << i3 << "," << i4 << "," << i5 << std::endl;  // 6,6,6
  i = 7;
  std::cout << i3 << "," << i4 << "," << i5 << std::endl;  // 6,7,6
}
}  // namespace test3
}  // namespace n1

namespace n2 {
// decltype的实际应用

namespace test1 {
// auto只能用于类的静态成员，不能用于类的非静态成员（普通成员），
// 如果想推导非静态成员的类型，这个时候就必须使用decltype了：
template <typename T>
class Base {
 public:
  void func(T &container) { m_it = container.begin(); }

 private:
  // 在C++98/03版本下只能想办法把const类型的容器用模板特化单独处理，
  // 增加了不少工作量，看起来也非常晦涩：
  typename T::iterator m_it;
};
void func() {
  std::vector<int> v1;
  Base<std::vector<int>> b1;
  b1.func(v1);

  const std::vector<int> v2;
  Base<const std::vector<int>> b2;
  // b2.func(v2);
  // T::iterator并不能包括所有的迭代器类型，
  // 当T是一个const容器时，应当使用const_iterator。
}
}  // namespace test1

namespace test2 {
template <typename T>
class Base {
 public:
  void func(T &container) { m_it = container.begin(); }

 private:
  // 有了C++11的decltype关键字，就可以直接这样写：
  decltype(T().begin()) m_it;
};
void func() {
  std::vector<int> v1;
  Base<std::vector<int>> b1;
  b1.func(v1);

  const std::vector<int> v2;
  Base<const std::vector<int>> b2;
  b2.func(v2);
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}