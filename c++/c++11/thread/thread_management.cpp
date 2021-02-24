#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

// C++11标准库扩展了：包含了用于管理线程、保护共享数据、线程间同步操作，以及低层原子操作的各种类。
// C++14中为并发和并行添加了一个新的互斥量类型，用于保护共享数据
// 在C++17就考虑的更多了：一开始就添加了一整套的并行算法
// C++线程库很大程度上，是基于上文提到的C++类库的经验积累。特别是，Boost线程库作为新类库的主要模型，很多类与Boost库中的相关类有着相同名称和结构。

// 为了效率，C++类整合了一些底层工具。这样就需要了解相关使用高级工具和使用低级工具的开销差，这个开销差就是抽象代价(abstraction penalty)。
// 绝大多数情况下，额外增加的复杂性和出错几率都远大于性能的小幅提升带来的收益。
// 即便是有证据确实表明瓶颈出现在C++标准库的工具中，也可能会归咎于低劣的应用设计，而非类库实现。
// C++标准库没有提供所需的性能或行为时，就需要使用与平台相关的工具。

// 每个程序至少有一个线程：执行main()函数的线程，其余线程有其各自的入口函数。线程与原始线程(以main()为入口函数的线程)同时运行。
// 如同main()函数执行完会退出一样，当线程执行完入口函数后，线程也会退出。在为一个线程创建了一个std::thread对象后，需要等待这个线程结束

// 使用C++线程库启动线程，可以归结为构造std::thread对象。
class background_task {
 public:
  // 将带有函数调用符类型的实例传入std::thread类中，替换默认的构造函数。
  void operator()() const { cout << "Constructors " << endl; }
};

// 启动了线程，你需要明确是要等待线程结束，还是让其自主运行。如果std::thread对象销毁之前还没有做出决定，程序就会终止(std::thread的析构函数会调用
// std::terminate())。因此，即便是有异常存在，也需要确保线程能够正确的加入(joined)或分离(detached)。
// 需要注意的是，必须在std::thread对象销毁之前做出决定，否则你的程序将会终止(std::thread的析构函数会调用std::terminate()，这时再去决定会触发相应异常)。
// 如果不等待线程，就必须保证线程结束之前，可访问的数据得有效性。这不是一个新问题——单线程代码中，对象销毁之后再去访问，也会产生未定义行为——不过，线程的生命
// 周期增加了这个问题发生的几率。这种情况很可能发生在线程还没结束，函数已经退出的时候，这时线程函数还持有函数局部变量的指针或引用。
void do_something(int i) { cout << i << endl; }
struct func {
  int& i;
  func(int& i_) : i(i_) {}
  void operator()() {
    for (unsigned j = 0; j < 1000000; ++j) {
      do_something(i);  // 潜在访问隐患：悬空引用
    }
  }
};
// 处理这种情况的常规方法：使线程函数的功能齐全，将数据复制到线程中，而非复制到共享数据中。如果使用一个可调用的对象作为线程函数，这个对象就会复制到线程中，
// 而后原始对象就会立即销毁。但对于对象中包含的指针和引用还需谨慎
// 使用一个能访问局部变量的函数去创建线程是一个糟糕的主意(除非十分确定线程会在函数完成前结束)。

// 如果需要等待线程，相关的std::thread实例需要使用join()。将detach替换为join，就可以确保局部变量在线程完成后，才被销毁。
// 在这种情况下，因为原始线程在其生命周期中并没有做什么事，使得用一个独立的线程去执行函数变得收益甚微，但在实际编程中，原始线程要么有自己的工作要做；
// 要么会启动多个子线程来做一些有用的工作，并等待这些线程结束。
// join()是简单粗暴的等待线程完成或不等待。当你需要对等待中的线程有更灵活的控制时，比如，看一下某个线程是否结束，或者只等待一段时间(超过时间就判定为超时)。
// 想要做到这些，你需要使用其他机制来完成，比如条件变量和期待(futures)
// 调用join()的行为，还清理了线程相关的存储部分，这样std::thread对象将不再与已经完成的线程有任何关联。这意味着，只能对一个线程使用一次join();
// 一旦已经使用过join()，std::thread对象就不能再次加入了，当对其使用joinable()时，将返回false。

// 如果想要分离一个线程，可以在线程启动后，直接使用detach()进行分离。如果打算等待对应线程，则需要细心挑选调用join()的位置。当在线程运行之后产生异常，
// 在join()调用之前抛出，就意味着这次调用会被跳过。
// 避免应用被抛出的异常所终止，就需要作出一个决定。通常，当倾向于在无异常的情况下使用join()时，需要在异常处理过程中调用join()，从而避免生命周期的问题。
// 等待线程完成
void f1() {
  int some_local_state = 0;
  func my_func(some_local_state);
  std::thread t(my_func);
  try {
    // do_something_in_current_thread();
  } catch (...) {
    t.join();
    throw;
  }
  t.join();
}
// 如需确保线程在函数之前结束:使用“资源获取即初始化方式”(RAII，Resource Acquisition Is Initialization)，并且提供一个类，在析构函数中使用join()
class thread_guard {
  std::thread& t;

