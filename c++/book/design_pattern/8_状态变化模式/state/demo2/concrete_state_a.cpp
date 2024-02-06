#include <iostream>

#include "concrete_state_a.h"
#include "concrete_state_b.h"

void ConcreteStateA::Handle(Context* con) {
  con->OperationForStateA();
  std::cout << ":: State change from A to B" << std::endl;
  State::ChangeState(con, new ConcreteStateB);
}
