#ifndef HOUSEBLEND_H
#define HOUSEBLEND_H
#include "beverage.h"

// 具体组件类，继承自抽象组件类，是要动态地加上新行为（即要装饰）的对象。
class HouseBlend : public Beverage {
 public:
  HouseBlend() { m_description = "HouseBlend"; }
  double cost() { return 0.89; }
};
#endif