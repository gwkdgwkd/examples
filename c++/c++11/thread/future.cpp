#include <future>
#include <iostream>

using namespace std;

// 想要从线程中返回异步任务结果，一般需要依靠全局变量；从安全角度看，有些不妥；为此C++11提供了std::future类模板，future对象提供访问异步操作结果的机制，很轻松解决
// 从异步任务中返回结果。
// 当在一个线程（creator）中创建（通过std::async，std::packaged_task 或者 std::promise）了一个异步操作（asynchronous operations，通常就是创建了一个新的
// 线程，执行操作）的时候，这个异步操作会返回一个future对象给当前的线程，供其访问异步操作的状态，结果等等。

// 成员函数：
// 构造函数、析构函数、operator=
// share：从*this转移共享状态给share_future并返回它
// get：返回结果
// vald：检查future是否拥有共享状态
// wait：等待结果变得可用
// wait_for：等待结果，如果在指定的时间后仍无结果，则返回。
// wait_until：等待结果，如果到达指定的时间点仍无结果，则返回。
// std::future一般由 std::async,std::promise::get_future,std::packaged_task::get_future创建，不过也提供了构造函数
// 不过std::future的拷贝构造函数是被禁用的，只提供了默认的构造函数和move构造函数。另外，std::future的普通赋值操作也被禁用，只提供了move赋值操作。
// 返回一个std::shared_future对象，调用该函数之后，该std::future对象本身已经不和任何共享状态相关联，因此该std::future的状态不再是valid的了。
// std::future::get()当与该std::future对象相关联的共享状态标志变为ready后，调用该函数将返回保存在共享状态中的值，如果共享状态的标志不为ready，
// 则调用该函数会阻塞当前的调用者，而此后一旦共享状态的标志变为ready，get返回Provider所设置的共享状态的值或者异常（如果抛出了异常）
// std::future::valid()检查当前的std::future对象是否有效，即释放与某个共享状态相关联。一个有效的std::future对象只能通过std::async(),
// std::future::get_future或者std::packaged_task::get_future来初始化。另外由std::future默认构造函数创建的std::future对象是无效(invalid)的，
// 当然通过std::future的move赋值后该std::future对象也可以变为valid。
// std::future::wait()等待与当前std::future 对象相关联的共享状态的标志变为ready.

// <future>头文件中包含了以下几个类和函数：
//   Providers类：std::promise, std::package_task
//   Futures类：std::future, shared_future.
//   Providers函数：std::async()
//   其他类型：std::future_error, std::future_errc, std::future_status, std::launch.

// 在C++标准库中，有两种“期望”，使用两种类型模板实现，
//   唯一期望(unique futures，std::future<>) std::future的实例只能与一个指定事件相关联。
//   共享期望(shared futures)(std::shared_future<>) std::shared_future的实例就能关联多个事件。

// std::async 、std::packaged_task或std::promise能提供一个std::future对象给该异步操作的创建者
// 异步操作的创建者能用各种方法查询、等待或从std::future 提取值。若异步操作仍未提供值，则这些方法可能阻塞。
// 异步操作准备好发送结果给创建者时，它能通过接口（std::promise::set_value std::future）修改共享状态的值。

