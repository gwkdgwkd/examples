#ifndef MESSAGER_PC_PERFECT_H
#define MESSAGER_PC_PERFECT_H

#include "messager_pc.h"

class MessagerPCPerfect : public MessagerPC {
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

#endif  // MESSAGER_PC_PERFECT_H