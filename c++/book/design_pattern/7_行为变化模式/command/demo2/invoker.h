#ifndef INVOKER_H
#define INVOKER_H

#include <iostream>

#include "command.h"

class Invoker {
 public:
  Invoker(Command* cmd) : cmd_(cmd) {}
  ~Invoker() { delete cmd_; }
  void Invoke() { cmd_->Excute(); }

 private:
  Command* cmd_;
};
#endif  // INVOKER_H