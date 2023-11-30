#include <pthread.h>
#include <stdarg.h>

#include <cstring>
#include <ctime>  // std::time_t, std::tm, std::localtime, std::mktime
#include <functional>
#include <iomanip>  // std::put_time
#include <iostream>
#include <mutex>
#include <thread>

// 并发指的是在单个系统里同时执行多个独立的活动，而不是顺序的一个接一个的执行。
// 单核CPU某个时刻只可能处理一个任务，但它不是完全执行完一个任务再执行下个任务，
// 而是一直在任务间切换，每个任务完成一点就去执行下个任务，看起来就像任务在并行发生，
// 虽然不是严格的同时执行多个任务，但是仍然称之为并发。
// 真正的并发是在在多核CPU上，能够真正的同时执行多个任务，称为硬件并发。
// 并发并非没有代价，在单核CPU并发执行两个任务需要付出上下文切换的时间代价。
// 线程是操作系统能够进行CPU调度的最小单位，一个进程可包含单个或者多个线程。

// 可以多个线程去完成一个任务，也可以多个进程去完成一个任务，本质是多人合伙完成一件事。
// 多进程并发：
// 将应用程序分为多个独立的、单线程的进程，他们可以同时运行。
// 这些独立的进程可以通过常规的进程间通信机制进行通信，如管道、信号、
// 消息队列、共享内存、存储映射I/O、信号量、套接字等。
// 缺点：
// 1.进程间通信较为复杂，速度相对线程间的通信更慢；
// 2.启动进程的开销比线程大，使用的系统资源也更多。
// 优点：
// 1.进程间通信的机制相对于线程更加安全；
// 2.能够很容易的将一台机器上的多进程程序部署在不同的机器上，如果通信机制是套接字的话。

// 多线程并发：
// 线程很像轻量级的进程，但是一个进程中的所有线程都共享相同的地址空间，
// 线程间的大部分数据都可以共享，线程间的通信一般都通过共享内存来实现。
// 任何程序都是一个进程，main()函数就是其中的主线程，单个线程都是顺序执行。
// 优点：
// 1.由于可以共享数据，多线程间的通信开销比进程小的多；
// 2.线程启动的比进程快，占用的资源更少。
// 缺点：
// 1.共享数据太过于灵活，为了维护正确的共享，代码写起来比较复杂；
// 2.无法部署在分布式系统上。

// 为什么使用并发,主要原因有两个：任务拆分和提高性能。
// 1.任务拆分
//   将相关的代码放在一起，将无关的代码分开，这样能够让程序更加容易理解和测试。
//   将程序划分成不同的任务，每个线程执行一个或多个任务，可以将程序的逻辑变得更加简单。
// 2.提高性能，在两种情况下，并发能够提高性能。
//   将一个单个任务分成若干个部分各自并行运行，从而降低运行时间。
//   虽然听起来很简单，但其实是一个相当复杂的过程，设想假如各个部分之间存在很多依赖，
//   一个部分的执行需要使用到另一个任务的执行结果，这个时候并不能很好的并行完成。
//   数据并行，每个线程在不同的数据部分上执行相同的操作。

// C++98标准中并没有线程库的存在，而在C++11中终于提供了多线程的标准库，
// 提供了管理线程、保护共享数据、线程间同步操作、原子操作等类。
// 多线程库对应的头文件是thread，类名为std::thread。
// C++11中std命名空间将Boost库中的Thread加入，Boost的多线程从准标准变为标准。
// linux系统有pthread，但有C++11的std::thread后，
// 可以在语言层面编写多线程程序，可移植性得到了很大的提高。

// C++11与多线程相关的头文件：
// 1.atomic主要声明了两个类,std::atomic和std::atomic_flag，
//   另外还声明了一套C风格的原子类型和与C兼容的原子操作的函数；
// 2.thread主要声明了std::thread类，另外std::this_thread命名空间也在其中；
// 3.mutex主要声明了与mutex相关的类，包括std::mutex系列类，
//   std::lock_guard，std::unique_lock，以及其他的类型和函数；
// 4.condition_variable主要声明了与条件变量相关的类，
//   包括std::condition_variable和std::condition_variable_any；
// 5.future主要声明了std::promise，std::package_task两个Provider类，
//   以及std::future和std::shared_future两个Future类，
//   另外还有一些与之相关的类型和函数，std::async()函数就声明在此头文件中。

