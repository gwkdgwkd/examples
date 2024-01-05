#include <climits>
#include <iostream>
#include <string>
#include <memory>

// 分配器allocator是C++标准库的组件，主要用来给容器vector，list，map等内存的分配和释放。
// C++标准库提供了默认使用的通用分配器std::allocator，但开发者可以自定义分配器。
// GNU STL除了提供默认分配器，还提供了：
// 1.__pool_alloc，SGI内存池分配器；
// 2.__mt_alloc，多线程内存池分配器；
// 3.array_allocator，全局内存分配，只分配不释放，交给系统来释放；
// 4.malloc_allocator，堆std::malloc和std::free进行的封装。

namespace n1 {
// new会分配内存并执行对象构造函数，delete会执行对象析构函数并释放内存。
// 如果将内存分配和对象构造分离，可以先分配大块内存，只在需要时才真正执行对象构造函数。
// STL在头文件memory中提供了一个allocator类，允许将分配和对象构造分离，
// 提供更好的性能和更灵活的内存管理能力，为了定义一个allocator对象，
// 必须指明allocator可以分配的对象类型，当allocator分配内存时，
// 会根据给定的对象类型来确定恰当的内存大小和对齐位置。

// 根据C++标准规范，STL中分配器的对外接口、成员变量都一样，只是接口内部实现有区别。
// STL默认分配器为std::allocator<_Tp>，对于内存分配和释放的接口allocate和deallocate，
// 内部实现只是将::operator new和::operator delete进行封装，没用做特殊处理。
}  // namespace n1

namespace n2 {
// 实现Allocator只需要实现allocate和deallocate，来实现自己的内存分配策略。

template <class T>
inline T* _allocate(ptrdiff_t num, T*) {
  std::cout << "_allocate" << std::endl;
  return static_cast<T*>(::operator new(num * sizeof(T)));
}
template <class T>
inline void _deallocate(T* buff) {
  std::cout << "_deallocate" << std::endl;
  ::operator delete(buff);
}

template <class T, class U>
inline void _construct(T* p, const U& value) {
  new (p) T(value);
}
template <class T>
inline void _destory(T* p) {
  p->~T();
}
template <typename T>
class MyAlloctor {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  template <typename _Tp1>
  struct rebind {
    typedef MyAlloctor<_Tp1> other;
  };

  MyAlloctor() = default;
  ~MyAlloctor() = default;
  pointer address(reference x) { return static_cast<pointer>(&x); }
  const_pointer address(const_reference x) {
    return static_cast<const_pointer>(&x);
  }
  pointer allocate(size_type _n, const void* hit = 0) {
    return _allocate(_n, (pointer)0);
  }
  void deallocate(pointer p, size_type _n) { _deallocate(p); }
  size_type max_size() const throw() {
    return static_cast<size_type>(INT_MAX / sizeof(T));
  }

  void construct(pointer p, const_reference value) { _construct(p, value); }
  void destroy(pointer p) { _destory(p); }
};

class base {
  std::string name;
  int value;

 public:
  base() : name("test"), value(0) { std::cout << "base ctor" << std::endl; }
  explicit base(int v) : name("explicit name"), value(v) {
    std::cout << "base ctor value=" << value << std::endl;
  }
  virtual ~base() { std::cout << "base dtor" << std::endl; }
  base(const base& value) { std::cout << "Copy ctor" << std::endl; }
  base(base&& value) { std::cout << "move ctor" << std::endl; }
  void who(void) { std::cout << name << " value= " << value << std::endl; }
};

void func() {
  __gnu_cxx::new_allocator<base> alloc_test;
  __gnu_cxx::new_allocator<base> test;
  base* p = alloc_test.allocate(3);

  std::cout << "test1 start" << std::endl;
  std::cout << "size of base=" << sizeof(base) << std::endl;
  std::cout << "size of *p=" << sizeof(*p) << std::endl;
  p->who();
  //(p+1)->who();
  std::cout << "test1 end\n" << std::endl;
  std::cout << "test2 start" << std::endl;
  // test.construct(p,base(1));
  // test.construct(p+1,base(2));
  // test.construct(p+2,base(3));

  alloc_test.construct(p, base(1));
  alloc_test.construct(p + 1, base(2));
  alloc_test.construct(p + 2, base(3));

  p->who();
  (p + 1)->who();
  (*p).who();
  alloc_test.deallocate(p, 3);
  getchar();
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 0]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n2::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}