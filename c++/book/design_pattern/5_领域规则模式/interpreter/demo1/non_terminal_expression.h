#ifndef NON_TERMINAL_EXPRESSION_H
#define NON_TERMINAL_EXPRESSION_H

#include "abstract_expression.h"

class NonTerminalExpression : public AbstractExpression {
 public:
  NonTerminalExpression(AbstractExpression* expression, int times)
      : expression_(expression), times_(times) {}
  ~NonTerminalExpression() {}
  void Interpret(const Context& c) {
    for (int i = 0; i < times_; i++) {
      this->expression_->Interpret(c);
    }
  }

 private:
  AbstractExpression* expression_;
  int times_;
};

#endif  // NONTERMINAL_EXPRESSION_H
