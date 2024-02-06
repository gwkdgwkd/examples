#ifndef ESPRESSO_H
#define ESPRESSO_H
#include "beverage.h"

// 具体组件类，继承自抽象组件类，是要动态地加上新行为（即要装饰）的对象。
class Espresso : public Beverage {
 public:
  Espresso() { m_description = "Espresso"; }
  double cost() { return 1.99; }
};
#endif