#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

int main() {
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  printf("Input a string: ");
  fgets(bufSend, BUF_SIZE, stdin);

  bufSend[strlen(bufSend) - 1] = 0;

  // 客户端连续三次向服务器端发送数据
  for (int i = 0; i < 3; ++i) {
    write(sock, bufSend, strlen(bufSend));
  }

  read(sock, bufRecv, BUF_SIZE);
  printf("Message form server: %s\n", bufRecv);

  close(sock);

  return 0;
}