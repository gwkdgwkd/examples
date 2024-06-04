#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stack>
#include <thread>
#include <vector>

// 保护共享数据结构的最基本的方式，是使用C++标准库提供的互斥量。
// mutex（互斥量）的种类:
// 1.std::mutex，最基本的mutex类，提供了独占所有权的特性，
//   即不支持递归地对std::mutex对象上锁；
// 2.std::recursive_mutex，递归mutex类；
// 3.std::time_mutex，定时mutex类；
// 4.std::recursive_timed_mutex，定时递归mutex类。

namespace n1 {
// std::mutex的成员函数：
// 1.lock()，调用线程将锁住该互斥量，线程调用该函数会发生下面3种情况：
//   a.如果该互斥量当前没有被锁住，则调用线程将该互斥量锁住，
//     直到调用unlock之前，该线程一直拥有该锁；
//   b.如果当前互斥量被其他线程锁住，则当前的调用线程被阻塞住；
//   c.如果当前互斥量被当前调用线程锁住，则会产生死锁deadlock。
// 2.unlock()，解锁，释放对互斥量的所有权；
// 3.try_lock()，尝试锁住互斥量，如果被其他线程占有，
//   当前线程也不会被阻塞，线程调用该函数也会出现下面3种情况：
//   a.如果当前互斥量没有被其他线程占有，则该线程锁住互斥量，
//     直到该线程调用unlock释放互斥量；
//   b.如果当前互斥量被其他线程锁住，则当前调用线程返回false，而并不会被阻塞；
//   c.如果当前互斥量被当前调用线程锁住，则会产生死锁deadlock。

void func1() {
  std::mutex m1;  // 默认构造函数，处于unlocked状态
  // std::mutex m2(m1);  // 不允许拷贝构造
  // std::mutex m3 = std::move(m1);  // 不允许move拷贝
}

volatile int counter(0);  // non-atomic counter
std::mutex mtx;
void attempt_10k_increases() {
  for (int i = 0; i < 10000; ++i) {
    if (mtx.try_lock()) {
      ++counter;
      mtx.unlock();
    }
  }
}
void func2() {
  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(attempt_10k_increases);
  }

  for (auto& th : threads) {
    th.join();
  }
  std::cout << counter << " successful increases of the counter.\n";
  // 每次结果都不一样：
  // 15068 successful increases of the counter.
  // 16166 successful increases of the counter.
}

// 在为一段程序加锁时要格外小心，否则很容易因为这种调用关系而造成死锁。
// 不要存在侥幸心理，觉得这种情况是很少出现的，当代码复杂到一定程度，
// 被多个人维护，用关系错综复杂时，程序中很容易犯这样的错误。
// 庆幸的是，这种原因造成的死锁很容易被排除。
// 但是这并不意味着应该用递归锁去代替非递归锁，递归锁用起来固然简单，
// 但往往会隐藏某些代码问题，比如调用函数和被调用函数以为自己拿到了锁，
// 都在修改同一个对象，这时就很容易出现问题。
// 因此在能使用非递归锁的情况下，应该尽量使用非递归锁，因为死锁相对来说，
// 更容易通过调试发现，如果有问题，应该暴露的越早越好。

// f1函数和f2函数都获取了同一个锁，而f2函数又会调用f1函数，
// 如果锁是个非递归锁，则这个程序会立即死锁：
std::mutex mu;
void f1() {
  mu.lock();
  mu.unlock();
}
void f2() {
  mu.lock();
  f1();
  mu.unlock();
}

void func3() {
  // 死锁了
  std::thread t1(f1);
  std::thread t2(f2);
  t1.join();
  t2.join();
}
}  // namespace n1

