#include <future>
#include <iostream>

using namespace std;

// 与std::future相关的函数主要是std::async()
// 两组std::async()的不同之处是第一类std::async没有指定异步任务（即执行某一函数）的启动策略(launch policy)，而第二类函数指定了启动策略，详见std::launch枚举类型，
// 指定启动策略的函数的policy参数可以是launch::async，launch::deferred，以及两者的按位或(|)。
// 与std::future相关的枚举类型包括：
// std::future_errc类型描述如下,enum class future_errc;：
// broken_promise 0 与该std::future共享状态相关联的std::promise对象在设置值或者异常之前一被销毁。
// future_already_retrieved 1 与该std::future对象相关联的共享状态的值已经被当前Provider获取了，即调用了std::future::get函数。
// promise_already_satisfied 2 std::promise对象已经对共享状态设置了某一值或者异常。
// no_state 3 无共享状态。
// std::future_status类型,enum class future_status;
// std::future_status类型主要用在std::future(或std::shared_future)中的wait_for和wait_until两个函数中的。
// future_status::ready 0 wait_for(或wait_until)因为共享状态的标志变为ready而返回。
// future_status::timeout 1 超时，即wait_for(或wait_until)因为在指定的时间段（或时刻）内共享状态的标志依然没有变为ready而返回。
// future_status::deferred 2 共享状态包含了deferred函数。
// std::launch类型,enum class launch;
// 该枚举类型主要是在调用std::async设置异步任务的启动策略的。
// launch::async Asynchronous:异步任务会在另外一个线程中调用，并通过共享状态返回异步任务的结果（一般是调用 std::future::get()获取异步任务的结果）。
// launch::deferred Deferred:异步任务将会在共享状态被访问时调用，相当与按需调用（即延迟(deferred)调用）。

