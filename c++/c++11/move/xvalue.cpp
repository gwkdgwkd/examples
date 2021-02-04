#include <iostream>

using namespace std;

// 很多初学者都感觉右值引用晦涩难懂，其实不然。右值引用只不过是一种新的C++语法，真正理解起来有难度的是基于右值引用引申出的2种C++编程技巧，分别为移动语义和完美转发。

int main() {
  // C++左值和右值:
  // 在C++或者C语言中，一个表达式（可以是字面量、变量、对象、函数的返回值等）根据其使用场景不同，分为左值表达式和右值表达式。
  // 确切的说C++中左值和右值的概念是从C语言继承过来的。
  // 值得一提的是，左值的英文简写为“lvalue”，右值的英文简写为“rvalue”。很多人认为它们分别是"left value"、"right value"的缩写，其实不然。
  // lvalue 是“loactor value”的缩写，可意为存储在内存中、有明确存储地址（可寻址）的数据，而rvalue译为"read value"，指的是那些可以提供数
  // 据值的数据（不一定可以寻址，例如存储于寄存器中的数据）。

  // 在C++中，一个表达式不是左值就是右值。通常情况下，判断某个表达式是左值还是右值，最常用的有以下2种方法(只适用于大部分场景):
  // 1) 可位于赋值号（=）左侧的表达式就是左值；反之，只能位于赋值号右侧的表达式就是右值：
  int a = 5;  // 其中，变量a就是一个左值，而字面量5就是一个右值。
  // 5 = a; // 错误，5不能为左值。
  // C++ 中的左值也可以当做右值使用：
  int b = 10;  // b是一个左值
  a = b;       // a、b都是左值，只不过将b可以当做右值使用

  // 2) 有名称的、可以获取到存储地址的表达式即为左值；反之则是右值。
  // a和b是变量名，且通过&a和&b可以获得他们的存储地址，因此a和b都是左值；
  // 反之，字面量5、10，它们既没有名称，也无法获取其存储地址（字面量通常存储在寄存器中，或者和代码存储在一起），因此5、10都是右值。

  // C++右值引用:
  // 右值引用可以从字面意思上理解，指的是以引用传递（而非值传递）的方式使用C++右值。
  // 其实C++98/03标准中就有引用，使用"&"表示。但此种引用方式有一个缺陷，即正常情况下只能操作C++中的左值，无法对右值添加引用:
  int num = 10;
  // int &c = 10; // 错误
  int &b1 = num;  // 正确

  // 编译器允许我们为num左值建立一个引用，但不可以为10这个右值建立引用。因此，C++98/03标准中的引用又称为左值引用。
  // 虽然C++98/03标准不支持为右值建立非常量左值引用，但允许使用常量左值引用操作右值。也就是说，常量左值引用既可以操作左值，也可以操作右值:
  const int &b2 = num;
  const int &c = 10;
  // 右值往往是没有名称的，因此要使用它只能借助引用的方式。这就产生一个问题，可能需要对右值进行修改（实现移动语义时就需要），显然左值引用的方式是行不通的。
  // 为此，C++11标准新引入了另一种引用方式，称为右值引用，用"&&"表示。
  // C++标准委员会在选定右值引用符号时，既希望能选用现有C++内部已有的符号，还不能与C++98/03 标准产生冲突，最终选定了2个'&'表示右值引用。
  // 需要注意的，和声明左值引用一样，右值引用也必须立即进行初始化操作，且只能使用右值进行初始化:
  // int && a = num;  // 右值引用不能初始化为左值
  int &&a1 = 10;
  // 和常量左值引用不同的是，右值引用还可以对右值进行修改:
  a1 = 100;
  cout << a1 << endl;  // 100

  // C++语法上是支持定义常量右值引用的:
  const int &&a2 = 10;  // 编译器不会报错
  // 但这种定义出来的右值引用并无实际用处。一方面，右值引用主要用于移动语义和完美转发，其中前者需要有修改右值的权限；
  // 其次，常量右值引用的作用就是引用一个不可修改的右值，这项工作完全可以交给常量左值引用完成。

  // 引用类型 	      可以引用的值类型 	                            使用场景
  //                 非常量左值 	常量左值 	非常量右值 	常量右值
  // 非常量左值引用 	 Y 	         N 	      N 	      N 	        无
  // 常量左值引用 	   Y 	         Y 	      Y 	      Y 	        常用于类中构建拷贝构造函数
  // 非常量右值引用 	 N 	         N 	      Y 	      N 	        移动语义、完美转发
  // 常量右值引用 	   N 	         N 	      Y 	      Y 	        无实际用途

  // C++11标准中对右值做了更细致的划分，分别称为纯右值（Pure value，简称pvalue）和将亡值（eXpiring value，简称xvalue）。
  // 其中纯右值就是C++98/03标准中的右值，而将亡值则指的是和右值引用相关的表达式（比如某函数返回的T &&类型的表达式）。对于纯右值和将亡值，都属于右值。

  return 0;
}