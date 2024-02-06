#ifndef AMERICA_BODY_H
#define AMERICA_BODY_H

#include "body.h"

class AmericaBody : public Body {
 public:
  void showBody() { std::cout << "America Body" << std::endl; }
};
#endif