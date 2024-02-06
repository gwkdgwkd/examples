#ifndef CONCRETE_COMMAND2_H
#define CONCRETE_COMMAND2_H

#include <iostream>
#include <string>

#include "command.h"

class ConcreteCommand2 : public Command {
 public:
  ConcreteCommand2(const std::string &a) : arg(a) {}
  void execute() override { std::cout << "#2 process..." << arg << std::endl; }

 private:
  std::string arg;
};
#endif  // CONCRETE_COMMAND2_H