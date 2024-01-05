#include <iostream>

// 条款11：优先使用delete关键字删除函数而不是private却又不实现的函数

namespace n1 {
// 有时C++会声明一些函数，如果不想被调用，需要特殊处理。

namespace test1 {
// C++98中阻止这类函数被调用的方法是声明为private，并且不定义它们：
class A {
 public:
  A() {}
  void test() {
    A a1;
    A a2(a1);  // 拷贝构造函数没有定义，可以编译，但链接出错
    a1 = a2;   // 赋值操作符没有定义，可以编译，但链接出错
  }

 private:
  A(const A&);  // 故意不实现它们，是防止成员函数或者友元调用它们
  A& operator=(const A&);  // 将函数声明为私有用来阻止用户调用它们
};
void func() {
  A a1;
  // A a2(a1);  // 由于拷贝构造函数是私有的，编译报错
  // a1 = a2;   // 由于赋值操作符是私有的，编译报错

  // ‘n1::test1::A::A(const n1::test1::A&)’ is private within this context

  // a1.test();  // 如果执行，链接错误
}
}  // namespace test1

namespace test2 {
// 在C++11中，可以用=delete标识拷贝构造函数和赋值操作符为删除的函数：
class A {
 public:
  A() {}
  void test() {
    A a1;
    // A a2(a1);  // 编译错误
    // a1 = a2;   // 编译错误
  }

  A(const A&) = delete;
  A& operator=(const A&) = delete;

  // 删除的函数声明为私有函数的区别看上去只是时尚了些，但实际的区别更多：
  // 1.删除的函数不能通过任何方式调用，即便是成员函数和友元函数都不行，
  //   都会导致编译失败，这是对C++98中直到链接才报错的行为的升级；
  // 2.删除函数被声明为公有的，而不是私有的，而编译器优先检查可访问权限，
  //   所以C++98的方式错误提示没有删除函数的错误提示更易读。
};
void func() {
  A a1;
  // A a2(a1);  // 编译错误
  // a1 = a2;   // 编译错误

  // use of deleted function ‘n1::test2::A::A(const n1::test2::A&)’
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n1

namespace n2 {
// 删除函数一个重要的优势是任何函数都可以是删除，然而仅有成员函数才可以是私有的。

namespace test1 {
bool isLucky(int number) { return number % 2; }
void func() {
  // C++继承于C，很多其他类型被隐式转换为int类型，编译没有任何问题：
  if (isLucky('a')) {
    std::cout << "is lucky" << std::endl;
  }

  if (isLucky(true)) {
    std::cout << "is lucky" << std::endl;
  }

  if (isLucky(3.5)) {
    std::cout << "is lucky" << std::endl;
  }

  // is lucky
  // is lucky
  // is lucky
}
}  // namespace test1

namespace test2 {
bool isLucky(int number) { return number % 2; }
// 如果幸运数字一定要是整数，可以阻止其他形式的调用：
bool isLucky(char) = delete;
bool isLucky(bool) = delete;
bool isLucky(double) = delete;  // 拒绝double和float类型

void func() {
  // 尽管删除函数不能被调用，但是它们仍是程序的一部分，重载解析时仍会考虑，所以报错：
  // isLucky('a');
  // isLucky(true);
  // isLucky(3.5);
  // isLucky(4.6f);

  if (isLucky(1)) {
    std::cout << "is lucky" << std::endl;
  }

  // is lucky
}
}  // namespace test2
void func() {
  test1::func();
  test2::func();
}
}  // namespace n2

namespace n3 {
// 还有一个删除函数可以完成而私有成员函数无法完成的技巧：
// 可以阻止那些应该被禁用的模板实现。

// 在指针家族中，有两个特殊的指针：
// 1.void*指针，没有办法对它们解引用、递增或递减等操作；
// 2.char*指针，往往表示指向C类型的字符串，而不是指向独立字符的指针。

namespace test1 {
template <typename T>
void processPointer(T* ptr) {}

// 不能以void*和char*为参数调用processPointer：
template <>
void processPointer(void*) = delete;
template <>
void processPointer(const void*) = delete;
template <>
void processPointer(char*) = delete;
template <>
void processPointer(const char*) = delete;

void func() {
  int i = 5;
  processPointer(&i);

  float* fptr;
  processPointer(fptr);

  char* cptr;
  // processPointer(cptr);
  // processPointer("nihao");
  // processPointer(nullptr);

  //  use of deleted function ‘void n3::test1::processPointer(T*) [withT=char]’
}
}  // namespace test1

namespace test2 {
// 在类内不能赋予成员函数模板主体和显示具体化拥有不同的访问权限：
class A {
 public:
  template <typename T>
  void processPointer(T* ptr) {}

 private:
  // 模板的显示具体化必须要在命名空间作用域内，而不是类的作用域内：
  // template <>
  // void processPointer(void* ptr);
};
template <>
void A::processPointer<void>(void*) = delete;  // 仍是公有的，但已被删除
template <>
void A::processPointer<const char>(const char*) = delete;

void func() {
  A a;

  int* iptr;
  a.processPointer(iptr);

  char* cptr;
  a.processPointer(cptr);
  // a.processPointer(nullptr);
  // a.processPointer("nihao");
}
}  // namespace test2

void func() {
  test1::func();
  test2::func();
}
}  // namespace n3

// 要记住的东西：
// 1.优先使用删除函数而不是私有而不定义的函数；
// 2.任何函数都可以被声明为删除，包括非成员函数和模板实现。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}