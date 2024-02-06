#ifndef MESSAGER_MOBILE_LITE_H
#define MESSAGER_MOBILE_LITE_H

#include "messager_mobile.h"

class MessagerMobileLite : public MessagerMobile {
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

#endif  // MESSAGER_MOBILE_LITE_H