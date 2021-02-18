#include <iostream>

using namespace std;

// 在C++中，我们有时可以将构造函数用作自动类型转换函数。但这种自动特性并非总是合乎要求的，有时会导致意外的类型转换，因此，C++新增了关键字explicit，
// 用于关闭这种自动特性。即被explicit关键字修饰的类构造函数，不能进行自动地隐式类型转换，只能显式地进行类型转换。
// 注意：只有一个参数的构造函数，或者构造函数有n个参数，但有n-1个参数提供了默认值，这样的情况才能进行类型转换。

class Demo {
 public:
  Demo() { cout << "Demo()" << endl; };  // 没有参数，无法进行类型转换
  Demo(double a) {  // 有一个参数，可以进行类型转换
    cout << "Demo(double a)" << endl;
  };
  Demo(int a, double b) {  // 有两个参数，且无默认值，故无法使用类型转换
    cout << "Demo(int a, double b)" << endl;
  };
  Demo(int a, int b = 10, double c = 1.6) {  // n-1个有默认值，可以进行隐式转换
    cout << "Demo(int a, int b = 10, double c = 1.6)" << endl;
  };
  ~Demo(){};
};

class Demo1 {
 public:
  Demo1() { cout << "Demo1()" << endl; };
  explicit Demo1(double a) { cout << "Demo1(double a)" << endl; };
  Demo1(int a, double b) { cout << "Demo1(int a, double b)" << endl; };
  ~Demo1(){};
};

// C++中的explicit关键字只能用于修饰只有一个参数的类构造函数,它的作用是表明该构造函数是显示的,而非隐式的,跟它相对应的另一个关键字是implicit,
// 意思是隐藏的,类构造函数默认情况下为implicit(隐式)。
// explicit关键字的作用：禁止隐式调用类内的单参数构造函数，这主要包括如下三层意思：
// 1 该关键字只能用来修饰类内部的构造函数
// 2 禁止隐式调用拷贝构造函数
// 3 禁止类对象之间的隐式转换

class CExplict {
 public:
  // explicit CExplict(bool _explicit) { this->is_explict = _explicit; }
  CExplict(bool _explicit) { this->is_explict = _explicit; }
  CExplict(const CExplict& other) { this->is_explict = other.is_explict; }
  friend void printExplicit(const CExplict& cx);

 private:
  bool is_explict;
};
void printExplicit(const CExplict& cx) {
  cout << "is_explict=" << cx.is_explict << endl;
}

class CExplict1 {
 public:
  explicit CExplict1(bool _explicit) { this->is_explict = _explicit; }
  CExplict1(const CExplict1& other) { this->is_explict = other.is_explict; }
  friend void printExplicit1(const CExplict1& cx);

 private:
  bool is_explict;
};
void printExplicit1(const CExplict1& cx) {
  cout << "is_explict=" << cx.is_explict << endl;
}

// 总结：
// 1 explicit关键字只需用于类内的单参数构造函数前面。由于无参数的构造函数和多参数的构造函数总是显示调用，这种情况在构造函数前加explicit无意义。
// 2 如果想禁止类A对象被隐式转换为类B对象，可在类B中使用关键字explicit。

// google的c++规范中提到explicit的优点是可以避免不合时宜的类型变换，缺点无。所以google约定所有单参数的构造函数都必须是显示的，只有极少数情况下
// 拷贝构造函数可以不声明称explicit。例如作为其他类的透明包装器的类。
// effective c++中说：被声明为explicit的构造函数通常比其non-explicit兄弟更受欢迎。因为它们禁止编译器执行非预期的类型转换,除非我有一个好理由
// 允许构造函数被用于隐式类型转换,否则我会把它声明为explicit.鼓励大家遵循相同的政策。

int main() {
  Demo test1;
  test1 = 12.2;
  // Demo()
  // Demo(double a)

  Demo test2;
  test2 = 10;
  // Demo()
  // Demo(int a, int b = 10, double c = 1.6)

  Demo1 test3;  // Demo1()
  // test3 = 12.2;  // 由于使用了explicit关键字，则无法进行隐式转换
  // 但是我们可以进行显示类型转换:
  test3 = Demo1(12.2);  // Demo1(double a)
  test3 = (Demo1)12.2;  // Demo1(double a)

  // 编译器首先会以true构造一个临时对象，在将此临时对象通过赋值构造函数赋值给cx1
  CExplict cx1{1};
  CExplict cx2{3.14};
  CExplict cx3 = false;
  // CExplict cx1{1}和printExplicit(false)这种表达形式看着很别扭，也很让人费解，将一个bool型的值赋给一个CExplicit类的cx1，使代码的可读性变差。
  // 因此，为了禁止对类的单参数构造函数的隐式调用，C++引入了关键字explicit。在类的定义中，在任何一个单参数构造函数钱加explicit关键字，就可以禁止对该构造函数的隐式调用。
  printExplicit(cx1);    // is_explict=1
  printExplicit(cx2);    // is_explict=1
  printExplicit(cx3);    // is_explict=0
  printExplicit(false);  // is_explict=0
  printExplicit(3.14);   // is_explict=1

  CExplict1 cx11 = CExplict1(true);
  CExplict1 cx12 = cx11;
  printExplicit1(cx11);              // is_explict=1
  printExplicit1(cx12);              // is_explict=1
  printExplicit1(CExplict1(false));  // is_explict=0

  return 0;
}