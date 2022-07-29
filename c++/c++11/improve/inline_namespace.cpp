#include <iostream>

// 命名空间是为了实际工程而生的东西，工程大了，
// 项目之间的命名冲突会经常碰到，命名空间就是为了解决这一问题。
// 内联命名空间将子命名空间的名字导入到父命名空间，
// 这样就可以在其它子命名空间使用父命名空间直接调用。
// 一方面是简化使用过程，另一方面可以实现功能函数版本迭代。

// 内联命名空间使用inline关键词前缀:
// 1.内联命名空间兼具普通命名空间的使用特点。
// 2.内联命名空间可以将自身名字导入父命名空间，
//   其它子命名空间可以通过父命名空间进行调用。
// 3.名字访问是命名空间加标识符。
// 4.不同命名空间内名字可以相同，不管他们是否是同类型。
// 5.命名空间可以可以实现功能的版本迭代。

namespace n1 {
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
void print(std::string& s) { std::cout << s << std::endl; }
}  // namespace test_3

inline namespace test_4 {
int num = 2;
}

namespace test_5 {
int t = a::one;
void print() {
  std::string s("ttttt");
  test_1::print(s);
  test_1::test_3::print(s);
}
}  // namespace test_5
}  // namespace test_1

void testN1() {
  std::cout << test_1::test_2::num << std::endl;         // 1
  std::cout << test_1::test_3::a::one << std::endl;      // 1
  std::cout << test_1::test_3::num::three << std::endl;  // 3
  std::cout << test_1::test_4::num << std::endl;         // 2
  std::cout << test_1::test_5::t << std::endl;           // 1
  std::string s("sssss");
  test_1::print(s);          // sssss
  test_1::test_3::print(s);  // sssss
  test_5::print();
  // ttttt
  // ttttt
}
}  // namespace n1

namespace n2 {
// 功能函数版本迭代：
// 导出一个child_fun_v1功能函数，这样写是可以的
inline namespace child_fun {
namespace child_fun_implement_v1 {
void child_fun_v1() { std::cout << "child_fun_v1" << std::endl; }
}  // namespace child_fun_implement_v1
}  // namespace child_fun

// 将这个函数分成默认版本与加强版本:
inline namespace child_fun1 {
inline namespace child_fun_implement_v1 {
void child_fun_v1() { std::cout << "child_fun_v1" << std::endl; }
}  // namespace child_fun_implement_v1
namespace child_fun_implement_v2 {
void child_fun_v1() { std::cout << "child_fun_v2" << std::endl; }
}  // namespace child_fun_implement_v2
}  // namespace child_fun1

void testN2() {
  child_fun::child_fun_implement_v1::child_fun_v1();  // child_fun_v1

  // 要是加强版本的程序，只需把调用child_fun::child_fun_implement_v1::child_fun_v1()
  // 的地方全部替换成child_fun::child_fun_implement_v2::child_fun_v1()
  child_fun1::child_fun_v1();  // child_fun_v1,默认版本
  child_fun1::child_fun_implement_v2::child_fun_v1();  // child_fun_v2,加强版本
}
}  // namespace n2

namespace n3 {
// 随着软件开发规模的扩大，类名，函数名重复的可能性也越来越大。
// 最朴素的解决办法就是改名，这种方法在向已经存在的类库中添加代码时问题不大，
// 但是如果是将两个从未谋面的代码库结合在一起时就不再适用了。
namespace x1 {
class A {
 public:
  void fun(int i) { std::cout << "x1::fun(" << i << ")" << std::endl; }
};
}  // namespace x1
namespace x2 {
class A {
 public:
  void fun(int i) { std::cout << "x2::fun(" << i << ")" << std::endl; }
};
}  // namespace x2

void func1() {
  x1::A a1;
  a1.fun(1);  // x1::fun(1)
  x2::A a2;
  a2.fun(2);  // x2::fun(2)
}

// 如果使用using语句，可以让某个命名空间的内容释放出来，
// 就好像它们都存在与外层命名空间一样。
void func2() {
  using namespace x1;
  A a;
  a.fun(1);  // x1::fun(1)
}

void func3() {
  using namespace x2;
  A a;
  a.fun(1);  // x2::fun(1)
}

void func4() {
  using namespace x1;
  using namespace x2;
  // A a;  // reference to ‘A’ is ambiguous
}

void testN3() {
  func1();
  func2();
  func3();
}
}  // namespace n3

