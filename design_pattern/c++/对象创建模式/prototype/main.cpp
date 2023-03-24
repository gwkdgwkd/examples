#include <iostream>

#include "beverage/darkroast.h"
#include "beverage/espresso.h"
#include "beverage/houseblend.h"
#include "decorator/milk.h"
#include "decorator/mocha.h"

// 装饰者模式动态地将责任附加到对象上。
// 若要扩展功能，装饰者模式提供了比继承更有弹性的替代方案。
// 装饰者模式遵循的设计原则：类应该对扩展开放，对修改关闭。

// Decorator 装饰模式

// 动机（Motivation）：
// 1.在软件系统中，经常面临着某些结构复杂的对象的创建工作；
//   由于需求的变化，这些对象经常面临着剧烈的变化，但是它们却拥有比较稳定一致的接口。
// 2.如何应对这种变化？如何向客户程序（使用这些对象的程序）隔离出这些易变对象，
//   从而使得依赖这些易变对象的客户程序不随需求改变而变化？

// 定义：使用原型实例指定创建对象的种类，然后通过拷贝这些原型来创建新的对象。

// 要点总结：
// 1.Prototype模式同样用于隔离类对象的使用者和具体类型（易变类）之间的耦合关系，
//   它同样要求这些易变类拥有稳定的接口。
// 2.Prototype模式对于如何创建易变类的实体对象采用原型克隆的方法来做，
//   它使得我们可以非常灵活地动态创建拥有某些稳定接口的新对象，
//   所需工作仅仅是注册一个新类的对象（即原型），然后在任何需要的地方Clone。
// 3.Prototype模式中的Clone方法可以利用某些框架中的系列化来实现深拷贝。

int main() {
  // 不加调料的Espresso
  Beverage* beverage = new Espresso();
  std::cout << beverage->getDescription() << " ￥" << beverage->cost()
            << std::endl;

  // 加双倍摩卡和奶泡的DarkRoast
  Beverage* beverage2 = new DarkRoast();
  beverage2 = new Mocha(beverage2);
  beverage2 = new Mocha(beverage2);
  beverage2 = new Milk(beverage2);
  std::cout << beverage2->getDescription() << " ￥" << beverage2->cost()
            << std::endl;

  // 加三倍奶泡的和一份摩卡的HouseBlend
  Beverage* beverage3 = new HouseBlend();
  beverage3 = new Mocha(beverage3);
  beverage3 = new Milk(beverage3);
  beverage3 = new Milk(beverage3);
  beverage3 = new Milk(beverage3);
  std::cout << beverage3->getDescription() << " ￥" << beverage3->cost()
            << std::endl;
  delete beverage;
  delete beverage2;
  delete beverage3;

  // Espresso ￥1.99
  // DarkRoast Mocha Mocha Milk ￥3.89
  // HouseBlend Mocha Milk Milk Milk ￥2.59
}