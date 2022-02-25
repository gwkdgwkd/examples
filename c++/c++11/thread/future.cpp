#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// 想要从线程中返回异步任务结果，一般需要依靠全局变量，从安全角度看，有些不妥；
// 为此C++11提供了std::future类模板，future对象提供访问异步操作结果的机制，很轻松解决从异步任务中返回结果。
// 当在一个线程中创建（通过std::async，std::packaged_task或者std::promise）了一个异步操作（通常是线程）的时候，
// 这个异步操作会返回一个future对象给当前的线程，供其访问异步操作的状态，结果等等。
// std::future_error继承子C++标准异常体系中的logic_error。

namespace future {
// future某种意义上表示的是一个异步操作，通过其成员函数我们能够获悉异步操作处于什么样的情况。
// 在C++标准库中，有两种“期望”，使用两种类型模板实现：
//   唯一期望(unique futures，std::future<>) std::future的实例只能与一个指定事件相关联。
//   共享期望(shared futures)(std::shared_future<>) std::shared_future的实例就能关联多个事件。
// 一般来说，通过异步操作创建的future会被这些异步操作设置共享状态。
// future对象可以通过valid()函数查询其共享状态是否有效，
// 一般来说，只有当valid()返回true的时候才调用get()去获取结果，这也是C++文档推荐的操作。
// 一个有效的future对象只能通过async(),promise::get_future或者packaged_task::get_future来初始化。
// future也提供了构造函数，不过std::future的拷贝构造函数是被禁用的，只提供了默认的构造函数和move构造函数。
// 另外，std::future的普通赋值操作也被禁用，只提供了move赋值操作。

void func0() {
  // 由默认构造函数创建的std::future对象是无效的
  // 通过std::future的move赋值后该std::future对象也可以变为valid
  std::promise<int> p1;
  std::future<int> f1, f2;
  std::cout << std::boolalpha << f1.valid() << "," << f2.valid()
            << std::endl;  // false,false
  f1 = p1.get_future();
  std::cout << std::boolalpha << f1.valid() << "," << f2.valid()
            << std::endl;  // true,false
  f2 = std::move(f1);
  std::cout << std::boolalpha << f1.valid() << "," << f2.valid()
            << std::endl;  // false,true

  // std::future<int> f3 = f2;  // 没有拷贝构造函数

  std::promise<int> p2;

  std::shared_future<int> sf1 = p2.get_future();
  std::shared_future<int> sf2 = sf1;             // 可以拷贝
  std::shared_future<int> sf3 = std::move(sf2);  // 可以move
  // std::shared_future<int> sf5 = f1;  // 不能隐式转换？

  // share返回一个std::shared_future对象，调用该函数之后，
  // 该std::future对象本身已经不和任何共享状态相关联，因此该std::future的状态不再是valid的了。
  std::shared_future<int> sf4 = f2.share();  // 可以显示转换
  std::cout << std::boolalpha << "valid :" << f2.valid() << std::endl;  // false
}

void func1(std::promise<std::string>& p, std::string name, int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  p.set_value("set future by " + name);
}

void func2(std::future<std::string>& f) {
  // 可以通过get来等待异步操作结束并返回结果，是一个阻塞过程。
  std::cout << f.get() << std::endl;
  // get()调用会改变其共享状态，不再可用
  std::cout << std::boolalpha << "valid after get:" << f.valid() << std::endl;
  // 也就是说get()只能被调用一次，多次调用会触发异常
  try {
    std::cout << f.get() << std::endl;
  } catch (std::future_error e) {
    std::cout << e.what() << " after get future" << std::endl;
  }
}

void func3(std::future<std::string>& f) {
  auto start = std::chrono::system_clock::now();
  f.wait();  // wait等待异步操作结束结束，也是一个阻塞过程。
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::system_clock::now() - start;
  std::cout
      << "waited for "
      << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
      << " milliseconds, value：" << f.get() << std::endl;
}

void func4(std::future<std::string>& f) {
  std::future_status status;
  do {
    status = f.wait_for(std::chrono::milliseconds(500));
    if (status == std::future_status::deferred) {
      std::cout << "deferred" << std::endl;
    } else if (status == std::future_status::timeout) {
      std::cout << "timeout" << std::endl;
    } else if (status == std::future_status::ready) {
      std::cout << "ready, value:" << f.get() << std::endl;
    }
  } while (status != std::future_status::ready);
}

void func5(std::future<std::string>& f) {
  std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
  std::future_status status;
  status = f.wait_until(time + std::chrono::seconds(1));
  if (status == std::future_status::deferred) {
    std::cout << "deferred" << std::endl;
  } else if (status == std::future_status::timeout) {
    std::cout << "timeout" << std::endl;
  } else if (status == std::future_status::ready) {
    std::cout << "ready, value:" << f.get() << std::endl;
  }
}

void func6(std::promise<int>& p) {
  // std::promise::set_value_at_thread_exit设置共享状态的值
  // 但是不将共享状态的标志设置为ready，当线程退出时该promise对象会自动设置为ready
  p.set_value_at_thread_exit(5);
  try {
    // 在线程结束之前有其他设置或者修改共享状态的值的操作，则会抛出future_error
    p.set_value(8);
  } catch (std::future_error e) {
    std::cout << e.what() << std::endl;  // Promise already satisfied
  }
}
std::mutex m;
void func7(std::shared_future<int>& sf, std::string name) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::lock_guard<std::mutex> lck(m);
  std::cout << name << ",1:" << sf.get() << std::endl;
  std::cout << name << ",2:" << sf.get() << std::endl;
}

