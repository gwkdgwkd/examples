#ifndef CONCRETE_MEDIATOR_H
#define CONCRETE_MEDIATOR_H

#include "colleage.h"
#include "mediator.h"

class ConcreteMediator : public Mediator {
 public:
  ConcreteMediator() = default;
  ConcreteMediator(Colleage* clgA, Colleage* clgB) {
    this->clgA_ = clgA;
    this->clgB_ = clgB;
  }
  ~ConcreteMediator() = default;
  void SetConcreteColleageA(Colleage* clgA) { this->clgA_ = clgA; }
  void SetConcreteColleageB(Colleage* clgB) { this->clgB_ = clgB; }
  Colleage* GetConcreteColleageA() { return clgA_; }
  Colleage* GetConcreteColleageB() { return clgB_; }
  void IntroColleage(Colleage* clgA, Colleage* clgB) {
    this->clgA_ = clgA;
    this->clgB_ = clgB;
  }
  void DoActionFromAtoB() { clgB_->SetState(clgA_->GetState()); }
  void DoActionFromBtoA() { clgA_->SetState(clgB_->GetState()); }

 private:
  Colleage* clgA_;
  Colleage* clgB_;
};
#endif  // CONCRETE_MEDIATOR_H