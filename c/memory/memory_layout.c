#include <stdio.h>

// 程序内存在地址空间中的分布情况称为内存模型（Memory Model）。内存模型由操作系统构建，在Linux和Windows下有所差异，并且会受到编译模式的影响。

// 内核空间和用户空间
// 对于32位环境，理论上程序可以拥有4GB的虚拟地址空间，在C语言中使用到的变量、函数、字符串等都会对应内存中的一块区域。
// 但是，在这4GB的地址空间中，要拿出一部分给操作系统内核使用，应用程序无法直接访问这一段内存，这一部分内存地址被称为内核空间（Kernel Space）。
// Windows在默认情况下会将高地址的2GB空间分配给内核（也可以配置为1GB），而Linux默认情况下会将高地址的1GB空间分配给内核。
// 也就是说，应用程序只能使用剩下的2GB或3GB的地址空间，称为用户空间（User Space）。

/* Linux下32位环境的一种经典内存模型：
 0xffffffff +--------------------------+
            |        内核空间           |
 0xc0000000 +--------------------------+  ----------------
            |       栈（向下增长）       |
            +--------------------------+
            |            ↓             |
            |       未被分配的内存       |
            |            ↑             |
            +--------------------------+
            |        动态链接库          |  这部分内存会随程序
 0x40000000 +--------------------------+  运行不断分配和释放
            |       未被分配的内存       |
            |            ↑             |
            +--------------------------+
            |       堆（向上增长）       |
            +--------------------------+  ----------------
            |        全局数据区          |  可读性
            |    （全局变量、静态变量）    |  数据
            +--------------------------+  ---这部分内存运行期间一直存在
↑虚拟        |          常量区           |
|地址        |   （一般常量、字符串常量）   |  只读
|增长        +--------------------------+  数据
|方向        |        程序代码区          |
|0x08048000 +--------------------------+  ----------------
|           |         保留区域           |
|0x00000000 +--------------------------+
*/
// 对各个内存分区的说明：
//  程序代码区(code) 存放函数体的二进制代码。一个C语言程序由多个函数构成，C语言程序的执行就是函数之间的相互调用。
//  常量区(constant) 存放一般的常量、字符串常量等。这块内存只有读取权限，没有写入权限，因此它们的值在程序运行期间不能改变。
//  全局数据区(global data) 存放全局变量、静态变量等。这块内存有读写权限，因此它们的值在程序运行期间可以任意改变。
//  堆区(heap) 一般由程序员分配和释放，若程序员不释放，程序运行结束时由操作系统回收。malloc()、calloc()、free()等函数操作的就是这块内存，这也是本章要讲解的重点。
//            注意：这里所说的堆区与数据结构中的堆不是一个概念，堆区的分配方式倒是类似于链表。
//  动态链接库 用于在程序运行期间加载和卸载动态链接库。
//  栈区(stack) 存放函数的参数值、局部变量的值等，其操作方式类似于数据结构中的栈。
// 在这些内存分区中（暂时不讨论动态链接库），程序代码区用来保存指令，常量区、全局数据区、堆、栈都用来保存数据。对内存的研究，重点是对数据分区的研究。
// 程序代码区、常量区、全局数据区在程序加载到内存后就分配好了，并且在程序运行期间一直存在，不能销毁也不能增加（大小已被固定），只能等到程序运行结束后由操作系统收回，
// 所以全局变量、字符串常量等在程序的任何地方都能访问，因为它们的内存一直都在。
// 常量区和全局数据区有时也被合称为静态数据区，意思是这段内存专门用来保存数据，在程序运行期间一直存在。
// 函数被调用时，会将参数、局部变量、返回地址等与函数相关的信息压入栈中，函数执行结束后，这些信息都将被销毁。所以局部变量、参数只在当前函数中有效，不能传递到函数外部，
// 因为它们的内存不在了。
// 常量区、全局数据区、栈上的内存由系统自动分配和释放，不能由程序员控制。程序员唯一能控制的内存区域就是堆（Heap）：它是一块巨大的内存空间，常常占据整个虚拟空间的绝大
// 部分，在这片空间中，程序可以申请一块内存，并自由地使用（放入任何数据）。堆内存在程序主动释放之前会一直存在，不随函数的结束而失效。在函数内部产生的数据只要放到堆中，
// 就可以在函数外部使用。

