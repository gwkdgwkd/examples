#include <pthread.h>
#include <stdarg.h>
#include <cassert>
#include <cstring>
#include <ctime>    // std::time_t, std::tm, std::localtime, std::mktime
#include <iomanip>  // std::put_time
#include <iostream>
#include <mutex>
#include <thread>

// https://www.freesion.com/article/32321338069/

using namespace std;

// 计算机术语中的"并发"，指的是在单个系统里同时执行多个独立的活动，而不是顺序的一个接一个的执行。对于单核CPU来说，在某个时刻只可能处理一个任务，
// 但它却不是完全执行完一个任务再执行一个下一任务，而是一直在任务间切换，每个任务完成一点就去执行下一个任务，看起来就像任务在并行发生，虽然不是严
// 格的同时执行多个任务，但是我们仍然称之为并发(concurrency)。
// 真正的并发是在在多核CPU上，能够真正的同时执行多个任务，称为硬件并发(hardware concurrency)。
// 并发并非没有代价，在单核CPU并发执行两个任务需要付出上下文切换的时间代价。

// 线程是操作系统能够进行CPU调度的最小单位，它被包含在进程之中，一个进程可包含单个或者多个线程。可以用多个线程去完成一个任务，也可以用多个进程去
// 完成一个任务，它们的本质都相当于多个人去合伙完成一件事。

// 并发的方式
// 1 多进程并发:
// 将应用程序分为多个独立的、单线程的进程，他们可以同时运行。
// 这些独立的进程可以通过常规的进程间通信机制进行通信，如管道、信号、消息队列、共享内存、存储映射I/O、信号量、套接字等。
// 缺点：
//     进程间通信较为复杂，速度相对线程间的通信更慢。
//     启动进程的开销比线程大，使用的系统资源也更多。
// 优点：
//     进程间通信的机制相对于线程更加安全。
//     能够很容易的将一台机器上的多进程程序部署在不同的机器上（如果通信机制选取的是套接字的话）。

// 2 多线程并发:
// 线程很像轻量级的进程，但是一个进程中的所有线程都共享相同的地址空间，线程间的大部分数据都可以共享。线程间的通信一般都通过共享内存来实现。
// 任何程序都是一个进程，main()函数就是其中的主线程，单个线程都是顺序执行。
// 优点：
//     由于可以共享数据，多线程间的通信开销比进程小的多。
//     线程启动的比进程快，占用的资源更少。
// 缺点：
//     共享数据太过于灵活，为了维护正确的共享，代码写起来比较复杂。
//     无法部署在分布式系统上。

// 为什么使用并发,主要原因有两个：任务拆分和提高性能。
// 1 任务拆分
// 在编写软件的时候，将相关的代码放在一起，将无关的代码分开，这是一个好主意，这样能够让程序更加容易理解和测试。
// 将程序划分成不同的任务，每个线程执行一个任务或者多个任务，可以将整个程序的逻辑变得更加简单。
// 2 提高性能,在两种情况下，并发能够提高性能。
// 任务并行(task parallelism): 将一个单个任务分成若干个部分各自并行运行，从而降低运行时间。虽然听起来很简单，但其实是一个相当复杂的过程，
// 设想假如各个部分之间存在很多以来，一个部分的执行需要使用到另一个任务的执行结果，这个时候并不能很好的并行完成。
// 数据并行(data parallelism)：每个线程在不同的数据部分上执行相同的操作。

// C++98标准中并没有线程库的存在，而在C++11中终于提供了多线程的标准库，提供了管理线程、保护共享数据、线程间同步操作、原子操作等类。
// 多线程库对应的头文件是#include <thread>，类名为std::thread。
// C++11中std命名空间将Boost库中的Thread加入，Boost的多线程从准标准变为标准
// Linux程序员都用过Pthread, 但有C++11的std::thread后，可以在语言层面编写多线程程序，直接的好处就是多线程程序的可移植性得到了很大的提高

// 与C++11多线程相关的头文件
// C++11新标准中引入了四个头文件来支持多线程编程，他们分别是<atomic> ,<thread>,<mutex>,<condition_variable>和<future>。
//  <atomic>：该头文主要声明了两个类,std::atomic和std::atomic_flag，另外还声明了一套C风格的原子类型和与C兼容的原子操作的函数。
//  <thread>：该头文件主要声明了std::thread类，另外std::this_thread命名空间也在该头文件中。
//  <mutex>：该头文件主要声明了与互斥量(mutex)相关的类，包括std::mutex系列类，std::lock_guard,std::unique_lock,以及其他的类型和函数。
//  <condition_variable>：该头文件主要声明了与条件变量相关的类，包括std::condition_variable和std::condition_variable_any。
//  <future>：该头文件主要声明了std::promise,std::package_task两个Provider类，以及std::future和std::shared_future两个Future类，
//            另外还有一些与之相关的类型和函数，std::async()函数就声明在此头文件中。

