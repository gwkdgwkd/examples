#include <cstring>
#include <iostream>
#include <type_traits>

using namespace std;
// POD全称Plain Old Data。通俗的讲，一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型。
// 普通旧数据类型，是C++中的一种数据类型概念
// Plain: 普通的
// Old: 与C兼容:
//  可以用memset()初始化，memcopy()进行复制。
//  提供了对C内存兼容，C++程序可以与C函数进行相互操作。
//  保证了静态初始化的安全有效。
// POD类型与C编程语言中使用的类型兼容，POD数据类型可以使用C库函数进行操作，也可以使用std::malloc创建，可以使用std::memmove等进行复制，
// 并且可以使用C语言库直接进行二进制形式的数据交换
// C++标准没有使用此名称定义命名的要求或概念。这是由核心语言定义的类型类别。它被包括在这里作为概念，只是为了一致性。

// 分为两类：
// 1.平凡的:
//     有平凡的构造函数
//     有平凡的拷贝构造函数
//     有平凡的移动构造函数
//     有平凡的拷贝赋值运算符
//     有平凡的移动赋值运算符
//     有平凡的析构函数
//     不能包含虚函数
//     不能包含虚基类

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

// 2.标准布局的:
//     所有非静态成员有相同的访问权限
//     继承树中最多只能有一个类有非静态数据成员
//     子类的第一个非静态成员不可以是基类类型
//     没有虚函数
//     没有虚基类
//     所有非静态成员都符合标准布局类型

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

  return 0;
}