#include <errno.h>  // ESRCH
#include <pthread.h>
#include <stdio.h>

// 如果想获取某个线程执行结束时返回的数据，可以调用pthread_join()函数来实现。

// pthread_join()函数声明在<pthread.h>头文件中，语法格式如下：
// int pthread_join(pthread_t thread, void **retval);
// thread参数用于指定接收哪个线程的返回值；
// retval参数表示接收到的返回值，如果thread线程没有返回值，
// 又或者不需要接收thread线程的返回值，可以将retval参数置为NULL。

// pthread_join()函数会一直阻塞调用它的线程，
// 直至目标线程执行结束（接收到目标线程的返回值），阻塞状态才会解除。
// 如果pthread_join()函数成功等到了目标线程执行结束，
// 即成功获取到目标线程的返回值，返回值为数字0；
// 反之如果执行失败，函数会根据失败原因返回相应的非零值，
// 每个非零值都对应着不同的宏，例如：
// 1.EDEADLK：检测到线程发生了死锁；
// 2.EINVAL：分为两种情况，要么目标线程本身不允许其它线程获取它的返回值，
//   要么事先就已经有线程调用pthread_join()函数获取到了目标线程的返回值；
// 3.ESRCH：找不到指定的thread线程。
// 以上这些宏都声明在<errno.h>中，如果想使用这些宏，需提前引入此头文件。

// 再次强调，一个线程执行结束的返回值只能由一个pthread_join()函数获取，
// 当有多个线程调用pthread_join()函数获取同一个线程的执行结果时，
// 哪个线程最先执行pthread_join()函数，执行结果就由那个线程获得，
// 其它线程的pthread_join()函数都将执行失败。

// 对于一个默认属性的线程A来说，线程占用的资源并不会因为执行结束而得到释放。
// 而通过在其它线程中执行pthread_join(A,NULL);语句，
// 可以轻松实现及时释放线程A所占资源的目的。

void *ThreadFun(void *arg) { pthread_exit("http://c.biancheng.net"); }
int main() {
  int res;
  void *thread_result;
  pthread_t myThread;
  res = pthread_create(&myThread, NULL, ThreadFun, NULL);
  if (res != 0) {
    printf("线程创建失败");
    return 0;
  }

  // 阻塞主线程，等待myThread线程执行结束：
  res = pthread_join(myThread, &thread_result);
  if (res != 0) {
    printf("1：等待线程失败");
  }
  // 输出获取到的myThread线程的返回值：
  printf("%s\n", (char *)thread_result);

  // 尝试再次获取myThread线程的返回值：
  res = pthread_join(myThread, &thread_result);
  if (res == ESRCH) {
    printf("2：等待线程失败，线程不存在\n");
  }
  return 0;

  // 第一个成功执行的pthread_join()会使myThread释放自己占用的资源，
  // myThread也就不复存在，所以第二个pthread_join()会返回ESRCH：

  // http://c.biancheng.net
  // 2：等待线程失败，线程不存在
}

// gcc return.c -lpthread