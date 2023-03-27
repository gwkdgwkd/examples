#ifndef CONCRETE_SUBJECT_H
#define CONCRETE_SUBJECT_H
#include <list>

#include "subject.h"

// 具体目标将有关状态存入各ConcreteObserver对象，当它的状态发生改变时，向它的各个观察者发出通知。
class ConcreteSubject : public Subject {
 public:
  void Attach(Observer *pObserver) { m_ObserverList.push_back(pObserver); }
  void Detach(Observer *pObserver) { m_ObserverList.remove(pObserver); }
  void Notify() {
    std::list<Observer *>::iterator it = m_ObserverList.begin();
    while (it != m_ObserverList.end()) {
      (*it)->Update(m_iState);
      ++it;
    }
  }

  void SetState(int state) { m_iState = state; }

 private:
  std::list<Observer *> m_ObserverList;
  int m_iState;
};
#endif