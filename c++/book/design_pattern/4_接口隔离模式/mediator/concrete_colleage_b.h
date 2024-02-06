#ifndef CONCRETE_COLLEAGE_B_H
#define CONCRETE_COLLEAGE_B_H

#include <iostream>

#include "colleage.h"

class ConcreteColleageB : public Colleage {
 public:
  ConcreteColleageB() = default;
  ConcreteColleageB(Mediator* mdt) : Colleage(mdt) {}
  ~ConcreteColleageB() = default;
  void Aciton() {
    mdt_->DoActionFromBtoA();
    std::cout << "State of ConcreteColleageB: " << this->GetState()
              << std::endl;
  }
  void SetState(const std::string& sdt) { sdt_ = sdt; }
  std::string GetState() { return sdt_; }

 private:
  std::string sdt_;
};
#endif  // CONCRETE_COLLEAGE_B_H