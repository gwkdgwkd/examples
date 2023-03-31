#include "messager.h"

// 客户需求演化：
// 1.提供了一个需求，需要实现消息类；

int main() {
  Messager msg;
  msg.login();
  msg.sendMessage();
  msg.sendPicture();

  // login: connect
  // sendMessage: writeText
  // sendPicture: drawShape
}