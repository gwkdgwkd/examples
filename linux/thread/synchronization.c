#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 多线程中各个线程除了可以使用自己的私有资源（局部变量、函数形参等）外，
// 还可以共享全局变量、静态变量、堆内存、打开的文件等资源。

int ticket_sum = 10;

void *sell_ticket1(void *arg) {
  int i;
  // 4个售票员负责将10张票全部卖出：
  for (i = 0; i < 10; i++) {
    // 直至所有票全部卖出，4个售票员才算完成任务：
    if (ticket_sum > 0) {
      sleep(1);
      // 每个线程代表一个售票员：
      printf("%u卖第%d张票\n", (unsigned)pthread_self(), 10 - ticket_sum + 1);
      ticket_sum--;
    }
  }
  return 0;
}

void func1() {
  int flag;
  int i;
  void *ans;

  // 创建4个线程，代表4个售票员：
  pthread_t tids[4];
  for (i = 0; i < 4; i++) {
    flag = pthread_create(&tids[i], NULL, &sell_ticket1, NULL);
    if (flag != 0) {
      printf("线程创建失败!");
      return;
    }
  }
  // 阻塞主线程，等待所有子线程执行结束：
  sleep(10);

  for (i = 0; i < 4; i++) {
    flag = pthread_join(tids[i], &ans);
    if (flag != 0) {
      printf("tid=%d等待失败！", (int)tids[i]);
      return;
    }
  }
  return;

  // 3080312576卖第1张票
  // 3071919872卖第1张票
  // 3055134464卖第3张票
  // 3063527168卖第1张票
  // 3080312576卖第5张票
  // 3071919872卖第6张票
  // 3063527168卖第7张票
  // 3055134464卖第7张票
  // 3080312576卖第9张票
  // 3063527168卖第10张票
  // 3055134464卖第10张票
  // 3071919872卖第9张票
  // 3080312576卖第13张票

  // 2180495104卖第1张票
  // 2155316992卖第2张票
  // 2163709696卖第2张票
  // 2172102400卖第1张票
  // 2180495104卖第5张票
  // 2155316992卖第6张票
  // 2163709696卖第7张票
  // 2172102400卖第8张票
  // 2180495104卖第9张票
  // 2163709696卖第10张票
  // 2172102400卖第10张票
  // 2155316992卖第9张票
  // 2180495104卖第13张票

  // 造成此类问题的根本原因在于，进程中公有资源的访问权限是完全开放的，
  // 各个线程可以随时访问这些资源，
  // 程序运行过程中很容易出现多个线程同时访问某公共资源的情况。
  // 之所以会出现多个售票员卖出同一张票的情况，
  // 因为这些线程几乎同一时间访问ticket_sum变量，得到的是相同的值。
  // 出现4个售票员多卖出3张票的原因是：
  // 4个线程访问ticket_sum变量得到的都是一个大于0的数，
  // 每个线程都可以继续执行if语句内的代码。
  // 由于各个线程先后执行的顺序不同，有的线程先执行ticket_sum--操作，
  // 导致其它线程计算10-ticket_sum+1表达式时，
  // 读取到的ticket_num变量的值为负数，因此表达式的值会出现大于10的情况。
}

// 通常将多个线程同时访问某一公共资源的现象，
// 称为线程间产生了资源竞争或者线程间抢夺公共资源，
// 线程间竞争资源往往会导致程序的运行结果出现异常，
// 感到匪夷所思，严重时还会导致程序运行崩溃。

// 幸运地是，Linux提供了很多种解决方案，
// 确定各个线程可以同步访问进程提供的公共资源，简称线程同步。
// 所谓线程同步，简单地理解就是：
// 当一个线程访问某公共资源时，其它线程不得访问该资源，
// 它们只能等待此线程访问完成后，再逐个访问该资源。

// Linux环境中，实现线程同步的常用方法有4种：
// 1.互斥锁（Mutex）又称互斥量或者互斥体，
//   是最简单也最有效地一种线程同步机制。
//   互斥锁的用法和实际生活中的锁非常类似，当一个线程访问公共资源时，
//   会及时地锁上该资源，阻止其它线程访问；
//   访问结束后再进行解锁操作，将该资源让给其它线程访问。
// 2.信号量又称信号灯，主要用于控制同时访问公共资源的线程数量，
//   当线程数量控制在≤1时，该信号量又称二元信号量，功能和互斥锁非常类似；
//   当线程数量控制在N（≥2）个时，该信号量又称多元信号量，
//   指的是同一时刻最多只能有N个线程访问该资源。
// 3.条件变量的功能类似于实际生活中的门，门有打开和关闭两种状态，
//   分别对应条件变量的成立状态和不成立状态。
//   当条件变量不成立时，任何线程都无法访问资源，只能等待条件变量成立；
//   一旦条件变量成立，所有等待的线程都会恢复执行，访问目标资源。
//   为了防止各个线程竞争资源，条件变量总是和互斥锁搭配使用。
// 4.多线程中，如果大多数线程都是对公共资源执行读取操作，
//   仅有少量的线程对公共资源进行修改，
//   这种情况下可以使用读写锁解决线程同步问题。

