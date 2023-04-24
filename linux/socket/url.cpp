#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 客户端中直接使用IP地址会有很大的弊端，IP地址会经常变动，
// 一旦IP地址变化，客户端软件就会出现错误。
// 而使用域名会方便很多，注册后的域名只要每年续费就永远属于自己的，
// 更换IP地址时修改域名解析即可，不会影响软件的正常使用。
// 域名仅仅是IP地址的一个助记符，目的是方便记忆，
// 通过域名并不能找到目标计算机，通信之前必须要将域名转换成IP地址。

// gethostbyname()函数可以完成这种转换，它的原型为：
// struct hostent *gethostbyname(const char *hostname);
// hostname为主机名，也就是域名。
// 使用该函数时，只要传递域名字符串，就会返回域名对应的IP地址。
// 返回的地址信息会装入hostent结构体，如果失败则返回NULL,该结构体的定义如下：
// struct hostent {
//   char *h_name;        // 主机规范名
//   char **h_aliases;    // 主机别名，可能有多个别名，以空指针结束
//   int h_addrtype;      // 主机IP地址类型
//   int h_length;        // 主机IP地址长度
//   char **h_addr_list;  // 主机IP地址，需要使用inet_ntop函数转换
// }
// 该结构体不只返回IP地址，还会附带其他信息，只需关注最后一个成员h_addr_list。
// 下面是对各成员的说明：
// 1.h_name：官方域名（Official domain name）。
//   官方域名代表某一主页，但实际上一些著名公司的域名并未用官方域名注册。
// 2.h_aliases：别名，可以通过多个域名访问同一主机。
//   同一IP地址可以绑定多个域名，因此除了当前域名还可以指定其他域名。
// 3.h_addrtype：gethostbyname()不仅支持IPv4，还支持IPv6，
//   可以通过此成员获取IP地址的地址族（地址类型）信息，
//   IPv4对应AF_INET，IPv6对应AF_INET6。
// 4.h_length：保存IP地址长度，IPv4的长度为4个字节，IPv6的长度为16个字节。
// 5.h_addr_list：这是最重要的成员，通过该成员以整数形式保存域名对应的IP地址。
//   对于用户较多的服务器，可能会分配多个IP地址给同一域名，利用多个服务器进行均衡负载。

// 通常都是在网络联通情况下调用gethostbyname函数，可能并不会发现它其实会阻塞。
// 在网络信号不好或连接的热点不能上网的情况下，gethostbyname会阻塞10秒钟。
// 要解决这个问题，我们可以引入超时信号或在调用gethostbyname之前主动查询DNS信息。
// 在访问DNS时我们可以控制等待时间，只有DNS访问成功的情况下才再调用gethostbyname。
// 也就是将阻塞时间挪到DNS访问，并可自控。

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage:%s www.baidu.com\n", argv[0]);
    return -1;
  }

  time_t begin = time(NULL);
  struct hostent *host = gethostbyname(argv[1]);
  if (!host) {
    // 如果gethostbyname函数失败，会设置全局的h_errno标识错误类型，
    // 并可通过const char *hstrerror(int err);函数输出具体错误描述：
    printf("gethostbyname error [%s] for host:%s\n", hstrerror(h_errno),
           argv[1]);
    time_t end = time(NULL);
    printf("gethostbyname error, used [%ld] times\n", end - begin);
    return 0;
  }

  printf("official hostname:%s\n", host->h_name);

  // 别名
  for (int i = 0; host->h_aliases[i]; i++) {
    printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
  }

  // 地址类型
  printf("Address type: %s\n",
         (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");

  // IP地址
  for (int i = 0; host->h_addr_list[i]; i++) {
    printf("IP addr %d: %s\n", i + 1,
           inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
  }

  // 用指针操作：
  // for (char **ptr = host->h_aliases; *ptr != nullptr; ptr++) {
  //   printf("alias:%s\n", *ptr);
  // }
  // char str[128];
  // switch (host->h_addrtype) {
  //   case AF_INET:
  //   case AF_INET6: {
  //     char **pptr = host->h_addr_list;
  //     for (; *pptr != NULL; pptr++)
  //       printf(" address:%s\n",
  //              inet_ntop(host->h_addrtype, *pptr, str, sizeof(str)));
  //     break;
  //   }
  //   default:
  //     printf("unknown address type\n");
  //     break;
  // }
}

// ./a.out asdfasdfsdf
// gethostbyname error [Host name lookup failure] for host:asdfasdfsdf
// gethostbyname error, used [0] times

// ./a.out www.baidu.com
// official hostname:www.a.shifen.com
// Aliases 1: www.baidu.com
// Address type: AF_INET
// IP addr 1: 110.242.68.4
// IP addr 2: 110.242.68.3

// ./a.out www.sina.com
// official hostname:spool.grid.sinaedge.com
// Aliases 1: www.sina.com
// Address type: AF_INET
// IP addr 1: 123.126.45.205

// ./a.out www.163.com
// official hostname:z163picipv6.v.bsgslb.cn
// Aliases 1: www.163.com
// Aliases 2: www.163.com.163jiasu.com
// Aliases 3: www.163.com.bsgslb.cn
// Address type: AF_INET
// IP addr 1: 116.177.237.157
// IP addr 2: 116.177.237.154
// IP addr 3: 116.177.237.156
// IP addr 4: 116.177.237.153
// IP addr 5: 116.177.237.155
// IP addr 6: 116.177.237.152
