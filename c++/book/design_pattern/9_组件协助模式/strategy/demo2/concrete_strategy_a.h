#ifndef CONCRETE_STRATEGY_A_H
#define CONCRETE_STRATEGY_A_H

#include <iostream>

#include "strategy.h"

class ConcreteStrategyA : public Strategy {
 public:
  void AlgrithmInterface() {
    std::cout << "ConcreteStrategyA::AlgrithmInterface" << std::endl;
  }
};
#endif  // CONCRETE_STRATEGY_A_H