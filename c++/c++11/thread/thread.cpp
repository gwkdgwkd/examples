#include <pthread.h>
#include <stdarg.h>
#include <cstring>
#include <ctime>  // std::time_t, std::tm, std::localtime, std::mktime
#include <functional>
#include <iomanip>  // std::put_time
#include <iostream>
#include <mutex>
#include <thread>

namespace create {
// 只要创建了线程对象，线程就开始执行。
// 并没有一个类似start的函数来显式启动。
// 使用C++线程库启动线程，可以归结为构造std::thread对象。

void func() { std::cout << __FUNCTION__ << " running" << std::endl; }

// std::thread支持移动，意味着std::thread可以当作函数的返回值和参数：
void func1(std::thread t) {}
std::thread func2() {
  return std::thread([]() {});
}

void testCreate() {
  // 默认构造函数，创建一个空的std::thread执行对象
  std::thread t1;
  std::thread threads[2];  // 空的thread数组

  // 初始化构造函数，创建一个std::thread对象，该std::thread对象可被joinable
  std::thread t2(func);
  t2.join();

  // 拷贝构造函数(被禁用)，意味着std::thread对象不可拷贝构造
  // std::thread t3 = t2;
  // std::thread t4(t2);

  // 赋值操作符
  t1 = std::thread(func);
  t1.join();
  for (int i = 0; i < 2; i++) {
    threads[i] = std::thread(func);
  }
  for (auto &t : threads) {
    t.join();
  }

  // 移动拷贝构造函数
  std::thread t5(func);
  std::cout << "t5 id is " << t5.get_id() << std::endl;  // 139858793432832
  std::thread t6 = std::move(t5);  // t6获t5的全部属性，t5被消耗了
  std::cout << "t6 id is " << t6.get_id() << std::endl;  // 139858793432832
  // t5.join();  // terminate called after throwing an instance of 'std::system_error'
  t6.join();

  std::thread t7(func);                                     // 栈上
  std::thread t8[2]{std::thread(func), std::thread(func)};  // 线程数组
  std::thread *t9(new std::thread(func));                   // 堆上
  std::thread *t10(new std::thread[2]{std::thread(func),
                                      std::thread(func)});  // 线程指针数组
  t7.join();
  t8[0].join();
  t8[1].join();
  t9->join();
  t10[0].join();
  t10[1].join();
}
}  // namespace create

namespace joinable {
// 一旦线程开始运行，就需要显式的决定是要等待它完成(join)，
// 或者分离它让它自行运行(detach)。
// 当线程启动后，一定要在和线程相关联的std::thread对象销毁前，
// 对线程运用join()或者detach()方法。
// 否则程序将会终止，因为std::thread的析构函数会调用std::terminate()，
// 这时再去决定会触发相应异常。
// join()与detach()都是std::thread类的成员函数，
// 是两种线程阻塞方法，两者的区别是是否等待子线程执行结束。
// join线程，调用该函数会阻塞当前线程，直到由*this所标示的线程执行完毕join才返回。
// detach线程,将当前线程对象所代表的执行实例与该线程对象分离，
// 使得线程的执行可以单独进行，线程执行完毕，资源将会被释放。

// join()函数的另一个任务是回收该线程中使用的资源，
// 会清理线程相关的存储部分，这代表了join()只能调用一次。
// detach()也只能调用一次，一旦detach()后就无法join()了。
// 使用joinable检查线程是否可被join，检查当前的线程对象是否表示了一个活动的执行线程。
// 有趣的是，detach()可否调用也是使用joinable()来判断。

// 如果使用detach()，就必须保证线程结束之前可访问数据的有效性，使用指针和引用需要格外谨慎。
// 主线程并不想等待子线程结束就想结束整个任务，直接删掉t1.join()是不行的，
// 程序会被终止，析构t1的时候会调用std::terminate，
// 程序会打印terminate called without an active exception。
// 调用t1.detach()，从而将线程放在后台运行，所有权和控制权被转交给C++运行时库，
// 以确保与线程相关联的资源在线程退出后能被正确的回收。
// 参考UNIX的守护进程(daemon process)的概念，
// 这种被分离的线程被称为守护线程(daemon threads)。
// 线程被分离之后，即使该线程对象被析构了，线程还是能够在后台运行，
// 只是由于对象被析构了，主线程不能够通过对象名与这个线程进行通信。

void func1() { std::cout << __FUNCTION__ << " running" << std::endl; }
void func2() {
  std::cout << "func2 start" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "func2 end" << std::endl;
}

void func3() {
  // 延时500ms为了保证func4或者func5运行结束之后才打印
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "I'm func3" << std::endl;
}
void func4() {
  std::thread t(func3);
  t.join();
  std::cout << "func4 join finished" << std::endl;
}
void func5() {
  std::thread t(func3);
  t.detach();
  std::cout << "func5 detach finished" << std::endl;
}

// 如果想要分离一个线程，可以在线程启动后，直接使用detach()进行分离。
// 如果打算等待对应线程，则需要细心挑选调用join()的位置。
// 当在线程运行之后产生的异常在join()调用之前抛出，意味着这次调用会被跳过。
// 所以在异常处理中也要调用join()。
void func6() {
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

// 如需确保线程在函数之前结束：
// 使用资源获取即初始化方式(RAII，Resource Acquisition Is Initialization)，
// 并且提供一个类，在析构函数中使用join()。
// std::thread支持移动的好处是可以创建thread_guard类的实例，并且拥有其线程所有权。
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
  // 拷贝构造函数和拷贝赋值操作被标记为=delete，是为了不让编译器自动生成它们。
  // 直接对一个对象进行拷贝或赋值是危险的，因为这可能会弄丢已经加入的线程。
  thread_guard(thread_guard const &) = delete;
  thread_guard &operator=(thread_guard const &) = delete;
};
void func7() {
  int some_local_state = 0;
  std::thread t([]() { std::cout << "thread_guard" << std::endl; });
  thread_guard g(t);
  // do_something_in_current_thread();

  // func7退出时局部对象就要被逆序销毁了。
  // 因此，thread_guard对象g是第一个被销毁的，这时线程在析构函数中被加入到原始线程中。
  // 即使do_something_in_current_thread抛出一个异常，这个销毁依旧会发生。
}

class scoped_thread {
  std::thread t;

