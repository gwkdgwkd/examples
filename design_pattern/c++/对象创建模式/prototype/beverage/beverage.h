#ifndef BEVERAGE_H
#define BEVERAGE_H
#include <string>

// 抽象组件类，即被装饰的类，每个组件都可以单独使用，或者被装饰者包起来使用。
// 该类中声明了一些接口，这些接口将在具体组件，以及具体装饰者中实现。

// 抽象组件类，表示饮料
class Beverage {
 public:
  Beverage() : m_description("Unknown Beverage") {}
  virtual std::string getDescription() { return m_description; }
  virtual double cost() = 0;

  BEVERAGE_H
  std::string m_description;
};
#endif