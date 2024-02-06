#ifndef ELEMENT_B_H
#define ELEMENT_B_H

#include "element.h"
#include "visitor.h"

class ElementB : public Element {
 public:
  void accept(Visitor& visitor) override {
    visitor.visitElementB(*this);  // 第二次多态辨析
  }
};
#endif  // ELEMENT_B_H