#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

// 不同CPU中，4字节整数1在内存空间的存储方式是不同的：
// 00000000 00000000 00000000 00000001
// 有些CPU以上面的顺序存储到内存，另外一些CPU则以倒序存储：
// 00000001 00000000 00000000 00000000
// 若不考虑这些就收发数据会发生问题，因为保存顺序的不同意味着对接收数据的解析顺序也不同。

// CPU向内存保存数据的方式有两种：
// 1.大端序（Big Endian）：高位字节存放到低位地址（高位字节在前）。
// 2.小端序（Little Endian）：高位字节存放到高位地址（低位字节在前）。

// 假设在0x20号开的地址中保存4字节int型数据0x12345678，
// 大端序CPU保存方式：12[0x20]34[0x21]56[0x22]78[0x24]
// 小端序CPU保存方式：78[0x20]56[0x21]34[0x22]12[0x24]
// 对于大端序，最高位字节0x12存放到低位地址，最低位字节0x78存放到高位地址。
// 对于小端序，最高位字节0x12存放到高位地址，最低位字节0x78存放到低位地址。

// 不同CPU保存和解析数据的方式不同（主流的Intel系列CPU为小端序），
// 小端序系统和大端序系统通信时会发生数据解析错误。
// 因此在发送数据前，要将数据转换为网络字节序（Network Byte Order）。
// 网络字节序统一为大端序。

int main() {
  unsigned short host_port = 0x1234, net_port;
  unsigned long host_addr = 0x12345678, net_addr;

  // htons()用来将当前主机字节序转换为网络字节序，其中：
  //  h代表主机（host）字节序；
  //  n代表网络（network）字节序；
  //  s代表short。
  // htons是h、to、n、s的组合，可以理解为将short型数据从当前主机字节序转换为网络字节序。
  // 常见的网络字节转换函数有：
  // 1.htons()：host to network short，
  //   将short类型数据从主机字节序转换为网络字节序。
  // 2.ntohs()：network to host short，
  //   将short类型数据从网络字节序转换为主机字节序。
  // 3.htonl()：host to network long，
  //   将long类型数据从主机字节序转换为网络字节序。
  // 4.ntohl()：network to host long，
  //   将long类型数据从网络字节序转换为主机字节序。
  // 通常，以s为后缀的函数中，s代表2个字节short，因此用于端口号转换；
  // 以l为后缀的函数中，l代表4个字节的long，因此用于IP地址转换。
  net_port = htons(host_port);
  net_addr = htonl(host_addr);

  printf("Host ordered port: %#x\n", host_port);
  printf("Network ordered port: %#x\n", net_port);
  printf("Host ordered address: %#lx\n", host_addr);
  printf("Network ordered address: %#lx\n", net_addr);

  // Host ordered port: 0x1234
  // Network ordered port: 0x3412
  // Host ordered address: 0x12345678
  // Network ordered address: 0x78563412

  // sockaddr_in中保存IP地址的成员为32位整数，而我们熟悉的是点分十进制表示法，
  // 例如127.0.0.1，它是一个字符串，因此为了分配IP地址，需要将字符串转换为4字节整数。
  // inet_addr()函数可以完成这种转换。
  // inet_addr()除了将字符串转换为32位整数，同时还进行网络字节序转换。
  char *addr1 = "1.2.3.4";
  unsigned long conv_addr = inet_addr(addr1);
  if (conv_addr == INADDR_NONE) {
    puts("Error occured!");
  } else {
    printf("Network ordered integer addr: %#lx\n", conv_addr);
  }
  // Network ordered integer addr : 0x4030201

  // inet_addr()不仅可以把IP地址转换为32位整数，还可以检测无效IP地址。
  char *addr2 = "1.2.3.256";
  conv_addr = inet_addr(addr2);
  if (conv_addr == INADDR_NONE) {
    puts("Error occured!");
  } else {
    printf("Network ordered integer addr: %#lx\n", conv_addr);
  }
  // Error occured!

  // 为sockaddr_in成员赋值时需要显式地将主机字节序转换为网络字节序，
  // 而通过write()/send()发送数据时TCP协议会自动转换为网络字节序，不需要再调用相应的函数。
}
