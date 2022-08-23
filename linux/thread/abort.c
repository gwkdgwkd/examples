#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// 多线程中，终止线程执行的方式有3种，分别是：
// 1.线程执行完成后，自行终止；
// 2.线程执行过程中遇到了pthread_exit()或者return，也会终止执行；
// 3.线程执行过程中，接收到其它线程发送的终止执行的信号，然后终止执行。

// 在C语言中，return关键字用于终止函数执行，
// 必要时还能将函数的执行结果反馈给调用者。
// return关键字不仅可以用于普通函数，线程函数中也可以使用它。
// <pthread.h>中，提供了一个和return功能相同的pthread_exit()函数。
// 和之前不同，pthread_exit()函数只适用于线程函数，而不能用于普通函数。

// pthread_exit()函数的语法格式如下：
// void pthread_exit(void *retval);
// retval是void*类型的指针，可以指向任何类型的数据，
// 它指向的数据将作为线程退出时的返回值。
// 如果线程不需要返回任何数据，将retval参数置为NULL即可。
// 注意，retval指针不能指向函数内部的局部数据（比如局部变量）。
// 换句话说，pthread_exit()函数不能返回一个指向局部数据的指针，
// 否则很可能使程序运行结果出错甚至崩溃。
void *ThreadFun1(void *arg) {
// 返回的字符串存储在常量区，并非当前线程的私有资源：
#if 1
  pthread_exit("http://c.biancheng.net");
#else
  return "http://c.biancheng.net";

  // 执行结果和pthread_exit完全一样，这意味着当线程执行结束时，
  // 无论是采用return语句还是调用pthread_exit()函数，
  // 主线程中的pthread_join()函数都可以接收到线程的返回值。
#endif

  // 此语句不会被线程执行：
  printf("*****************");
}
void func1() {
  int res;
  void *thread_result;
  pthread_t myThread;
  res = pthread_create(&myThread, NULL, ThreadFun1, NULL);
  if (res != 0) {
    printf("线程创建失败");
  }
  res = pthread_join(myThread, &thread_result);
  if (res != 0) {
    printf("等待线程失败");
  }
  printf("%s\n", (char *)thread_result);
}
// pthread_exit()和return的区别：
// 1.return语句和pthread_exit()函数的含义不同，
//   return的含义是返回，它不仅可以用于线程执行的函数，普通函数也可以使用；
//   pthread_exit()函数的含义是线程退出，它专门用于结束某个线程的执行。
// 2.pthread_exit()函数还会自动调用线程清理程序，
//   本质是一个由pthread_cleanup_push()指定的自定义函数，
//   而return不具备这个能力。
// 总之，如果实际场景中想终止某个子线程，强烈建议大家使用pthread_exit()函数。
// 终止主线程时，return和pthread_exit()函数发挥的功能不同，可以根据需要选择。
void *ThreadFun2(void *arg) {
  sleep(1);
  printf("http://c.biancheng.net\n");
}
void func2() {
  int res;
  pthread_t myThread;

  res = pthread_create(&myThread, NULL, ThreadFun2, NULL);
  if (res != 0) {
    printf("线程创建失败");
    return;
  }
  printf("C语言中文网\n");

#if 0
  return;
  // 主线程执行速度很快，主线程最后执行的return语句不仅会终止主线程执行，
  // 还会终止其它子线程执行。
  // 也就是说，myThread2线程还没有执行输出语句就被终止了，
  // 所以只有下面的打印：

  // C语言中文网
#else
  pthread_exit(NULL);
  // pthread_exit()函数只会终止当前线程，不会影响其它线程的执行。
  // 所以有下面的打印：

  // C语言中文网
  // http://c.biancheng.net
#endif
}

// 多线程中，一个线程还可以向另一个线程发送终止执行的信号（Cancel信号），
// 这时就需要调用pthread_cancel()函数。
// pthread_cancel()函数声明在<pthread.h>头文件中，语法格式如下：
// int pthread_cancel(pthread_t thread);
// 参数thread用于接收Cancel信号的目标线程。

// 如果pthread_cancel()函数成功地发送了Cancel信号，
// 返回数字0，否则返回非零数。
// 对于因未找到目标线程导致的信号发送失败，函数返回ESRCH宏，
// 定义在<errno.h>头文件中，该宏的值为整数3。

// 注意，pthread_cancel()函数的功能仅仅是向目标线程发送Cancel信号，
// 至于目标线程是否接收该信号，何时响应该信号，全由目标线程决定。

// 对于接收Cancel信号后结束执行的目标线程，等同于该线程自己执行如下语句：
// pthread_exit(PTHREAD_CANCELED);
// 也就是说，当一个线程被强制终止执行时，
// 它会返回PTHREAD_CANCELED这个宏，定义在<pthread.h>头文件中。
void *thread_Fun3(void *arg) {
  printf("新建线程开始执行\n");
  sleep(10);
}
void func3() {
  pthread_t myThread;
  void *mess;
  int value;
  int res;

  res = pthread_create(&myThread, NULL, thread_Fun3, NULL);
  if (res != 0) {
    printf("线程创建失败\n");
    return;
  }
  sleep(1);

  // 向线程发送cancel信号：
  res = pthread_cancel(myThread);
  if (res != 0) {
    printf("终止myThread线程失败\n");
    return;
  }
  // 获取已终止线程的返回值：
  res = pthread_join(myThread, &mess);
  if (res != 0) {
    printf("等待线程失败\n");
    return;
  }
  //如果线程被强制终止，其返回值为PTHREAD_CANCELED：
  if (mess == PTHREAD_CANCELED) {
    printf("myThread线程被强制终止\n");
  } else {
    printf("error\n");
  }
  return;
}

