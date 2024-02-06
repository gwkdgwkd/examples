#ifndef CONCRETE_OBSERVER_2_H
#define CONCRETE_OBSERVER_2_H
#include <iostream>

#include "../subject/subject.h"
#include "observer.h"

class ConcreteObserver2 : public Observer {
 public:
  ConcreteObserver2(Subject *pSubject) : m_pSubject(pSubject) {}

  void Update(int value) {
    std::cout << "ConcreteObserver2 get the update. New State:" << value
              << std::endl;
  }

 private:
  Subject *m_pSubject;
};
#endif