#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

void showHost(int fd, const char *str) {
  struct sockaddr_in addr;
  int len = sizeof(addr);
  getsockname(fd, (struct sockaddr *)&addr, (socklen_t *)&len);
  char ip[20];
  printf("%s addr = %s:%d\n", str,
         inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, sizeof(ip)),
         ntohs(addr.sin_port));
}

int main() {
  int serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
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

  struct sockaddr clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUF_SIZE] = {0};

  int strLen =
      recvfrom(serv_sock, buffer, BUF_SIZE, 0, &clnt_addr, &clnt_addr_size);

  char ip[20];
  printf(
      "received from %s at port %d\n",
      inet_ntop(AF_INET, &(((struct sockaddr_in *)&clnt_addr)->sin_addr.s_addr),
                ip, sizeof(ip)),
      ntohs(((struct sockaddr_in *)&clnt_addr)->sin_port));
  // received from 127.0.0.1 at port 51285

  sendto(serv_sock, buffer, strLen, 0, &clnt_addr, clnt_addr_size);

  close(serv_sock);

  return 0;
}