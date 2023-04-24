#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// 对于客户端socket，一般创建socket后，直接链接服务器的ip和端口，
// 一般来说，只需知道服务器的ip/域名和端口号，客户端端一般默认，
// 不进行显示的绑定固定端口，系统会默认自动分配空闲的端口。
// 但是有时需要知道客户端的端口号，可以使用getsockname函数获取。

// 对于客户端来说，在调用socket时候内核还不会分配IP和端口，
// 此时调用getsockname不会获得正确的端口和地址，链接没建立更不能用getpeername，
// 当然如果客户端调用bind绑定端口后也可以使用getsockname。
// 想要正确得到对方地址，则必须在链接建立以后，同样链接建立以后，
// 此时客户端地址和端口就已经被指定，也是调用getsockname的时机。

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
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  showHost(sock, "client after create :");
  // client after create : addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  showPeer(sock, "server after create :");
  // server after create : addr = 255.127.0.0:12559

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  showHost(sock, "client after connect:");
  // client after connect: addr = 127.0.0.1:35326
  showPeer(sock, "server after connect:");
  // server after connect: addr = 127.0.0.1:1234

  char buffer[40];
  read(sock, buffer, sizeof(buffer) - 1);
  printf("Message form server: %s\n", buffer);
  close(sock);
}

void func2() {
  // 服务器和客户端都可以bind，bind并不是服务器的专利，服务端进程必须要bind端口，
  // 而一般情况下客户端是不用调用bind函数的，一切都交给内核搞定，除非：
  // 1.需要在建连前就知道端口；
  // 2.需要通过指定的端口来通讯。
  // 绑定之后只能运行一个client的程序，因为如果运行多个，端口重复就重复了。

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  showHost(sock, "client after create :");
  // client after create : addr = 0.0.0.0:0

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  showPeer(sock, "server after create :");
  // server after create : addr = 255.127.0.0:13212

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = htonl(INADDR_ANY);
  local.sin_port = htons(9000);
  bind(sock, (struct sockaddr *)&local, sizeof(local));

  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  showHost(sock, "client after connect:");
  // client after connect: addr = 127.0.0.1:9000
  showPeer(sock, "server after connect:");
  // server after connect: addr = 127.0.0.1:1234

  char buffer[40];
  read(sock, buffer, sizeof(buffer) - 1);
  printf("Message form server: %s\n", buffer);
  close(sock);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s 1\n", argv[0]);
    return 1;
  }

  if (argv[1][0] == '1') {
    func1();
  } else {
    func2();
  }

  return 0;
}