#ifndef CONCRETE_COMMAND1_H
#define CONCRETE_COMMAND1_H

#include <iostream>
#include <string>

#include "command.h"

class ConcreteCommand1 : public Command {
 public:
  ConcreteCommand1(const std::string &a) : arg(a) {}
  void execute() override { std::cout << "#1 process..." << arg << std::endl; }

 private:
  std::string arg;
};
#endif  // CONCRETE_COMMAND1_H