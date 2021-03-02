#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

// Mutex又称互斥量，C++11中与Mutex相关的类（包括锁类型）和函数都声明在<mutex>头文件中，所以如果你需要使用std::mutex，就必须包含<mutex>头文件。
// Mutex系列类(四种):
//     std::mutex，最基本的Mutex类。
//     std::recursive_mutex，递归Mutex类。
//     std::time_mutex，定时Mutex类。
//     std::recursive_timed_mutex，定时递归Mutex类。
// Lock类（两种）:
//     std::lock_guard，与Mutex RAII相关，方便线程对互斥量上锁。
//     std::unique_lock，与Mutex RAII相关，方便线程对互斥量上锁，但提供了更好的上锁和解锁控制。
// 其他类型：
//     std::once_flag
//     std::adopt_lock_t
//     std::defer_lock_t
//     std::try_to_lock_t
// 函数：
//     std::try_lock，尝试同时对多个互斥量上锁。
//     std::lock，可以同时对多个互斥量上锁。
//     std::call_once，如果多个线程需要同时调用某个函数，call_once 可以保证多个线程对该函数只调用一次。

// std::mutex是C++11中最基本的互斥量，std::mutex对象提供了独占所有权的特性——即不支持递归地对std::mutex对象上锁，而std::recursive_lock则可以递归地对互斥量对象上锁。
// std::mutex的成员函数:
// 构造函数，std::mutex不允许拷贝构造，也不允许move拷贝，最初产生的mutex对象是处于unlocked状态的。
// lock()，调用线程将锁住该互斥量。线程调用该函数会发生下面3种情况：
//   (1). 如果该互斥量当前没有被锁住，则调用线程将该互斥量锁住，直到调用unlock之前，该线程一直拥有该锁。
//   (2). 如果当前互斥量被其他线程锁住，则当前的调用线程被阻塞住。
//   (3). 如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。
// unlock()，解锁，释放对互斥量的所有权。
// try_lock()，尝试锁住互斥量，如果互斥量被其他线程占有，则当前线程也不会被阻塞。线程调用该函数也会出现下面3种情况:
//   (1). 如果当前互斥量没有被其他线程占有，则该线程锁住互斥量，直到该线程调用unlock释放互斥量。
//   (2). 如果当前互斥量被其他线程锁住，则当前调用线程返回false，而并不会被阻塞掉。
//   (3). 如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。
volatile int counter(0);  // non-atomic counter
std::mutex mtx;           // locks access to counter
void attempt_10k_increases() {
  for (int i = 0; i < 10000; ++i) {
    if (mtx.try_lock()) {  // only increase if currently not locked:
      ++counter;
      mtx.unlock();
    }
  }
}

// std::recursive_mutex与std::mutex一样，也是一种可以被上锁的对象，但是和std::mutex不同的是，std::recursive_mutex允许同一个线程对互斥量多次上锁（即递归上锁），
// 来获得对互斥量对象的多层所有权，std::recursive_mutex释放互斥量时需要调用与该锁层次深度相同次数的unlock()，可理解为lock()次数和unlock()次数相同，除此之外，
// std::recursive_mutex的特性和std::mutex大致相同。
class X {
  std::recursive_mutex m;
  std::string shared;

 public:
  void fun1() {
    std::lock_guard<std::recursive_mutex> lk(m);
    shared = "fun1";
    std::cout << "in fun1, shared variable is now " << shared << '\n';
  }
  void fun2() {
    std::lock_guard<std::recursive_mutex> lk(m);
    shared = "fun2";
    std::cout << "in fun2, shared variable is now " << shared << '\n';
    fun1();  // recursive lock becomes useful here
    std::cout << "back in fun2, shared variable is " << shared << '\n';
  };
};
std::mutex mu;
void foo() {
  mu.lock();
  mu.unlock();
}
void bar() {
  mu.lock();
  foo();
  mu.unlock();
}
// foo函数和bar函数都获取了同一个锁，而bar函数又会调用foo函数。如果MutexLock锁是个非递归锁，则这个程序会立即死锁。
// 在为一段程序加锁时要格外小心，否则很容易因为这种调用关系而造成死锁。不要存在侥幸心理，觉得这种情况是很少出现的。当代码复杂到一定程度，被多个人维护，调用关系错综复杂时，
// 程序中很容易犯这样的错误。庆幸的是，这种原因造成的死锁很容易被排除。
// 但是这并不意味着应该用递归锁去代替非递归锁。递归锁用起来固然简单，但往往会隐藏某些代码问题。比如调用函数和被调用函数以为自己拿到了锁，都在修改同一个对象，这时就很容易出
// 现问题。因此在能使用非递归锁的情况下，应该尽量使用非递归锁，因为死锁相对来说，更容易通过调试发现。程序设计如果有问题，应该暴露的越早越好。
int g_num = 0;  // 为g_num_mutex所保护
std::recursive_mutex g_num_mutex;
class RecursiveMutex {
 public:
  RecursiveMutex() { num_of_locks = 0; }
  ~RecursiveMutex() {}
  void lock() {
    if (num_of_locks == 0) {
      my_mutex.lock();
      owner_thread_id = this_thread::get_id();
    } else if (this_thread::get_id() == owner_thread_id)
      num_of_locks++;
  }
  void unlock() {
    if (num_of_locks > 0) num_of_locks--;
    if (num_of_locks == 0) my_mutex.unlock();
  }

