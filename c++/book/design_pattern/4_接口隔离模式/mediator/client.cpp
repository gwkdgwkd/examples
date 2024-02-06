#include <iostream>

#include "concrete_colleage_a.h"
#include "concrete_colleage_b.h"
#include "concrete_mediator.h"

// Mediator 中介者模式

// 动机（Motivation）：
// 1.在软件构建过程中，经常会出现多个对象相互关联交互的情况，
//   对象之间常常会维持一种复杂的引用关系，如果遇到一些需求的更改，
//   这种直接的引用关系将面临不断的变化；
// 2.在这种情况下，可使用一个中介对象来管理对象间的关联关系，
//   避免相互交互的对象之间的紧耦合引用关系，从而更好地抵御变化。

// 定义：
// 用一个中介对象来封装（封装变化）一系列对象的交互。
// 中介者使各对象不需要显式的相互引用（编译时依赖->运行时依赖），
// 从而使其耦合松散（管理变化），而且独立地改变它们之间的交互。

// 要点总结：
// 1.将多个对象间复杂的关联关系解耦，Mediator模式将多个对象间的控制逻辑进行集中管理，
//   将多个对象互相关联变为多个对象和一个中介者关联，简化了系统的维护，抵御了可能变化；
// 2.随着控制逻辑的复杂化，Mediator具体对象的实现可能相当复杂，
//   这时候可以对Mediator对象进行分解处理；
// 3.Facade模式是解耦系统间（单向）的对象关联关系，
//   Mediator模式是解耦系统内各个对象（双向）的关联关系。

// Mediator模式是一种很有用并且很常用的模式，它通过将对象间的通信封装到一个类中，
// 将多对多的通信转化为一对多的通信，降低了系统的复杂性。
// Mediator还获得系统解耦的特性，通过Mediator，
// 各个Colleague就不必维护各自通信的对象和通信协议，降低了系统的耦合性，
// Mediator和各个Colleague就可以相互独立地修改了。
// Mediator模式还有一个很显著额特点就是将控制集中，集中的优点就是便于管理，
// 也正式符合了OO设计中的每个类的职责要单一和集中的原则。

int main() {
  ConcreteMediator* m = new ConcreteMediator();
  ConcreteColleageA* c1 = new ConcreteColleageA(m);
  ConcreteColleageB* c2 = new ConcreteColleageB(m);

  m->IntroColleage(c1, c2);

  // 将ConcreteColleageA对象设置状态old，ConcreteColleageB也设置状态old：
  c1->SetState("old");
  c2->SetState("old");
  c1->Aciton();  // State of ConcreteColleageA: old
  c2->Aciton();  // State of ConcreteColleageB: old

  // ConcreteColleageA对象改变状态，并在Action中和ConcreteColleageB对象进行通信，
  // 并改变ConcreteColleageB对象的状态为new：
  c1->SetState("new");
  c1->Aciton();  // State of ConcreteColleageA: new
  c2->Aciton();  // State of ConcreteColleageB: new

  // ConcreteColleageB对象改变状态，并在Action中和ConcreteColleageA对象进行通信，
  // 并改变ConcreteColleageA对象的状态为old：
  c2->SetState("old");
  c2->Aciton();  // State of ConcreteColleageB: old
  c1->Aciton();  // State of ConcreteColleageA: old

  // Mediator模式的实现关键就是将对象Colleague之间的通信封装到一个类种单独处理，
  // 两个Colleague对象并不知道它交互的对象，并且也不是显示地处理交互过程，
  // 这一切都是通过Mediator对象完成的。

  return 0;
}