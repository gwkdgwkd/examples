
#include <string.h>

#include <iostream>
#include <stack>

#include "add_expression.h"
#include "sub_expression.h"
#include "var_expression.h"

// Interpreter 解析器

// 动机（Motivation）：
// 1.在软件构建过程中，如果某一特定领域的问题比较复杂，类似的结构不断重复出现，
//   如果使用普通的编程方式来实现将面临非常频繁的变化；
// 2.在这种情况下，将特定领域的问题表达为某种语法规则下的句子，
//   然后构建一个解释器来解释这样的句子，从而达到解决问题的目的。

// 定义：
// 给定一个语言，定义它的文法的一种表示，并定义一种解释器，
// 这个解释器使用该表示来解释语言中的句子。

// 要点总结：
// 1.Interpreter模式的应用场合是Interpreter模式应用中的难点，只有满足：
//   a.业务规则频繁变化；
//   b.类似的结构不断重复出现；
//   c.容易抽象为语法规则的问题。
//   才适合使用Interpreter模式；
// 2.使用Interpreter来表示文法规则，从而可以使用面向对象技巧来方便地扩展文法；
// 3.Interpreter比较适合简单的文法表示，对于复杂的文法表示，
//   Interpreter会产生比较大的类层次结构，需要求助于语法分析生成器这样的标准工具。

Expression* analyse(std::string expStr) {
  std::stack<Expression*> expStack;
  Expression* left = nullptr;
  Expression* right = nullptr;
  for (int i = 0; i < expStr.size(); i++) {
    switch (expStr[i]) {
      case '+':
        // 加法运算
        left = expStack.top();
        right = new VarExpression(expStr[++i]);
        expStack.push(new AddExpression(left, right));
        break;
      case '-':
        // 减法运算
        left = expStack.top();
        right = new VarExpression(expStr[++i]);
        expStack.push(new SubExpression(left, right));
        break;
      default:
        // 变量表达式
        expStack.push(new VarExpression(expStr[i]));
    }
  }

  Expression* expression = expStack.top();

  return expression;
}

void release(Expression* expression) {
  // 释放表达式树的节点内存
}

int main() {
  std::string expStr = "a+b-c+d-e";
  std::map<char, int> var;
  var.insert(std::make_pair('a', 5));
  var.insert(std::make_pair('b', 2));
  var.insert(std::make_pair('c', 1));
  var.insert(std::make_pair('d', 6));
  var.insert(std::make_pair('e', 10));

  Expression* expression = analyse(expStr);
  int result = expression->interpreter(var);

  std::cout << result << std::endl;  // 2

  release(expression);

  return 0;
}