// 互斥锁实现多线程同步的核心思想是：
// 有线程访问进程空间中的公共资源时，该线程执行加锁操作，阻止其它线程访问。
// 访问完成后，该线程负责完成解锁操作，将资源让给其它线程。
// 当有多个线程想访问资源时，谁最先完成加锁操作，谁就最先访问资源。

// 当有多个线程想访问加锁状态下的公共资源时，它们只能等待资源解锁，
// 所有线程会排成一个等待（阻塞）队列。
// 资源解锁后，操作系统会唤醒等待队列中的所有线程，
// 第一个访问资源的线程会率先将资源锁起来，其它线程则继续等待。

// 本质上，互斥锁就是一个全局变量，它只有lock和unlock两个值，含义是：
// 1.unlock表示当前资源可以访问，
//   第一个访问资源的线程负责将互斥锁的值改为lock，
//   访问完成后再重置为unlock；
// 2.lock表示有线程正在访问资源，
//   其它线程需等待互斥锁的值为unlock后才能开始访问。
// 通过对资源进行加锁和解锁，可以确保同一时刻最多有1个线程访问该资源，
// 从根本上避免了多线程抢夺资源的情况发生。
// 再次强调，对资源进行加锁和解锁操作的必须是同一个线程。
// 换句话说，哪个线程对资源执行了加锁操作，那么解锁操作也必须由该线程负责。

// POSIX标准规定，用pthread_mutex_t类型的变量来表示一个互斥锁，
// 该类型以结构体的形式定义在<pthread.h>头文件中。

// 初始化pthread_mutex_t变量的方式有两种，分别为：
// 1.使用特定的宏
//   pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
// 2.调用初始化的函数
//   pthread_mutex_t myMutex;
//   pthread_mutex_init(&myMutex, NULL);
// 以上两种初始化方式是完全等价的，都定义在<pthread.h>头文件中，
// 它们的主要区别在于：
// 1.pthread_mutex_init()函数可以自定义互斥锁的属性。
// 2.对于调用malloc()函数分配动态内存的互斥锁，只能以第2种方法完成初始化；

// pthread_mutex_init()函数专门用于初始化互斥锁，语法格式如下：
// int pthread_mutex_init(pthread_mutex_t *mutex,
//                        const pthread_mutexattr_t *attr);
// mutex参数表示要初始化的互斥锁；
// attr参数用于自定义新建互斥锁的属性，
// attr的值为NULL时表示以默认属性创建互斥锁。
// pthread_mutex_init()函数成功完成初始化操作时，
// 返回数字0；如果初始化失败，函数返回非零数。
// 注意，不能对一个已经初始化过的互斥锁再进行初始化操作，
// 否则会导致程序出现无法预料的错误。

// 对于互斥锁的加锁和解锁操作，常用的函数有以下3种：
// int pthread_mutex_lock(pthread_mutex_t* mutex);
// int pthread_mutex_trylock(pthread_mutex_t* mutex);
// int pthread_mutex_unlock(pthread_mutex_t* mutex);
// 参数mutex表示我们要操控的互斥锁。
// 函数执行成功时返回数字0，否则返回非零数。
// pthread_mutex_unlock()
// 函数用于对指定互斥锁进行解锁操作，
// pthread_mutex_lock()和pthread_mutex_trylock()
// 函数都用于实现加锁操作，不同之处在于当互斥锁已经处于加锁状态时：
// 1.执行pthread_mutex_lock()函数会使线程进入等待（阻塞）状态，
//   直至互斥锁得到释放；
// 2.执行pthread_mutex_trylock()函数不会阻塞线程，
//   直接返回非零数（表示加锁失败）。

