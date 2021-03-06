#include <stdio.h>

// 大端和小端是指数据在内存中的存储模式，它由CPU决定：
//  1) 大端模式（Big-endian）是指将数据的低位（比如1234中的34就是低位）放在内存的高地址上，而数据的高位（比如1234中的12就是高位）放在内存的低地址上。
//     这种存储模式有点儿类似于把数据当作字符串顺序处理，地址由小到大增加，而数据从高位往低位存放。
//  2) 小端模式（Little-endian）是指将数据的低位放在内存的低地址上，而数据的高位放在内存的高地址上。这种存储模式将地址的高低和数据的大小结合起来，高地
//     址存放数值较大的部分，低地址存放数值较小的部分，这和我们的思维习惯是一致，比较容易理解。

// 为什么有大小端模式之分
// 计算机中的数据是以字节（Byte）为单位存储的，每个字节都有不同的地址。现代CPU的位数（可以理解为一次能处理的数据的位数）都超过了8位（一个字节），PC机、
// 服务器的CPU基本都是64位的，嵌入式系统或单片机系统仍然在使用32位和16位的CPU。
// 对于一次能处理多个字节的CPU，必然存在着如何安排多个字节的问题，也就是大端和小端模式。以int类型的0x12345678为例，它占用4个字节，
//  内存地址                      0x4000      0x4001      0x4002      0x4003
//  小端模式（Little-endian）       0x78        0x56        0x34        0x12
//  大端模式（Big-endian）          0x12        0x34        0x56        0x78
// PC机上使用的是X86结构的CPU，它是小端模式；51单片机是大端模式；很多ARM、DSP也是小端模式（部分ARM处理器还可以由硬件来选择是大端模式还是小端模式）。

int main() {
  // 借助共用体，我们可以检测CPU是大端模式还是小端模式
  union {
    int n;
    char ch;
  } data;
  data.n = 0x00000001;  // 也可以直接写作data.n = 1;
  if (data.ch == 1) {
    printf("Little-endian\n");
  } else {
    printf("Big-endian\n");
  }
  // Little-endian

  return 0;
}