#include "messager_mobile_lite.h"
#include "messager_mobile_perfect.h"
#include "messager_pc_lite.h"
#include "messager_pc_perfect.h"

// 客户需求演化：
// 3.客户又说要支持不同的操作系统，于是再抽象一层表示操作系统，
//   再分别为每个操作系统派生具体类；

// 假设m表示要支持的操作系统数量，n表示要支持的算法的数量，
// 那么有m*n个具体类，也就是4个具体类。
// 可能用户的需求又有变化，比如说增加了一个新的算法，有要支持平板，
// 那么需要3*3，9个具体类，导致了类的数量迅速膨胀。

void func(Messager *p) {
  p->login();
  p->sendMessage();
  p->sendPicture();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  Messager *p;
  switch (type) {
    case 0:
      p = new MessagerPCLite();  // 编译时装配
      func(p);
      // login: pc connect
      // sendMessage: pc writeText
      // sendPicture: pc drawShape
      break;
    case 1:
      p = new MessagerPCPerfect();
      func(p);
      // pc playSound -> login: pc connect
      // pc playSound -> sendMessage: pc writeText
      // pc playSound -> sendPicture: pc drawShape
      break;
    case 2:
      p = new MessagerMobileLite();
      func(p);
      // login: mobile connect
      // sendMessage: mobile writeText
      // sendPicture: mobile drawShape
      break;
    case 3:
      p = new MessagerMobilePerfect();
      func(p);
      // mobile playSound -> login: mobile connect
      // mobile playSound -> sendMessage: mobile writeText
      // mobile playSound -> sendPicture: mobile drawShape
      break;
    default:
      return -1;
  }
  delete p;

  return 0;
}