// 对于使用动态内存创建的互斥锁，例如：
// pthread_mutex_t *myMutex =
//   (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
// pthread_mutex_init(myMutex , NULL);
// 手动释放myMutex占用的内存（调用free()函数）之前，
// 必须先调用pthread_mutex_destory()函数销毁该对象。
// pthread_mutex_destory()函数用于销毁创建好的互斥锁，语法格式如下：
// int pthread_mutex_destroy(pthread_mutex_t *mutex);
// 参数mutex表示要销毁的互斥锁。
// 如果函数成功销毁指定的互斥锁，返回数字0，反之返回非零数。
// 注意，对于用PTHREAD_MUTEX_INITIALIZER，
// 或者pthread_mutex_init()函数直接初始化的互斥锁，
// 无需调用pthread_mutex_destory()函数手动销毁。

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
void *sell_ticket2(void *arg) {
  // 输出当前执行函数的线程ID：
  printf("当前线程ID：%u\n", (unsigned)pthread_self());
  int i;
  int islock = 0;
  for (i = 0; i < 10; i++) {
    // 当前线程加锁：
    islock = pthread_mutex_lock(&myMutex);
    // 如果加锁成功，执行如下代码：
    if (islock == 0) {
      // 如果票数>0 ,开始卖票：
      if (ticket_sum > 0) {
        sleep(1);
        printf("%u卖第%d张票\n", (unsigned)pthread_self(), 10 - ticket_sum + 1);
        ticket_sum--;
      }
      // 当前线程模拟完卖票过程，执行解锁操作：
      pthread_mutex_unlock(&myMutex);
    }
  }
  return 0;
}
void func2() {
  int flag;
  int i;
  void *ans;

  pthread_t tids[4];
  for (i = 0; i < 4; i++) {
    flag = pthread_create(&tids[i], NULL, &sell_ticket2, NULL);
    if (flag != 0) {
      printf("线程创建失败!");
      return;
    }
  }
  sleep(10);
  for (i = 0; i < 4; i++) {
    flag = pthread_join(tids[i], &ans);
    if (flag != 0) {
      printf("tid=%d等待失败！", (int)tids[i]);
      return;
    }
  }
  return;

  // 当前线程ID：258115328
  // 当前线程ID：134215424
  // 当前线程ID：249722624
  // 当前线程ID：241329920
  // 258115328卖第1张票
  // 258115328卖第2张票
  // 258115328卖第3张票
  // 258115328卖第4张票
  // 258115328卖第5张票
  // 258115328卖第6张票
  // 258115328卖第7张票
  // 258115328卖第8张票
  // 258115328卖第9张票
  // 258115328卖第10张票

  // 怎么都是一个人卖的？
}

// 和互斥锁类似，信号量本质也是一个全局变量。
// 不同之处在于，互斥锁的值只有2个（lock和unlock），
// 而信号量的值可以根据实际场景的需要自行设置（取值范围为≥0）。
// 更重要的是，信号量还支持做加1或者减1运算，
// 且修改值的过程以原子操作的方式实现。

// 原子操作是指当多个线程试图修改同一个信号量的值时，
// 各线程修改值的过程不会互相干扰。
// 例如信号量的初始值为1，此时有2个线程试图对信号量做加1操作，
// 则信号量的值最终一定是3，而不会是其它的值。
// 反之若不以原子操作方式修改信号量的值，
// 那么最终的计算结果还可能是2，
// 两个线程同时读取到的值为1，各自在其基础上加1，得到的结果即为2。

// 多线程中，使用信号量需遵守以下几条规则：
// 1.信号量的值不能小于0；
// 2.有线程访问资源时，信号量执行减1操作，访问完成后再执行加1操作；
// 3.当信号量的值为0时，想访问资源的线程必须等待，
//   直至信号量的值大于0，等待的线程才能开始访问。

// 根据初始值的不同，信号量可以细分为2类，分别为二进制信号量和计数信号量：
// 1.二进制信号量：指初始值为1的信号量，此类信号量只有1和0两个值，
//   通常用来替代互斥锁实现线程同步；
// 2.计数信号量：指初始值大于1的信号量，当进程中存在多个线程，
//   但某公共资源允许同时访问的线程数量是有限的（出现了狼多肉少的情况），
//   这时就可以用计数信号量来限制同时访问资源的线程数量。

// POSIX标准中，信号量用sem_t类型的变量表示，
// 该类型定义在<semaphore.h>头文件中。
// sem_init()函数专门用来初始化信号量，语法格式如下：
// int sem_init(sem_t *sem, int pshared, unsigned int value);
// 各个参数的含义分别为：
// sem：表示要初始化的目标信号量；
// pshared：表示该信号量是否可以和其他进程共享，
// pshared值为0时表示不共享，值为1时表示共享；
// value：设置信号量的初始值。
// 当sem_init()成功完成初始化操作时，返回值为0，否则返回-1。

// 对于初始化了的信号量，可以借助<semaphore.h>提供的一些函数操作它，比如：
// int sem_post(sem_t* sem);
// int sem_wait(sem_t* sem);
// int sem_trywait(sem_t* sem);
// int sem_destroy(sem_t* sem);
// 参数sem都表示要操作的目标信号量。
// 各个函数的功能如下：
// 1.sem_post()函数的功能是：
//   将信号量的值加1，同时唤醒其它等待访问资源的线程；
// 2.当信号量的值大于0时，sem_wait()函数会对信号量做减1操作；
//   当信号量的值为0时，sem_wait()函数会阻塞当前线程，
//   直至有线程执行sem_post()函数，暂停的线程才会继续执行；
// 3.sem_trywait()函数的功能和sem_wait()函数类似，
//   唯一的不同在于，当信号量的值为0时，
//   sem_trywait()函数并不会阻塞当前线程，而是立即返回-1；
// 4.sem_destory()函数用于手动销毁信号量。
// 以上函数执行成功时，返回值均为0；如果执行失败，返回值均为-1。

