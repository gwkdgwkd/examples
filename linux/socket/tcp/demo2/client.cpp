#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

// 回声客户端

int main() {
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  while (1) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("Input a string: ");
    fgets(bufSend, BUF_SIZE, stdin);

    write(sock, bufSend, strlen(bufSend));

    read(sock, bufRecv, BUF_SIZE);
    printf("Message form server: %s\n", bufRecv);

    memset(bufSend, 0, BUF_SIZE);
    memset(bufRecv, 0, BUF_SIZE);

    // 客户端也会清理socket相关资源，所以需要将socket()放在while循环内部，
    // 因为每次请求完毕都会清理socket，下次发起请求时需要重新创建
    close(sock);
  }

  return 0;
}