#ifdef STDIO
#include <stdio.h>
#else
#include <iomanip>
#include <iostream>
using namespace std;
#endif
const int VALUES = 30000;

// 考虑变更程序库

// 程序库的设计就是一个折衷的过程。
// 理想的程序库应该是短小的、快速的、强大的、灵活的、可扩展的、
// 直观的、普遍适用的、具有良好的支持、没有使用约束、没有错误的。
// 这也是不存在的，为尺寸和速度而进行优化的程序库一般不能被移植。
// 具有大量功能的的程序库不会具有直观性。
// 没有错误的程序库在使用范围上会有限制。
// 真实的世界里，你不能拥有每一件东西，总得有付出。

// 不同的设计者给这些条件赋予了不同的优先级。
// 他们从而在设计中牺牲了不同的东西。
// 因此一般两个提供相同功能的程序库却有着完全不同的性能特征。
// iostream程序库与C中的stdio相比有几个优点。
// 例如它是类型安全的（type-safe），它是可扩展的。
// 然而在效率方面，iostream程序库总是不如stdio，
// 因为stdio产生的执行文件，
// 与iostream产生的执行文件相比尺寸小而且执行速度快。

// 首先考虑执行速度的问题。
// 要想掌握iostream和stdio之间的性能差别，
// 一种方法就是用这两个程序库来运行benchmark程序。
// 做了几种计算机、操作系统和编译器的不同组合，在其上运行这个程序，
// 在每一种情况下都是使用stdio的程序运行得较快。
// 有时它仅仅快一些（大约20％），有时则快很多（接近200％），
// 但是我从来没有遇到过一种iostream的实现，
// 和与其相对应的stdio的实现运行速度一样快。
// 另外，使用stdio的程序的尺寸比与相应的使用iostream的程序要小。

// 一旦你找到软件的瓶颈，你应该知道是否可能通过替换程序库来消除瓶颈。
// 比如如果你的程序有I/O瓶颈，你可以考虑用stdio替代iostream，
// 如果程序在动态分配和释放内存上使用了大量时间，
// 你可以想想是否有其他的operator new和operator delete的实现可用。
// 因为不同的程序库在效率、可扩展性、移植性、
// 类型安全和其他一些领域上蕴含着不同的设计理念，
// 通过变换使用给予性能更多考虑的程序库，你有时可以大幅度地提高软件的效率。

int main() {
  double d;
  for (int n = 1; n <= VALUES; ++n) {
#ifdef STDIO
    scanf("%lf", &d);
    printf("%10.5f", d);
#else
    cin >> d;
    cout << setw(10)                     // 设定field宽度
         << setprecision(5)              // 设置小数位置
         << setiosflags(ios::showpoint)  // keep trailing 0s
         << setiosflags(ios::fixed)      // 使用这些设置
         << d;
#endif
    if (n % 5 == 0) {
#ifdef STDIO
      printf("\n");
#else
      cout << '\n';
#endif
    }
  }
  return 0;
}