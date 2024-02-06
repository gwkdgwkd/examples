#ifndef SYMBOL_EXPRESSION_H
#define SYMBOL_EXPRESSION_H

#include "expression.h"

// 符号表达式
class SymbolExpression : public Expression {
 public:
  SymbolExpression(Expression* left, Expression* right)
      : left(left), right(right) {}

 protected:
  // 运算符左右两个参数：
  Expression* left;
  Expression* right;
};
#endif  // SYMBOL_EXPRESSION_H