 public:
  // thread_guard类，需要在析构中检查线程是否可加入。
  // 这里把检查放在了构造函数中，并且当线程不可加入时，抛出异常。
  explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
    if (!t.joinable()) throw std::logic_error("No thread");
  }
  // 当主线程到达函数末尾时，scoped_thread对象就会销毁，然后加入到的构造函数创建的线程对象中去。
  ~scoped_thread() { t.join(); }
  scoped_thread(scoped_thread const &) = delete;
  scoped_thread &operator=(scoped_thread const &) = delete;
};

void testJoinable() {
  // 由默认构造函数创建的线程是不能被join的
  std::thread t1;
  std::cout << "null thread :" << std::boolalpha << t1.joinable()
            << std::endl;  // false
  // t1.join();  // terminate called after throwing an instance of 'std::system_error'

  // 线程已经执行完任务，但是没有被join的话，该线程依然会被认为是一个活动的执行线程，因此也是可以被join的
  std::thread t2(func1);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  t2.join();

  // join后，joinable为false；没有执行join或detach，joinable为true
  std::thread t3(func2);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "before join : " << std::boolalpha << t3.joinable()
            << std::endl;  // true
  t3.join();
  std::cout << "after join:" << std::boolalpha << t3.joinable()
            << std::endl;  // false

  // detach后，joinable为false，线程中的打印也看不到了
  // 脱离主线程的绑定，主线程挂了，子线程不报错，子线程执行完自动退出
  // detach以后，子线程会成为孤儿线程，线程之间将无法通信
  std::thread t4(func2);
  t4.detach();
  std::cout << "after detach:" << std::boolalpha << t4.joinable()
            << std::endl;  // false
  // 线程被分离了，就不能够再被join了。如果调用，会崩溃
  // t4.join();  // terminate called after throwing an instance of 'std::system_error'

  // 线程对象和对象内部管理的线程的生命周期并不一样，如果线程执行的快，
  // 可能内部的线程已经结束了，但是线程对象还活着，也有可能线程对象已经被析构了，内部的线程还在运行。
  func4();
  // I'm func3
  // func4 join finished
  func5();  // 退出时，局部的线程t被析构，但是它负责的线程还能运行，也会有打印
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // func5 detach finished
  // I'm func3
  func5();  // 没有func3的打印，因为主线程执行的太快，整个程序已经结束了，那个后台线程被C++运行时库回收了
  // func5 detach finished

  // 不用写join
  func7();  // thread_guard
  scoped_thread t6(std::thread(
      []() { std::cout << "scoped_thread" << std::endl; }));  // scoped_thread
}
}  // namespace joinable

