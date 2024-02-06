#ifndef VISITOR2_H
#define VISITOR2_H

#include <iostream>

#include "visitor.h"

// 扩展2
class Visitor2 : public Visitor {
 public:
  void visitElementA(ElementA& element) override {
    std::cout << "Visitor2 is processing ElementA" << std::endl;
  }

  void visitElementB(ElementB& element) override {
    std::cout << "Visitor2 is processing ElementB" << std::endl;
  }
};
#endif  // VISITOR2_H