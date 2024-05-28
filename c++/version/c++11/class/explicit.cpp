#include <iostream>

// 在C++中，有时可以将构造函数用作自动类型转换函数：
// 1.只有一个参数的构造函数；
// 2.构造函数有n个参数，但第一个参数右边的n-1个参数提供了默认值。

// 但这种自动特性并非总是合乎要求的，隐式的类型转换有时会导致意外的类型转换，
// 因此，C++新增了关键字explicit，被explicit关键字修饰的类构造函数，
// 不能进行自动地隐式类型转换，只能显式地进行类型转换。

// C++中的explicit关键字只能用于修饰只有一个参数的类构造函数，
// 它的作用是表明该构造函数是显示的，而非隐式的，
// 跟它相对应的另一个关键字是implicit，类构造函数默认情况下为implicit。
// explicit关键字的作用，禁止隐式调用类内的单参数构造函数，意味着：
// 1.该关键字只能用来修饰类内部的构造函数；
// 2.禁止隐式调用拷贝构造函数；
// 3.禁止类对象之间的隐式转换。

// google的c++规范中提到explicit的优点是可以避免不合时宜的类型变换，缺点无。
// 所以google约定所有单参数的构造函数都必须是显示的，
// 只有极少数情况下拷贝构造函数可以不是explicit，如作为其他类的透明包装器的类。
// effective c++中说：
// 被声明为explicit的构造函数通常比其non-explicit兄弟更受欢迎。
// 因为它们禁止编译器执行非预期的类型转换，
// 除非有一个好理由允许构造函数被用于隐式类型转换，否则把它声明为explicit。

namespace n1 {
class A {
 public:
  // 没有参数，无法进行类型转换：
  A() { std::cout << "A()" << std::endl; };
  // 有一个参数，可以进行类型转换：
  A(double a) { std::cout << "A(double a)" << std::endl; };
  // 有两个参数，且无默认值，故无法使用类型转换：
  A(int a, double b) { std::cout << "A(int a, double b)" << std::endl; };
  // n个参数中n-1个参数有默认值，可以进行隐式转换：
  A(int a, int b = 10, double c = 1.6) {
    std::cout << "A(int a, int b = 10, double c = 1.6)" << std::endl;
  };
  ~A(){};
};

void func() {
  A a1;             // A()
  A a2(1.23);       // A(double a)
  A a3(5, 2.4);     // A(int a, double b)
  A a4(6, 6);       // A(int a, int b = 10, double c = 1.6)
  A a5(6, 8, 1.7);  // A(int a, int b = 10, double c = 1.6)

  // 发生隐式转换
  A a6 = 9.8;  // A(double a)
  A a7 = 1;    // A(int a, int b = 10, double c = 1.6)
}
}  // namespace n1

namespace n2 {
class A {
 public:
  A() { std::cout << "A()" << std::endl; };
  explicit A(double a) { std::cout << "A(double a)" << std::endl; };
  A(int a, double b) { std::cout << "A(int a, double b)" << std::endl; };
  explicit A(int a, int b = 10, double c = 1.6) {
    std::cout << "A(int a, int b = 10, double c = 1.6)" << std::endl;
  };
  ~A(){};
};

void func() {
  // 不能发生隐式转换：
  // A a1 = 9.8;
  // A a2 = 1;

  // 可以进行显示类型转换：
  A a3 = A(12.2);               // A(double a)
  A a4 = (A)12.2;               // A(double a)
  A a5 = static_cast<A>(12.2);  // A(double a)
  A a6 = A(7);                  // A(int a, int b = 10, double c = 1.6)
  A a7 = (A)9;                  // A(int a, int b = 10, double c = 1.6)
  A a8 = static_cast<A>(9);     // A(int a, int b = 10, double c = 1.6)
}
}  // namespace n2

namespace n3 {
class A {
 public:
  A(bool b) { this->is_explict = b; }
  A(const A& other) { this->is_explict = other.is_explict; }
  friend void print(const A& cx);

 private:
  bool is_explict;
};
void print(const A& cx) {
  std::cout << "is_explict=" << cx.is_explict << std::endl;
}

void func() {
  // 编译器首先会以true构造临时对象，再将临时对象通过赋值构造函数赋值给a1：
  A a1{1};
  A a2 = false;

  print(a1);     // is_explict=1
  print(a2);     // is_explict=0
  print(false);  // is_explict=0
  print(3.14);   // is_explict=1

  // A a1{1}和print(false)这种表达形式看着很别扭，也很让人费解，
  // 将一个bool型的值赋给一个A类对象，使代码的可读性变差。
}
}  // namespace n3

namespace n4 {
class A {
 public:
  explicit A(bool b) { this->is_explict = b; }
  A(const A& other) { this->is_explict = other.is_explict; }
  friend void print(const A& cx);

 private:
  bool is_explict;
};
void print(const A& cx) {
  std::cout << "is_explict=" << cx.is_explict << std::endl;
}

void func() {
  A a1 = A(true);
  A a2 = a1;
  print(a1);        // is_explict=1
  print(a2);        // is_explict=1
  print(A(false));  // is_explict=0
  print(A(1));      // is_explict=1
}
}  // namespace n4

// 总结：
// 1.explicit关键字只需用于类内的单参数构造函数前面，
//   由于无参数的构造函数和多参数的构造函数总是显示调用，加explicit无意义；
// 2.如果想禁止类A对象被隐式转换为类B对象，可在类B中使用关键字explicit。

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
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}