#include <atomic>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

// 当std::condition_variable对象的某个wait函数被调用的时候，它使用std::unique_lock(通过 std::mutex)来锁住当前线程。
// 当前线程会一直被阻塞，直到另外一个线程在相同的std::condition_variable对象上调用了notification函数来唤醒当前线程。
// condition_variable条件变量可以阻塞（wait、wait_for、wait_until）调用的线程直到使用（notify_one或notify_all）通知恢复为止。
// std::condition_variable对象通常使用std::unique_lock<std::mutex>来等待，
// 如果需要使用另外的lockable类型，可以使用std::condition_variable_any类。

namespace wait {
// std::condition_variable提供了两种wait()函数。
// 第一种只包含unique_lock对象，另外一个还有Predicate对象（等待条件）。

// 这里必须使用unique_lock，因为wait函数的工作原理：
// 当前线程调用wait()后将被阻塞并且函数会解锁互斥量，如果使用lock_guard则不能调用unlock函数，
// 所以这里只能使用unique_lock对象，直到另外某个线程调用notify_one或者notify_all唤醒当前线程；
// 一旦当前线程获得通知(notify)，wait()函数也是自动调用lock()，同理不能使用lock_guard对象。

// 如果wait没有第二个参数，第一次调用默认条件不成立，直接解锁互斥量并阻塞到本行，
// 直到某一个线程调用notify_one或notify_all为止，被唤醒后，wait重新尝试获取互斥量，
// 如果得不到，线程会卡在这里，直到获取到互斥量，然后无条件地继续进行后面的操作。
// 如果wait包含第二个参数，如果第二个参数不满足，那么wait将解锁互斥量并堵塞到本行，
// 直到某一个线程调用notify_one或notify_all为止，被唤醒后，wait重新尝试获取互斥量，
// 如果得不到，线程会卡在这里，直到获取到互斥量，然后继续判断第二个参数，
// 如果表达式为false，wait对互斥量解锁，然后休眠，如果为true，则进行后面的操作。
// 当前线程调用wait()后将被阻塞(此时当前线程应该获得了锁（mutex）lck)，直到另外某个线程调用notify_*唤醒了当前线程。
// 在线程被阻塞时，该函数会自动调用lck.unlock()释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。
// 另外，一旦当前线程获得通知(notified，通常是另外某个线程调用notify_*唤醒了当前线程)，
// wait()函数也是自动调用lck.lock()，使得lck的状态和wait函数被调用时相同。
// 在第二种情况下（即设置了Predicate），只有当pred条件为false时调用wait()才会阻塞当前线程，
// 并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞。类似以下代码：
// while (!pred()) wait(lck);
std::mutex mtx;
std::condition_variable cv;
bool ready = false;
void do_print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx);
  while (!ready)   // 如果标志位不为true, 则等待...
    cv.wait(lck);  // 当前线程被阻塞, 当全局标志位变为true之后,
  // 线程被唤醒, 继续往下执行打印线程编号id.
  std::cout << "thread " << id << '\n';
}
void go() {
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;     // 设置全局标志位为true.
  cv.notify_all();  // 唤醒所有线程.
}

std::mutex mtx1;
std::condition_variable cv1;
int cargo = 0;
bool shipment_available() { return cargo != 0; }
void consume(int n) {  // 消费者线程
  for (int i = 0; i < n; ++i) {
    std::unique_lock<std::mutex> lck(mtx1);
    cv1.wait(lck, shipment_available);
    std::cout << cargo << std::endl;
    cargo = 0;
  }
}

void testWait() {
  // std::condition_variable的拷贝构造函数被禁用，只提供了默认构造函数
  // std::condition_variable cv2 = cv1;  // error
  // std::condition_variable cv3 = std::move(cv1);  // error

  std::thread threads[10];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) threads[i] = std::thread(do_print_id, i);
  std::cout << "10 threads ready to race...\n";
  go();  // go!
  for (auto& th : threads) th.join();
  // 10 threads ready to race...
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

  std::thread consumer_thread(consume, 10);  // 消费者线程
  // 主线程为生产者线程, 生产10个物品.
  for (int i = 0; i < 10; ++i) {
    while (shipment_available()) std::this_thread::yield();
    std::unique_lock<std::mutex> lck(mtx1);
    cargo = i + 1;
    cv1.notify_one();
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
}  // namespace wait

namespace waitfor {
// wait_for()与wait()类似，不过wait_for可以指定一个时间段，
// 在当前线程收到通知或者指定的时间rel_time超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了其他线程的通知，wait_for返回，剩下的处理步骤和wait()类似。
// wait_for的重载版本的最后一个参数pred表示wait_for的预测条件，只有当pred条件为false时调用wait()才会阻塞当前线程，
// 并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞。因此相当于如下代码：
// return wait_until (lck, chrono::steady_clock::now() + rel_time, std::move(pred));
std::condition_variable cv1;
int value;
void do_read_value() {
  std::cin >> value;
  cv1.notify_one();
}

void testWaitfor() {
  std::cout << "Please, enter an integer (I'll be printing dots): \n";
  std::thread t1(do_read_value);
  std::mutex mtx1;
  std::unique_lock<std::mutex> lck(mtx1);
  while (cv1.wait_for(lck, std::chrono::seconds(1)) ==
         std::cv_status::timeout) {
    std::cout << '.';
    std::cout.flush();
  }
  std::cout << "You entered: " << value << '\n';
  t1.join();
}
}  // namespace waitfor

