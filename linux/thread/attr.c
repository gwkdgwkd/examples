#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// POSIX标准中，线程的属性用pthread_attr_t类型的变量表示。
// 使用此变量前，必须调用pthread_attr_init()函数进行初始化，
// 该函数的语法格式如下：
// int pthread_attr_init(pthread_attr_t * attr);
// 此函数定义在<pthread.h>头文件中，函数执行成功时，
// 返回数字0，反之返回非零数。
// 通过调用pthread_attr_init()函数，
// myAttr变量就拥有了系统默认的线程属性。
// 在此基础上，可以根据需要对myAttr变量的属性值进行修改。

// pthread_attr_t是一种结构体类型，内部包含多种线程属性：
// typedef struct {
//   int __detachstate;
//   int __schedpolicy;
//   struct sched_param __schedparam;
//   int __inheritsched;
//   int __scope;
//   size_t __guardsize;
//   int __stackaddr_set;
//   void* __stackaddr;
//   size_t __stacksize;
// } pthread_attr_t;

// 1.__detachstate
//   默认属性的线程在执行完目标函数后，占用的私有资源并不会立即释放，
//   要么执行完pthread_join()函数后释放，要么整个进程执行结束后释放。
//   某些场景中，并不需要接收线程执行结束后的返回值，
//   如果想让线程执行完后立即释放占用的私有资源，
//   就可以通过修改__detachstate属性值来实现。
//   __detachstate属性值用于指定线程终止执行的时机，该属性的值有两个，
//   PTHREAD_CREATE_JOINABLE（默认值）：
//   线程执行完函数后不会自行释放资源；
//   PTHREAD_CREATE_DETACHED：
//   线程执行完函数后，会自行终止并释放占用的资源。
//   关于__detachstate属性，<pthread.h>中提供了2个与它相关的函数：
//   int pthread_attr_getdetachstate(const pthread_attr_t *attr,
//                                   int *detachstate);
//   int pthread_attr_setdetachstate(pthread_attr_t *sttr，
//                                   int detachstate);
//   pthread_attr_getdetachstate()函数用于获取__detachstate属性的值，
//   detachstate指针用于接收__detachstate属性的值；
//   pthread_attr_setdetachstate()函数用于修改__detachstate属性的值，
//   detachstate整形变量即为新的__detachstate属性值。
//   两个函数执行成功时返回数字0，反之返回非零数。
//   此外，<pthread.h>头文件还提供有pthread_detach()函数，
//   可以直接将目标线程的__detachstate属性改为PTHREAD_CREATE_DETACHED，
//   语法格式如下：
//   int pthread_detach(pthread_t thread);
//   函数执行成功时返回数字0 ，反之返回非零数。

// 2.__schedpolicy
//   __schedpolicy属性用于指定系统调度该线程所用的算法，它的值有以下3个：
//   SCHED_OTHER（默认值）：分时调度算法；
//   SCHED_FIFO：先到先得（实时调度）算法；
//   SCHED_RR：轮转法；
//   其中，SCHED_OTHER调度算法不支持为线程设置优先级，
//   而另外两种调度算法支持。
//   <pthread.h>头文件提供了如下两个函数，
//   专门用于访问和修改__schedpolicy属性：
//   int pthread_attr_getschedpolicy(const pthread_attr_t *attr,
//                                   int *policy)
//   int pthread_attr_setschedpolicy(pthread_attr_*, int policy)
//   pthread_attr_getschedpolicy()用于获取当前__schedpolicy属性的值；
//   pthread_attr_setschedpolicy()用于修改 __schedpolicy属性的值。
//   函数执行成功时，返回值为数字0，反之返回非零数。

// 3.__schedparam
//   __scheparam用于设置线程的优先级（默认值为0），
//   该属性仅当线程的__schedpolicy属性为SCHED_FIFO，
//   或者SCHED_RR时才能发挥作用。
//   <pthread.h>中提供了如下两个函数，用于获取和修改__schedparam属性的值：
//   int pthread_attr_getschedparam(const pthread_attr_t *attr,
//                                  struct sched_param *param);
//   int pthread_attr_setschedparam(pthread_attr_t *attr,
//                                  const struct sched_param *param);
//   其中，param参数用于接收或者修改__scheparam属性的优先级，
//   它是sched_param结构体类型的变量，定义在<sched.h>头文件中，
//   内部仅有一个sched_priority整形变量，用于表示线程的优先级。
//   函数执行成功时返回数字0，反之返回非零数。
//   当需要修改线程的优先级时，只需创建一个sched_param类型的变量，
//   并为其内部的sched_priority成员赋值，
//   然后将其传递给pthrerd_attr_setschedparam()函数。
//   不同的操作系统，线程优先级的值的范围不同，
//   可以通过调用如下两个系统函数获得当前系统支持的最大和最小优先级的值：
//   int sched_get_priority_max(int policy);
//   int sched_get_priority_min(int policy);
//   其中，policy的值可以为SCHED_FIFO、SCHED_RR或者SCHED_OTHER，
//   当policy的值为SCHED_OTHER时，最大和最小优先级的值都为0。

