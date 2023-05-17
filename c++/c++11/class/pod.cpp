#include <array>
#include <cstring>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>  // 定义了对这些类型的检测
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// POD全称Plain Old Data，普通旧数据类型，是C++中的一种数据类型概念。
// 一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型：
// 1.可以用memset()初始化，memcopy()进行复制；
// 2.提供了对C内存兼容，C++程序可以与C函数进行相互操作；
// 3.保证了静态初始化的安全有效。
// POD类型与C编程语言中使用的类型兼容，POD数据类型可以使用C库函数进行操作，
// 也可以使用std::malloc创建，可以使用std::memmove等进行复制，
// 并且可以使用C语言库直接进行二进制形式的数据交换。

// 当一个数据类型满足了下面两条则认为它是一个POD数据：
// 1.平凡的定义；
// 2.标准布局。
// 可以通过std::is_pod来判断一个类型是否为POD类型。

namespace n1 {
// 平凡的(trivial)：
// 1.有平凡的构造函数；
// 2.有平凡的拷贝构造函数；
// 3.有平凡的移动构造函数；
// 4.有平凡的拷贝赋值运算符；
// 5.有平凡的移动赋值运算符；
// 6.有平凡的析构函数；
// 7.不能包含虚函数；
// 8.不能包含虚基类；
// 好处：
// 1.支持静态初始化，就是C-style array，struct和array<int>使用{}的初始化方式，
//   vector等的初始化列表不属于静态初始化；
// 2.是拷贝不变的（trivially copyable）的class，
//   可以使用memcpy，memmove不改变它的语义。

class A {  // 有不平凡的构造函数
  A() {}
};
class B {  // 有不平凡的拷贝构造函数
  B(B &) {}
};
class C {  // 有不平凡的移动构造函数
  C(C &&) {}
};
class D {  // 有不平凡的拷贝赋值运算符
  D &operator=(D &) { return *this; }
};
class E {  // 有不平凡的移动赋值运算符
  E &operator=(E &&) { return *this; }
};
class F {  // 有不平凡的析构函数
  ~F() {}
};
class G {  // 有虚函数
  virtual void foo() = 0;
};
class H : G {};          // 有虚基类
class I : virtual G {};  // 有虚继承
class J {                // 平凡的类，相关的不平凡函数都没有定义
  int i;
};

void func() {
  std::cout << std::is_trivial<A>::value << std::endl;  // 0
  std::cout << std::is_trivial<B>::value << std::endl;  // 0
  std::cout << std::is_trivial<C>::value << std::endl;  // 0
  std::cout << std::is_trivial<D>::value << std::endl;  // 0
  std::cout << std::is_trivial<E>::value << std::endl;  // 0
  std::cout << std::is_trivial<F>::value << std::endl;  // 0
  std::cout << std::is_trivial<G>::value << std::endl;  // 0
  std::cout << std::is_trivial<H>::value << std::endl;  // 0
  std::cout << std::is_trivial<I>::value << std::endl;  // 0
  std::cout << std::is_trivial<J>::value << std::endl;  // 1
}
}  // namespace n1

namespace n2 {
// 标准布局的(standard_layout)：
// 1.所有非静态成员有相同的访问权限；
// 2.继承树中最多只能有一个类有非静态数据成员；
// 3.子类的第一个非静态成员不可以是基类类型；
// 4.没有虚函数；
// 5.没有虚基类；
// 6.所有非静态成员都符合标准布局类型。
// 好处：
// 1.它对语言间交互很有用，这是因为C++ standard-layout类型，
//   和C中struct或union类型有相同的内存布局；
// 2.但是，它不满足trival的两个好处。

class A {  // 违反定义1，成员a和b具有不同的访问权限
 private:
  int a;

 public:
  int b;
};
class B1 {
  static int x1;
};
class B2 {
  int x2;
};
class B3 : B1, B2 {  // 违反定义2，继承树有两个（含）以上的类有非静态成员
  int x;
};
class C1 {};
class C2 : C1 {  // 违反定义3，第一个非静态成员是基类类型
  C1 c;
};
class D {  // 违反定义4，有虚函数
  virtual void foo() = 0;
};
class E : D {};          // 违反定义5，有虚基类
class F : virtual D {};  // 有虚继承，也违反定义5么？
class G {  // 违反定义6，非静态成员x不符合标准布局类型
  A x;
};

void func() {
  std::cout << std::is_standard_layout<A>::value << std::endl;   // 0
  std::cout << std::is_standard_layout<B3>::value << std::endl;  // 0
  std::cout << std::is_standard_layout<C2>::value << std::endl;  // 0
  std::cout << std::is_standard_layout<D>::value << std::endl;   // 0
  std::cout << std::is_standard_layout<E>::value << std::endl;   // 0
  std::cout << std::is_standard_layout<F>::value << std::endl;   // 0
  std::cout << std::is_standard_layout<G>::value << std::endl;   // 0
}
}  // namespace n2

