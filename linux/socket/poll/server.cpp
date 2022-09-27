#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// 当需要同时监听多个文件描述符时，
// 就需要I/O复用函数，I/O复用函数有select、poll、epoll。

// poll()接受一个指向结构struct pollfd列表的指针，
// 其中包括了你想测试的文件描述符和事件。
// 事件由一个在结构中事件域的比特掩码确定。
// 当前的结构在调用后将被填写并在事件发生后返回。
// 通过传入的events的类型去判断返回的类型是否一致，如果一致就该干事了。

// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// struct pollfd {
//   int fd;         // file descriptor
//   short events;   // requested events
//   short revents;  // returned events
// }
// fds是要监听的fd的数组；
// nfds是数组个数；
// timeout是超时时间，-1是阻塞；

// events
// POLLIN：普通或优先级带数据可读；
// POLLRDNORM：普通数据可读；
// POLLRDBAND：优先级带数据可读；
// POLLPRI：高优先级数据可读；
// POLLOUT：普通数据可写；
// POLLWRNORM：普通数据可写；
// POLLWRBAND：优先级带数据可写；
// POLLERR：发生错误；
// POLLHUP：发生挂起；
// POLLNVAL：描述字不是一个打开的文件；

#define MAX_POLLFD_NUM 2
#define BUFF_MAX 1024

typedef void *(*Server_Rrocess_Thread_Fun)(void *arg);
typedef struct sever_thread_fun {
  Server_Rrocess_Thread_Fun Server_Process_Client_Conn1;
  Server_Rrocess_Thread_Fun Server_Process_Client_Conn2;
} Poll_Server_Process_Clinet_FUN_t;

void *Poll_Conn1_Process(void *arg) {
  unsigned char ucBufArr[BUFF_MAX] = {0};
  unsigned long ulSize = 0;
  int connect_fd;
  printf("come pthread conn1 fun to proess conn1\n");
  if ((connect_fd = accept(*(int *)arg, (struct sockaddr *)NULL, NULL)) == -1) {
    printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
  }
  ulSize = recv(connect_fd, ucBufArr, BUFF_MAX, 0);

  if (send(connect_fd, "Hello,you are connected 1!\n", 26, 0) == -1)
    perror("send error");
  ucBufArr[ulSize] = '\0';
  // sleep(20);
  printf("recv msg from client: %s\n", ucBufArr);
  close(connect_fd);
  pthread_exit((void *)1);
}

void *Poll_Conn2_Process(void *arg) {
  unsigned char ucBufArr[BUFF_MAX] = {0};
  unsigned long ulSize = 0;
  int connect_fd;
  printf("come pthread conn2 fun to proess conn2\n");
  if ((connect_fd = accept(*(int *)arg, (struct sockaddr *)NULL, NULL)) == -1) {
    printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
  }
  ulSize = recv(connect_fd, ucBufArr, BUFF_MAX, 0);

  if (send(connect_fd, "Hello,you are connected 2!\n", 26, 0) == -1)
    perror("send error");
  ucBufArr[ulSize] = '\0';
  // sleep(20);
  printf("recv msg from client: %s\n", ucBufArr);
  close(connect_fd);
  pthread_exit((void *)2);
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("Usage: %s port_number\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);

  int poll_ret = 0;
  pthread_t thread_conn1;
  pthread_t thread_conn2;
  int server_socket_fd_conn1;
  int server_socket_fd_conn2;
  struct sockaddr_in servaddr_conn1;
  struct sockaddr_in serveraddr_conn2;

  struct pollfd pollfd_arr[MAX_POLLFD_NUM];  // poll var
  Poll_Server_Process_Clinet_FUN_t server_conn_handel;
  server_conn_handel.Server_Process_Client_Conn1 = Poll_Conn1_Process;
  server_conn_handel.Server_Process_Client_Conn2 = Poll_Conn2_Process;

  if ((server_socket_fd_conn1 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("create socket conn1 error: %s(errno: %d)\n", strerror(errno),
           errno);
    exit(0);
  }
  if ((server_socket_fd_conn2 = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("create socket conn2 error: %s(errno: %d)\n", strerror(errno),
           errno);
    exit(0);
  }

  // init socket 1 for conn1
  memset(&servaddr_conn1, 0, sizeof(servaddr_conn1));
  servaddr_conn1.sin_family = AF_INET;
  servaddr_conn1.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr_conn1.sin_port = htons(port);

  // init socket 2 for conn2
  memset(&serveraddr_conn2, 0, sizeof(serveraddr_conn2));
  serveraddr_conn2.sin_family = AF_INET;
  serveraddr_conn2.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr_conn2.sin_port = htons(port + 1);

  // bind connect 1 socket
  if (bind(server_socket_fd_conn1, (struct sockaddr *)&servaddr_conn1,
           sizeof(servaddr_conn1)) == -1) {
    printf("bind socket error1: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }
  // bind connect 2 socket
  if (bind(server_socket_fd_conn2, (struct sockaddr *)&serveraddr_conn2,
           sizeof(serveraddr_conn2)) == -1) {
    printf("bind socket error2: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }
  // listen connect 1
  if (listen(server_socket_fd_conn1, 10) == -1) {
    printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }
  // listen connect 2
  if (listen(server_socket_fd_conn2, 10) == -1) {
    printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
    exit(0);
  }
  pollfd_arr[0].events = POLLRDNORM;  // only read event
  pollfd_arr[0].fd = server_socket_fd_conn1;

  pollfd_arr[1].events = POLLRDNORM;  // only read event
  pollfd_arr[1].fd = server_socket_fd_conn2;
  while (1) {
    poll_ret = poll(pollfd_arr, MAX_POLLFD_NUM, -1);
    if (pollfd_arr[0].revents & POLLRDNORM) {
      pthread_create(&thread_conn1, NULL,
                     server_conn_handel.Server_Process_Client_Conn1,
                     (void *)(&pollfd_arr[0].fd));
    }
    if (pollfd_arr[1].revents & POLLRDNORM) {
      pthread_create(&thread_conn2, NULL,
                     server_conn_handel.Server_Process_Client_Conn2,
                     (void *)(&pollfd_arr[1].fd));
    }
    sleep(1);
  }
  return 0;
}