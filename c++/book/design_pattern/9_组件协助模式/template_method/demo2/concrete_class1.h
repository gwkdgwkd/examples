#ifndef CONCRETE_CLASS1_H
#define CONCRETE_CLASS1_H

#include <iostream>

#include "template.h"

class ConcreteClass1 : public AbstractClass {
 public:
  ConcreteClass1() = default;
  ~ConcreteClass1() = default;

 protected:
  void PrimitiveOperation1() {
    std::cout << "ConcreteClass1::PrimitiveOperation1" << std::endl;
  }
  void PrimitiveOperation2() {
    std::cout << "ConcreteClass1::PrimitiveOperation2" << std::endl;
  }
};
#endif  // CONCRETE_CLASS1_H