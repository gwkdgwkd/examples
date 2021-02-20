#include <iostream>

using namespace std;

// 有的时候，main或者使用exit函数调用结束程序的方式，不是那么令人满意：代码中有很多类，这些类在堆上分配了大量零散的内存，而main、exit调用会导致类的析构
// 函数依次将这些零散的内存还给操作系统，这很费时。实际上，这些堆内存会在进程结束是由操作系统同以回收，这种方式的速度很快。如果这些堆内存对其他程序不产生影
// 响的化，在程序结束时调用类的析构函数往往是无意义的。在这种情况下，我们也许需要更快地推出程序。

// c++11标准引入了quick_exit函数来完成不调用析构函数的程序退出方式。与abort不同的是，quikc_exit不是异常退出，而是和exit一样的正常退出。同时我们可以像
// 用atexit那样，使用at_quick_exit注册函数，在quick_exit时调用。

// at_quick_exit()函数是cstdlib标头的库函数。它用于设置应在快速退出时执行的功能。有时我们需要编写这样的代码(例如释放全局变量占用的内存，关闭文件对象等)，
// 该代码应在快速退出时执行，可以为其编写任何用户定义的代码，并将其设置为使用at_quick_exit()函数的快速退出功能。
// 该函数的返回类型为int，如果该函数成功注册，则返回0；否则非零。

class A {
 public:
  A() { cout << "A" << endl; }
  ~A() { cout << "~A" << endl; }
} a;

void myfunc(void) { cout << "Bye, Bye..." << endl; }

int main() {
  at_quick_exit(myfunc);

  // quick exit
  quick_exit(EXIT_SUCCESS);

  // A
  // Bye, Bye...

  cout << "End of the main()..." << endl;
  return 0;

  // A
  // End of the main()...
  // ~A
}