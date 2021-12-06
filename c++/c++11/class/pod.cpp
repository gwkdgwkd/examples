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

using namespace std;
// POD全称Plain Old Data。通俗的讲，一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型。
// 普通旧数据类型，是C++中的一种数据类型概念
// Plain:普通的
// Old:与C兼容:
//  可以用memset()初始化，memcopy()进行复制。
//  提供了对C内存兼容，C++程序可以与C函数进行相互操作。
//  保证了静态初始化的安全有效。
// POD类型与C编程语言中使用的类型兼容，POD数据类型可以使用C库函数进行操作，也可以使用std::malloc创建，
// 可以使用std::memmove等进行复制，并且可以使用C语言库直接进行二进制形式的数据交换
// C++标准没有使用此名称定义命名的要求或概念。这是由核心语言定义的类型类别。它被包括在这里作为概念，只是为了一致性。

// 分为两类：
// 1.平凡的(trivial):
//     有平凡的构造函数
//     有平凡的拷贝构造函数
//     有平凡的移动构造函数
//     有平凡的拷贝赋值运算符
//     有平凡的移动赋值运算符
//     有平凡的析构函数
//     不能包含虚函数
//     不能包含虚基类
//   好处：
//   支持静态初始化，就是C-style array, struct和array<int>使用{}的初始化方式。注：vector等的初始化列表不属于静态初始化。
//   是拷贝不变的（trivially copyable）的class，可以使用memcpy，memmove不改变它的语义。
class A {
  A() {}
};
class B {
  B(B &) {}
};
class C {
  C(C &&) {}
};
class D {
  D operator=(D &) {}
};
class E {
  E operator=(E &&) {}
};
class F {
  ~F() {}
};
class G {
  virtual void foo() = 0;
};
class H : G {};
class I {};

// 2.标准布局的(standard_layout):
//     所有非静态成员有相同的访问权限
//     继承树中最多只能有一个类有非静态数据成员
//     子类的第一个非静态成员不可以是基类类型
//     没有虚函数
//     没有虚基类
//     所有非静态成员都符合标准布局类型
//  好处：
//  它对语言间交互很有用，这是因为C++ standard-layout类型和C中struct或union类型有相同的内存布局。
//  但是，它不满足trival的两个好处。
class A1 {
 private:
  int a;

 public:
  int b;
};
class B11 {
  static int x1;
};
class B12 {
  int x2;
};
class B1 : B11, B12 {
  int x;
};
class C11 {};
class C1 : C11 {
  C11 c;
};
class D1 {
  virtual void foo() = 0;
};
class E1 : D1 {};
class F1 {
  A1 x;
};

// 当一个数据类型满足了”平凡的定义“和”标准布局“，我们则认为它是一个POD数据。可以通过std::is_pod来判断一个类型是否为POD类型。
// 一个POD类型是可以进行二进制拷贝的:
class A2 {
 public:
  int x;
  double y;
};

int main() {
  cout << is_trivial<A>::value << endl;  // 0 有不平凡的构造函数
  cout << is_trivial<B>::value << endl;  // 0 有不平凡的拷贝构造函数
  cout << is_trivial<C>::value << endl;  // 0 有不平凡的拷贝赋值运算符
  cout << is_trivial<D>::value << endl;  // 0 有不平凡的拷贝赋值运算符
  cout << is_trivial<E>::value << endl;  // 0 有不平凡的移动赋值运算符
  cout << is_trivial<F>::value << endl;  // 0 有不平凡的析构函数
  cout << is_trivial<G>::value << endl;  // 0 有虚函数
  cout << is_trivial<H>::value << endl;  // 0 有虚基类
  cout << is_trivial<I>::value << endl;  // 1 平凡的类

  cout << is_standard_layout<A1>::value;
  cout << endl;  // 0 违反定义1。成员a和b具有不同的访问权限
  cout << is_standard_layout<B1>::value;
  cout << endl;  // 0 违反定义2。继承树有两个(含)以上的类有非静态成员
  cout << is_standard_layout<C1>::value;
  cout << endl;  // 0 违反定义3。第一个非静态成员是基类类型
  cout << is_standard_layout<D1>::value;
  cout << endl;  // 0 违反定义4。有虚函数
  cout << is_standard_layout<E1>::value;
  cout << endl;  // 0 违反定义5。有虚基类
  cout << is_standard_layout<F1>::value;
  cout << endl;  // 0 违反定义6。非静态成员x不符合标准布局类型

  // 可以看到，对一个POD类型进行二进制拷贝后，数据都成功的迁移过来了。
  if (is_pod<A2>::value) {  // C++中判断数据类型是否为POD的函数：is_pod(C++11)
    cout << "before" << endl;
    A2 a;
    a.x = 8;
    a.y = 10.5;
    cout << a.x << endl;  // 8
    cout << a.y << endl;  // 10.5

    size_t size = sizeof(a);
    char *p = new char[size];
    memcpy(p, &a, size);
    A2 *pA = (A2 *)p;

    cout << "after" << endl;
    cout << pA->x << endl;  // 8
    cout << pA->y << endl;  // 10.5

    delete p;
  }

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

  // 结论：
  // 1.内置类型(如: int, float)，array<int>是POD类型
  // 2.POD类型是递归的，array<string>就不是POD类型，因为string不是
  // 3.所有测试类型包括：pair(tuple),vector,list,deque,set,map,unordered_set,unordered_map,shared_ptr,unique_ptr
  //   都满足is_standard_layout但是不满足is_trivial，因此也不满足is_pod类型。
  // 4.unique_ptr具有和普通指针一样大小，大多数时候应该使用它(当然还有其它原因：如unique性能更好等等)，而不是shared_ptr。

  return 0;
}