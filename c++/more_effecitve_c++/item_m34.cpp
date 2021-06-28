#include <iostream>

using namespace std;

// 如何在同一程序中混合使用C++和C

// 许多年来，你一直担心编制程序时一部分使用C++一部分使用C，就如同在全部用C编程的年代同时使用多个编译器来生成程序一样。
// 没办法多编译器编程的，除非不同的编译器在与实现相关的特性（如int和double的字节大小，传参方式）上相同。
// 但这个问题在语言的标准化中被忽略了，所以唯一的办法就是两个编译器的生产商承诺它们间兼容。
// C++和C混合编程时同样是这个问题，所以在实体混合编程前，确保你的C++编译器和C编译器兼容。

// 确认兼容后，还有四个要考虑的问题：名变换，静态初始化，内存动态分配，数据结构兼容。

// 名变换
// 名变换，就是C++编译器给程序的每个函数换一个独一无二的名字。
// 在C中，这个过程是不需要的，因为没有函数重载，但几乎所有C++程序都有函数重名（例如，流运行库就申明了几个版本的 operator<<和 operator>>）。
// 重载不兼容于绝大部分链接程序，因为链接程序通常无法分辨同名的函数。名变换是对链接程序的妥协；链接程序通常坚持函数名必须独一无二。
// 如果只在C++范围内，名变换不会影响你。如果你你有一个函数叫drawline而编译器将它变换为xyzzy，你总使用名字drawLine，不会注意到背后的obj文件引用的是xyzzy的。
// 如果drawLine位于C运行库中，那就是一个不同的故事了。你的C++源文件包含的头文件中申明为:
// void drawLine(int x1, int y1, int x2, int y2);
// 代码体中通常也是调用drawLine。每个这样的调用都被编译器转换为调用名变换后的函数，所以写下的是
// drawLine(a, b, c, d); // call to unmangled function name
// obj文件中调用的是:
// xyzzy(a, b, c, d); // call to mangled function mame
// 但如果drawLine是一个C函数，obj文件（或者是动态链接库之类的文件）中包含的编译后的drawLine函数仍然叫drawLine；没有名变换动作。
// 当你试图将obj文件链接为程序时，将得到一个错误，因为链接程序在寻找一个叫xyzzy的函数，而没有这样的函数存在。
// 要解决这个问题，你需要一种方法来告诉C++编译器不要在这个函数上进行名变换。你不期望对用其它语言写的函数进行名变换，如C、汇编、Fortran、LISP、Forth或其它。
// 总之，如果你调用一个名字为drawLine的C函数，它实际上就叫drawLine，你的obj文件应该包含这样的一个引用，而不是引用进行了名变换的版本。
// 要禁止名变换，使用C++的extern 'C'指示：
extern "C" void drawLine(int x1, int y1, int x2, int y2);
// 不要以为有一个 extern 'C'，那么就应该同样有一个extern 'Pascal'和extern 'FORTRAN'。没有，至少在C++标准中没有。
// 不要将extern 'C'看作是申明这个函数是用C语言写的，应该看作是申明在个函数应该被当作好象C写的一样而进行调用。
// 使用术语就是，extern 'C'意思是这个函数有C链接，但这个意思表达实在不怎么清晰。不管如何，它总意味着一件事：名变换被禁止了。
// 如果不幸到必须要用汇编写一个函数，你也可以申明它为 extern 'C'：
// extern "C" void twiddleBits(unsigned char bits);
// 你甚至可以在C++函数上申明extern 'C'。这在你用C++写一个库给使用其它语言的客户使用时有用。
// 通过禁止这些C++函数的名变换，你的客户可以使用你选择的自然而直观的名字，而不用使用你的编译生成的变换后的名字。
// 经常，你有一堆函数不想进行名变换，为每一个函数添加extern 'C'是痛苦的。幸好，这没必要。extern 'C'可以对一组函数生效，只要将它们放入一对大括号中：
extern "C" {
void twiddleBits(unsigned char bits);
void simulate(int iterations);
}
// 这样使用extern 'C'简化了维护那些必须同时供C++和C使用的头文件的工作。当用C++编译时，你应该加extern 'C'，但用C编译时，不应该这样。
// 通过只在C++编译器下定义的宏__cplusplus，你可以将头文件组织得这样：
#ifdef __cplusplus
extern "C" {
#endif
void drawLine(int x1, int y1, int x2, int y2);
void twiddleBits(unsigned char bits);
void simulate(int iterations);
#ifdef __cplusplus
}
#endif
// 顺便提一下，没有标准的名变换规则。不同的编译器可以随意使用不同的变换方式，而事实上不同的编译器也是这么做的。
// 这是一件好事。如果所有的编译器使用同样的变换规则，你会误认为它们生成的代码是兼容的。
// 现在，如果混合链接来自于不同编译器的obj文件，极可能得到应该链接错误，因为变换后的名字不匹配。
// 这个错误暗示了，你可能还有其它兼容性问题，早些找到它比以后找到要好。

