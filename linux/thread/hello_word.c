#include <pthread.h>
#include <stdio.h>

// 所有线程共享的进程资源有：
// 1.代码，即应用程序的代码；
// 2.数据，包括全局变量、函数内的静态变量、堆空间的数据等；
// 3.进程空间，操作系统分配给进程的内存空间；
// 4.打开的文件，各个线程打开的文件资源，也可以为所有线程所共享，
//   例如线程A打开的文件允许线程B进行读写操作。

// 各个线程也可以拥有自己的私有资源，
// 包括寄存器中存储的数据、线程执行所需的局部变量（函数参数）等。

// 进程仅负责为各个线程提供所需的资源，真正执行任务的是线程，而不是进程。

// 所谓多线程，即一个进程中拥有多（≥2）个线程，
// 线程之间相互协作、共同执行一个应用程序。

// 大多数操作系统都支持同时执行多个程序，
// 包括常见的Windows、Linux、Mac OS X操作系统等。
// 为了避免多个程序访问系统资源（包括文件资源、I/O设备、网络等）时产生冲突，
// 操作系统会将可能产生冲突的系统资源保护起来，阻止应用程序直接访问。
// 如果程序中需要访问被操作系统保护起来的资源，
// 需使用操作系统规定的方法（函数、命令），
// 习惯将这些调用方法（函数、命令）称为接口，
// Application Programming Interface，简称API。
// 无论用哪种编程语言编写多线程程序，最终都要借助操作系统预留的接口实现。

// POSIX标准
// POSIX标准全称Portable Operating System Interface，
// 中文译为可移植操作系统接口，最后的字母X代指类UNIX操作系统。
// 简单地理解，POSIX标准发布的初衷就是为了统一所有类UNIX操作系统的接口，
// 这意味着，只要我们编写的程序严格按照POSIX标准调用系统接口，
// 它就可以在任何兼容POSIX标准的类UNIX系统上运行。
// 类UNIX系统有很多种版本，包括Linux、FreeBSD、OpenBSD等，
// 它们预留的系统调用接口各不相同。
// 但幸运的是，几乎所有的类UNIX系统都兼容POSIX标准。

// 所谓兼容，很多支持POSIX标准的类UNIX操作系统并没有从根本上修改自己的API，
// 它们仅仅通过对现有的API进行再封装，生成了一套符合POSIX标准的系统接口，
// 进而间接地支持POSIX标准。

// 值得一提的是，POSIX标准中规范了与多线程相关的系统接口。
// 在Linux系统上编写多线程程序，只需在程序中引入<pthread.h>头文件，
// 调用该文件中包含的函数即可实现多线程编程。

// pthread.h头文件中只包含各个函数的声明部分，
// 具体实现位于libpthread.a库中。

void* Thread1(void* arg) {
  printf("http://c.biancheng.net\n");
  return "Thread1成功执行";
}
void* Thread2(void* arg) {
  printf("C语言中文网\n");
  return "Thread2成功执行";
}
int main() {
  int res;
  pthread_t mythread1, mythread2;
  void* thread_result;
  // 创建线程：
  // &mythread，要创建的线程；
  // NULL，不修改新建线程的任何属性；
  // ThreadFun，新建线程要执行的任务；
  // NULL，不传递给ThreadFun()函数任何参数；
  // 返回值res为0表示线程创建成功，反之则创建失败。
  res = pthread_create(&mythread1, NULL, Thread1, NULL);
  if (res != 0) {
    printf("线程创建失败");
    return 0;
  }
  res = pthread_create(&mythread2, NULL, Thread2, NULL);
  if (res != 0) {
    printf("线程创建失败");
    return 0;
  }

  // 等待指定线程执行完毕：
  // mtThread，指定等待的线程；
  // &thead_result，接收ThreadFun()函数的返回值，
  // 或者接收pthread_exit()函数指定的值，
  // 返回值res为0表示函数执行成功，反之则执行失败。
  res = pthread_join(mythread1, &thread_result);
  printf("%s\n", (char*)thread_result);

  res = pthread_join(mythread2, &thread_result);
  printf("%s\n", (char*)thread_result);

  printf("主线程执行完毕\n");
  return 0;
}

// gcc hello_word.c -lpthread
