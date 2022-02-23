#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

// 原子操作是在多线程程序中“最小的且不可并行化的”操作，意味着多个线程访问同一个资源时，有且仅有一个线程能对资源进行操作。
// 原子操作是一个不可分割的操作，这种操作有个特点，要么做完，要么没做完，在其他线程访问的时候，不能够访问到这种过程的中间态。
// 通常情况下原子操作可以通过互斥的访问方式来保证，例如Linux下的互斥锁（mutex），Windows下的临界区（Critical Section）等。
// 原子类型，是c++11中提供的一类数据类型，这种数据类型，不需要显式使用互斥锁。但是，编译器却可以保证原子类型在线程间被互斥地访问。
// 可以理解为：c++11将原子类型的互斥锁、临界区给抽象化了，而不需要再去进行相关操作（平台相关）。

namespace whyatomic {
int64_t total1 = 0;
void func1() {
  // 没有锁，会得出一个小于10000100000随机的错误值
  for (int64_t i = 1; i <= 100000; ++i) {
    total1 += i;
  }
}

// 使用锁实现原子操作，pthread线程库的锁也可以实现
int64_t total2 = 0;
std::mutex m;
void func2() {
  for (int64_t i = 1; i <= 100000; ++i) {
    m.lock();
    total2 += i;
    m.unlock();
  }
}

// 在C++11之前，使用第三方API可以实现并行编程，比如pthread多线程库，但是在使用时需要创建互斥锁，
// 以及进行加锁、解锁等操作来保证多线程对临界资源的原子操作，这无疑增加了开发的工作量。
// 从C++11开始，C++从语言层面开始支持并行编程，内容包括了管理线程、保护共享数据、线程间的同步操作、低级原子操作等各种类。
// 新标准极大地提高了程序的可移植性，以前的多线程依赖于具体的平台，而现在有了统一的接口。
std::atomic_int64_t total3{0L};  // 本身就拥有原子性
void func3() {
  for (int64_t i = 1; i <= 100000; ++i) {
    total3 += i;
  }
}

void testWhyatomic() {
  std::cout << "Before func1,total1 = " << total1 << std::endl;  // 0
  std::thread t1(func1);
  std::thread t2(func1);
  t1.join();
  t2.join();
  std::cout << "After func1,total1 = " << total1 << std::endl;  // 5477937886

  std::cout << "Before func2,total1 = " << total2 << std::endl;  // 0
  std::thread t3(func2);
  std::thread t4(func2);
  t3.join();
  t4.join();
  std::cout << "After func2,total1 = " << total2 << std::endl;  // 10000100000

  std::cout << "Before func3,total3 = " << total3 << std::endl;  // 0
  std::thread t5(func3);
  std::thread t6(func3);
  t5.join();
  t6.join();
  std::cout << "After func3,total3 = " << total3 << std::endl;  // 10000100000
}
}  // namespace whyatomic

