#include <cstdlib>
#include <iostream>

using namespace std;

// 了解new-handler的行为

// 当计算环境（Java和.NET）夸耀自己内置垃圾回收能力的当今，
// C++对内存管理的纯手工法看起来有点老气。

// STL容器所使用的heap内存由容器所拥有的分配器对象管理，
// 不是被new和delete直接管理。

// 当operator new无法满足某一内存分配需求时，它会抛出异常。
// 以前会返回一个null指针。
// 当operator new抛出异常以反映一个未满足的内存需求之前，
// 它会先调用一个客户指定的错误处理函数，一个所谓的new-handler。
// 需要调用set_new_handler函数，声明于<new>的标准程序库函数。

// 请记住：
// 1.set_new_handler允许客户指定一个函数，在内存分配无法满足时被调用。
// 2.Nothrow new是一个颇为局限的工具，因为它只适用于内存分配；
//   后继的构造函数调用还是可能抛出异常。
//   namespace std {
//     定义一个指针指向函数，该函数没有参数也不返回任何东西：
//     typedef void (*new_handler)();
//     // 该函数不抛出任何异常：
//     new_handler set_new_handler(new_handler p) throw();
//   }
void outOfMem() {
  cerr << "Unable to satisfy requeset for memory" << endl;
  abort();
}

// 当operator new无法满足内存申请时，
// 会不断调用new-handler函数，直到找到足够的内存。
// 良好的new-handler函数必须做以下事情：
// 1.让更多的内存可被使用。
//   让下一次内存分配动作可能成功。
//   一般的策略是，程序一开始执行就分配一大块内存，
//   当new-handler第一次被调用，释放内存给程序用。
// 2.安装另一个new-handler。
//   如果目前这个new-handler无法取得更多可用内存，
//   或许知道另外哪个new-handler有此能力，
//   那么这个new-handler就可以安装另外的new-handler替换自己。
// 3.卸除new-handler，也就是将null指针传给set_new_handler。
//   一旦没有安装任何new-handler，
//   operator new会在内存分配不成功时抛出异常。
// 4.抛出bad_alloc(或派生自bad_alloc)的异常。
//   这样的异常不会被operator new捕获，
//   因此会被传播到内存申请的地方。
// 5.不返回，通常调用abort或exit。

// C++并不支持class专属的new-handlers，其实也不需要，可以自己实现。
// 只需令每一个class提供自己的set_new_handler和operator new即可。
class NewHandlerHolder {
 public:
  explicit NewHandlerHolder(new_handler nh) : handler(nh) {}
  ~NewHandlerHolder() { std::set_new_handler(handler); }

 private:
  std::new_handler handler;
  NewHandlerHolder(const NewHandlerHolder &);
  NewHandlerHolder &operator=(const NewHandlerHolder &);
};
class X {
 public:
  static std::new_handler set_new_handler(new_handler p) throw() {
    new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
  }
  static void *operator new(size_t size) throw(bad_alloc) {
    NewHandlerHolder h(set_new_handler(currentHandler));
    return ::operator new(size);
  }

 private:
  static new_handler currentHandler;
};
new_handler X::currentHandler = 0;

// 使用template实现
template <typename T>
class NewHandlerSupport {
  // 从未使用类型T。
  // 继承自NewHandlerSupport的每一个class，
  // 拥有实体互异的NewHandlerSupport复件，
  // 更明确地说是其static成员变量currentHandler。
  // 参数类型T只是用来区分不同的derived class。
  // template机制会自动为每一个T生成一份currentHandler。
 public:
  static new_handler set_new_handler(new_handler p) throw();
  static void *operator new(size_t size) throw(bad_alloc);

 private:
  static new_handler currentHandler;
};
template <typename T>
new_handler NewHandlerSupport<T>::set_new_handler(new_handler p) throw() {
  new_handler oldHandler = currentHandler;
  currentHandler = p;
  return oldHandler;
}
template <typename T>
void *NewHandlerSupport<T>::operator new(size_t size) throw(bad_alloc) {
  NewHandlerHolder h(set_new_handler(currentHandler));
  return ::operator new(size);
}
template <typename T>
new_handler NewHandlerSupport<T>::currentHandler = 0;

// Widget继承自一个模板化的base class，而后者又以Widget作为类型参数。
// 这是一个有用的技术，叫做怪异的循环模板模式（CRTP）。
class Widget : public NewHandlerSupport<Widget> {
  // 不必声明set_new_handler或operator new
};
// 这就是为Widget为了提供class专属的set_new_handler所需的全部动作。

// 直到1993年，C++都还要求operator new必须在无法分配足够内存时返回null。
// 新一代的operator new则应该抛出bad_alloc异常。
// 很多程序是在编译器开始支持新规范之前写的。
// C++标准委员会不想抛弃那些侦测null的族群，
// 于是提供另一形式的operator new，分配失败返回null，
// 被称为nothrow形式。
class Widget1 {};
// nothrow new对异常的强制保证性并不高。
// 表达式new (nothrow)发生两件事：
// 第一，nothrow版的operator new被调用，
// 分配内存，如果失败返回null，如果成功，构造函数会被调用。
// 构造函数可以做它想做的任何事，可能又new一些内存，
// 而没人可以强迫它再次使用nothrow new。
// nothrow new并不抛出异常，但是对象的构造函数却可能会抛出异常。

int main() {
  // set_new_handler(outOfMem);
  // int *pBigDataArray = new int[10000000000000000L];
  // Unable to satisfy requeset for memory

  X::set_new_handler(outOfMem);
  // 如果分配失败，调用outOfMem：
  X *p1 = new X;

  // 如果分配失败，调用global new-handing函数：
  string *ps = new string;

  X::set_new_handler(0);
  // 如果分配失败，立刻抛出异常：
  X *p2 = new X;

  Widget1 *pw1 = new Widget1;
  if (pw1 == 0) {  // 这个测试一定失败
  }

  Widget1 *pw2 = new (nothrow) Widget1;
  if (pw2 == 0) {  // 这个测试可能成功
  }

  return 0;
}