// future某种意义上表示的是一个异步操作，通过其成员函数我们能够获悉异步操作处于什么样的情况。可以通过get来等待异步操作结束并返回结果，是一个阻塞过程。wait等待异步操
// 作结束结束，也是一个阻塞过程。wait_for是超时等待返回结果，wait_utild类似。
// 一般来说，通过异步操作创建的future会被这些异步操作设置共享状态 。future对象可以通过valid()函数查询其共享状态是否有效，一般来说，只有当valid()返回true的时候才
// 调用get()去获取结果，这也是C++文档推荐的操作。
// 一个有效的std::future对象只能通过std::async(),std::promise::get_future或者std::packaged_task::get_future来初始化。另外由std::future默认构造函数创
// 建的std::future对象是无效（invalid）的，当然通过std::future的move赋值后该std::future对象也可以变为valid。
// 还有一点需要特别注意，get()调用会改变其共享状态，不再可用，也就是说get()只能被调用一次，多次调用会触发异常。如果想要在多个线程中多次获取产出值需要使用shared_future。
// 如果共享状态的标志不是ready（此时Provider没有在共享状态上设置值（或者异常）），调用该函数会被阻塞当前线程，直到共享状态的标志变为ready。
// 一旦共享状态的标志变为ready，wait()函数返回，当前线程被解除阻塞，但是wait()并不读取共享状态的值或者异常。
// std::future::wait_for()与std::future::wait()的功能类似，即等待与该std::future对象相关联的共享状态的标志变为ready
// 而与std::future::wait()不同的是，wait_for()可以设置一个时间段rel_time，如果共享状态的标志在该时间段结束之前没有被Provider设置为ready，则调用wait_for的线
// 程被阻塞，在等待了rel_time的时间长度后wait_until()返回，返回值如下：
// future_status::ready 	共享状态的标志已经变为ready，即 Provider在共享状态上设置了值或者异常。
// future_status::timeout 	超时，即在规定的时间内共享状态的标志没有变为ready。
// future_status::deferred 	共享状态包含一个deferred函数。
// std::future::wait_until()与std::future::wait()的功能类似，即等待与该 std::future 对象相关联的共享状态的标志变为ready
// 而与std::future::wait()不同的是，wait_until()可以设置一个系统绝对时间点abs_time，如果共享状态的标志在该时间点到来之前没有被Provider设置为ready，则调用
// wait_until的线程被阻塞，在abs_time这一时刻到来之后wait_for()返回，返回值如下：
// future_status::ready 	共享状态的标志已经变为ready，即Provider在共享状态上设置了值或者异常。
// future_status::timeout 	超时，即在规定的时间内共享状态的标志没有变为ready。
// future_status::deferred 	共享状态包含一个deferred函数。

// std::shared_future与std::future类似，但是std::shared_future可以拷贝、多个std::shared_future可以共享某个共享状态的最终结果(即共享状态的某个值或者异常)。
// shared_future可以通过某个std::future对象隐式转换（std::shared_future的构造函数），或者通过std::future::share()显示转换，无论哪种转换，被转换的那个
// std::future对象都会变为not-valid。std::shared_future的成员函数和std::future大部分相同
// std::shared_future其他成员函数
//  operator=赋值操作符，与std::future的赋值操作不同，std::shared_future除了支持move赋值操作外，还支持普通的赋值操作。
//  get获取与该std::shared_future对象相关联的共享状态的值（或者异常）。
//  valid有效性检查。
//  wait等待与该std::shared_future对象相关联的共享状态的标志变为ready。
//  wait_for等待与该std::shared_future对象相关联的共享状态的标志变为ready。（等待一段时间，超过该时间段wait_for返回。）
//  wait_until等待与该std::shared_future对象相关联的共享状态的标志变为ready。（在某一时刻前等待，超过该时刻wait_until返回。）
// std::future_error继承子 C++标准异常体系中的logic_error

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

// promise类
//  默认构造函数，初始化一个空的共享状态。
//  带自定义内存分配器的构造函数，与默认构造函数类似，但是使用自定义分配器来分配共享状态。
//  拷贝构造函数，被禁用。
//  移动构造函数。
// 另外，std::promise的operator=没有拷贝语义，即std::promise普通的赋值操作被禁用，operator=只有move语义，所以std::promise对象是禁止拷贝的。
// promise对象可以保存某一类型T的值，该值可被future对象读取（可能在另外一个线程中），因此promise也提供了一种线程同步的手段。
// 在promise对象构造时可以和一个共享状态（通常）相关联，并可以在相关联的共享状态上保存一个类型为T的值。
// std::promise::get_future函数返回一个与promise共享状态相关联的future。返回的future对象可以访问由promise对象设置在共享状态上的值或者某个异常对象。
// 只能从promise共享状态获取一个future对象。在调用该函数之后，promise对象通常会在某个时间点准备好(设置一个值或者一个异常对象)，如果不设置值或者异常，promise对象在析构时
// 会自动地设置一个future_error异常(broken_promise)来设置其自身的准备状态。
// std::promise::set_value函数设置共享状态的值，此后promise的共享状态标志变为ready.
// std::promise::set_value_at_thread_exit设置共享状态的值，但是不将共享状态的标志设置为ready，当线程退出时该promise对象会自动设置为ready。如果某个std::future对象
// 与该promise对象的共享状态相关联，并且该future正在调用get，则调用get的线程会被阻塞，当线程退出时，调用future::get的线程解除阻塞，同时get返回set_value_at_thread_exit
// 所设置的值。注意，该函数已经设置了promise共享状态的值，如果在线程结束之前有其他设置或者修改共享状态的值的操作，则会抛出future_error(promise_already_satisfied)
// std::promise::swap交换promise的共享状态
// future对象可以异步返回共享状态的值，或者在必要的情况下阻塞调用者并等待共享状态标志变为ready,然后才能获取共享状态的值
void test01() {
  promise<int> p;                  // 生成一个promise对象
  future<int> f = p.get_future();  // 和future关联
  auto lam = [](future<int>& f) {
    int x = f.get();  // 获取共享状态的值
    cout << "value = " << x << "\n";
  };
  thread t(lam, std::ref(f));
  p.set_value(10);  // 设置共享状态的值，此处和线程t保持同步
  t.join();
}
// 如果是把promise对象赋值给一个新的promise对象，线程入口函数就不能有参数
promise<int> p;
void test02() {
  auto lam = []() {
    future<int> f = p.get_future();
    int x = f.get();
    cout << "value = " << x << endl;
  };
  thread t1(lam);
  p.set_value(10);
  t1.join();
  p = promise<int>();  // p被move赋值给一个新的promise对象
  thread t2(lam);
  p.set_value(20);
  t2.join();
}