sem_t mySem;
void *sell_ticket3(void *arg) {
  printf("当前线程ID：%u\n", (unsigned)pthread_self());
  int i;
  int flag;
  for (i = 0; i < 10; i++) {
    // 完成信号量减1操作，否则暂停执行：
    flag = sem_wait(&mySem);
    if (flag == 0) {
      if (ticket_sum > 0) {
        sleep(1);
        printf("%u卖第%d张票\n", (unsigned)pthread_self(), 10 - ticket_sum + 1);
        ticket_sum--;
      }
      // 执行加1操作：
      sem_post(&mySem);
      sleep(1);
    }
  }
  return 0;
}
void func3() {
  int flag;
  int i;
  void *ans;
  pthread_t tids[4];
  // 初始化信号量：
  flag = sem_init(&mySem, 0, 1);
  if (flag != 0) {
    printf("初始化信号量失败\n");
  }
  for (i = 0; i < 4; i++) {
    flag = pthread_create(&tids[i], NULL, &sell_ticket3, NULL);
    if (flag != 0) {
      printf("线程创建失败!");
      return;
    }
  }
  sleep(10);
  for (i = 0; i < 4; i++) {
    flag = pthread_join(tids[i], &ans);
    if (flag != 0) {
      printf("tid=%d等待失败！", (int)tids[i]);
      return;
    }
  }
  // 执行结束前，销毁信号量：
  sem_destroy(&mySem);
  return;

  // 当前线程ID：2431469312
  // 当前线程ID：2423076608
  // 当前线程ID：2414683904
  // 当前线程ID：2406291200
  // 2431469312卖第1张票
  // 2423076608卖第2张票
  // 2414683904卖第3张票
  // 2406291200卖第4张票
  // 2414683904卖第5张票
  // 2423076608卖第6张票
  // 2431469312卖第7张票
  // 2406291200卖第8张票
  // 2414683904卖第9张票
  // 2423076608卖第10张票
}

int num = 5;
sem_t sem;
void *get_service(void *arg) {
  int id = *((int *)arg);
  // 信号量成功减1后才能继续执行：
  if (sem_wait(&sem) == 0) {
    // sem信号量的初始化为2，因此该信号量属于计数信号量。
    // 借助sem信号量，这里的代码段最多只能有2个线程同时访问。
    printf("---customer%d正在办理业务\n", id);
    sleep(2);
    printf("---customer%d已办完业务\n", id);
    // 信号量加1：
    sem_post(&sem);
  }
  return 0;
}
void func4() {
  int flag, i, j;
  pthread_t customer[5];
  sem_init(&sem, 0, 2);
  for (i = 0; i < num; i++) {
    flag = pthread_create(&customer[i], NULL, get_service, &i);
    if (flag != 0) {
      printf("线程创建失败!\n");
      return;
    } else {
      printf("customer%d来办理业务\n", i);
    }
    sleep(1);
  }
  for (j = 0; j < num; j++) {
    flag = pthread_join(customer[j], NULL);
    if (flag != 0) {
      printf("tid=%d等待失败！", (int)customer[i]);
      return;
    }
  }
  sem_destroy(&sem);
  return;

  // customer0来办理业务
  // ---customer0正在办理业务
  // customer1来办理业务
  // ---customer1正在办理业务
  // ---customer0已办完业务
  // customer2来办理业务
  // ---customer2正在办理业务
  // ---customer1已办完业务
  // customer3来办理业务
  // ---customer3正在办理业务
  // ---customer2已办完业务
  // customer4来办理业务
  // ---customer4正在办理业务
  // ---customer3已办完业务
  // ---customer4已办完业务
}

// 假设一个进程中包含多个线程，这些线程共享变量x，
// 希望某个线程等待x==10条件成立后再执行后续的代码，该如何实现呢？
// 您可能想到用while循环实现，例如：
// void *threadFun(void *args) {
//   while (x != 10) {
//     sleep(5);
//   }
//   // 待条件成立后，执行后续的代码
// }
// 当线程执行此函数时，会判断x的值是否等于10，
// 如果不等则间隔5秒后再重复判断，
// 直到x的值等于10，线程才能执行后续的代码。
// 直观上看，while循环确实能够阻塞线程，但这种方法存在严重的效率问题。
// 当线程因条件不成立进入等待状态时，如果此时恰好有另一个线程将x的值改为10，
// 该线程必须等待5秒后才能继续执行。
// 如果将等待时间缩短（或者直接将sleep(5)注释掉），
// 线程将反复判断x的值是否等于10，它可能会一直霸占着CPU资源，
// 导致其它线程无法执行，x变量的值会出现长时间不改变的情况。
// 针对类似的场景，推荐您用条件变量来实现。

