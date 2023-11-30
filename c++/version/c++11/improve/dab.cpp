#include <iostream>

// 在C++98/03的泛型编程中，模板实例化有一个很烦琐的地方，
// 那就是连续两个右尖括号（>>）会被编译器解释成右移操作符，而不是模板参数表的结束。

// 各种C++98/03编译器除了支持标准（ISO/IEC14882：2003及其之前的标准）之外，
// 还自行做了不少的拓展：
// 这些拓展中的一部分，后来经过了C++委员会的斟酌和完善，进入了C++11。
// 所以有一部分C++11的新特征，在一些C++98/03的老编译器下也是可以支持的，
// 只是由于没有标准化，无法保证各种平台/编译器下的兼容性。
// 比如像MicrosoftVisualC++2005这种不支持C++11的编译器，
// 在对模板右尖括号的处理上和现在的C++11是一致的。

template <typename T>
struct Foo {
  typedef T type;
};
template <typename T>
class A {};
template <int N>
struct Fooo {};

int main(void) {
  // ‘>>’ should be ‘> >’ within a nested template argument list
  Foo<A<int>>::type xx;  // C++98/03编译出错
  // Foo<A<int>>要写成这样Foo<A<int> >，这种限制无疑是很没有必要的。
  // 在C++的各种成对括号中，目前只有右尖括号连续写两个会出现这种二义性。
  // static_cast、reinterpret_cast等C++标准转换运算符，
  // 都是使用<>来获得待转换类型（type-id）的。
  // 若这个type-id本身是一个模板，用起来会很不方便。

  // 在C++11中，这种限制终于被取消了，要求编译器对模板的右尖括号做单独处理，
  // 使编译器能够正确判断出：
  // >>是一个右移操作符还是模板参数表的结束标记（delimiter，界定符）。
  // 不过这种自动化的处理在某些时候会与老标准不兼容：
  // type/value mismatch at argument 1 in template parameter list for
  // ‘template<class T> struct Foo’
  // Fooo<100 >> 2 > xxxx; // 在C++98/03的编译器中编译是没问题的
  Fooo<(100 >> 2)> xxx;  // 这种加括号的写法其实也是一个良好的编程习惯

  return 0;
}