namespace n4 {
// 内联命名空间特点就是不需要使用using就可以直接在外层命
// 名空间使用该命名空间内的内容，而且无需使用前缀。
namespace inline_test {
// 第一次使用了inline关键字，这叫显式内联；
inline namespace yyy {
class B1 {
 public:
  int iv;
};
}  // namespace yyy

// 第二次没有使用inline关键字，但由于第一次已经声明了inline，
// 声明的还是内联命名空间，这种情况称为隐式内联。
// 两处声明的命名空间同名，它们同属一个命名空间，这是C++命名空间从来就有的特性。
namespace yyy {
class B2 {
 public:
  double dv;
};
}  // namespace yyy

// 内联命名空间声明之后，就可以在外层命名空间不适用前缀而直接使用它们了:
// 处在yyy的外层，所以可以直接使用B1和B2
void func1() {
  B1 b1;
  b1.iv = 1;
  std::cout << b1.iv << std::endl;  // 1

  B2 b2;
  b2.dv = 2;
  std::cout << b2.dv << std::endl;  // 2
}
}  // namespace inline_test

// 处在yyy更外层，这时也只是需要使用外层命名空间inline_test前缀即可，
// 看起来inline_namespace就像不存在一样
void func2() {
  inline_test::B1 b1;
  b1.iv = 5;
  std::cout << b1.iv << std::endl;  // 5

  inline_test::B2 b2;
  b2.dv = 6;
  std::cout << b2.dv << std::endl;  // 6
}

void testN4() {
  inline_test::func1();
  func2();
}
}  // namespace n4

namespace n5 {
// 内联命名空间就像不存在一样，那么就产生了一个严肃的问题：它有什么用？
class A1 {
 public:
  void fun1() { std::cout << "A1 fun1" << std::endl; }
};
class B1 {
 public:
  void fun1() { std::cout << "B1 fun1" << std::endl; }
};

// 假设对类库进行了升级，同时又希望：
// 1.使用者代码不受影响，除非使用者自己想改；
// 2.可以自由使用新类库的功能；
// 3.如果有需要仍然可以使用原来的类库。
// 解决方法当然是使用内联命名空间。首先是对类库进行处理：
namespace ver1 {
class A2 {
 public:
  void fun1() { std::cout << "ver1 A2 fun1" << std::endl; }
};
}  // namespace ver1
namespace ver1 {
class B2 {
 public:
  void fun1() { std::cout << "ver1 B2 fun1" << std::endl; }
};
}  // namespace ver1

inline namespace ver2 {
class A2 {
 public:
  void fun1() { std::cout << "ver2 A2 fun1" << std::endl; }
  void fun2() { std::cout << "ver2 A2 fun2" << std::endl; }
};
}  // namespace ver2
namespace ver2 {  // 不用写inline
class B2 {
 public:
  void fun1() { std::cout << "ver2 B2 fun1" << std::endl; }
};

class C2 {
 public:
  void fun3() { std::cout << "ver2 C2 fun3" << std::endl; }
};
}  // namespace ver2

// 由于隐式内联语法的存在，将来出现ver3的时候，
// 只要将唯一的一个inline关键字移动到第一次出现的ver3定义之前就可以了！

void testN5() {
  A1 a1;
  a1.fun1();  // A1 fun1
  B1 b1;
  b1.fun1();  // B1 fun1

  // 使用最新类库的时候，就好像没有定义过命名空间一样
  // 客户的代码不用变，以及可以使用最新版本的ver2的功能了
  A2 a2;
  a2.fun1();  // ver2 A2 fun1
  a2.fun2();  // ver2 A2 fun2
  B2 b2;
  b2.fun1();  // ver2 B2 fun1
  C2 c2;
  c2.fun3();  // ver2 C2 fun3

  // 如果实在是需要原来的类库，可以通用版本前缀加类名的方式
  ver1::A2 a3;
  a3.fun1();  // ver1 A2 fun1
  ver1::B2 b3;
  b3.fun1();  // ver1 B2 fun1
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::testN1();
      break;
    case 1:
      n2::testN2();
      break;
    case 2:
      n3::testN3();
      break;
    case 3:
      n4::testN4();
      break;
    case 4:
      n5::testN5();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}