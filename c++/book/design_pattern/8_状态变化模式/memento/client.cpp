#include <iostream>

#include "memento.h"
#include "originator.h"

// Memento 备忘录

// 没有人想犯错误，但是没有人能够不犯错误，犯了错误一般只能改过，却很难改正（恢复）。
// 世界上没有后悔药，但是在进行软件系统的设计时候是要给用户后悔的权利，
// 实际上可能也是用户要求的权利：
// 对一些关键性的操作肯定需要提供诸如撤销（Undo）的操作，后悔药就是Memento提供的。

// 动机（Motivation）：
// 1.在软件构建过程中，某些对象的状态在转换过程中，可能由于某种需要，
//   要求程序能够回溯到对象之前处于某个点时的状态，
//   如果使用一些公有接口来让其他对象得到对象的状态，便会暴露对象的细节实现；
// 2.如何实现对象状态的良好保存与恢复，但同时又不会因此而破坏对象本身的封装性。

// 定义：
// 在不破坏封装性的前提下，捕获一个对象的内部状态，
// 并在该对象之外保存这个状态，这样以后就可以将该对象恢复到原先保存的状态。

// 要点总结：
// 1.备忘录存储原发器对象的内部状态，在需要时恢复原发器状态；
// 2.Memenot模式的核心时信息隐藏，即Originator需要向外界隐藏信息，
//   保持其封装性，但同时又需要将状态保持到外界；
// 3.由于现代语言运行时（如C#、Java等）都具有相当的对象序列化支持，
//   因此往往采用效率较高、又较容易正确实现的序列化方案来实现Memento模式。

// 在Command模式中，Memento模式经常被用来维护可以撤销（Undo）操作的状态。

int main() {
  Originator* o = new Originator();
  o->SetState("old");
  o->PrintState();  // old

  Memento* m = o->CreateMemento();
  o->SetState("new");
  o->PrintState();  // new

  o->RestoreToMemento(m);
  o->PrintState();  // old

  return 0;
}