 private:
  int num_of_locks;
  std::mutex my_mutex;
  thread::id owner_thread_id;
};
RecursiveMutex myRecursiveMutex;
void slow_increment_stl(int id) {
  for (int i = 0; i < 3; ++i) {
    g_num_mutex.lock();
    ++g_num;
    std::cout << id << " => " << g_num << '\n';
    g_num_mutex.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}
void slow_increment(int id) {
  for (int i = 0; i < 3; ++i) {
    myRecursiveMutex.lock();
    ++g_num;
    std::cout << id << " => " << g_num << '\n';
    myRecursiveMutex.unlock();
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
}

// std::time_mutex比std::mutex多了两个成员函数，try_lock_for()，try_lock_until()。
// try_lock_for函数接受一个时间范围，表示在这一段时间范围之内线程如果没有获得锁则被阻塞住（与std::mutex的try_lock()不同，try_lock如果被调用时没有获得锁则直接返回
// false），如果在此期间其他线程释放了锁，则该线程可以获得对互斥量的锁，如果超时（即在指定时间内还是没有获得锁），则返回false。
// try_lock_until函数则接受一个时间点作为参数，在指定时间点未到来之前线程如果没有获得锁则被阻塞住，如果在此期间其他线程释放了锁，则该线程可以获得对互斥量的锁，如果超时（
// 即在指定时间内还是没有获得锁），则返回false。
std::timed_mutex time_mtx;
void fireworks() {
  // waiting to get a lock: each thread prints "-" every 200ms:
  if (!time_mtx.try_lock_for(std::chrono::milliseconds(200))) {
    std::cout << this_thread::get_id() << " -" << endl;
  }
  // got a lock! - wait for 1s, then this thread prints "*"
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << this_thread::get_id() << " *" << endl;
  mtx.unlock();
}
std::timed_mutex cinderella;
std::chrono::time_point<std::chrono::system_clock> midnight() {
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t(system_clock::now());
  struct std::tm* ptm = std::localtime(&tt);
  ++ptm->tm_mday;
  ptm->tm_hour = 0;
  ptm->tm_min = 0;
  ptm->tm_sec = 0;
  return system_clock::from_time_t(mktime(ptm));
}
void carriage() {
  if (cinderella.try_lock_until(midnight())) {
    std::cout << "ride back home on carriage\n";
    cinderella.unlock();
  } else
    std::cout << "carriage reverts to pumpkin\n";
}
void ball() {
  cinderella.lock();
  std::cout << "at the ball...\n";
  cinderella.unlock();
}

// 和std:recursive_mutex与std::mutex的关系一样，std::recursive_timed_mutex的特性也可以从std::timed_mutex推导出来
// 递归定时互斥锁将recursive_timed和timed_mutex的功能结合到一个类中：它既支持通过单个线程获取多个锁定级别又支持定时的try_lock请求。成员函数与timed_mutex相同。

// mutex的RAII相关实现，方便线程的互斥量的使用。
// lock_guard在构造函数中上锁（等待），然后在析构函数中释放锁。
volatile int counter1(0);  // non-atomic counter
std::mutex mtx1;           // locks access to counter
void attempt_10k_increases1() {
  for (int i = 0; i < 100; ++i) {
    try {
      std::lock_guard<std::mutex> lck(mtx1);  // RAII方式使用mutex
      counter1++;
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    } catch (std::logic_error&) {
      std::cout << "[exception caught]\n";
    }
  }
}
// 互斥锁保证了线程间的同步，但是却将并行操作变成了串行操作，这对性能有很大的影响，所以我们要尽可能的减小锁定的区域，也就是使用细粒度锁。
// 这一点lock_guard做的不好，不够灵活，lock_guard只能保证在析构的时候执行解锁操作，lock_guard本身并没有提供加锁和解锁的接口，但是有些时候会有这种需求。

// unique_lock与RAII相关，方便线程对互斥量上锁，但提供了更好的上锁和解锁控制。
std::mutex mtx2;  // mutex for critical section
void print_block(int n, char c) {
  std::unique_lock<std::mutex> lck(mtx2);
  for (int i = 0; i < n; ++i) {
    std::cout << c;
  }
  std::cout << '\n';
}
// 一个函数内部有两段代码需要进行保护，这个时候使用lock_guard就需要创建两个局部对象来管理同一个互斥锁（其实也可以只创建一个，但是锁的力度太大，效率不行），修
// 改方法是使用unique_lock。它提供了lock()和unlock()接口，能记录现在处于上锁还是没上锁状态，在析构的时候，会根据当前状态来决定是否要进行解锁（lock_guard
// 就一定会解锁）
// 在无需加锁的操作时，可以先临时释放锁，然后需要继续保护的时候，可以继续上锁，这样就无需重复的实例化lock_guard对象，还能减少锁的区域。同样，可以使用
// std::defer_lock设置初始化的时候不进行默认的上锁操作：
// void shared_print(string msg, int id) {
//   std::unique_lock<std::mutex> guard(_mu, std::defer_lock);
//   // do something 1
//   guard.lock();
//   // do something protected
//   guard.unlock();  // 临时解锁
//   // do something 2
//   guard.lock();  // 继续上锁
//   // do something 3
//   f << msg << id << endl;
//   cout << msg << id << endl;
//   // 结束时析构guard会临时解锁
// }
// unique_lock和lock_guard用法很相似，但是实际上unique_lock更加灵活，可以在任意的时候加锁或者解锁，因此其资源消耗也更大，通常是在有需要的时候（比
// 如和条件变量配合使用）才会使用，否则用lock_guard。
// lock_guard更加灵活！然后这也是有代价的，因为它内部需要维护锁的状态，所以效率要比lock_guard低一点，在lock_guard能解决问题的时候，就是用
// lock_guard，反之，使用unique_lock。
// unique_lock和lock_guard都不能复制，lock_guard不能移动，但是unique_lock可以！
std::mutex _mu;
// unique_lock 可以移动，不能复制
std::unique_lock<std::mutex> guard11(_mu);
// std::unique_lock<std::mutex> guard2 = guard11;             // error
std::unique_lock<std::mutex> guard3 = std::move(guard11);  // ok
// lock_guard 不能移动，不能复制
std::lock_guard<std::mutex> guard44(_mu);
// std::lock_guard<std::mutex> guard5 = guard44;             // error
// std::lock_guard<std::mutex> guard6 = std::move(guard44);  // error

int main() {
  std::thread threads1[10];
  for (int i = 0; i < 10; ++i) threads1[i] = std::thread(attempt_10k_increases);
  for (auto& th : threads1) th.join();
  std::cout << counter << " successful increases of the counter.\n";
  // 每次结果都一样：
  // 15068 successful increases of the counter.
  // 16166 successful increases of the counter.

  X x;
  std::thread t1(&X::fun1, &x);
  std::thread t2(&X::fun2, &x);
  t1.join();
  t2.join();
  // in fun1, shared variable is now fun1
  // in fun2, shared variable is now fun2
  // in fun1, shared variable is now fun1
  // back in fun2, shared variable is fun1

  // bar();  // 死锁

  cout << "使用stl中的recursive_mutex:" << endl;
  std::thread tt1(slow_increment_stl, 0);
  std::thread tt2(slow_increment_stl, 1);
  tt1.join();
  tt2.join();
  cout << "使用自写类RecursiveMutex:" << endl;
  std::thread tt3(slow_increment, 0);
  std::thread tt4(slow_increment, 1);
  tt3.join();
  tt4.join();
  // 使用stl中的recursive_mutex
  // 0 => 1
  // 1 => 2
  // 0 => 3
  // 1 => 4
  // 0 => 5
  // 1 => 6
  // 使用自写类RecursiveMutex
  // 0 => 7
  // 1 => 8
  // 0 => 9
  // 1 => 10
  // 0 => 11
  // 1 => 12

  std::thread threads2[2];
  for (int i = 0; i < 2; ++i) threads2[i] = std::thread(fireworks);
  for (auto& th : threads2) th.join();
  // 139788017358592 -
  // 139788008965888 *
  // 139788017358592 *

  std::thread th1(ball);
  std::thread th2(carriage);
  th1.join();
  th2.join();
  // at the ball...
  // ride back home on carriage

  std::thread threads11[6];
  for (int i = 0; i < 6; ++i) {
    threads11[i] = std::thread(attempt_10k_increases1);
  }
  for (auto& th : threads11) {
    th.join();
  }
  std::cout << counter1 << " successful increases of the counter.\n";
  // 600 successful increases of the counter.

  std::thread th11(print_block, 50, '*');
  std::thread th22(print_block, 50, '$');
  th11.join();
  th22.join();

  return 0;
}