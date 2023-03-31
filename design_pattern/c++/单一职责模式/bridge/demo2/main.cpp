#include "messager_lite.h"
#include "messager_perfect.h"

// 客户需求演化：
// 2.需求变化，需要在操作时播放声音，也就是说需要支持两个算法，
//   改变设计，通过一个抽象的基类，再定义两个具体类实现两个不同的算法；

void func(Messager *p) {
  p->login();
  p->sendMessage();
  p->sendPicture();
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  Messager *p;
  switch (type) {
    case 0:
      p = new MessagerLite();
      func(p);
      // login: connect
      // sendMessage: writeText
      // sendPicture: drawShape
      break;
    case 1:
      p = new MessagerPerfect();
      func(p);
      // playSound -> login: connect
      // playSound -> sendMessage: writeText
      // playSound -> sendPicture: drawShape
      break;
    default:
      return -1;
  }
  delete p;

  return 0;
}