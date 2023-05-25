#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

// UDP分为链接和没有链接2种：
// 1.没有链接的UDP不能调用getpeername，但是可以调用getsockname，和TCP一样，
//   他的地址和端口不是在调用socket就指定了，而是在第一次调用sendto函数以后
// 2.已经链接的UDP，在调用connect以后，这2个函数都是可以用的，同样，getpeername也没太大意义。
//   如果不知道对方的地址和端口，不可能会调用connect。

void showHost(int fd, const char *str) {
  struct sockaddr_in addr;
  int len = sizeof(addr);
  getsockname(fd, (struct sockaddr *)&addr, (socklen_t *)&len);
  char ip[20];
  printf("%s addr = %s:%d\n", str,
         inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, sizeof(ip)),
         ntohs(addr.sin_port));
}
void showPeer(int fd, const char *str) {
  struct sockaddr_in addr;
  int len = sizeof(addr);
  getpeername(fd, (struct sockaddr *)&addr, (socklen_t *)&len);
  char ip[20];
  printf("%s addr = %s:%d\n", str,
         inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, sizeof(ip)),
         ntohs(addr.sin_port));
}

void func1() {
  // getsockname可以获得一个与socket相关的地址，服务器端可以通过它得到相关客户端地址，
  // 而客户端也可以得到当前已连接成功的socket的ip和端口。
  // 第二种情况在客户端不进行bind而直接连接服务器时，
  // 而且客户端需要知道当前使用哪个ip进行通信时比较有用（如多网卡的情况）。
  // 对于TCP连接的情况，如果不进行bind指定IP和端口，那么调用connect连接成功后，
  // 使用getsockname可以正确获得当前正在通信的socket的IP和端口地址。
  // 而对于UDP的情况，无论是在调用sendto之后还是收到服务器返回的信息之后调用，
  // 都无法得到正确的ip地址，使用getsockname得到ip为0，端口正确。

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  showHost(sock, "client after create :");
  // client after create : addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  struct sockaddr from_addr;
  socklen_t from_addr_size = sizeof(from_addr);

  char buffer[BUF_SIZE] = "nihao";
  sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr,
         sizeof(serv_addr));
  showHost(sock, "client after sendto :");
  // client after sendto : addr = 0.0.0.0:51285

  int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &from_addr, &from_addr_size);
  showHost(sock, "client after recvfrom :");
  // client after recvfrom : addr = 0.0.0.0:51285
  buffer[strLen] = 0;
  printf("Message form server: %s\n", buffer);

  write(sock, buffer, strlen(buffer));

  close(sock);
}
void func2() {
  // udp中可以使用connect系统调用，udp中connect与TCP中connect有着本质区别：
  // 1.tcp中调用connect会引起三次握手，client与server建立连接，
  //   udp中调用connect内核仅仅把对端ip和port记录下来；
  // 2.udp中可以多次调用connect，tcp只能调用一次connect；
  //   udp多次调用connect有两种用途：
  //   a.指定一个新的ip和port连接，直接设置connect第二个参数即可；
  //   b.断开和之ip和port的连接，将connect第二个参数的sin_family设置成AF_UNSPEC。

  // udp中使用connect可以提高效率，
  // 普通的udp发送两个报文内核做了如下：
  // 1.建立连接；
  // 2.发送报文；
  // 3.断开连接；
  // 4.建立连接；
  // 5.发送报文；
  // 6.断开连接。
  // 采用connect方式的udp发送两个报文内核如下处理：
  // 1.建立连结；
  // 2.发送报文；
  // 3.发送报文另外一个报文。

  // 采用connect的udp发送接受报文可以用send，write和recv，read，也可以用sendto，recvfrom。
  // 调用sendto的时候第五个参数必须是NULL，第六个参数是0。
  // 调用recvfrom，recv，read系统调用只能获取到先前connect的ip和port发送的报文。

  // udp中使用connect的好处：
  // 1.会提升效率；
  // 2.高并发服务中会增加系统稳定性。

  // int send(int s,const void *msg,size_t len,int flags);
  // int sendto(int s,const void *msg,size_t len,int flags,
  //            const struct sockaddr *to,socklen_t tolen);
  // int recv(int s,void *buf, size_t len, int flags);
  // int recvfrom(int s,void *buf,size_t len,int flags,
  //              struct sockaddr *from,socklen_t *fromlen);
  // 从定义可以看出，sendto和recvfrom在收发时指定地址，
  // 而send和recv则没有，那么他们的地址是在那里指定的呢，答案就在于connect，
  // int connect(int sockfd,const struct sockaddr *serv_addr,socklen_t addrlen);
  // 在udp编程中，如果只往一个地址发送，那么可以使用send和recv，
  // 在使用它们之前用connect把它们的目的地址指定一下就可以了。

  // 对于已连接udp套接口，与缺省的未连接套接口相比，发生了三个变化：
  // 1.再也不能给输出操作指定IP和端口，也就是说不使用sendto，而改用write或send，
  //   写到已连接udp套接口上的任何内容都自动发送到由connect指定的协议地址；
  // 2.不必使用recvfrom以获悉数据报的发送者，而改用read，recv或recvmsg，
  //   在一个已连接UDP套接口上，由内核为输入操作返回的数据报，
  //   仅仅是那些来自connect所指定协议地址的数据报。
  //   目的地为这个已连接udp套接口的本地协议地址，
  //   发源地却不是该套接口早先connect到的协议地址的数据报，不会投递到该套接口。
  //   这样就限制了一个已连接udp套接口而且仅能与一个对端交换数据报。
  // 3.由已连接的udp套接口引发的异步错误，返回给他们所在的进程，
  //   相反未连接udp套接口不接收任何异步错误给一个udp套接口。

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  showHost(sock, "client after create :");
  // client after create : addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  showPeer(sock, "server after connect:");
  // server after connect: addr = 127.0.0.1:1234
  showHost(sock, "client after connect:");
  // client after connect: addr = 127.0.0.1:52924

  char buffer[BUF_SIZE] = "nihao";
  write(sock, buffer, sizeof(buffer));
  memset(buffer, 0, sizeof(buffer));
  int strLen = read(sock, buffer, sizeof(buffer) - 1);
  buffer[strLen] = 0;
  printf("Message form server: %s\n", buffer);
  write(sock, buffer, strlen(buffer));

  close(sock);
}

void func3() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  showHost(sock, "client after create :");
  // client after create : addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = htonl(INADDR_ANY);
  local.sin_port = htons(9000);
  bind(sock, (struct sockaddr *)&local, sizeof(local));

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  showPeer(sock, "server after connect:");
  // server after connect: addr = 127.0.0.1:1234
  showHost(sock, "client after connect:");
  // client after connect: addr = 127.0.0.1:9000

  char buffer[BUF_SIZE] = "nihao";
  write(sock, buffer, sizeof(buffer));
  memset(buffer, 0, sizeof(buffer));
  int strLen = read(sock, buffer, sizeof(buffer) - 1);
  buffer[strLen] = 0;
  printf("Message form server: %s\n", buffer);
  write(sock, buffer, strlen(buffer));

  close(sock);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s 1\n", argv[0]);
    return 1;
  }

  if (argv[1][0] == '1') {
    func1();
  } else if (argv[1][0] == '2') {
    func2();
  } else {
    func3();
  }

  return 0;
}