namespace n1 {
// 只要创建了线程对象，线程就开始执行，并没有一个类似start的函数来显式启动。
// 使用C++线程库启动线程，可以归结为构造std::thread对象。

void func1() { std::cout << __FUNCTION__ << " running" << std::endl; }

// std::thread支持移动，意味着std::thread可以当作函数的返回值和参数：
void func2(std::thread t) {}
std::thread func3() {
  return std::thread([]() {});
}

void func() {
  // 默认构造函数，创建一个空的std::thread执行对象：
  std::thread t1;
  std::thread threads[2];  // 空的thread数组

  // 创建一个std::thread对象，该std::thread对象可被joinable：
  std::thread t2(func1);
  t2.join();

  // 拷贝构造函数被禁用，意味着std::thread对象不可拷贝构造：
  // std::thread t3 = t2;
  // std::thread t4(t2);

  // 移动赋值操作符：
  t1 = std::thread(func1);
  t1.join();
  for (int i = 0; i < 2; i++) {
    threads[i] = std::thread(func1);
  }
  for (auto &t : threads) {
    t.join();
  }

  // 移动拷贝构造函数：
  std::thread t5(func1);
  std::cout << "t5 id is " << t5.get_id() << std::endl;
  // t5 id is 139799432115968

  // t6获t5的全部属性，t5被消耗了：
  std::thread t6 = std::move(t5);
  std::cout << "t6 id is " << t6.get_id() << std::endl;
  // t6 id is 139799432115968

  // t5.join();
  // terminate called after throwing an instance of 'std::system_error'
  t6.join();

  std::thread t7(func1);                                      // 栈上
  std::thread t8[2]{std::thread(func1), std::thread(func1)};  // 线程数组
  std::thread *t9(new std::thread(func1));                    // 堆上
  // 线程指针数组：
  std::thread *t10(new std::thread[2]{std::thread(func1), std::thread(func1)});
  t7.join();
  t8[0].join();
  t8[1].join();
  t9->join();
  t10[0].join();
  t10[1].join();
}
}  // namespace n1

