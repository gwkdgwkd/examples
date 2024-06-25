#include <future>
#include <iostream>
#include <thread>
#include <vector>

// 条款37：使std::thread类型对象在所有路径皆不可联结

// 每个std::thread类型对象皆处于两种状态之一：可联结或不可联结。
// 可联结的std::thread对应底层以异步方式已运行或可运行的线程。
// std::thread类型对象对应的底层线程若处于阻塞或等待调度，则它可联结。
// std::thread类型对象对应的底层线程如已运行至结束，则亦认为其可联结。

// 不可联结的std::thread类型对象包括：
// 1.默认构造的std::thread，此类std::thread没有可以执行的函数，
//   因此也没有对应的底层执行线程；
// 2.已移动的std::thread，移动操作的结果是，
//   一个std::thread所对应的底层执行线程（若有）被对应到另外一个std::thread；
// 3.已联结的std::thread，联结后，
//   std::thread类型对象不再对应至已结束运行的底层执行线程；
// 4.已分离的std::thread，
//   分离操作会把std::thread类型对象和它对应的底层执行线程之间的连接段断开。

#if __cplusplus == 201402L
constexpr auto tenMillion = 10'000'000;
#elif __cplusplus == 201103L
constexpr auto tenMillion = 10000000;
#endif
bool conditionsAreSatisfied() { return true; }
void performComputation(std::vector<int>) {}

namespace n1 {
// std::thread的可联结性之所以重要的原因之一是：
// 如果可联结的线程对象的析构函数被调用，则程序的执行就终止了。

// 举个例子，假设有一个函数doWork，它接受筛选器函数filter和最大值maxVal作为形参。
// doWork会校验计算的条件是否成立，尔后会针对筛选器选出的0到maxVal的值实施计算。
// 如果筛选是费时的，而条件检验也是费时的，那么并发地做这两件事就是合理的。

// 优先选用使用基于任务的设计，见条款35，但是让假定会去设置实施筛选的那个线程的优先级。
// 条款35解释过，这要求使用线程的低级句柄，从而只能通过std::thread的API来访问。
// 基于任务的API没有提供这个功能，因此，唯有采用基于线程一途，基于任务在此不可行。

bool doWork(std::function<bool(int)> filters, int maxVal = tenMillion) {
  std::vector<int> goodVals;

  std::thread t([&filters, maxVal, &goodVals] {
    for (auto i = 0; i < maxVal; ++i) {
      if (filters(i)) {
        goodVals.push_back(i);
      }
    }
  });

  // 更好设计是以暂停状态启动线程t，而不是线程t开始执行之后才去设置它的优先级，
  // 因为可以在它执行计算之前调整其优先级。
  auto nh = t.native_handle();
  if (conditionsAreSatisfied()) {
    t.join();
    performComputation(goodVals);
    return true;
  }

  return false;

  // 如果conditionsAreSatisfied返回true则没问题，但如果返回false或抛出了异常，
  // 那么在doWork的末尾调用std::thread类型对象t的析构函数时，
  // 它会是处于可联结状态，从而导致程序执行终止。

  // std::thread的析构函数为何这样运作，那是因为，另外两种明显的选项会更糟糕：
  // 1.隐式join，在这种情况下，std::thread的析构函数会等待底层异步执行线程完成，
  //   这听上去合理，但却可能导致难以追踪的性能异常，例如，
  //   如果conditionAreSatisfied()早已返回false，
  //   doWork却还在等待所有值上遍历筛选，这是违反直觉的；
  // 2.隐式detach，在这种情况下，
  //   std::thread的析构函数会分离std::thread类型对象与底层执行线程之间的连接。
  //   而该底层执行线程会继续执行，这听起来和join途径相比在合理性方面并不逊色，
  //   但它导致的调试问题会更加要命，例如，在doWork内goodVals是个通过引用捕获的局部变量，
  //   它也会在lambda式内被修改（通过对push_back的调用），
  //   然后，假如lambda式以异步方式运行时，conditionsAreSatisfied()返回了false，
  //   那种情况下，doWork会直接返回，它的局部变量（包括goodVals）会被销毁，
  //   doWork的栈帧会被弹出，可是线程却仍然在doWork的调用方继续运行着，
  //   在doWork调用方此后的语句中，在某个时刻，会调用其他函数，
  //   而至少会有一个函数可能会使用一部分或者全部doWork栈帧占用过的内存，
  //   不妨把这个函数称为f，当f运行时，doWork发起的lambda式依然在异步执行，
  //   该lambda式在原先的栈上对goodVals调用push_back，不过那已是在f的栈帧中了，
  //   这样的调用会修改过去属于goodVals的内存，而意味着从f的视角看，
  //   栈帧上的内存内容会莫名奇妙地改变。

  // 标准委员会注意到，销毁一个可联结的线程实在太过可怕，
  // 所以实际上已经封印了这件事，通过规定可联结的线程的析构函数导致程序终止。

  // 这么一来，黑锅就甩给了程序员，如果使用了std::thread类型对象，
  // 就得确保从它定义的作用域出去的任何路径，都是不可联结状态，但是覆盖所有路径是复杂的，
  // 这包括正常走完作用域，还有经由return，continue，break，goto或异常跳出作用域。
}
}  // namespace n1

