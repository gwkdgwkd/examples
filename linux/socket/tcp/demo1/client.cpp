#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#else
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

int main() {
#ifdef WIN32
  // 初始化DLL
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

// 创建套接字
#ifdef WIN32
  SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
  int sock = socket(AF_INET, SOCK_STREAM, 0);
#endif

  // 向服务器（特定的IP和端口）发起请求
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
#ifdef WIN32
  sock_addr.sin_family = PF_INET;
#else
  serv_addr.sin_family = AF_INET;  // 使用IPv4地址
#endif
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 具体的IP地址
  serv_addr.sin_port = htons(1234);                    // 端口

// connect()函数用来建立连接，它的原型为：
// int connect(int sock, struct sockaddr *serv_addr, socklen_t addrlen); // L
// int connect(SOCKET sock, const struct sockaddr *serv_addr, int addrlen); // W
// 各个参数的说明和bind()相同。
#ifdef WIN32
  connect(sock, (SOCKADDR*)&sock_addr, sizeof(SOCKADDR));
#else
  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
#endif

#ifdef WIN32
  // 接收服务器传回的数据
  char szBuffer[MAXBYTE] = {0};
  // 在客户端接收数据使用recv()函数，它的原型为：
  // int recv(SOCKET sock, char *buf, int len, int flags);
  recv(sock, szBuffer, MAXBYTE, NULL);
  // 输出接收到的数据
  printf("Message form server: %s\n", szBuffer);
  // 关闭套接字
  closesocket(sock);
  // 终止使用DLL
  WSACleanup();
#else
  // 读取服务器传回的数据
  char buffer[40];
  // read()的原型为：
  // ssize_t read(int fd, void *buf, size_t nbytes);
  // fd为要读取的文件的描述符，buf为要接收数据的缓冲区地址，nbytes为要读取的数据的字节数。
  // read()函数会从fd文件中读取nbytes个字节并保存到缓冲区buf，
  // 成功则返回读取到的字节数（但遇到文件结尾则返回0），失败则返回-1。
  read(sock, buffer, sizeof(buffer) - 1);

  printf("Message form server: %s\n", buffer);

  // 关闭套接字
  close(sock);
#endif

  return 0;
}