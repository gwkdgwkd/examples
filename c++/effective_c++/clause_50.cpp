#include <cstdlib>
#include <iostream>

using namespace std;

// 了解new和delete的合理替换时机

// 有人想要替换编译器提供的operator new或operator delete，
// 下面是三个最常见的理由：
// 1.用来检测运用上的错误
//   如果将new的内存delete时不幸失败，会导致内存泄露。
//   如果多次delete则会导致不确定行为。
// 2.为了强化效率
//   编译器所带的new和delete主要用于一般目的，采取了中庸之道，
//   它对每个场景都适合，但对某些特定的场景没有最佳表现。
// 3 为了收集使用上的统计数据

// 定制一个operator new，协助检查：
// overruns，写入点在分配区块尾端之后；
// underruns写入点在分配区块起始点之前。
static const int signature = 0xDEADBEEEF;
typedef unsigned char Byte;
void *operator new(size_t size) throw(bad_alloc) {
  using namespace std;
  // 增加大小，塞入两个signature：
  size_t realSize = size + 2 * sizeof(int);
  void *pMem = malloc(realSize);
  if (!pMem) throw bad_alloc();

  *(static_cast<int *>(pMem)) = signature;
  *(reinterpret_cast<int *>(static_cast<Byte *>(pMem) + realSize -
                            sizeof(int))) = signature;

  return static_cast<Byte *>(pMem) + sizeof(int);
}
// 上面的函数的缺点主要在于疏忽了这个特殊函数应该具备的坚持C++规矩的态度。
// 比如条款51所说，operator new都应该内含一个循环，
// 反复调用某个new_handling函数，这里却没有。

// 许多计算机体系结构要求特定的类型必须放在特定的内存地址上。
// 如果没有奉行这个约束条件，可能导致运行期硬件异常。
// 有些体系比较慈悲，宣称如果对齐，便提供较佳效率。
// C++要求所有的operator new返回的指针都有对齐（取决于数据类型）。
// malloc就是在这样的要求下工作，
// 所以new返回一个得自malloc的指针是安全的。
// 上面的函数中并未返回一个得自malloc的指针，
// 而是返回一个得自malloc且偏移一个int大小的指针。
// 没人能够保证它的安全。

// Boost程序库的Pool就是这样一个分配器，
// 它对于最常见的分配大量小型对象很有帮助。

// 何时可在全局性的或class专属的基础上合理替换缺省的new和delete：
// 1.用来检测运用错误
// 2.为了收集动态分配内存的使用统计信息
// 3.为了增加分配和归还的速度
// 4.为了降低缺省内存管理器带来的空间额外开销
// 5.为了弥补缺省分配器中的非最佳对齐
// 6.为了将相关对象成簇集中
// 7.为了获得非传统的行为

// 请记住：
// 1.有许多理由需要写个自定义的new和delete，
//   包括改善性能、对heap运用错误进行调试、收集heap适用信息。

int main() { return 0; }