namespace operation {
// C++11标准将原子操作定义为atomic模板类的成员函数，包括读（load）、写（store）、交换（exchange）等。
// 对于内置类型而言，主要是通过重载一些全局操作符来完成的。
// 对于原子操作并非只有成员函数，当然也存在非成员函数，对于大多数非成员函数只是在原来函数基础上添加atomic_前缀。
// 他们有两个变种：一个是没有标签的，一个是添加_explict后缀和额外的参数作为内存顺序的标签。

//                                                   原子类型的可用操作
// 操作 	                atomic_flag 	atomic<bool> 	atomic<T*> 	atomic<integral-type> 	atomic<othre-type>
// test_and_set 	            √
// clear 	                    √
// is_lock_free 	  	                        √ 	            √ 	                √ 	                √
// load 	  	                                √ 	            √ 	                √ 	                √
// store 	  	                                √ 	            √ 	                √ 	                √
// exchange 	  	                            √ 	            √ 	                √ 	                √
// compare_exchange_weak 	  	                √ 	            √ 	                √ 	                √
// compare_exchange_strong
// fetch_add, += 	  	  	                                    √ 	                √
// fetch_sub, -= 	  	  	                                    √ 	                √
// fetch_or, |= 	  	  	  	                                                    √
// fetch_and, &= 	  	  	  	                                                    √
// fetch_xor, ^= 	  	  	  	                                                    √
// ++, -- 	  	  	                                          √ 	                √

void testOperation() {
  // 原子类型可以直接对普通类型进行赋值，这是因为atomic类模板定义了从atomic到T的类型转换函数，可以隐式转换。
  std::atomic_int ai1{10};
  int i = ai1;
  std::cout << i << ", " << ai1 << std::endl;

  // std::atomic_int ai2 = 10;  // 原子类型不支持拷贝构造

  std::atomic_int ai3;
  ai3 = 10;
  // ai3 = ai1;  // 不支持拷贝构造，因为原子读和原子写都是独立原子操作，无法保证2个独立的操作加在一起仍然保证原子性。

  ai3 += 5;
  std::cout << ai3.load() << std::endl;  // 15
  ai3 += ai1;
  std::cout << ++ai3 << std::endl;             // 26
  ai3.exchange(ai1);                           // 过程是原子的
  std::cout << "ai1 is " << ai1 << std::endl;  // 10
  std::cout << "ai3 is " << ai3 << std::endl;  // 10

  std::atomic_store<int>(&ai1, 50);
  std::atomic_exchange<int>(&ai3, ai1);
  std::cout << std::atomic_load<int>(&ai3) << std::endl;  // 50

  std::atomic<int64_t> ai4;
  std::cout << std::boolalpha << ai4.is_lock_free() << std::endl;  // true
  std::atomic<bool> ab;
  std::cout << std::boolalpha << ab.is_lock_free() << std::endl;  // true
}
}  // namespace operation

namespace CAS {
// CAS(compare and swap)是个原子操作，保证了如果需要更新的地址没有被他人改动多，那么它可以安全的写入。
// 而这也是我们对于某个数据或者数据结构加锁要保护的内容，保证读写的一致性，不出现dirty data。
// 现在几乎所有的CPU指令都支持CAS的原子操作。
// compare_exchange_weak和compare_exchange_strong则是著名的CAS（compare and set）。
// 参数会要求在这里传入期待的数值和新的数值。它们对比变量的值和期待的值是否一致，如果是，则替换为用户指定的一个新的数值。
// 如果不是，则将变量的值和期待的值交换。
// weak版本的CAS允许偶然出乎意料的返回（比如在字段值和期待值一样的时候却返回了false），不过在一些循环算法中，这是可以接受的。
// 通常weak比起strong有更高的性能。

// 无锁栈
template <typename T>
class lock_free_stack {  // 栈的底层数据结构采用单向链表实现
 private:
  struct node {
    // 采用shared_ptr管理的好处在于：若栈内存放对象pop中return栈顶对象可能拷贝异常，栈内只存储指针还可以提高性能
    std::shared_ptr<T> data;
    node* next;
    // 注意make_shared比直接shared_ptr构造的内存开销小
    node(T const& data_) : data(std::make_shared<T>(data_)) {}
  };
  // 采用原子类型管理栈顶元素，主要利用atomic::compare_exchange_weak实现lock free
  std::atomic<node*> head;

