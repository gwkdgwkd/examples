#include <iostream>

// 写了placement new也要写placement delete

namespace n1 {
class A {};
void func() {
  A* p = new A;
  // 共有两个函数被调用：
  // 1.operator new；
  // 2.A的default构造函数。

  // 假设new调用成功，default构造函数抛出异常，
  // 那么new分配的内存必须取消并恢复原样，否则会造成内存泄露：
  // 1.这个时候客户无法归还内存，因为p尚未被赋值，
  //   客户手上也就没有指针指向未被归还的内存；
  // 2.取消步骤需要C++运行期系统身上，
  //   运行期系统会高高兴兴地调用new对应的delete版本，
  //   前提当然是它必须知道哪一个delete该被调用。
}
}  // namespace n1

namespace n2 {
// 写了一个class专属的operator new，
// 要求接受一个ostream，用来标记相关分配信息，
// 同时有写了一个正常形式的class专属operator delete：
class A {
 public:
  A() { throw 5; }
  // 非正常形式的new，这便是所谓的placement new：
  static void* operator new(size_t size, char*) throw(std::bad_alloc) {
    std::cout << "A::placement new(char*)" << std::endl;
  }
  // 正常的class专属delete：
  static void operator delete(void* pMemory, size_t size) throw() {
    std::cout << "operator delete(void* pMemory, size_t size)" << std::endl;
  }

  // placement new意味带任意额外参数的new，placement delete直接派生自它。
  // 当new调用成功，default构造函数抛出异常时，
  // C++运行系统无法知道真正被调用的那个new如何运作，
  // 也就无法取消分配并恢复原样（没有任何delete被调用）。
  // 运行期系统寻找参数个数和类型都与operator new相同的某个delete，
  // 如果找到就调用，所以delete应该是：
  // void operator delete(void*,char*) throw();
};
void func() {
  char c[10];
  try {
    // 构造函数发生异常，还会发生内存泄露：
    A* p = new (c) A;
  } catch (int e) {
    std::cout << e << std::endl;
  }

  // A::placement new(char*)
  // 5
}
}  // namespace n2

namespace n3 {
class A {
 public:
  A(int i) {
    if (i % 2) {
      throw i;
    }
  }

  // placement delete只有在：
  // 伴随placement new调用而触发的构造函数出现异常时才会被调用。
  // 对着指针执行delete绝不会导致调用placement delete。
  // 所以必须同时提供两个版本：
  // 1.placement delete，构造函数抛出异常；
  // 2.正常的operator delete，没有异常。
  static void* operator new(size_t size, char*) throw(std::bad_alloc) {
    std::cout << "A::placement new(char*)" << std::endl;
  }
  static void operator delete(void* pMemory, char*) throw() {
    std::cout << "operator delete(void* pMemory, char*)" << std::endl;
  }
  // 正常delete版本：
  static void operator delete(void* pMemory) throw() {
    std::cout << "operator delete(void* pMemory)" << std::endl;
  }
};
void func1() {
  char c[10];
  try {
    // 构造函数发生异常，不会再泄露内存：
    A* p = new (c) A(3);
  } catch (int e) {
    std::cout << e << std::endl;
  }

  // A::placement new(char*)
  // operator delete(void* pMemory, char*)
  // 3
}
void func2() {
  char c[10];
  A* p = new (c) A(2);
  // A::placement new(char*)

  delete p;  // 调用正常的operator delete
  // operator delete(void* pMemory)
}
void func() {
  func1();
  func2();
}
}  // namespace n3

namespace n4 {
// 由于成员函数的名称会掩盖其外围作用域中的相同名称，
// 必须小心避免让class专属的new掩盖客户期望的其他new（包括正常版本）。
// 假设有一个base class，声明唯一一个placement operator new，
// 客户端会发现无法使用正常形式的new：
class B {
 public:
  static void* operator new(size_t size, char*) throw(std::bad_alloc) {
    std::cout << "B::operator new(size_t size, char*)" << std::endl;
  }
};
// 同样的道理，derived class中的operator new，
// 会掩盖global版本和继承而得的operator new版本：
class D : public B {
 public:
  static void* operator new(size_t size) throw(std::bad_alloc) {
    std::cout << "D::operator new(size_t size)" << std::endl;
  }
};
void func() {
  char c[10];

  // 正确，调用Base的placement new：
  B* pb1 = new (c) B;  // B::operator new(size_t size, char*)
  // 错误，正常形式的operator new被掩盖：
  // B* pb2 = new B;

  D* pd1 = new D;  // D::operator new(size_t size)
  // 错误，Base的placement new被掩盖了：
  // D* pd2 = new (c) D;
}
}  // namespace n4

