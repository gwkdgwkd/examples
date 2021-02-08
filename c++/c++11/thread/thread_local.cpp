#include <cstdio>
#include <iostream>
#include <thread>

using namespace std;

// thread_local变量是C++11新引入的一种存储类型。它会影响变量的存储周期(Storage duration)，C++中有4种存储周期：
// automatic、static、dynamic、thread

// 有且只有thread_local关键字修饰的变量具有线程周期，这些变量(或者说对象）在线程开始的时候被生成，在线程结束的时候被销毁(deallocated)。
// 并且每 一个线程都拥有一个独立的变量实例。thread_local可以和static与extern关键字联合使用，这将影响变量的链接属性。

// 哪些变量可以被声明为thread_local:
// 1 命名空间下的全局变量
// 2 类的static成员变量
// 3 本地变量

// 既然每个线程都拥有一份独立的thread_local变量，那么就有2个问题需要考虑：
// 各线程的thread_local变量是如何初始化的
// 各线程的thread_local变量在初始化之后拥有怎样的生命周期，特别是被声明为thread_local的本地变量(local variables)
thread_local int g_n = 1;
void f() {
  g_n++;
  printf("id=%d, n=%d\n", std::this_thread::get_id(), g_n);
}
void foo() {
  thread_local int i = 0;
  printf("id=%d, n=%d\n", std::this_thread::get_id(), i);
  i++;
}
void f2() {
  foo();
  foo();
}

int main() {
  g_n++;  // 修改操作并不影响g_n在线程t1和t2中的初始值(值为1)
  f();
  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  f2();
  std::thread t4(f2);
  std::thread t5(f2);
  t4.join();
  t5.join();

  // 输出的前3行打印能帮助解答thread_local变量是如何初始化的，可以看到每个线程都会进行一次初始化，例子中的g_n在主线程中最早被初始化为1，
  // 随后被修改为2和3，但这些修改操作并不影响g_n在线程t1和t2中的初始值(值为1)，虽t1和t2线程启动的时候主线程中的变量值已被更新为3，所以主
  // 线程、thread1、thread2打印结果分别为3，2，2。
  // id=-1378629824, n=3
  // id=-1395951872, n=2
  // id=-1404344576, n=2
  // 后6行打印说明了一个事实，声明为thread_local的本地变量在线程中是持续存在的，不同于普通临时变量的生命周期，它具有static变量一样的初始化特
  // 征和生命周期，虽并没有被声明为static。foo中的thread_local变量i在每个线程第一次执行到的时候初始化，在每个线程各自累加，在线程结束时释放。
  // id=-1378629824, n=0
  // id=-1378629824, n=1
  // id=-1404344576, n=0
  // id=-1404344576, n=1
  // id=-1395951872, n=0
  // id=-1395951872, n=1

  return 0;
}

// g++ -std=c++11 thread_local.cpp -lpthread