#include <stdio.h>

// #if、#elif、#else和#endif都是预处理命令。
// 这种能够根据不同情况编译不同代码、产生不同目标文件的机制，称为条件编译。条件编译是预处理程序的功能，不是编译器的功能。

// #if的格式，#elif和#else也可以省略
// #if 整型常量表达式1
//     程序段1
// #elif 整型常量表达式2
//     程序段2
// #elif 整型常量表达式3
//     程序段3
// #else
//     程序段4
// #endif
// #if命令要求判断条件为“整型常量表达式”，也就是说，表达式中不能包含变量，而且结果必须是整数；而if后面的表达式没有限制，只要符合语法就行。这是#if和if的一个重要区别。

// #ifdef用法的一般格式为（也可以省略#else）：
// #ifdef 宏名
//     程序段1
// #else
//     程序段2
// #endif

// #ifndef用法的一般格式为：
// #ifndef 宏名
//     程序段1
// #else
//     程序段2
// #endif

// 最后需要注意的是，#if后面跟的是“整型常量表达式”，而#ifdef和#ifndef后面跟的只能是一个宏名，不能是其他的。
// #ifdef可以认为是#if defined的缩写。

int main() {
  // 开发一个C语言程序，让它输出红色的文字，并且要求跨平台，在Windows和Linux下都能运行，怎么办呢？
  // Windows有专有的宏_WIN32，Linux 有专有的宏__linux__
#if _WIN32
  system("color 0c");
  printf("http://c.biancheng.net\n");
#elif __linux__
  printf("\033[22;31mhttp://c.biancheng.net\n\033[22;30m");
#else
  printf("http://c.biancheng.net\n");
#endif

  return 0;
}