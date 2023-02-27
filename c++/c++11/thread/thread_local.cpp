#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// thread_local是C++11为线程安全引进的变量声明符，是一个存储器指定符。
// 所谓存储器指定符，其作用类似命名空间，指定了变量名的存储期以及链接方式。
// 同类型的关键字还有：
// 1.auto：自动存储期；
// 2.register：自动存储期，提示编译器将此变量置于寄存器中；
// 3.static：静态或线程存储期，同时提示是内部链接；
// 4.extern：静态或线程存储期，同时提示是外部链接；
// 5.thread_local：线程存储期；
// 6.mutable：不影响存储期或链接。

// thread_local关键词只对声明于命名空间作用域的对象、
// 声明于块作用域的对象及静态数据成员允许，它指示对象拥有线程存储期。
// 它能与static或extern结合，以分别指定内部或外部链接，
// 除了静态数据成员始终拥有外部链接，但附加的static不影响存储期。
// 线程存储期：
// 对象的存储在线程开始时分配，而在线程结束时销毁，每个线程拥有自己的实例。
// 唯有声明为thread_local的对象拥有此存储期。
// thread_local能与static或extern一同出现，以调整链接属性。

// 哪些变量可以被声明为thread_local：
// 1.命名空间下的全局变量；
// 2.类的static成员变量；
// 3.局部变量。

std::mutex cout_mutex;

namespace n1 {
// 全局的thread_local变量在每个线程里是分别自加互不干扰的。
thread_local int g = 1;
void f(const std::string& thread_name) {
  for (int i = 0; i < 3; ++i) {
    g++;
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "thread[" << thread_name << "]: g = " << g << std::endl;
  }
  return;
}

void func() {
  std::thread t1(f, "t1");
  std::thread t2(f, "t2");
  t1.join();
  t2.join();
  // thread[t1]: g = 2
  // thread[t1]: g = 3
  // thread[t1]: g = 4
  // thread[t2]: g = 2
  // thread[t2]: g = 3
  // thread[t2]: g = 4
}
}  // namespace n1

namespace n2 {
// 本地变量在线程中是持续存在的，不同于普通临时变量的生命周期，
// 它具有static变量一样的初始化特征和生命周期。
// 虽然是局部变量，但是在每个线程的每次for循环中，
// 使用的都是线程中的同一个变量，也印证了thread_local变量会自动static。
void f(const std::string& thread_name) {
  for (int i = 0; i < 3; ++i) {
    thread_local int x = 1;
    x++;
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "thread[" << thread_name << "]: x = " << x << std::endl;
  }
  // thread_local虽然改变了变量的存储周期，但是并没有改变变量的作用域：
  // x++;
  return;
}

void func() {
  std::thread t1(f, "t1");
  std::thread t2(f, "t2");
  t1.join();
  t2.join();
  // thread[t1]: x = 2
  // thread[t1]: x = 3
  // thread[t1]: x = 4
  // thread[t2]: x = 2
  // thread[t2]: x = 3
  // thread[t2]: x = 4
}
}  // namespace n2

namespace n3 {
// 类对象的使用和创建和内部类型类似，都不会创建多个。
class A {
 public:
  A() {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "create A" << std::endl;
  }

  ~A() {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "destroy A" << std::endl;
  }

  int counter = 0;
  int get_value() { return counter++; }
};

void func1(const std::string& thread_name) {
  for (int i = 0; i < 3; ++i) {
    thread_local A* a = new A();
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "thread[" << thread_name << "]: a.counter:" << a->get_value()
              << std::endl;
  }
  return;
}

// 在函数间或通过函数返回实例也不会创建多个。
// 虽然create A()看上去被调用了多次，实际上只被调用了一次，
// 因为thread_local只会在每个线程最开始时候进行初始化，并且只会被初始化一次。
A* creatA() { return new A(); }
void loopin_func(const std::string& thread_name) {
  thread_local A* a = creatA();
  std::lock_guard<std::mutex> lock(cout_mutex);
  std::cout << "thread[" << thread_name << "]: a.counter:" << a->get_value()
            << std::endl;
  return;
}
void func2(const std::string& thread_name) {
  for (int i = 0; i < 3; ++i) {
    loopin_func(thread_name);
  }
  return;
}

void func() {
  std::thread t1(func1, "t1");
  std::thread t2(func1, "t2");
  t1.join();
  t2.join();
  // create A
  // thread[t1]: a.counter:0
  // thread[t1]: a.counter:1
  // thread[t1]: a.counter:2
  // create A
  // thread[t2]: a.counter:0
  // thread[t2]: a.counter:1
  // thread[t2]: a.counter:2
}
}  // namespace n3

namespace n4 {
// thread_local作为类成员变量时必须是static的。
class B {
 public:
  B(std::string n) : name(n) { std::lock_guard<std::mutex> lock(cout_mutex); }
  ~B() {}

  void reduce() {
    b_key--;
    b_value--;
    b_static--;
  }

  void show() {
    std::cout << name << "  v:" << b_value << ",k:" << b_key
              << ",s:" << b_static << std::endl;
  }

 private:
  std::string name;

  // b_key是thread_local，虽然其也是static的，
  // 但是每个线程中有一个，每次线程中的所有调用共享这个变量：
  thread_local static int b_key;
  // thread_local int b_key;
  // storage class specified for ‘b_key’

  int b_value = 24;

  // b_static是真正的static，全局只有一个，所有线程共享这个变量：
  static int b_static;
};

thread_local int B::b_key = 12;
int B::b_static = 36;

void f(const std::string& thread_name) {
  B b[2] = {B("b1"), B("b2")};
  std::lock_guard<std::mutex> lock(cout_mutex);
  std::cout << "thread[" << thread_name << "]:" << std::endl;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 3; ++j) {
      b[i].reduce();
      b[i].show();
    }
  }

  return;
}

void func() {
  std::thread t1(f, "t1");
  std::thread t2(f, "t2");
  t1.join();
  t2.join();

  // thread[t1]:
  // b1  v:23,k:11,s:35
  // b1  v:22,k:10,s:34
  // b1  v:21,k:9,s:33
  // b2  v:23,k:8,s:32
  // b2  v:22,k:7,s:31
  // b2  v:21,k:6,s:30
  // thread[t2]:
  // b1  v:23,k:11,s:29
  // b1  v:22,k:10,s:28
  // b1  v:21,k:9,s:27
  // b2  v:23,k:8,s:26
  // b2  v:22,k:7,s:25
  // b2  v:21,k:6,s:24
}
}  // namespace n4

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
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
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}

// g++ -std=c++11 thread_local.cpp -lpthread