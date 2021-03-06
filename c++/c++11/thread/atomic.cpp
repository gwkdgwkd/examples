#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <iostream>
#include <thread>

using namespace std;

// 原子操作是在多线程程序中“最小的且不可并行化的”操作，意味着多个线程访问同一个资源时，有且仅有一个线程能对资源进行操作。通常情况下原子操作可以通过互
// 斥的访问方式来保证，例如Linux下的互斥锁（mutex），Windows下的临界区（Critical Section）等

int64_t total = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// Linux环境使用POSIX标准的pthread库实现多线程下的原子操作：
void* threadFunc1(void* args) {
  int64_t endNum = *(int64_t*)args;
  for (int64_t i = 1; i <= endNum; ++i) {
    pthread_mutex_lock(&m);
    total += i;
    pthread_mutex_unlock(&m);
  }
}

// 在C++11之前，使用第三方API可以实现并行编程，比如pthread多线程库，但是在使用时需要创建互斥锁，以及进行加锁、解锁等操作来保证多线程对临界资源的原子操作，
// 这无疑增加了开发的工作量。不过从C++11开始，C++从语言层面开始支持并行编程，内容包括了管理线程、保护共享数据、线程间的同步操作、低级原子操作等各种类。新标
// 准极大地提高了程序的可移植性，以前的多线程依赖于具体的平台，而现在有了统一的接口。
atomic<long> total1{0};  // atomic_int64_t相当于int64_t，但是本身就拥有原子性
void threadFunc2(int64_t endNum) {
  for (int64_t i = 1; i <= endNum; ++i) {
    total1 += i;
  }
}

// 原子操作是平台相关的，原子类型能够实现原子操作是因为 C++11 对原子类型的操作进行了抽象，定义了统一的接口，并要求编译器产生平台相关的原子操作的具体实现。
// C++11标准将原子操作定义为atomic模板类的成员函数，包括读（load）、写（store）、交换（exchange）等。对于内置类型而言，主要是通过重载一些全局操作符来完成的。

// 有一个比较特殊的原子类型是atomic_flag，因为atomic_flag与其他原子类型不同，它是无锁（lock_free）的，即线程对其访问不需要加锁，而其他的原子类型不一定是
// 无锁的。因为atomic<T>并不能保证类型T是无锁的，另外不同平台的处理器处理方式不同，也不能保证必定无锁，所以其他的类型都会有is_lock_free() 成员函数来判断是
// 否是无锁的。atomic_flag只支持test_and_set()以及clear()两个成员函数，test_and_set()函数检查std::atomic_flag标志，如果std::atomic_flag之前没有
// 被设置过，则设置std::atomic_flag的标志；如果之前std::atomic_flag已被设置，则返回true，否则返回false。clear()函数清除std::atomic_flag标志使得下一
// 次调用std::atomic_flag::test_and_set()返回false。可以用atomic_flag的成员函数test_and_set()和clear()来实现一个自旋锁（spin lock）：
std::atomic_flag lock = ATOMIC_FLAG_INIT;
void func1() {
  // 在主线程中设置为true，需要等待t4线程clear
  while (lock.test_and_set(std::memory_order_acquire)) {
    std::cout << "func1 wait" << std::endl;
  }
  std::cout << "func1 do something" << std::endl;
}
void func2() {
  std::cout << "func2 start" << std::endl;
  lock.clear();
}
// 可以封装成锁操作的方式，比如：
void Lock(atomic_flag& lock) {
  while (lock.test_and_set())
    ;
}
void UnLock(atomic_flag& lock) { lock.clear(); }

// 内存模型通常是硬件上的概念，表示的是机器指令是以什么样的顺序被处理器执行的，现代的处理器并不是逐条处理机器指令的
// 现实中，x86_64以及SPARC（TSO模式）都是采用强顺序内存模型的平台。在多线程程序中，强顺序类型意味着对于各个线程看到的指令执行顺序是一致的。对于处理器而言，
// 内存中的数据被改变的顺序与机器指令中的一致。相反的，弱顺序就是各个线程看到的内存数据被改变的顺序与机器指令中声明的不一致。弱顺序内存模型可能会导致程序问题，
// 为什么有些平台，诸如Alpha、PowerPC、Itanlium、ArmV7等平台会使用这种模型？简单地说，这种模型能让处理器有更好的并行性，提高指令执行的效率。并且，为了保证
// 指令执行的顺序，通常需要在汇编指令中加入一条内存栅栏（memory barrier）指令，但是会影响处理器性能。比如在PowerPC上，就有一条名为sync的内存栅栏指令。该指
// 令迫使已经进入流水线中的指令都完成后处理器才会执行sync以后的指令。
// C++11中的原子操作还可以包含一个参数：内存顺序（memory_order），是C++11为原子类型定义的内存模型，让程序员根据实际情况灵活地控制原子类型的执行顺序。通常情
// 况下，使用该参数将有利于编译器进一步提高并行性能
atomic<int> a{0};
atomic<int> b{0};

