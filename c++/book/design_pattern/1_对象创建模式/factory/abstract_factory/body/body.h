#ifndef BODY_H
#define BODY_H

#include <iostream>

class Body {
 public:
  virtual void showBody() = 0;
  virtual ~Body() {}
};
#endif