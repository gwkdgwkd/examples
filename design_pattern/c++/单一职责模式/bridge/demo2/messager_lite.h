#ifndef MESSAGER_LITE_H
#define MESSAGER_LITE_H

#include "messager.h"

class MessagerLite : public Messager {
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

 private:
  void playSound() { std::cout << "playSound" << std::endl; }
  void drawShape() { std::cout << "drawShape" << std::endl; }
  void writeText() { std::cout << "writeText" << std::endl; }
  void connect() { std::cout << "connect" << std::endl; }
};

#endif  // MESSAGER_LITE_H