namespace n2 {
// std::recursive_mutex与std::mutex一样，
// 也是一种可以被上锁的对象，但是和std::mutex不同的是，
// std::recursive_mutex允许同一个线程对互斥量多次上锁（即递归上锁），
// 来获得对互斥量对象的多层所有权，std::recursive_mutex释放互斥量时，
// lock()次数和unlock()次数必须相同，除此之外，
// std::recursive_mutex的特性和std::mutex大致相同。

namespace test1 {
std::recursive_mutex rm;
std::string shared;
void f1() {
  rm.lock();
  shared = "f1";
  std::cout << "in f1, shared variable is now " << shared << '\n';
  rm.unlock();
}
void f2() {
  rm.lock();
  shared = "f2";
  std::cout << "in f2, shared variable is now " << shared << '\n';
  f1();  // 递归锁的作用
  std::cout << "back in f2, shared variable is " << shared << '\n';
  rm.unlock();
};

void func() {
  std::thread t1(f1);
  std::thread t2(f2);
  t1.join();
  t2.join();

  // in f1, shared variable is now f1
  // in f2, shared variable is now f2
  // in f1, shared variable is now f1
  // back in f2, shared variable is f1
}
}  // namespace test1

namespace test2 {
class RecursiveMutex {  // 实现递归锁
 public:
  RecursiveMutex() { num_of_locks = 0; }
  ~RecursiveMutex() {}
  void lock() {
    if (num_of_locks == 0) {
      my_mutex.lock();
      owner_thread_id = std::this_thread::get_id();
    } else if (std::this_thread::get_id() == owner_thread_id) {
      num_of_locks++;
    }
  }
  void unlock() {
    if (num_of_locks > 0) num_of_locks--;
    if (num_of_locks == 0) my_mutex.unlock();
  }

