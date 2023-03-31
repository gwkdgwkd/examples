#ifndef MESSAGER_PERFECT_H
#define MESSAGER_PERFECT_H

#include "messager.h"

class MessagerPerfect : public Messager {
 public:
  using Messager::Messager;

  void login() {
    imp->playSound();
    std::cout << "login: ";
    imp->connect();
  }
  void sendMessage() {
    imp->playSound();
    std::cout << "sendMessage: ";
    imp->writeText();
  }
  void sendPicture() {
    imp->playSound();
    std::cout << "sendPicture: ";
    imp->drawShape();
  }
};

#endif  // MESSAGER_PERFECT_H