 public:
  explicit thread_guard(std::thread& t_) : t(t_) {}
  ~thread_guard() {
    if (t.joinable()) {
      t.join();
    }
  }
  // 拷贝构造函数和拷贝赋值操作被标记为=delete，是为了不让编译器自动生成它们。直接对一个对象进行拷贝或赋值是危险的，因为这可能会弄丢已经加入的线程。
  thread_guard(thread_guard const&) = delete;
  thread_guard& operator=(thread_guard const&) = delete;
};
void f2() {
  int some_local_state = 0;
  func my_func(some_local_state);
  std::thread t(my_func);
  thread_guard g(t);
  // do_something_in_current_thread();
}
// f2退出时局部对象就要被逆序销毁了。因此，thread_guard对象g是第一个被销毁的，这时线程在析构函数中被加入到原始线程中。
// 即使do_something_in_current_thread抛出一个异常，这个销毁依旧会发生。

// 如果不想等待线程结束，可以分离_(_detaching)线程，从而避免异常安全(exception-safety)问题。不过，这就打破了线程与std::thread对象的联系，即使线程仍然
// 在后台运行着，分离操作也能确保std::terminate()在std::thread对象销毁才被调用。
// 使用detach()会让线程在后台运行，这就意味着主线程不能与之产生直接交互。也就是说，不会等待这个线程结束；如果线程分离，那么就不可能有std::thread对象能引用它，
// 分离线程的确在后台运行，所以分离线程不能被加入。不过C++运行库保证，当线程退出时，相关资源的能够正确回收，后台线程的归属和控制C++运行库都会处理。
// 通常称分离线程为守护线程(daemon threads)，UNIX中守护线程是指，没有任何显式的用户接口，并在后台运行的线程。这种线程的特点就是长时间运行；线程的生命周期可能
// 会从某一个应用起始到结束，可能会在后台监视文件系统，还有可能对缓存进行清理，亦或对数据结构进行优化。另一方面，分离线程的另一方面只能确定线程什么时候结束，发后
// 即忘(fire and forget)的任务就使用到线程的这种方式。
void do_background_work() { cout << "do_background_work" << endl; }
// detach()要用join()同样的方式进行检查——当std::thread对象使用joinable()返回的是true，就可以使用detach()。

// 默认参数要拷贝到线程独立内存中，即使参数是引用的形式，也可以在新线程中进行访问。
void f3(int i, string const& str) { cout << i << " " << str << endl; }

// 期望传递一个非常量引用(但这不会被编译)，但整个对象被复制了。
// 虽然f4的参数期待传入一个引用，但是std::thread的构造函数并不知晓；构造函数无视函数期待的参数类型，并盲目的拷贝已提供的变量。
void f4(int& i) { ++i; }

// std::thread构造函数和std::bind的操作都在标准库中定义好了，可以传递一个成员函数指针作为线程函数，并提供一个合适的对象指针作为第一个参数
class X {
 public:
  void do_lengthy_work(int i) { cout << i << endl; };
};

// 提供的参数可以移动，但不能拷贝。”移动”是指:原始对象中的数据转移给另一对象，而转移的这些数据就不再在原始对象中保存了
// std::unique_ptr就是这样一种类型()，这种类型为动态分配的对象提供内存自动管理机制(类似垃圾回收)。同一时间内，只允许一个std::unique_ptr实现指向一个给定对象，
// 并且当这个实现销毁时，指向的对象也将被删除。
// 移动构造函数(move constructor)和移动赋值操作符(move assignment operator)允许一个对象在多个std::unique_ptr实现中传递。
// 使用”移动”转移原对象后，就会留下一个空指针(NULL)。移动操作可以将对象转换成可接受的类型，例如:函数参数或函数返回的类型。
// 当原对象是一个临时变量时，自动进行移动操作，但当原对象是一个命名变量，那么转移的时候就需要使用std::move()进行显示移动。
// class big_object {
//  public:
//   big_object(int&& i){};

//  private:
//   int i;
// };
// void process_big_object(big_object*) {}

// c++标准线程库中和std::unique_ptr在所属权上有相似语义类型的类有好几种，std::thread为其中之一。虽然，std::thread实例不像std::unique_ptr那样能占有一个动
// 态对象的所有权，但是它能占有其他资源：每个实例都负责管理一个执行线程。执行线程的所有权可以在多个std::thread实例中互相转移，这是依赖于std::thread实例的可移动
// 且不可复制性。不可复制保性证了在同一时间点，一个std::thread实例只能关联一个执行线程；可移动性使得开发者可以自己决定，哪个实例拥有实际执行线程的所有权。

