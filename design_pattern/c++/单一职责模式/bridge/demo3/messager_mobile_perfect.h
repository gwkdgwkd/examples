#ifndef MESSAGER_MOBILE_PERFECT_H
#define MESSAGER_MOBILE_PERFECT_H

#include "messager_mobile.h"

class MessagerMobilePerfect : public MessagerMobile {
 public:
  void login() {
    playSound();
    std::cout << "login: ";
    connect();
  }
  void sendMessage() {
    playSound();
    std::cout << "sendMessage: ";
    writeText();
  }
  void sendPicture() {
    playSound();
    std::cout << "sendPicture: ";
    drawShape();
  }
};

#endif  // MESSAGER_MOBILE_PERFECT_H