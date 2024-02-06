#ifndef STATE_H
#define STATE_H

#include "context.h"

class State {
 public:
  State() = default;
  virtual ~State() = default;
  virtual void Handle(Context* con) = 0;
  void ChangeState(Context* con, State* st);
};
#endif  // STATE_H