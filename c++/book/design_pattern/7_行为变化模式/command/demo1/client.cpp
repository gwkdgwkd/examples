#include <iostream>

#include "concrete_command1.h"
#include "concrete_command2.h"
#include "macro_command.h"

// Command 命令模式

// 动机（Motivation）：
// 1.在软件构建过程中，行为请求者与行为实现者通常呈现一种紧耦合。
//   但在某些场合，比如需要对行为进行记录、撤销、undo、redo、事物等处理，
//   这种无法抵御变化的紧耦合是不合适的。
// 2.在这种情况下，如何将行为请求者与行为实现者解耦？
//   将一组行为抽象为对象，可以实现二者之间的松耦合。

// 定义：
// 将一个请求（行为）封装为一个对象，从而使你可用不同的请求对客户进行参数化；
// 对请求排队或记录请求日志，以及支持可撤销的操作。

// 要点总结：
// 1.Command模式的根本目的在于将行为请求者与行为实现者解耦，
//   在面向对象语言中，常见的实现手段是将行为抽象为对象；
// 2.实现Command接口的具体命令对象ConcreteCommand有时可能会保存一些额外的状态信息，
//   通过使用Command模式，可以将多个命令封装为一个复合命令MacroCommand；
// 3.Command模式与C++中的函数对象有些类似，但两者定义行为接口的规范有所区别：
//   Command以面向对象中的接口-实现来定义行为接口的规范，更严格，但有性能损失；
//   C++函数对象以函数签名来定义行为接口规范，更灵活，性能更高。

int main() {
  ConcreteCommand1 command1("Arg ###");
  ConcreteCommand2 command2("Arg $$$");
  command1.execute();  // #1 process...Arg ###
  command2.execute();  // #2 process...Arg $$$

  MacroCommand macro;
  macro.addCommand(&command1);
  macro.addCommand(&command2);
  macro.execute();
  // #1 process...Arg ###
  // #2 process...Arg $$$
}