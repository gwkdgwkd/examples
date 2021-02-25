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
// std::mutex 的成员函数:
// 构造函数，std::mutex不允许拷贝构造，也不允许move拷贝，最初产生的mutex对象是处于unlocked状态的。
// lock()，调用线程将锁住该互斥量。线程调用该函数会发生下面3种情况：
//   (1). 如果该互斥量当前没有被锁住，则调用线程将该互斥量锁住，直到调用 unlock之前，该线程一直拥有该锁。
//   (2). 如果当前互斥量被其他线程锁住，则当前的调用线程被阻塞住。
//   (3). 如果当前互斥量被当前调用线程锁住，则会产生死锁(deadlock)。
// unlock()， 解锁，释放对互斥量的所有权。
// try_lock()，尝试锁住互斥量，如果互斥量被其他线程占有，则当前线程也不会被阻塞。线程调用该函数也会出现下面3种情况:
//   (1). 如果当前互斥量没有被其他线程占有，则该线程锁住互斥量，直到该线程调用 unlock 释放互斥量。
//   (2). 如果当前互斥量被其他线程锁住，则当前调用线程返回 false，而并不会被阻塞掉。
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

  std::thread threads2[2];
  for (int i = 0; i < 2; ++i) threads2[i] = std::thread(fireworks);
  for (auto& th : threads2) th.join();
  // 139788017358592 -
  // 139788008965888 *
  // 139788017358592 *

  return 0;
}