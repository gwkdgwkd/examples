#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

// 当std::condition_variable对象的某个wait函数被调用的时候，它使用std::unique_lock(通过 std::mutex)来锁住当前线程。当前线程会一直被阻塞，直到另外一个线程在相同的
// std::condition_variable对象上调用了notification函数来唤醒当前线程。
// condition_variable条件变量可以阻塞（wait、wait_for、wait_until）调用的线程直到使用（notify_one或notify_all）通知恢复为止
// std::condition_variable对象通常使用std::unique_lock<std::mutex>来等待，如果需要使用另外的lockable类型，可以使用std::condition_variable_any类。
std::mutex mtx;              // 全局互斥锁.
std::condition_variable cv;  // 全局条件变量.
bool ready = false;          // 全局标志位.
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

// std::condition_variable的拷贝构造函数被禁用，只提供了默认构造函数
// condition_variable();
// condition_variable (const condition_variable&) = delete;
// condition_variable是一个类，这个类既有构造函数也有析构函数，使用时需要构造对应的condition_variable对象，调用对象相应的函数来实现上面的功能。

// std::condition_variable::wait()
// void wait (unique_lock<mutex>& lck);
// template <class Predicate> void wait (unique_lock<mutex>& lck, Predicate pred);
// std::condition_variable提供了两种wait()函数。第一种只包含unique_lock对象，另外一个还有Predicate对象（等待条件），这里必须使用unique_lock，因为wait函数的工作原理：
// 当前线程调用wait()后将被阻塞并且函数会解锁互斥量，如果使用lock_guard则不能调用unlock函数，所以这里只能使用unique_lock对象，直到另外某个线程调用notify_one或者
// notify_all唤醒当前线程；一旦当前线程获得通知(notify)，wait()函数也是自动调用lock()，同理不能使用lock_guard对象。
// 如果wait没有第二个参数，第一次调用默认条件不成立，直接解锁互斥量并阻塞到本行，直到某一个线程调用notify_one或notify_all为止，被唤醒后，wait重新尝试获取互斥量，如果得不到，
// 线程会卡在这里，直到获取到互斥量，然后无条件地继续进行后面的操作
// 如果wait包含第二个参数，如果第二个参数不满足，那么wait将解锁互斥量并堵塞到本行，直到某一个线程调用notify_one或notify_all为止，被唤醒后，wait重新尝试获取互斥量，如果得
// 不到，线程会卡在这里，直到获取到互斥量，然后继续判断第二个参数，如果表达式为false，wait对互斥量解锁，然后休眠，如果为true，则进行后面的操作
// 当前线程调用wait()后将被阻塞(此时当前线程应该获得了锁（mutex）lck)，直到另外某个线程调用notify_*唤醒了当前线程。
// 在线程被阻塞时，该函数会自动调用lck.unlock()释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。另外，一旦当前线程获得通知(notified，通常是另外某个线程调用notify_*
// 唤醒了当前线程)，wait()函数也是自动调用lck.lock()，使得lck的状态和wait函数被调用时相同。
// 在第二种情况下（即设置了Predicate），只有当pred条件为false时调用wait()才会阻塞当前线程，并且在收到其他线程的通知后只有当pred为true时才会被解除阻塞。类似以下代码：
// while (!pred()) wait(lck);
std::mutex mtx1;
std::condition_variable cv1;
int cargo = 0;
bool shipment_available() { return cargo != 0; }
// 消费者线程.
void consume(int n) {
  for (int i = 0; i < n; ++i) {
    std::unique_lock<std::mutex> lck(mtx1);
    cv1.wait(lck, shipment_available);
    std::cout << cargo << '\n';
    cargo = 0;
  }
}

// std::condition_variable::wait_for()
// template <class Rep, class Period>
// cv_status wait_for (unique_lock<mutex>& lck, const chrono::duration<Rep,Period>& rel_time);
// template <class Rep, class Period, class Predicate>
// bool wait_for (unique_lock<mutex>& lck, const chrono::duration<Rep,Period>& rel_time, Predicate pred);
// 与std::condition_variable::wait()类似，不过wait_for可以指定一个时间段，在当前线程收到通知或者指定的时间rel_time超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了其他线程的通知，wait_for返回，剩下的处理步骤和wait()类似。
// wait_for的重载版本（predicte(2)）的最后一个参数pred表示wait_for的预测条件，只有当pred条件为false时调用wait()才会阻塞当前线程，并且在收到其他线程的通知后只有
// 当pred为true时才会被解除阻塞。因此相当于如下代码：
// return wait_until (lck, chrono::steady_clock::now() + rel_time, std::move(pred));
std::condition_variable cv2;
int value;
void do_read_value() {
  std::cin >> value;
  cv2.notify_one();
}

