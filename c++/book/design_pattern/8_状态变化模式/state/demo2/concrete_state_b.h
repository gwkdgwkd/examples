#ifndef CONCRETE_STATE_B_H
#define CONCRETE_STATE_B_H

#include "state.h"

class ConcreteStateB : public State {
 public:
  ConcreteStateB() = default;
  ~ConcreteStateB() = default;
  void Handle(Context* con);
};
#endif  // CONCRETE_STATE_B_H