// set_exception设置异常
// void get(promise<int>& p) {
//   int x;
//   cout << "please,enter an integer value = ";
//   cin.exceptions(ios::failbit);  //throw on failbit
//   try {
//     cin >> x;
//     p.set_value(x);
//   } catch (exception&) {
//     p.set_exception(current_exception());
//   }
// }
// void print(future<int>& f) {
//   try {
//     int x = f.get();
//     cout << "value = " << x << endl;
//   } catch (exception& e) {
//     cout << "[exception caught >> " << e.what() << "]\n";
//   }
// }
// void test03() {
//   promise<int> p;
//   future<int> f = p.get_future();

//   thread t1(get, std::ref(p));
//   thread t2(print, std::ref(f));

//   t1.join();
//   t2.join();
// }

// packaged_task类
// 几种构造函数的语义：
//   默认构造函数，初始化一个空的共享状态，并且该 packaged_task 对象无包装任务。
//   初始化一个共享状态，并且被包装任务由参数 fn 指定。
//   带自定义内存分配器的构造函数，与默认构造函数类似，但是使用自定义分配器来分配共享状态。
//   拷贝构造函数，被禁用。
//   移动构造函数。
// 与std::promise类似，std::packaged_task也禁用了普通的赋值操作运算，只允许move赋值运算。
// packaged_task对象内部包含两个基本元素：
// 1 包装的任务，即一个可调用对象
// 2 共享状态，用于保存任务的返回值。可以通过future对象来达到异步访问共享状态的效果
// packaged_task是对一个任务的抽象，我们可以给其传递一个函数来完成其构造。相较于promise，它应该算是更高层次的一个抽象了吧，同样地，我们可以将任务投递给任何线程去完成，
// 然后通过packaged_task::get_future()方法获取的future对象来获取任务完成后的产出值。总结来说，packaged_task是连数据操作创建都封装进去了的promise。packaged_task
// 也是一个类模板，模板参数为函数签名，也就是传递函数的类型。
// packaged_task::valid()可以帮忙检查当前packaged_task是否处于一个有效的共享状态，对于由默认构造函数生成的packaged_task对象，该函数返回false，除非中间进行了move()
// 赋值操作或者swap()操作。另外我们也可以通过reset()来重置其共享状态。
// std::packaged_task::get_future返回一个与packaged_task对象共享状态相关的future对象。返回的future对象可以获得由另外一个线程在该packaged_task对象的共享状态上设置
// 的某个值或者异常。
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
  //   std::future_status status;
  //   do {
  //     status = future.wait_for(std::chrono::seconds(1));
  //     if (status == std::future_status::deferred) {
  //       std::cout << "deferredn";
  //     } else if (status == std::future_status::timeout) {
  //       std::cout << "timeoutn";
  //     } else if (status == std::future_status::ready) {
  //       std::cout << "ready!n";
  //     }
  //   } while (status != std::future_status::ready);

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

  test01();  // value = 10

  test02();
  // value = 10
  // value = 20

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