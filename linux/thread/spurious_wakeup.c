#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// Linux中帮助中提到，在多核处理器下，当一个线程调用pthread_cond_signal()后，
// 可能会激活多于一个阻塞在条件变量上线程，多个调用pthread_cond_wait()的线程返回，
// 调用pthread_cond_timedwait()的线程也会返回，这种效应称为虚假唤醒。
// 调用pthread_cond_broadcast会唤醒所有等待线程，也会出现虚拟唤醒情况。
// 虽然虚假唤醒在pthread_cond_wait函数中可以解决，为了发生概率很低的情况而降低边缘条件，
// 效率是不值得的，纠正这个问题会降低对所有基于它的所有更高级的同步操作的并发度。
// 所以pthread_cond_wait的实现上没有去解决它。
// 添加while检查的做法被认为是增加了程序的健壮性，
// 在IEEE Std 1003.1-2001中认为spurious wakeup是允许的。

// 通常的标准解决办法是将条件的判断从if改为while：
// static void *thread_func(void *arg) {
//   while (1) {
//     // 这个mutex主要是用来保证pthread_cond_wait的并发性：
//     pthread_mutex_lock(&mtx);
//     // pthread_cond_wait里的线程可能会被意外唤醒（虚假唤醒），
//     // 如果不是想要的情况，应该让线程继续进入pthread_cond_wait：
//     while (msg_list.empty()) {
//       pthread_cond_wait(&cond, &mtx);
//     }
//     msg = msg_list.pop();
//     // 临界区数据操作完毕，释放互斥锁：
//     pthread_mutex_unlock(&mtx);
//   }
//   return 0;
// }

// 如果不存在虚假唤醒的情况，那么下面代码：
// while (msg_list.empty()) {
//   pthread_cond_wait(&cond, &mtx);
// }
// 可以改为：
// if (msg_list.empty()) {
//   pthread_cond_wait(&cond, &mtx);
// }

// 但是存在虚假唤醒的时候，如果用if，而不用while，那么被虚假唤醒的时候，
// 不会再次while判断，而是继续下面执行msg=msg_list.pop();
// 这其实是逻辑上有问题的，因为代码已经假定了msg_list不是空的。

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flag = 1;
void *thread(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);
    while (flag != 0) {
      printf("thread %d: wait\n", *(int *)arg);
      pthread_cond_wait(&cond, &mutex);
      printf("thread %d: get cond, %d\n", *(int *)arg, flag);
      flag = 0;
      printf("thread %d: end\n", *(int *)arg);
    }
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(0);
}

int main() {
  pthread_t id;

  int i = 0;
  pthread_create(&id, NULL, thread, (void *)&i);
  int j = 1;
  pthread_create(&id, NULL, thread, (void *)&j);
  sleep(3);
  printf("broadcast\n");
  pthread_cond_broadcast(&cond);

  sleep(2);
  return 0;

  // thread 0: wait
  // thread 1: wait
  // broadcast
  // thread 1: get cond, 1
  // thread 1: end
  // thread 0: get cond, 0
  // thread 0: end

  // 主线程调用pthread_cond_broadcast后，线程1比线程0先唤醒，
  // 线程1在pthread_cond_wait函数中成功加锁，
  // 此时线程0虽唤醒但阻塞在pthread_cond_wait中的加锁操作，
  // 线程1修改完flag值后释放锁，线程0拿到锁继续执行pthread_cond_wait，
  // 但flag值已被线程1更改，所以这次唤醒对于线程0来说是虚假唤醒。
}

// gcc spurious_wakeup.c -lpthread
