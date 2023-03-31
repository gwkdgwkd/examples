#ifndef MESSAGER_PC_LITE_H
#define MESSAGER_PC_LITE_H

#include "messager_pc.h"

class MessagerPCLite : public MessagerPC {
 public:
  void login() {
    std::cout << "login: ";
    connect();
  }
  void sendMessage() {
    std::cout << "sendMessage: ";
    writeText();
  }
  void sendPicture() {
    std::cout << "sendPicture: ";
    drawShape();
  }
};

#endif  // MESSAGER_PC_LITE_H