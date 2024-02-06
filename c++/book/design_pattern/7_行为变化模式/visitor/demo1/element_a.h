#ifndef ELEMENT_A_H
#define ELEMENT_A_H

#include "element.h"
#include "visitor.h"

class ElementA : public Element {
 public:
  void accept(Visitor& visitor) override {
    visitor.visitElementA(*this);  // 第二次多态辨析
  }
};
#endif  // ELEMENT_A_H