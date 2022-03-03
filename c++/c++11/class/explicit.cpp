#include <iostream>

// 在C++中，我们有时可以将构造函数用作自动类型转换函数。
// 但这种自动特性并非总是合乎要求的，有时会导致意外的类型转换，
// 因此，C++新增了关键字explicit，用于关闭这种自动特性。
// 即被explicit关键字修饰的类构造函数，不能进行自动地隐式类型转换，只能显式地进行类型转换。
// 注意：只有一个参数的构造函数，或者构造函数有n个参数，但有n-1个参数提供了默认值，这样的情况才能进行类型转换。

// C++中的explicit关键字只能用于修饰只有一个参数的类构造函数,它的作用是表明该构造函数是显示的，
// 而非隐式的，跟它相对应的另一个关键字是implicit，意思是隐藏的，类构造函数默认情况下为implicit(隐式)。
// explicit关键字的作用：禁止隐式调用类内的单参数构造函数，这主要包括如下三层意思：
// 1.该关键字只能用来修饰类内部的构造函数
// 2.禁止隐式调用拷贝构造函数
// 3.禁止类对象之间的隐式转换

// 总结：
// 1.explicit关键字只需用于类内的单参数构造函数前面。
//   由于无参数的构造函数和多参数的构造函数总是显示调用，这种情况在构造函数前加explicit无意义。
// 2 如果想禁止类A对象被隐式转换为类B对象，可在类B中使用关键字explicit。

// google的c++规范中提到explicit的优点是可以避免不合时宜的类型变换，缺点无。
// 所以google约定所有单参数的构造函数都必须是显示的，只有极少数情况下拷贝构造函数可以不声明称explicit。
// 例如作为其他类的透明包装器的类。
// effective c++中说：被声明为explicit的构造函数通常比其non-explicit兄弟更受欢迎。
// 因为它们禁止编译器执行非预期的类型转换，除非我有一个好理由允许构造函数被用于隐式类型转换，否则把它声明为explicit。

namespace n1 {
class Demo {
 public:
  Demo() { std::cout << "Demo()" << std::endl; };  // 没有参数，无法进行类型转换
  Demo(double a) {  // 有一个参数，可以进行类型转换
    std::cout << "Demo(double a)" << std::endl;
  };
  Demo(int a, double b) {  // 有两个参数，且无默认值，故无法使用类型转换
    std::cout << "Demo(int a, double b)" << std::endl;
  };
  Demo(int a, int b = 10, double c = 1.6) {  // n-1个有默认值，可以进行隐式转换
    std::cout << "Demo(int a, int b = 10, double c = 1.6)" << std::endl;
  };
  ~Demo(){};
};

void testN1() {
  Demo d1;             // Demo()
  Demo d2(1.23);       // Demo(double a)
  Demo d3(5, 2.4);     // Demo(int a, double b)
  Demo d4(6, 6);       // Demo(int a, int b = 10, double c = 1.6)
  Demo d5(6, 8, 1.7);  // Demo(int a, int b = 10, double c = 1.6)

  // 发生隐式转换
  Demo d6 = 9.8;  // Demo(double a)
  Demo d7 = 1;    // Demo(int a, int b = 10, double c = 1.6)
}
}  // namespace n1

namespace n2 {
class Demo {
 public:
  Demo() { std::cout << "Demo()" << std::endl; };
  explicit Demo(double a) { std::cout << "Demo(double a)" << std::endl; };
  Demo(int a, double b) { std::cout << "Demo(int a, double b)" << std::endl; };
  explicit Demo(int a, int b = 10, double c = 1.6) {
    std::cout << "Demo(int a, int b = 10, double c = 1.6)" << std::endl;
  };
  ~Demo(){};
};

void testN2() {
  // 不能发生隐式转换：
  // Demo d1 = 9.8;
  // Demo d2 = 1;

  // 可以进行显示类型转换：
  Demo d3 = Demo(12.2);  // Demo(double a)
  Demo d4 = (Demo)12.2;  // Demo(double a)
  Demo d5 = Demo(7);     // Demo(int a, int b = 10, double c = 1.6)
  Demo d6 = (Demo)9;     // Demo(int a, int b = 10, double c = 1.6)
}
}  // namespace n2

namespace n3 {
class CExplict {
 public:
  CExplict(bool _explicit) { this->is_explict = _explicit; }
  CExplict(const CExplict& other) { this->is_explict = other.is_explict; }
  friend void printExplicit(const CExplict& cx);

 private:
  bool is_explict;
};
void printExplicit(const CExplict& cx) {
  std::cout << "is_explict=" << cx.is_explict << std::endl;
}

void testN3() {
  // 编译器首先会以true构造一个临时对象，在将此临时对象通过赋值构造函数赋值给cx1
  CExplict cx1{1};
  CExplict cx2 = false;
  // CExplict cx1{1}和printExplicit(false)这种表达形式看着很别扭，也很让人费解，
  // 将一个bool型的值赋给一个CExplicit类的cx1，使代码的可读性变差。
  printExplicit(cx1);    // is_explict=1
  printExplicit(cx2);    // is_explict=0
  printExplicit(false);  // is_explict=0
  printExplicit(3.14);   // is_explict=1
}
}  // namespace n3

namespace n4 {
class CExplict {
 public:
  explicit CExplict(bool _explicit) { this->is_explict = _explicit; }
  CExplict(const CExplict& other) { this->is_explict = other.is_explict; }
  friend void printExplicit(const CExplict& cx);

 private:
  bool is_explict;
};
void printExplicit(const CExplict& cx) {
  std::cout << "is_explict=" << cx.is_explict << std::endl;
}

void testN4() {
  CExplict cx1 = CExplict(true);
  CExplict cx2 = cx1;
  printExplicit(cx1);              // is_explict=1
  printExplicit(cx2);              // is_explict=1
  printExplicit(CExplict(false));  // is_explict=0
  printExplicit(CExplict(1));      // is_explict=1
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}