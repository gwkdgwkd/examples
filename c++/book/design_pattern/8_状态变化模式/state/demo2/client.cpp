#include <iostream>

#include "concrete_state_a.h"
#include "concrete_state_b.h"
#include "context.h"

// 每个人、事物在不同的状态下会有不同表现（动作），
// 而一个状态又会在不同的表现下转移到下一个不同的状态（State）。
// 最简单的一个生活中的例子就是：
// 地铁入口处，如果你放入正确的地铁票，门就会打开让你通过。
// 在出口处也是验票，如果正确你就可以ok，否则就不让你通过，
// 如果你动作野蛮，或许会有报警（Alarm）。

// 有限状态自动机（FSM）也是一个典型的状态不同，对输入有不同的响应（状态转移）。
// 通常在实现这类系统会使用到很多的Switch/Case语句，Case某种状态，发生什么动作，
// Case另外一种状态，则发生另外一种状态，但是这种实现方式至少有以下两个问题：
// 1.当状态数目不是很多的时候，Switch/Case可能可以搞定，
//   但是当状态数目很多的时候（实际系统中也正是如此），
//   维护一大组的Switch/Case语句将是一件异常困难并且容易出错的事情；
// 2.状态逻辑和动作实现没有分离，在很多的系统实现中，
//   动作的实现代码直接写在状态的逻辑当中，这带来的后果就是系统的扩展性和维护得不到保证。

// State模式就是被用来解决上面两个问题的，在State模式中将状态逻辑和动作实现进行分离。
// 当一个操作中要维护大量的case分支语句，并且这些分支依赖于对象的状态。
// State模式将每一个分支都封装到独立的类中。

// State模式的应用也非常广泛，从最高层逻辑用户接口GUI到最底层的通讯协议都有其用武之地。
// State模式和Strategy模式又很大程度上的相似：它们都有一个Context类，
// 都是通过委托（组合）给一个具有多个派生类的多态基类实现Context的算法逻辑。

// 两者最大的差别就是State模式中派生类持有指向Context对象的引用，
// 并通过这个引用调用Context中的方法，但在Strategy模式中就没有这种情况。
// 因此可以说一个State实例同样是Strategy模式的一个实例，反之却不成立。

// 实际上State模式和Strategy模式的区别还在于它们所关注的点不尽相同：
// State模式主要是要适应对象对于状态改变时的不同处理策略的实现，
// 而Strategy则主要是具体算法和实现接口的解耦，
// Strategy模式中并没有状态的概念，并且更加不关心状态的改变了。
// State模式很好地实现了对象的状态逻辑和动作实现的分离，
// 状态逻辑分布在State的派生类中实现，而动作实现则可以放在Context类中实现。
// 这使得两者的变化相互独立，改变State的状态逻辑可以很容易复用Context的动作，
// 也可以在不影响State派生类的前提下创建Context的子类来更改或替换动作实现。
// State模式问题主要是逻辑分散化，状态逻辑分布到了很多的State的子类中，
// 很难看到整个的状态逻辑图，这也带来了代码的维护问题。

int main() {
  State* st = new ConcreteStateA;
  Context* con = new Context(st);
  con->Handle();  // Do operation in State A :: State change from A to B
  con->Handle();  // Do operation in State B :: State change from B to A
  con->Handle();  // Do operation in State A :: State change from A to B
  if (con != nullptr) delete con;
  if (st != nullptr) st = nullptr;

  return 0;
}

// g++ client.cpp context.cpp state.cpp concrete_state_a.cpp
// concrete_state_b.cpp