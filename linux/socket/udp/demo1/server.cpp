#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

// 基于UDP的回声服务端

int main() {
  int serv_sock = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // 利用常数INADDR_ANY自动获取IP地址有一个明显的好处，
  // 就是当软件安装到其他服务器或者服务器IP地址改变时，
  // 不用再更改源码重新编译，也不用在启动软件时手动输入。
  // 而且，如果一台计算机中已分配多个IP地址（例如路由器），
  // 那么只要端口号一致，就可以从不同的IP地址接收数据。
  // 所以，服务器中优先考虑使用INADDR_ANY；
  // 而客户端中除非带有一部分服务器功能，否则不会采用。
  // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 自动获取IP地址，崩溃
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  // UDP不像TCP，无需在连接状态下交换数据，因此基于UDP的服务器端和客户端也无需经过连接过程。
  // 也就是说，不必调用listen()和accept()函数，UDP中只有创建套接字的过程和数据交换的过程。

  // TCP中，套接字是一对一的关系。
  // 如要向10个客户端提供服务，那么除了负责监听的套接字外，还需要创建10套接字。
  // 但在UDP中，不管是服务器端还是客户端都只需要1个套接字。

  // 创建好TCP套接字后，传输数据时无需再添加地址信息，
  // 因为TCP套接字将保持与对方套接字的连接，换言之，TCP套接字知道目标地址信息。
  // 但UDP套接字不会保持连接状态，每次传输数据都要添加目标地址信息，这相当于填写收件人地址。
  // 发送数据使用sendto()函数：
  // ssize_t sendto(int sock, void *buf, size_t nbytes, int flags, struct
  //                sockaddr *to, socklen_t addrlen);  // Linux
  // int sendto(SOCKET sock, const char *buf, int nbytes, int flags,
  //            const struct sockadr *to, int addrlen);  // Windows
  // Linux和Windows下的sendto()函数类似，下面是详细参数说明：
  // 1.sock，用于传输UDP数据的套接字；
  // 2.buf，保存待传输数据的缓冲区地址；
  // 3.nbytes，带传输数据的长度（以字节计）；
  // 4.flags，可选项参数，若没有可传递0；
  // 5.to，存有目标地址信息的sockaddr结构体变量的地址；
  // 6.addrlen，传递给参数to的地址值结构体变量的长度。

  // UDP发送函数sendto()与TCP发送函数write()/send()最大的区别是，sendto()需要目标地址信息。
  // 接收数据使用recvfrom()函数：
  // ssize_t recvfrom(int sock, void *buf, size_t nbytes, int flags, struct
  //                  sockadr *from, socklen_t *addrlen);  // Linux
  // int recvfrom(SOCKET sock, char *buf, int nbytes, int flags,
  //              const struct sockaddr *from, int *addrlen);  // Windows
  // 由于UDP数据的发送端不定，所以recvfrom()函数定义为可接收发送端信息的形式，具体参数如下：
  // 1.sock，用于接收UDP数据的套接字；
  // 2.buf，保存接收数据的缓冲区地址；
  // 3.nbytes，可接收的最大字节数（不能超过buf缓冲区的大小）；
  // 4.flags，可选项参数，若没有可传递0；
  // 5.from，存有发送端地址信息的sockaddr结构体变量的地址；
  // 6.addrlen，保存参数from的结构体变量长度的变量地址值。

  // 需要注意的是，UDP不同于TCP，不存在请求连接和受理过程，
  // 因此在某种意义上无法明确区分服务器端和客户端，只是因为其提供服务而称为服务器端。

  struct sockaddr clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUF_SIZE] = {0};
  while (1) {
    int strLen =
        recvfrom(serv_sock, buffer, BUF_SIZE, 0, &clnt_addr, &clnt_addr_size);
    sendto(serv_sock, buffer, strLen, 0, &clnt_addr, clnt_addr_size);
  }

  close(serv_sock);

  return 0;
}