// 和互斥锁、信号量类似，条件变量本质也是一个全局变量，它的功能是阻塞线程，
// 直至接收到条件成立的信号后，被阻塞的线程才能继续执行。
// 一个条件变量可以阻塞多个线程，这些线程会组成一个等待队列。
// 当条件成立时，条件变量可以解除线程的被阻塞状态。
// 也就是说，条件变量可以完成以下两项操作：
// 1.阻塞线程，直至接收到条件成立的信号；
// 2.向等待队列中的一个或所有线程发送条件成立的信号，解除它们的被阻塞状态。
// 为了避免多线程之间发生抢夺资源的问题，
// 条件变量在使用过程中必须和一个互斥锁搭配使用。

// POSIX标准中，条件变量用pthread_cond_t类型的变量表示，
// 此类型定义在<pthread.h>头文件中。

// 初始化条件变量的方式有两种：
// 1.直接将PTHREAD_COND_INITIALIZER赋值给条件变量，例如：
//   pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;
// 2.借助pthread_cond_init()函数初始化条件变量，语法格式如下：
//   int pthread_cond_init(pthread_cond_t *cond,
//                         const pthread_condattr_t *attr);
// 参数cond用于指明要初始化的条件变量；
// 参数attr用于自定义条件变量的属性，通常我们将它赋值为NULL，
// 表示以系统默认的属性完成初始化操作。
// pthread_cond_init()函数初始化成功时返回数字0，反之函数返回非零数。
// 当attr参数为NULL时，以上两种初始化方式完全等价。

// 当条件不成立时，条件变量可以阻塞当前线程，
// 所有被阻塞的线程会构成一个等待队列。
// 阻塞线程可以借助以下两个函数实现：
// int pthread_cond_wait(pthread_cond_t* cond,
//                       pthread_mutex_t* mutex);
// int pthread_cond_timedwait(pthread_cond_t* cond,
//                            pthread_mutex_t* mutex,
//                            const struct timespec* abstime);
// cond参数表示已初始化好的条件变量；
// mutex参数表示与条件变量配合使用的互斥锁；
// abstime参数表示阻塞线程的时间。
// 注意，abstime参数指的是绝对时间，例如您打算阻塞线程5秒钟，
// 那么首先要得到当前系统的时间，然后再加上5秒，
// 最终得到的时间才是传递的实参值。
// 调用两个函数之前，必须先创建好一个互斥锁并完成加锁操作，
// 然后才能作为实参传递给mutex参数。
// 两个函数会完成以下两项工作：
// 1.阻塞线程，直至接收到条件成立的信号；
// 2.当线程被添加到等待队列上时，将互斥锁解锁。
// 也就是说，函数尚未接收到条件成立的信号之前，它将一直阻塞线程执行。
// 注意，当函数接收到条件成立的信号后，它并不会立即结束对线程的阻塞，
// 而是先完成对互斥锁的加锁操作，然后才解除阻塞。
// 两个函数都以原子操作的方式完成阻塞线程+解锁或者重新加锁+解除阻塞这两个过程。
// 以上两个函数都能用来阻塞线程，它们的区别在于：
// 1.pthread_cond_wait()函数可以永久阻塞线程，直到条件变量成立的那一刻；
// 2.pthread_cond_timedwait()函数只能在abstime参数指定的时间内阻塞线程，
//   超出时限后，该函数将重新对互斥锁执行加锁操作，
//   解除对线程的阻塞，函数的返回值为ETIMEDOUT。
// 如果函数成功接收到了条件成立的信号，
// 重新对互斥锁完成了加锁并使线程继续执行，函数返回数字0，反之则返回非零数。
// POSIX标准规定，这两个函数可以作为取消点的函数。
// 当线程接收到强制终止执行的信号后，执行到这两个函数时，线程就会终止执行。

// 对于被pthread_cond_wait()或pthread_cond_timedwait()函数阻塞的线程，
// 可以借助如下两个函数向它们发送条件成立的信号，解除它们的被阻塞状态：
// int pthread_cond_signal(pthread_cond_t* cond);
// int pthread_cond_broadcast(pthread_cond_t* cond);
// cond参数表示初始化好的条件变量。
// 当函数成功解除线程的被阻塞状态时，返回数字0，反之返回非零数。
// 两个函数都能解除线程的被阻塞状态，区别在于：
// 1.pthread_cond_signal()函数至少解除一个线程的被阻塞状态，
//   如果等待队列中包含多个线程，
//   优先解除哪个线程将由操作系统的线程调度程序决定；
// 2.pthread_cond_broadcast()函数可以解除等待队列中所有线程的被阻塞状态。
// 由于互斥锁的存在，解除阻塞后的线程也不一定能立即执行。
// 当互斥锁处于加锁状态时，解除阻塞状态的所有线程会组成等待互斥锁资源的队列，
// 等待互斥锁解锁。