namespace n2 {
// 一旦线程开始运行，就需要显式的决定是要等待它完成，或者分离它让它自行运行。
// 当线程启动后，一定要在和线程相关联的std::thread对象销毁前，
// 对线程运用join()或者detach()方法，否则程序将会终止，
// 因为std::thread的析构函数会调用std::terminate()，这会触发相应异常。
// join()与detach()都是std::thread类的成员函数，
// 是两种线程阻塞方法，两者的区别是是否等待子线程执行结束。
// join线程，调用该函数会阻塞当前线程，直到线程执行完毕join才返回。
// detach线程，将当前线程对象所代表的执行实例与该线程对象分离，
// 使得线程的执行可以单独进行，线程执行完毕，资源将会被释放。

// join()函数的另一个任务是回收该线程中使用的资源，
// 会清理线程相关的存储部分，这代表了join()只能调用一次。
// detach()也只能调用一次，一旦detach()后就无法join()了。
// 使用joinable检查线程是否可被join，检查线程是否为活动的执行线程。
// 有趣的是，detach()可否调用也是使用joinable()来判断。

// 如果detach()，就必须保证线程结束之前可访问数据的有效性，特别是指针和引用。
// 主线程并不想等待子线程结束就想结束整个任务，直接删掉t1.join()是不行的，
// 程序会被终止，析构的时候会调用std::terminate，
// 程序会打印terminate called without an active exception。
// 调用detach()，从而将线程放在后台运行，所有权和控制权被转交给C++运行时库，
// 以确保与线程相关联的资源在线程退出后能被正确的回收。
// 参考UNIX守护进程的概念，这种被分离的线程被称为守护线程（daemon threads）。
// 线程被分离之后，即使该线程对象被析构了，线程还是能够在后台运行，
// 只是由于对象被析构了，主线程不能够通过对象名与这个线程进行通信。

namespace test1 {
void func() {
  // 由默认构造函数创建的线程是不能被join的：
  std::thread t;
  std::cout << "null thread :" << std::boolalpha << t.joinable() << std::endl;
  t.join();

  // null thread :false
  // terminate called after throwing an instance of 'std::system_error'
}
}  // namespace test1

namespace test2 {
void f() { std::cout << __FUNCTION__ << " running" << std::endl; }
void func() {
  std::thread t(f);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << __FUNCTION__ << " sleep finish" << std::endl;
  // 线程已经执行完任务，但是没有被join的话，
  // 该线程依然会被认为是一个活动的执行线程，因此也是可以被join的：
  t.join();

  // f running
  // func sleep finish
}
}  // namespace test2

namespace test3 {
void f() {
  std::cout << "f start" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "f end" << std::endl;
}

void func() {
  // join后，joinable为false，没有执行join或detach，joinable为true。
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "before join : " << std::boolalpha << t.joinable() << std::endl;
  t.join();
  std::cout << "after join : " << std::boolalpha << t.joinable() << std::endl;

  // f start
  // before join : true
  // f end
  // after join : false
}
}  // namespace test3

namespace test4 {
void f() {
  std::cout << "f start" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "f end" << std::endl;
}

void func() {
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // detach后，joinable为false，线程中的打印也看不到了。
  // 脱离主线程的绑定，主线程挂了，子线程不报错，子线程执行完自动退出。
  // detach以后，子线程会成为孤儿线程，线程之间将无法通信：
  t.detach();
  std::cout << "after detach:" << std::boolalpha << t.joinable() << std::endl;
  // 线程被分离了，就不能够再被join了，如果调用，会崩溃：
  t.join();

  // f start
  // after detach:false
  // terminate called after throwing an instance of 'std::system_error'
  //   what():  Invalid argument
}
}  // namespace test4

namespace test5 {
void f() {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "I'm f" << std::endl;
}
void f1() {
  std::thread t(f);
  t.join();
  std::cout << "f1 join finished" << std::endl;
}
void f2() {
  std::thread t(f);
  t.detach();
  std::cout << "f2 detach finished" << std::endl;
}
void func() {
  // 线程对象和对象内部管理的线程的生命周期并不一样，
  // 如果线程执行的快，可能内部的线程已经结束了，但是线程对象还活着，
  // 也有可能线程对象已经被析构了，内部的线程还在运行。
  f1();
  f2();
  // I'm f
  // f1 join finished

  // 退出时，局部的线程对象t被析构，但是它负责的线程还能运行，也会有打印。
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // f2 detach finished
  // I'm f
  f2();
  // 没有f2的打印，因为主线程执行的太快，整个程序已经结束了，
  // 那个后台线程被C++运行时库回收了。
  // f2 detach finished
}
}  // namespace test5

namespace test6 {
void func() {
  int some_local_state = 0;
  std::thread t([]() {});
  try {
    // do_something_in_current_thread();
  } catch (...) {
    t.join();
    throw;
  }
  t.join();
}
}  // namespace test6

namespace test7 {
// 如需确保线程在函数之前结束，使用资源获取即初始化方式，
// RAII，Resource Acquisition Is Initialization，
// 并且提供一个类，在析构函数中使用join()。
// std::thread支持移动的好处是可以创建thread_guard实例，并且拥有线程所有权。
// 当thread_guard对象所持有的线程被引用时，移动操作就可以避免很多不必要的麻烦；
// 这意味着，当某个对象转移了线程的所有权后，它就不能对线程进行加入或分离。
class thread_guard {
  std::thread &t;

 public:
  explicit thread_guard(std::thread &t_) : t(t_) {}
  ~thread_guard() {
    if (t.joinable()) {
      t.join();
    }
  }
  // 拷贝构造函数和拷贝赋值操作被标记为=delete，是为了不自动生成它们。
  // 直接对一个对象进行拷贝或赋值是危险的，因为这可能会弄丢已经加入的线程。
  thread_guard(thread_guard const &) = delete;
  thread_guard &operator=(thread_guard const &) = delete;
};
void func() {
  std::thread t([]() { std::cout << "thread_guard" << std::endl; });
  thread_guard g(t);
  // do_something_in_current_thread

  // func退出时局部对象就要被逆序销毁了，thread_guard对象g是第一个被销毁的，
  // 这时线程在析构函数中被加入到原始线程中。
  // 即使do_something_in_current_thread抛出一个异常，这个销毁依旧会发生。
}
}  // namespace test7

namespace test8 {
class scoped_thread {
  std::thread t;