 private:
  int num_of_locks;
  std::mutex my_mutex;
  std::thread::id owner_thread_id;
};

int g_num = 0;
RecursiveMutex myRecursiveMutex;
void slow_increment(int id) {
  for (int i = 0; i < 3; ++i) {
    myRecursiveMutex.lock();
    ++g_num;
    std::cout << id << " => " << g_num << '\n';
    myRecursiveMutex.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
void func() {
  std::thread t1(slow_increment, 0);
  std::thread t2(slow_increment, 1);
  t1.join();
  t2.join();

  // 0 => 1
  // 1 => 2
  // 0 => 3
  // 1 => 4
  // 0 => 5
  // 1 => 6
}
}  // namespace test2

namespace test3 {
int g_num = 0;
std::recursive_mutex g_num_mutex;
void slow_increment_stl(int id) {
  for (int i = 0; i < 3; ++i) {
    g_num_mutex.lock();
    ++g_num;
    std::cout << id << " => " << g_num << '\n';
    g_num_mutex.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void func() {
  std::thread t3(slow_increment_stl, 0);
  std::thread t4(slow_increment_stl, 1);
  t3.join();
  t4.join();

  // 0 => 1
  // 1 => 2
  // 0 => 3
  // 1 => 4
  // 0 => 5
  // 1 => 6
}
}  // namespace test3
}  // namespace n2

namespace n3 {
// std::time_mutex比std::mutex多了两个成员函数：
// 1.try_lock_for
//   函数接受一个时间范围，在时间范围之内线程如果没有获得锁则被阻塞，
//   如果在此期间其他线程释放了锁，则可以获得锁，如果超时，则返回false。
// 2.try_lock_until
//   函数则接受一个时间点作为参数，在指定时间点未到来之前没有获得锁则被阻塞，
//   如果在此期间其他线程释放了锁，则该线程可以获得锁，如果超时，则返回false。

std::timed_mutex mtx;
namespace test1 {
void f() {
  // waiting to get a lock: each thread prints "-" every 200ms：
  while (!mtx.try_lock_for(std::chrono::milliseconds(200))) {
    std::cout << "-" << std::flush;
  }
  // got a lock! - wait for 1s, then this thread prints "*"
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "*" << std::endl;
  mtx.unlock();
}
void func() {
  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(f);
  }
  for (auto& th : threads) {
    th.join();
  }

  // ------------------------------------*
  // ----------------------------------------*
  // -----------------------------------*
  // ------------------------------*
  // -------------------------*
  // --------------------*
  // ---------------*
  // ----------*
  // -----*
  // *
}
}  // namespace test1

namespace test2 {
void f() {
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm* ptm = std::localtime(&tt);
  std::cout << "Current time: " << std::put_time(ptm, "%X") << std::endl;
  std::cout << "Waiting for the next minute to begin..." << std::endl;
  ++ptm->tm_min;
  ptm->tm_sec = 0;

  bool ret = mtx.try_lock_until(system_clock::from_time_t(mktime(ptm)));
  std::cout << std::put_time(ptm, "%X") << " reached! " << std::boolalpha << ret
            << std::endl;
  mtx.unlock();
}

void func() {
  mtx.lock();
  std::thread t1(f);
  t1.join();
  // Current time: 12:28:52
  // Waiting for the next minute to begin...
  // 12:29:00 reached!false

  std::cout << std::flush;
  std::thread t2(f);
  t2.join();
  // Current time: 12:29:00
  // Waiting for the next minute to begin...
  // 12:30:00 reached!true
}
}  // namespace test2
}  // namespace n3

namespace n4 {
// 和std:recursive_mutex与std::mutex的关系一样，
// std::recursive_timed_mutex的特性也可以从std::timed_mutex推导出来。
// 递归定时互斥锁将recursive_timed和timed_mutex的功能结合到一个类中：
// 它既支持通过单个线程获取多个锁定级别又支持定时的try_lock请求。
// 成员函数与timed_mutex相同。
}  // namespace n4

namespace n5 {
std::mutex mtx;
namespace test1 {
// C++中通过实例化std::mutex创建互斥量实例，
// 通过成员函数lock()对互斥量上锁，unlock()进行解锁。
// 不推荐直接去调用成员函数，那意味着必须在每出口都要unlock()，也包括异常。
// C++标准库为互斥量提供了一个RAII语法的模板类std::lock_guard，
// 在构造时就能提供已锁的互斥量，并在析构的时候进行解锁，从而保证了正确解锁。
// 互斥锁保证了线程间的同步，但是却将并行操作变成了串行操作，
// 这对性能有很大的影响，所以要尽可能的减小锁定的区域，也就是使用细粒度锁。
// 这一点lock_guard做的不好，不够灵活，只能保证在析构的时候执行解锁操作，
// lock_guard本身并没有提供加锁和解锁的接口，但是有些时候会有这种需求。
volatile int counter(0);
void attempt_10k_increases() {
  for (int i = 0; i < 100; ++i) {
    try {
      std::lock_guard<std::mutex> lck(mtx);  // RAII方式使用mutex
      counter++;
    } catch (std::logic_error&) {
      std::cout << "[exception caught]\n";
    }
  }
}
void func() {
  std::thread threads[6];
  for (int i = 0; i < 6; ++i) {
    threads[i] = std::thread(attempt_10k_increases);
  }
  for (auto& th : threads) {
    th.join();
  }
  std::cout << counter << " successful increases of the counter.\n";

  // 600 successful increases of the counter.
}
}  // namespace test1

namespace test2 {
// lock_guard最大的特点就是安全易于使用，在异常抛出时也可以正确地解锁：
void print_even(int x) {
  if (x % 2 == 0)
    std::cout << x << " is even\n";
  else
    throw(std::logic_error("not even"));
}
void print_thread_id(int id) {
  try {
    std::lock_guard<std::mutex> lck(mtx);
    print_even(id);
  } catch (std::logic_error&) {
    std::cout << "[exception caught]\n";
  }
}
void func() {
  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(print_thread_id, i + 1);
  }
  for (auto& th : threads) {
    th.join();
  }

  // [exception caught]
  // 2 is even
  // 6 is even
  // 4 is even
  // [exception caught]
  // [exception caught]
  // [exception caught]
  // [exception caught]
  // 10 is even
  // 8 is even
}
}  // namespace test2

namespace test3 {
// unique_lock是mutex的RAII相关实现，提供了更好的上锁和解锁控制。
// unique_lock和lock_guard用法很相似，但是实际上unique_lock更加灵活，
// 可以在任意的时候加锁或者解锁，因此其资源消耗也更大，效率比lock_guard低。
void print_block(int n, char c) {
  std::unique_lock<std::mutex> lck(mtx);
  for (int i = 0; i < n; ++i) {
    std::cout << c;
  }
  std::cout << '\n';
}
void func() {
  std::thread t1(print_block, 50, '*');
  std::thread t2(print_block, 50, '$');
  t1.join();
  t2.join();
}
}  // namespace test3

namespace test4 {
// 一个函数内部有两段代码需要保护，使用lock_guard要两个对象来管理同一个锁。
// 其实也可以只用一个，但是锁的粒度太大，效率低，使用unique_lock更好一点，
// 它提供了lock()和unlock()接口，能记录现在上锁还是没上锁状态，
// 析构时根据当前状态决定是否解锁，lock_guard就一定会解锁。
// 在无需加锁的操作时unlock，需要保护的时候lock，
// 这样就无需重复的实例化lock_guard对象，还能减少锁的区域。
// 同样，可以使用std::defer_lock设置初始化的时候不进行默认的上锁操作。
void func() {
  // 虽然创建了unique_lock，但此时是unlock状态：
  std::unique_lock<std::mutex> unique(mtx, std::defer_lock);
  // do something 1
  unique.lock();
  // do something protected
  unique.unlock();  // 临时解锁
  // do something 2
  unique.lock();  // 继续上锁
  // do something protected

  // 结束时析构unique会解锁
}
}  // namespace test4
namespace test5 {
// lock_guard不能移动，unique_lock可以，它们都不可以复制。
void func() {
  // unique_lock可以移动，不能复制：
  std::mutex mu1;
  std::unique_lock<std::mutex> unique1(mu1);
  // std::unique_lock<std::mutex> unique2 = unique1;          // error
  std::unique_lock<std::mutex> unique3 = std::move(unique1);  // ok

  // lock_guard不能移动，不能复制：
  std::mutex mu2;
  std::lock_guard<std::mutex> guard1(mu2);
  // std::lock_guard<std::mutex> guard2 = guard1;             // error
  // std::lock_guard<std::mutex> guard3 = std::move(guard1);  // error
}
}  // namespace test5
}  // namespace n5

namespace n6 {
namespace test1 {
// std::adopt_lock_t是一个空类，作为adopt_lock的类型。
std::mutex mtx;
void f(int id) {
  // 先对mtx进行上锁，再用mtx对象构造一个lock_guard对象：
  mtx.lock();
  // 将adopt_lock作为参数传递，表明当前线程已经获得了锁，
  // 此后mtx对象的解锁操作交由lock_guard对象lck来管理：
  std::lock_guard<std::mutex> lck(mtx, std::adopt_lock);
  // std::unique_lock<std::mutex> lck(mtx, std::adopt_lock);  // 也可以
  std::cout << "thread #" << id << '\n';
  // 在lck的生命周期结束之后，mtx对象会自动解锁。
}
void func() {
  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(f, i + 1);
  }
  for (auto& th : threads) {
    th.join();
  }

