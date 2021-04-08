#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 在C语言中，我们一般使用<stdlib.h>头文件中的rand()函数来生成随机数。
// C语言中还有一个random()函数可以获取随机数，但是random()不是标准函数，不能在VC/VS等编译器通过，所以比较少用。
// rand()会随机生成一个位于0 ~ RAND_MAX之间的整数。
// RAND_MAX是<stdlib.h>头文件中的一个宏，它用来指明rand()所能返回的随机数的最大值。
// C语言标准并没有规定RAND_MAX的具体数值，只是规定它的值至少为32767。在实际编程中，也不需要知道RAND_MAX的具体值，把它当做一个很大的数来对待即可。

// 随机数的本质
// 实际上，rand()函数产生的随机数是伪随机数，是根据一个数值按照某个公式推算出来的，这个数值我们称之为“种子”。种子和随机数之间的关系是一种正态分布。
// 种子在每次启动计算机时是随机的，但是一旦计算机启动以后它就不再变化了；
// 也就是说，每次启动计算机以后，种子就是定值了，所以根据公式推算出来的结果（也就是生成的随机数）就是固定的。

// 重新播种
// 可以通过srand()函数来重新“播种”，这样种子就会发生改变。
// srand()的用法为：void srand (unsigned int seed);
// 它需要一个unsigned int类型的参数。在实际开发中，我们可以用时间作为参数，只要每次播种的时间不同，那么生成的种子就不同，最终的随机数也就不同。
// 使用<time.h>头文件中的time()函数即可得到当前的时间（精确到秒）。

// 生成一定范围内的随机数
// 在实际开发中，我们往往需要一定范围内的随机数，过大或者过小都不符合要求，那么，如何产生一定范围的随机数呢？我们可以利用取模的方法：
// int a = rand() % 10;  // 产生0~9的随机数，注意10会被整除
// 如果要规定上下限：
// int a = rand() % 51 + 13;  // 产生13~63的随机数

// 连续生成随机数

int main() {
  int a = rand();
  printf("%d\n", a);  // 1804289383
  // 多次运行上面的代码，你会发现每次产生的随机数都一样，这是怎么回事呢？为什么随机数并不随机呢？

  srand((unsigned)time(NULL));
  a = rand();
  printf("%d\n", a);

  srand((unsigned)time(NULL));
  a = rand() % 51 + 13;
  printf("%d\n", a);

  // 有时候我们需要一组随机数（多个随机数），该怎么生成呢？很容易想到的一种解决方案是使用循环，每次循环都重新播种，请看下面的代码：
  // 使用for循环生成10个随机数
  for (int i = 0; i < 10; i++) {
    srand((unsigned)time(NULL));
    a = rand();
    printf("%d ", a);
  }
  printf("\n");
  // 2002180793 2002180793 2002180793 2002180793 2002180793 2002180793 2002180793 2002180793 2002180793 2002180793
  // 这是因为，for循环运行速度非常快，在一秒之内就运行完成了，而time()函数得到的时间只能精确到秒，所以每次循环得到的时间都是一样的，
  // 这样一来，种子也就是一样的，随机数也就一样了。

  return 0;
}