#include <iostream>
#include "demo.h"

using namespace std;

// 多文件编程中代码的划分原则是：将变量、函数或者类的声明部分存放在.h文件，对应的实现部分放在.cpp文件中。此规律适用于大部分场景，但const常量是一个例外。
// 用const修饰的变量必须在定义的同时进行初始化操作（除非用extern修饰）。
// 与此同时，C++中const关键字的功能有2个，除了表明其修饰的变量为常量外，还将所修饰变量的可见范围限制为当前文件。
// 这意味着，除非const常量的定义和main主函数位于同一个.cpp文件，否则该const常量只能在其所在的.cpp文件中使用。

// 那么，如何定义const常量，才能在其他文件中使用呢？有3种在C++多文件编程中定义const常量的方法。
// 1) 一种最常用也最简单的方法，就是将const常量定义在.h文件中。
//    显然此方式违背了“声明位于.h文件，定义（实现）位于.cpp文件”的规律。在C++多文件编程中，还有2种特殊情况是违背此规律的，分别是类的定义和内联函数的定义，
//    s通常情况下它们也都定义在.h文件中。使用此方式定义的const常量，只需引入其头文件，就可以使用它。
// 2) 借助extern先声明再定义const常量。
//    const常量的定义也可以遵循“声明在.h文件，定义在.cpp文件”，借助extern关键字即可。
//    C++ const关键字会限定变量的可见范围为当前文件，即无法在其它文件中使用该常量。
//    而extern关键字会const限定可见范围的功能，它可以使const常量的可见范围恢复至整个项目。
// 3) 借助extern直接定义const常量
//    第2种方法demo.cpp和main.cpp中都引入了demo.h，而demo.h文件中只包含对const num常量的声明。
//    要知道C++编译器在运行项目时，会在预处理阶段直接将#include引入的头文件替换成该头文件中的内容（就可以理解为傻瓜式的复制粘贴）。
//    显然相比第二种方法，此项目中省略了demo.h头文件的创建，一定程序上提高了项目的编译效率。
// 3种在多文件编程中使用const常量的方法，相比后2种借助extern修饰const常量的方式，第一种方式更简单、更常用，推荐使用。

extern const int num3;
int main() {
  std::cout << num1 << std::endl;  // 10
  std::cout << num2 << std::endl;  // 20
  std::cout << num3 << std::endl;  // 30

  return 0;
}