namespace n2 {
// 想在每条路径上都执行某动作，最常用的方法就是在局部对象的析构函数中执行该动作。
// 这样的对象称为RAII对象，它们来自RAII类，资源获取即初始化，
// 即使该技术的关键其实在于析构而非初始化。
// RAII类在标准库中很常见，例如STL容器在析构函数都会析构容器内容并释放其内存、
// 标准智能指针、std::fstream类型对象（其析构函数会关闭对应的文件），还有很多。
// 然而，没有和std::thread类型对象对应的标准RAII类，
// 可能是因为标准委员会把join或detach用作默认选项的途径都堵死了，
// 这么一来也就不知道真有这样的类的话该如何运作。

// 自己写一个不难，允许调用者指定ThreadRAII类型对象销毁时是调用join还是detach：
class ThreadRAII {
 public:
  enum class DtorAction { detach, join };

  ThreadRAII(std::thread&& thread, DtorAction action)
      : m_action(action), m_thread(std::move(thread)) {}

  ~ThreadRAII() {
    // 在m_thread.joinable()的执行和join或detach的调用之间，
    // 另一个线程可能让m_therad变得不可联结，直觉可圈可点，但不用担心。
    // 一个std::thread类型对象只能通过调用成员函数以从可联结状态转换为不可联结状态，
    // 例如join、detach或移动操作，当ThreadRAII对象的析构函数被调用时，
    // 不应该有其他线程调用该对象的成员函数。
    // 如果同时发生多个调用，那的确会有竞险，但这个竞险不是发生在析构函数内，
    // 而是发生在试图同时调用两个成员函数（析构函数和其他成员函数）的用户代码内。
    // 一般地，对对象同时调用多个成员函数，只有当所有这些函数都是const成员函数时才安全。

    if (m_thread.joinable()) {
      if (m_action == DtorAction::join) {
        m_thread.join();
      } else {
        m_thread.detach();
      }
    }
  }

  std::thread& getThread() { return m_thread; }

  // 因为ThreadRAII声明了析构函数，所以不会有编译器生成的移动操作，
  // 但这里ThreadRAII对象没有理由实现为不可移动的：
  ThreadRAII(ThreadRAII&&) = default;
  ThreadRAII& operator=(ThreadRAII&&) = default;

 private:
  DtorAction m_action;
  std::thread m_thread;

  // 这段代码基本上不言自明，但指出以下几点可能会有帮助：
  // 1.std::thread类型对象是不可复制的，所以构造函数只接受右值类型的std::thread，
  //   目的是要把传入的std::thread类型对象移入ThreadRAII对象；
  // 2.读/写哪个线程的thread_local变量并无影响，
  //   或者可以给出保证在async返回的期值上调用get或wait，或者可以接受任务可能永不执行。
  //   使用wait_for或wait_unitil的代码会将任务被推迟的可能性纳入考量；
  // 3.ThreadRAII提供了一个get函数，用以访问底层的std::thread类型对象，
  //   这和标准智能指针提供的get函数一样（后者用以访问底层裸指针），
  //   提供get可以避免让ThreadRAII去重复std::thread的所有接口，
  //   也意味着ThreadRAII类型对象可以用于需要直接使用std::thread类型对象的语境；
  // 4.ThreadRAII的析构函数在调用std::thread类型对象t的成员函数之前会先实施校验，
  //   以确保m_treahd可联结，这是必要的，
  //   因为针对一个不可联结的线程调用join或detach会产生未定义行为，
  //   用户有可能会构建了一个std::thread类型对象，然后从它出发创建一个ThreadRAII类型对象，
  //   再使用get访问m_thread，接着针对m_thread实施移动或是对m_thread调用join或detach，
  //   而这样的行为会使m_thread变得不可联结。
};

bool doWork(std::function<bool(int)> filters, int maxVal = tenMillion) {
  std::vector<int> goodVals;

  ThreadRAII t(std::thread([&filters, maxVal, &goodVals] {
                 for (auto i = 0; i < maxVal; ++i) {
                   if (filters(i)) goodVals.push_back(i);
                 }
               }),
               ThreadRAII::DtorAction::join);
  // 选择在ThreadRAII析构函数中对异步执行线程调用join。
  // 因为之前已经看到了，调用detach函数会导致噩梦般的调试。
  // 之前也看到过join会导致性能异常，但detach导致的未定义行为、
  // 可在std::thread产生的程序终止和性能异常之间做出选择，
  // 性能异常也是权衡之下选取的弊端最小的一个。

  auto nh = t.getThread().native_handle();
  if (conditionsAreSatisfied()) {
    performComputation(goodVals);
    return true;
  }

  return false;
}
}  // namespace n2

// 要记住的东西：
// 1.使std::thread类型对象在所有路径皆不可联结；
// 2.在析构调用join可能导致难以调试的性能异常；
// 3.在析构时调用detch可能导致难以调试的未定义行为；
// 4.在成员列表的最后声明std::thread类型对象。

int main() { return 0; }