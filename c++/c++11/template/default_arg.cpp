#include <iostream>

using namespace std;

// C++11支持函数模板的默认模板参数

// 在C++98/03标准中，类模板可以有默认的模板参数，如下：
template <typename T = int, typename U = int, U N = 0>
struct Foo {};
// 但是却不支持函数的默认模板参数：
template <typename T = int>  // error in C++98/03: default template arguments
void func() {}
// error: default template arguments may not be used in function templates without -std=c++11 or -std=gnu++11

// 函数模板的默认模板参数在使用规则上和其他的默认参数也有一些不同，它没有必须写在参数表最后的限制。
// 类模板在为多个默认模板参数声明指定默认值时，必须遵照“从右往左”的规则进行指定。而这个规则对函数模板来说并不是必须的。
// 类模板的默认模板参数必须从右往左定义，函数模板的默认模板参数则没这个限定:
template <class T1, class T2 = int>
class DefClass1;
// template<class T1 = int, class T2> class DefClass2; // 无法通过编译
template <class T, int i = 0>
class DefClass3;
// template<int i = 0, class T> class DefClass4; // 无法通过编译
template <class T1 = int, class T2>
void DefFunc1(T1 a, T2 b);
template <int i = 0, class T>
void DefFunc2(T a);

// 甚至于根据实际场景中函数模板被调用的情形，编译器还可以自行推导出部分模板参数的类型。
// 函数模板的参数推导规则也并不复杂。简单地讲：如果能够从函数实参中推导出类型的话，那么默认模板参数就不会被使用，反之，默认模板参数则可能会被使用。
// 这意味着当默认模板参数和编译器自行推导出模板参数类型的能力一起结合使用时，代码的书写将变得异常灵活。可以一部分模板参数采用默认参数，而另一部分使用自动推导:
template <typename R = int, typename U>
R func1(U val) {
  return val;
}

template <typename T, typename U = double>
void func2(T val1 = 0, U val2 = 0) {}

template <class T, class U = double>
void f(T t = 0, U u = 0){};  // f同时使用了默认模板参数和默认函数参数。
// 默认模板参数通常是需要跟默认函数参数一起使用的

// 总的来说，C++11 支持为函数模板中的参数设置默认值，在实际使用过程中，我们可以选择使用默认值，也可以尝试由编译器自行推导得到，还可以亲自指定各个模板参数的类型。

int main() {
  // 当所有模板参数都有默认参数时，函数模板的调用如同一个普通函数。但对于类模板而言，哪怕所有参数都有默认参数，在使用时也必须在模板名后跟随<>来实例化。
  func();  // 此时模板参数T的类型就为默认值int
  Foo<> a;
  // Foo a;   // error: missing template arguments before ‘a’

  // C++11标准中，可以像func(97)这样调用模板函数，因为编译器可以根据实参97自行推导出模板参数U的类型为int，并且根据返回值val=97推导出R的类型也为int；
  // 而func<char>(97)手动指定了模板参数R的类型为char（默认模板参数将无效），并通过实参97推导出了U=int；
  // 最后func<double,int>(97)手动指定的R和U的类型值，因此无需编译器自行推导。
  func1(97);               // R=int, U=int
  func1<char>(97);         // R=char, U=int
  func1<double, int>(97);  // R=double, U=int

  // 当默认模板参数和自行推导的模板参数同时使用时，若无法推导出函数模板参数的类型，编译器会选择使用默认模板参数；
  // 如果模板参数即无法推导出来，又未设置其默认值，则编译器直接报错
  func2('c');  // T=char, U=double
  // func2(); // 编译报错
  // func2('c')的这种调用方式，编译器通过实参'c'可以推导出T=char，但由于未传递第2个实参，因此模板参数U使用的是默认参数double；
  // 但func2()的调用方式是不行的，虽然val1设置有默认值，但编译器无法通过该默认值推导出模板参数T的类型。由此不难看出，编译器的自动推导能力并没有想象的那么强大。

  // 由于函数的模板参数可以由函数的实参推导而出:
  f(1, 'c');  // f<int, char>(1, 'c')
  f(1);       // f<int, double>(1, 0), 使用了默认模板参数double
  // f(); // 错误: T无法被推导出来
  f<int>();        // f<int, double>(0, 0), 使用了默认模板参数double
  f<int, char>();  // f<int, char>(0, 0)
  // 还有一点应该注意：模板函数的默认形参值不是模板参数推导的依据。函数模板参数的选择，终究是由函数的实参推导而来的。

  return 0;
}