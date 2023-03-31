#ifndef MESSAGER_H
#define MESSAGER_H

#include <iostream>

#include "../messager_imp/messager_imp.h"

class Messager {
 public:
  Messager(MessagerImp *p) : imp(p) {}
  virtual void login() = 0;
  virtual void sendMessage() = 0;
  virtual void sendPicture() = 0;
  virtual ~Messager() {}

 protected:
  MessagerImp *imp;
};

#endif  // MESSAGER_H