// 对于初始化好的条件变量，可以调用pthread_cond_destory()函数销毁它。
// pthread_cond_destory() 函数的语法格式如下：
// int pthread_cond_destroy(pthread_cond_t *cond);
// cond参数表示要销毁的条件变量。
// 如果函数成功销毁cond参数指定的条件变量，返回数字0，反之返回非零数。
// 值得一提的是，
// 销毁后的条件变量还可以调用pthread_cond_init()函数重新初始化后使用。

// 初始化互斥锁：
pthread_mutex_t myMutex5 = PTHREAD_MUTEX_INITIALIZER;
// 初始化条件变量：
pthread_cond_t myCond5 = PTHREAD_COND_INITIALIZER;
// 设置全局变量：
int x = 0;
void *waitForTrue(void *args) {
  int res;
  // 条件变量阻塞线程之前，先对互斥锁执行加锁操作：
  res = pthread_mutex_lock(&myMutex5);
  if (res != 0) {
    printf("waitForTrue加锁失败\n");
    return NULL;
  }
  printf("------等待x的值为10\n");
  if (pthread_cond_wait(&myCond5, &myMutex5) == 0) {
    printf("x = %d\n", x);
  }
  // 最终将互斥锁解锁：
  pthread_mutex_unlock(&myMutex5);
  return NULL;
}
void *doneForTrue(void *args) {
  int res;
  while (x != 10) {
    // 对互斥锁执行加锁操作：
    res = pthread_mutex_lock(&myMutex5);
    if (res == 0) {
      x++;
      printf("doneForTrue：x = %d\n", x);
      sleep(1);
      // 对互斥锁解锁：
      pthread_mutex_unlock(&myMutex5);
    }
  }
  // 发送条件成立的信号，解除mythread1线程的被阻塞状态：
  res = pthread_cond_signal(&myCond5);
  if (res != 0) {
    printf("解除阻塞失败\n");
  }
  return NULL;
}
void func5() {
  int res;
  pthread_t mythread1, mythread2;
  res = pthread_create(&mythread1, NULL, waitForTrue, NULL);
  if (res != 0) {
    printf("mythread1线程创建失败\n");
    return;
  }
  res = pthread_create(&mythread2, NULL, doneForTrue, NULL);
  if (res != 0) {
    printf("mythread2线程创建失败\n");
    return;
  }
  // 等待mythread1线程执行完成：
  res = pthread_join(mythread1, NULL);
  if (res != 0) {
    printf("1：等待线程失败\n");
  }
  // 等待mythread2线程执行完成：
  res = pthread_join(mythread2, NULL);
  if (res != 0) {
    printf("2：等待线程失败\n");
  }
  // 销毁条件变量：
  pthread_cond_destroy(&myCond5);
  return;

  // ------等待x的值为10
  // doneForTrue：x = 1
  // doneForTrue：x = 2
  // doneForTrue：x = 3
  // doneForTrue：x = 4
  // doneForTrue：x = 5
  // doneForTrue：x = 6
  // doneForTrue：x = 7
  // doneForTrue：x = 8
  // doneForTrue：x = 9
  // doneForTrue：x = 10
  // x = 10
}

// 多线程程序中，如果仅有少数线程会对共享数据进行修改，
// 多数线程只是读取共享数据的值，就适合用读写锁解决线程间抢夺资源的问题。
// 读写锁的核心思想是：将线程访问共享数据时发出的请求分为两种，分别是：
// 1.读请求：只读取共享数据，不做任何修改；
// 2.写请求：存在修改共享数据的行为。

// 当有多个线程发出读请求时，这些线程可以同时执行，也就是说，
// 共享数据的值可以同时被多个发出读请求的线程获取；
// 当有多个线程发出写请求时，这些线程只能一个一个地执行（同步执行）。
// 此外，当发出读请求的线程正在执行时，
// 发出写请求的线程必须等待前者执行完后才能开始执行；
// 当发出写请求的线程正在执行时，
// 发出读请求的线程也必须等待前者执行完后才能开始执行。

// 本质上，读写锁就是一个全局变量，发出读请求和写请求的线程都可以访问它。
// 为了区别线程发出的请求类别，当读写锁被发出读请求的线程占用时，称它为读锁；
// 当读写锁被发出写请求的线程占用时，称它为写锁。

// 当前读写锁的状态 线程发出读请求 线程发出写请求
// 无锁           允许占用      允许占用
// 读锁           允许占用      阻塞线程执行
// 写锁           阻塞线程执行   阻塞线程执行
// 1.当读写锁未被任何线程占用时，发出读请求和写请求的线程都可以占用它。
//   注意，由于读请求和写请求的线程不能同时执行，
//   读写锁默认会优先分配给发出读请求的线程。
// 2.当读写锁的状态为读锁时，表明当前执行的是发出读请求的线程（可能有多个）。
//   此时如果又有线程发出读请求，该线程不会被阻塞，
//   但如果有线程发出写请求，它就会被阻塞，直到读写锁状态改为无锁。
// 3.当读写锁状态为写锁时，表明当前执行的是发出写请求的线程（只能有1个）。
//   此时无论其它线程发出的是读请求还是写请求，
//   都必须等待读写锁状态改为无锁后才能执行。
// 总的来说，对于进程空间中的共享资源，读写锁允许发出读请求的线程共享资源，
// 发出写请求的线程必须独占资源，进而实现线程同步。