int valueSet(int) {
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
// 上面的代码使用了store函数进行赋值，store函数接受两个参数，第一个是要写入的值，第二个是名为memory_order的枚举值。这里使用了std::memory_order_relaxed，
// 表示松散内存顺序，该枚举值代表编译器可以任由编译器重新排序或则由处理器乱序处理。这样a和b的赋值执行顺序性就被解除了。
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
// 1 store函数可以使用memory_order_seq_cst、memory_order_release、memory_order_relaxed。
// 2 load函数可以使用memory_order_seq_cst、memory_order_acquire、memory_order_consume、memory_order_relaxed。
// 3 需要同时读写的操作，例如test_and_flag、exchange等操作。可以使用memory_order_seq_cst、memory_order_release、memory_order_acquire、
// memory_order_consume、memory_order_relaxed。

// 原子类型提供的一些操作符，比如operator=、operator+=等函数，都是以memory_order_seq_cst为memory_order的实参的原子操作封装，所以他们都是顺序一致性的。
// 如果要指定内存顺序的话，则应该采用store、load、atomic_fetch_add这样的版本。
// 最后说明一下，std::atomic和std::memory_order只有在多线程无锁编程时才会用到。
// 在x86_64平台，由于是强顺序内存模型的，为了保险起见，不要使用std::memory_order，使用std::atmoic默认形式即可，因为std::atmoic默认是强顺序内存模型。

void Observer() { cout << a << " " << b << endl; }

// ==========

// 原子类型，是c++11中提供的一类数据类型，这种数据类型，不需要我们显式使用互斥锁，但是，编译器却可以保证原子类型在线程间被互斥地访问。
// 我们可以理解为：c++11将原子类型的互斥锁、临界区给抽象化了，而不需要我们再去进行相关操作。

long xxx = total1;  // 原子类型可以直接对普通类型进行赋值
// 这是因为atomic类模板定义了从atomic到T的类型转换函数的缘故，在需要时，会进行隐式转换。但是需要注意：原子类型不支持拷贝构造。

// 对于原子操作并非只有成员函数，当然也存在非成员函数，对于大多数非成员函数只是在原来函数基础上添加atomic_前缀。在有机会指定内存顺序标签的地方，
// 他们有两个变种：一个是没有标签的，一个是添加_explict后缀和额外的参数作为内存顺序的标签。
// 原子操作是一个不可分割的操作，这种操作有个特点，要么做完，要么没做完，在其他线程访问的时候，不能够访问到这种过程的中间态。
//                                                      原子类型的可用操作
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
// ++, -- 	  	  	                                            √ 	                √

int main() {
  int64_t endNum = 100000;

  pthread_t thread1ID = 0, thread2ID = 0;
  pthread_create(&thread1ID, NULL, threadFunc1, &endNum);  // 创建线程1
  pthread_create(&thread2ID, NULL, threadFunc1, &endNum);  // 创建线程2
  pthread_join(thread1ID, NULL);  // 阻塞等待线程1结束并回收资源
  pthread_join(thread2ID, NULL);  // 阻塞等待线程2结束并回收资源
  cout << "total=" << total << endl;  // 10000100000
  // 如果没有锁，会得出一个小于10000100000随机的错误值

  thread t1(threadFunc2, endNum);
  thread t2(threadFunc2, endNum);
  t1.join();
  t2.join();
  cout << "total=" << total1 << endl;  // 10000100000

  lock.test_and_set();  // 设置状态
  std::thread t3(func1);
  usleep(1);  // 睡眠1us
  std::thread t4(func2);
  t3.join();
  t4.join();

  // 可能会打印出a == 0;b == 2这样的结果
  thread t5(valueSet1);
  thread t6(Observer);
  t5.join();  // 主线程(调用方)等待子线程t5执行完成,才能继续执行,阻塞
  t6.join();  // 同上,执行这一行前:t5已经结束,t6很可能也结束了

  return 0;
}