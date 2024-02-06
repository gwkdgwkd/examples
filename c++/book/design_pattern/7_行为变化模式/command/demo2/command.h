#ifndef COMMAND_H
#define COMMAND_H

#include "reciever.h"

class Command {
 public:
  virtual ~Command() = default;
  virtual void Excute() = 0;

 protected:
  Command() {}
};
#endif  // COMMAND_H