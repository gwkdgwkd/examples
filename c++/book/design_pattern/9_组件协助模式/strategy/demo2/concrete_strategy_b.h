#ifndef CONCRETE_STRATEGY_B_H
#define CONCRETE_STRATEGY_B_H

#include <iostream>

#include "strategy.h"

class ConcreteStrategyB : public Strategy {
 public:
  void AlgrithmInterface() {
    std::cout << "ConcreteStrategyB::AlgrithmInterface" << std::endl;
  }
};
#endif  // CONCRETE_STRATEGY_B_H