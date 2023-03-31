#ifndef MESSAGER_LITE_H
#define MESSAGER_LITE_H

#include "messager.h"

class MessagerLite : public Messager {
 public:
 using Messager::Messager;
 
  void login() {
    std::cout << "login: ";
    imp->connect();
  }
  void sendMessage() {
    std::cout << "sendMessage: ";
    imp->writeText();
  }
  void sendPicture() {
    std::cout << "sendPicture: ";
    imp->drawShape();
  }
};

#endif  // MESSAGER_LITE_H