#ifndef CONCRETE_HANDLE_A_H
#define CONCRETE_HANDLE_A_H

#include "handle.h"

class ConcreteHandleA : public Handle {
 public:
  ConcreteHandleA() = default;
  ~ConcreteHandleA() = default;
  ConcreteHandleA(Handle* succ) : Handle(succ) {}
  void HandleRequest() {
    if (this->GetSuccessor() != nullptr) {
      this->GetSuccessor()->HandleRequest();
    } else {
      std::cout << "ConcreteHandleA::HandleRequest" << std::endl;
    }
  }
};
#endif  // CONCRETE_HANDLE_A_H