namespace parameter {
// 创建线程时需要传递函数名作为参数，提供的函数对象会复制到新的线程的内存空间中执行与调用。
// 如果用于创建线程的函数为含参函数，那么在创建线程时，要一并将函数的参数传入。
// 常见的，传入的参数的形式有基本数据类型(int，char,string等)、引用、指针、对象。
// 总体来说，std::thread的构造函数会拷贝传入的参数:
// 1.当传入参数为基本数据类型(int，char,string等)时，会拷贝一份给创建的线程；
// 2.当传入参数为指针时，会浅拷贝一份给创建的线程，也就是说，只会拷贝对象的指针，不会拷贝指针指向的对象本身;
// 3.当传入的参数为引用时，实参必须用ref()函数处理后传递给形参，否则编译不通过，此时不存在“拷贝”行为。
//   引用只是变量的别名，在线程中传递对象的引用，那么该对象始终只有一份，只是存在多个别名罢了。

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

void testParameter() {
  int i = 1;
  std::cout << "before, i: " << i << std::endl;  // 1
  std::thread t1(func1, i);                      // 2
  t1.join();
  std::cout << "after func1, i: " << i << std::endl;  // 1

  std::thread t2(func2, std::ref(i));  // 2
  t2.join();
  std::cout << "after func2, i: " << i << std::endl;  // 2

  std::thread t3(func3, &i);  // 3
  t3.join();
  std::cout << "after func3, i: " << i << std::endl;  // 3

  // hello world! 100 A 3.141590
  std::thread t4(func4, "%s %d %c %f", "hello world!", 100, 'A', 3.14159);
  t4.join();
}
}  // namespace parameter

namespace functype {
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

void testFuncType() {
  std::thread t1(globleFunc);  // 全局函数
  t1.join();

  std::thread t2(ClassFunc::staticMemberFunc);  // 类中的静态成员函数
  t2.join();

  ClassFunc cf;
  std::thread t3(std::bind(&ClassFunc::memberFunc, &cf));  // 类的普通成员函数
  t3.join();
  std::thread t4(&ClassFunc::memberFunc, &cf);  // 类的普通成员函数
  t4.join();

  std::thread t5([]() { std::cout << "lamdafunc" << std::endl; });
  t5.join();

  std::function<void()> f1 = []() { std::cout << "function" << std::endl; };
  std::thread t6(f1);  // function对象
  t6.join();

  typedef void (*FF)();
  FF f2 = globleFunc;
  std::thread t7(f2);  // 函数指针
  t7.join();

  // 当把函数对象传入到线程构造函数中时，需要避免“最令人头痛的语法解析”(C++’s most vexing parse)。
  // 如果你传递了一个临时变量，而不是一个命名的变量；C++编译器会将其解析为函数声明，而不是类型对象的定义。
  // 声明一个名为t8的函数，带有一个参数，返回std::thread1对象的函数，而非启动了一个线程。
  std::thread t8(ClassOperator());
  // 下面方式可以避免上面的问题
  // 将带有函数调用符类型的实例传入std::thread类中，替换默认的构造函数。
  std::thread t9((ClassOperator()));  // 使用多组括号
  std::thread t10{ClassOperator()};   // 使用新统一的初始化语法
  t9.join();
  t10.join();
  // 使用lambda表达式也能避免这个问题
  std::thread t11([] { std::cout << "lamda " << std::endl; });
  t11.join();
  // constructor
  // constructor
  // operator()
  // operator()
  // lamda
}
}  // namespace functype

namespace classext {
// 线程功能拓展
class MyThread : public std::thread {  // 继承thread
 public:
  // 子类MyThread()继承thread()的构造函数
  MyThread() : std::thread() {}
  // MyThread()初始化构造函数
  template <typename T, typename... Args>
  MyThread(T &&func, Args &&... args)
      : std::thread(std::forward<T>(func), std::forward<Args>(args)...) {}
  void showcmd(const char *str) { system(str); }
};

// 在类内部创建线程
class HelloThread {
 public:
  // start()和hellothread()方法都必须是static方法
  static void hellothread() {
    std::cout << "hello world, I'm a thread!" << std::endl;
  }
  static void start() {
    std::thread thrd(hellothread);
    thrd.join();
  }
};

void testClassext() {
  MyThread t1([]() { std::cout << "hello" << std::endl; });
  t1.showcmd("ls");
  t1.join();

  MyThread t2([](const char *str) { std::cout << "hello" << str << std::endl; },
              " this is MyThread");
  t2.showcmd("pwd");
  t2.join();

  HelloThread::start();
}
}  // namespace classext