// 实际使用pthread_cancel()函数时，会发现Cancel信号成功发送，
// 但目标线程并未立即终止执行：
void *thread_Fun4(void *arg) {
  printf("新建线程开始执行\n");
  // 插入无限循环的代码，测试pthread_cancel()函数的有效性：
  while (1)
    ;
}
void func4() {
  pthread_t myThread;
  void *mess;
  int value;
  int res;
  res = pthread_create(&myThread, NULL, thread_Fun4, NULL);
  if (res != 0) {
    printf("线程创建失败\n");
    return;
  }
  sleep(1);
  res = pthread_cancel(myThread);
  if (res != 0) {
    printf("终止myThread线程失败\n");
    return;
  }
  printf("等待myThread线程执行结束：\n");
  res = pthread_join(myThread, &mess);
  if (res != 0) {
    printf("等待线程失败\n");
    return;
  }
  if (mess == PTHREAD_CANCELED) {
    printf("myThread线程被强制终止\n");
  } else {
    printf("error\n");
  }
  return;

  // 新建线程开始执行
  // 等待myThread线程执行结束：

  // pthread_cancel()函数成功发送了Cancel信号，但目标线程仍在执行。
}

// 线程对Cancel信号的处理
// 对于默认属性的线程，当有线程用pthread_cancel()向它发送Cancel信号时，
// 它并不会立即结束执行，而是选择在一个适当的时机结束执行。
// 所谓适当的时机，POSIX标准中规定，当线程执行一些特殊的函数时，
// 会响应Cancel信号并终止执行，比如常见的pthread_join()、
// pthread_testcancel()、sleep()、system()等，
// POSIX标准称此类函数为cancellation points，取消点。

// 此外，<pthread.h>还提供了两个函数：
// pthread_setcancelstate()和pthread_setcanceltype()，
// 可以手动修改目标线程处理Cancel信号的方式。

// 借助pthread_setcancelstate()函数，可以令目标线程处理Cancal信号，
// 也可以令目标线程不理会其它线程发来的Cancel信号。
// pthread_setcancelstate()函数的语法格式如下：
// int pthread_setcancelstate(int state , int *oldstate);
// state参数有两个可选值，分别是：
// 1.PTHREAD_CANCEL_ENABLE，默认值
//   当前线程会处理其它线程发送的Cancel信号；
// 2.PTHREAD_CANCEL_DISABLE
//   当前线程不理会其它线程发送的Cancel信号，
//   直到线程状态重新调整为PTHREAD_CANCEL_ENABLE后，
//   才处理接收到的Cancel信号。
// oldtate参数用于接收线程先前所遵循的state值，通常用于对线程进行重置。
// 如果不需要接收此参数的值，置为NULL即可。
// pthread_setcancelstate()函数执行成功时，返回数字0，反之返回非零数。

// 当线程会对Cancel信号进行处理时，
// 可以借助pthread_setcanceltype()函数设置线程响应Cancel信号的时机。
// pthread_setcanceltype()函数的语法格式如下：
// int pthread_setcanceltype( int type , int * oldtype );
// type参数有两个可选值，分别是：
// 1.PTHREAD_CANCEL_DEFERRED，默认值
//   当线程执行到某个可作为取消点的函数时终止执行；
// 2.PTHREAD_CANCEL_ASYNCHRONOUS
//   线程接收到Cancel信号后立即结束执行。
// oldtype参数用于接收线程先前所遵循的type值，
// 如果不需要接收该值，置为NULL即可。
// pthread_setcanceltype()函数执行成功时，返回数字0，反之返回非零数。
void *thread_Fun5(void *arg) {
  printf("新建线程开始执行\n");
  int res;
  // 设置线程为可取消状态：
  res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (res != 0) {
    printf("修改线程可取消状态失败\n");
    return NULL;
  }
  // 设置线程接收到Cancel信号后立即结束执行：
  res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  if (res != 0) {
    printf("修改线程响应Cancel信号的方式失败\n");
    return NULL;
  }
  while (1)
    ;
  return NULL;
}
void func5() {
  pthread_t myThread;
  void *mess;
  int value;
  int res;
  res = pthread_create(&myThread, NULL, thread_Fun5, NULL);
  if (res != 0) {
    printf("线程创建失败\n");
    return;
  }
  sleep(1);
  res = pthread_cancel(myThread);
  if (res != 0) {
    printf("终止myThread线程失败\n");
    return;
  }
  res = pthread_join(myThread, &mess);
  if (res != 0) {
    printf("等待线程失败\n");
    return;
  }
  if (mess == PTHREAD_CANCELED) {
    printf("myThread线程被强制终止\n");
  } else {
    printf("error\n");
  }
  return;

  // 新建线程开始执行
  // myThread线程被强制终止
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s  i [0 - 4]\n", argv[0]);
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0:
      func1();
      break;
    case 1:
      func2();
      break;
    case 2:
      func3();
      break;
    case 3:
      func4();
      break;
    case 4:
      func5();
      break;
    default:
      printf("invalid type\n");
      break;
  }

  return 0;
}

// gcc abort.c -lpthread
