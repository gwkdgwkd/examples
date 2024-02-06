#ifndef VISITOR1_H
#define VISITOR1_H

#include <iostream>

#include "visitor.h"

// 扩展1
class Visitor1 : public Visitor {
 public:
  void visitElementA(ElementA& element) override {
    std::cout << "Visitor1 is processing ElementA" << std::endl;
  }

  void visitElementB(ElementB& element) override {
    std::cout << "Visitor1 is processing ElementB" << std::endl;
  }
};
#endif  // VISITOR1_H