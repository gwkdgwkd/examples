#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// getsockname与getpeername是返回套接口关联的本地协议地址和远程协议地址：
// int getsockname(int sockfd, struct sockaddr *localaddr, socken_t *addrlen);
// int getpeername(int sockfd, struct sockaddr *peeraddr, socken_t *addrlen);
// 参数：
// sockfd表示要获取的套接口的描述字；
// localaddr返回本地协议地址描述结构；
// peeraddr返回远程协议地址描述结构；
// addrlen是对应结构的长度。
// 返回0表示成功，返回1表示出错。

// getsockname和getpeername调度时机很重要，如果调用时机不对，则无法正确获得地址和端口。

// 服务端在bind后就可以调用getsockname来获取本地地址和端口，虽然这没有什么太多的意义。
// getpeername只有在链接建立以后才调用，否则不能正确获得对方地址和端口，
// 所以他的参数描述字一般是链接描述字而非监听套接口描述字。

// 对于tcp或udp server来说：
// bind以后就可以调用getsockname来获取本地地址和端口了，
// 只不过这样没啥意义，因为就是自己绑定的。
// 只有在accept以后（就是有连接之后才）才能用getpeername获取client的ip和端口。
// client的ip和port也可以在accept函数的第二个参数中带出。
// 对于tcp或udp client来说：
// 创建socket并不会分配ip和端口，用getsockname获取出来的数据全是0。
// 在连接connect之后才可以用getsockname获取自己的ip和端口，
// 也可以用getpeername获取服务器的。

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

int main() {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  showHost(serv_sock, "server after create:");
  // server after create: addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  showHost(serv_sock, "server after bind  :");
  // server after bind  : addr = 127.0.0.1:1234

  listen(serv_sock, 20);

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);

  showPeer(serv_sock, "server before accept:");
  // server before accept: addr = 127.0.0.1:1234
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  char ip[20];
  printf("after accept: received from %s at port %d\n",
         inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, ip, sizeof(ip)),
         ntohs(clnt_addr.sin_port));
  // after accept: received from 127.0.0.1 at port 35326

  showPeer(clnt_sock, "client after accept:");
  // client after accept: addr = 127.0.0.1:35326

  char str[] = "Hello World!";
  write(clnt_sock, str, sizeof(str));

  close(clnt_sock);
  close(serv_sock);

  return 0;
}