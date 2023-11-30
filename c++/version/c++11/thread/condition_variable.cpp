#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

// 当std::condition_variable对象的某个wait函数被调用的时候，
// 它使用std::unique_lock（通过std::mutex）来锁住当前线程，
// 当前线程会阻塞直到其他线程在相同的cv对象上调用notification来唤醒。
// condition_variable条件变量的wait、wait_for、wait_until，
// 会阻塞调用的线程直到使用（notify_one或notify_all）通知恢复为止。
// condition_variable对象，通常使用unique_lock<mutex>来等待，
// 如果需要使用其他lockable类型，可使用std::condition_variable_any。

namespace n1 {
// std::condition_variable提供了两种wait()函数：
// 1.第一种只包含unique_lock对象；
// 2.另外一个还有Predicate对象（等待条件）。

// 这里必须使用unique_lock，因为wait函数的工作原理：
// 当前线程调用wait()后将被阻塞并且函数会解锁互斥量，
// 如果使用lock_guard则不能调用unlock函数，所以只能用unique_lock，
// 直到另外某个线程调用notify_one或者notify_all唤醒当前线程；
// 一旦当前线程获得通知，wait()自动调用lock()，也不能用lock_guard。

// 如果wait没有Pred，第一次调用默认条件不成立，解锁互斥量并阻塞到本行，
// 直到某一个线程调用notify_one或notify_all为止，被唤醒后，
// wait重新尝试获取互斥量，如果得不到，线程会卡在这里，
// 直到获取到互斥量，然后无条件地继续进行后面的操作。

// 如果wait包含Pred，如果第二个参数不满足，解锁互斥量并堵塞到本行，
// 直到某一个线程调用notify_one或notify_all为止，被唤醒后，
// wait重新尝试获取互斥量，如果得不到，线程会卡在这里，
// 直到获取到互斥量，然后继续判断第二个参数，如果表达式为false，
// wait对互斥量解锁，然后休眠，如果为true，则进行后面的操作。
// while (!pred()) {
//   wait(lck);
// }

std::mutex mtx;
std::condition_variable cv;
namespace test1 {
bool ready = false;
void f1(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  while (!ready) {  // 如果标志位不为true, 则等待
    cv.wait(lck);   // 当前线程被阻塞
  }
  // 当全局标志位变为true之后，线程被唤醒, 继续执行打印线程编号：
  std::cout << "thread " << id << '\n';
}
void f2() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;     // 设置全局标志位为true
  cv.notify_all();  // 唤醒所有线程
}
void func() {
  // std::condition_variable的拷贝构造函数被禁用，只提供了默认构造函数：
  // std::condition_variable cv2 = cv1;             // error
  // std::condition_variable cv3 = std::move(cv1);  // error

  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(f1, i);
  }
  f2();
  for (auto& th : threads) {
    th.join();
  }

  // thread 0
  // thread 6
  // thread 4
  // thread 3
  // thread 1
  // thread 9
  // thread 5
  // thread 2
  // thread 7
  // thread 8
}
}  // namespace test1

namespace test2 {
int num = 0;
bool shipment_available() { return num != 0; }
void consume(int n) {  // 消费者线程
  for (int i = 0; i < n; ++i) {
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, shipment_available);
    std::cout << num << std::endl;
    num = 0;
  }
}

void func() {
  std::thread consumer_thread(consume, 10);  // 消费者线程
  // 主线程为生产者线程, 生产10个物品：
  for (int i = 0; i < 10; ++i) {
    while (shipment_available()) std::this_thread::yield();
    std::unique_lock<std::mutex> lck(mtx);
    num = i + 1;
    cv.notify_one();
  }
  consumer_thread.join();
  // 1
  // 2
  // 3
  // 4
  // 5
  // 6
  // 7
  // 8
  // 9
  // 10
}
}  // namespace test2
}  // namespace n1

namespace n2 {
// wait_for()与wait()类似，不过wait_for可以指定一个时间段，
// 在当前线程收到通知或者指定的时间超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了其他线程的通知，wait_for返回，剩下的和wait()类似。
// wait_for的重载版本的最后一个参数pred表示wait_for的预测条件，
// 只有当pred条件为false时调用wait()才会阻塞当前线程，
// 并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞。
// return wait_until(lck,
//                   chrono::steady_clock::now() + rel_time,
//                   std::move(pred));
std::condition_variable cv;
int value;
void f() {
  std::cin >> value;
  cv.notify_one();
}

void func() {
  std::cout << "Please, enter an integer (I'll be printing dots): \n";
  std::thread t(f);
  std::mutex mtx;
  std::unique_lock<std::mutex> lck(mtx);
  while (cv.wait_for(lck, std::chrono::seconds(1)) == std::cv_status::timeout) {
    std::cout << '.';
    std::cout.flush();
  }
  std::cout << "You entered: " << value << '\n';
  t.join();
}
}  // namespace n2

