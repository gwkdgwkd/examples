#ifndef DARKROAST_H
#define DARKROAST_H
#include "beverage.h"

// 具体组件类，继承自抽象组件类，是要动态地加上新行为（即要装饰）的对象。
class DarkRoast : public Beverage {
 public:
  DarkRoast() { m_description = "DarkRoast"; }
  double cost() { return 2.99; }
};
#endif