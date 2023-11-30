#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// 想要从线程中返回异步任务结果，一般依靠全局变量，从安全角度看，有些不妥。
// 为此C++11提供了std::future类模板，对象提供访问异步操作结果的机制。
// 当通过async，packaged_task或者promise，创建了一个异步操作（线程）时，
// 会返回future对象给当前的线程，供其访问异步操作的状态，结果等等。

// std::future_error继承于C++标准异常体系中的logic_error：
// 1.broken_promise：0
//   与该future共享状态相关联的promise对象在设置值或者异常之前已被销毁。
// 2.future_already_retrieved：1
//   与该future对象相关联的共享状态的值已经被获取了，即调用了get函数。
// 3.promise_already_satisfied：2
//   std::promise对象已经对共享状态设置了某一值或者异常。
// 4.no_state：3
//   无共享状态。

// std::future_status类型主要用在std::future，
// 或std::shared_future的wait_for和wait_until两个函数中的。
// ready：0，wait_for或wait_until因为共享状态的标志变为ready而返回。
// timeout：1，超时，即wait_for或wait_until因为在指定的时间段或时刻内，
//         共享状态的标志依然没有变为ready而返回。
// deferred：2，共享状态包含了deferred函数。

namespace n1 {
// future表示的是一个异步操作，通过其成员函数能够获悉异步操作的情况。
// 在C++标准库中，有两种期望，使用两种类型模板实现：
// 1.唯一期望，std::future<>，实例只能与一个指定事件相关联；
// 2.共享期望，std::shared_future<>，实例能关联多个事件。
// 一般来说，通过异步操作创建的future会被这些异步操作设置共享状态。
// future对象可以通过valid()函数查询其共享状态是否有效，
// 一般来说，只有当valid()返回true时才调用get()获取结果，这是C++推荐的操作。

// 一个有效的future对象只能通过下面来初始化：
// 1.async()；
// 2.promise::get_future()；
// 3.packaged_task::get_future()。
// future也提供了构造函数，不过std::future的拷贝构造函数是被禁用的，
// 只提供了默认的构造函数和move构造函数。
// 另外，std::future的普通赋值操作也被禁用，只提供了move赋值操作。

namespace test1 {
void func() {
  // 由默认构造函数创建的std::future对象是无效的，
  // 通过std::future的move赋值后该std::future对象也可以变为valid：
  std::promise<int> p1;
  std::future<int> f1, f2;
  std::cout.setf(std::ios_base::boolalpha);
  std::cout << f1.valid() << "," << f2.valid() << std::endl;  // false,false
  f1 = p1.get_future();
  std::cout << f1.valid() << "," << f2.valid() << std::endl;  // true,false
  f2 = std::move(f1);
  std::cout << f1.valid() << "," << f2.valid() << std::endl;  // false,true

  // std::future<int> f3 = f2;  // 没有拷贝构造函数

  std::promise<int> p2;
  std::shared_future<int> sf1 = p2.get_future();
  std::shared_future<int> sf2 = sf1;             // 可以拷贝
  std::shared_future<int> sf3 = std::move(sf2);  // 可以move

  // share返回一个std::shared_future对象，调用该函数之后，
  // 该future对象本身已经不和任何共享状态相关联，因此该future不再是valid的了：
  std::shared_future<int> sf4 = f2.share();  // 可以显示转换
  std::cout << f2.valid() << std::endl;      // false
  // std::shared_future<int> sf5 = f1;  // 不能隐式转换
}
}  // namespace test1

void fun(std::promise<std::string>& p, std::string name, int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  p.set_value("set future by " + name);
}
namespace test2 {
void get(std::future<std::string>& f) {
  // 可以通过get来等待异步操作结束并返回结果，是一个阻塞过程：
  std::cout << f.get() << std::endl;
  // get()调用会改变其共享状态，不再可用：
  std::cout << std::boolalpha << "valid after get:" << f.valid() << std::endl;
  try {
    // get()只能被调用一次，多次调用会触发异常：
    std::cout << f.get() << std::endl;
  } catch (std::future_error e) {
    std::cout << e.what() << " after get future" << std::endl;
  }
}
void func() {
  std::cout.setf(std::ios_base::boolalpha);
  std::promise<std::string> p;
  std::future<std::string> f;
  std::cout << "valid before init:" << f.valid() << std::endl;
  f = p.get_future();
  std::cout << "valid before run:" << f.valid() << std::endl;
  std::thread t1(fun, std::ref(p), "fun", 0);
  std::thread t2(get, std::ref(f));
  t1.join();
  t2.join();

  // valid before init:false
  // valid before run:true
  // set future by fun
  // valid after get:false
  // std::future_error: No associated state after get future
}
}  // namespace test2

namespace test3 {
void get(std::future<std::string>& f) {
  auto start = std::chrono::system_clock::now();
  // wait等待异步操作结束，也是一个阻塞过程：
  f.wait();
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::system_clock::now() - start;
  std::cout
      << "waited for "
      << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
      << " milliseconds, value：" << f.get() << std::endl;
}

void func() {
  std::promise<std::string> p;
  std::future<std::string> f = p.get_future();
  std::thread t1(fun, std::ref(p), "fun", 675);
  std::thread t2(get, std::ref(f));
  t1.join();
  t2.join();

  // waited for 675 milliseconds, value：set future by fun
}
}  // namespace test3

namespace test4 {
void get(std::future<std::string>& f) {
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
void func() {
  std::promise<std::string> p;
  std::future<std::string> f = p.get_future();
  std::thread t1(fun, std::ref(p), "func", 1500);
  std::thread t2(get, std::ref(f));
  t1.join();
  t2.join();

  // timeout
  // timeout
  // ready, value:set future by func
}
}  // namespace test4

namespace test5 {
void get(std::future<std::string>& f) {
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
void func() {
  std::promise<std::string> p;
  std::future<std::string> f = p.get_future();
  // std::thread t1(fun, std::ref(p), "fun", 2000);  // timeout
  std::thread t1(fun, std::ref(p), "fun", 500);
  std::thread t2(get, std::ref(f));
  t1.join();
  t2.join();

  // ready, value:set future by fun
}
}  // namespace test5

namespace test6 {
void f1(std::promise<int>& p) {
  // std::promise::set_value_at_thread_exit设置共享状态的值，
  // 但是不将共享状态的标志设置为ready，线程退出时自动设置为ready：
  p.set_value_at_thread_exit(5);
  try {
    // 在线程结束前有其他设置或修改共享状态值的操作，则抛出future_error：
    p.set_value(8);
  } catch (const std::future_error& e) {
    std::cout << e.what() << std::endl;
  }
}
std::mutex m;
void f2(std::shared_future<int>& sf, std::string name) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::lock_guard<std::mutex> lck(m);
  std::cout << name << ",1:" << sf.get() << std::endl;
  std::cout << name << ",2:" << sf.get() << std::endl;
}
void func() {
  std::promise<int> p;
  std::shared_future<int> sf = p.get_future();
  try {
    std::shared_future<int> sf = p.get_future();
  } catch (const std::future_error& e) {
    std::cout << e.what() << std::endl;
  }
  // Future already retrieved

  std::thread t(f1, std::ref(p));
  // std::future_error: Promise already satisfied

  std::thread ths[5];
  for (int i = 0; i < 5; ++i) {
    std::string name = "th" + std::to_string(i);
    ths[i] = std::thread(f2, std::ref(sf), name);
  }

  // th1,1:5
  // th1,2:5
  // th0,1:5
  // th0,2:5
  // th2,1:5
  // th2,2:5
  // th4,1:5
  // th4,2:5
  // th3,1:5
  // th3,2:5

  t.join();
  for (auto& t : ths) {
    t.join();
  }
}
}  // namespace test6
}  // namespace n1

namespace n2 {
// promise对象保存某一类型的值，该值可被future对象读取（可能在其他线程），
// 因此promise也提供了一种线程同步的手段。
// std::promise::get_future函数返回一个与promise共享状态相关联的future。
// 返回的future对象可以访问由promise对象设置在共享状态上的值或者某个异常对象。
// 只能从promise共享状态获取一个future对象。
// 在调用该函数之后，promise对象通常会在某个时间点准备好（设置值或者异常），
// 如果不设置值或者异常，在析构时，会自动地设置异常broken_promise。
// std::promise::set_value函数设置共享状态的值，共享状态标志变为ready。
// std::promise::swap交换promise的共享状态。
// std::promise::set_exception_at_thread_exit。

void f1() {
  std::promise<int> p1;  // 默认构造函数，初始化一个空的共享状态
  // 带自定义内存分配器的话，与默认构造函数类似，用自定义分配器来分配共享状态。

  // std::promise<int> p2 = p1;  // 拷贝构造函数被禁用
  std::promise<int> p3;
  // p3 = p1;  // 赋值操作符号被禁用

  p3 = std::move(p1);                   // 移动赋值操作符
  std::promise<int> p4(std::move(p1));  // 移动构造函数
}

void f2(std::promise<int>& p) {
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

void f3(std::future<int>& f) {
  try {
    int x = f.get();
    std::cout << "value = " << x << std::endl;
  } catch (std::exception& e) {
    std::cout << "[exception caught >> " << e.what() << "]\n";
  }
}

void func() {
  f1();

  std::promise<int> p;
  std::future<int> f = p.get_future();
  std::thread t1(f2, std::ref(p));
  std::thread t2(f3, std::ref(f));
  t1.join();
  t2.join();

  // please,enter an integer value = 1
  // value = 1

  // please,enter an integer value = sdf
  // [exception caught >> basic_ios::clear: iostream error]
}
}  // namespace n2

namespace n3 {
// 几种构造函数的语义：
// 1.默认构造函数，初始化空的共享状态，并且该packaged_task对象无包装任务；
// 2.初始化一个共享状态，并且被包装任务由参数fn指定；
// 3.带自定义内存分配器，与默认构造函数类似，使用自定义分配器来分配共享状态；
// 4.与std::promise类似，拷贝构造函数，被禁用；
// 5.只允许移动构造函数。

// packaged_task对象内部包含两个基本元素：
// 1.包装的任务，即一个可调用对象；
// 2.共享状态，用于保存任务的返回值，可通过future来达到异步访问的效果。

// packaged_task是对一个任务的抽象，可以给其传递一个函数来完成其构造。
// 相较于promise，它应是更高层次的一个抽象。
// 同样地，可以将任务投递给任何线程去完成，然后通过future来获取共享数据。
// 总结来说，packaged_task是连数据操作创建都封装进去了的promise。
// std::packaged_task::get_future返回一个：
// 与packaged_task对象共享状态相关的future对象。
// 返回的future对象可以获得由另外一个线程设置的某个值或者异常。

// std::packaged_task::operator()(Args... args)，
// 调用该packaged_task对象所包装的对象，
// 通常为函数指针，函数对象，lambda表达式等，传入的参数为args。
// 调用该函数一般会发生两种情况：
// 1.如果调用包装的对象成功，如果被包装的对象有返回值的话，
//   则返回值被保存在packaged_task的共享状态中；
// 2.如果调用包装的对象失败，并且抛出了异常，
//   则异常也会被保存在packaged_task的共享状态中。
// 以上两种情况都使共享状态的标志变为ready，
// 因此其他等待该共享状态的线程可以获取共享状态的值或者异常并继续执行。

// 被包装的任务在构造时指定，因此调用operator()的效果，
// 由packaged_task对象构造时所指定的可调用对象来决定：
// 1.如果被包装的任务是函数指针或者函数对象，
//   调用operator()只是将参数传递给被包装的对象。
// 2.如果被包装的任务是指向类的非静态成员函数的指针，
//   那么operator()的第一个参数应该指定为成员函数被调用的那个对象，
//   剩余的参数作为该成员函数的参数。
// 3.如果被包装的任务是指向类的非静态成员变量，
//   那么std::packaged_task::operator()只允许单个参数。 ？

// std::packaged_task::make_ready_at_thread_exit()
// 该函数会调用被包装的任务，并向任务传递参数，
// 类似std::packaged_task的operator()成员函数。
// 但是与operator()函数不同的是，并不会立即设置共享状态的标志为ready，
// 而是在线程退出时设置共享状态的标志。

// future::get处调用会被阻塞，直到线程退出。
// 而一旦线程退出，future::get调用继续执行，或者抛出异常。
// 注意，该函数已经设置了promise共享状态的值，
// 如果在线程结束之前有其他设置或者修改共享状态的值的操作，
// 则会抛出future_error（promise_already_satisfied）。

// std::packaged_task::swap()
// 交换packaged_task的共享状态。

namespace test1 {
int f1(int a, int b) { return a + b; }
void f2(std::future<int>& f) {
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::cout << f.get() << std::endl;
}
void func() {
  std::packaged_task<int(int, int)> task(f1);
  std::future<int> f = task.get_future();
  std::thread t1(std::move(task), 5, 6);
  std::thread t2(f2, std::ref(f));
  t1.join();
  t2.join();

  // 11
}
}  // namespace test1

namespace test2 {
void func() {
  auto lam = [](int x) { return 3 * x; };

  std::packaged_task<int(int)> task(lam);
  std::future<int> f = task.get_future();
  std::thread(std::ref(task), 100).detach();
  std::cout << f.get() << std::endl;  // 300

  // std::packaged_task::reset()重置的共享状态，但是保留了包装的任务：
  task.reset();
  f = task.get_future();  // 被reset后，需要重新get_future
  task(200);
  std::cout << f.get() << std::endl;  // 600

  task.reset();
  f = task.get_future();
  // 转移了所有权：
  std::thread t = std::thread(std::move(task), 300);
  t.join();
  std::cout << f.get() << std::endl;  // 900

  task.reset();  // 使用了move，不能再reset
  // terminate called after throwing an instance of 'std::future_error'
  //   what():  std::future_error: No associated state
}
}  // namespace test2

namespace test3 {
class A {
 public:
  void func1(int i) { std::cout << "A::func1 " << i << std::endl; }
  static void func2(int i, int j) {
    std::cout << "A::func2 " << i << "," << j << std::endl;
  }
  void operator()() { std::cout << "A()" << std::endl; }
};
void func() {
  A a;
  std::packaged_task<void(int)> task1(std::bind(&A::func1, &a, 6));
  task1(5);
  // A::func1 6，为啥是6？

  std::packaged_task<void(A*, int)> task2(&A::func1);
  task2(&a, 7);
  // A::func1 7

  std::packaged_task<void(int, int)> task3(A::func2);
  task3(7, 8);
  // A::func2 7,8

  // 声明来operator()的类怎么绑定到task？
  // std::packaged_task<void(A*)> task4(A());
  // task4(&a);

  std::packaged_task<void(std::string)> task5(
      [](std::string s) { std::cout << s << std::endl; });
  task5("nihao");
  // nihao
}
}  // namespace test3
}  // namespace n3

namespace n4 {
// 不用thread对象也可以创建线程，std::async大概的工作过程：
// 1.先将异步操作用std::packaged_task包装起来，
//   然后将异步操作的结果放到std::promise中，这个过程就是创造期望的过程；
// 2.外面再通过future.get/wait来获取这个未来的结果。

// async将future、promise和packaged_task三者结合了起来。
// async返回一个与函数返回值相对应类型的future，通过它可以获取异步结果。

// 与std::future相关的函数主要是std::async()，
// 第一类std::async没有指定异步任务的启动策略（launch policy），
// 第二类函数指定了启动策略，std::launch：
// 1.launch::async：
//   异步任务会在另外一个线程中调用，并通过共享状态返回异步任务的结果。
// 2.launch::deferred：
//   异步任务将会在共享状态被访问时调用，相当与按需调用，即延迟调用，
//   也就是说只有当调用future.get()时子线程才会被创建以执行任务。

void func1() {
  std::future<bool> f = std::async(
      [](int x) -> bool {
        for (int i = 2; i < x; ++i)
          if (x % i == 0) return false;
        return true;
      },
      7);
  std::cout << std::boolalpha << f.get() << std::endl;  // true
}

void fun(std::future<int>& f) {
  std::future_status status;
  do {
    status = f.wait_for(std::chrono::milliseconds(500));
    if (status == std::future_status::deferred) {
      std::cout << "deferred, value:" << f.get() << std::endl;
      break;
    } else if (status == std::future_status::timeout) {
      std::cout << "timeout" << std::endl;
    } else if (status == std::future_status::ready) {
      std::cout << "ready, value:" << f.get() << std::endl;
    }
  } while (status != std::future_status::ready);
}

auto lma = []() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  return 5;
};

void func2() {
  // 方式不确定，交由实现自行选择运行方式：
  std::future<int> f = std::async(lma);
  std::thread(fun, std::ref(f)).join();

  // timeout
  // timeout
  // ready, value:5
}

void func3() {
  // launch::deferred表示延迟调用，调用wait()或者get()时，才执行：
  std::future<int> f = std::async(std::launch::deferred, lma);
  std::thread(fun, std::ref(f)).join();

  // deferred, value:5
}

void func4() {
  // launch::async表示在调用async函数的时候就开始创建新线程：
  std::future<int> f = std::async(std::launch::async, lma);
  std::thread(fun, std::ref(f)).join();

  // timeout
  // timeout
  // ready, value:5
}

void func5() {
  // 可能创建新线程，也可能延迟调用，系统会根据当前的资源情况选择：
  std::future<int> f =
      std::async(std::launch::async | std::launch::deferred, lma);
  std::thread(fun, std::ref(f)).join();

  // timeout
  // timeout
  // ready, value:5
}

void func6() {
  auto print = [](std::string s) {
    std::time_t tt =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm* ptm = std::localtime(&tt);
    std::cout << s << " time: " << std::put_time(ptm, "%X") << std::endl;
  };

  print("before");
  // std::future对象的析构需要等待std::async执行完毕，也就是说下面代码并不能并发。
  // 原因在于std::async的返回的std::future对象无人接收，是个临时变量，
  // 临时变量的析构会阻塞，直至std::async异步任务执行完成。
  std::async(std::launch::async,
             [] { std::this_thread::sleep_for(std::chrono::seconds(5)); });
  print("after");
  // before time: 17:50:18
  // after time: 17:50:23

  print("before");
  // 使用static变量接收future，after不阻塞，实现并发，但程序退出时还会等async执行完成。
  static auto f = std::async(std::launch::async, [] {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  });
  print("after");
  // before time: 17:50:23
  // after time: 17:50:23
}
}  // namespace n4

namespace n5 {
class A {
 public:
  static bool func() {
    static int i = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cout << "thread func" << std::endl;
    if (i++ % 2) {
      return true;
    } else {
      return false;
    }
  }
  void run() {
    f_ = std::async(std::launch::async, func);
    std::cout << "valid:" << std::boolalpha << f_.valid() << std::endl;
  }
  void get() {
    std::cout << std::boolalpha << f_.get() << "," << f_.valid() << std::endl;
  }

 private:
  std::future<bool> f_;
};

void func() {
  A a;
  for (int i = 0; i < 2; ++i) {
    a.run();
    a.get();
  }

  // valid:true
  // thread func
  // false,false
  // valid:true
  // thread func
  // true,false
}
}  // namespace n5

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 15]" << std::endl;
    return 0;
  }
  int type = atoi(argv[1]);
  switch (type) {
    case 0:
      n1::test1::func();
      break;
    case 1:
      n1::test2::func();
      break;
    case 2:
      n1::test3::func();
      break;
    case 3:
      n1::test4::func();
      break;
    case 4:
      n1::test5::func();
      break;
    case 5:
      n1::test6::func();
      break;
    case 6:
      n2::func();
      break;
    case 7:
      n3::test1::func();
      break;
    case 8:
      n3::test2::func();
      break;
    case 9:
      n3::test3::func();
      break;
    case 10:
      n4::func1();
      break;
    case 11:
      n4::func2();
      break;
    case 12:
      n4::func3();
      break;
    case 13:
      n4::func4();
      break;
    case 14:
      n4::func5();
      break;
    case 15:
      n4::func6();
      break;
    case 16:
      n5::func();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}