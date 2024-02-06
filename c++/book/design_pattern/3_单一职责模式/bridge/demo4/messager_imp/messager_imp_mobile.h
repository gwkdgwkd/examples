#ifndef MESSAGER_IMP_MOBILE_H
#define MESSAGER_IMP_MOBILE_H

#include "messager_imp.h"

class MessagerImpMobile : public MessagerImp {
 protected:
  void playSound() { std::cout << "mobile playSound -> "; }
  void drawShape() { std::cout << "mobile drawShape" << std::endl; }
  void writeText() { std::cout << "mobile writeText" << std::endl; }
  void connect() { std::cout << "mobile connect" << std::endl; }
};

#endif  // MESSAGER_IMP_MOBILE_H