#ifndef ELEMENT_A_H
#define ELEMENT_A_H

#include <iostream>

#include "element.h"
#include "visitor.h"

class ConcreteElementA : public Element {
 public:
  ConcreteElementA() = default;
  ~ConcreteElementA() = default;
  void Accept(Visitor* vis) {
    vis->VisitConcreteElementA(this);
    std::cout << "visiting ConcreteElementA..." << std::endl;
  }
};
#endif  // ELEMENT_A_H