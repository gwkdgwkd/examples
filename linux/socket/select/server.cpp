#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

// int select(int maxfdp,
//            fd_set *readfds,
//            fd_set *writefds,
//            fd_set *errorfds,
//            struct timeval *timeout);
// 1.maxfdp：传入参数，
//   集合中所有文件描述符的范围，即最大文件描述符值+1；
// 2.readfds：传入传出参数，
//   select调用时传入要监听的可读文件描述符集合，
//   select返回时传出发生可读事件的文件描述符集合；
// 3.writefds：传入传出参数，
//   select调用时传入要监听的可写文件描述符集合，
//   select返回时传出发生可写事件的文件描述符集合；
// 4.errorfds：传出参数，
//   select返回时传出发生事件（可读和可写）中异常事件的文件描述符集合；
// 5.timeout：传入参数，设置select阻塞的时间，
//   若设置为NULL，则select一直阻塞直到有事件发生；
//   若设置为0，则select为非阻塞模式，执行后立即返回；
//   若设置为一个大于0的数，即select的阻塞时间，
//   若阻塞时间内有事件发生就返回，否则时间到了立即返回。

// fd_set是自定义的一个数据结构，可看作一个集合，存放可读、可写或异常事件的文件描述符。
// fd_set集合通常有以下四个宏来操作：
// 1.清空fdset中所有文件描述符，void FD_ZERO(fd_set *fdset);
// 2.添加文件描述符fd到集合fdset中，void FD_SET(int fd,fd_set *fdset);
// 3.将文件描述符fd从集合fdset中去除，void FD_CLR(int fd,fd_set *fdset);
// 4.判断文件描述符fd是否在集合fdset中，int FD_ISSET(int fd,fd_set *fdset);

// select工作原理：
// 传入要监听的文件描述符集合（可读、可写或异常）开始监听，
// select处于阻塞状态，当有事件发生或设置的等待时间timeout到了就会返回，
// 返回之前自动去除集合中无事件发生的文件描述符，返回时传出有事件发生的文件描述符集合。
// 但select传出的集合并没有告诉用户集合中包括哪几个就绪的文件描述符，需要续进行遍历操作。

// select优点：
// 1.select的可移植性较好，可以跨平台；
// 2.select可设置的监听时间timeout精度更好，可精确到微秒，而poll为毫秒。
// select缺点：
// 1.select支持的文件描述符数量上限为1024，不能根据用户需求进行更改；
// 2.select每次调用时都要将文件描述符集合从用户态拷贝到内核态，开销较大；
// 3.select返回的就绪文件描述符集合，需要用户循环遍历所监听的所有文件描述符是否在该集合中，
//   当监听描述符数量很大时效率较低。

#define SERV_PORT 6666

int main() {
  int i, j, n;
  int maxfd, listenfd, connfd, sockfd;
  // FD_SETSIZE=1024，定义数组clientnt来储存已连接描述符，最多1023个：
  int nready, clientnt[FD_SETSIZE - 1];
  char buf[BUFSIZ], str;

  struct sockaddr_in client_addr, serv_addr;
  socklen_t client_addr_len;
  // 定义监听描述符集合allset和发生事件描述符集合readset：
  fd_set allset, readset;

  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // 端口号，将无符号短整型转换为网络字节顺序：
  serv_addr.sin_port = htons(SERV_PORT);
  // 一个主机可能有多个网卡，所以是本机的任意IP地址：
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // AF_INET表示使用32位IP地址，SOCK_STREAM表示使用TCP连接：
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  // 将服务器套接字地址与套接字描述符联系起来：
  bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  // 设置可监听的连接数量为1024：
  listen(listenfd, 1024);

  // 初始化最大文件描述符为监听描述符listenfd：
  maxfd = listenfd;

  // 数组clientnt储存的文件描述符的个数，初始化为-1：
  int maxi = -1;
  // 初始化clientnt数组，将数组所有元素置为-1：
  for (i = 0; i < FD_SETSIZE; i++) {
    clientnt[i] = -1;
  }

  FD_ZERO(&allset);           // 初始化监听集合
  FD_SET(listenfd, &allset);  // 将监听描述符listenfd添加到集合中

  while (1) {
    readset = allset;
    // select只监听可读事件，且为永久阻塞直到有事件发生：
    nready = select(maxfd + 1, &readset, NULL, NULL, NULL);
    if (nready < 0) {
      fputs("select error\n", stderr);
      exit(-1);
    }

    // 判断listenfd是否发生事件，若发生，则处理新客户端连接请求
    if (FD_ISSET(listenfd, &readset)) {
      client_addr_len = sizeof(client_addr);
      // 与请求客户端建立连接：
      connfd =
          accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);

      // 打印该客户端的IP地址和端口号：
      printf(
          "received from %s at port %d\n",
          inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, &str, sizeof(str)),
          ntohs(client_addr.sin_port));
      // 将connfd赋值给clientnt数组中第一个为-1的元素位置：
      for (i = 0; i < FD_SETSIZE; i++) {
        if (clientnt[i] < 0) {
          clientnt[i] = connfd;
          break;
        }
      }

      // 判断select监听的文件描述符的个数是否超过上限，
      // 减1的原因是要考虑监听描述符listenfd也属于select监控：
      if (i == FD_SETSIZE - 1) {
        fputs("too many clientnts\n", stderr);
        exit(1);
      }

      // 向监控的文件描述符集合allset中添加新的描述符connfd：
      FD_SET(connfd, &allset);
      if (connfd > maxfd) {
        maxfd = connfd;  // 更新最大文件描述符值
      }

      // 保证maxi永远是clientnt数组中最后一个非-1的元素的位置：
      if (i > maxi) {
        maxi = i;
      }

      // 如果nready=1，即只有一个发生事件的描述符，
      // 在此条件下必为listenfd，则返回循环位置，继续调用select监控；
      // 否则继续向下执行：
      if (--nready == 0) {
        continue;
      }
    }

    // 找到clientnt数组中发生事件的已连接描述符，并读取、处理数据：
    for (i = 0; i <= maxi; i++) {
      sockfd = clientnt[i];
      if (sockfd < 0) {
        continue;  // 已连接描述符失效，重新开始循环
      }

      if (FD_ISSET(sockfd, &readset)) {
        n = read(sockfd, buf, sizeof(buf));
        // 当客户端关闭连接，服务端也关闭连接：
        if (n == 0) {
          close(sockfd);
          // 解除select对该已连接文件描述符的监控：
          FD_CLR(sockfd, &allset);
          clientnt[i] = -1;
        } else if (n > 0) {
          for (j = 0; j < n; j++) buf[j] = toupper(buf[j]);
          sleep(2);
          write(sockfd, buf, n);
        }

        --nready;
        if (nready == 0) {
          break;  // 跳出for循环，还在while中
        }
      }
    }
  }
  close(listenfd);
  return 0;
}
