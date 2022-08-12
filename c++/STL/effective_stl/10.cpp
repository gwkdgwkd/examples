#include <iostream>
#include <list>

// 了解分配子（allocator）的约定和限制

// 像new和new[]一样，STL内存分配子负责分配（和释放）原始内存，
// 但是分配子给使用者的接口与new、new[]，甚至malloc一点都不相似。
// 分配子要为它所定义的内存模型中的指针和引用提供类型定义。
// 在C++标准中，一个类型为T的对象，
// 默认分配子（allocator<T>）提供了两个类型定义，
// 分别为allocator<T>::pointer和allocator<T>::reference，
// 用户定义的分配子也应该提供这些类型定义。
// 在C++中，没办法仿冒引用。
// 这需要重载点操作符，而这种重载是被禁止的。
// 创建这种具有引用行为特点的对象是使用代理对象的一个例子，
// 而代理对象会导致很多问题。

// 就STL中的分配子来说，降低指针和引用的有效性并不是代理对象的技术缺陷，
// 事实上，C++标准很明确地指出，
// 允许库实现者假定每个分配子的指针类型等同于T*，
// 而分配子的引用类型就是T&。
// 没错，库实现者可以忽略类型定义，而直接使用指针和引用。
// 所以，即便能找到一个办法，从而可以成功地提供新的指针和引用类型，
// 也是无济于事，因为STL实现可能忽略了类型定义，太简洁了。

// 分配子是对象，这意味着可以有成员函数、
// 嵌套类型和类型定义（如pointer和reference），等待。
// C++标准说，STL的实现可以假定所有属于同一种类型的分配子对象都是等价的，
// 并且相互比较的结果总是相等的。
// template <typename T>
// class SpecialAllocator {};
// class Widget {};
// typedef SpecialAllocator<Widget> SAW;
// list<Widget, SAW> L1;
// list<Widget, SAW> L2;
// L1.splice(L1.begin(), L2);
// list的链接操作没有拷贝任何东西，只有一些指针被调整，
// 这使得操作既快速又是异常安全的。
// 当L1析构时，必须析构自己的所有节点（并释放内存）。
// 由于包含了L2分配的节点，所以，
// L1的分配子必须释放最初由L2的分配子分配的节点。
// 这就解释了为什么C++标准允许STL的实现可以假定同一类型的分配子是等价的了。
// 这样，一个分配子对象分配的内存就可以由另一个分配子对象安全地删除。
// 如果没有这个假设，那么链接操作实现起来就困难很多。
// 这意味着可移植的分配子对象（不同的STL实现下都能正确工作）不可以有状态。
// 也就是说不可以有任何非静态的数据成员，至少不能有会影响行为的数据成员。
// 带状态的分配子在编译时仍可以顺利通过，只不过可能不会按照期望运行。

// void operator new(size_t bytes);
// pointer allocator<T>::allocate(size_type numObjects);
// pointer是个类型定义，总是T*
// new的参数指明需要多少字节，
// 而allocator的参数指明的是内存中要容纳多少个T对象。
// 返回值也不同，new返回void*，而allocator返回的是T*。
// allocator返回的指针并没有指向T对象，因为T尚未构造。
// STL隐含着这样的期望：allocator的调用者会在内存中构造一个或多个T对象。
// 大多数标准容器从来没有单独调用过对应的分配子，list和标准关联容器，
// 因为它们都是基于节点的容器，每当新的值被存入到容器中，新的节点是被动态分配的：
// list<int> L;
// set<Widget, SAW> s;

// 需要记住的内容：
// 1.分配子是一个模板，模板参数T代表你为它分配内存的对象的类型。
// 2.提供类型定义pointer和reference，
//   但是始终pointer为T*，reference为T&。
// 3.千万别让你的分配子拥有随对象而不同的状态。
//   通常，分配子不应该有非静态的数据成员。
// 4.记住，传给分配子的allocate成员函数的是那些要求内存的对象的个数，
//   而不是所需的字节数。
//   返回的T*指针，尚未有T对象被构造出来。
// 5.一定要提供嵌套的rebind模板，因为标准容器依赖该模板。

int main() { return 0; }