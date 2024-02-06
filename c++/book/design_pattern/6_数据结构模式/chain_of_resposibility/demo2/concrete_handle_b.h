#ifndef CONCRETE_HANDLE_B_H
#define CONCRETE_HANDLE_B_H

#include "handle.h"

class ConcreteHandleB : public Handle {
 public:
  ConcreteHandleB() = default;
  ~ConcreteHandleB() = default;
  ConcreteHandleB(Handle* succ) : Handle(succ) {}
  void HandleRequest() {
    if (this->GetSuccessor() != nullptr) {
      this->GetSuccessor()->HandleRequest();
    } else {
      std::cout << "ConcreteHandleB::HandleRequest" << std::endl;
    }
  }
};
#endif  // CONCRETE_HANDLE_B_H