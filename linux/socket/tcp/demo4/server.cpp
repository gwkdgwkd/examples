#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

// socket编程实现文件传输功能

int main() {
  FILE *fp = fopen("./server.cpp", "rb");
  if (fp == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    exit(0);
  }

  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  listen(serv_sock, 20);

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

  char buffer[BUF_SIZE] = {0};
  int count = 0;
  // 文件大小不确定，有可能比缓冲区大很多，调用一次write()/send()函数不能完成文件内容的发送。
  // 接收数据时也会遇到同样的情况，要解决这个问题，可以使用while循环。
  // 对于Server端的代码，当读取到文件末尾，fread()会返回0，结束循环。
  while ((count = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
    printf("%s\n", buffer);
    write(clnt_sock, buffer, count);
  }

  // 调用close()/closesocket()函数意味着完全断开连接，即不能发送数据也不能接收数据，
  // 实际上，是完全无法调用与数据收发有关的函数，这种“生硬”的方式有时候会显得不太“优雅”。
  // 一般情况下这不会有问题，但有些特殊时刻，需要只断开一条数据传输通道，而保留另一条。
  // 使用shutdown()函数可以达到这个目的，它的原型为：
  // int shutdown(int sock, int howto);  // Linux
  // int shutdown(SOCKET s, int howto);  // Windows
  // sock为需要断开的套接字，howto为断开方式。
  // howto在Linux下有以下取值：
  // 1.SHUT_RD：断开输入流。
  //   套接字无法接收数据（即使输入缓冲区收到数据也被抹去），无法调用输入相关函数。
  // 2.SHUT_WR：断开输出流。
  //   套接字无法发送数据，但如果输出缓冲区中还有未传输的数据，则将传递到目标主机。
  // 3.SHUT_RDWR：同时断开I/O流。
  //   相当于分两次调用shutdown()，其中一次以SHUT_RD为参数，另一次以SHUT_WR为参数。
  // howto在Windows下有以下取值：
  // 1.SD_RECEIVE：关闭接收操作，也就是断开输入流。
  // 2.SD_SEND：关闭发送操作，也就是断开输出流。
  // 3.SD_BOTH：同时关闭接收和发送操作。

  // close()/closesocket()和shutdown()的区别
  // 确切地说，close()/closesocket()用来关闭套接字，将套接字描述符（或句柄）从内存清除，
  // 之后再也不能使用该套接字，与C语言中的fclose()类似。
  // 应用程序关闭套接字后，与该套接字相关的连接和缓存也失去了意义，TCP协议会自动触发关闭连接的操作。
  // shutdown()用来关闭连接，而不是套接字，不管调用多少次shutdown()，套接字依然存在，
  // 直到调用close()/closesocket()将套接字从内存清除。
  // 调用close()/closesockt()关闭套接字时，或调用shutdown()关闭输出流时，都会向对方发送FIN包。
  // FIN包表示数据传输完毕，计算机收到FIN包就知道不会再有数据传送过来了。
  // 默认情况下，close()/closesocket()会立即向网络中发送FIN包，不管输出缓冲区中是否还有数据，
  // 而shutdown()会等输出缓冲区中的数据传输完毕再发送FIN包。
  // 也就意味着，调用close()/closesocket()将丢失输出缓冲区中的数据，而调用shutdown()不会。

  shutdown(clnt_sock, SHUT_WR);  // 文件读取完毕，断开输出流，向客户端发送FIN包
  read(clnt_sock, buffer, BUF_SIZE);  // 阻塞，等待客户端接收完毕

  fclose(fp);
  close(clnt_sock);
  close(serv_sock);

  return 0;
}