// std::condition_variable::wait_until
// template <class Clock, class Duration>
// cv_status wait_until(unique_lock<mutex>& lck, const chrono::time_point<Clock, Duration>& abs_time);
// template <class Clock, class Duration, class Predicate>
// bool wait_until (unique_lock<mutex>& lck, const chrono::time_point<Clock,Duration>& abs_time, Predicate pred);
// 与std::condition_variable::wait_for类似，但是wait_until可以指定一个时间点，在当前线程收到通知或者指定的时间点abs_time超时之前，该线程都会处于阻塞状态。
// 而一旦超时或者收到了其他线程的通知，wait_until返回，剩下的处理步骤和wait_until()类似。
// 另外，wait_until的重载版本（predicte(2)）的最后一个参数pred表示wait_until的预测条件，只有当pred条件为false时调用wait()才会阻塞当前线程，并且在收到其他线程的通知
// 后只有当pred为true时才会被解除阻塞，因此相当于如下代码：
// while (!pred())
//   if ( wait_until(lck,abs_time) == cv_status::timeout)
//     return pred();
// return true;

// std::condition_variable::notify_one()
// 唤醒某个等待(wait)线程。如果当前没有等待线程，则该函数什么也不做，如果同时存在多个等待线程，则唤醒某个线程是不确定的(unspecified)。
std::mutex mtx3;
std::condition_variable cv3;
int cargo1 = 0;  // shared value by producers and consumers
void consumer1() {
  std::unique_lock<std::mutex> lck(mtx3);
  while (cargo1 == 0) cv3.wait(lck);
  std::cout << cargo1 << '\n';
  cargo1 = 0;
}
void producer1(int id) {
  std::unique_lock<std::mutex> lck(mtx3);
  cargo1 = id;
  cv3.notify_one();
}

// std::condition_variable::notify_all()
// 唤醒所有的等待(wait)线程。如果当前没有等待线程，则该函数什么也不做。

// std::condition_variable_any
// 与std::condition_variable类似，只不过std::condition_variable_any的wait函数可以接受任何lockable参数，而std::condition_variable只能接受
// std::unique_lock<std::mutex>类型的参数，除此以外，和std::condition_variable几乎完全一样。

// std::cv_status枚举类型介绍
// cv_status::no_timeout : wait_for或者wait_until没有超时，即在规定的时间段内线程收到了通知。
// cv_status::timeout : wait_for或者wait_until超时。

// std::notify_all_at_thread_exit
// void notify_all_at_thread_exit (condition_variable& cond, unique_lock<mutex> lck);
std::mutex mtx4;
std::condition_variable cv4;
bool ready4 = false;
void print_id(int id) {
  std::unique_lock<std::mutex> lck(mtx4);
  while (!ready4) cv4.wait(lck);
  std::cout << "thxxxxx " << id << '\n';
}
void go4() {
  std::unique_lock<std::mutex> lck(mtx4);
  std::notify_all_at_thread_exit(cv4, std::move(lck));
  ready4 = true;
}

int main() {
  std::thread threads[10];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) threads[i] = std::thread(do_print_id, i);
  std::cout << "10 threads ready to race...\n";
  go();  // go!
  for (auto& th : threads) th.join();
  // 10 threads ready to race...
  // thread 0
  // thread 1
  // thread 3
  // thread 4
  // thread 5
  // thread 6
  // thread 7
  // thread 8
  // thread 9
  // thread 2

  std::thread consumer_thread(consume, 10);  // 消费者线程.
  // 主线程为生产者线程, 生产10个物品.
  for (int i = 0; i < 10; ++i) {
    while (shipment_available()) std::this_thread::yield();
    std::unique_lock<std::mutex> lck(mtx1);
    cargo = i + 1;
    cv1.notify_one();
  }
  consumer_thread.join();

  std::cout << "Please, enter an integer (I'll be printing dots): \n";
  std::thread th(do_read_value);
  std::mutex mtx2;
  std::unique_lock<std::mutex> lck(mtx2);
  while (cv2.wait_for(lck, std::chrono::seconds(1)) ==
         std::cv_status::timeout) {
    std::cout << '.';
    std::cout.flush();
  }
  std::cout << "You entered: " << value << '\n';
  th.join();

  std::thread consumers[10], producers[10];
  // spawn 10 consumers and 10 producers:
  for (int i = 0; i < 10; ++i) {
    consumers[i] = std::thread(consumer1);
    producers[i] = std::thread(producer1, i + 1);
  }
  // join them back:
  for (int i = 0; i < 10; ++i) {
    producers[i].join();
    consumers[i].join();
  }

  std::thread threads4[10];
  // spawn 10 threads:
  for (int i = 0; i < 10; ++i) threads4[i] = std::thread(print_id, i);
  std::cout << "10 threads ready to race...\n";
  this_thread::sleep_for(std::chrono::microseconds(1000));
  std::thread(go4).detach();  // go!
  for (auto& th : threads4) th.join();

  return 0;
}