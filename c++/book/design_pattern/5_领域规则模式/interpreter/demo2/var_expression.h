#ifndef VAR_EXPRESSION_H
#define VAR_EXPRESSION_H

#include "expression.h"

// 变量表达式
class VarExpression : public Expression {
 public:
  VarExpression(const char& key) { this->key = key; }
  int interpreter(std::map<char, int> var) override { return var[key]; }

 private:
  char key;
};
#endif  // VAR_EXPRESSION_H