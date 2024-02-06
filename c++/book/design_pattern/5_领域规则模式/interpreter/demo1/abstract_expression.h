#ifndef ABSTRACT_EXPRESSION_H
#define ABSTRACT_EXPRESSION_H

#include "context.h"

class AbstractExpression {
 public:
  virtual ~AbstractExpression() {}
  virtual void Interpret(const Context& c) {}

 protected:
  AbstractExpression() {}
};

#endif  // ABSTRACT_EXPRESSION_H