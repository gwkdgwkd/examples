#ifndef FACADE_H
#define FACADE_H

#include <iostream>

#include "subsystem1.h"
#include "subsystem2.h"

class Facade {
 public:
  Facade() {
    this->subs1_ = new Subsystem1();
    this->subs2_ = new Subsystem2();
  }
  ~Facade() {
    delete subs1_;
    delete subs2_;
  }
  void OperationWrapper() {
    this->subs1_->Operation();
    this->subs2_->Operation();
  }

 private:
  Subsystem1* subs1_;
  Subsystem2* subs2_;
};
#endif  // FACADE_H