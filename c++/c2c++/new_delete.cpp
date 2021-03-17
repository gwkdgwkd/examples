#include <iostream>

using namespace std;

// 和malloc()一样，new也是在堆区分配内存，必须手动释放，否则只能等到程序运行结束由操作系统回收。
// 为了避免内存泄露，通常new和delete、new[]和delete[]操作符应该成对出现，并且不要和C语言中malloc()、free()一起混用。
// 在C++中，建议使用new和delete来管理内存，它们可以使用C++的一些新特性，最明显的是可以自动调用构造函数和析构函数。

int main() {
  // 在C语言中，动态分配内存用malloc()函数，释放内存用free()函数：
  int *p1 = (int *)malloc(sizeof(int) * 10);
  free(p1);

  // 在C++中，这两个函数仍然可以使用，但是C++又新增了两个关键字，new和delete：new用来动态分配内存，delete用来释放内存。
  int *p2 = new int;  // new操作符会根据后面的数据类型来推断所需空间的大小。
  delete p2;

  // 分配一组连续的数据，可以使用new[]：
  int *p3 = new int[10];
  delete[] p3;  // 用new[]分配的内存需要用delete[]释放，它们是一一对应的。

  return 0;
}