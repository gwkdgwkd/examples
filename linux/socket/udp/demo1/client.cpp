#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 100

// 基于UDP的回声客户端

int main() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  // server.cpp中没有使用listen()函数，client.cpp中也没有使用connect()函数，因为UDP不需要连接。

  struct sockaddr from_addr;
  socklen_t from_addr_size = sizeof(from_addr);
  while (1) {
    char buffer[BUF_SIZE] = {0};

    printf("Input a string: ");
    fgets(buffer, BUF_SIZE, stdin);

    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr,
           sizeof(serv_addr));
    int strLen =
        recvfrom(sock, buffer, BUF_SIZE, 0, &from_addr, &from_addr_size);
    buffer[strLen] = 0;
    printf("Message form server: %s\n", buffer);

    write(sock, buffer, strlen(buffer));
  }

  close(sock);

  return 0;
}