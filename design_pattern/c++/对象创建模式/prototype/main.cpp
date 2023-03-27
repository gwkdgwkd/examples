#include <iostream>

#include "beverage/darkroast.h"
#include "beverage/espresso.h"
#include "beverage/houseblend.h"
#include "decorator/milk.h"
#include "decorator/mocha.h"

// Decorator 装饰模式

// 时常会遇到这样一种情况，已经设计好了一个接口，并且也有几个实现类，
// 但这时发现设计的时候疏忽了，忘记了一些功能，或者后来需求变动要求加入一些功能，
// 最简单的做法就是修改接口，添加函数，然后继承类中都相应添加实现，这样做倒也没什么问题，
// 但是如果这种变化会经常出现或者继承类非常多，工作量将会大大增加。

// 装饰者模式，在不修改现有的接口和实现类的基础上实现功能或者状态的添加，
// 动态地给一个对象添加一些额外的职责，装饰模式一般针对接口或者抽象类的变化。
// 若要扩展功能，装饰者模式提供了比继承更有弹性的替代方案。
// 装饰者模式遵循的设计原则：类应该对扩展开放，对修改关闭。

// 装饰者模式提供了更加灵活的向对象添加职责的方式，可以用添加和分离的方法，
// 用装饰在运行时刻增加和删除职责，装饰者模式提供了一种即用即付的方法来添加职责。
// 它并不试图在一个复杂的可定制的类中支持所有的可预见的特征，相反，可以定义一个简单的类，
// 并且用装饰类给它逐渐的添加功能，可以从简单的部件组合出复杂的功能。
// 但是同时装饰者模式会使复杂性增加，且导致许多小类的产生。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';
  switch (type) {
    case 0: {
      // 不加调料的Espresso：
      Beverage* beverage = new Espresso();
      std::cout << beverage->getDescription() << " ￥" << beverage->cost()
                << std::endl;
      // Espresso ￥1.99
      break;
    }
    case 1: {
      // 加双倍摩卡和奶泡的DarkRoast：
      Beverage* beverage = new DarkRoast();
      beverage = new Mocha(beverage);
      beverage = new Mocha(beverage);
      beverage = new Milk(beverage);
      std::cout << beverage->getDescription() << " ￥" << beverage->cost()
                << std::endl;
      // DarkRoast Mocha Mocha Milk ￥3.89
      break;
    }
    case 2: {
      // 加三倍奶泡的和一份摩卡的HouseBlend：
      Beverage* beverage = new HouseBlend();
      beverage = new Mocha(beverage);
      beverage = new Milk(beverage);
      beverage = new Milk(beverage);
      beverage = new Milk(beverage);
      std::cout << beverage->getDescription() << " ￥" << beverage->cost()
                << std::endl;
      // HouseBlend Mocha Milk Milk Milk ￥2.59
      break;
    }
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}