#ifndef CONCRETE_VISITOR_B_H
#define CONCRETE_VISITOR_B_H

#include <iostream>

#include "element.h"
#include "visitor.h"

class ConcreteVisitorB : public Visitor {
 public:
  ConcreteVisitorB() = default;
  virtual ~ConcreteVisitorB() = default;
  virtual void VisitConcreteElementA(Element* elm) {
    std::cout << "ConcreteVisitorB will visit ConcreteElementA..." << std::endl;
  }
  virtual void VisitConcreteElementB(Element* elm) {
    std::cout << "ConcreteVisitorB will visit ConcreteElementB..." << std::endl;
  }
};
#endif  // CONCRETE_VISITOR_B_H