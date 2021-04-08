#include <stdio.h>

// 在编译和链接之前，还需要对源文件进行一些文本方面的操作，比如文本替换、文件包含、删除部分代码等，这个过程叫做预处理，由预处理程序完成。
// 以#号开头的命令称为预处理命令。
// 编译器会将预处理的结果保存到和源文件同名的.i文件中，例如main.c的预处理结果在main.i中。和.c一样，.i也是文本文件，可以用编辑器打开直接查看内容。
// C语言提供了多种预处理功能，如宏定义、文件包含、条件编译等，合理地使用它们会使编写的程序便于阅读、修改、移植和调试，也有利于模块化程序设计。

// 不同的平台下引入不同的头文件
#if _WIN32  // 识别windows平台
#include <windows.h>
#elif __linux__  // 识别linux平台
#include <unistd.h>
#endif

// #include叫做文件包含命令，用来引入对应的头文件（.h文件）。#include也是C语言预处理命令的一种。
// #include的处理过程很简单，就是将头文件的内容插入到该命令所在的位置，从而把头文件和当前源文件连接成一个源文件，这与复制粘贴的效果相同。
// 使用尖括号<>和双引号""的区别在于头文件的搜索路径不同：
//  使用尖括号<>，编译器会到系统路径下查找头文件；
//  而使用双引号""，编译器首先在当前目录下查找头文件，如果没有找到，再到系统路径下查找。
// 也就是说，使用双引号比使用尖括号多了一个查找路径，它的功能更为强大。
// 关于#include用法的注意事项：
//  一个#include命令只能包含一个头文件，多个头文件需要多个#include命令。
//  同一个头文件可以被多次引入，多次引入的效果和一次引入的效果相同，因为头文件在代码层面有防止重复引入的机制。
//  文件包含允许嵌套，也就是说在一个被包含的文件中又可以包含另一个文件。
// 「在头文件中定义定义函数和全局变量」这种认知是原则性的错误！不管是标准头文件，还是自定义头文件，都只能包含变量和函数的声明，不能包含定义，否则在多次引入时会引起重复定义错误。

// 预处理指令是以#号开头的代码行，#号必须是该行除了任何空白字符外的第一个字符。
// #后是指令关键字，在关键字和#号之间允许存在任意个数的空白字符，整行语句构成了一条预处理指令，该指令将在编译器进行编译之前对源代码做某些转换。
//  # 	      空指令，无任何效果
//  #include 	包含一个源代码文件
//  #define 	定义宏
//  #undef 	  取消已定义的宏
//  #if 	    如果给定条件为真，则编译下面代码
//  #ifdef 	  如果宏已经定义，则编译下面代码
//  #ifndef 	如果宏没有定义，则编译下面代码
//  #elif 	  如果前面的#if给定条件不为真，当前条件为真，则编译下面代码
//  #endif 	  结束一个#if……#else条件编译块

// 预处理功能是C语言特有的功能，它是在对源程序正式编译前由预处理程序完成的，程序员在程序中用预处理命令来调用这些功能。
// 宏定义可以带有参数，宏调用时是以实参代换形参，而不是“值传送”。
// 为了避免宏代换时发生错误，宏定义中的字符串应加括号，字符串中出现的形式参数两边也应加括号。
// 文件包含是预处理的一个重要功能，它可用来把多个源文件连接成一个源文件进行编译，结果将生成一个目标文件。
// 条件编译允许只编译源程序中满足条件的程序段，使生成的目标程序较短，从而减少了内存的开销并提高了程序的效率。
// 使用预处理功能便于程序的修改、阅读、移植和调试，也便于实现模块化程序设计。

int main() {
  // 不同的平台下调用不同的函数
#if _WIN32  // 识别windows平台
  Sleep(5000);
#elif __linux__  // 识别linux平台
  sleep(5);
#endif

  return 0;
}