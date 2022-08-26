#include <stdio.h>

// 多文件编程就是把多个头文件和源文件组合在一起构成一个程序，
// 这是C语言的重点，也是C语言的难点。
// 全局变量的作用范围不是从变量定义处到该文件结束，在其他文件中也有效。

// extern关键字，用来声明一个变量或函数。
// C语言代码是由上到下依次执行的，不管是变量还是函数，
// 原则上都要先定义再使用，否则就会报错。
// 但在实际开发中，经常会在函数或变量定义之前就使用它们，这时就需要提前声明。
// 所谓声明（Declaration），就是告诉编译器我要使用这个变量或函数，
// 你现在没有找到它的定义不要紧，请不要报错，稍后我会把定义补上。
// 1.函数的声明
//   函数的定义有函数体，函数的声明没有函数体，编译器很容易区分定义和声明，
//   所以对于函数声明来说，有没有extern都是一样的。
//   总结起来，函数声明有四种形式：
//   datatype function(datatype1 name1, datatype2 name2, ...);
//   datatype function(datatype1, datatype2, ...);
//   extern datatype function(datatype1 name1, datatype2 name2, ...);
//   extern datatype function(datatype1, datatype2, ...);
// 2.变量的声明
//   变量和函数不同，编译器只能根据extern来区分，
//   有extern才是声明，没有extern就是定义。
//   变量的定义有两种形式，你可以在定义的同时初始化，也可以不初始化：
//   datatype name = value;
//   datatype name;
//   而变量的声明只有一种形式，就是使用extern关键字：
//   extern datatype name;
//   另外，变量也可以在声明的同时初始化，格式为：
//   extern datatype name = value;
//   这种似是而非的方式是不被推荐的，有的编译器也会给出警告，尽量不要这样写。
// extern是外部的意思，很多教材讲到，
// extern用来声明一个外部（其他文件中）的变量或函数，
// 也就是说，变量或函数的定义在其他文件中。
// 不过这样讲不妥，因为除了定义在外部，定义在当前文件中也是正确的。
// 例如，将module.c中的intm=100;移动到main.c中的任意位置都是可以的。
// 所以我认为，extern是用来声明的，
// 不管具体的定义是在当前文件内部还是外部，都是正确的。

extern void func();
extern int m;
int n = 200;

int main() {
  func();
  printf("m = %d, n = %d\n", m, n);
  return 0;
}

// gcc main.c module.c