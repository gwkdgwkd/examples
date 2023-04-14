#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

int main() {
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // 每个socket被创建后，都会分配两个缓冲区，输入缓冲区和输出缓冲区。
  // write()/send()并不立即向网络中传输数据，而是先将数据写入缓冲区中，
  // 再由TCP协议将数据从缓冲区发送到目标机器。
  // 一旦将数据写入到缓冲区，函数就可以成功返回，不管它们有没有到达目标机器，
  // 也不管它们何时被发送到网络，这些都是TCP协议负责的事情。
  // TCP协议独立于write()/send()函数，数据有可能刚被写入缓冲区就发送到网络，
  // 也可能在缓冲区中不断积压，多次写入的数据被一次性发送到网络，
  // 这取决于当时的网络情况、当前线程是否空闲等诸多因素，不由程序员控制。
  // read()/recv()函数也是如此，也从输入缓冲区中读取数据，而不是直接从网络中读取。
  // 这些I/O缓冲区特性可整理如下：
  // 1.I/O缓冲区在每个TCP套接字中单独存在；
  // 2.I/O缓冲区在创建套接字时自动生成；
  // 3.即使关闭套接字也会继续传送输出缓冲区中遗留的数据；
  // 4.关闭套接字将丢失输入缓冲区中的数据。
  // 输入输出缓冲区的默认大小一般都是8K，可以通过getsockopt()函数获取：
  unsigned optVal;
  int optLen = sizeof(int);
  getsockopt(serv_sock, SOL_SOCKET, SO_SNDBUF, (char *)&optVal,
             (socklen_t *)&optLen);
  printf("Buffer length: %d\n", optVal);
  // Buffer length: 16384

  // 对于TCP套接字（默认情况下），当使用write()/send()发送数据时：
  // 1.首先会检查缓冲区，如果缓冲区的可用空间长度小于要发送的数据，
  //   那么write()/send()会被阻塞（暂停执行），直到缓冲区中的数据被发送到目标机器，
  //   腾出足够的空间，才唤醒write()/send()函数继续写入数据；
  // 2.如果TCP协议正在向网络发送数据，那么输出缓冲区会被锁定，不允许写入，
  //   write()/send()也会被阻塞，直到数据发送完毕缓冲区解锁，write()/send()才会被唤醒；
  // 3.如果要写入的数据大于缓冲区的最大长度，那么将分批写入；
  // 4.直到所有数据被写入缓冲区 write()/send()才能返回。

  // 当使用read()/recv()读取数据时：
  // 1.首先会检查缓冲区，如果有数据，那么就读取，否则函数会被阻塞，直到网络上有数据到来。
  // 2.如果要读取的数据长度小于缓冲区中的数据长度，那么就不能一次性将缓冲区中的所有数据读出，
  //   剩余数据将不断积压，直到有read()/recv()函数再次读取。
  // 3.直到读取到数据后read()/recv()函数才会返回，否则就一直被阻塞。

  // 这就是TCP套接字的阻塞模式。
  // 所谓阻塞，就是上一步动作没有完成，下一步动作将暂停，直到上一步动作完成后才能继续，
  // 以保持同步性，TCP套接字默认情况下是阻塞模式，也是最常用的，当然也可以更改为非阻塞模式。

  // 根据socket缓冲区和数据的传递过程，可以看到数据的接收和发送是无关的，
  // read()/recv()函数不管数据发送了多少次，都会尽可能多的接收数据。
  // 也就是说，read()/recv()和write()/send()的执行次数可能不同。
  // 这就是数据的粘包问题，客户端发送的多个数据包被当做一个数据包接收，
  // 也称数据的无边界性，read()/recv()函数不知道数据包的开始或结束标志，
  // 实际上也没有任何开始或结束标志，只把它们当做连续的数据流来处理。

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  listen(serv_sock, 20);

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUF_SIZE] = {0};
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  sleep(10);  // 休眠10秒，客户端发送的3次数据都保存到缓存区了

  // 服务器端一次性接收到所有数据
  int strLen = read(clnt_sock, buffer, BUF_SIZE);
  printf("%s\n", buffer);

  write(clnt_sock, buffer, strLen);

  close(clnt_sock);
  close(serv_sock);

  return 0;
}