// std::packaged_task::operator()(Args... args)
// 调用该packaged_task对象所包装的对象(通常为函数指针，函数对象，lambda 表达式等)，传入的参数为args.调用该函数一般会发生两种情况：
//   如果成功调用packaged_task所包装的对象，则返回值（如果被包装的对象有返回值的话）被保存在packaged_task的共享状态中。
//   如果调用packaged_task所包装的对象失败，并且抛出了异常，则异常也会被保存在packaged_task的共享状态中。
// 以上两种情况都使共享状态的标志变为ready，因此其他等待该共享状态的线程可以获取共享状态的值或者异常并继续执行下去。
// 共享状态的值可以通过在future对象(由get_future获得)上调用get来获得。
// 由于被包装的任务在packaged_task构造时指定，因此调用operator()的效果由packaged_task对象构造时所指定的可调用对象来决定：
//   如果被包装的任务是函数指针或者函数对象，调用std::packaged_task::operator()只是将参数传递给被包装的对象。
//   如果被包装的任务是指向类的非静态成员函数的指针，那么std::packaged_task::operator()的第一个参数应该指定为成员函数被调用的那个对象，剩余的参数作为该成员函数的参数。
//   如果被包装的任务是指向类的非静态成员变量，那么std::packaged_task::operator()只允许单个参数。
// std::packaged_task::make_ready_at_thread_exit
// 该函数会调用被包装的任务，并向任务传递参数，类似std::packaged_task的operator()成员函数。但是与operator()函数不同的是，make_ready_at_thread_exit并不会立即设置共享
// 状态的标志为ready，而是在线程退出时设置共享状态的标志。
// 如果与该packaged_task共享状态相关联的future对象在future::get处等待，则当前的future::get调用会被阻塞，直到线程退出。而一旦线程退出，future::get调用继续执行，或者抛出异常。
// 注意，该函数已经设置了promise共享状态的值，如果在线程结束之前有其他设置或者修改共享状态的值的操作，则会抛出future_error(promise_already_satisfied)。
// std::packaged_task::reset()重置packaged_task的共享状态，但是保留之前的被包装的任务。
// std::packaged_task::swap()交换packaged_task的共享状态。
void test04() {
  auto lam = [](int from, int to) -> int {
    for (int i = from; i != to; i--) {
      cout << i << "\n";
      this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "Finished!\n";
    return from - to;
  };

  // 传递一个函数类型，返回类型（参数类型，参数类型. .. ）
  packaged_task<int(int, int)> task(lam);  // 设置packaged_task
  future<int> f =
      task.get_future();  // 获得与packaged_task共享状态相关联的 future对象

  thread t(std::move(task), 5, 0);  // 创建一个新线程完成计数任务

  int x = f.get();
  cout << "the lam lasted for " << x << " seconds\n";
  t.join();
}
// std::move转移所有权、重置packaged_task对象的共享状态（保留了之前包装的任务）
void test05() {
  auto lam = [](int x) { return 3 * x; };

  packaged_task<int(int)> task(lam);
  future<int> f = task.get_future();
  thread(std::ref(task), 100).detach();  // 分离  这里千万不要传递task所有权
  cout << f.get() << endl;

  task.reset();

  f = task.get_future();
  thread(std::move(task), 200).detach();  // 分离
  cout << f.get() << endl;
}

// async()
// 不用thread对象也可以创建线程
// std::async大概的工作过程：先将异步操作用std::packaged_task包装起来，然后将异步操作的结果放到std::promise中，这个过程就是创造未来的过程。外面再通过future.get/wait
// 来获取这个未来的结果。可以说，std::async帮我们将std::future、std::promise和std::packaged_task三者结合了起来。
// async返回一个与函数返回值相对应类型的future，通过它我们可以在其他任何地方获取异步结果。
// 除了std::launch::async，还有一个std::launch::deferred策略，它会延迟线程地创造，也就是说只有当我们调用future.get()时子线程才会被创建以执行任务
bool is_prime(int x) {
  for (int i = 2; i < x; ++i)
    if (x % i == 0) return false;
  return true;
}
void test06() {
  future<bool> f = async(is_prime, 7);
  cout << "checking, please wait";
  chrono::milliseconds s(1000);
  while (f.wait_for(s) == future_status::timeout) cout << " . ";
  bool x = f.get();
  cout << "\7 " << (x ? "is" : "is not") << " prime.\n";
}
// future_status::ready:共享状态的标志已经变成ready,即Provider在共享状态上设定了值或设定了异常
// future_status::timeout:超时，在规定的时间内共享状态的标志没有变成ready
// future_status::deferred:共享状态包含一个deferred函数
void print(char c, int ms) {
  for (int i = 0; i < 10; i++) {
    this_thread::sleep_for(chrono::milliseconds(ms));
    cout << c;
  }
}
void test07() {
  cout << " with launch::async >> \n";
  future<void> f = async(launch::async, print, '*', 100);
  future<void> b = async(launch::async, print, '@', 200);
  f.get();
  b.get();
  cout << "\n\n with launch::deferred >> \n";
  f = async(launch::deferred, print, '*', 100);
  b = async(launch::deferred, print, '@', 200);
  f.get();
  b.get();
}

int main() {
  // 通过async来获取异步操作结果
  std::future<int> result = std::async([]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 8;
  });
  std::cout << "the future result : " << result.get() << std::endl;
  std::cout << "the future status : " << result.valid() << std::endl;
  try {
    result.wait();  // 或者result.get(),会异常
    // 因此std::future只能用于单线程中调用，多线程调用使用std::share_future();
  } catch (...) {
    std::cout << "get error....\n ";
  }
  // the future result : 8
  // the future status : 0
  // get error....

  test04();
  // 5
  // 4
  // 3
  // 2
  // 1
  // Finished!
  // the lam lasted for 10 seconds

  test05();
  // 300
  // 600

  test06();  // checking, please wait is prime

  test07();
  // with launch::async >>
  // **@*@**@**@**@*@@@@@
  // with launch::deferred >>
  // **********@@@@@@@@@@

  return 0;
}