namespace waituntil {
// wait_until与wait_for类似，但是wait_until可以指定一个时间点，
// 在当前线程收到通知或者指定的时间点abs_time超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了其他线程的通知，wait_until返回，剩下的处理步骤和wait_until()类似。
// 另外，wait_until的重载版本的最后一个参数pred表示wait_until的预测条件，
// 只有当pred条件为false时调用wait()才会阻塞当前线程；
// 并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞，因此相当于如下代码：
// while (!pred())
//   if ( wait_until(lck,abs_time) == cv_status::timeout)
//     return pred();
// return true;

std::condition_variable cv;
std::mutex cv_m;
std::atomic<int> i{0};

void waits(int idx) {
  std::unique_lock<std::mutex> lk(cv_m);
  auto now = std::chrono::system_clock::now();
  if (cv.wait_until(lk, now + idx * std::chrono::milliseconds(100),
                    []() { return i == 1; }))
    std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
  else
    std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
}

void signals() {
  std::this_thread::sleep_for(std::chrono::milliseconds(120));
  std::cerr << "Notifying...\n";
  cv.notify_all();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  i = 1;
  std::cerr << "Notifying again...\n";
  cv.notify_all();
}

void testWaituntil() {
  std::thread t1(waits, 1), t2(waits, 2), t3(waits, 3), t4(signals);
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
}  // namespace waituntil

namespace notify {
// notify_one()唤醒某个等待(wait)线程。
// 如果当前没有等待线程，则该函数什么也不做，如果同时存在多个等待线程，则唤醒某个线程是不确定的。
// notify_all()唤醒所有的等待(wait)线程。如果当前没有等待线程，则该函数什么也不做。

std::mutex mtx1;
std::condition_variable cv1;
void func1(int i) {
  std::unique_lock<std::mutex> lck(mtx1);
  cv1.wait(lck);
  std::cout << "thread " << i << std::endl;
}

void testNotify() {
  std::thread ts1[10];
  for (int i = 0; i < 10; ++i) {
    ts1[i] = std::thread(func1, i);
  }
  std::cout << "notify one" << std::endl;
  cv1.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify one" << std::endl;
  cv1.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify one" << std::endl;
  cv1.notify_one();
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << "notify all" << std::endl;
  cv1.notify_all();
  for (auto& ts : ts1) {
    ts.join();
  }
  cv1.notify_all();  // do nothing
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
}  // namespace notify

namespace other {
// std::condition_variable_any与std::condition_variable类似，
// 只不过std::condition_variable_any的wait函数可以接受任何lockable参数，
// 而std::condition_variable只能接受std::unique_lock<std::mutex>类型的参数，
// 除此以外，和std::condition_variable几乎完全一样。
std::condition_variable cv1;
std::mutex m1;
void func1() {
  std::unique_lock<std::mutex> lck(m1);
  cv1.wait(lck);
  // cv1.wait(m1);  // error
}

std::condition_variable_any cva1;
std::mutex m2;
void func2() {
  std::unique_lock<std::mutex> lck(m2);
  cva1.wait(lck);
}

std::condition_variable_any cva2;
std::mutex m3;
void func3() {
  // std::lock_guard<std::mutex> lck(m3);  // 这样可以么？
  m3.lock();
  cva2.wait(m3);
}

// std::cv_status枚举类型
// cv_status::no_timeout:wait_for或者wait_until没有超时，即在规定的时间段内线程收到了通知。
// cv_status::timeout:wait_for或者wait_until超时。

// void notify_all_at_thread_exit (condition_variable& cond, unique_lock<mutex> lck);
std::mutex m4;
std::condition_variable cv4;
bool ready = false;
void print_id(int id) {
  std::unique_lock<std::mutex> lck(m4);
  while (!ready) cv4.wait(lck);
  std::cout << "thxxxxx " << id << std::endl << std::flush;
}
void go() {
  std::unique_lock<std::mutex> lck(m4);
  std::cout << "notify_one" << std::endl;
  ready = true;
  cv4.notify_one();
  lck.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "notify_all when exit" << std::endl;
  std::notify_all_at_thread_exit(cv4, std::move(lck));
}

void testOther() {
  std::thread t1(func1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cv1.notify_all();
  t1.join();

  std::thread t2(func2);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cva1.notify_all();
  t2.join();

  std::thread t3(func3);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  cva2.notify_all();
  t3.join();

  std::thread threads[10];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) threads[i] = std::thread(print_id, i);
  std::cout << "10 threads ready to race...\n";
  std::this_thread::sleep_for(std::chrono::microseconds(1000));
  std::thread(go).detach();
  for (auto& th : threads) th.join();
  // 10 threads ready to race...
  // notify_one
  // thxxxxx 0
  // notify_all when exit
  // thxxxxx 1
  // thxxxxx 2
  // thxxxxx 7
  // thxxxxx 9
  // thxxxxx 6
  // thxxxxx 4
  // thxxxxx 5
  // thxxxxx 3
  // thxxxxx 8
}
}  // namespace other

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      wait::testWait();
      break;
    case 1:
      waitfor::testWaitfor();
      break;
    case 2:
      waituntil::testWaituntil();
      break;
    case 3:
      notify::testNotify();
      break;
    case 4:
      other::testOther();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}