#include <iostream>

#include "facade.h"

// facade 门面模式

// 动机（Motivation）：
// 1.客户和组件中各个复杂的子系统有了过多的耦合，
//   随着外部客户程序和各个子系统的演化，这种过多的耦合面临很多变化的挑战；
// 2.如何简化外部客户程序和系统间的交互接口？
//   如何将外部客户程序的演化和内部子系统的变化之间的依赖相互解耦？

// 定义：
// 为子系统中的一组接口提供一个一致（稳定）的界面，
// Facade模式定义了一个高层接口，这个接口使得这一子系统更加容易使用（复用）。

// 要点总结：
// 1.从客户程序的角度来看，Facade模式简化了整个组件系统的接口，
//   对于组件内部与外部客户程序来说，达到了一种解耦的效果，
//   内部子系统的任何变化不会影响到Facade接口的变化；
// 2.Facade设计模式更注重从架构的层次去看整个系统，而不是单个类的层次，
//   Facade很多时候更是一种架构设计模式；
// 3.Facade设计模式并非一个集装箱，可以任意地放进任何多个对象，
//   Facade模式中组件的内部应该是相互耦合关系比较大的一系列组件，
//   而不是一个简单的功能集合。

// Facade模式的想法、思路和实现都非常简单，但是其思想却是非常有意义的。
// 并且Facade设计模式在实际的开发设计中也是应用最广、最多的模式之一。

// Facade模式在高层提供了一个统一的接口，解耦了系统。
// 设计模式中还有另一种模式Mediator也和Facade有类似的地方。
// 但是Mediator主要目的是对象间的访问的解耦（通讯时候的协议）。

int main() {
  Facade* f = new Facade();
  f->OperationWrapper();
  // Subsystem1::operation
  // Subsystem2::operation

  return 0;
}