namespace n3 {
// 一个POD类型是可以进行二进制拷贝的：
class A {
 public:
  int x;
  double y;
};

void func() {
  // C++中判断数据类型是否为POD的函数：is_pod(C++11)
  if (std::is_pod<A>::value) {
    A a;
    a.x = 8;
    a.y = 10.5;
    std::cout << a.x << std::endl;  // 8
    std::cout << a.y << std::endl;  // 10.5

    size_t size = sizeof(a);
    char *p = new char[size];
    memcpy(p, &a, size);
    A *pA = (A *)p;

    // 对一个POD类型进行二进制拷贝后，数据都成功的迁移过来了：
    std::cout << pA->x << std::endl;  // 8
    std::cout << pA->y << std::endl;  // 10.5

    delete p;
  }
}
}  // namespace n3

namespace n4 {
// 1.内置类型(如：int，float)，array<int>是POD类型；
// 2.POD类型是递归的，array<string>就不是POD类型，因为string不是；
// 3.pair(tuple)，vector，list，deque，set，map，unordered_set，
//   unordered_map，shared_ptr，unique_ptr都满足is_standard_layout，
//   但是不满足is_trivial，因此也不满足is_pod类型；
// 4.unique_ptr具有和普通指针一样大小，大多数时候应该使用它，而不是shared_ptr。

void func() {
  using namespace std;

  cout.setf(ios_base::boolalpha);
  cout << "int:" << endl;
  cout << "is_trivial:" << is_trivial<int>::value << endl;  // true
  cout << "is_standard_layout:" << is_standard_layout<int>::value
       << endl;                                     // true
  cout << "is_pod:" << is_pod<int>::value << endl;  // true

  cout << "string:" << endl;
  cout << "is_trivial:" << is_trivial<string>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<string>::value
       << endl;                                         // true
  cout << "is_pod:" << is_pod<string>::value << endl;   // false
  cout << "sizeof(string):" << sizeof(string) << endl;  // 32

  cout << "array<int,5>:" << endl;
  cout << "is_trivial:" << is_trivial<array<int, 5>>::value << endl;  // true
  cout << "is_standard_layout:" << is_standard_layout<array<int, 5>>::value
       << endl;                                               // true
  cout << "is_pod:" << is_pod<array<int, 5>>::value << endl;  // true

  cout << "array<string, 5>:" << endl;
  cout << "is_trivial:" << is_trivial<array<string, 5>>::value
       << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<array<string, 5>>::value
       << endl;                                                  // true
  cout << "is_pod:" << is_pod<array<string, 5>>::value << endl;  // false

  cout << "pair<int,int>:" << endl;
  cout << "is_trivial:" << is_trivial<pair<int, int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<pair<int, int>>::value
       << endl;                                                // true
  cout << "is_pod:" << is_pod<pair<int, int>>::value << endl;  // false

  cout << "vector<int>:" << endl;
  cout << "is_trivial:" << is_trivial<vector<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<vector<int>>::value
       << endl;                                             // true
  cout << "is_pod:" << is_pod<vector<int>>::value << endl;  // false

  cout << "vector<string>:" << endl;
  cout << "is_trivial:" << is_trivial<vector<string>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<vector<string>>::value
       << endl;                                                // true
  cout << "is_pod:" << is_pod<vector<string>>::value << endl;  // false

  cout << "deque<int>:" << endl;
  cout << "is_trivial:" << is_trivial<deque<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<deque<int>>::value
       << endl;                                            // true
  cout << "is_pod:" << is_pod<deque<int>>::value << endl;  // false

  cout << "deque<string>:" << endl;
  cout << "is_trivial:" << is_trivial<deque<string>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<deque<string>>::value
       << endl;                                               // true
  cout << "is_pod:" << is_pod<deque<string>>::value << endl;  // false

  cout << "list<int>:" << endl;
  cout << "is_trivial:" << is_trivial<list<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<list<int>>::value
       << endl;                                           // false
  cout << "is_pod:" << is_pod<list<int>>::value << endl;  // false

  cout << "list<string>:" << endl;
  cout << "is_trivial:" << is_trivial<list<string>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<list<string>>::value
       << endl;                                              // false
  cout << "is_pod:" << is_pod<list<string>>::value << endl;  // false

  cout << "vector<list<int>>:" << endl;
  cout << "is_trivial:" << is_trivial<vector<list<int>>>::value
       << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<vector<list<int>>>::value
       << endl;                                                   // true
  cout << "is_pod:" << is_pod<vector<list<int>>>::value << endl;  // false

  cout << "vector<list<string>>:" << endl;
  cout << "is_trivial:" << is_trivial<vector<list<string>>>::value
       << endl;  // false
  cout << "is_standard_layout:"
       << is_standard_layout<vector<list<string>>>::value << endl;   // true
  cout << "is_pod:" << is_pod<vector<list<string>>>::value << endl;  // false

  cout << "set<int>:" << endl;
  cout << "is_trivial:" << is_trivial<set<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<set<int>>::value
       << endl;                                          // false
  cout << "is_pod:" << is_pod<set<int>>::value << endl;  // false

  cout << "set<string>:" << endl;
  cout << "is_trivial:" << is_trivial<set<string>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<set<string>>::value
       << endl;                                             // false
  cout << "is_pod:" << is_pod<set<string>>::value << endl;  // false

  cout << "map<int, int>:" << endl;
  cout << "is_trivial:" << is_trivial<map<int, int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<map<int, int>>::value
       << endl;                                               // false
  cout << "is_pod:" << is_pod<map<int, int>>::value << endl;  // false

  cout << "map<string, string>:" << endl;
  cout << "is_trivial:" << is_trivial<map<string, string>>::value
       << endl;  // false
  cout << "is_standard_layout:"
       << is_standard_layout<map<string, string>>::value << endl;   // false
  cout << "is_pod:" << is_pod<map<string, string>>::value << endl;  // false

  cout << "unordered_set<int>:" << endl;
  cout << "is_trivial:" << is_trivial<unordered_set<int>>::value
       << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<unordered_set<int>>::value
       << endl;                                                    // true
  cout << "is_pod:" << is_pod<unordered_set<int>>::value << endl;  // false

  cout << "unordered_set<string>:" << endl;
  cout << "is_trivial:" << is_trivial<unordered_set<string>>::value
       << endl;  // false
  cout << "is_standard_layout:"
       << is_standard_layout<unordered_set<string>>::value << endl;   // true
  cout << "is_pod:" << is_pod<unordered_set<string>>::value << endl;  // false

  cout << "unordered_map<int, int>:" << endl;
  cout << "is_trivial:" << is_trivial<unordered_map<int, int>>::value
       << endl;  // false
  cout << "is_standard_layout:"
       << is_standard_layout<unordered_map<int, int>>::value << endl;   // true
  cout << "is_pod:" << is_pod<unordered_map<int, int>>::value << endl;  // false

  cout << "unordered_map<string, string>:" << endl;
  cout << "is_trivial:" << is_trivial<unordered_map<string, string>>::value
       << endl;  // false
  cout << "is_standard_layout:"
       << is_standard_layout<unordered_map<string, string>>::value
       << endl;  // true
  cout << "is_pod:" << is_pod<unordered_map<string, string>>::value
       << endl;  // false

  cout << "shared_ptr<int>:" << endl;
  cout << "is_trivial:" << is_trivial<shared_ptr<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<shared_ptr<int>>::value
       << endl;                                                 // true
  cout << "is_pod:" << is_pod<shared_ptr<int>>::value << endl;  // false
  cout << "sizeof(shared_ptr<int>):" << sizeof(shared_ptr<int>) << endl;  // 16

  cout << "unique_ptr<int>:" << endl;
  cout << "is_trivial:" << is_trivial<unique_ptr<int>>::value << endl;  // false
  cout << "is_standard_layout:" << is_standard_layout<unique_ptr<int>>::value
       << endl;                                                 // true
  cout << "is_pod:" << is_pod<unique_ptr<int>>::value << endl;  // false
  cout << "sizeof(unique_ptr<int>):" << sizeof(unique_ptr<int>) << endl;  // 8
}
}  // namespace n4

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}