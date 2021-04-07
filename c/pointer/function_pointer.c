#include <stdio.h>
#include <string.h>

// 指针变量作为函数参数
// 像数组、字符串、动态分配的内存等都是一系列数据的集合，没有办法通过一个参数全部传入函数内部，只能传递它们的指针，在函数内部通过指针来影响这些数据集合。
// 有的时候，对于整数、小数、字符等基本类型数据的操作也必须要借助指针，一个典型的例子就是交换两个变量的值。
void swap1(int a, int b) {
  int temp;  // 临时变量
  temp = a;
  a = b;
  b = temp;
}
void swap2(int *p1, int *p2) {
  int temp;  // 临时变量
  temp = *p1;
  *p1 = *p2;
  *p2 = temp;
}

// 用数组作函数参数
// 数组是一系列数据的集合，无法通过参数将它们一次性传递到函数内部，如果希望在函数内部操作数组，必须传递数组指针。
// C语言为什么不允许直接传递数组的所有元素，而必须传递数组指针呢？
// 参数的传递本质上是一次赋值的过程，赋值就是对内存进行拷贝。所谓内存拷贝，是指将一块内存上的数据复制到另一块内存上。
// 对于像int、float、char等基本类型的数据，它们占用的内存往往只有几个字节，对它们进行内存拷贝非常快速。而数组是一系列数据的集合，数据的数量没有限制，可能很少，
// 也可能成千上万，对它们进行内存拷贝有可能是一个漫长的过程，会严重拖慢程序的效率，为了防止技艺不佳的程序员写出低效的代码，C语言没有从语法上支持数据集合的直接赋值。
// 除了C语言，C++、Java、Python等其它语言也禁止对大块内存进行拷贝，在底层都使用类似指针的方式来实现。
int max(int *intArr, int len) {
  // 参数列表也可以写成下面两种形式：
  // int intArr[6]好像定义了一个拥有6个元素的数组，调用max()时可以将数组的所有元素“一股脑”传递进来。
  // int max(int intArr[6], int len){  // 用数组做函数参数时，参数也能够以“真正”的数组形式给出。
  // int intArr[]虽然定义了一个数组，但没有指定数组长度，好像可以接受任意长度的数组。
  // int max(int intArr[], int len){  // 也可以省略数组长度
  // 两种形式的数组定义都是假象，不管是int intArr[6]还是int intArr[]都不会创建一个数组出来，编译器也不会为它们分配内存，实际的数组是不存在的，它们最
  // 终还是会转换为int *intArr这样的指针。这就意味着，两种形式都不能将数组的所有元素“一股脑”传递进来，大家还得规规矩矩使用数组指针。
  // int intArr[6]这种形式只能说明函数期望用户传递的数组有6个元素，并不意味着数组只能有6个元素，真正传递的数组可以有少于或多于6个的元素。
  // 不管使用哪种方式传递数组，都不能在函数内部求得数组长度，因为intArr仅仅是一个指针，而不是真正的数组，所以必须要额外增加一个参数来传递数组长度。
  int i, maxValue = intArr[0];  // 假设第0个元素是最大值
  for (i = 1; i < len; i++) {
    if (maxValue < intArr[i]) {
      maxValue = intArr[i];
    }
  }

  return maxValue;
}

// 指针作为函数返回值
// C语言允许函数的返回值是一个指针（地址），将这样的函数称为指针函数。
char *strlong(char *str1, char *str2) {
  if (strlen(str1) >= strlen(str2)) {
    return str1;
  } else {
    return str2;
  }
}
// 用指针作为函数返回值时需要注意的一点是，函数运行结束后会销毁在它内部定义的所有局部数据，包括局部变量、局部数组和形式参数，函数返回的指针请尽量不要指向这些
// 数据，C语言没有任何机制来保证这些数据会一直有效，它们在后续使用过程中可能会引发运行时错误。
int *func() {
  int n = 100;
  return &n;
}

// 一个函数总是占用一段连续的内存区域，函数名在表达式中有时也会被转换为该函数所在内存区域的首地址，这和数组名非常类似。
// 可以把函数的这个首地址（或称入口地址）赋予一个指针变量，使指针变量指向函数所在的内存区域，然后通过指针变量就可以找到并调用该函数。这种指针就是函数指针。
// 函数指针的定义形式为： returnType (*pointerName)(param list);
// returnType为函数返回值类型，pointerName为指针名称，param list为函数参数列表。
// 参数列表中可以同时给出参数的类型和名称，也可以只给出参数的类型，省略参数的名称，这一点和函数原型非常类似。
// 注意()的优先级高于*，第一个括号不能省略，如果写作returnType *pointerName(param list);就成了函数原型，它表明函数的返回值类型为returnType*。
int max1(int a, int b) { return a > b ? a : b; }

int main() {
  int a = 66, b = 99;
  swap1(a, b);
  printf("a = %d, b = %d\n", a, b);  // a = 66, b = 99
  swap2(&a, &b);
  printf("a = %d, b = %d\n", a, b);  // a = 99, b = 66

  int nums[7] = {1, 2, 4, 3, 6, 9, 7};
  int len = sizeof(nums) / sizeof(int);
  printf("Max value is %d!\n", max(nums, len));

  char str1[] = "nihao", str2[] = "dajiahao";
  char *str = strlong(str1, str2);
  printf("Longer string: %s\n", str);

  int *p = func(), n;
  n = *p;
  printf("value = %d\n", n);

  int x = 5, y = 8;
  int (*pmax)(int, int) = max1;  // 也可以写作int (*pmax)(int a, int b)
  printf("Max value: %d\n", (*pmax)(x, y));  // 8

  return 0;
}