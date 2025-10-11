#include <iostream>
#include <memory>

namespace n1 {
// 1.使用同一个原始指针创建shared_ptr，是错误的行为，因为会导致资源被释放两次；
// 2.使用this指针创建shared_ptr也一样，会导致资源被释放两次。
class A {
 public:
  A() { std::cout << "A" << std::endl; }
  ~A() { std::cout << "~A" << std::endl; }

  std::shared_ptr<A> getptr() { return std::shared_ptr<A>(this); }
};

void func1() {
  A* p = new A();
  std::shared_ptr<A> p1 = std::shared_ptr<A>(p);
  std::shared_ptr<A> p2 = std::shared_ptr<A>(p);

  // A
  // ~A
  // ~A
  // free(): double free detected in tcache 2
  // 已放弃 (核心已转储)
}

void func2() {
  std::shared_ptr<A> bp1(new A());
  std::shared_ptr<A> bp2 = bp1->getptr();
  std::cout << "bp1.use_count() = " << bp1.use_count() << std::endl;
  std::cout << "bp2.use_count() = " << bp2.use_count() << std::endl;

  // A
  // bp1.use_count() = 1
  // bp2.use_count() = 1
  // ~A
  // ~A
  // free(): double free detected in tcache 2
  // 已放弃 (核心已转储)
}
}  // namespace n1

namespace n2 {
// 有时候我们确实需要通过this返回智能指针，
// 例如需要在类内传递自己的shared_ptr，又或是需要返回自己的shared_ptr。
// std::enable_shared_from_this是C++11引入的模板类，
// 用于解决对象在成员函数中需要安全获取指向自身的std::shared_ptr的问题。

// 通过继承该类，可调用shared_from_this()返回与现有shared_ptr共享引用计数的智能指针，
// 避免直接构造shared_ptr(this)导致重复析构的问题。

// 类内部维护一个weak_ptr成员，当对象首次被shared_ptr管理时，
// shared_ptr的构造函数会初始化此weak_ptr，
// 后续调用shared_from_this()即基于此生成共享所有权的shared_ptr。

// 典型使用场景：
// 1.异步编程‌：在回调函数或线程任务中延长对象生命周期，确保操作完成前对象不被销毁；
// ‌2.事件处理‌：如发布-订阅模式中，订阅者需持有对象的共享指针以保证事件处理期间对象存活；
// ‌3.链式调用‌：对象需将自身作为参数传递给其他需要shared_ptr的接口。

// 在异步调用中，存在一个保活机制，异步函数执行的时间点我们是无法确定的，
// 然而异步函数可能会使用到异步调用之前就存在的变量。
// 为了保证该变量在异步函数执期间一直有效，可以传递一个指向自身的share_ptr给异步函数，
// 这样在异步函数执行期间share_ptr所管理的对象就不会析构，所使用的变量也会一直有效了（保活）。

// 多继承处理‌：需确保enable_shared_from_this在继承链中仅出现一次，否则可能引发未定义行为。

// weak_from_this()‌：C++17新增方法，返回weak_ptr以避免循环引用。

class A : public std::enable_shared_from_this<A> {
 public:
  std::shared_ptr<A> getptr() {
    // 一定要使用public继承,否则无法使用shared_from_this()方法：
    return shared_from_this();
  }
};

void func1() {
  std::shared_ptr<A> bp1(new A());
  std::shared_ptr<A> bp2 = bp1->getptr();
  std::cout << "bp1.use_count() = " << bp1.use_count() << std::endl;
  std::cout << "bp2.use_count() = " << bp2.use_count() << std::endl;

  // bp1.use_count() = 2
  // bp2.use_count() = 2
}

void func2() {
  // 使用enable_shared_from_this必须建立在对象本身被shared_ptr管理的前提下，
  // 否则会出现std::bad_weak_ptr的异常问题：

  A a;
  std::shared_ptr<A> bp = a.getptr();

  // terminate called after throwing an instance of 'std::bad_weak_ptr'
  //   what():  bad_weak_ptr
  // 已放弃 (核心已转储)
}
}  // namespace n2

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 16]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::func1();
      break;
    case 1:
      n1::func2();
      break;
    case 2:
      n2::func1();
      break;
    case 3:
      n2::func2();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}