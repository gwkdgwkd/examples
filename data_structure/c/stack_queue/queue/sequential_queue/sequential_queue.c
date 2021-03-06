#include "sequential_queue.h"

// 队列，和栈一样，也是一种对数据的"存"和"取"有严格要求的线性存储结构。
// 与栈结构不同的是，队列的两端都"开口"，要求数据只能从一端进，从另一端出。
// 通常，称进数据的一端为"队尾"，出数据的一端为"队头"，数据元素进队列的过程称为"入队"，出队列的过程称为"出队"。
// 队列中数据的进出要遵循"先进先出"的原则，即最先进队列的数据元素，同样要最先出队列。
// 栈和队列不要混淆，栈结构是一端封口，特点是"先进后出"；而队列的两端全是开口，特点是"先进先出"。
// 数据从表的一端进，从另一端出，且遵循"先进先出"原则的线性存储结构就是队列。

// 队列存储结构的实现有以下两种方式：
//  顺序队列：在顺序表的基础上实现的队列结构；
//  链队列：在链表的基础上实现的队列结构；
// 两者的区别仅是顺序表和链表的区别，即在实际的物理空间中，数据集中存储的队列是顺序队列，分散存储的队列是链队列。
// 实际生活中，队列的应用随处可见，比如排队买XXX、医院的挂号系统等，采用的都是队列的结构。

// 顺序队列，即采用顺序表模拟实现的队列结构。
// 队列具有以下两个特点：
//  数据从队列的一端进，另一端出；
//  数据的入队和出队遵循"先进先出"的原则；
// 因此，只要使用顺序表按以上两个要求操作数据，即可实现顺序队列。
// 由于顺序队列的底层使用的是数组，因此需预先申请一块足够大的内存空间初始化顺序队列。
// 除此之外，为了满足顺序队列中数据从队尾进，队头出且先进先出的要求，我们还需要定义两个指针（top和rear）分别用于指向顺序队列中的队头元素和队尾元素。
// 由于顺序队列初始状态没有存储任何元素，因此top指针和rear指针重合，且由于顺序队列底层实现靠的是数组，因此top和rear实际上是两个变量，它的值分别是队头元素和队尾元素所在数组位置的下标。
#if 0
int enQueue(int *a, int rear, int data) {
  a[rear] = data;
  rear++;
  return rear;
}
void deQueue(int *a, int front, int rear) {
  // 如果front==rear，表示队列为空
  while (front != rear) {
    printf("出队元素：%d\n", a[front]);
    front++;
  }
}
#endif
// 此方法存在的问题:指针top和rear重合位置指向了a[4]而不再是a[0]。也就是说，整个顺序队列在数据不断地进队出队过程中，在顺序表中的位置不断后移。 顺序队列整体后移造成的影响是：
//  顺序队列之前的数组存储空间将无法再被使用，造成了空间浪费；
//  如果顺序表申请的空间不足够大，则直接造成程序中数组a溢出，产生溢出错误；

// 顺序队列另一种实现方法，使用巧妙的方法将顺序表打造成一个环状表。
int enQueue(int *a, int front, int rear, int data) {
  // 添加判断语句，如果rear超过max，则直接将其从a[0]重新开始存储，如果rear+1和front重合，则表示数组已满
  if ((rear + 1) % max == front) {
    printf("空间已满");
    return rear;
  }
  a[rear % max] = data;
  rear++;
  return rear;
}
int deQueue(int *a, int front, int rear) {
  // 如果front==rear，表示队列为空
  if (front == rear % max) {
    printf("队列为空");
    return front;
  }
  printf("%d\n", a[front]);
  // front不再直接+1，而是+1后同max进行比较，如果=max，则直接跳转到a[0]
  front = (front + 1) % max;
  return front;
}
// 顺序队列在判断数组是否已满时，出现下面情况：
//  当队列为空时，队列的头指针等于队列的尾指针；
//  当数组满员时，队列的头指针等于队列的尾指针；
// 顺序队列的存储状态不同，但是判断条件相同。为了对其进行区分，最简单的解决办法是：牺牲掉数组中的一个存储空间，判断数组满员的条件是：尾指针的下一个位置和头指针相遇，就说明数组满了。
