#ifndef CHINA_BODY_H
#define CHINA_BODY_H

#include "body.h"

class ChinaBody : public Body {
 public:
  void showBody() { std::cout << "China Body" << std::endl; }
};
#endif