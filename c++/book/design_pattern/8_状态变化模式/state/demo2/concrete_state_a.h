#ifndef CONCRETE_STATE_A_H
#define CONCRETE_STATE_A_H

#include "state.h"

class ConcreteStateA : public State {
 public:
  ConcreteStateA() = default;
  ~ConcreteStateA() = default;
  void Handle(Context* con);
};
#endif  // CONCRETE_STATE_A_H