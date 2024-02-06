#include <iostream>

#include "concrete_state_a.h"
#include "concrete_state_b.h"

void ConcreteStateB::Handle(Context* con) {
  con->OperationForStateB();
  std::cout << ":: State change from B to A" << std::endl;
  State::ChangeState(con, new ConcreteStateA);
}

