#ifndef SUBJECT_H
#define SUBJECT_H
#include "../observer/observer.h"

// 目标知道它的观察者，可以有任意多个观察者观察同一个目标，提供注册和删除观察者对象的接口。
class Subject {
 public:
  virtual void Attach(Observer *) = 0;
  virtual void Detach(Observer *) = 0;
  virtual void Notify() = 0;
};
#endif