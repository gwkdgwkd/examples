#ifndef MESSAGER_MOBILE_H
#define MESSAGER_MOBILE_H

#include "messager.h"

class MessagerMobile : public Messager {
 protected:
  void playSound() { std::cout << "mobile playSound ->"; }
  void drawShape() { std::cout << "mobile drawShape" << std::endl; }
  void writeText() { std::cout << "mobile writeText" << std::endl; }
  void connect() { std::cout << "mobile connect" << std::endl; }
};

#endif  // MESSAGER_MOBILE_H