// std::thread构造函数:
// 默认构造函数，创建一个空的std::thread执行对象。
// 初始化构造函数，创建一个std::thread对象，该std::thread对象可被joinable，新产生的线程会调用fn函数，该函数的参数由args给出。
// 拷贝构造函数(被禁用)，意味着 std::thread对象不可拷贝构造。
// move构造函数，调用成功之后x不代表任何std::thread执行对象。
// 注：可被joinable的std::thread对象必须在他们销毁之前被主线程join或者将其设置为detached.
void f1(int n) {  // 初始化构造函数
  for (int i = 0; i < 5; ++i) {
    std::cout << "Thread " << n << " executing\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
void f2(int &n) {  // 拷贝构造函数
  for (int i = 0; i < 5; ++i) {
    std::cout << "Thread 2 executing\n";
    ++n;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

// std::thread赋值操作:
// Move赋值操作(1)，如果当前对象不可joinable，需要传递一个右值引用(rhs)给move赋值操作；如果当前对象可被joinable，则会调用terminate()报错。
// 拷贝赋值操作(2)，被禁用，因此std::thread对象不可拷贝赋值。
void thread_task(int n) {
  std::this_thread::sleep_for(std::chrono::seconds(n));
  std::cout << "hello thread " << std::this_thread::get_id() << " paused " << n
            << " seconds" << std::endl;
}

// 其他成员函数:
// get_id:获取线程ID，返回一个类型为std::thread::id的对象。

// joinable:检查线程是否可被join。检查当前的线程对象是否表示了一个活动的执行线程，由默认构造函数创建的线程是不能被join的。另外，如果某个线程已
// 经执行完任务，但是没有被join的话，该线程依然会被认为是一个活动的执行线程，因此也是可以被join的。

// join: Join线程，调用该函数会阻塞当前线程，直到由*this所标示的线程执行完毕join才返回。

// detach:Detach线程。将当前线程对象所代表的执行实例与该线程对象分离，使得线程的执行可以单独进行。一旦线程执行完毕，它所分配的资源将会被释放。
// 调用detach 函数之后：
//   *this 不再代表任何的线程执行实例。
//   joinable() == false
//   get_id() == std::thread::id()
// 另外，如果出错或者joinable()==false，则会抛出std::system_error。

// swap:Swap线程，交换两个线程对象所代表的底层句柄(underlying handles)。

// native_handle:返回native handle（由于std::thread的实现和操作系统相关，因此该函数返回与std::thread具体实现相关的线程句柄，例如在符合
// Posix标准的平台下(如Unix/Linux)是Pthread库）。

// hardware_concurrency [static]: 检测硬件并发特性，返回当前平台的线程实现所支持的线程并发数目，但返回值仅仅只作为系统提示(hint)。

// std::this_thread命名空间中相关辅助函数介绍:
// get_id:获取线程ID。
// yield:当前线程放弃执行，操作系统调度另一线程继续执行。
// sleep_until:线程休眠至某个指定的时刻(time point)，该线程才被重新唤醒。
// sleep_for:线程休眠某个指定的时间片(time span)，该线程才被重新唤醒，不过由于线程调度等原因，实际休眠时间可能比sleep_duration所表示的时间片更长。

void foo() { std::this_thread::sleep_for(std::chrono::seconds(1)); }
void bar() { std::this_thread::sleep_for(std::chrono::seconds(1)); }
void independentThread() {
  std::cout << "Starting concurrent thread.\n";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "Exiting concurrent thread.\n";
}
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
std::mutex g_display_mutex;
void foo1() {
  std::thread::id this_id = std::this_thread::get_id();
  g_display_mutex.lock();
  std::cout << "thread " << this_id << " sleeping...\n";
  g_display_mutex.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}
void little_sleep(std::chrono::microseconds us) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + us;
  do {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < end);
}

// 创建线程:
// 语句"std::thread th1(proc1);"创建了一个名为th1的线程，并且线程th1开始执行。
// 实例化std::thread类对象时，至少需要传递函数名作为参数(这个函数就是这个线程的入口函数，函数执行完了，整个线程也就执行完了)。如果函数为有参函
// 数,如"void proc2(int a,int b)",那么实例化std::thread类对象时，则需要传递更多参数，参数顺序依次为函数名、该函数的第一个参数、该函数的第
// 二个参数，···，如"std::thread th2(proc2,a,b);"。
// 只要创建了线程对象（前提是，实例化std::thread对象时传递了“函数名/可调用对象”作为参数），线程就开始执行。并没有一个类似start的函数来显式启动。
// 总之，使用C++线程库启动线程，可以归结为构造std::thread对象。
// 一旦线程开始运行， 就需要显式的决定是要等待它完成(join)，或者分离它让它自行运行(detach)。注意：只需要在std::thread对象被销毁之前做出这个决定。
// 当线程启动后，一定要在和线程相关联的std::thread对象销毁前，对线程运用join()或者detach()方法。
// join()与detach()都是std::thread类的成员函数，是两种线程阻塞方法，两者的区别是是否等待子线程执行结束。
// join()函数的另一个任务是回收该线程中使用的资源,会清理线程相关的存储部分，这代表了join()只能调用一次。使用joinable()来判断join()可否调用。
// 同样，detach()也只能调用一次，一旦detach()后就无法join()了，有趣的是，detach()可否调用也是使用joinable()来判断。
// 如果使用detach()，就必须保证线程结束之前可访问数据的有效性，使用指针和引用需要格外谨慎。
// 线程对象和对象内部管理的线程的生命周期并不一样，如果线程执行的快，可能内部的线程已经结束了，但是线程对象还活着，也有可能线程对象已经被析构了，内部
// 的线程还在运行。
// 主线程并不想等待子线程结束就想结束整个任务，直接删掉t1.join()是不行的，程序会被终止（析构t1的时候会调用std::terminate，
// 程序会打印terminate called without an active exception）。
// 调用t1.detach()，从而将线程放在后台运行，所有权和控制权被转交给C++运行时库，以确保与线程相关联的资源在线程退出后能被正确的回收。参考UNIX的守护
// 进程(daemon process)的概念，这种被分离的线程被称为守护线程(daemon threads)。线程被分离之后，即使该线程对象被析构了，线程还是能够在后台运行，
// 只是由于对象被析构了，主线程不能够通过对象名与这个线程进行通信。

void function_1() {
  // 延时500ms为了保证test()运行结束之后才打印
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "I'm function_1()" << std::endl;
}
void test() {
  std::thread t1(function_1);
  // t1.detach();
  t1.join();
  std::cout << "test() finished" << std::endl;
}

// 一旦一个线程被分离了，就不能够再被join了。如果非要调用，程序就会崩溃，可以使用joinable()函数判断一个线程对象能否调用join()。
void test1() {
  std::thread t1(function_1);
  t1.detach();  // 脱离主线程的绑定，主线程挂了，子线程不报错，子线程执行完自动退出。detach以后，子线程会成为孤儿线程，线程之间将无法通信。
  if (t1.joinable()) t1.join();
  assert(!t1.joinable());
}

void show() { cout << "hello cplusplus!" << endl; }
void show1(const char *str, const int id) {
  cout << "线程 " << id + 1 << " ：" << str << endl;
}
// 多线程可变参数
int show2(const char *fun, ...) {
  va_list ap;         // 指针
  va_start(ap, fun);  // 开始
  vprintf(fun, ap);   // 调用
  va_end(ap);
  return 0;
}

// 线程功能拓展
class MyThread : public thread  // 继承thread
{
 public:
  // 子类MyThread()继承thread()的构造函数
  MyThread() : thread() {}
  // MyThread()初始化构造函数
  template <typename T, typename... Args>
  MyThread(T &&func, Args &&... args)
      : thread(forward<T>(func), forward<Args>(args)...) {}
  void showcmd(const char *str)  // 运行system
  {
    system(str);
  }
};

// 在类内部创建线程
class HelloThread {
 public:
  // start()和hellothread()方法都必须是static方法
  static void hellothread() { cout << "hello world, I'm a thread!" << endl; }
  static void start() {
    thread thrd(hellothread);
    thrd.join();
  }
};

// 用类内部函数在类外部创建线程
class HelloWorld {
 public:
  void hello(int year) { cout << "I am " << year << " years old" << endl; }
};

// 创建线程时的传参问题分析
// 如“std::thread th1(proc1)”,创建线程时需要传递函数名作为参数，提供的函数对象会复制到新的线程的内存空间中执行与调用。
// 如果用于创建线程的函数为含参函数，那么在创建线程时，要一并将函数的参数传入。常见的，传入的参数的形式有基本数据类型(int，char,string等)、引用、指针、对象这些
// 总体来说，std::thread的构造函数会拷贝传入的参数:
// 1 当传入参数为基本数据类型(int，char,string等)时，会拷贝一份给创建的线程；
// 2 当传入参数为指针时，会浅拷贝一份给创建的线程，也就是说，只会拷贝对象的指针，不会拷贝指针指向的对象本身;
// 3 当传入的参数为引用时，实参必须用ref()函数处理后传递给形参，否则编译不通过，此时不存在“拷贝”行为。引用只是变量的别名，在线程中传递对象的引用，
//   那么该对象始终只有一份，只是存在多个别名罢了
void proc(int &x) {
  cout << x << "," << &x << endl;
  x *= 10;
}

int main() {
  // 构造函数
  int n = 0;
  std::thread t1;                   // t1 is not a thread
  std::thread t2(f1, n + 1);        // pass by value
  std::thread t3(f2, std::ref(n));  // pass by reference
  // t4 is now running f2(). t3 is no longer a thread
  std::thread t4(std::move(t3));
  t2.join();
  t4.join();
  std::cout << "Final value of n is " << n << '\n';

  // 赋值操作
  std::thread threads[2];
  std::cout << "Spawning 2 threads...\n";
  for (int i = 0; i < 2; i++) {
    threads[i] = std::thread(thread_task, i + 1);
  }
  std::cout << "Done spawning threads! Now wait for them to join\n";
  for (auto &t : threads) {
    t.join();
  }
  std::cout << "All threads joined.\n";

  // get_id
  std::thread t11(foo);
  std::thread::id t1_id = t11.get_id();
  std::thread t22(foo);
  std::thread::id t2_id = t22.get_id();
  // id每次的值不同
  std::cout << "t1's id: " << t1_id << '\n';  // 140420714456832
  std::cout << "t2's id: " << t2_id << '\n';  // 140420722849536
  t11.join();
  t22.join();

  // joinable
  std::thread t33;
  std::cout << "before starting, joinable: " << t33.joinable() << '\n';  // 0
  t33 = std::thread(foo);
  std::cout << "after starting, joinable: " << t33.joinable() << '\n';  // 1
  t33.join();

  // join
  std::cout << "starting first helper...\n";
  std::thread helper1(foo);
  std::cout << "starting second helper...\n";
  std::thread helper2(bar);
  std::cout << "waiting for helpers to finish..." << std::endl;
  helper1.join();
  helper2.join();
  std::cout << "done!\n";

  // detach
  std::cout << "Starting thread caller.\n";
  std::thread t44(independentThread);
  t44.detach();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Exiting thread caller.\n";
  std::this_thread::sleep_for(std::chrono::seconds(3));
  // Starting thread caller.
  // Starting concurrent thread.
  // Exiting thread caller.
  // Exiting concurrent thread.

  // swap
  std::thread tt1(foo);
  std::thread tt2(bar);
  std::cout << "thread 1 id: " << tt1.get_id() << std::endl;
  std::cout << "thread 2 id: " << tt2.get_id() << std::endl;
  std::swap(tt1, tt2);
  std::cout << "after std::swap(tt1, tt2):" << std::endl;
  std::cout << "thread 1 id: " << tt1.get_id() << std::endl;
  std::cout << "thread 2 id: " << tt2.get_id() << std::endl;
  tt1.swap(tt2);
  std::cout << "after tt1.swap(tt2):" << std::endl;
  std::cout << "thread 1 id: " << tt1.get_id() << std::endl;
  std::cout << "thread 2 id: " << tt2.get_id() << std::endl;
  tt1.join();
  tt2.join();
  // thread 1 id: 139857369171712
  // thread 2 id: 139857360779008
  // after std::swap(tt1, tt2):
  // thread 1 id: 139857360779008
  // thread 2 id: 139857369171712
  // after tt1.swap(tt2):
  // thread 1 id: 139857369171712
  // thread 2 id: 139857360779008

  // native_handle
  std::thread x1(f, 1), x2(f, 2);
  sched_param sch;
  int policy;
  pthread_getschedparam(x1.native_handle(), &policy, &sch);
  sch.sched_priority = 20;
  if (pthread_setschedparam(x1.native_handle(), SCHED_FIFO, &sch)) {
    std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
  }
  x1.join();
  x2.join();
  // Failed to setschedparam: Operation not permitted
  // Thread 1 is executing at priority 0
  // Thread 2 is executing at priority 0
  // sudo
  // Thread 2 is executing at priority 0
  // Thread 1 is executing at priority 20

  // hardware_concurrency
  unsigned int n1 = std::thread::hardware_concurrency();
  std::cout << n1 << " concurrent threads are supported.\n";
  // 8 concurrent threads are supported.

  // this_thread::get_id
  std::thread y1(foo1);
  std::thread y2(foo1);
  std::cout << "thread : " << y1.get_id() << std::endl;
  std::cout << "thread : " << y2.get_id() << std::endl;
  y1.join();
  y2.join();
  // thread 139677731620608 sleeping...
  // thread : 139677731620608
  // thread thread : 139677723227904
  // 139677723227904 sleeping...

  // this_thread::yield
  auto start = std::chrono::high_resolution_clock::now();
  little_sleep(std::chrono::microseconds(100));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  std::cout
      << "waited for "
      << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
      << " microseconds\n";

  // this_thread::sleep_until
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

  // this_thread::sleep_for
  std::cout << "Hello waiter" << std::endl;
  std::chrono::milliseconds dura(2000);
  std::this_thread::sleep_for(dura);
  std::cout << "Waited 2000 ms\n";

  test();
  // 让主线程晚于子线程结束
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // 延时1s
  // 使用t1.detach()时
  // test() finished
  // I'm function_1()
  // 使用t1.join()时
  // I'm function_1()
  // test() finished
  // 由于线程入口函数内部有个500ms的延时，所以在还没有打印的时候，test()已经执行完成了，t1已经被析构了，但是它负责的那个线程还是能够运行，这就是detach()的作用。
  // 如果去掉main函数中的1s延时，会发现什么都没有打印，因为主线程执行的太快，整个程序已经结束了，那个后台线程被C++运行时库回收了。
  // 如果将t1.detach()换成t1.join()，test函数会在t1线程执行结束之后，才会执行结束。

  // 栈上
  // thread z1(show);  // 根据函数初始化执行
  // thread z2(show);
  // thread z3(show);
  // 线程数组
  // thread th[3]{thread(show), thread(show), thread(show)};
  // 堆上
  // thread *pt1(new thread(show));
  // thread *pt2(new thread(show));
  // thread *pt3(new thread(show));
  // 线程指针数组
  // thread *pth(new thread[3]{thread(show), thread(show), thread(show)});

  thread v1(show1, "hello cplusplus!", 0);
  thread v2(show1, "你好，C++！", 1);
  thread v3(show1, "hello!", 2);
  v1.join();
  v2.join();
  v3.join();

  auto fun = [](const char *str) { cout << str << endl; };
  thread l1(fun, "hello world!");
  thread l2(fun, "hello beijing!");
  l1.join();
  l2.join();

  // 线程d2可以通过move移动t1来获取d1的全部属性，而d1却销毁了。
  thread d1([]() { cout << "thread1" << endl; });
  cout << "thread1' id is " << d1.get_id() << endl;
  thread d2 = move(d1);
  cout << "thread2' id is " << d2.get_id() << endl;
  // d1.join();
  d2.join();

  thread m1(show2, "%s %d %c %f", "hello world!", 100, 'A', 3.14159);
  m1.join();

  MyThread c1([]() { cout << "hello" << endl; });
  c1.showcmd("calc");  // 运行calc
  MyThread c2([](const char *str) { cout << "hello" << str << endl; },
              " this is MyThread");
  c2.showcmd("notepad");  // 运行notepad
  c1.join();
  c2.join();

  HelloThread::start();

  HelloWorld obj;
  thread thrd(std::bind(&HelloWorld::hello, &obj, 26));  // I am 26 years old
  thrd.join();

  int a = 10;
  cout << a << "," << &a << endl;
  thread a1(proc, ref(a));
  a1.join();
  cout << a << "," << &a << endl;
  // 10,0x7ffc49835df4
  // 10,0x7ffc49835df4
  // 100,0x7ffc49835df4

  return 0;
}

// GCC默认没有加载pthread库，据说在后续的版本中可以不用在编译时添加-pthread选项。
// g++ -std=c++11 thread.cpp -lpthread

// https://www.bookstack.cn/books/CPP-Concurrency-In-Action-2ed-2019