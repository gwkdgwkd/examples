#include <iostream>

// C++中声明自定义的类，编译器会默认生成未定义的成员函数：
// 1.构造函数
// 2.拷贝构造函数
// 3.重载的赋值运算符（operator=）
// 4.移动构造函数
// 5.重载的移动赋值运算符
// 6.析构函数
// 编译器还会提供全局默认操作符函数：
// 1.operator,
// 2.operator &
// 3.operator &&
// 4.operator *
// 5.operator ->
// 6.operator ->*
// 7.operator new
// 8.operator delete

// =default修饰的函数为显式缺省函数
// =delete修饰的函数为删除函数

namespace n1 {
// 有时候可能需要限制一些默认函数的生成，例如需要禁止拷贝构造函数的使用。
// 原来，通过把拷贝构造函数声明为private成员，这样一旦使用编译器就会报错。
// 而在C++11中，在函数的定义或者声明后面加上=delete就能实现这个效果。
class NoCopyCator {
 public:
  NoCopyCator() = default;
  // 阻止使用拷贝构造函数：
  NoCopyCator(const NoCopyCator&) = delete;
};
void func1() {
  NoCopyCator c1;
  // NoCopyCator c2(c1);
  // NoCopyCator c3 = c1;
}

class ConvType {
 public:
  ConvType(int i) {}
  // delete可以避免一些不必要的隐式数据类型转换：
  ConvType(char c) = delete;
};
void f1(ConvType ct) {}
void func2() {
  // ConvType c = 'a';
  // f1('c');
}

// delete不局限于缺省版本的类成员函数或者全局函数，
// 对于普通的函数也可以通过显式删除来禁止类型转换：
void f2(int i) {}
void f3(char c) = delete;
void func3() {
  f2(5);
  // f3('b');
}

class NoHeapAlloc {
 public:
  // 避免在堆上分配该class对象：
  void* operator new(std::size_t) = delete;
};
void func4() {
  // NoHeapAlloc c1 = new NoHeapAlloc();
  NoHeapAlloc c2;
}

// 需要在指定内存位置进行内存分配，并且不需要析构函数来完成对象的清理，
// 可以通过显示删除析构函数来限制自定义类型在栈上或者静态的构造。
class NoStackAlloc {
 public:
  ~NoStackAlloc() = delete;
};
void func5() {
  // NoStackAlloc c1;
  // static NoStackAlloc c2;
  char p[10];
  // placement new，无需调用析构函数：
  NoStackAlloc* ptr1 = new (p) NoStackAlloc();

  // 在堆上创建对象：
  NoStackAlloc* ptr2 = new NoStackAlloc();
}

void func() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace n1

namespace n2 {
namespace test1 {
// 如果实现了这些默认函数的自定义版本后，编译器就不会去生成默认的版本。
// 有时候需要声明带参数的构造函数，此时就不会生成默认的构造函数，
// 这样会导致类不再是POD类型，从而影响类的优化：
class A {
 public:
  A(int i) : data(i) {}

 private:
  int data;
};
void func() {
  // A a;
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_standard_layout<A>::value << std::endl;  // true
  std::cout << std::is_trivial<A>::value << std::endl;          // false
  std::cout << std::is_pod<A>::value << std::endl;              // false
}
}  // namespace test1

namespace test2 {
// C++11中提供了新的机制来控制默认函数生成来避免这个问题，
// 在函数声明末尾加上=default来显式地指示编译器去生成该函数的默认版本，
// 这样就保证了类还是POD类型：
class A {
 public:
  A() = default;
  A(int i) : data(i) {}

 private:
  int data;
};

void func() {
  A a;
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_standard_layout<A>::value << std::endl;  // true
  std::cout << std::is_trivial<A>::value << std::endl;          // true
  std::cout << std::is_pod<A>::value << std::endl;              // true
}
}  // namespace test2

namespace test3 {
// =default和=delete能够更加精准的控制类的默认函数版本。
// 其中，=default同样也能在类外的定义中修饰成员函数：
class A {
 public:
  A() = default;
  A(const A&);

 private:
  int data;
};
// 这样的好处是，能够为一个类实现多个版本，在头文件里声明同样的函数，
// 而在不同的cpp中用不同的方法实现，当选择不同的cpp编译时产生的版本就不同。
A::A(const A& ex) = default;

void func() { A a; }
}  // namespace test3
}  // namespace n2

int main(int argc, char* argv[]) {
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
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}