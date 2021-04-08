#include <stdio.h>

// 在C语言中，还有另外一种和结构体非常类似的语法，叫做共用体（Union），它的定义格式为：
// union 共用体名{
//     成员列表
// };
// 结构体和共用体的区别在于：结构体的各个成员会占用不同的内存，互相之间没有影响；而共用体的所有成员占用同一段内存，修改一个成员会影响其余所有成员。
// 结构体占用的内存大于等于所有成员占用的内存的总和（成员之间可能会存在缝隙），共用体占用的内存等于最长的成员占用的内存。共用体使用了内存覆盖技术，
// 同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。
// 共用体在一般的编程中应用较少，在单片机中应用较多。

// 共用体也是一种自定义类型，可以通过它来创建变量:
union data1 {
  int n;
  char ch;
  double f;
};
union data a1, b1, c1;
// 也可以在定义共用体的同时创建变量：
union data2 {
  int n;
  char ch;
  double f;
} a2, b2, c2;
// 如果不再定义新的变量，也可以将共用体的名字省略：
union {
  int n;
  char ch;
  double f;
} a3, b3, c3;

union data {
  int n;
  char ch;
  short m;
};
int main() {
  union data a;
  printf("%d, %d\n", sizeof(a), sizeof(union data));  // 4, 4
  a.n = 0x40;
  printf("%X, %c, %hX\n", a.n, a.ch, a.m);  // 40, @, 40
  a.ch = '9';
  printf("%X, %c, %hX\n", a.n, a.ch, a.m);  // 39, 9, 39
  a.m = 0x2059;
  printf("%X, %c, %hX\n", a.n, a.ch, a.m);  // 2059, Y, 2059
  a.n = 0x3E25AD54;
  printf("%X, %c, %hX\n", a.n, a.ch, a.m);  // 3E25AD54, T, AD54

  return 0;
}