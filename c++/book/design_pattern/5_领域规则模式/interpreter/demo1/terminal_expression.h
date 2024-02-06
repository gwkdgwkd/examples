#ifndef TREMINAL_EXPRESSION_H
#define TREMINAL_EXPRESSION_H

#include <iostream>
#include <string>

#include "abstract_expression.h"

class TerminalExpression : public AbstractExpression {
 public:
  TerminalExpression(const std::string& statement) : statement_(statement) {}
  ~TerminalExpression() {}
  void Interpret(const Context& c) {
    std::cout << " TerminalExpression::Interpret " << statement_ << std::endl;
  }

 private:
  std::string statement_;
};

#endif  // TREMINAL_EXPRESSION_H
