#ifndef MACRO_COMMAND_H
#define MACRO_COMMAND_H

#include <iostream>
#include <vector>

#include "command.h"

class MacroCommand : public Command {
 public:
  void addCommand(Command *c) { commands.push_back(c); }
  void execute() override {
    for (auto &c : commands) {
      c->execute();
    }
  }

 private:
  std::vector<Command *> commands;
};
#endif  // MACRO_COMMAND_H