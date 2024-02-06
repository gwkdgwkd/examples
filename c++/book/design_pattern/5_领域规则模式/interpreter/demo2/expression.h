#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <map>

class Expression {
 public:
  virtual int interpreter(std::map<char, int> var) = 0;
  virtual ~Expression() {}
};
#endif  // EXPRESSION_H