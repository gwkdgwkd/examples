#include <iostream>

#include "observer/concrete_observer1.h"
#include "observer/concrete_observer2.h"
#include "observer/concrete_observer3.h"
#include "subject/concrete_subject.h"

// 观察者模式是目前应用最广、影响范围最大的模式之一，是软件开发过程中必须要掌握和使用的：
// 1.Model/View/Control（MVC），在系统开发架构设计中有着很重要的地位和意义，
//   MVC实现了业务逻辑和表示层的解耦，在MFC中，Doc/View提供了实现MVC的框架结构；
// 2.Qt中的核心机制信号和槽的实现也同样有观察者的影子。

// Observer模式要解决的问题为：建立一个一（Subject）对多（Observer）的依赖关系，
// 并且做到当一变化的时候，依赖这个一的多也能够同步改变。
// Observer模式提供给关联对象一种同步通信的手段，使某个对象与依赖它的其他对象之间保持状态同步。

// Observer模式也称为发布-订阅模式，目标就是通知的发布者，观察者则是通知的订阅者（接受通知）。
// 最常见的一个例子就是：对同一组数据进行统计分析时候，希望能够提供多种形式的表示，
// 例如以表格进行统计显示、柱状图统计显示、百分比统计显示等，这些表示都依赖于同一组数据，
// 当然需要当数据改变的时候，所有的统计的显示都能够同时改变，Observer模式就是解决了这一个问题。

int main(int argc, char *argv[]) {
  ConcreteSubject *pSubject = new ConcreteSubject();
  ConcreteObserver1 *pObserver1 = new ConcreteObserver1(pSubject);
  ConcreteObserver2 *pObserver2 = new ConcreteObserver2(pSubject);
  ConcreteObserver3 *pObserver3 = new ConcreteObserver3(pSubject);

  if (argc < 2) {
    std::cout << argv[0] << " i [0 - 1]" << std::endl;
    return 0;
  }
  int state = argv[1][0] - '0';
  switch (state) {
    case 0:
      pSubject->SetState(state);
      pSubject->Attach(pObserver1);
      pSubject->Attach(pObserver2);
      pSubject->Attach(pObserver3);
      pSubject->Notify();
      // ConcreteObserver1 get the update. New State:0
      // ConcreteObserver2 get the update. New State:0
      // ConcreteObserver3 get the update. New State:0

      pSubject->Detach(pObserver1);
      pSubject->Notify();
      // ConcreteObserver2 get the update. New State:0
      // ConcreteObserver3 get the update. New State:0
      break;
    case 1:
      pSubject->SetState(state);
      pSubject->Attach(pObserver2);
      pSubject->Notify();
      // ConcreteObserver2 get the update. New State:1

      pSubject->SetState(state + 10);
      pSubject->Attach(pObserver3);
      pSubject->Notify();
      // ConcreteObserver2 get the update. New State:11
      // ConcreteObserver3 get the update. New State:11
      break;
    default:
      std::cout << "invalid type" << std::endl;
      break;
  }

  return 0;
}