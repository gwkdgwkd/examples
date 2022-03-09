#include <string.h>  // C 风格字符串相关函数的头文件，比如strlen
#include <iostream>
#include <string>  // string类库的头文件

// C++大大增强了对字符串的支持，除了可以使用C风格的字符串，还可以使用内置的string类。
// string类处理起字符串来会方便很多，完全可以代替C语言中的字符数组或字符串指针。
// string变量可以直接通过赋值操作符=进行赋值。
// string变量也可以用C风格的字符串进行赋值。

// string的内部究竟是什么样的？
// 在C语言中，有两种方式表示字符串(两种形式总是以\0作为结束标志)：
//  一种是用字符数组来容纳字符串，例如char str[10] = "abc"，这样的字符串是可读写的；
//  一种是使用字符串常量，例如char *str = "abc"，这样的字符串只能读，不能写。
// C++ string与它们在C语言中的前身截然不同。
// 首先，也是最重要的不同点，C++ string隐藏了它所包含的字符序列的物理表示。
// 程序设计人员不必关心数组的维数或\0方面的问题。
// string在内部封装了与内存和容量有关的信息。
// 具体地说，C++ string对象知道自己在内存中的开始位置、包含的字符序列以及字符序列长度；
// 当内存空间不足时，string还会自动调整，让内存空间增长到足以容纳下所有字符序列的大小。
// C++ string的这种做法，极大地减少了C语言编程中三种最常见且最具破坏性的错误：
//  数组越界；
//  通过未被初始化或者被赋以错误值的指针来访问数组元紊；
//  释放了数组所占内存，但是仍然保留了“悬空”指针。
// C++标准没有定义string类的内存布局，各个编译器厂商可以提供不同的实现，但必须保证string的行为一致。
// 采用这种做法是为了获得足够的灵活性。
// C++标准没有定义在哪种确切的情况下应该为string对象分配内存空间来存储字符序列。
// string内存分配规则明确规定：允许但不要求以引用计数（reference counting）的方式实现。
// 但无论是否采用引用计数，其语义都必须一致。
// C++的这种做法和C语言不同，在C语言中，每个字符型数组都占据各自的物理存储区。
// 在C++中，独立的几个string对象可以占据也可以不占据各自特定的物理存储区，但是，如果采用引用计数避免
// 了保存同一数据的拷贝副本，那么各个独立的对象必须看起来并表现得就像独占地拥有各自的存储区一样。
// 只有当字符串被修改的时候才创建各自的拷贝，这种实现方式称为写时复制（copy-on-write）策略。
// 当字符串只是作为值参数或在其他只读情形下使用，这种方法能够节省时间和空间。
// 不论一个库的实现是不是采用引用计数，它对string类的使用者来说都应该是透明的。
// 遗憾的是，情况并不总是这样，在多线程程序中，几乎不可能安全地使用引用计数来实现。

// 字符串是存储在内存的连续字节中的一系列字符。C++处理字符串的方式有两种：
// 一种来自C语言，常被称为C风格字符串；另一种是基于string类库的字符串处理方式。

namespace n1 {
void func1() {
  std::string s1;            // 创建一个空的string对象
  std::string s2("Hello!");  // 将string对象初始化为参数指向的字符串
  std::string s3(10, 'a');    // 创建string对象，包含n个字符c
  std::string s4 = "hello!";  // 使用C语言风格来初始化string对象
  std::string s5(s2);         // 复制构造函数
  std::string s6 = s2;        // 复制构造函数
  std::string s7;
  s7 = s2;  // 赋值操作符

  std::cout << std::boolalpha << s1.empty() << std::endl;  // true
  std::cout << s2 << std::endl;                            // Hello!
  std::cout << s3 << std::endl;                            // aaaaaaaaaa
  std::cout << s4 << std::endl;                            // hello!
  std::cout << s5 << std::endl;                            // Hello!
  std::cout << s6 << std::endl;                            // Hello!
  std::cout << s7 << std::endl;                            // Hello!
}

void func2() {
  std::string s = "hello!";

  // 转换函数c_str()，能够将string字符串转换为C风格的字符串，
  // 并返回该字符串的const指针（const char*）。
  // 在C语言中，使用strlen函数获取字符串的长度：
  std::cout << strlen(s.c_str()) << std::endl;  // 6

  // 在C++中，可以使用string.size()函数或string.length()函数来获得string对象的长度：
  std::cout << s.length() << std::endl;  // 6
  std::cout << s.size() << std::endl;    // 6

  // lenght()和size()的实现并没有区别。
  // length()方法是C语言习惯保留的，size()方法则是为了兼容STL容器而引入的。
}
void testN1() {
  func1();
  func2();
}
}  // namespace n1

namespace n2 {

void testN2() {}
}  // namespace n2

namespace n3 {

void testN3() {}
}  // namespace n3

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
