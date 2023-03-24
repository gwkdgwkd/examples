#ifndef MOCHA_H
#define MOCHA_H
#include "condiment_decorator.h"

// 具体装饰者类，装饰者类要实现抽象装饰者类中定义的方法，另外可以加一些新的方法。
class Mocha : public CondimentDecorator {
 public:
  Mocha(Beverage* beverage) : CondimentDecorator(beverage) {}
  std::string getDescription() {
    return m_beverage->getDescription() + " Mocha";
  }
  double cost() { return 0.2 + m_beverage->cost(); }
};
#endif