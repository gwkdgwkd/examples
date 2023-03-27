#ifndef CONCRETE_OBSERVER_1_H
#define CONCRETE_OBSERVER_1_H
#include <iostream>

#include "../subject/subject.h"
#include "observer.h"

// 具体观察者：
// 维护一个指向ConcreteSubject对象的引用；
// 存储有关状态，这些状态应与目标的状态保持一致；
// 实现Observer的更新接口以使自身状态与目标的状态保持一致。
class ConcreteObserver1 : public Observer {
 public:
  ConcreteObserver1(Subject *pSubject) : m_pSubject(pSubject) {}

  void Update(int value) {
    std::cout << "ConcreteObserver1 get the update. New State:" << value
              << std::endl;
  }

 private:
  Subject *m_pSubject;
};
#endif