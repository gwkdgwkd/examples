#include <stdio.h>
#include <string.h>
#include <time.h>

// 一个函数在它的函数体内调用它自身称为递归调用，这种函数称为递归函数。执行递归函数将反复调用其自身，每调用一次就进入新的一层，当最内层的函数执行完毕后，再一层一层地由里到外退出。
// 递归函数不是C语言的专利，Java、C#、JavaScript、PHP等其他编程语言也都支持递归函数。

// 递归的条件
// 每一个递归函数都应该只进行有限次的递归调用，否则它就会进入死胡同，永远也不能退出了，这样的程序是没有意义的。
// 要想让递归函数逐层进入再逐层退出，需要解决两个方面的问题：
//  存在限制条件，当符合这个条件时递归便不再继续。对于factorial()，当形参n等于0或1时，递归就结束了。
//  每次递归调用之后越来越接近这个限制条件。对于factorial()，每次递归调用的实参为n-1，这会使得形参n的值逐渐减小，越来越趋近于1或0。

// 递归种类：
//  尾递归，也就是递归调用位于函数体的结尾处；
//  中间递归：发生递归调用的位置在函数体的中间；
//  多层递归：在一个函数里面多次调用自己。
// 尾递归在逐层退出时除了return语句，一般不再执行其他操作；而中间递归在逐层退出时还要执行一些其他的操作，所以比较复杂。
// 多层递归的调用关系比较复杂，整体上看起来像一颗倒立的树：对于双层递归，树的每个节点有两个分叉；对于三层递归，树的每个节点有三个分叉；以此类推……
// 双层递归的调用关系和数据结构中二叉树的结构完全吻合，所以双层递归常用于二叉树的遍历。

// 尾递归
long factorial(int n) {
  if (n == 0 || n == 1) {
    return 1;
  } else {
    return factorial(n - 1) * n;  // 递归调用
  }
}

// 中间递归
char *reverse(char *str) {
  int len = strlen(str);
  if (len > 1) {
    char ctemp = str[0];
    str[0] = str[len - 1];
    str[len - 1] = '\0';  // 最后一个字符在下次递归时不再处理
    reverse(str + 1);     // 递归调用
    str[len - 1] = ctemp;
  }
  return str;
}

// 多层递归
long fib(int n) {
  if (n <= 2) {
    return 1;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}

// 递归函数是一种强有力的技巧，用来解决某些问题很顺手，比如前面提到的求阶乘、求菲波那契数；但是和其他技巧一样，递归函数也是有缺陷的，而且这种缺陷是致命性的。
// 具体来说就是：递归函数的时间开销和内存开销都非常大，极端情况下会导致程序崩溃。
// 1 递归函数的空间开销
//  在程序占用的整个内存中，有一块内存区域叫做栈（Stack），它是专门用来给函数分配内存的，每次调用函数，都会将相关数据压入栈中，包括局部变量、局部数组、形参、寄存器、冗余数据等。
//  栈是针对线程来说的，每个线程都拥有一个栈，如果一个程序包含了多个线程，那么它就拥有多个栈。
//  对每个线程来说，栈能使用的内存是有限的，一般是1M~8M，这在编译时就已经决定了，程序运行期间不能再改变。如果程序使用的栈内存超出最大值，就会发生栈溢出（Stack Overflow）错误。
//  栈内存的大小和编译器有关，编译器会为栈内存指定一个最大值，在VC/VS下，默认是1M，在C-Free下，默认是2M，在Linux GCC下，默认是8M。当然，我们也可以通过参数来修改栈内存的大小。
//  发生函数调用时会将相关数据压入栈中，函数调用结束会释放这一部分内存，对于一般的函数来说，这不会有任何问题，但是对于递归函数，这会导致严重的问题！
//  递归函数内部嵌套了对自身的调用，除非等到最内层的函数调用结束，否则外层的所有函数都不会调用结束。通俗地讲，外层函数被卡主了，它要等待所有的内层函数调用完成后，它自己才能调用完成。
//  例如，一个递归函数需要递归10000次，每次需要1KB的内存，那么最终就需要10MB的内存。
// 2 递归函数的时间开销
//  每次调用函数都会在栈上分配内存，函数调用结束后再释放这一部分内存，内存的分配和释放都是需要时间的。
//  每次调用函数还会多次修改寄存器的值，函数调用结束后还需要找到上层函数的位置再继续执行，这也是需要时间的。
//  所有的这些时间加在一起是非常恐怖的。

long sum(int n) {
  // 为了增加每次函数调用的内存，额外增加了一个无用的数组，它占用1KB的内存
  int arr[250];
  if (n <= 1) {
    return n;
  } else {
    return n + sum(n - 1);
  }
}

// 使用迭代来替换递归函数
// 既然递归函数的解决方案存在巨大的内存开销和时间开销，那么我们如何进行优化呢？这是函数实现原理层面的缺陷，无法优化。
// 其实，大部分能用递归解决的问题也能用迭代来解决。所谓迭代，就是循环。
// 许多问题是以递归的形式进行解释的，这只是因为它比非递归形式更为清晰。但是，这些问题的迭代实现往往比递归实现效率更高，虽然代码的可读性可能稍差一些。
// 与递归函数相比，迭代不但没有额外的内存开销，也没有额外的时间开销。
// 函数调用本来就存在内存开销和时间开销，递归一次这种开销就增加一倍，如果有成千上万次的递归，那么所有开销的总和就是巨大的。
// 这是递归的致命缺陷，无法优化。所以建议尽量少用递归，能用迭代就用迭代吧。

// 迭代计算斐波那契数
long fib_iteration(int n) {
  long result;
  long previous_result;
  long next_older_result;
  result = previous_result = 1;
  while (n > 2) {
    n -= 1;
    next_older_result = previous_result;
    previous_result = result;
    result = previous_result + next_older_result;
  }
  return result;
}

int main() {
  int a = 10;
  printf("Factorial(%d) = %ld\n", a, factorial(a));

  char str[20] = "123456789";
  printf("%s\n", reverse(str));

  a = 42;
  clock_t time_start, time_end;
  time_start = clock();
  printf("Factorial(%d) = %ld\n", a, fib(a));
  time_end = clock();
  printf("run time: %lfs\n", (double)(time_end - time_start) / CLOCKS_PER_SEC);
  // run time: 1.044216s

  clock_t time_start_iteration, time_end_iteration;
  time_start_iteration = clock();
  printf("Fib_iteration(%d) = %ld\n", a, fib_iteration(a));
  time_end_iteration = clock();
  printf("run time with iteration: %lfs\n",
         (double)(time_end_iteration - time_start_iteration) / CLOCKS_PER_SEC);
  // run time with iteration: 0.000003s

  printf("从1加到1000的值为 %ld\n", sum(10000));  // 程序崩溃

  return 0;
}