// C++标准库中有很多资源占有(resource-owning)类型，比如std::ifstream，std::unique_ptr还有std::thread都是可移动，但不可拷贝。
// 这就说明执行线程的所有权可以在std::thread实例中移动
void some_function(){};
void some_other_function(){};
// std::thread支持移动，就意味着线程的所有权可以在函数外进行转移
void some_other_function1(int i){};
std::thread h() { return std::thread(some_function); }
std::thread g() {
  std::thread t(some_other_function1, 42);
  return t;
}
// 当所有权可以在函数内部传递，就允许std::thread实例可作为参数进行传递
void j(std::thread t) { t.join(); };

// std::thread支持移动的好处是可以创建thread_guard类的实例，并且拥有其线程所有权。当thread_guard对象所持有的线程被引用时，移动操作就可以避免很多不必要的麻烦；
// 这意味着，当某个对象转移了线程的所有权后，它就不能对线程进行加入或分离。
class scoped_thread {
  std::thread t;

 public:
  // thread_guard类，需要在析构中检查线程是否”可加入”。这里把检查放在了构造函数中，并且当线程不可加入时，抛出异常。
  explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
    if (!t.joinable()) throw std::logic_error("No thread");
  }
  // 当主线程到达函数末尾时，scoped_thread对象就会销毁，然后加入到的构造函数创建的线程对象中去。
  ~scoped_thread() { t.join(); }
  scoped_thread(scoped_thread const&) = delete;
  scoped_thread& operator=(scoped_thread const&) = delete;
};

// std::thread对象的容器，如果这个容器是移动敏感的(比如，标准中的std::vector<>)，那么移动操作同样适用于这些容器
void do_work(unsigned id){};
void f() {
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < 20; ++i) {
    threads.push_back(std::thread(do_work, i));  // 产生线程
  }
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));  // 对每个线程调用join()
}
// 将std::thread放入std::vector是向线程自动化管理迈出的第一步：并非为这些线程创建独立的变量，并且直接加入，而是把它们当做一个组

// 运行时决定线程数量
// std::thread::hardware_concurrency()在新版C++标准库中是一个很有用的函数。这个函数会返回能并发在一个程序中的线程数量。
// 多核系统中，返回值可以是CPU核芯的数量。返回值也仅仅是一个提示，当系统信息无法获取时，函数也会返回0。但是，这也无法掩盖这个函数对启动线程数量的帮助。
// 因为上下文频繁的切换会降低线程的性能，所以你肯定不想启动的线程数多于硬件支持的线程数量。当std::thread::hardware_concurrency()返回0，自己设置一个。

// 标识线程
// 线程标识类型为std::thread::id，并可以通过两种方式进行检索:
// 第一种，可以通过调用std::thread对象的成员函数get_id()来直接获取。 如果std::thread对象没有与任何执行线程相关联，get_id()将返回std::thread::type
// 默认构造值，这个值表示“无线程”。
// 第二种，当前线程中调用std::this_thread::get_id()(这个函数定义在<thread>头文件中)也可以获得线程标识。
// std::thread::id对象可以自由的拷贝和对比,因为标识符就可以复用。如果两个对象的std::thread::id相等，那它们就是同一个线程，或者都“无线程”。如果不等，
// 那么就代表了两个不同线程，或者一个有线程，另一没有线程。
// C++线程库不会限制你去检查线程标识是否一样，std::thread::id类型对象提供相当丰富的对比操作；比如，提供为不同的值进行排序。这意味着允许程序员将其当做为容器
// 的键值，做排序，或做其他方式的比较。按默认顺序比较不同值的std::thread::id，所以这个行为可预见的：当a<b，b<c时，得a<c，等等。
// 标准库也提供std::hash<std::thread::id>容器，所以std::thread::id也可以作为无序容器的键值。
// std::thread::id可以作为一个线程的通用标识符，当标识符只与语义相关(比如，数组的索引)时，就需要这个方案了。
// 也可以使用输出流(std::cout)来记录一个std::thread::id对象的值。
// 具体的输出结果是严格依赖于具体实现的，C++标准的唯一要求就是要保证ID比较结果相等的线程，必须有相同的输出。
void id() { cout << "get id in thread: " << this_thread::get_id() << endl; };