 public:
  // thread_guard类，需要在析构中检查线程是否可加入，
  // 这里把检查放在了构造函数中，并且当线程不可加入时，抛出异常。
  explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
    if (!t.joinable()) throw std::logic_error("No thread");
  }
  // 当主线程到达函数末尾时，scoped_thread对象就会销毁，
  // 然后加入到的构造函数创建的线程对象中去。
  ~scoped_thread() { t.join(); }
  scoped_thread(scoped_thread const &) = delete;
  scoped_thread &operator=(scoped_thread const &) = delete;
};

void func() {
  scoped_thread t(
      std::thread([]() { std::cout << "scoped_thread" << std::endl; }));
  // scoped_thread

  try {
    // scoped_thread t(std::thread());  // 函数声明
    scoped_thread t((std::thread()));
  } catch (const std::logic_error &e) {
    std::cout << e.what() << std::endl;  // No thread
  }
}
}  // namespace test8
}  // namespace n2

namespace n3 {
// 创建线程时需要传递函数名，函数对象会被复制到线程内存空间中执行与调用。
// 如果用于创建线程的函数有参数，那么在创建线程时，要将函数的参数也传入。
// 常见的，传入的参数的形式有基本数据类型、引用、指针、对象。
// 总体来说，std::thread的构造函数会拷贝传入的参数：
// 1.当传入参数为基本数据类型时，会拷贝一份给创建的线程；
// 2.当传入参数为指针时，会浅拷贝一份给创建的线程，也就是说，
//   只会拷贝对象的指针，不会拷贝指针指向的对象本身;
// 3.当传入的参数为引用时，实参必须用ref()函数处理后传递给形参，
//   否则编译不通过，此时不存在拷贝行为。

void func1(int i) { std::cout << "func1, i: " << ++i << std::endl; }
void func2(int &i) { std::cout << "func2, i: " << ++i << std::endl; }
void func3(int *i) { std::cout << "func3, i: " << ++(*i) << std::endl; }
int func4(const char *fun, ...) {
  va_list ap;         // 指针
  va_start(ap, fun);  // 开始
  vprintf(fun, ap);   // 调用
  va_end(ap);
  return 0;
}

void func() {
  int i = 1;
  std::cout << "before, i: " << i << std::endl;
  // before, i: 1

  std::thread t1(func1, i);
  t1.join();
  std::cout << "after func1, i: " << i << std::endl;
  // func1, i: 2
  // after func1, i: 1

  std::thread t2(func2, std::ref(i));
  t2.join();
  std::cout << "after func2, i: " << i << std::endl;
  // func2, i: 2
  // after func2, i: 2

  std::thread t3(func3, &i);
  std::cout << "after func3, i: " << i << std::endl;
  // func3, i: 3
  // after func3, i: 3

  std::thread t4(func4, "%s %d %c %f %c", "hello!", 100, 'A', 3.14159, '\n');
  t4.join();
  // hello! 100 A 3.141590
}
}  // namespace n3

namespace n4 {
void globleFunc() { std::cout << "globleFunc" << std::endl; }

class ClassFunc {
 public:
  void memberFunc() { std::cout << "memberFunc" << std::endl; }
  static void staticMemberFunc() {
    std::cout << "staticMemberFunc" << std::endl;
  }
};

class ClassOperator {
 public:
  ClassOperator() { std::cout << "constructor" << std::endl; }
  void operator()() const { std::cout << "operator() " << std::endl; }
};

void func() {
  std::thread t1(globleFunc);  // 全局函数
  t1.join();
  // globleFunc

  std::thread t2(ClassFunc::staticMemberFunc);  // 类中的静态成员函数
  t2.join();
  // staticMemberFunc

  ClassFunc cf;
  std::thread t3(std::bind(&ClassFunc::memberFunc, &cf));  // 类的普通成员函数
  t3.join();
  std::thread t4(&ClassFunc::memberFunc, &cf);  // 类的普通成员函数
  t4.join();
  // memberFunc
  // memberFunc

  std::thread t5([]() { std::cout << "lamdafunc" << std::endl; });
  t5.join();
  // lamdafunc

  std::function<void()> f1 = []() { std::cout << "function" << std::endl; };
  std::thread t6(f1);  // function对象
  t6.join();
  // function

  typedef void (*FF)();
  FF f2 = globleFunc;
  std::thread t7(f2);  // 函数指针
  t7.join();
  // globleFunc

  // 当把函数对象传入到线程构造函数中时， 需要避免最令人头痛的语法解析。
  // 如果传递了一个临时变量，而不是一个命名的变量，会将其解析为函数声明。
  // 声明为函数，带有一个参数，返回std::thread的函数，而非启动了一个线程：
  std::thread t8(ClassOperator());
  // 下面方式可以避免上面的问题：
  // 1.将带有函数调用符类型的实例传入std::thread类中，替换默认的构造函数：
  std::thread t9((ClassOperator()));  // 使用多组括号
  std::thread t10{ClassOperator()};   // 使用新统一的初始化语法
  t9.join();
  t10.join();
  // constructor
  // constructor
  // operator()
  // operator()

  // 2.使用lambda表达式也能避免这个问题：
  std::thread t11([] { std::cout << "lamda " << std::endl; });
  t11.join();
  // lambda
}
}  // namespace n4

