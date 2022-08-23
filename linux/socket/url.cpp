#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

// 客户端中直接使用IP地址会有很大的弊端，一旦IP地址变化（IP地址会经常变动），客户端软件就会出现错误。
// 而使用域名会方便很多，注册后的域名只要每年续费就永远属于自己的，
// 更换IP地址时修改域名解析即可，不会影响软件的正常使用。

int main() {
  // 域名仅仅是IP地址的一个助记符，目的是方便记忆，通过域名并不能找到目标计算机，
  // 通信之前必须要将域名转换成IP地址。
  // gethostbyname()函数可以完成这种转换，它的原型为：
  // struct hostent *gethostbyname(const char *hostname);
  // hostname为主机名，也就是域名。
  // 使用该函数时，只要传递域名字符串，就会返回域名对应的IP地址。
  // 返回的地址信息会装入hostent结构体，该结构体的定义如下：
  // struct hostent {
  //   char *h_name;        // official name
  //   char **h_aliases;    // alias list
  //   int h_addrtype;      // host address type
  //   int h_length;        // address lenght
  //   char **h_addr_list;  // address list
  // }
  // 从该结构体可以看出，不只返回IP地址，还会附带其他信息，只需关注最后一个成员h_addr_list。
  // 下面是对各成员的说明：
  // 1.h_name：官方域名（Official domain name）。
  //   官方域名代表某一主页，但实际上一些著名公司的域名并未用官方域名注册。
  // 2.h_aliases：别名，可以通过多个域名访问同一主机。
  //   同一IP地址可以绑定多个域名，因此除了当前域名还可以指定其他域名。
  // 3.h_addrtype：gethostbyname()不仅支持IPv4，还支持IPv6，
  //   可以通过此成员获取IP地址的地址族（地址类型）信息，
  //   IPv4对应AF_INET，IPv6对应AF_INET6。
  // 4.h_length：保存IP地址长度。IPv4的长度为4个字节，IPv6的长度为16个字节。
  // 5.h_addr_list：这是最重要的成员。通过该成员以整数形式保存域名对应的IP地址。
  //   对于用户较多的服务器，可能会分配多个IP地址给同一域名，利用多个服务器进行均衡负载。

  struct hostent *host = gethostbyname("www.baidu.com");
  if (!host) {
    puts("Get IP address error!");
    exit(0);
  }

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

  // Aliases 1: www.baidu.com
  // Address type: AF_INET
  // IP addr 1: 110.242.68.3
  // IP addr 2: 110.242.68.4
}
