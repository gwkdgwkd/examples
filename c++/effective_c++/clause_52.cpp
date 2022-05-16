#include <iostream>

using namespace std;

// 写了placement new也要写placement delete

class Widget1 {};

// 写了一个class专属的operator new，要求接受一个ostream，用来标记相关分配信息，
// 同时有写了一个正常形式的class专属operator delete：
class Widget {
 public:
  // 非正常形式的new，这便是所谓的placement new。
  static void* operator new(size_t size, ostream& logStream) throw(bad_alloc) {}
  // 正常的class专属delete
  static void operator delete(void* pMemory, size_t size) throw() {}
};
// placement new意味带任意额外参数的new，placement delete直接派生自它。
// 当new调用成功，default构造函数抛出异常时，C++运行系统无法知道真正被调用的那个new如何运作，
// 也就无法取消分配并恢复原样(没有任何delete被调用)。
// 运行期系统寻找”参数个数和类型都与operator new相同“的某个delete，如果找到就调用。
// 所以delete应该是：
// void operator delete(void*,ostream&) throw();
class Widget2 {
 public:
  static void* operator new(size_t size, ostream& logStream) throw(bad_alloc) {}
  static void operator delete(void* pMemory, ostream& logStream) throw() {}
  // placement delete只有在“伴随placement new调用而触发的构造函数”出现异常时才会被调用。
  // 对着指针执行delete绝不会导致调用placement delete。
  // 所以必须同时placement delete（构造函数抛出异常）和正常的operator delete（没有异常）两个版本。
  static void operator delete(void* pMemory) throw() {}  // 正常delete版本
};

// 由于成员函数的名称会掩盖其外围作用域中的相同名称，
// 必须小心避免让class专属的new掩盖客户期望的其他new（包括正常版本）。
// 假设有一个base class，声明唯一一个placement operator new，客户端会发现无法使用正常形式的new：
class Base {
 public:
  static void* operator new(size_t size, ostream& logStream) throw(bad_alloc) {}
};
// 同样的道理，derived class中的operator new会掩盖global版本和继承而得的operator new版本：
class Derived : public Base {
 public:
  static void* operator new(size_t size) throw(bad_alloc) {}
};

// 缺省情况下C++在global作用域内提供以下形式的operator new：
// void* operator new(size_t) throw(bad_alloc);           // normal new
// void* operator new(size_t, void*) throw();             // placement new
// void* operator new(size_t, const nothrow_t&) throw();  // nothrow new
// 在class内声明任何operator new，都会遮掩上述的这些标准形式。
// 如果希望这些函数有着平常的行为，只要令你的class专属版本调用global版本即可。

// 简单的做法是，建立一个base class，内含所有正常形式的new和delete：
class StandardNewDeleteForms {
 public:
  // normal new/delete
  static void* operator new(size_t size) throw(bad_alloc) {
    return ::operator new(size);
  }
  static void operator delete(void* pMemory) throw() {
    ::operator delete(pMemory);
  }
  // placement new/delete
  static void* operator new(size_t size, void* ptr) throw() {
    ::operator new(size, ptr);
  }
  static void operator delete(void* pMemory, void* ptr) throw() {
    ::operator delete(pMemory, ptr);
  }
  // nothrow new/delete
  static void* operator new(size_t size, const nothrow_t& nt) throw() {
    return ::operator new(size, nt);
  }
  static void operator delete(void* pMemory, const nothrow_t&) throw() {
    ::operator delete(pMemory);
  }
};
// 凡是想扩充标准形式的客户，可以利用继承机制及using声明获取标准形式：
class Widget3 : public StandardNewDeleteForms {
  using StandardNewDeleteForms::operator new;
  using StandardNewDeleteForms::operator delete;
  static void* operator new(size_t, ostream& logStream) throw(bad_alloc) {}
  static void operator delete(void* pMemory, ostream& logStream) throw() {}
};

// 请记住：
// 当写一个placement operator new，请确定也出了对应的placement operator delete。
// 否则程序可能发生隐秘的时断时续的内存泄露。
// 当声明placement new和placement delete，请确定不要无意识（非故意）地遮掩了它们的正常版本。

int main() {
  Widget1* pw = new Widget1;
  // 共有两个函数被调用，一个是operator new，一个是Widget的default构造函数。
  // 假设new调用成功，default构造函数抛出异常。
  // 那么new分配的内存必须取消并恢复原样，否则会造成内存泄露。
  // 这个时候客户无法归还内存，因为pw尚未被赋值，客户手上也就没有指针指向未被归还的内存。
  // 取消步骤需要C++运行期系统身上。
  // 运行期系统会高高兴兴地调用new对应的delete版本，前提当然是它必须知道哪一个delete该被调用。

  Widget* pw1 = new (cerr) Widget;  // 构造函数发生异常，还会发生内存泄露

  Widget2* pw2 = new (cerr) Widget2;  // 构造函数发生异常，不会在泄露内存
  delete pw2;                         // 调用正常的operator delete

  Base* pb2 = new (cerr) Base;  // 正确，调用Base的placement new
  // Base* pb1 = new Base;  // 错误，正常形式的operator new被掩盖

  Derived* pd1 = new Derived;
  // Derived* pd2 = new (clog) Derived;  // 错误，Base的placement new被掩盖了

  return 0;
}