void testFuture() {
  func0();

  std::promise<std::string> p1;
  std::future<std::string> f1;
  std::cout << std::boolalpha << "f1 valid before init:" << f1.valid()
            << std::endl;
  f1 = p1.get_future();
  std::cout << std::boolalpha << "f1 valid before run:" << f1.valid()
            << std::endl;
  std::thread t1(func1, std::ref(p1), "t1", 0);
  std::thread t2(func2, std::ref(f1));
  t1.join();
  t2.join();
  // valid before init:false
  // valid before run:true
  // set future by t1
  // valid after get:false
  // std::future_error: No associated state after get future

  std::promise<std::string> p2;
  std::future<std::string> f2 = p2.get_future();
  std::thread t3(func1, std::ref(p2), "t3", 675);
  std::thread t4(func3, std::ref(f2));
  t3.join();
  t4.join();
  // waited for 675 milliseconds, value：set future by t3

  std::promise<std::string> p3;
  std::future<std::string> f3 = p3.get_future();
  std::thread t5(func1, std::ref(p3), "t4", 1500);
  std::thread t6(func4, std::ref(f3));
  t5.join();
  t6.join();
  // timeout
  // timeout
  // ready, value:set future by t4

  std::promise<std::string> p4;
  std::future<std::string> f4 = p4.get_future();
  // std::thread t7(func1, std::ref(p4), "t7", 2000);  // timeout
  std::thread t7(func1, std::ref(p4), "t7", 500);
  std::thread t8(func5, std::ref(f4));
  t7.join();
  t8.join();
  // ready, value:set future by t7

  std::promise<int> p5;
  std::shared_future<int> sf1 = p5.get_future();
  try {
    std::shared_future<int> sf2 = p5.get_future();
  } catch (std::future_error e) {
    std::cout << e.what() << std::endl;  // Future already retrieved
  }
  std::thread t9(func6, std::ref(p5));
  std::thread ths[5];
  for (int i = 0; i < 5; ++i) {
    std::string name = "th" + std::to_string(i);
    ths[i] = std::thread(func7, std::ref(sf1), name);
  }
  t9.join();
  for (auto& t : ths) {
    t.join();
  }
  // th0,1:5
  // th0,2:5
  // th1,1:5
  // th1,2:5
  // th4,1:5
  // th4,2:5
  // th3,1:5
  // th3,2:5
  // th2,1:5
  // th2,2:5
}
}  // namespace future

