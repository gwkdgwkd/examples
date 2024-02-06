#ifndef CONCRETE_COMMAND_H
#define CONCRETE_COMMAND_H

#include <iostream>

#include "command.h"

class ConcreteCommand : public Command {
 public:
  ConcreteCommand(Reciever* rev) : rev_(rev) {}
  ~ConcreteCommand() = default;
  void Excute() {
    rev_->Action();
    std::cout << "ConcreteCommand..." << std::endl;
  }

 private:
  Reciever* rev_;
};
#endif  // CONCRETE_COMMAND_H