namespace otherfunc {
// 线程标识类型为std::thread::id，并可以通过两种方式进行检索:
// 第一种，可以通过调用std::thread对象的成员函数get_id()来直接获取。
// 如果std::thread对象没有与任何执行线程相关联，
// get_id()将返回std::thread::type默认构造值，这个值表示“无线程”。
// 第二种，当前线程中调用std::this_thread::get_id()也可以获得线程标识。
// std::thread::id对象可以自由的拷贝和对比,因为标识符就可以复用。
// 如果两个对象的std::thread::id相等，那它们就是同一个线程，或者都“无线程”。
// 如果不等，那么就代表了两个不同线程，或者一个有线程，另一没有线程。
// C++线程库不会限制你去检查线程标识是否一样，std::thread::id类型对象提供相当丰富的对比操作；
// 比如，提供为不同的值进行排序。这意味着允许程序员将其当做为容器的键值，做排序，或做其他方式的比较。
// 按默认顺序比较不同值的std::thread::id，所以这个行为可预见的：当a<b，b<c时，得a<c，等等。
// 标准库也提供std::hash<std::thread::id>容器，所以std::thread::id也可以作为无序容器的键值。
// std::thread::id可以作为一个线程的通用标识符，当标识符只与语义相关(比如，数组的索引)时，就需要这个方案了。
// 也可以使用输出流(std::cout)来记录一个std::thread::id对象的值。
// 具体的输出结果是严格依赖于具体实现的，C++标准的唯一要求就是要保证ID比较结果相等的线程，必须有相同的输出。

void func1() {
  // sleep_for：线程休眠某个指定的时间片(time span)，该线程才被重新唤醒
  // 不过由于线程调度等原因，实际休眠时间可能比sleep_duration所表示的时间片更长
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

std::mutex iomutex;
void func2(int num) {
  std::this_thread::sleep_for(std::chrono::seconds(1));

  sched_param sch;
  int policy;
  pthread_getschedparam(pthread_self(), &policy, &sch);
  std::lock_guard<std::mutex> lk(iomutex);
  std::cout << "Thread " << num << " is executing at priority "
            << sch.sched_priority << '\n';
}

void little_sleep(std::chrono::microseconds us) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + us;
  do {
    std::this_thread::yield();
  } while (std::chrono::high_resolution_clock::now() < end);
}

void testOtherfunc() {
  // get_id：获取线程ID，返回一个类型为std::thread::id的对象
  // swap：交换两个线程对象所代表的底层句柄
  std::thread t1(func1);
  std::thread t2(func1);
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;  // 139681204881152
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;  // 139681196488448
  std::swap(t1, t2);
  std::cout << "after std::swap(t1, t2):" << std::endl;
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;  // 139681196488448
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;  // 139681204881152
  t1.swap(t2);
  std::cout << "after t1.swap(t2):" << std::endl;
  std::cout << "thread 1 id: " << t1.get_id() << std::endl;  // 139681204881152
  std::cout << "thread 2 id: " << t2.get_id() << std::endl;  // 139681196488448
  t1.join();
  t2.join();

  // native_handle：返回与std::thread具体实现相关的线程句柄
  // std::thread的实现和操作系统相关，在符合Posix标准的平台下(如Unix/Linux)是Pthread库
  std::thread t3(func2, 1), t4(func2, 2);
  sched_param sch;
  int policy;
  pthread_getschedparam(t3.native_handle(), &policy, &sch);  // unix api
  sch.sched_priority = 20;
  if (pthread_setschedparam(t3.native_handle(), SCHED_FIFO, &sch)) {
    std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
  }
  t3.join();
  t4.join();
  // Failed to setschedparam: Operation not permitted
  // Thread 1 is executing at priority 0
  // Thread 2 is executing at priority 0
  // sudo
  // Thread 2 is executing at priority 0
  // Thread 1 is executing at priority 20

  // std::thread::hardware_concurrency()在新版C++标准库中是一个很有用的函数。
  // 这个函数会返回能并发在一个程序中的线程数量。多核系统中，返回值可以是CPU核芯的数量。
  // 返回值也仅仅是一个提示，当系统信息无法获取时，函数也会返回0。
  // 但是，这也无法掩盖这个函数对启动线程数量的帮助。
  // 因为上下文频繁的切换会降低线程的性能，所以你肯定不想启动的线程数多于硬件支持的线程数量。
  // 当std::thread::hardware_concurrency()返回0，自己设置一个。
  unsigned int n1 = std::thread::hardware_concurrency();
  std::cout << n1 << " concurrent threads are supported." << std::endl;  // 8

  // this_thread::yield，当前线程放弃执行，操作系统调度另一线程继续执行。
  auto start = std::chrono::high_resolution_clock::now();
  little_sleep(std::chrono::microseconds(100));
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  std::cout
      << "waited for "
      << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()
      << " microseconds\n";

  // sleep_until:线程休眠至某个指定的时刻(time point)，该线程才被重新唤醒。
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
}  // namespace otherfunc

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      create::testCreate();
      break;
    case 1:
      joinable::testJoinable();
      break;
    case 2:
      parameter::testParameter();
      break;
    case 3:
      functype::testFuncType();
      break;
    case 4:
      classext::testClassext();
      break;
    case 5:
      otherfunc::testOtherfunc();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}

// GCC默认没有加载pthread库，据说在后续的版本中可以不用在编译时添加-pthread选项。
// g++ -std=c++11 thread.cpp -lpthread