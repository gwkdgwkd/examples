#ifndef MESSAGER_PERFECT_H
#define MESSAGER_PERFECT_H

#include "messager.h"

class MessagerPerfect : public Messager {
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

 private:
  void playSound() { std::cout << "playSound -> "; }
  void drawShape() { std::cout << "drawShape" << std::endl; }
  void writeText() { std::cout << "writeText" << std::endl; }
  void connect() { std::cout << "connect" << std::endl; }
};

#endif  // MESSAGER_PERFECT_H