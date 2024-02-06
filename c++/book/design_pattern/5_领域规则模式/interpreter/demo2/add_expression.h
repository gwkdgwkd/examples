#ifndef ADD_EXPRESSION_H
#define ADD_EXPRESSION_H

#include "symbol_expression.h"

// 加法运算
class AddExpression : public SymbolExpression {
 public:
  AddExpression(Expression* left, Expression* right)
      : SymbolExpression(left, right) {}
  int interpreter(std::map<char, int> var) override {
    return left->interpreter(var) + right->interpreter(var);
  }
};
#endif  // ADD_EXPRESSION_H