#include <stdio.h>

// #define叫做宏定义命令，它也是C语言预处理命令的一种。
// 所谓宏定义，就是用一个标识符来表示一个字符串，如果在后面的代码中出现了该标识符，那么就全部替换成指定的字符串。
#define N 100
// 宏定义的一般形式为：#define 宏名 字符串
// #表示这是一条预处理命令，所有的预处理命令都以#开头。宏名是标识符的一种，命名规则和变量相同。字符串可以是数字、表达式、if语句、函数等。
// 这里所说的字符串是一般意义上的字符序列，不要和C语言中的字符串等同，它不需要双引号。
// 在宏定义中表达式两边的括号不能少，否则在宏展开以后可能会产生歧义。
#define M1 (n * n + 3 * n)
#define M2 n* n + 3 * n

// 对#define用法的几点说明
// 1) 宏定义是用宏名来表示一个字符串，在宏展开时又以该字符串取代宏名，这只是一种简单粗暴的替换。
//    字符串中可以含任何字符，它可以是常数、表达式、if 语句、函数等，预处理程序对它不作任何检查，如有错误，只能在编译已被宏展开后的源程序时发现。
// 2) 宏定义不是说明或语句，在行末不必加分号，如加上分号则连分号也一起替换。
// 3) 宏定义必须写在函数之外，其作用域为宏定义命令起到源程序结束。如要终止其作用域可使用#undef命令。
#define PI 3.14159
#undef PI
// 4) 代码中的宏名如果被引号包围，那么预处理程序不对其作宏代替
#define OK 100
// 5) 宏定义允许嵌套，在宏定义的字符串中可以使用已经定义的宏名，在宏展开时由预处理程序层层代换。
#define PI 3.1415926
#define S PI* y* y
// 6) 习惯上宏名用大写字母表示，以便于与变量区别。但也允许用小写字母。
// 7) 可用宏定义表示数据类型，使书写方便。
//    应注意用宏定义表示数据类型和用typedef 定义数据说明符的区别。宏定义只是简单的字符串替换，由预处理器来处理；
//    而typedef是在编译阶段由编译器处理的，它并不是简单的字符串替换，而给原有的数据类型起一个新的名字，将它作为一种新的数据类型。
#define PIN1 int*
typedef int* PIN2;  // 也可以写作typedef int (*PIN2);

// C语言允许宏带有参数。在宏定义中的参数称为“形式参数”，在宏调用中的参数称为“实际参数”，这点和函数有些类似。
// 对带参数的宏，在展开过程中不仅要进行字符串替换，还要用实参去替换形参。
// 带参宏定义的一般形式为：#define 宏名(形参列表) 字符串
#define MAX1(a, b) (a > b) ? a : b
// 对带参宏定义的说明:
// 1) 带参宏定义中，形参之间可以出现空格，但是宏名和形参列表之间不能有空格出现。例如把：
//    #define MAX(a,b) (a>b)?a:b 写为： #define MAX  (a,b)  (a>b)?a:b将被认为是无参宏定义，宏名MAX代表字符串(a,b) (a>b)?a:b。
// 2) 在带参宏定义中，不会为形式参数分配内存，因此不必指明数据类型。而在宏调用中，实参包含了具体的数据，要用它们去替换形参，因此实参必须要指明数据类型。
//    这一点和函数是不同的：在函数中，形参和实参是两个不同的变量，都有自己的作用域，调用时要把实参的值传递给形参；而在带参数的宏中，只是符号的替换，不存在值传递的问题。
// 3) 在宏定义中，字符串内的形参通常要用括号括起来以避免出错。
//    由此可见，对于带参宏定义不仅要在参数两侧加括号，还应该在整个字符串外加括号。
#define SQ1(y) y* y
#define SQ2(y) ((y) * (y))

// 带参数的宏和函数很相似，但有本质上的区别：宏展开仅仅是字符串的替换，不会对表达式进行计算；宏在编译之前就被处理掉了，它没有机会参与编译，也不会占用内存。
// 而函数是一段可以重复使用的代码，会被编译，会给它分配内存，每次调用函数，就是执行这块内存中的代码。
int SQ3(int y) { return ((y) * (y)); }
// 宏和函数只是在形式上相似，本质上是完全不同的。
// 带参数的宏也可以用来定义多个语句，在宏调用时，把这些语句又替换到源程序中:
#define SSSV(s1, s2, s3, v) \
  s1 = length * width;      \
  s2 = length * height;     \
  s3 = width * height;      \
  v = width * length * height;

// 在宏定义中，有时还会用到#和##两个符号，它们能够对宏参数进行操作。
// #用来将宏参数转换为字符串，也就是在宏参数的开头和末尾添加引号。
#define STR(s) #s
// ##称为连接符，用来将宏参数或其他的串连接起来。
#define CON1(a, b) a##e##b
#define CON2(a, b) a##b##00

// ANSI C规定了以下几个预定义宏，它们在各个编译器下都可以使用：
//  __LINE__：表示当前源代码的行号；
//  __FILE__：表示当前源文件的名称；
//  __DATE__：表示当前的编译日期；
//  __TIME__：表示当前的编译时间；
//  __STDC__：当要求程序严格遵循ANSI C标准时该标识被赋值为1；
//  __cplusplus：当编写C++程序时该标识符被定
int main() {
  int sum = 20 + N;
  printf("%d\n", sum);

  int n = 10;
  sum = 3 * M1 + 4 * M1 + 5 * M1;
  printf("sum1=%d\n", sum);
  sum = 3 * M2 + 4 * M2 + 5 * M2;
  printf("sum2=%d\n", sum);

  // float b = PI;  // 编译错误

  printf("OK\n");

  // 宏定义虽然也可表示数据类型， 但毕竟只是简单的字符串替换。在使用时要格外小心，以避出错。
  PIN1 a1, b1;  // int * a, b;
  PIN2 a2, b2;

  int x = 5, y = 6;
  printf("max=%d\n", MAX1(5, 6));
  printf("max=%d\n", SQ1(6 + 1));  // 13，被替换成sq=a+1*a+1;
  printf("max=%d\n", SQ2(6 + 1));  // 49

  int i = 1;
  while (i <= 5) {
    printf("%d^2 = %d\n", (i - 1), SQ3(i++));
  }
  i = 1;
  // 宏调用只是简单的字符串替换，SQ(i++)会被替换为((i++)*(i++))，这样每循环一次i的值增加2，所以最终只循环3次。
  while (i <= 5) {
    printf("%d^2 = %d\n", i, SQ2(i++));
  }
  // 1^2 = 1
  // 2^2 = 4
  // 3^2 = 9
  // 4^2 = 16
  // 5^2 = 25
  // 3^2 = 2
  // 5^2 = 12
  // 7^2 = 30

  int length = 3, width = 4, height = 5, sa, sb, sc, vv;
  SSSV(sa, sb, sc, vv);
  printf("sa=%d, sb=%d, sc=%d, vv=%d\n", sa, sb, sc, vv);

  printf("%s\n", STR(c.biancheng.net));    // c.biancheng.net
  printf("%s\n", STR("c.biancheng.net"));  // "c.biancheng.net"

  printf("%f\n", CON1(8.5, 2));  // 850.000000
  printf("%d\n", CON2(12, 34));  // 123400

  printf("Date : %s\n", __DATE__);
  printf("Time : %s\n", __TIME__);
  printf("File : %s\n", __FILE__);
  printf("Line : %d\n", __LINE__);
  // Date : Apr  6 2021
  // Time : 22:13:59
  // File : define.c
  // Line : 125

  return 0;
}