int main() {
  background_task f;  // 在前面命名函数对象的方式
  cout << "before" << endl;
  // 函数对象会复制到新线程的存储空间当中，函数对象的执行和调用都在线程的内存空间中进行。
  thread a(f);
  a.join();
  // before
  // Constructors
  // 当把函数对象传入到线程构造函数中时，需要避免“最令人头痛的语法解析”(C++’s most vexing parse)。如果你传递了一个临时变量，
  // 而不是一个命名的变量；C++编译器会将其解析为函数声明，而不是类型对象的定义。
  // 声明一个名为my_thread1的函数，带有一个参数(函数指针指向没有参数并返回background_task对象的函数)，返回std::thread1对象的函数，而非启动了一个线程。
  std::thread my_thread1(background_task());
  // 下面方式可以避免上面的问题
  std::thread my_thread2((background_task()));  // 使用多组括号
  std::thread my_thread3{background_task()};  // 使用新统一的初始化语法
  my_thread2.join();
  my_thread3.join();
  // 使用lambda表达式也能避免这个问题
  std::thread my_thread4([] { cout << "Constructors " << endl; });
  my_thread4.join();

  // int some_local_state = 0;
  // func my_func(some_local_state);
  // std::thread my_thread(my_func);
  // my_thread.detach();
  // 结果不唯一，最后main退出了，some_local_state销毁。新线程还没退出，打印的i是main中some_local_state的引用，行为未定义。
  // 0
  // 0
  // 0
  // 0
  // 32571

  std::thread b(do_background_work);
  b.detach();
  assert(!b.joinable());

  this_thread::sleep_for(std::chrono::microseconds(1000));
  // 函数f需要一个std::string对象作为第二个参数，但这里使用的是字符串的字面值，也就是char const *类型。之后，在线程的上下文中完成字面值向std::string对象的转化。
  thread c(f3, 5, "nihao");  // 5 nihao
  c.join();
  // 需要特别要注意，当指向动态变量的指针作为参数传递给线程的情况
  char buffer[1024];  // 一个指针变量，指向本地变量，传递到新线程中
  sprintf(buffer, "%s", "some_param");
  // 函数有很有可能会在字面值转化成std::string对象之前崩溃(oops)，从而导致一些未定义的行为
  std::thread d(f3, 3, buffer);  // 3 @w�A
  d.detach();
  // 使用std::string，避免悬垂指针
  std::thread e(f3, 4, std::string(buffer));  // 4 some_param
  e.detach();

  int ii = 5;
  cout << "before : " << ii << endl;  // 5
  // 在代码会将参数以右值的方式进行拷贝传递，这是为了照顾到那些只能进行移动的类型，而后会以右值为参数调用。
  // 因为函数期望的是一个非常量引用作为参数，而非一个右值作为参数，所以会在编译时出错。
  // std::thread g(f4, ii);
  // f4会接收到一个ii变量的引用，而非一个ii变量拷贝的引用，这样代码就能顺利的通过编译。
  std::thread g(f4, std::ref(ii));  // 可以使用std::ref将参数转换成引用的形式
  g.join();
  cout << "after : " << ii << endl;  // 6

  X my_x;
  int num(9);
  std::thread h(&X::do_lengthy_work, &my_x, num);  // 9
  h.join();

  // void process_big_object(std::unique_ptr<big_object>);
  // std::unique_ptr<big_object> p(new big_object);
  // std::thread t(process_big_object, std::move(p));
  // std::thread的构造函数中指定std::move(p)，big_object对象的所有权就被首先转移到新创建线程的的内部存储中，之后传递给process_big_object函数。

  std::thread t1(some_function);
  std::thread t2 = std::move(t1);  // t1和线程已经没有关联，执行线程与t2关联。
  // 一个临时std::thread对象相关的线程启动了。为什么不显式调用std::move()转移所有权呢？因为，所有者是一个临时对象——移动操作将会隐式的调用。
  t1 = std::thread(some_other_function);
  std::thread t3;  // t3使用默认构造方式创建，与任何执行线程都没有关联。
  t3 = std::move(t2);
  // 最后一个移动操作，将some_function线程的所有权转移给t1。不过，t1已经有了一个关联的线程(执行some_other_function的线程)，所以这里系统直接调
  // 用std::terminate()终止程序继续运行。这样做（不抛出异常，std::terminate()是noexcept函数)是为了保证与std::thread的析构函数的行为一致。
  // t1 = std::move(t3);  // 赋值操作将使程序崩溃
  t1.join();
  t3.join();

  j(std::thread(some_function));
  std::thread t(some_function);
  j(std::move(t));

  int some_local_state;
  scoped_thread st(std::thread(func(some_local_state)));  // 不用再写join了

  thread n(id);
  // 具体的输出结果是严格依赖于具体实现的，C++标准的唯一要求就是要保证ID比较结果相等的线程，必须有相同的输出。
  cout << "get id in main: " << n.get_id() << endl;
  n.join();
  // get id in main: 140002670114560
  // get id in thread: 140002670114560

  return 0;
}