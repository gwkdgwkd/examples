#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_MAX 1024

int main(int argc, char** argv) {
  int sockfd, n, rec_len;
  char recvline[BUFF_MAX], sendline[BUFF_MAX];
  char buf[BUFF_MAX];
  struct sockaddr_in servaddr;

  if (argc <= 2) {
    printf("Usage: %s ip_address port_number\n", argv[0]);
    return 1;
  }
  const char* ip = argv[1];
  int port = atoi(argv[2]);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
    printf("inet_pton error for %s\n", ip);
    exit(0);
  }

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }

  printf("send msg to server: \n");
  fgets(sendline, BUFF_MAX, stdin);
  if (send(sockfd, sendline, strlen(sendline), 0) < 0) {
    printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }
  if ((rec_len = recv(sockfd, buf, BUFF_MAX, 0)) == -1) {
    perror("recv error");
    exit(1);
  }
  buf[rec_len] = '\0';
  printf("Received : %s \n", buf);
  close(sockfd);
  exit(0);
}