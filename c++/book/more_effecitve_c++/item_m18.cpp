#include <iostream>
#include <map>
#include <string>

// 分期摊还期望的计算

// 让程序做的事情比被要求的还要多，通过这种方式来提高软件的性能。
// 核心就是over-eager evaluation，过度热情计算法：
// 在要求做某些事情以前就完成它们。

namespace n1 {
// 用来表示放有大量数字型数据的一个集合：
template <class NumericalType>
class DataCollection {
 public:
  NumericalType min() const;
  NumericalType max() const;
  NumericalType avg() const;
};
// 有三种方法实现这三种函数：
// 1.使用eager evaluation，热情计算法，当min，max和avg函数被调用时，
//   检测集合内所有的数值，然后返回一个合适的值；
// 2.使用lazy evaluation，懒惰计算法，
//   只有确实需要函数的返回值时才要求函数返回能用来确定准确数值的数据结构；
// 3.使用over-eager evaluation，过度热情计算法，随时跟踪目前集合的最小值，
//   最大值和平均值，这样当min，max或avg被调用时，可以不用计算就立刻返回正确的数值。
// 如果频繁调用min，max和avg，把跟踪集合最小值、最大值和平均值的开销分摊到所有函数上，
// 每次函数调用所分摊的开销比eager evaluation或lazy evaluation要小。
}  // namespace n1

namespace n2 {
// 隐藏在over-eager evaluation后面的思想是如果认为一个计算需要频繁进行，
// 就可以设计一个数据结构高效地处理这些计算需求，这样可以降低每次计算需求时的开销。
// 采用over-eager最简单的方法就是缓存那些已经被计算出来而以后还有可能需要的值。
int findCubicleNumber(const std::string& employeeName) {
  // 定义静态map，存储pairs，这个map是local cache：
  typedef std::map<std::string, int> CubicleMap;
  static CubicleMap cubes;
  CubicleMap::iterator it = cubes.find(employeeName);
  if (it == cubes.end()) {
    int cubicle = 5;  // 开销大的操作，比如查找数据库
    cubes[employeeName] = cubicle;
    return cubicle;
  } else {
    return (*it).second;
  }
}
}  // namespace n2

namespace n3 {
// catching是一种分摊期望的计算开销的方法，Prefetching(预提取)是另一种方法。
// 可以把prefech想象成购买大批商品而获得的折扣。
// 例如磁盘控制器从磁盘读取数据时，它们会读取一整块或整个扇区的数据，即使程序仅需要一小块数据。
// 这是因为一次读取一大块数据比在不同时间读取两个或三个小块数据要快。
// 而且经验显示如果需要一个地方的数据，则很可能也需要它旁边的数据，这是位置相关现象。
// 正因为这种现象，系统设计者才有理由为指令和数据使用磁盘cache和内存cache，
// 还有使用指令prefetch，prefetch在高端应用里也有优点。
// 例如要为dynamic数组实现一个模板，dynamic就是开始时具有一定的尺寸，
// 以后可以自动扩展的数组，所以所有非负的索引都是合法的：
template <class T>  // dynamic数组
class DynArray {
 public:
  T& operator[](int index);
};
template <class T>
T& DynArray<T>::operator[](int index) {
  if (index < 0) {
    // throw an exception; // 负数索引仍不合法
  }
  if (index > 5) {  // 当前最大的索引值
    // 调用new分配足够的额外内存，以使得索引合法;
    // 每次需要增加数组长度时，这种方法都要调用new，
    // 但是调用new会触发operator new，
    // operator new和operator delete的调用通常开销很大。
    // 因为它们将导致底层操作系统的调用，系统调用的速度一般比进程内函数调用的速度慢。
    // 因此我们应该尽量少使用系统调用。
  }
  // 返回index位置上的数组元素
}
// 使用Over-eager evaluation方法，其原因是现在必须增加数组的尺寸以容纳索引i，
// 那么根据位置相关性原则可能还会增加数组尺寸以在未来容纳比i大的其它索引。
// 为了避免为扩展而进行第二次（预料中的）内存分配，
// 现在增加DynArray的尺寸比能使i合法的尺寸要大，希望未来的扩展将被包含在提供的范围内。
// template <class T>
// T& DynArray<T>::operator[](int index) {
//   if (index < 0) throw an exception;
//   if (index > 当前最大的索引值) {
//     int diff = index – 当前最大的索引值;
//     调用new分配足够的额外内存，使得index + diff合法;
//   }
//   返回index位置上的数组元素;
// }
// 这个函数每次分配的内存是数组扩展所需内存的两倍。

// 即通过over-eager方法分摊预期计算的开销，例如caching和prefething，
// 并不与lazy evaluation的建议相矛盾，当必须支持某些操作而不总需要其结果时，
// lazy evaluation是在这种时候使用的用以提高程序效率的技术。
// 当你必须支持某些操作而其结果几乎总是被需要或被不止一次地需要时，
// over-eager是在这种时候使用的用以提高程序效率的一种技术。
// 它们所产生的巨大的性能提高证明在这方面花些精力是值得的。

void func() {
  DynArray<double> a;  // 在这时, 只有a[0]是合法的数组元素
  a[22] = 3.5;         // a自动扩展：现在索引0－22是合法的
  a[32] = 0;           // 又自行扩展

  // Over-eager evaluation:
  // DynArray<double> a;  // 仅仅a[0]是合法的
  // a[22] = 3.5;  // 调用new扩展a的存储空间到索引44，a的逻辑尺寸变为23
  // a[32] = 0;  // a的逻辑尺寸被改变，允许使用a[32]，但是没有调用new
}
}  // namespace n3

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}