namespace n3 {
// wait_until与wait_for类似，但是wait_until可以指定一个时间点，
// 在当前线程收到通知或者指定的时间点超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了通知，wait_until返回，剩下的和wait_until()类似。
// 另外，wait_until的重载版本的最后一个参数pred表示wait_until的预测条件，
// 只有当pred条件为false时调用wait()才会阻塞当前线程；
// 并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞，
// while (!pred())
//   if (wait_until(lck,abs_time) == cv_status::timeout)
//     return pred();
// return true;

std::condition_variable cv;
std::mutex cv_m;
std::atomic<int> i{0};

void f1(int idx) {
  std::unique_lock<std::mutex> lk(cv_m);
  auto now = std::chrono::system_clock::now();
  if (cv.wait_until(lk, now + idx * std::chrono::milliseconds(100),
                    []() { return i == 1; }))
    std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
  else
    std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
}

void f2() {
  std::this_thread::sleep_for(std::chrono::milliseconds(120));
  std::cerr << "Notifying...\n";
  cv.notify_all();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  i = 1;
  std::cerr << "Notifying again...\n";
  cv.notify_all();
}

void func() {
  std::thread t1(f1, 1), t2(f1, 2), t3(f1, 3), t4(f2);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  // Thread 1 timed out. i == 0
  // Notifying...
  // Thread 2 timed out. i == 0
  // Notifying again...
  // Thread 3 finished waiting. i == 1
}
}  // namespace n3

namespace n4 {
// notify_one()唤醒某个等待线程：
// 1.如果当前没有等待线程，则该函数什么也不做；
// 2.如果同时存在多个等待线程，则唤醒某个线程是不确定的。
// notify_all()唤醒所有的等待线程，如果当前没有等待线程，则该函数什么也不做。

std::mutex mtx;
std::condition_variable cv;
void f(int i) {
  std::unique_lock<std::mutex> lck(mtx);
  cv.wait(lck);
  std::cout << "thread " << i << std::endl;
}

void func() {
  std::thread ts[10];
  for (int i = 0; i < 10; ++i) {
    ts[i] = std::thread(f, i);
  }
  std::cout << "notify one" << std::endl;
  cv.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify one" << std::endl;
  cv.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify one" << std::endl;
  cv.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify all" << std::endl;
  cv.notify_all();
  for (auto& ts : ts) {
    ts.join();
  }
  cv.notify_all();  // do nothing

  // 每次结果不一样：
  // notify one
  // thread 0
  // notify one
  // thread 1
  // notify one
  // thread 2
  // notify all
  // thread 4
  // thread 6
  // thread 5
  // thread 8
  // thread 9
  // thread 3
  // thread 7
}
}  // namespace n4

namespace n5 {
// condition_variable_any与condition_variable类似，
// 只不过condition_variable_any的wait函数可以接受任何lockable参数，
// 而condition_variable只能接受unique_lock<mutex>类型的参数，
// 除此以外，和condition_variable几乎完全一样。

std::mutex m;
namespace test1 {
std::mutex m;
std::condition_variable cv;
void f() {
  std::unique_lock<std::mutex> lck(m);
  cv.wait(lck);
  // cv.wait(m);  // error
}
void func() {
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cv.notify_all();
  t.join();
}
}  // namespace test1

namespace test2 {
std::mutex m;
std::condition_variable_any cv;
void f() {
  std::unique_lock<std::mutex> lck(m);
  cv.wait(lck);
}
void func() {
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cv.notify_all();
  t.join();
}
}  // namespace test2

namespace test3 {
std::mutex m;
std::condition_variable_any cv;
void f() {
  m.lock();
  cv.wait(m);
}
void func() {
  std::thread t(f);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cv.notify_all();
  t.join();
}
}  // namespace test3

namespace test4 {
// std::cv_status枚举类型：
// 1.cv_status::no_timeout，wait_for或者wait_until没有超时；
// 2.cv_status::timeout，wait_for或者wait_until超时。

std::mutex m;
std::condition_variable cv;
bool ready = false;
void f1(int id) {
  std::unique_lock<std::mutex> lck(m);
  while (!ready) {
    cv.wait(lck);
  }
  std::cout << "thread " << id << std::endl << std::flush;
}
void f2() {
  std::unique_lock<std::mutex> lck(m);
  std::cout << "notify_one" << std::endl;
  ready = true;
  cv.notify_one();
  lck.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "notify_all when exit" << std::endl;
  std::notify_all_at_thread_exit(cv, std::move(lck));
}

void func() {
  std::thread threads[10];
  for (int i = 0; i < 10; ++i) {
    threads[i] = std::thread(f1, i);
  }
  std::this_thread::sleep_for(std::chrono::microseconds(1000));
  std::thread(f2).detach();
  for (auto& th : threads) {
    th.join();
  }

  // 每次结果都不一样：
  // notify_one
  // thread 0
  // notify_all when exit
  // thread 2
  // thread 8
  // thread 5
  // thread 9
  // thread 4
  // thread 7
  // thread 1
  // thread 3
  // thread 6
}
}  // namespace test4

void func() {
  test1::func();
  test2::func();
  test3::func();
  test4::func();
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n2::func();
      break;
    case 3:
      n3::func();
      break;
    case 4:
      n4::func();
      break;
    case 5:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}