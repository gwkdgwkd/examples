#ifndef CONCRETE_VISITOR_A_H
#define CONCRETE_VISITOR_A_H

#include <iostream>

#include "element.h"
#include "visitor.h"

class ConcreteVisitorA : public Visitor {
 public:
  ConcreteVisitorA() = default;
  virtual ~ConcreteVisitorA() = default;
  virtual void VisitConcreteElementA(Element* elm) {
    std::cout << "ConcreteVisitorA will visit ConcreteElementA..." << std::endl;
  }
  virtual void VisitConcreteElementB(Element* elm) {
    std::cout << "ConcreteVisitorA will visit ConcreteElementB..." << std::endl;
  }
};
#endif  // CONCRETE_VISITOR_A_H