namespace n5 {
// 线程功能拓展
class MyThread : public std::thread {  // 继承thread
 public:
  // 子类MyThread()继承thread()的构造函数：
  MyThread() : std::thread() {}
  // MyThread()初始化构造函数：
  template <typename T, typename... Args>
  MyThread(T &&func, Args &&... args)
      : std::thread(std::forward<T>(func), std::forward<Args>(args)...) {}
  void showcmd(const char *str) { system(str); }
};

// 在类内部创建线程
class HelloThread {
 public:
  // start()和hellothread()方法都必须是static方法：
  static void hellothread() {
    std::cout << "hello world, I'm a thread!" << std::endl;
  }
  static void start() {
    std::thread thrd(hellothread);
    thrd.join();
  }
};

void func() {
  MyThread t1([]() { std::cout << "hello" << std::endl; });
  t1.showcmd("ls");
  t1.join();
  // hello
  // a.out       condition_variable.cpp  mutex.cpp  thread_base.cpp
  // atomic.cpp  future.cpp              readme.md  thread_base.h

  MyThread t2([](const char *str) { std::cout << "hello" << str << std::endl; },
              " this is MyThread");
  t2.showcmd("pwd");
  t2.join();
  // hello this is MyThread
  // /disk/me/examples/c++/c++11/thread

  HelloThread::start();
  // hello world, I'm a thread!
}
}  // namespace n5

