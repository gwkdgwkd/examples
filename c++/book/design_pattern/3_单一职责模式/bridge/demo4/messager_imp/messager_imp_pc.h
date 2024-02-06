#ifndef MESSAGER_IMP_PC_H
#define MESSAGER_IMP_PC_H

#include "messager_imp.h"

class MessagerImpPC : public MessagerImp {
 protected:
  void playSound() { std::cout << "pc playSound -> "; }
  void drawShape() { std::cout << "pc drawShape" << std::endl; }
  void writeText() { std::cout << "pc writeText" << std::endl; }
  void connect() { std::cout << "pc connect" << std::endl; }
};

#endif  // MESSAGER_PC_H