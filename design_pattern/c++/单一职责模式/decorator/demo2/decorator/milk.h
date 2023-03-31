#ifndef MILK_H
#define MILK_H
#include "condiment_decorator.h"

// 具体装饰者类，装饰者类要实现抽象装饰者类中定义的方法，另外可以加一些新的方法。
class Milk : public CondimentDecorator {
 public:
  Milk(Beverage* beverage) : CondimentDecorator(beverage) {}
  std::string getDescription() {
    return m_beverage->getDescription() + " Milk";
  }
  double cost() { return 0.5 + m_beverage->cost(); }
};
#endif