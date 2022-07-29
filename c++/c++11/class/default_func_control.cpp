#include <iostream>

// C++中声明自定义的类，编译器会默认生成未定义的成员函数：
//  构造函数
//  拷贝构造函数
//  拷贝赋值函数（operator=）
//  移动构造函数
//  移动拷贝函数
//  析构函数
// 编译器还会提供全局默认操作符函数：
//  operator,
//  operator &
//  operator &&
//  operator *
//  operator ->
//  operator ->*
//  operator new
//  operator delete

// =default修饰的函数为显式缺省函数
// =delete修饰的函数为删除函数

namespace del {
// 有时候可能需要限制一些默认函数的生成，例如需要禁止拷贝构造函数的使用。
// 原来，通过把拷贝构造函数声明为private成员，这样一旦使用编译器就会报错。
// 而在C++11中，在函数的定义或者声明后面加上= delete就能实现这个效果。
class NoCopyCator {
 public:
  NoCopyCator() = default;
  NoCopyCator(const NoCopyCator&) = delete;  // 阻止使用拷贝构造函数
};
void func1() {
  NoCopyCator c1;
  // NoCopyCator c2(c1);
  // NoCopyCator c3 = c1;
}

class ConvType {
 public:
  ConvType(int i) {}
  ConvType(char c) = delete;  // delete可以避免一些不必要的隐式数据类型转换
};
void f1(ConvType ct) {}
void func2() {
  // ConvType c = 'a';
  // f('c');
}

// delete不局限于缺省版本的类成员函数或者全局函数，
// 对于普通的函数也可以通过显式删除来禁止类型转换。
void f2(int i) {}
void f3(char c) = delete;
void func3() {
  f2(5);
  // f3('b');
}

class NoHeapAlloc {
 public:
  void* operator new(std::size_t) = delete;  // 避免在堆上分配该class对象
};
void func4() {
  // NoHeapAlloc c1 = new NoHeapAlloc();
  NoHeapAlloc c2;
}

// 需要在指定内存位置进行内存分配，并且不需要析构函数来完成一些对象级别的清理，
// 可以通过显示删除析构函数来限制自定义类型在栈上或者静态的构造。

class NoStackAlloc {
 public:
  ~NoStackAlloc() = delete;
};
void func5() {
  // NoStackAlloc c1;
  // static NoStackAlloc c2;
  char p[10];
  new (p) NoStackAlloc();  // placement new, p无需调用析构函数

  NoStackAlloc* ptr = new NoStackAlloc();  // 在堆上创建对象
}

void testDel() {
  func1();
  func2();
  func3();
  func4();
  func5();
}
}  // namespace del

namespace def1 {
// 如果实现了这些默认函数的自定义版本后，编译器就不会去生成默认的版本。
// 有时候需要声明带参数的构造函数，此时就不会生成默认的构造函数，
// 这样会导致类不再是POD类型，从而影响类的优化：
class A {
 public:
  A(int i) : data(i) {}

 private:
  int data;
};
void testDef1() {
  // A a;
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_standard_layout<A>::value << std::endl;  // true
  std::cout << std::is_trivial<A>::value << std::endl;          // false
  std::cout << std::is_pod<A>::value << std::endl;              // false
}
}  // namespace def1

namespace def2 {
// C++11中提供了新的机制来控制默认函数生成来避免这个问题，
// 在声明时在函数末尾加上=default来显式地指示编译器去生成该函数的默认版本，
// 这样就保证了类还是POD类型：
class A {
 public:
  A() = default;
  A(int i) : data(i) {}

 private:
  int data;
};

void testDef2() {
  A a;
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << std::is_standard_layout<A>::value << std::endl;  // true
  std::cout << std::is_trivial<A>::value << std::endl;          // true
  std::cout << std::is_pod<A>::value << std::endl;              // true
}
}  // namespace def2

namespace def3 {
// = default和= delete能够更加精准的控制类的默认函数版本。
// 其中，= default同样也能在类外的定义中修饰成员函数：
class A {
 public:
  A() = default;
  A(const A&);

 private:
  int data;
};
// 这样的好处是，能够为一个类实现多个版本，只要在头文件里声明同样的函数，
// 而在不同的cpp文件中用不同的方法实现，当选择不同的cpp编译时产生的版本就不同。
A::A(const A& ex) = default;

void testDef3() { A a; }
}  // namespace def3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 2]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      del::testDel();
      break;
    case 1:
      def1::testDef1();
      break;
    case 2:
      def2::testDef2();
      break;
    case 3:
      def3::testDef3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}