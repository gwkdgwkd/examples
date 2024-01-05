#include <iostream>
#include "student_han.h"
#include "student_li.h"

// C++引入命名空间是为了避免合作开发项目时产生命名冲突，当进行多文件编程时：
// 1.通常是将声明部分放到.h文件中，例如变量、函数和类等；
// 2.将实现部分放到.cpp文件中。
// 在此基础上，如果要给变量、函数或者类指定命名空间，则该命令空间应至少包含它们的声明。
// 所以当进行多文件编程时，命名空间常位于.h头文件中。

// 当类的声明位于指定的命名空间中时，类的外部实现成员方法，需同时注明命名空间名和类名。
namespace n1 {
class A {
 public:
  void func1();
};
}  // namespace n1
void n1::A::func1() { std::cout << "n1::A::func1" << std::endl; }

// 不同的头文件中可以使用的是不同的命名空间，除此之外，
// 不同头文件中也可以使用名称相同的命名空间，但前提是位于该命名空间中的成员必须互不相同。

int main() {
  n1::A a;
  a.func1();  // n1::A::func1

  Li::Student stu1;
  stu1.display();  // Li::display
  Han::Student stu2;
  stu2.display();  // Han::display

  // 虽然同一项目的不同文件中可以定义相同的命名空间，但必须保证空间中的成员互不相同，
  // 否则编译器会报重定义错误，这里的display()和display(int n)为重载函数：
  demo::display();
  demo::display(2);
  std::cout << demo::num << std::endl;  // 20

  return 0;
}