  // 每次结果都不一样：
  // thread #1
  // thread #4
  // thread #5
  // thread #6
  // thread #7
  // thread #9
  // thread #3
  // thread #8
  // thread #10
  // thread #2
}
}  // namespace test1

namespace test2 {
std::mutex mtx1, mtx2;
void f1() {
  std::lock(mtx1, mtx2);
  std::unique_lock<std::mutex> lck1(mtx1, std::adopt_lock);
  std::unique_lock<std::mutex> lck2(mtx2, std::adopt_lock);
  std::cout << "f1" << std::endl;
}
// std::defer_lock_t是一个空类，作为defer_lock的类型。
// 对unique_lock的构造函数传递defer_lock，不会在构建阶段自动上锁：
void f2() {
  std::unique_lock<std::mutex> lck1, lck2;
  lck1 = std::unique_lock<std::mutex>(mtx1, std::defer_lock);
  lck2 = std::unique_lock<std::mutex>(mtx2, std::defer_lock);
  std::lock(lck1, lck2);
  std::cout << "f2" << std::endl;
}
void func() {
  std::thread t1(f1);
  std::thread t2(f2);
  t1.join();
  t2.join();
}
}  // namespace test2

namespace test3 {
// std::try_to_lock_t是一个空类，作为try_to_lock的类型。
std::mutex mtx;
void f() {
  std::unique_lock<std::mutex> lck(mtx, std::try_to_lock);
  // print '*' if successfully locked, 'x' otherwise：
  if (lck.owns_lock()) {
    std::cout << '*' << std::flush;
  } else {
    std::cout << 'x' << std::flush;
  }
}
void func() {
  std::vector<std::thread> threads;
  for (int i = 0; i < 50; ++i) {
    threads.emplace_back(f);
  }
  for (auto& x : threads) {
    x.join();
  }
  // *x*x*x*****x*******x******x*xx********************
}
}  // namespace test3

namespace test4 {
// 为了保证在多线程环境下，某个代码段仅仅被调用一次，
// 比如，初始化某个对象，而这个对象只能初始化一次，
// 就可以用std::call_once来保证代码段在多线程环境下只被调用一次。
// 使用std::call_once时候，需要一个once_flag作为call_once的参数。
// 实际上once_flag相当于锁，使用它的线程都会等待，仅仅有一个线程会运行。
// 假设该线程抛出异常，那么从等待中的线程中选择一个。
// once_flag的生命期必需要比使用它的线程的要长，通常定义成全局变量。
std::once_flag flag;
void f() {
  std::cout << "Call Before!" << std::endl;
  std::call_once(flag, []() { std::cout << "Calling...!" << std::endl; });
  std::cout << "Call After!" << std::endl;
}
void func() {
  std::thread t1(f);
  std::thread t2(f);
  std::thread t3(f);
  t1.join();
  t2.join();
  t3.join();
  // Call Before!
  // Calling...!
  // Call After!
  // Call Before!
  // Call After!
  // Call Before!
  // Call After!
}
}  // namespace test4

namespace test5 {
class Singleton {
 public:
  static Singleton* getInstance() {
    std::cout << "getInstance" << std::endl;
    call_once(onceFlag, createInstance);
    return instance;
  }

  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

