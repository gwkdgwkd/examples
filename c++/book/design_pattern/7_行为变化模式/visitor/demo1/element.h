#ifndef ELEMENT_H
#define ELEMENT_H

#include "visitor.h"

class Element {
 public:
  virtual void accept(Visitor& visitor) = 0;  // 第一次多态辨析
  virtual ~Element() {}
};
#endif  // ELEMENT_H