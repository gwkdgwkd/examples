#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main() {
  char filename[BUF_SIZE] = {0};
  printf("filename to save: ");
  fgets(filename, BUF_SIZE, stdin);
  filename[strlen(filename) - 1] = 0;
  FILE *fp = fopen(filename, "wb");  // 以二进制方式打开（创建）文件
  if (fp == NULL) {
    printf("Cannot open file, press any key to exit!\n");
    exit(0);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  char buffer[BUF_SIZE] = {0};
  int count = 0;
  // Client端如何判断文件接收完毕，也就是何时结束while循环。
  // 最简单的结束while循环的方法当然是文件接收完毕后让read()函数返回0，那么，如何让read()返回0呢？
  // read()返回0的唯一时机就是收到FIN包时，也就是EOF。
  // FIN包表示数据传输完毕，计算机收到FIN包后就知道对方不会再向自己传输数据，当调用read()/recv()函数时，
  // 如果缓冲区中没有数据，就会返回0，表示读到了socket文件的末尾。
  // 这里Server调用shutdown()来发送FIN包。
  // server端直接调用close()/closesocket()会使输出缓冲区中的数据失效，
  // 文件内容很有可能没有传输完毕连接就断开了，而调用shutdown()会等待输出缓冲区中的数据传输完毕。
  while ((count = read(sock, buffer, BUF_SIZE)) > 0) {
    printf("%s\n", buffer);
    fwrite(buffer, count, 1, fp);
  }
  puts("File transfer success!");

  fclose(fp);
  close(sock);

  return 0;
}