#include <stdio.h>

// Windows下的socket程序和Linux思路相同，但细节有所差别：
// 1.Windows下的socket程序依赖Winsock.dll或ws2_32.dll，必须提前加载，DLL有两种加载方式。
// 2.Linux使用文件描述符的概念，而Windows使用文件句柄的概念；
//   Linux不区分socket文件和普通文件，而Windows区分；
//   Linux下socket()函数的返回值为int类型，而Windows下为SOCKET类型，也就是句柄。
// 3.Linux下使用read()/write()函数读写，而Windows下使用recv()/send()函数发送和接收。
// 4.关闭socket时，Linux使用close()函数，而Windows使用closesocket()函数。

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")  // 加载ws2_32.dll
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

int main() {
#ifdef WIN32
  // 使用DLL之前，还需要调用WSAStartup()函数进行初始化，以指明WinSock规范的版本，它的原型为：
  // int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
  // wVersionRequested为WinSock规范的版本号，低字节为主版本号，高字节为副版本号（修正版本号）；
  // lpWSAData为指向WSAData结构体的指针。
  // wVersionRequested参数用来指明我们希望使用的版本号，它的类型为WORD，
  // 等价于unsigned short，是一个整数，所以需要用MAKEWORD()宏对版本号进行转换。
  // ws2_32.dll支持的最高版本为2.2，建议使用的版本也是2.2。
  // 综上所述：
  // WinSock编程的第一步就是加载ws2_32.dll，然后调用WSAStartup()函数进行初始化，并指明要使用的版本号。
  WSADATA wsaData;  // 初始化DLL
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  // 创建套接字
#ifdef WIN32
  // Windows也用socket函数创建套接字，除了返回值类型于Linux不同，其他都是相同的。
  // Windows不把套接字作为普通文件对待，而是返回SOCKET类型的句柄。
  SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
  // 在Linux下使用<sys/socket.h>头文件中socket()函数来创建套接字，原型为：
  // int socket(int af, int type, int protocol);
  // 1.af为地址族，也就是IP地址类型，常用的有AF_INET和AF_INET6。
  //   AF是Address Family的简写，INET是Inetnet的简写。
  //   AF_INET表示IPv4地址，例如127.0.0.1，代表本机；
  //   AF_INET6表示IPv6地址，例如1030::C9B4:FF12:48AA:1A2B。
  //   也可以使用PF前缀，PF是Protocol Family的简写，它和AF是一样的。
  //   例如，PF_INET等价于AF_INET，PF_INET6等价于AF_INET6。
  // 2.type为数据传输方式/套接字类型，常用的有:
  //   SOCK_STREAM（流格式套接字/面向连接的套接字）;
  //   SOCK_DGRAM（数据报套接字/无连接的套接字）。
  // 3.protocol表示传输协议，常用的有IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP传输协议和UDP传输协议。

  // 有了地址类型和数据传输方式，还不足以决定采用哪种协议吗？为什么还需要第三个参数呢？
  // 正如大家所想，一般情况下有了af和type两个参数就可以创建套接字了，
  // 操作系统会自动推演出协议类型，除非遇到这样的情况：有两种不同的协议支持同一种地址类型和数据传输类型。
  // 如果我们不指明使用哪种协议，操作系统是没办法自动推演的。

  // 如果使用SOCK_STREAM传输数据，那么满足这两个条件的协议只有TCP，因此可以这样来调用socket()函数：
  // int tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  // 这种套接字称为TCP套接字。
  // 如果使用SOCK_DGRAM传输方式，那么满足这两个条件的协议只有UDP，因此可以这样来调用socket()函数：
  // int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // 这种套接字称为UDP套接字。

  // 上面两种情况都只有一种协议满足条件，可以将protocol的值设为0，
  // 系统会自动推演出应该使用什么协议，如下所示：
  // int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);  // 创建TCP套接字
  // int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);  // 创建UDP套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif

  // socket()函数用来创建套接字，确定套接字的各种属性，
  // 然后服务器端要用bind()函数将套接字与特定的IP地址和端口绑定起来，
  // 只有这样，流经该IP地址和端口的数据才能交给套接字处理。
  // 类似地，客户端也要用connect()函数建立连接。

  // 将套接字和IP、端口绑定
  struct sockaddr_in serv_addr;
  //   struct sockaddr_in {
  //     sa_family_t sin_family;   // 地址族，也就是地址类型
  //     uint16_t sin_port;        // 16位的端口号
  //     struct in_addr sin_addr;  // 32位IP地址
  //     char sin_zero[8];         // 不使用，一般用0填充
  //   };
  // 1.sin_family和socket()的第一个参数的含义相同，取值也要保持一致。
  // 2.sin_prot为端口号。uint16_t的长度为两个字节，理论上端口号的取值范围为0~65536，
  //   但0~1023的端口一般由系统分配给特定的服务程序，例如Web服务为80，FTP服务为21，
  //   所以程序要尽量在1024~65536之间分配端口号，端口号需要用htons()函数转换。
  // 3.sin_addr是struct in_addr结构体类型的变量：
  //   struct in_addr {
  //     in_addr_t s_addr;  // 32位的IP地址
  //   };
  //   in_addr_t在<netinet/in.h>中定义，等价于unsigned long，长度为4个字节。
  //   也就是说，s_addr是一个整数，而IP地址是一个字符串，所以需要inet_addr()函数进行转换。
  // 4.sin_zero[8]是多余的8个字节，没有用，一般使用memset()函数填充为0。

  // 为什么要搞这么复杂，结构体中嵌套结构体，而不用sockaddr_in的一个成员变量来指明IP地址呢？
  // socket()函数的第一个参数已经指明了地址类型，
  // 为什么在sockaddr_in结构体中还要再说明一次呢，这不是啰嗦吗？
  // 应该是历史原因吧，后面的接口总要兼容前面的代码。

  // 为什么使用sockaddr_in而不使用sockaddr？
  // bind()第二个参数的类型为sockaddr，而代码中却使用sockaddr_in，
  // 然后再强制转换为sockaddr，这是为什么呢？
  //   struct sockaddr {
  //     sa_family_t sin_family;  // 地址族（Address Family），也就是地址类型
  //     char sa_data[14];        // IP地址和端口号
  //   };
  // sockaddr和sockaddr_in的长度相同，都是16字节，
  // 只是将IP地址和端口号合并到一起，用一个成员sa_data表示。
  // 要想给a_data赋值，必须同时指明IP地址和端口号，例如127.0.0.1:80，
  // 遗憾的是，没有相关函数将这个字符串转换成需要的形式，
  // 也就很难给sockaddr类型的变量赋值，所以使用sockaddr_in来代替。
  // 这两个结构体的长度相同，强制转换类型时不会丢失字节，也没有多余的字节。

  // 可以认为，sockaddr是一种通用的结构体，可以用来保存多种类型的IP地址和端口号，
  // 而sockaddr_in是专门用来保存IPv4地址的结构体。
  // 另外还有sockaddr_in6，用来保存IPv6地址，它的定义如下：
  //   struct sockaddr_in6 {
  //     sa_family_t sin6_family;    // (2)地址类型，取值为AF_INET6
  //     in_port_t sin6_port;        // (2)16位端口号
  //     uint32_t sin6_flowinfo;     // (4)IPv6流信息
  //     struct in6_addr sin6_addr;  // (4)具体的IPv6地址
  //     uint32_t sin6_scope_id;     // (4)接口范围ID
  //   };
  // 正是由于通用结构体sockaddr使用不便，才针对不同的地址类型定义了不同的结构体。

  memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
#ifdef WIN32
  sock_addr.sin_family = PF_INET;  // 使用IPv4地址
#else
  serv_addr.sin_family = AF_INET;  // 使用IPv4地址
#endif
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 具体的IP地址
  serv_addr.sin_port = htons(1234);                    // 端口

  // bind()函数的原型为：
  // int bind(int sock, struct sockaddr *addr, socklen_t addrlen); // Linux
  // int bind(SOCKET sock, const struct sockaddr *addr, int addrlen); // Windows
  // sock为socket文件描述符，addr为sockaddr结构体变量的指针，
  // addrlen为addr变量的大小，可由sizeof()计算得出。

#ifdef WIN32
  bind(serv_sock, (SOCKADDR *)&sock_addr, sizeof(SOCKADDR));
#else
  bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
#endif

  // 对于服务器端程序，使用bind()绑定套接字后，还需要使用listen()函数让套接字进入被动监听状态，
  // 再调用accept()函数，就可以随时响应客户端的请求了。

  // 通过listen()函数可以让套接字进入被动监听状态，它的原型为：
  // int listen(int sock, int backlog);  // Linux
  // int listen(SOCKET sock, int backlog);  // Windows
  // sock为需要进入监听状态的套接字，backlog为请求队列的最大长度。
  // 所谓被动监听，是指当没有客户端请求时，套接字处于睡眠状态，
  // 只有当接收到客户端请求时，套接字才会被唤醒来响应请求。
  listen(serv_sock, 20);
  // 当套接字正在处理客户端请求时，如果有新的请求进来，套接字是没法处理的，
  // 只能把它放进缓冲区，待当前请求处理完毕后，再从缓冲区中读取出来处理。
  // 如果不断有新的请求进来，它们就按照先后顺序在缓冲区中排队，直到缓冲区满。
  // 这个缓冲区，就称为请求队列（Request Queue）。
  // 缓冲区的长度（能存放多少个客户端请求）可以通过listen()函数的backlog参数指定，
  // 但究竟为多少并没有什么标准，可以根据你的需求来定，并发量小的话可以是10或者20。
  // 如果将backlog的值设置为SOMAXCONN，就由系统来决定请求队列长度，
  // 这个值一般比较大，可能是几百，或者更多。

  // 当请求队列满时，就不再接收新的请求，对于Linux，客户端会收到ECONNREFUSED错误，
  // 对于Windows，客户端会收到WSAECONNREFUSED错误。
  // 注意，listen()只是让套接字处于监听状态，并没有接收请求，接收请求需要使用accept()函数。

  // 当套接字处于监听状态时，可以通过accept()函数来接收客户端请求，它的原型为：
  // int accept(int sock, struct sockaddr *addr, socklen_t *addrlen); // Linux
  // SOCKET accept(SOCKET sock, struct sockaddr *addr, int *addrlen); // Windows
  // 它的参数与listen()和connect()是相同的：
  // sock为服务器端套接字，addr为sockaddr_in结构体变量，addrlen为参数addr的长度，可由sizeof()求得。
  // accept()返回一个新的套接字来和客户端通信，addr保存了客户端的IP地址和端口号，
  // 而sock是服务器端的套接字，大家注意区分。
  // 后面和客户端通信时，要使用这个新生成的套接字，而不是原来服务器端的套接字。

  // 最后需要说明的是：listen()只是让套接字进入监听状态，并没有真正接收客户端请求，
  // listen()后面的代码会继续执行，直到遇到accept()。
  // accept()会阻塞程序执行（后面代码不能被执行），直到有新的请求到来。

#ifdef WIN32
  // 接收客户端请求
  SOCKADDR clntAddr;
  int nSize = sizeof(SOCKADDR);
  SOCKET clntSock = accept(serv_sock, (SOCKADDR *)&clntAddr, &nSize);
  // 向客户端发送数据
  char *str = "Hello World!";
  // Windows和Linux不同，Windows区分普通文件和套接字，并定义了专门的接收和发送的函数。
  // 从服务器端发送数据使用send()函数，它的原型为：
  // int send(SOCKET sock, const char *buf, int len, int flags);
  // sock为要发送数据的套接字，buf为要发送的数据的缓冲区地址，
  // len为要发送的数据的字节数，flags为发送数据时的选项。
  // 最后的flags参数一般设置为0或NULL，初学者不必深究。
  send(clntSock, str, strlen(str) + sizeof(char), NULL);

  // 关闭套接字
  closesocket(clntSock);
  closesocket(serv_sock);

  // 终止DLL的使用
  WSACleanup();
#else
  // 接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  // 向客户端发送数据
  char str[] = "Hello World!";
  // Linux不区分套接字文件和普通文件：
  // 使用write()可以向套接字中写入数据，使用read()可以从套接字中读取数据。
  // write()的原型为：
  // ssize_t write(int fd, const void *buf, size_t nbytes);
  // fd为要写入的文件的描述符，buf为要写入的数据的缓冲区地址，nbytes为要写入的数据的字节数。
  // size_t是通过typedef声明的unsigned int类型；
  // ssize_t在size_t前面加了一个s，代表signed，即ssize_t是typedef声明的signed int类型。
  // write()函数会将缓冲区buf中的nbytes个字节写入文件fd，成功则返回写入的字节数，失败则返回-1。
  write(clnt_sock, str, sizeof(str));

  // 关闭套接字
  close(clnt_sock);
  close(serv_sock);
#endif

  return 0;
}