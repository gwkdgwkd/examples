#ifndef MESSAGER_H
#define MESSAGER_H

#include <iostream>

class Messager {
 public:
  virtual void login() = 0;
  virtual void sendMessage() = 0;
  virtual void sendPicture() = 0;
  virtual ~Messager() {}

 protected:
  virtual void playSound() = 0;
  virtual void drawShape() = 0;
  virtual void writeText() = 0;
  virtual void connect() = 0;
};

#endif  // MESSAGER_H