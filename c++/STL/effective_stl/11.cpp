#include <iostream>
#include <list>

// 理解自定义分配子的合理用法

// STL默认的内存管理器太慢、或者浪费内存，
// 或者在使用STL的情形下导致了太多的内存碎片。
// 下面情形中的每一个都对应了自定义分配子所适合解决的一个问题：
// 1.你相信自己可以做的更好；
// 2.或者发现allocator<T>是线程安全的，
//   而你所感兴趣的是单线程环境下执行，不愿意为同步付出不必要的开销；
// 3.或者指定某些容器中的对象通常是一起使用的，
//   想把它们放在一个特殊堆中的相邻位置上，以便尽可能地做到引用局部化；
// 4.或者你想建立一个与共享内存相对应的特殊的堆，
//   然后在内存中存放一个或多个容器，以便其他进程可以共享这些容器。

// 有一些特殊过程，采用malloc和free内存模型来管理一个位于共享内存的堆：
void mallocShared(size_t byteNeeded);
void freeShared(void* ptr);
// 而想把STL容器的内存放到这块共享内存中去：
// template <typename T>
// class ShareMemoryAllocator {
//  public:
//   pointer allocate<T>(size_type numObjects, const void* localityHint = 0) {
//     return static_cast<pointer>(mallocShared(numObjects * sizeof(T)));
//   }
//   void deallocate(pointer ptrToMemory, size_type numObjects) {
//     freeShared(ptrToMemory);
//   }
// };
// typedef vector<double, ShareMemoryAllocator<double> > SharedDoubleVec;
// SharedDoubleVec v;
// v所分配的用来容纳其元素的内存将来自共享内存。
// 而v自己（包括所有的数据成员）几乎肯定不会位于共享内存中。
// v只是普通的基于栈的对象。
// 如果想把v的内容和v自身都放到共享内存中，需要这样做：
// void* pVectorMemory = mallocShared(sizeof(SharedDoubleVec));
// SharedDoubleVec* pv = new (pVectorMemory) SharedDoubleVec;
// pv->~SharedDoubleVec();
// freeShared(pVectorMemory);

// 假设有两个堆，每个堆都有相应的静态成员函数来执行内存分配和释放操作：
// class Heap1 {
//  public:
//   static void* alloc(size_t numBytes,
//                      const void* memoryBlockToBeNear);
//   static void dealloc(void* ptr);
// };
// class Heap2 {
//  public:
//   static void* alloc(size_t numBytes,
//                      const void* memoryBlockToBeNear);
//   static void dealloc(void* ptr);
// };
// template <typename T, typename Heap>
// class SpecificHeapAllocator {
//  public:
//   pointer allocate<T>(size_type numObjects,
//                       const void* localityHint = 0) {
//     return static_cast<pointer>(
//         Heap::alloc(numObjects * sizeof(T), localityHint));
//   }
//   void deallocate(pointer ptrToMemory, size_type numObjects) {
//     Heap::dealloc(ptrToMemory);
//   }
// };
// vector<int, SpecificHeapAllocator<int, Heap1> > v;
// set<int, SpecificHeapAllocator<int, Heap1> > s;
// list<Widget, SpecificHeapAllocator<Widget, Heap2> > L;
// map<int, string, less<int>,
//     SpecificHeapAllocator<pair<const int, string>, Heap2> > m;
// Heap1和Heap2都是类型而不是对象。
// 如果它们是对象而不是类型，那么将不会是等价的分配子。
// STL提供了使用同一类的不同分配子对象来初始化不同STL容器的语法形式。

// 只要遵守同一类型的分配子必须是等价的这一限制要求，
// 那么，当使用自定义分配子来控制通用的内存管理策略的时候，
// 或者在聚集成员关系的时候，
// 或者在使用共享内存和其他特殊堆的时候，就不会陷入麻烦。

int main() { return 0; }