// 静态初始化
// 在掌握了名变换后，你需要面对一个C++中事实：在main执行前和执行后都有大量代码被执行。
// 尤其是，静态的类对象和定义在全局的、命名空间中的或文件体中的类对象的构造函数通常在main被执行前就被调用。这个过程称为静态初始化。
// 这和我们对C++和C程序的通常认识相反，我们一直把main当作程序的入口。
// 同样，通过静态初始化产生的对象也要在静态析构过程中调用其析构函数；这个过程通常发生在main结束运行之后。
// 为了解决main()应该首先被调用，而对象又需要在main()执行前被构造的两难问题，许多编译器在main()的最开始处插入了一个特别的函数，由它来负责静态初始化。
// 同样地，编译器在main()结束处插入了一个函数来析构静态对象。产生的代码通常看起来象这样：
// int main(int argc, char *argv[]) {
//   performStaticInitialization();  // generated by the
//   // implementation
//   the statements you put in main go here;
//   performStaticDestruction();  // generated by the
//                                // implementation
// }
// 不要注重于这些名字。函数performStaticInitialization()和performStaticDestruction()通常是更含糊的名字，甚至是内联函数（这时在你的obj文件中将找不到这些函数）。
// 要点是：如果一个C++编译器采用这种方法来初始化和析构静态对象，除非main()是用C++写的，这些对象将从没被初始化和析构。
// 因为这种初始化和析构静态对象的方法是如此通用，只要程序的任意部分是C++写的，你就应该用C++写main()函数。
// 有时看起来用C写main()更有意义，比如程序的大部分是C的，C++部分只是一个支持库。
// 然而，这个C++库很可能含有静态对象（即使现在没有，以后可能会有），所以用C++写main()仍然是个好主意。这并不意味着你需要重写你的C代码。
// 只要将C写的main()改名为realMain()，然后用C++版本的main()调用realMain()：
// extern "C" int realMain(int argc, char *argv[]);  // function in C
// int main(int argc, char *argv[]) { return realMain(argc, argv); }
// 这么做时，最好加上注释来解释原因。
// 如果不能用C++写main()，你就有麻烦了，因为没有其它办法确保静态对象的构造和析构函数被调用了。
// 不是说没救了，只是处理起来比较麻烦一些。编译器生产商们知道这个问题，几乎全都提供了一个额外的体系来启动静态初始化和静态析构的过程。
// 要知道你的编译器是怎么实现的，挖掘它的随机文档或联系生产商。

// 动态内存分配
// 现在提到动态内存分配。通行规则很简单：C++部分使用new和delete，C部分使用malloc（或其变形）和free。
// 只要new分配的内存使用delete释放，malloc分配的内存用free释放，那么就没问题。
// 用free释放new分配的内存或用delete释放malloc分配的内存，其行为没有定义。
// 那么，唯一要记住的就是：将你的new和delete与mallco和free进行严格的隔离。
// 说比做容易。看一下这个粗糙（但很方便）的strdup函数，它并不在C和C++标准（运行库）中，却很常见：
// char* strdup(const char *ps);
// 要想没有内存泄漏，strdup的调用着必须释放在strdup()中分配的内存。但这内存这么释放？
// 用delete？用free？如果你调用的strdup来自于C函数库中，那么是后者。如果它是用C++写的，那么恐怕是前者。
// 在调用strdup后所需要做的操作，在不同的操作系统下不同，在不同的编译器下也不同。
// 要减少这种可移植性问题，尽可能避免调用那些既不在标准运行库中也没有固定形式（在大多数计算机平台下）的函数。

// 数据结构的兼容性
// 最后一个问题是在C++和C之间传递数据。不可能让C的函数了解C++的特性的，它们的交互必须限定在C可表示的概念上。
// 因此，很清楚，没有可移植的方法来传递对象或传递指向成员函数的指针给C写的函数。
// 但是，C了解普通指针，所以想让你的C++和C编译器生产兼容的输出，两种语言间的函数可以安全地交换指向对象的指针和指向非成员的函数或静态成员函数的指针。
// 自然地，结构和内建类型（如int、char等）的变量也可自由通过。
// 因为C++中的struct的规则兼容了C中的规则，假设“在两类编译器下定义的同一结构将按同样的方式进行处理”是安全的。这样的结构可以在 C++和 C 见安全地来回传递。
// 如果你在C++版本中增加了非虚函数，其内存结构没有改变，所以，只有非虚函数的结构（或类）的对象兼容于它们在C中的孪生版本（其定义只是去掉了这些成员函数的申明）。
// 增加虚函数将结束游戏，因为其对象将使用一个不同的内存结构。从其它结构（或类）进行继承的结构，通常也改变其内存结构，所以有基类的结构也不能与C函数交互。
// 就数据结构而言，结论是：在C++和C之间这样相互传递数据结构是安全的。在C++和C下提供同样的定义来进行编译。
// 在C++版本中增加非虚成员函数或许不影响兼容性，但几乎其它的改变都将影响兼容。

// 如果想在同一程序下混合C++与C编程，记住下面的指导原则：
//  确保C++和C编译器产生兼容的obj文件。
//  将在两种语言下都使用的函数申明为extern 'C'。
//  只要可能，用C++写main()。
//  总用delete释放new分配的内存；总用free释放malloc分配的内存。
//  将在两种语言间传递的东西限制在用C编译的数据结构的范围内；这些结构的C++版本可以包含非虚成员函数。

int main() { return 0; }