namespace promise {
// promise对象保存某一类型的值，该值可被future对象读取（可能在其他线程），因此promise也提供了一种线程同步的手段。
// std::promise::get_future函数返回一个与promise共享状态相关联的future。
// 返回的future对象可以访问由promise对象设置在共享状态上的值或者某个异常对象。
// 只能从promise共享状态获取一个future对象。
// 在调用该函数之后，promise对象通常会在某个时间点准备好(设置值或者异常)，如果不设置值或者异常，
// promise对象在析构时会自动地设置一个future_error异常(broken_promise)来设置其自身的准备状态。
// std::promise::set_value函数设置共享状态的值，此后promise的共享状态标志变为ready。
// std::promise::set_value_at_thread_exit设置共享状态的值，
// 但是不将共享状态的标志设置为ready，当线程退出时该promise对象会自动设置为ready，
// 如果在线程结束之前有其他设置或者修改共享状态的值的操作，则会抛出future_error(promise_already_satisfied)。
// std::promise::swap交换promise的共享状态。
// std::promise::set_exception_at_thread_exit。

void func1() {
  std::promise<int> p1;  // 默认构造函数，初始化一个空的共享状态。
  // 带自定义内存分配器的构造函数，与默认构造函数类似，但是使用自定义分配器来分配共享状态。
  // std::promise<int> p2 = p1;  // 拷贝构造函数被禁用
  std::promise<int> p3;
  // p3 = p1;  // 赋值操作符号被禁用
  p3 = std::move(p1);                   // 移动赋值操作符
  std::promise<int> p4(std::move(p1));  // 移动构造函数
}

void func2(std::promise<int>& p) {
  int x;
  std::cout << "please,enter an integer value = ";
  std::cin.exceptions(std::ios::failbit);  // throw on failbit
  try {
    std::cin >> x;
    p.set_value(x);
  } catch (std::exception&) {
    p.set_exception(std::current_exception());
  }
}
void func3(std::future<int>& f) {
  try {
    int x = f.get();
    std::cout << "value = " << x << std::endl;
  } catch (std::exception& e) {
    std::cout << "[exception caught >> " << e.what() << "]\n";
  }
}

void testPromise() {
  func1();

  std::promise<int> p;
  std::future<int> f = p.get_future();
  std::thread t1(func2, std::ref(p));
  std::thread t2(func3, std::ref(f));
  t1.join();
  t2.join();
  // please,enter an integer value = 1
  // value = 1
  // please,enter an integer value = sdf
  // [exception caught >> basic_ios::clear: iostream error]
}
}  // namespace promise

namespace packagedtask {
// 几种构造函数的语义：
//   默认构造函数，初始化一个空的共享状态，并且该packaged_task对象无包装任务。
//   初始化一个共享状态，并且被包装任务由参数fn指定。
//   带自定义内存分配器的构造函数，与默认构造函数类似，但是使用自定义分配器来分配共享状态。
//   拷贝构造函数，被禁用。
//   移动构造函数。
// 与std::promise类似，std::packaged_task也禁用了普通的赋值操作运算，只允许move赋值运算。

// packaged_task对象内部包含两个基本元素：
//  1.包装的任务，即一个可调用对象
//  2.共享状态，用于保存任务的返回值。可以通过future对象来达到异步访问共享状态的效果
// packaged_task是对一个任务的抽象，可以给其传递一个函数来完成其构造。
// 相较于promise，它应是更高层次的一个抽象。
// 同样地，可以将任务投递给任何线程去完成，然后获取packaged_task的future来获取共享数据。
// 总结来说，packaged_task是连数据操作创建都封装进去了的promise。
// std::packaged_task::get_future返回一个与packaged_task对象共享状态相关的future对象。
// 返回的future对象可以获得由另外一个线程在该packaged_task对象的共享状态上设置的某个值或者异常。

int func1(int a, int b) { return a + b; }
void func2(std::future<int>& f) {
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << f.get() << std::endl;
}

void func3() {
  auto lam = [](int x) { return 3 * x; };

  std::packaged_task<int(int)> task(lam);
  std::future<int> f = task.get_future();
  std::thread(std::ref(task), 100)
      .detach();  // 分离  这里千万不要传递task所有权
  std::cout << f.get() << std::endl;

  task.reset();

  f = task.get_future();
  // std::thread(std::move(task), 200).detach();  // 分离
  task(200);
  std::cout << f.get() << std::endl;
}

void testPackagedtask() {
  std::packaged_task<int(int, int)> task1(func1);
  std::future<int> f1 = task1.get_future();
  std::thread t1(std::move(task1), 5, 6);
  std::thread t2(func2, std::ref(f1));
  t1.join();
  t2.join();

  func3();
}
}  // namespace packagedtask

namespace async {
void testAsync() {}
}  // namespace async

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 5]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      future::testFuture();
      break;
    case 1:
      promise::testPromise();
      break;
    case 2:
      packagedtask::testPackagedtask();
      break;
    case 3:
      async::testAsync();
      break;
    case 4:
      break;
    case 5:
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}