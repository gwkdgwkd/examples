#include <pthread.h>
#include <stdio.h>
#include <unistd.h>  // sleep()

// pthread_create()函数声明在<pthread.h>头文件中，语法格式如下：
// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
//                    void *(*start_routine)(void *), void *arg);

// 各个参数的含义是：
// 1.pthread_t *thread
//   传递一个 pthread_t类型的指针变量，
//   也可以直接传递某个pthread_t类型变量的地址。
//   pthread_t是一种用于表示线程的数据类型，
//   每一个pthread_t类型的变量都可以表示一个线程。
// 2.const pthread_attr_t *attr
//   用于手动设置新建线程的属性，
//   例如线程的调用策略、线程所能使用的栈内存的大小等。
//   大部分场景中，我们都不需要手动修改线程的属性，
//   将attr参数赋值为NULL，
//   pthread_create()函数会采用系统默认的属性值创建线程。
//   pthread_attr_t类型以结构体的形式定义在<pthread.h>头文件中，
//   此类型的变量专门表示线程的属性。
// 3.void *(*start_routine) (void*)
//   以函数指针的方式指明新建线程需要执行的函数，
//   该函数的参数最多有1个（可以省略不写），
//   形参和返回值的类型都必须为void*类型。
//   void*类型又称空指针类型，表明指针所指数据的类型是未知的。
//   使用此类型指针时，通常需要先对其进行强制类型转换，
//   然后才能正常访问指针指向的数据。
//   如果该函数有返回值，则线程执行完函数后，
//   函数的返回值可以由pthread_join()函数接收。
// 4.void *arg
//   指定传递给start_routine函数的实参，
//   当不需要传递任何数据时，将arg赋值为NULL即可。

// 如果成功创建线程，pthread_create()函数返回数字0，反之返回非零值。
// 各个非零值都对应着不同的宏，指明创建失败的原因，常见的宏有以下几种：
// 1.EAGAIN：系统资源不足，无法提供创建线程所需的资源。
// 2.EINVAL：传递给pthread_create()函数的attr参数无效。
// 3.EPERM：传递给pthread_create()函数的attr参数中，
//   某些属性的设置为非法操作，程序没有相关的设置权限。
// 以上这些宏都声明在<errno.h>头文件中，
// 如果程序中想使用这些宏，需提前引入此头文件。

void *ThreadFun(void *arg) {
  if (arg == NULL) {
    printf("arg is NULL\n");
  } else {
    printf("%s\n", (char *)arg);
  }
  return NULL;
}
int main() {
  int res;
  char *url = "http://www.biancheng.net";
  pthread_t myThread1, myThread2;

  res = pthread_create(&myThread1, NULL, ThreadFun, NULL);
  if (res != 0) {
    printf("线程创建失败");
    return 0;
  }
  sleep(1);  // 令主线程等到myThread1线程执行完成

  res = pthread_create(&myThread2, NULL, ThreadFun, (void *)url);
  if (res != 0) {
    printf("线程创建失败");
    return 0;
  }
  sleep(1);  // 令主线程等到mythread2线程执行完成
  return 0;
}

// gcc create.c -lpthread
