#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

// 回声服务器

int main() {
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
  char buffer[BUF_SIZE] = {0};
  while (1) {
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    int strLen = read(clnt_sock, buffer, BUF_SIZE);
    printf("%s\n", buffer);

    write(clnt_sock, buffer, strLen);

    // close()不仅会关闭服务器端的socket，还会通知客户端连接已断开
    close(clnt_sock);
    memset(buffer, 0, BUF_SIZE);
  }

  close(serv_sock);

  return 0;
}