namespace n5 {
// 缺省情况下C++在global作用域内提供以下形式的operator new：
// void* operator new(size_t) throw(std::bad_alloc);           // normal new
// void* operator new(size_t, void*) throw();                  // placement new
// void* operator new(size_t, const std::nothrow_t&) throw();  // nothrow new
// 在class内声明任何operator new，都会遮掩上述的这些标准形式。
// 如果希望这些函数有着平常的行为，只要令class专属版本调用global版本即可。

// 简单的做法是，建立一个base class，内含所有正常形式的new和delete：
class StandardNewDeleteForms {
 public:
  // normal new/delete
  static void* operator new(size_t size) throw(std::bad_alloc) {
    std::cout << "operator new(size_t size) throw(std::bad_alloc)" << std::endl;
    return ::operator new(size);
  }
  static void operator delete(void* pMemory) throw() {
    std::cout << "operator delete(void* pMemory)" << std::endl;
    ::operator delete(pMemory);
  }
  // placement new/delete
  static void* operator new(size_t size, void* ptr) throw() {
    std::cout << "operator new(size_t size, void* ptr) throw()" << std::endl;
    ::operator new(size, ptr);
  }
  static void operator delete(void* pMemory, void* ptr) throw() {
    std::cout << "operator delete(void* pMemory, void* ptr)" << std::endl;
    ::operator delete(pMemory, ptr);
  }
  // nothrow new/delete
  static void* operator new(size_t size, const std::nothrow_t& nt) throw() {
    std::cout << "operator new(size_t size, const std::nothrow_t& nt) throw()"
              << std::endl;
    return ::operator new(size, nt);
  }
  static void operator delete(void* pMemory, const std::nothrow_t&) throw() {
    std::cout << "operator delete(void* pMemory, const std::nothrow_t&)"
              << std::endl;
    ::operator delete(pMemory);
  }
};
// 凡是想扩充标准形式的客户，可以利用继承机制及using声明获取标准形式：
class A : public StandardNewDeleteForms {
 public:
  A(int i) {
    if (i % 2) {
      throw i;
    }
  }
  using StandardNewDeleteForms::operator new;
  using StandardNewDeleteForms::operator delete;
  // static void* operator new(size_t, void* ptr) throw(std::bad_alloc) {}
  // static void operator delete(void* pMemory, void* ptr) throw() {}
};
void func1() {
  A* p = new A(2);
  delete p;

  // operator new(size_t size) throw(std::bad_alloc)
  // operator delete(void* pMemory)
}
void func2() {
  char* c = new char[10];
  A* p = new ((void*)c) A(2);
  delete p;

  // operator new(size_t size, void* ptr) throw()
  // operator delete(void* pMemory)
}
void func3() {
  // delete运算符永远不应引发异常，它有两个版本，nothrow和普通版本(非零)。

  A* p1 = new (std::nothrow) A(2);
  delete p1;  // 无法手动调用delete运算符的nothrow版本
  // operator new(size_t size, const std::nothrow_t& nt) throw()
  // operator delete(void* pMemory)

  // 那么operator delete的nothrow版本的用途是什么？
  // 如果调用new创建对象，内存分配成功但对象的构造函数发生了异常，
  // new操作将失败，并传播构造函数引发的异常。
  // 但是在此之前，需要取消分配所获得的内存，这是通过delete运算符自动完成的。
  // 如果刚失败的new是nothrow版本，则要调用的delete运算符将是nothrow，
  // 否则将是正常的，两个版本的delete运算符都不会并且不应抛出。
  try {
    A* p2 = new (std::nothrow) A(3);
  } catch (int e) {
    std::cout << e << std::endl;
  }
  // operator new(size_t size, const std::nothrow_t& nt) throw()
  // operator delete(void* pMemory, const std::nothrow_t&)
  // 3
}
}  // namespace n5

// 请记住：
// 1.当写一个placement operator new时，
//   请确定也出了对应的placement operator delete，
//   否则程序可能发生隐秘的时断时续的内存泄露；
// 2.当声明placement new和placement delete时，
//   请确定不要无意识（非故意）地遮掩了它们的正常版本。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 6]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::func();
      break;
    case 2:
      n3::func();
      break;
    case 3:
      n4::func();
      break;
    case 4:
      n5::func1();
      break;
    case 5:
      n5::func2();
      break;
    case 6:
      n5::func3();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}