namespace n6 {
// 线程标识类型为std::thread::id，并可以通过两种方式进行检索：
// 1.可以通过调用std::thread对象的成员函数get_id()来直接获取。
//   如果std::thread对象没有与任何执行线程相关联，
//   get_id()将返回std::thread::type默认构造值，这个值表示无线程。
// 2.当前线程中调用std::this_thread::get_id()也可以获得线程标识。

// std::thread::id对象可以自由的拷贝和对比，因为标识符就可以复用：
// 1.如果两个对象的std::thread::id相等，那它们就是同一个线程，或者都无线程。
// 2.如果不等，那么就代表了两个不同线程，或者一个有线程，另一没有线程。

// C++不会限制检查线程标识是否一样，std::thread::id提供相当丰富的对比操作，
// 比如，提供为不同的值进行排序，这意味着允许做为容器的键或做其他方式的比较。
// 按默认顺序比较不同值的std::thread::id，所以这个行为可预见的：
// 当a<b，b<c时，得a<c，等等。
// 标准库也提供std::hash<std::thread::id>容器，
// 所以std::thread::id也可以作为无序容器的键值。
// std::thread::id可以作为一个线程的通用标识符。
// 也可以使用输出流来记录一个std::thread::id对象的值。
// 具体的输出结果是严格依赖于具体实现的，
// C++标准的唯一要求就是要保证ID比较结果相等的线程，必须有相同的输出。

namespace test1 {
void f() {
  // sleep_for：线程休眠某个指定的时间片(time span)，
  // 该线程才被重新唤醒，不过由于线程调度等原因，
  // 实际休眠时间可能比sleep_duration所表示的时间片更长。
  std::this_thread::sleep_for(std::chrono::seconds(1));
}
void func() {
  // get_id：获取线程ID，返回一个类型为std::thread::id的对象。
  // swap：交换两个线程对象所代表的底层句柄。
  std::thread t1(f);
  std::thread t2(f);
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;
  std::swap(t1, t2);
  std::cout << "after std::swap(t1, t2):" << std::endl;
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;
  t1.swap(t2);
  std::cout << "after t1.swap(t2):" << std::endl;
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;
  t1.join();
  t2.join();

  // thread 1 id: 140418301437696
  // thread 2 id: 140418293044992
  // after std::swap(t1, t2):
  // thread 1 id: 140418293044992
  // thread 2 id: 140418301437696
  // after t1.swap(t2):
  // thread 1 id: 140418301437696
  // thread 2 id: 140418293044992
}
}  // namespace test1

namespace test2 {
std::mutex iomutex;
void f(int num) {
  std::this_thread::sleep_for(std::chrono::seconds(1));

  sched_param sch;
  int policy;
  pthread_getschedparam(pthread_self(), &policy, &sch);
  std::lock_guard<std::mutex> lk(iomutex);
  std::cout << "Thread " << num << " is executing at priority "
            << sch.sched_priority << '\n';
}

void func() {
  // native_handle：返回与std::thread具体实现相关的线程句柄。
  // std::thread的实现和操作系统相关，
  // 在符合Posix标准的平台下（如Unix/Linux）是pthread库：
  std::thread t1(f, 1), t2(f, 2);
  sched_param sch;
  int policy;
  pthread_getschedparam(t1.native_handle(), &policy, &sch);  // unix api
  sch.sched_priority = 20;
  if (pthread_setschedparam(t1.native_handle(), SCHED_FIFO, &sch)) {
    std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
  }
  t1.join();
  t2.join();

  // Failed to setschedparam: Operation not permitted
  // Thread 1 is executing at priority 0
  // Thread 2 is executing at priority 0
}
}  // namespace test2

namespace test3 {
void func1() {
  // std::thread::hardware_concurrency()：
  // 在新版C++标准库中是一个很有用的函数，返回能并发在一个程序中的线程数量。
  // 多核系统中，返回值可以是CPU核芯的数量。
  // 返回值也仅仅是一个提示，当系统信息无法获取时，函数也会返回0。
  // 但是，这也无法掩盖这个函数对启动线程数量的帮助。
  // 因为上下文频繁切换会降低线程性能，所以不希望线程数多于硬件支持的线程数量。
  // 当std::thread::hardware_concurrency()返回0，自己设置一个。
  unsigned int n = std::thread::hardware_concurrency();
  std::cout << n << " concurrent threads are supported." << std::endl;
  // 8 concurrent threads are supported.
}

void little_sleep(std::chrono::microseconds us) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + us;
  do {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < end);
}

void func2() {
  // this_thread::yield：当前线程放弃执行，操作系统调度另一线程继续执行。
  auto start = std::chrono::high_resolution_clock::now();
  little_sleep(std::chrono::microseconds(100));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  std::cout
      << "waited for "
      << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
      << " microseconds\n";
  // waited for 102 microseconds
}
void func3() {
  // sleep_until：线程休眠至某个指定的时刻，该线程才被重新唤醒。
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm *ptm = std::localtime(&tt);
  std::cout << "Current time: " << std::put_time(ptm, "%X") << '\n';
  std::cout << "Waiting for the next minute to begin...\n";
  ++ptm->tm_min;
  ptm->tm_sec = 0;
  std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
  std::cout << std::put_time(ptm, "%X") << " reached!\n";
  // Current time: 15:44:27
  // Waiting for the next minute to begin...
  // 15:45:00 reached!
}

void func() {
  func1();
  func2();
  func3();
}
}  // namespace test3
}  // namespace n6

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 14]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::func();
      break;
    case 1:
      n2::test1::func();
      break;
    case 2:
      n2::test2::func();
      break;
    case 3:
      n2::test3::func();
      break;
    case 4:
      n2::test4::func();
      break;
    case 5:
      n2::test5::func();
      break;
    case 6:
      n2::test6::func();
      break;
    case 7:
      n2::test7::func();
      break;
    case 8:
      n2::test8::func();
      break;
    case 9:
      n3::func();
      break;
    case 10:
      n4::func();
      break;
    case 11:
      n5::func();
      break;
    case 12:
      n6::test1::func();
      break;
    case 13:
      n6::test2::func();
      break;
    case 14:
      n6::test3::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}

// GCC默认没有加载pthread库，据说在后续的版本中可以不加-lpthread选项：
// g++ -std=c++11 thread.cpp -lpthread