// POSIX标准中，读写锁用pthread_rwlock_t类型的变量表示，
// 此类型定义在<pthread.h>头文件中。

// 初始化读写锁的方法有两种：
// 1.直接将PTHREAD_RWLOCK_INITIALIZER宏赋值给读写锁变量，例如：
//   pthread_rwlock_t myRWLock = PTHREAD_RWLOCK_INITIALIZER;
// 2.借助pthread_rwlock_init()函数初始化读写锁，此函数的语法格式为：
//   int pthread_rwlock_init(pthread_rwlock_t *rwlock,
//                           const pthread_rwlockattr_t *attr);
// rwlock参数用于指定要初始化的读写锁变量；
// attr参数用于自定义读写锁变量的属性，为NULL时表示以默认属性初始化读写锁。
// 当pthread_rwlock_init()函数初始化成功时，返回数字0，反之返回非零数。
// 当attr参数为NULL时，以上两种初始化方式完全等价。

// 通过以下两个函数，线程可以向读写锁发出读锁请求：
// int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
// int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock);
// 其中，rwlock参数指的是初始化好的读写锁。
// 当读写锁处于无锁或者读锁状态时，以上两个函数都能成功获得读锁；
// 读写锁处于写锁状态时：
// pthread_rwlock_rdlock()函数会阻塞当前线程，直至读写锁被释放；
// pthread_rwlock_tryrdlock()函数不会阻塞当前线程，直接返回EBUSY。
// 以上两个函数如果能成功获得读锁，函数返回数字0，反之返回非零数。

// 通过以下两个函数，线程可以向读写锁发出写锁请求：
// int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);
// int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock);
// rwlock参数指的是初始化好的读写锁。
// 当读写锁处于无锁状态时，两个函数都能成功获得写锁；
// 当读写锁处于读锁或写锁状态时：
// pthread_rwlock_wrlock()函数将阻塞线程，直至读写锁被释放；
// pthread_rwlock_trywrlock()函数不会阻塞线程，直接返回EBUSY。
// 以上两个函数如果能成功获得写锁，函数返回数字0，反之返回非零数。

// 无论是处于无锁、读锁还是写锁的读写锁，都可以使用如下函数释放读写锁：
// int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);
// rwlock参数表示要释放的读写锁。
// 当函数成功释放读写锁时，返回数字0，反之则返回非零数。
// 注意，由于多个线程可以同时获得读锁状态下的读写锁，
// 这种情况下一个线程释放读写锁后，读写锁仍处于读锁状态，
// 直至所有线程都释放读写锁，读写锁的状态才为无锁状态。

// 当读写锁不再使用时，可以借助如下函数将它销毁：
// int pthread_rwlock_destroy(pthread_rwlock_t* rwlock);
// 参数rwlock表示要销毁的目标读写锁。
// 如果函数成功销毁指定的读写锁，返回数字0，反之则返回非零数。

pthread_rwlock_t myrwlock;
void *read_thread(void *args) {
  printf("------%uread_thread ready\n", (unsigned)pthread_self());
  while (1) {
    sleep(1);
    // 请求读锁：
    pthread_rwlock_rdlock(&myrwlock);
    printf("read_thread: %u,x=%d\n", (unsigned)pthread_self(), x);
    sleep(1);
    // 释放读写锁：
    pthread_rwlock_unlock(&myrwlock);
  }
  return NULL;
}
void *write_thread(void *param) {
  printf("------%u write_thread ready!\n", (unsigned)pthread_self());
  while (1) {
    sleep(1);
    // 请求写锁：
    pthread_rwlock_wrlock(&myrwlock);
    ++x;
    printf("write_thread: %u,x=%d\n", (unsigned)pthread_self(), x);
    sleep(1);
    // 释放读写锁：
    pthread_rwlock_unlock(&myrwlock);
  }
  return NULL;
}
void func6() {
  int i;
  // 初始化读写锁：
  pthread_rwlock_init(&myrwlock, NULL);
  // 创建3个读x变量的线程：
  pthread_t readThread[3];
  for (i = 0; i < 3; ++i) {
    pthread_create(&readThread[i], NULL, read_thread, NULL);
  }
  // 创建1个修改x变量的线程：
  pthread_t writeThread;
  pthread_create(&writeThread, NULL, write_thread, NULL);
  // 等待各个线程执行完成：
  pthread_join(writeThread, NULL);
  for (int i = 0; i < 3; ++i) {
    pthread_join(readThread[i], NULL);
  }
  // 销毁读写锁：
  pthread_rwlock_destroy(&myrwlock);
  return;

  // ------265328384read_thread ready
  // ------273721088read_thread ready
  // ------256935680read_thread ready
  // ------248542976 write_thread ready!
  // read_thread: 265328384,x=0
  // read_thread: 273721088,x=0
  // read_thread: 256935680,x=0
  // write_thread: 248542976,x=1
  // read_thread: 273721088,x=1
  // read_thread: 256935680,x=1
  // read_thread: 265328384,x=1
  // write_thread: 248542976,x=2
  // read_thread: 273721088,x=2
  // read_thread: 256935680,x=2
  // read_thread: 265328384,x=2
}

