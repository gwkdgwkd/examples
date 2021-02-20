#include <iostream>

using namespace std;

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

class NoCopyCator {
 public:
  NoCopyCator() = default;
  NoCopyCator(const NoCopyCator&) = delete;  // 阻止使用拷贝构造函数
};

// delete可以避免一些不必要的隐式数据类型转换
class ConvType {
 public:
  ConvType(int i) {}
  ConvType(char c) = delete;
};
void Func(ConvType ct) {}

// delete不局限于缺省版本的类成员函数或者全局函数，对于普通的函数也可以通过显式删除来禁止类型转换
void Func1(int i) {}
void Func1(char c) = delete;

// 显示删除可以删除自定义类型的operator new操作来避免在堆上分配该class对象。
class NoHeapAlloc {
 public:
  void* operator new(std::size_t) = delete;
};

// 如果需要在指定内存位置进行内存分配，并且不需要析构函数来完成一些对象级别的清理。可以通过显示删除析构函数来限制自定义类型在栈上或者静态的构造。
char p[10];
class NoStackAlloc {
 public:
  ~NoStackAlloc() = delete;
};

// ==========

// 如果实现了这些默认函数的自定义版本后，编译器就不会去生成默认的版本。有时候我们需要声明带参数的构造函数，此时就不会生成默认的构造函数，
// 这样会导致类不再是POD类型，从而影响类的优化：
class Example1 {
 public:
  Example1(int i) : data(i) {}

 private:
  int data;
};
// C++11中提供了新的机制来控制默认函数生成来避免这个问题，在声明时在函数末尾加上”=default”来显式地指示编译器去生成该函数的默认版本，这样就保证了类还是POD类型：
class Example2 {
 public:
  Example2() = default;
  Example2(int i) : data(i) {}

 private:
  int data;
};

// 另一方面，有时候可能需要限制一些默认函数的生成，例如需要禁止拷贝构造函数的使用。原来，通过把拷贝构造函数声明为private成员，这样一旦使用编译器就会报错。
// 而在C++11中，在函数的定义或者声明后面加上”= delete”就能实现这个效果。

// = default和= delete能够更加精准的控制类的默认函数版本。其中，= default同样也能在类外的定义中修饰成员函数：
class Example3 {
 public:
  Example3() = default;
  Example3(const Example3&);

 private:
  int data;
};
Example3::Example3(const Example3& ex) = default;
// 这样的好处是，能够为一个类实现多个版本，只要我们在头文件里声明同样的函数，而在不同的cpp文件中用不同的方法实现，当选择不同的cpp编译时产生的版本就不同。

int main() {
  NoCopyCator a;
  // NoCopyCator b(a);  // error: use of deleted function ‘NoCopyCator::NoCopyCator(const NoCopyCator&)’

  Func(3);
  // Func('a');  // error: use of deleted function ‘ConvType::ConvType(char)’
  ConvType ci(3);
  // ConvType cc('a');  // error: use of deleted function ‘ConvType::ConvType(char)’

  Func1(3);
  // Func1('c');  // error: use of deleted function ‘void Func1(char)’

  NoHeapAlloc nha;
  // NoHeapAlloc* pnha = new NoHeapAlloc; // error: use of deleted function ‘static void* NoHeapAlloc::operator new(std::size_t)’

  new (p) NoStackAlloc();  // placement new, p无需调用析构函数
  // NoStackAlloc nsa;  // error: use of deleted function ‘NoStackAlloc::~NoStackAlloc()’

  std::cout << std::is_pod<Example1>::value << std::endl;  // 0
  std::cout << std::is_pod<Example2>::value << std::endl;  // 1

  return 0;
}