 public:
  void push(T const& data) {
    node* const new_node = new node(data);
    new_node->next = head.load();  // 每次从链表头插入
    // 若head==new_node->next则更新head为new_node,返回true结束循环，插入成功；
    // 若head!=new_node->next表明有其它线程在此期间对head操作了，将new_node->next更新为新的head，
    // 返回false，继续进入下一次while循环。
    // 这里采用atomic::compare_exchange_weak比atomic::compare_exchange_strong快，
    // 因为compare_exchange_weak可能在元素相等的时候返回false所以适合在循环中，
    // 而atomic::compare_exchange_strong保证了比较的正确性，不适合用于循环
    while (!head.compare_exchange_weak(new_node->next, new_node))
      ;
  }
  std::shared_ptr<T> pop() {
    // 拿住栈顶元素，但是可能后续被更新，更新发生在head!=old_head时
    node* old_head = head.load();
    // 首先要先判断old_head是否为nullptr防止操作空链表，然后按照compare_exchange_weak语义更新链表头结点。
    // 若head==old_head则更新head为old_head->next并返回true，结束循环，删除栈顶元素成功;
    // 若head!=old_head表明有其它线程操作了head，因此更新old_head为新的head,返回false进入下一轮循环，直至删除成功。
    while (old_head && !head.compare_exchange_weak(old_head, old_head->next))
      ;
    // 空链表时返回的是一个空的shared_ptr对象
    return old_head ? old_head->data : std::shared_ptr<T>();
  }
  // 这里只是lock free，由于while循环可能无限期循环不能在有限步骤内完成，故不是wait free
};

void testCAS() {
  std::atomic<int> ai1(10);
  int i1 = 20;
  int i2 = 30;
  bool ret = false;
  std::cout << std::boolalpha << ret << ", ai1:" << ai1 << ",i1:" << i1
            << std::endl;  // false, ai1:10,i1:20

  // 当前值10与期望值(参数1是20)不等时，将期望值(参数1是20)修改为当前值（10），返回false
  ret = ai1.compare_exchange_weak(i1, i2);
  std::cout << std::boolalpha << ret << ", ai1:" << ai1 << ",i1:" << i1
            << std::endl;  // false, ai1:10,i1:10

  // 当前值10与期望值(参数1是10)相等时，修改当前值为设定值(参数2是30)，返回true
  ret = ai1.compare_exchange_weak(i1, i2);
  std::cout << std::boolalpha << ret << ", ai1:" << ai1 << ",i1:" << i1
            << std::endl;  // true, ai1:30,i1:10

  ret = std::atomic_compare_exchange_strong<int>(&ai1, &i1, i2);
  std::cout << std::boolalpha << ret << ", ai1:" << ai1 << ",i1:" << i1
            << std::endl;  // false, ai1:30,i1:30

  i2 = 40;
  ret = std::atomic_compare_exchange_strong<int>(&ai1, &i1, i2);
  std::cout << std::boolalpha << ret << ", ai1:" << ai1 << ",i1:" << i1
            << std::endl;  // true, ai1:40,i1:30
}
}  // namespace CAS

namespace atomicflag {
// 有一个比较特殊的原子类型是atomic_flag，因为atomic_flag与其他原子类型不同，它是无锁（lock_free）的。
// 线程对其访问不需要加锁，而其他的原子类型不一定是无锁的。
// 因为atomic<T>并不能保证类型T是无锁的，另外不同平台的处理器处理方式不同，也不能保证必定无锁。
// 所以其他的类型都会有is_lock_free()成员函数来判断是否是无锁的。
// atomic_flag只支持test_and_set()以及clear()两个成员函数。

// 当一个线程尝试去获取某一把锁的时候，如果这个锁此时已经被别人获取(占用)，那么此线程就无法获取到这把锁，
// 该线程将会等待，间隔一段时间后会再次尝试获取。这种采用循环加锁->等待的机制被称为自旋锁（spin lock）。
// 可以用atomic_flag的成员函数test_and_set()和clear()来实现一个自旋锁：
std::atomic_flag lock = ATOMIC_FLAG_INIT;
void Lock(std::atomic_flag& lock) {
  while (lock.test_and_set()) {
    std::cout << "waitting" << std::endl;
  };
}
void UnLock(std::atomic_flag& lock) { lock.clear(); }

void func1() {
  Lock(lock);
  std::cout << "func1 do something" << std::endl;
}
void func2() {
  std::cout << "func2 start" << std::endl;
  UnLock(lock);
}

void testAtoicflag() {
  std::atomic_flag f = ATOMIC_FLAG_INIT;
  // test_and_set()函数检查std::atomic_flag标志，如果std::atomic_flag没有被设置，则设置std::atomic_flag；
  // 如果之前std::atomic_flag已被设置，则test_and_set()返回true，否则返回false。
  std::cout << std::boolalpha << f.test_and_set() << std::endl;  // false
  std::cout << std::boolalpha << f.test_and_set() << std::endl;  // true
  f.clear();  // 清除std::atomic_flag标志使得下一次调用std::atomic_flag::test_and_set()返回false
  std::cout << std::boolalpha << f.test_and_set() << std::endl;  // false

  Lock(lock);
  std::thread t1(func1);
  std::thread t2(func2);
  t1.join();
  t2.join();
  // waitting
  // waitting
  // waitting
  // waitting
  // waitting
  // func2 start
  // func1 do something
}
}  // namespace atomicflag

