#ifndef MESSAGER_H
#define MESSAGER_H

#include <iostream>

class Messager {
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

#endif  // MESSAGER_H