#include <stdio.h>

// 有些数据在存储时并不需要占用一个完整的字节，
// 只需要占用一个或几个二进制位即可。
// 位域是指在结构体定义时，
// 可以指定某个成员变量所占用的二进制位数（Bit）。
// struct bs {
//   unsigned m;
//   unsigned n : 4;
//   unsigned char ch : 6;
// };
// :后面的数字用来限定成员变量占用的位数。
// 成员m没有限制，根据数据类型即可推算出它占用4个字节（Byte）的内存。
// 成员n、ch被:后面的数字限制，不能再根据数据类型计算长度，
// 它们分别占用4、6位（Bit）的内存。
// C语言标准规定，位域的宽度不能超过它所依附的数据类型的长度。
// 通俗地讲，成员变量都是有类型的，这个类型限制了成员变量的最大长度，
// :后面的数字不能超过这个长度。
// C语言标准规定，只有几种数据类型可以用于位域。
// 在ANSI C中，这几种数据类型是int、signed int和unsigned int；
// 到了C99，_Bool也被支持了。
// 但编译器在具体实现时都进行了扩展，
// 额外支持了char、signed char、unsigned char以及enum类型。

// 位域的存储
// C语言标准并没有规定位域的具体存储方式，
// 不同的编译器有不同的实现，但它们都尽量压缩存储空间。
// 位域的具体存储规则如下：
// 1.当相邻成员的类型相同时，如果它们的位宽之和小于类型的sizeof大小，
//   那么后面的成员紧邻前一个成员存储，直到不能容纳为止；
//   如果它们的位宽之和大于类型的sizeof大小，
//   那么后面的成员将从新的存储单元开始，其偏移量为类型大小的整数倍。
// 2.当相邻成员的类型不同时，不同的编译器有不同的实现方案，
//   GCC会压缩存储，而VC/VS不会。
// 3.如果成员之间穿插着非位域成员，那么不会进行压缩。
// 位域往往不占用完整的字节，有时也不处于字节的开头位置，
// 因此使用&获取位域成员的地址是没有意义的，C语言也禁止这样做。
// 地址是字节（Byte）的编号，而不是位（Bit）的编号。

// 无名位域
// 位域成员可以没有名称，只给出数据类型和位宽
// 无名位域一般用来作填充或者调整成员位置。
// 因为没有名称，无名位域不能使用。
// struct bs {
//   int m : 12;
//   int : 20;  // 该位域成员不能使用
//   int n : 4;
// };

int main() {
  struct bs {
    unsigned m;
    unsigned n : 4;
    unsigned char ch : 6;
  } a = {0xad, 0xE, '$'};
  // 第一次输出时，n、ch的值分别是0xE、0x24（'$'的ASCII码为0x24），
  // 换算成二进制是1110、10 0100，都没有超出限定的位数，能够正常输出。
  printf("%#x, %#x, %c\n", a.m, a.n, a.ch);
  // 0xad, 0xe, $

  a.m = 0xb8901c;
  a.n = 0x2d;
  a.ch = 'z';
  // 第二次输出时，n、ch的值变为0x2d、0x7a（'z'的ASCII码为0x7a），
  // 换算成二进制分别是10 1101、111 1010，都超出了限定的位数。
  // 超出部分被直接截去，剩下1101、11 1010，
  // 换算成十六进制为0xd、0x3a（0x3a对应的字符是:）。
  printf("%#x, %#x, %c\n", a.m, a.n, a.ch);
  // 0xb8901c, 0xd, :

  struct bs1 {
    unsigned m : 6;
    unsigned n : 12;
    unsigned p : 4;
  };
  printf("%d\n", sizeof(struct bs1));  // 4
  // m、n、p都是unsigned int，sizeof的结果为4字节（Byte）。
  // m、n、p的位宽之和为6+12+4=22，小于32，所以会挨着存储，没有缝隙。
  struct bs2 {
    unsigned m : 22;
    unsigned n : 12;
    unsigned p : 4;
  };
  printf("%d\n", sizeof(struct bs2));  // 8

  struct bs3 {
    unsigned m : 12;
    unsigned char ch : 4;
    unsigned p : 4;
  };
  printf("%d\n", sizeof(struct bs3));  // 4
  // 在GCC下的运行结果为4，三个成员挨着存储；
  // 在VC/VS下的运行结果为12，三个成员按照各自的类型存储，
  // 与不指定位宽时的存储方式相同。

  struct bs4 {
    unsigned m : 12;
    unsigned ch;
    unsigned p : 4;
  };
  printf("%d\n", sizeof(struct bs4));  // 12

  return 0;
}