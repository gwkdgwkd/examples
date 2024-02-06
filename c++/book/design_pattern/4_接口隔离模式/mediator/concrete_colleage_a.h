#ifndef CONCRETE_COLLEAGE_A_H
#define CONCRETE_COLLEAGE_A_H

#include <iostream>

#include "colleage.h"

class ConcreteColleageA : public Colleage {
 public:
  ConcreteColleageA() = default;
  ConcreteColleageA(Mediator* mdt) : Colleage(mdt) {}
  ~ConcreteColleageA() = default;
  void Aciton() {
    mdt_->DoActionFromAtoB();
    std::cout << "State of ConcreteColleageA: " << this->GetState()
              << std::endl;
  }
  void SetState(const std::string& sdt) { sdt_ = sdt; }
  std::string GetState() { return sdt_; }

 private:
  std::string sdt_;
};
#endif  // CONCRETE_COLLEAGE_A_H