 private:
  Singleton() = default;

  static void createInstance() {
    std::cout << "createInstance" << std::endl;
    instance = new Singleton();
  }

  static Singleton* instance;
  static std::once_flag onceFlag;
};
Singleton* Singleton::instance = nullptr;
std::once_flag Singleton::onceFlag;
void f() { Singleton* p = Singleton::getInstance(); }
void func() {
  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  // getInstance
  // createInstance
  // getInstance
}
}  // namespace test5

namespace test6 {
// 一个给定操作需要两个或两个以上的互斥量时，另一个潜在问题是死锁。
// 与条件竞争完全相反，不同的两个线程会互相等待，从而什么都没做。
// 避免死锁的一般建议，就是让两个互斥量总以相同的顺序上锁：
// 总在互斥量B之前锁住互斥量A，就永远不会死锁。
// 某些情况下是可以这样用，因为不同的互斥量用于不同的地方。
// 不过，事情没那么简单，当有多个互斥量保护同一个类的独立实例时，
// 一个操作对同一个类的两个不同实例进行数据的交换操作，
// 为了保证数据交换操作的正确性，就要避免数据被并发修改，
// 并确保每个实例上的互斥量都能锁住自己要保护的区域。
// 不过，选择一个固定的顺序，可能会适得其反：
// 在参数交换之后，两个线程试图在相同的两个实例进行数据交换时，又死锁了！
// C++可以解决这个问题，std::lock可以一次锁住多个的锁，并且没有死锁风险。
class some_big_object {};
void swap(some_big_object& lhs, some_big_object& rhs);
class X {
 private:
  some_big_object some_detail;
  std::mutex m;

 public:
  X(some_big_object const& sd) : some_detail(sd) {}
  friend void swap(X& lhs, X& rhs) {
    if (&lhs == &rhs) return;
    std::lock(lhs.m, rhs.m);
    std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);
    swap(lhs.some_detail, rhs.some_detail);
  }
};
// 虽然std::lock和std::scoped_lock<>可以在获取两个以上的锁时避免死锁，
// 但它没办法只获取其中一个锁。
// 这时，依赖于开发者的经验，来确保你的程序不会死锁。
// 这并不简单：死锁是多线程编程中一个令人相当头痛的问题，
// 并且死锁经常是不可预见的，因为在大多数时间里，所有工作都能很好的完成。
// 不过，一些相对简单的规则能帮助写出无死锁的代码。
// 1.避免嵌套锁
//   一个线程已获得一个锁时，再别去获取第二个，因为每个线程只持有一个锁，
//   锁上就不会产生死锁，当需要获取多个锁，使用std::lock，避免产生死锁；
// 2.避免在持有锁时调用用户提供的代码
//   代码是用户提供的，没有办法确定用户要做什么，可能做任何事情，
//   包括获取锁，这样就会违反第一个指导意见，并造成死锁；
// 3.使用固定顺序获取锁
//   当硬性条件要求获取两个或两个以上的锁，并且不能使用std::lock，
//   那么最好在每个线程上，用固定的顺序获取锁；
// 4.使用锁的层次结构
//   这个建议需要对应用进行分层，并且识别在给定层上所有可上锁的互斥量，
//   当代码试图对一个互斥量上锁，在该层锁已被低层持有时，上锁是不允许的。
}  // namespace test6
}  // namespace n6