/* Linux下64位环境的用户空间内存分布情况
 ---------- +--------------------------+ 0xFFFFFFFFFFFFFFFF
    128TB   |        内核空间           |
 ---------- +--------------------------+ 0xFFFF800000000000
            |        未定义区域          |
 ---------- +--------------------------+
            |       栈（向下增长）        |
            +--------------------------+
            |            ↓             |
            |       未被分配的内存       |
            |            ↑             |
            +--------------------------+
            |        动态链接库          |
            +--------------------------+ 0x00002AAAAAAAA000
            |       未被分配的内存       |
            |            ↑             |
    128TB   +--------------------------+
            |       堆（向上增长）       |
            +--------------------------+
            |        全局数据区          |
            |    （全局变量、静态变量）    |
            +--------------------------+
            |          常量区           |
            |   （一般常量、字符串常量）   |
            +--------------------------+
            |        程序代码区          |
            +--------------------------+ 0x0000000000400000
            |         保留区域           |
 ---------- +--------------------------+ 0x0
*/
// 在64位环境下，虚拟地址空间大小为256TB，Linux将高128TB的空间分配给内核使用，而将低128TB的空间分配给用户程序使用。
// 在64位环境下，虚拟地址虽然占用64位，但只有最低48位有效。这里需要补充的一点是，任何虚拟地址的48位至63位必须与47位一致。

// 在32位环境下，Windows默认会将高地址的2GB空间分配给内核（也可以配置为1GB），而将剩下的2GB空间分配给用户程序。
// 不像Linux，Windows是闭源的，有版权保护，资料较少，不好深入研究每一个细节，至今仍有一些内部原理不被大家知晓。
// 关于Windows地址空间的内存分布，官网上只给出了简单的说明：
//  对于32位程序，内核占用较高的2GB，剩下的2GB分配给用户程序；
//  对于64位程序，内核占用最高的248TB，用户程序占用最低的8TB。

// 简单来说，一个可执行程序就是一个进程，使用C语言编译生成的程序，运行后就是一个进程。进程最显著的特点就是拥有独立的地址空间。
// 严格来说，程序是存储在磁盘上的一个文件，是指令和数据的集合，是一个静态的概念；进程是程序加载到内存运行后一些列的活动，是一个动态的概念。
// 应该说“进程的地址空间”。一个进程对应一个地址空间，而一个程序可能会创建多个进程。
// 内核模式和用户模式
//  内核空间存放的是操作系统内核代码和数据，是被所有程序共享的，在程序中修改内核空间中的数据不仅会影响操作系统本身的稳定性，还会影响其他程序，这是非常危险的行为，
//  所以操作系统禁止用户程序直接访问内核空间。
//  要想访问内核空间，必须借助操作系统提供的API函数，执行内核提供的代码，让内核自己来访问，这样才能保证内核空间的数据不会被随意修改，才能保证操作系统本身和其他程序的稳定性。
//  用户程序调用系统API函数称为系统调用（System Call）；发生系统调用时会暂停用户程序，转而执行内核代码（内核也是程序），访问内核空间，这称为内核模式（Kernel Mode）。
//  用户空间保存的是应用程序的代码和数据，是程序私有的，其他程序一般无法访问。当执行应用程序自己的代码时，称为用户模式（User Mode）。
//  计算机会经常在内核模式和用户模式之间切换：
//   当运行在用户模式的应用程序需要输入输出、申请内存等比较底层的操作时，就必须调用操作系统提供的API函数，从而进入内核模式；
//   操作完成后，继续执行应用程序的代码，就又回到了用户模式。
//  总结：用户模式就是执行应用程序代码，访问用户空间；内核模式就是执行内核代码，访问内核空间（当然也有权限访问用户空间）。
// 为什么要区分两种模式
//  内核最主要的任务是管理硬件，包括显示器、键盘、鼠标、内存、硬盘等，并且内核也提供了接口（也就是函数），供上层程序使用。当程序要进行输入输出、分配内存、响应鼠标等与硬件有关的
//  操作时，必须要使用内核提供的接口。但是用户程序是非常不安全的，内核对用户程序也是充分不信任的，当程序调用内核接口时，内核要做各种校验，以防止出错。
//  从Intel 80386开始，出于安全性和稳定性的考虑，CPU可以运行在ring0 ~ ring3四个不同的权限级别，也对数据提供相应的四个保护级别。不过Linux和Windows只利用了其中的两个运行级别：
//   一个是内核模式，对应ring0级，操作系统的核心部分和设备驱动都运行在该模式下。
//   另一个是用户模式，对应ring3级，操作系统的用户接口部分（例如Windows API）以及所有的用户程序都运行在该级别。
//  为什么内核和用户程序要共用地址空间
//   既然内核也是一个应用程序，为何不让它拥有独立的4GB地址空间，而是要和用户程序共享、占用有限的内存呢？
//   让内核拥有完全独立的地址空间，就是让内核处于一个独立的进程中，这样每次进行系统调用都需要切换进程。切换进程的消耗是巨大的，不仅需要寄存器进栈出栈，还会使CPU中的数据缓存失效、
//   MMU中的页表缓存失效，这将导致内存的访问在一段时间内相当低效。
//   而让内核和用户程序共享地址空间，发生系统调用时进行的是模式切换，模式切换仅仅需要寄存器进栈出栈，不会导致缓存失效；现代CPU也都提供了快速进出内核模式的指令，与进程切换比起来，
//   效率大大提高了。

