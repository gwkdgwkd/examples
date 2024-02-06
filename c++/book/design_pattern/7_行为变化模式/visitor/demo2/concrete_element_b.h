#ifndef ELEMENT_B_H
#define ELEMENT_B_H

#include <iostream>

#include "element.h"
#include "visitor.h"

class ConcreteElementB : public Element {
 public:
  ConcreteElementB() = default;
  ~ConcreteElementB() = default;
  void Accept(Visitor* vis) {
    vis->VisitConcreteElementB(this);
    std::cout << "visiting ConcreteElementB..." << std::endl;
  }
};
#endif  // ELEMENT_B_H