// 线程死锁指的是线程需要使用的公共资源一直被其它线程占用，
// 导致该线程一直处于阻塞状态，无法继续执行。

void *thread_func(void *arg) {
  int islock;
  // 为线程加锁：
  islock = pthread_mutex_lock(&myMutex);
  if (islock == 0) {
    printf("线程%u已加锁\n", (unsigned)pthread_self());
  }
  // 没有解锁
  return 0;
}
void func7() {
  int flag;
  int i;
  pthread_t tids[4];
  for (i = 0; i < 4; i++) {
    flag = pthread_create(&tids[i], NULL, thread_func, NULL);
    if (flag == 0) {
      printf("线程%u创建完成\n", (unsigned)tids[i]);
    }
  }
  for (i = 0; i < 4; i++) {
    pthread_join(tids[i], NULL);
    printf("线程%u执行完成\n", (unsigned)tids[i]);
  }
  return;

  // 线程2509817600创建完成
  // 线程2509817600已加锁
  // 线程2501424896创建完成
  // 线程2493032192创建完成
  // 线程2484639488创建完成
  // 线程2509817600执行完成
}

pthread_mutex_t mutex;
pthread_mutex_t mutex2;
void *test1(void *args) {
  pthread_mutex_lock(&mutex);
  printf("t1成功申请mytex锁\n");
  sleep(2);
  pthread_mutex_lock(&mutex2);
  printf("t1成功申请mytex2锁\n");
  printf("%u is running\n", (unsigned)pthread_self());
  pthread_mutex_unlock(&mutex);
  printf("------%u done\n", (unsigned)pthread_self());
}
void *test2(void *args) {
  pthread_mutex_lock(&mutex2);
  printf("t2成功申请mytex2锁\n");
  sleep(2);
  pthread_mutex_lock(&mutex);
  printf("t2成功申请mytex锁\n");
  printf("%u is running\n", (unsigned)pthread_self());
  pthread_mutex_unlock(&mutex);
  printf("------%u done\n", (unsigned)pthread_self());
}
void func8() {
  int ret;
  pthread_t t1;
  pthread_t t2;
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex2, NULL);
  ret = pthread_create(&t1, NULL, test1, NULL);
  if (ret != 0) {
    printf("create t1 fail\n");
  }
  ret = pthread_create(&t2, NULL, test2, NULL);
  if (ret != 0) {
    printf("create t2 fail\n");
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_mutex_destroy(&mutex);
  pthread_mutex_destroy(&mutex2);
  return;

  // t1成功申请了mutex锁，t2成功申请了mutex2锁，
  // t1一直等待t2释放mutex2锁，而t2一直等待t1释放mutex锁，
  // 两个线程都因等待对方释放资源产生了死锁：

  // t1成功申请mytex锁
  // t2成功申请mytex2锁
}

// 避免线程死锁的几点建议：
// 1.使用互斥锁、信号量、条件变量和读写锁实现线程同步时，要注意以下几点：
//   占用互斥锁的线程，执行完成前必须及时解锁；
//   通过sem_wait()函数占用信号量资源的线程，
//   执行完成前必须调用sem_post()函数及时释放；
//   当线程因pthread_cond_wait()函数被阻塞时，
//   一定要保证有其它线程唤醒此线程；
//   无论线程占用的是读锁还是写锁，都必须及时解锁。
//   注意，函数中可以设置多种结束执行的路径，
//   但无论线程选择哪个路径结束执行，都要保证能够将占用的资源释放掉。
// 2.POSIX标准中，很多阻塞线程的函数都有tryxxx()和timexxx()两个版本，
//   例如pthread_mutex_lock()和pthread_mutex_trylock()、
//   sem_wait()和sem_trywait()、
//   pthread_cond_wait()和pthread_cond_timedwait()等，
//   它们可以完成同样的功能，但tryxxx()版本的函数不会阻塞线程，
//   timexxx()版本的函数不会一直阻塞线程。
//   实际开发中，建议您优先选择tryxxx()或者timexxx()版本的函数，
//   可以大大降低线程产生死锁的概率。
// 3.多线程程序中，多个线程请求资源的顺序最好保持一致。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("%s  i [0 - 7]\n", argv[0]);
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
    case 5:
      func6();
      break;
    case 6:
      func7();
      break;
    case 7:
      func8();
      break;
    default:
      printf("invalid type\n");
      break;
  }

  return 0;
}