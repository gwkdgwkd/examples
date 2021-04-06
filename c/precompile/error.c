#include <stdio.h>

// #error指令用于在编译期间产生错误信息，并阻止程序的编译，其形式如下：
// #error error_message
// 这和发生语法错误的效果是一样的，程序编译失败。
// 需要注意的是：报错信息不需要加引号""，如果加上，引号会被一起输出。

#ifndef WIN32
#error This programme cannot compile at Linux Platform
#endif

#ifndef __cplusplus
#error 当前程序必须以C++方式编译
#endif

int main() { return 0; }