// 4.__inheritsched
//   新建线程的调度属性（__schedpolicy和__schedparam属性），
//   默认遵循父线程的属性（谁创建它，谁就是它的父线程），
//   如果想自定义线程的调度属性，就需要借助__inheritsched属性。
//   也就是说，新线程的调度属性要么遵循父线程，要么遵循myAttr规定的属性，
//   默认情况下__inheritsched规定新线程的调度属性遵循父线程，
//   也可以修改__inheritsched的值，
//   使新线程的调度属性遵循自定义的属性变量规定的值。
//   <pthread.h>提供了两个函数，用于获取和修改__inheritsched属性的值：
//   int pthread_attr_getinheritsched(const pthread_attr_t *attr,
//                                    int *inheritsched);
//   int pthread_attr_setinheritsched(pthread_attr_t *attr,
//                                    int inheritsched);
//   其中在pthread_attr_setinheritsched()函数中，
//   inheritsched参数的可选值有两个，分别是：
//   PTHREAD_INHERIT_SCHED（默认值）：
//   新线程的调度属性继承自父线程；
//   PTHREAD_EXPLICIT_SCHED：
//   新线程的调度属性继承自myAttr规定的值。
//   以上两个函数执行成功时返回数字0，反之返回非零数。

// 5.__scope
//   线程执行过程中，可以只和同进程内的其它线程争夺CPU资源，
//   也可以和系统中所有的其它线程争夺CPU资源，
//   __scope属性用于指定目标线程和哪些线程抢夺CPU资源。
//   <pthread.h>中提供了两个函数，分别用于获取和修改__scope属性的值：
//   int pthread_attr_getscope(const pthread_attr_t *attr,
//                             int *scope);
//   int pthread_attr_setscope(pthread_attr_t *attr,
//                             int *scope);
//   当调用pthread_attr_setscope()函数时，scope参数的可选值有两个，
//   PTHREAD_SCOPE_PROCESS：同一进程内争夺CPU资源；
//   PTHREAD_SCOPE_SYSTEM：系统所有线程之间争夺CPU资源。
//   Linux系统仅支持PTHREAD_SCOPE_SYSTEM，即所有线程之间争夺CPU资源。
//   当函数执行成功时，返回值为数字0，反之返回非零数。

// 6.__guardsize
//   每个线程中，栈内存的后面都紧挨着一块空闲的内存空间，
//   通常称这块内存为警戒缓冲区，它的功能是：
//   一旦我们使用的栈空间超出了额定值，
//   警戒缓冲区可以确保线程不会因栈溢出立刻执行崩溃。
//   __guardsize属性专门用来设置警戒缓冲区的大小，
//   <pthread.h>提供了两个函数，分别用于获取和修改__guardsize属性的值：
//   int pthread_attr_getguardsize(
//       const pthread_attr_t *restrict attr,
//       size_t *restrict guardsize);
//   int pthread_attr_setguardsize(pthread_attr_t *attr,
//                                 size_t *guardsize);
//   pthread_attr_setguardsize()函数中，
//   设置警戒缓冲区的大小为参数guardsize指定的字节数。
//   函数执行成功时返回数字0，反之返回非零数。

// 7.__stacksize
//   每个线程都有属于自己的内存空间，
//   通常称为栈（有时也称堆栈、栈空间、栈内存等）。
//   某些场景中，线程执行可能需要较大的栈内存，
//   此时就需要自定义线程拥有的栈的大小。
//   __stacksize属性用于指定线程所拥有的栈内存的大小。
//   <pthread.h>提供有以下两个函数，分别用于获取和修改栈空间的大小：
//   int pthread_attr_getstacksize(const pthread_attr_t *attr,
//                                 size_t *stacksize);
//   int pthread_attr_setsstacksize(pthread_attr_t * attr,
//                                  size_t * stacksize);
//   函数执行成功时，返回值为数字0，反之返回非零数。

