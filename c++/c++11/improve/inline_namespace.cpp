#include <iostream>

using namespace std;

// 命名空间是为了实际工程而生的东西，工程大了，项目之间的命名冲突会经常碰到，命名空间就是为了解决这一问题。
// 内联命名空间将子命名空间的名字导入到父命名空间，这样就可以在其它子命名空间使用父命名空间直接调用。一方面是简化使用过程，另一方面可以实现功能函数版本迭代。
// 内联命名空间使用inline关键词前缀:
//  内联命名空间兼具普通命名空间的使用特点。
//  内联命名空间可以将自身名字导入父命名空间，其它子命名空间可以通过父命名空间进行调用。
//  名字访问是命名空间加标识符。
//  不同命名空间内名字可以相同，不管他们是否是同类型。

// inline 命名空间兼具普通命名空间特点。
// inline 命名空间可以将自己的名字导入到父空间。
// inline 命名空间可以可以实现功能的版本迭代。

inline namespace test_1 {
namespace test_2 {
int num = 1;
}
inline namespace test_3 {
enum a {
  one = 1,
  two,
};
enum num {
  three = 3,
  four,
};
void print(string& s) { cout << s << endl; }
}
inline namespace test_4 { int num = 2; }

namespace test_5 {
int t = a::one;
void print() {
  string s("ttttt");
  test_1::print(s);
  test_1::test_3::print(s);
}
}
}

// 功能函数版本迭代:
// 导出一个child_fun_v1功能函数，这样写是可以的
inline namespace child_fun {
namespace child_fun_implement_v1 {
void child_fun_v1() { std::cout << "child_fun_v1" << std::endl; }
}
}

// 将这个函数分成默认版本与加强版本:
inline namespace child_fun1 {
inline namespace child_fun_implement_v1 {
void child_fun_v1() { std::cout << "child_fun_v1" << std::endl; }
}
namespace child_fun_implement_v2 {
void child_fun_v1() { std::cout << "child_fun_v2" << std::endl; }
}
}

// ==========

// 随着软件开发规模的扩大，类名，函数名重复的可能性也越来越大。最朴素的解决办法就是改名，这种方法在向已经存在的类库中添加代码时问题不大，
// 但是如果是将两个从未谋面的代码库结合在一起时就不再适用了。
namespace xxx1 {
class A {
 public:
  void fun(int i) { cout << "xxx1::fun(" << i << ")" << endl; }
};
}
namespace xxx2 {
class A {
 public:
  void fun(int i) { cout << "xxx2::fun(" << i << ")" << endl; }
};
}

// 如果使用using语句，可以让某个命名空间的内容释放出来，就好像它们都存在与外层命名空间一样。
using namespace xxx1;
using namespace xxx2;

// C++11引入了内联命名空间（inline namespace），特点就是不需要使用using语句就可以直接在外层命名空间使用该命名空间内部的内容，而且无需使用命名空间前缀
namespace inline_test {
inline namespace yyy {
class B1 {
 public:
  int iv;
};
}
namespace yyy {
class B2 {
 public:
  double dv;
};
}
// 内联命名空间声明之后，就可以在外层命名空间不适用前缀而直接使用它们了:
// 处在yyy的外层，所以可以直接使用B1和B2
void test() {
  B1 b1;
  b1.iv = 5;
  B2 b2;
  b2.dv = 2;
}
}
// 处在yyy更外层，这时也只是需要使用外层命名空间inline_test前缀即可。看起来inline_namespace就像不存在一样
void test2() {
  inline_test::B1 b1;
  b1.iv = 5;
  inline_test::B2 b2;
  b2.dv = 2;
}
// 两处声明的命名空间同名，它们同属一个命名空间。这是C++命名空间从来就有的特性。
// 第一次使用了inline关键字，这叫显式内联；第二次没有使用inline关键字，但由于第一次已经声明了inline，声明的还是内联命名空间。这种情况称为隐式内联。

// 内联命名空间就像不存在一样，那么就产生了一个严肃的问题：它有什么用？
class AA1 {
 public:
  void fun1() { cout << "AA1 fun1" << endl; }
};
class BB1 {
 public:
  void fun1() { cout << "BB1 fun1" << endl; }
};

// 假设我们队类库进行了升级，同时又希望：
//  使用者代码不受影响，除非使用者自己想改。
//  可以自由使用新类库的功能
//  如果有需要仍然可以使用原来的类库
// 解决方法当然是使用内联命名空间。首先是对类库进行处理：
namespace ver1 {
class AA2 {
 public:
  void fun1() { cout << "ver1 AA2 fun1" << endl; }
};
}
namespace ver1 {
class BB2 {
 public:
  void fun1() { cout << "ver1 BB2 fun1" << endl; }
};
}

inline namespace ver2 {
class AA2 {
 public:
  void fun1() { cout << "ver2 AA2 fun1" << endl; }
  void fun2() { cout << "ver2 AA2 fun2" << endl; }
};
}
namespace ver2 {
class BB2 {
 public:
  void fun1() { cout << "ver2 BB2 fun1" << endl; }
};

class CC2 {
 public:
  void fun3() { cout << "ver2 CC2 fun3" << endl; }
};
}

// 还有一点很重要：由于隐式内联语法的存在，将来出现ver3的时候，只要将唯一的一个inline关键字移动到第一次出现的ver3定义之前就可以了！

int main() {
  cout << test_1::test_2::num << endl;         // 1
  cout << test_1::test_3::a::one << endl;      // 1
  cout << test_1::test_3::num::three << endl;  // 3
  cout << test_1::test_4::num << endl;         // 2
  cout << test_1::test_5::t << endl;           // 1
  string s("sssss");
  test_1::print(s);          // sssss
  test_1::test_3::print(s);  // sssss
  test_5::print();
  // ttttt
  // ttttt

  child_fun::child_fun_implement_v1::child_fun_v1();  // child_fun_v1

  // 要是加强版本的程序，只需把调用child_fun::child_fun_implement_v1::child_fun_v1()的地方全部替换成child_fun::child_fun_implement_v2::child_fun_v1()
  child_fun1::child_fun_v1();  // child_fun_v1, 默认版本
  child_fun1::child_fun_implement_v2::child_fun_v1();  // child_fun_v2, 加强版本

  xxx1::A a1;
  a1.fun(1);  // xxx1::fun(1)
  xxx2::A a2;
  a2.fun(2);  // xxx2::fun(2)

  // 这种方法(使用using语句)的一个弊端就是有可能发生类名重复的情况:
  // A a3;  // reference to ‘A’ is ambiguous

  AA1 aa1;
  aa1.fun1();  // AA1 fun1
  BB1 bb1;
  bb1.fun1();  // BB1 fun1

  AA2 aa2;
  aa2.fun1();  // ver2 AA2 fun1
  aa2.fun2();  // ver2 AA2 fun2
  BB2 bb2;
  bb2.fun1();  // ver2 BB2 fun1
  CC2 cc2;
  cc2.fun3();  // ver2 CC2 fun3
  // 使用最新类库的时候，就好像没有定义过命名空间一样；如果实在是需要原来的类库，可以通用版本前缀加类名的方式。
  ver1::AA2 aa3;
  aa3.fun1();  // ver1 AA2 fun1
  ver1::BB2 bb3;
  bb3.fun1();  // ver1 BB2 fun1

  return 0;
}