char *str1 = "c.biancheng.net";  // 字符串在常量区，str1在全局数据区
int n;                           // 全局数据区
char *func() {
  char *str = "C语言中文网";  // 字符串在常量区，str在栈区
  return str;
}

int main() {
  int *p;
  printf("pointer %d\n", sizeof(p));
  int a;                   // 栈区
  char *str2 = "01234";    // 字符串在常量区，str2在栈区
  char arr[20] = "56789";  // 字符串和arr都在栈区
  char *pstr = func();     // 栈区
  int b;                   // 栈区
  printf("str1: %#X\npstr: %#X\nstr2: %#X\n", str1, pstr, str2);
  puts("--------------");
  printf("&str1: %#X\n   &n: %#X\n", &str1, &n);
  puts("--------------");
  printf("  &a: %#X\n arr: %#X\n  &b: %#X\n", &a, arr, &b);
  puts("--------------");
  printf("n: %d\na :%d\nb: %d\n", n, a, b);
  puts("--------------");
  printf("%s\n", pstr);
  // gcc -m32 memory_layout.c
  // pointer 4
  // str1: 0X8048660
  // pstr: 0X8048670
  // str2: 0X804868D
  // --------------
  // &str1: 0X804A024
  //    &n: 0X804A02C
  // --------------
  //   &a: 0XFFCF5718
  //  arr: 0XFFCF5728
  //   &b: 0XFFCF571C
  // --------------
  // n: 0
  // a :-134847932
  // b: -136523524
  // --------------
  // C语言中文网

  // gcc memory_layout.c
  // pointer 8
  // str1: 0X400798
  // pstr: 0X4007A8
  // str2: 0X4007C5
  // --------------
  // &str1: 0X601048
  //    &n: 0X601054
  // --------------
  //   &a: 0X8EE7F168
  //  arr: 0X8EE7F180
  //   &b: 0X8EE7F16C
  // --------------
  // n: 0
  // a :4196189
  // b: 0
  // --------------
  // C语言中文网

  // 对代码的说明：
  // 1) 全局变量的内存在编译时就已经分配好了，它的默认初始值是 0（它所占用的每一个字节都是0值），局部变量的内存在函数调用时分配，它默认初始值是不确定的，
  //    由编译器决定，一般是垃圾值，这在《用一个实例来深入剖析函数进栈出栈的过程》中会详细讲解。
  // 2) 函数func()中的局部字符串常量"C语言中文网"也被存储到常量区，不会随着func()的运行结束而销毁，所以最后依然能够输出。
  // 3) 字符数组arr[20]在栈区分配内存，字符串"56789"就保存在这块内存中，而不是在常量区，大家要注意区分。

  return 0;
}