#include <iostream>

using namespace std;

// 所谓重载，就是赋予新的含义。函数重载（Function Overloading）可以让一个函数名有多种功能，在不同情况下进行不同的操作。
// 运算符重载（Operator Overloading）也是一个道理，同一个运算符可以有不同的功能。
// 实际上，已经在不知不觉中使用了运算符重载。例如，+号可以对不同类型（int、float 等）的数据进行加法操作；
// <<既是位移运算符，又可以配合cout向控制台输出数据。C++本身已经对这些运算符进行了重载。
// C++也允许程序员自己重载运算符，这给我们带来了很大的便利。
// 运算符重载其实就是定义一个函数，在函数体内实现想要的功能，当用到该运算符时，编译器会自动调用这个函数。也就是说，运算符重载是通过函数实现的，它本质上是函数重载。
// 运算符重载的格式为：返回值类型 operator 运算符名称 (形参表列){}
// operator是关键字，专门用于定义重载运算符的函数。可以将operator运算符名称这一部分看做函数名。
// 运算符重载函数除了函数名有特定的格式，其它地方和普通函数并没有区别。
// 虽然运算符重载所实现的功能完全可以用函数替代，但运算符重载使得程序的书写更加人性化，易于阅读。运算符被重载后，原有的功能仍然保留，没有丧失或改变。
// 通过运算符重载，扩大了C++已有运算符的功能，使之能用于对象。
class complex {
 public:
  complex();
  complex(double real, double imag);

 public:
  complex operator+(const complex &A) const;
  void display() const;

 private:
  double m_real;
  double m_imag;
};
complex::complex() : m_real(0.0), m_imag(0.0) {}
complex::complex(double real, double imag) : m_real(real), m_imag(imag) {}
// complex complex::operator+(const complex &A) const {
//   complex B;
//   B.m_real = this->m_real + A.m_real;
//   B.m_imag = this->m_imag + A.m_imag;
//   return B;
// }
complex complex::operator+(const complex &A) const {
  // 会创建一个临时对象，这个对象没有名称，是一个匿名对象。在创建临时对象过程中调用构造函数，return语句将该临时对象作为函数返回值。
  return complex(this->m_real + A.m_real, this->m_imag + A.m_imag);
}
void complex::display() const {
  cout << m_real << " + " << m_imag << "i" << endl;
}

// 运算符重载函数不仅可以作为类的成员函数，还可以作为全局函数。
class complex1 {
 public:
  complex1();
  complex1(double real, double imag);

 public:
  void display() const;
  // 运算符重载函数不是complex类的成员函数，但是却用到了complex类的private成员变量，所以必须在complex类中将该函数声明为友元函数。
  friend complex1 operator+(const complex1 &A, const complex1 &B);

 private:
  double m_real;
  double m_imag;
};
complex1 operator+(const complex1 &A, const complex1 &B);
complex1::complex1() : m_real(0.0), m_imag(0.0) {}
complex1::complex1(double real, double imag) : m_real(real), m_imag(imag) {}
void complex1::display() const {
  cout << m_real << " + " << m_imag << "i" << endl;
}
complex1 operator+(const complex1 &A, const complex1 &B) {
  complex1 C;
  C.m_real = A.m_real + B.m_real;
  C.m_imag = A.m_imag + B.m_imag;
  return C;
}

// 运算符重载的注意事项:
//  1) 并不是所有的运算符都可以重载。能够重载的运算符包括：
//     + - * / % ^ & | ~ ! = < > += -= *= /= %= ^= &= |= << >> <<= >>= == != <= >= && || ++ -- , ->* -> () [] new new[] delete delete[]
//     上述运算符中，[]是下标运算符，()是函数调用运算符。自增自减运算符的前置和后置形式都可以重载。
//     长度运算符sizeof、条件运算符: ?、成员选择符.和域解析运算符::不能被重载。
//  2) 重载不能改变运算符的优先级和结合性。
//  3) 重载不会改变运算符的用法，原有有几个操作数、操作数在左边还是在右边，这些都不会改变。例如~号右边只有一个操作数，+号总是出现在两个操作数之间，重载后也必须如此。
//  4) 运算符重载函数不能有默认的参数，否则就改变了运算符操作数的个数，这显然是错误的。
//  5) 运算符重载函数既可以作为类的成员函数，也可以作为全局函数。
//     将运算符重载函数作为类的成员函数时，二元运算符的参数只有一个，一元运算符不需要参数。之所以少一个参数，是因为这个参数是隐含的。
//     将运算符重载函数作为全局函数时，二元操作符就需要两个参数，一元操作符需要一个参数，而且其中必须有一个参数是对象，好让编译器区分这是程序员自定义的运算符，
//     防止程序员修改用于内置类型的运算符的性质。如果有两个参数，这两个参数可以都是对象，也可以一个是对象，一个是C++内置类型的数据。
//     另外，将运算符重载函数作为全局函数时，一般都需要在类中将该函数声明为友元函数。原因很简单，该函数大部分情况下都需要使用类的private成员。
//  6) 箭头运算符->、下标运算符[]、函数调用运算符()、赋值运算符=只能以成员函数的形式重载。

// 在C++中进行运算符重载时，有以下问题需要注意：
//  重载后运算符的含义应该符合原有用法习惯。例如重载+运算符，完成的功能就应该类似于做加法，在重载的+运算符中做减法是不合适的。此外，重载应尽量保留运算符原有的特性。
//  C++规定，运算符重载不改变运算符的优先级。
//  以下运算符不能被重载：.、.*、::、? :、sizeof。
//  重载运算符()、[]、->、或者赋值运算符=时，只能将它们重载为成员函数，不能重载为全局函数。
// 运算符重载的实质是将运算符重载为一个函数，使用运算符的表达式就被解释为对重载函数的调用。
// 运算符可以重载为全局函数。此时函数的参数个数就是运算符的操作数个数，运算符的操作数就成为函数的实参。
// 运算符也可以重载为成员函数。此时函数的参数个数就是运算符的操作数个数减一，运算符的操作数有一个成为函数作用的对象，其余的成为函数的实参。
// 必要时需要重载赋值运算符=，以避免两个对象内部的指针指向同一片存储空间。
// 运算符可以重载为全局函数，然后声明为类的友元。
// <<和>>是在iostream中被重载，才成为所谓的“流插入运算符”和“流提取运算符”的。
// 类型的名字可以作为强制类型转换运算符，也可以被重载为类的成员函数。它能使得对象被自动转换为某种类型。
// 自增、自减运算符各有两种重载方式，用于区别前置用法和后置用法。
// 运算符重载不改变运算符的优先级。重载运算符时，应该尽量保留运算符原本的特性。

int main() {
  complex c1(4.3, 5.8);
  complex c2(2.4, 3.7);
  complex c3;
  c3 = c1 + c2;
  c3.display();  // 6.7 + 9.5i

  complex1 c11(4.3, 5.8);
  complex1 c12(2.4, 3.7);
  complex1 c13;
  c13 = c11 + c12;
  c13.display();  // 6.7 + 9.5i

  return 0;
}