namespace memoryorder {
// 内存模型通常是硬件上的概念，表示的是机器指令是以什么样的顺序被处理器执行的，现代的处理器并不是逐条处理机器指令的。
// 现实中，x86_64以及SPARC（TSO模式）都是采用强顺序内存模型的平台。
// 在多线程程序中，强顺序类型意味着对于各个线程看到的指令执行顺序是一致的。
// 对于处理器而言，内存中的数据被改变的顺序与机器指令中的一致。
// 相反的，弱顺序就是各个线程看到的内存数据被改变的顺序与机器指令中声明的不一致，弱顺序内存模型可能会导致程序问题。
// 为什么有些平台，诸如Alpha、PowerPC、Itanlium、ArmV7等平台会使用这种模型？
// 简单地说，这种模型能让处理器有更好的并行性，提高指令执行的效率。
// 并且，为了保证指令执行的顺序，通常需要在汇编指令中加入一条内存栅栏（memory barrier）指令，但是会影响处理器性能。
// 比如在PowerPC上，就有一条名为sync的内存栅栏指令。该指令迫使已经进入流水线中的指令都完成后处理器才会执行sync以后的指令。
// C++11中的原子操作还可以包含一个参数：内存顺序（memory_order），是C++11为原子类型定义的内存模型，
// 让程序员根据实际情况灵活地控制原子类型的执行顺序。通常情况下，使用该参数将有利于编译器进一步提高并行性。

// 在C++11中一共有7种memory_order枚举值，默认按照memory_order_seq_cst执行：
// typedef enum memory_order {
//   memory_order_relaxed,  // 不对执行顺序做保证
//   memory_order_acquire,  // 本线程中,所有后续的读操作必须在本条原子操作完成后执行
//   memory_order_release,  // 本线程中,所有之前的写操作完成后才能执行本条原子操作
//   memory_order_acq_rel,  // 同时包含memory_order_acquire和memory_order_release
//   memory_order_consume,  // 本线程中,所有后续的有关本原子类型的操作,必须在本条原子操作完成之后执行
//   memory_order_seq_cst   // 全部存取都按顺序执行
// } memory_order;

// 需要注意的是，不是所有的memory_order都能被atomic成员使用：
// store函数可以使用memory_order_seq_cst、memory_order_release、memory_order_relaxed。
// load函数可以使用memory_order_seq_cst、memory_order_acquire、memory_order_consume、memory_order_relaxed。
// 需要同时读写的操作，例如test_and_flag、exchange等操作，
// 可以使用memory_order_seq_cst、memory_order_release、memory_order_acquire、memory_order_consume、memory_order_relaxed。

// 原子类型提供的一些操作符，比如operator=、operator+=等函数，
// 都是以memory_order_seq_cst为memory_order的实参的原子操作封装，所以他们都是顺序一致性的。
// 如果要指定内存顺序的话，则应该采用store、load、atomic_fetch_add这样的版本。
// 最后说明一下，std::atomic和std::memory_order只有在多线程无锁编程时才会用到。
// 在x86_64平台，由于是强顺序内存模型的，为了保险起见，不要使用std::memory_order，
// 使用std::atmoic默认形式即可，因为std::atmoic默认是强顺序内存模型。

std::atomic<int> a{0};
std::atomic<int> b{0};
void valueSet() {
  int t = 1;
  a.store(t);
  b.store(2);
}
// 如果原子类型变量a和b并没有要求执行的顺序性，那么可以采用一种松散的内存模型来放松对原子操作的执行顺序的要求:
void valueSet1() {
  int t = 1;
  a.store(t, std::memory_order_relaxed);
  b.store(2, std::memory_order_relaxed);
}
void Observer() { std::cout << a << " " << b << std::endl; }

void testMemoryorder() {
  std::thread t1(valueSet1);
  std::thread t2(Observer);
  // 可能会打印出0 2这样的结果
  t1.join();
  t2.join();
}
}  // namespace memoryorder

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 4]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      whyatomic::testWhyatomic();
      break;
    case 1:
      operation::testOperation();
      break;
    case 2:
      CAS::testCAS();
      break;
    case 3:
      atomicflag::testAtoicflag();
      break;
    case 4:
      memoryorder::testMemoryorder();
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }
}