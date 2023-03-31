#ifndef MESSAGER_IMP_H
#define MESSAGER_IMP_H

#include <iostream>

class MessagerImp {
 public:
  virtual void playSound() = 0;
  virtual void drawShape() = 0;
  virtual void writeText() = 0;
  virtual void connect() = 0;
  virtual ~MessagerImp() {}
};

#endif  // MESSAGER_IMP_H