void *Thread1(void *arg) {
  printf("Thread1 正在执行\n");
  printf("http://c.biancheng.net\n");
  printf("Thread1 执行完毕\n");
  return NULL;
}
void *Thread2(void *arg) {
  printf("Thread2 正在执行\n");
  printf("C语言中文网\n");
  printf("Thread2 执行完毕\n");
  return NULL;
}
int main(int argc, char *argv[]) {
  int num1, num2, res;
  pthread_t mythread1, mythread2;

  // 创建两个表示线程优先级的变量：
  struct sched_param param1, param2;
  // 创建两个表示线程属性的变量：
  pthread_attr_t myAttr1, myAttr2;
  // 接收2个整数，用于设定线程的优先级：
  if (argc != 3) {
    printf("未向程序传入2个表示优先级的数字\n");
    return 0;
  }
  // 初始化线程属性：
  res = pthread_attr_init(&myAttr1);
  if (res != 0) {
    printf("myAttr1 init Fail\n");
  }

  res = pthread_attr_init(&myAttr2);
  if (res != 0) {
    printf("myAttr2 init Fail\n");
  }

  // 设置myAttr1的__detachstate属性值为PTHREAD_CREATE_DETACHED，
  // 遵循myAttr1属性的线程执行函数完毕后会自行释放占用私有资源，
  // 不支持pthread_join()函数：
  res = pthread_attr_setdetachstate(&myAttr1, PTHREAD_CREATE_DETACHED);
  if (res != 0) {
    printf("myAttr1 set_detachstate Fail\n");
  }

  // 设置myAttr1的__scope属性值为PTHREAD_SCOPE_SYSTEM，
  // 遵循myAttr1属性的线程将同系统中的所有其它线程争夺CPU资源：
  res = pthread_attr_setscope(&myAttr1, PTHREAD_SCOPE_SYSTEM);
  if (res != 0) {
    printf("myAttr1 set_scope Fail\n");
  }

  // 设置myAttr2的__scope属性值为PTHREAD_SCOPE_SYSTEM，
  // 遵循myAttr2属性的线程将同系统中的所有其它线程争夺CPU资源：
  res = pthread_attr_setscope(&myAttr2, PTHREAD_SCOPE_SYSTEM);
  if (res != 0) {
    printf("myAttr2 set_scope Fail\n");
  }

  // 设置myAttr1的__schedpolicy属性值为SCHED_FIFO，
  // 系统会以实时调用的方式执行遵循myAttr1属性的线程：
  res = pthread_attr_setschedpolicy(&myAttr1, SCHED_FIFO);
  if (res != 0) {
    printf("myAttr1 set_policy Fail\n");
  }

  // 设置myAttr2的__schedpolicy属性值为SCHED_FIFO，
  // 系统会以实时调用的方式执行遵循myAttr2属性的线程：
  res = pthread_attr_setschedpolicy(&myAttr2, SCHED_FIFO);
  if (res != 0) {
    printf("myAttr2 set_policy Fail\n");
  }

  // 设置myAttr1的__inheritsched属性值为PTHREAD_EXPLICIT_SCHED，
  // myAttr1属性的线程将遵循自定义的线程属性：
  res = pthread_attr_setinheritsched(&myAttr1, PTHREAD_EXPLICIT_SCHED);
  if (res != 0) {
    printf("myAttr1 set_inheritsched fail\n");
  }

  // 设置myAttr2的__inheritsched属性值为PTHREAD_EXPLICIT_SCHED，
  // myAttr2属性的线程将遵循自定义的线程属性：
  res = pthread_attr_setinheritsched(&myAttr2, PTHREAD_EXPLICIT_SCHED);
  if (res != 0) {
    printf("myAttr2 set_inheritsched fail\n");
  }

  num1 = atoi(argv[1]);
  num2 = atoi(argv[2]);
  // 分别将num1和num2作为线程优先级的值：
  param1.sched_priority = num1;
  param2.sched_priority = num2;
  // 设置myAttr1属性的优先级为param1：
  res = pthread_attr_setschedparam(&myAttr1, &param1);
  if (res != 0) {
    printf("param1 setscheparam Fail\n");
  }
  // 设置myAttr2属性的优先级为 param2：
  res = pthread_attr_setschedparam(&myAttr2, &param2);
  if (res != 0) {
    printf("param2 setscheparam Fail\n");
  }
  // 创建新线程并遵循myAttr1属性：
  res = pthread_create(&mythread1, &myAttr1, Thread1, NULL);
  if (res != 0) {
    printf("mythread1 create Fail, %d\n", res);
  }
  // 创建新线程并遵循myAttr2属性：
  res = pthread_create(&mythread2, &myAttr2, Thread2, NULL);
  if (res != 0) {
    printf("mythread2 create Fail, %d\n", res);
  }

  // 等待mythread1和mythread2两个线程执行完：
  sleep(5);

  // 尝试pthread_join()函数等待mythread1线程执行结束：
  res = pthread_join(mythread1, NULL);
  if (res != 0) {
    if (res == EINVAL) {
      printf("mythread1不支持调用pthread_join()函数\n");
    }
  }
  // 尝试等待mythread2线程执行结束：
  res = pthread_join(mythread2, NULL);
  if (res != 0) {
    printf("mythread2 has finished\n");
  }
  printf("主线程执行完毕\n");

  return 0;
}

// gcc attr.c -lpthread