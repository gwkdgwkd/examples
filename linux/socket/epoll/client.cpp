#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("Usage: %s ip_address port_number\n", argv[0]);
    return 1;
  }
  const char *ip = argv[1];
  int port = atoi(argv[2]);

  int cfd;
  struct sockaddr_in serv_addr;
  char buf[BUFSIZ];
  int n;

  cfd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  // 将点十进制字节串转换为网络字节序：
  inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);

  connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  while (1) {
    fgets(buf, sizeof(buf), stdin);
    write(cfd, buf, strlen(buf));
    usleep(1000);
  }
  close(cfd);

  return 0;
}