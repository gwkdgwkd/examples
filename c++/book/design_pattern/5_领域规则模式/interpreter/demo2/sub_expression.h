#ifndef SUB_EXPRESSION_H
#define SUB_EXPRESSION_H

#include "symbol_expression.h"

// 减法运算
class SubExpression : public SymbolExpression {
 public:
  SubExpression(Expression* left, Expression* right)
      : SymbolExpression(left, right) {}
  int interpreter(std::map<char, int> var) override {
    return left->interpreter(var) - right->interpreter(var);
  }
};
#endif  // SUB_EXPRESSION_H