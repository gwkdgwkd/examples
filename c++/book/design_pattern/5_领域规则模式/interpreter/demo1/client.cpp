#include <iostream>

#include "context.h"
#include "non_terminal_expression.h"
#include "terminal_expression.h"

// 一些应用提供了内建的脚本或者宏语言来让用户可以定义他们能够在系统中进行的操作。
// Interpreter模式的目的就是使用一个解释器为用户提供一门定义语言的语法表示的解释器，
// 然后通过这个解释器来解释语言中的句子。

// XML格式的数据解析是一个在应用开发中很常见并且有时候是很难处理的事情，
// 虽然目前很多的开发平台、语言都提供了对XML格式数据的解析，但是例如到了移动终端设备上，
// 由于处理速度、计算能力、存储容量的原因解析XML格式的数据却是很复杂的一件事情，
// 最近也提出了很多的移动设备的XML格式解析器，但有时还是需要自己去设计和实现这一个过程。
// Interpreter模式则提供了一种很好的组织和设计这种解析器的架构。
// Interpreter模式中使用类来表示文法规则，因此可以很容易实现文法的扩展。
// 另外对于终结符可以使用Flyweight模式来实现终结符的共享。

int main() {
  Context* c = new Context();
  AbstractExpression* te = new TerminalExpression("hello");
  AbstractExpression* nte = new NonTerminalExpression(te, 2);
  nte->Interpret(*c);

  // TerminalExpression::Interpret hello
  // TerminalExpression::Interpret hello

  return 0;
}