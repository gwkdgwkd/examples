#include <iostream>

#include "concrete_element_a.h"
#include "concrete_element_b.h"
#include "concrete_visitor_a.h"
#include "concrete_visitor_b.h"

// 在面向对象系统的开发和设计过程，经常会遇到一种情况就是需求变更，
// 经常做好的一个设计、实现了一个系统原型，咱们的客户又会有了新的需求。
// 因此不得不去修改已有的设计：
// 最常见就是解决方案就是给已经设计、实现好的类添加新的方法去实现客户新的需求，
// 这样就陷入了设计变更的梦魇：
// 不停地打补丁，其带来的后果就是设计根本就不可能封闭、编译永远都是整个系统代码。
// Visitor模式则提供了一种解决方案：
// 将更新（变更）封装到一个类中（访问操作），并由待更改类提供一个接收接口，则可达到效果。

// Visitor模式在不破坏类的前提下，为类提供增加新的新操作。
// C++语言支持的是单分派，Visitor模式的关键是双分派（Double-Dispatch）的技术。
// 在Visitor模式中Accept操作是一个双分派的操作。
// 具体调用哪一个具体的Accept操作，有两个决定因素：
// 1.Element的类型，因为Accept是多态的操作，
//   需要具体的Element类型的子类才可以决定到底调用哪一个Accept实现；
// 2.Visitor的类型，Accept操作有一个参数Visitor* vis，
//   要决定了实际传进来的Visitor的实际类别才可以决定具体是调用哪个VisitConcrete实现。

// 单分派的含义比较好理解，单分派就是说我们在选择一个方法的时候，
// 仅仅需要根据消息接收者的运行时型别，实际上这也就是经常提到的多态的概念。
// 双分派则在选择一个方法的时候，不仅仅要根据消息接收者的运行时型别，
// 还要根据参数的运行时型别，当然如果所有参数都考虑的话就是多分派。
// Visitor模式就是提供了一种多分派中的双分派的实现方式。

// 遗憾的是，当前的主流面向对象程序设计语言（C++/Java/C#等）都并不支持双分派（多分派），
// 仅仅支持单分派，为了支持双分派（多分派），
// 一个权宜的方法就是借助RTTI和if语言来人工确定一个对象的运行时型别，
// 并使用向下类型转换（downcast）来实现。
// 一个常见的例子就是，取得对象的RTTI信息，然后if对象是某个具体类，
// 则执行一部分操作，else属于另外的类则执行另外的操作。
// 然而，RTTI一是占用较多的时间和空间，并且不是很安全，可能在downcast中出现exception。

// 因此，Visitor模式实际上提供了对于支持单分派语言的双分派策略。

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 3]" << std::endl;
    return 0;
  }
  int type = argv[1][0] - '0';

  Visitor* vis;
  Element* elm;
  switch (type) {
    case 0:
      vis = new ConcreteVisitorA();
      elm = new ConcreteElementA();
      elm->Accept(vis);
      // ConcreteVisitorA will visit ConcreteElementA...
      // visiting ConcreteElementA...
      break;
    case 1:
      vis = new ConcreteVisitorA();
      elm = new ConcreteElementB();
      elm->Accept(vis);
      // ConcreteVisitorA will visit ConcreteElementB...
      // visiting ConcreteElementB...
      break;
    case 2:
      vis = new ConcreteVisitorB();
      elm = new ConcreteElementA();
      elm->Accept(vis);
      // ConcreteVisitorB will visit ConcreteElementA...
      // visiting ConcreteElementA...
      break;
    case 3:
      vis = new ConcreteVisitorB();
      elm = new ConcreteElementB();
      elm->Accept(vis);
      // ConcreteVisitorB will visit ConcreteElementB...
      // visiting ConcreteElementB...
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}