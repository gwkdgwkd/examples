#ifndef CONCRETE_OBSERVER_3_H
#define CONCRETE_OBSERVER_3_H
#include <iostream>

#include "../subject/subject.h"
#include "observer.h"

class ConcreteObserver3 : public Observer {
 public:
  ConcreteObserver3(Subject *pSubject) : m_pSubject(pSubject) {}

  void Update(int value) {
    std::cout << "ConcreteObserver3 get the update. New State:" << value
              << std::endl;
  }

 private:
  Subject *m_pSubject;
};
#endif