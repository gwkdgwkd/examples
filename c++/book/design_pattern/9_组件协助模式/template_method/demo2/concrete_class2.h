#ifndef CONCRETE_CLASS2_H
#define CONCRETE_CLASS2_H

#include <iostream>

#include "template.h"

class ConcreteClass2 : public AbstractClass {
 public:
  ConcreteClass2() = default;
  ~ConcreteClass2() = default;

 protected:
  void PrimitiveOperation1() {
    std::cout << "ConcreteClass2::PrimitiveOperation1" << std::endl;
  }
  void PrimitiveOperation2() {
    std::cout << "ConcreteClass2::PrimitiveOperation2" << std::endl;
  }
};
#endif  // CONCRETE_CLASS2_H