#ifndef ELEMENT_H
#define ELEMENT_H

#include "visitor.h"

class Element {
 public:
  virtual ~Element() = default;
  virtual void Accept(Visitor* vis) = 0;

 protected:
  Element() = default;
};
#endif  // ELEMENT_H