namespace n7 {
// 当一个成员函数返回的是保护数据的指针或引用时，会破坏数据封装性。
// 具有访问能力的指针或引用可以访问并可能修改被保护的数据，而不会被互斥锁限制。
// 这就需要对接口有相当谨慎的设计，要确保互斥量能锁住数据的访问，并且不留后门。
// 切勿将受保护数据的指针或引用传递到互斥锁作用域之外，无论是函数返回值，
// 还是存储在外部可见内存，亦或是以参数的形式传递到用户提供的函数中去，
// 这是常犯的错误，绝不仅仅是一个潜在的陷阱而已。
class some_data {
  int a = 1;
  std::string b = "hello";

 public:
  void do_something() {
    a = 5;
    b = "llll";
  }
  void display() { std::cout << a << " " << b << std::endl; }
};
class data_wrapper {
 private:
  some_data data;
  std::mutex m;

 public:
  template <typename Function>
  void process_data(Function func) {
    std::lock_guard<std::mutex> l(m);
    func(data);  // 传递保护数据给用户函数
  }
  void display() {
    std::lock_guard<std::mutex> l(m);
    data.display();
  }
};
some_data* unprotected;
void malicious_function(some_data& protected_data) {
  unprotected = &protected_data;
}

void func() {
  data_wrapper x;
  x.display();                         // 1 hello
  x.process_data(malicious_function);  // 传递一个恶意函数
  // 在无保护的情况下访问保护数据：
  unprotected->do_something();
  unprotected->display();  // 5 llll
  x.display();             // 5 llll
}
}  // namespace n7

namespace n8 {
// 线程安全堆栈
struct empty_stack : std::exception {
  const char* what() const throw() { return "empty stack!"; };
};
template <typename T>
class threadsafe_stack {
 private:
  std::stack<T> data;
  mutable std::mutex m;

 public:
  threadsafe_stack() : data(std::stack<T>()) {}
  threadsafe_stack(const threadsafe_stack& other) {
    std::lock_guard<std::mutex> lock(other.m);
    // 2.在构造函数体中的执行拷贝：
    data = other.data;
  }
  threadsafe_stack& operator=(const threadsafe_stack&) = delete;
  void push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    data.push(new_value);
  }
  std::shared_ptr<T> pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();  // 在调用pop前，检查栈是否为空
    std::shared_ptr<T> const res(
        std::make_shared<T>(data.top()));  // 在修改堆栈前，分配出返回值
    data.pop();
    return res;
  }
  void pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();
    value = data.top();
    data.pop();
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }
};
}  // namespace n8

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 18]" << std::endl;
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
      n1::func3();
      break;
    case 3:
      n2::test1::func();
      break;
    case 4:
      n2::test2::func();
      break;
    case 5:
      n2::test3::func();
      break;
    case 6:
      n3::test1::func();
      break;
    case 7:
      n3::test2::func();
      break;
    case 8:
      n5::test1::func();
      break;
    case 9:
      n5::test2::func();
      break;
    case 10:
      n5::test3::func();
      break;
    case 11:
      n5::test4::func();
      break;
    case 12:
      n5::test5::func();
      break;
    case 13:
      n6::test1::func();
      break;
    case 14:
      n6::test2::func();
      break;
    case 15:
      n6::test3::func();
      break;
    case 16:
      n6::test4::func();
      break;
    case 17:
      n6::test5::func();
      break;
    case 18:
      n7::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}