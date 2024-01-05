#include <chrono>
#include <future>
#include <iostream>

// 条款36：如果异步是必要的，则指定std::launch::async

namespace n1 {
void f() {}

// 当调用std::async来执行函数或可调用对象时，基本是上都会想让函数以异步方式运行。
// 但仅仅通过std::async来运行，实际上要求的并非一定会达成异步运行的结果，
// 要求的仅仅是让该函数以符合std::async的启动策略来运行。
// 标准策略有二，它们都是用限定作用域的枚举类型std::launch中的枚举量来表示的。
// 假设函数f要传递给std::async以执行，则：
// 1.std::launch::async启动策略意味着函数f必须以异步方式运行，
//   亦即，在另一线程之上执行；
// 2.std::launch::deferred启动策略意味着，
//   函数f只会在std::async所返回的期望值的get或wait得到调用时才运行，
//   当调用get或wait时，f会同步运行，调用方会阻塞至f运行结束为止，
//   如果get或wait都没有得到调用，f是不会运行的。

void func() {
  // 也许有点出人意料，std::aysnc的默认启动策略，也就是如果不积极指定一个的话，
  // 它采用的并非以上两者中的一者，相反地，它采用的是对二者进行或运算的结果。
  // 下面的两个调用有着完全相同的意义：
  auto fut1 = std::async(f);
  auto fut2 = std::async(std::launch::async | std::launch::deferred, f);

  // 这么一来，默认启动策略就允许f以异步或同步的方式运行皆可，正如条款35所指出的那样，
  // 这种弹性使得async与标准库的线程管理组件能够承担得起线程的创建和销毁、避免超订，
  // 以及负载均衡的责任，这也是使用async来做并发程序设计如此方便的诸多因素中的一部分。

  auto fut = std::async(f);  // 在线程t中
  // 但以默认启动策略来使用std::async，会触及一些意味深长的暗流：
  // 1.无法预知f是否会和t并发运行，因为f可能会被调度为推迟运行；
  // 2.无法预知f是否运行在与调用fut的get或wait函数的线程不同的某线程之上，
  //   如果那个线程是t，那就是说无法预知f是否会运行在于t不同的某线程之上；
  // 3.连f是否会运行这件起码的事情都是无法预知的，
  //   这是因为无法保证在程序的每条路径上，fut的get和wait都会得到调用。
}
}  // namespace n1

namespace n2 {
// 默认启动策略在调度上的弹性常会在使用thread_local变量时导致不明不白的混淆，
// 因为这意味着如果f读或写此线程级局部存储时，无法预知会取到的是哪个线程的局部存储。
}  // namespace n2

namespace n3 {
void f() { std::this_thread::sleep_for(std::chrono::seconds(1)); }

void func1() {
  auto fut = std::async(f);

  // 默认启动策略也会影响那些基于wati的循环中以超时为条件者，
  // 因为对任务调用wait_for或者wait_until会产生std::launch::deferred一值。
  // 这意味着以下循环虽然貌似会最终停止，但是，实际上，可能会永远运行下去：
  while (fut.wait_for(std::chrono::microseconds(200)) !=
         std::future_status::ready) {
    // 循环至f完成，但可能永远不能退出循环
  }

  // 如果f与调用async的线程是并发执行的，即选用了std::launch::async启动策略，
  // 这就没有问题，但如果f被推迟执行，则wait_for将总返回deferred。
  // 而那永远也不会取值std::future_status::ready，所以循环也就永远不会终止。

  // 这一类缺陷在开发或测试单元测试中很容易被忽略，因为它只会在运行负载很重时才会现身。
  // 这样的负载是把计算机逼向超订或线程耗尽的条件，而那就是任务很可能会被推迟的机会了。
  // 毕竟，如果硬件层面没有面临超订或者线程耗尽的威胁，
  // 运行期系统并无理由不去调度任务以并发方式执行。
}

void func2() {
  // 修正这个缺陷并不难：校验std::async返回的期值，确定任务是否被推迟，
  // 然后如果确定被推迟了，则避免进入基于超时的循环。
  // 不幸的是，没有直接的办法来询问期值任务是否被推迟了。
  // 作为替代手法，必须先调用一个基于超时的函数，例如wait_for，在此情况下，
  // 其实并不是要等待任何事情，而是要看返回值是否为std::future_status::deferred，
  // 所以请搁置怀疑，径自调用一个超时为零的wait_for即可：
  auto fut = std::async(f);  //
  if (fut.wait_for(std::chrono::seconds(0)) == std::future_status::deferred) {
    // 则使用fut的wait或get以异步方式调用f
  } else {  // 任务未被推迟
    while (fut.wait_for(std::chrono::microseconds(100)) !=
           std::future_status::ready) {  // 不可能死循环
      // 任务既未被推迟，也未就绪，则做并发工作，直至任务就绪
    }
  }
}

// 以默认启动策略对任务使用std::async能正常工作需要满足以下所有条件：
// 1.任务不需要与调用get或wait的线程并发执行；
// 2.读/写哪个线程的thread_local变量并无影响；
// 3.或者可以给出保证在std::async返回的期望值上调用get或wait，
//   或者可以接受任务可能永不执行；
// 4.使用wait_for或wait_unitil的代码会将任务被推迟的可能性纳入考量。

// 只要其中一个条件不满足，就很有可能想要确保任务以异步方式执行。
// 实现这一点的手法，就是在调用时把std::launch::async作为第一个实参传递：
// auto fut = std::async(std::launch::async, f);  // 以异步方式启动f
}  // namespace n3

namespace n4 {
// 如果能像async那样运行，并自动使用launch::aysnc为启动策略，那将是个趁手的工具：
#if __cplusplus == 201402L
template <typename F, typename... Ts>
inline auto reallyAsync(F&& f, Ts&&... params) {
  return std::async(std::launch::async, std::forward<F>(f),
                    std::forward<Ts>(params)...);
}
// reallyAsync所做的一起，就是以std::launch::asyn启动策略来调整了std::async。
#elif __cplusplus == 201103L
template <typename F, typename... Ts>
inline std::future<typename std::result_of<F(Ts...)>::type> reallyAsync(
    F&& f, Ts&&... params) {
  return std::async(std::launch::async, std::forward<F>(f),
                    std::forward<Ts>(params)...);
}
// 该函数接受一个可调用对象，以及零个或多个形参params，
// 并将后者完美转发给std::async，同时传递std::launch::async作为启动策略。
// 就像std::async,它会返回一个类型为std::future的对象作为使用params调用f的结果。
// 决定该结果的类型很容易，std::result_of这个类型特征就会把结果调用者。
#endif

void f() {}
void func() { auto fut = reallyAsync(f); }
}  // namespace n4

// 要记住的东西：
// 1.std::async的默认启动策略既允许任务以异步方式执行，也允许任务以同步方式执行；
// 2.如此的弹性会导致使用thread_local变量时的不确定性，
//   隐含着任务可能永远不会执行，还会影响运用了基于超时的wait调用的程序逻辑；
// 3.如果异步是必要的，则指定std::launch::async。

int main() { return 0; }