#ifndef CONDIMENT_BEVERAGE_H
#define CONDIMENT_BEVERAGE_H
#include "../beverage/beverage.h"

// 抽象装饰者类是具体装饰者的基类，装饰者类中包含抽象组件的指针，是为了记录被装饰的对象。
// 当需要获得装饰之后的行为时可以通过该指针获得被装饰者的行为加上装饰者自身的行为。

// 装饰者和被装饰者必须是一样地类型，也就是有共同的基类，
// 这是相当关键的地方，这是因为装饰者必须能取代被装饰者。

// 抽象装饰者类，表示调料，继承自饮料类
class CondimentDecorator : public Beverage {
 public:
  CondimentDecorator(Beverage* berverge) : m_beverage(berverge) {}
  // 定义成纯虚函数，是为了强制子类实例化时必须实现它：
  virtual std::string getDescription() = 0;

 protected:
  Beverage* m_beverage;
};
#endif