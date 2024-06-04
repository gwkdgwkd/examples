#include <iostream>

// 在C++中，定义函数时可以给形参指定一个默认的值，如果用户指定了参数的值，
// 那么就使用用户指定的值，否则使用参数的默认值。

namespace n1 {
// C++规定，默认参数只能放在形参列表的最后，
// 而且一旦为某个形参指定了默认值，那么它后面的所有形参都必须有默认值。
// 实参和形参的传值是从左到右依次匹配的，默认参数的连续性是保证正确传参的前提。
void func_1(int a, int b = 10, int c = 20) {
  std::cout << a << " " << b << " " << c << std::endl;
}
// 默认参数除了使用数值常量指定，也可以使用表达式指定：
int g = 10;
void func_2(int a, int b, int c = g + 1) {
  std::cout << a << " " << b << " " << c << std::endl;
}
// void func_3(int a, int b = 10, int c = 20, int d);  // 报错
// void func_4(int a, int b = 10, int c, int d = 20);  // 报错

// 默认参数并非编程方面的重大突破，而只是提供了一种便捷的方式。
// 通过使用默认参数，可以减少构造函数以及方法重载的数量：
class A {
 public:
  A() {}
  A(int i) {}
  A(int i, int j) {}
};
class B {
 public:
  B(int i = 0, int j = 0) {}
};

void func() {
  func_1(1);        // 1 10 20
  func_1(1, 2);     // 1 2 20
  func_1(1, 2, 3);  // 1 2 3

  func_2(1, 2);     // 1 2 11
  func_2(1, 2, 3);  // 1 2 3

  A a1;
  A a2(5);
  A a3(5, 6);

  B b1;
  B b2(7);
  B b3(8, 9);
}
}  // namespace n1

namespace n2 {
// 默认参数可以：
// 1.定义处指定；
// 2.仅在声明处指定；
// 3.声明处和定义处同时指定。

// C++规定：在给定的作用域中函数的每个参数只能指定一次默认参数。
// C语言有四种作用域：
// 1.函数原型作用域；
// 2.局部作用域（函数作用域）；
// 3.块作用域；
// 4.全局（文件）作用域。
// 站在编译器的角度看，它不管当前作用域中是函数声明还是函数定义，
// 只要有默认参数就可以使用。

// 编译时会报错，错误信息表明不能在函数定义和函数声明中同时指定默认参数：
// void func_1(int a, int b = 10, int c = 36);
// void func_1(int a, int b = 10, int c = 36) {
//   std::cout << a << ", " << b << ", " << c << std::endl;
// }

// 把函数定义放到其他源文件中（另一个文件作用域）是可以编译通过的：
void func_2(int a, int b = 10, int c = 36);
void func_3(int a, int b = 20, int c = 40);
void func_4();

// 在多文件编程时，通常的做法是将函数声明放在头文件中，
// 并且一个函数只声明一次，但是多次声明同一函数也是合法的。
// 在给定的作用域中一个形参只能被赋予一次默认参数。
// 换句话说，函数的后续声明只能为之前那些没有默认值的形参添加默认值，
// 而且该形参右侧的所有形参必须都有默认值。
void func_5(int a, int b, int c = 80);
void func_5(int a, int b = 40, int c);  // 第二次声明不能再给c指定默认参数
void func_5(int a = 20, int b, int c) {
  std::cout << a << "," << b << "," << c << std::endl;
}

void func1() {
  func_2(1);        // 1,10,36
  func_2(1, 2);     // 1,2,36
  func_2(1, 2, 3);  // 1,2,3
}
void func2() {
  // 默认值使用的是func_3声明中的默认值，是因为执行的代码和函数声明在同一作用域：
  func_3(1);        // 1,20,40
  func_3(1, 2);     // 1,2,40
  func_3(1, 2, 3);  // 1,2,3
}
void func3() {
  // func_4的定义和func_3的定义在同一作用域，所以使用func3定义中的默认参数：
  func_4();  // 按同样的方式调用func_3
  // 1,30,60
  // 1,2,60
  // 1,2,3
}
void func4() {
  func_5();         // 20,40,80
  func_5(1);        // 1,40,80
  func_5(1, 2);     // 1,2,80
  func_5(1, 2, 3);  // 1,2,3
}
}  // namespace n2

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func1();
      break;
    case 2:
      n2::func2();
      break;
    case 3:
      n2::func3();
      break;
    case 4:
      n2::func4();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}
