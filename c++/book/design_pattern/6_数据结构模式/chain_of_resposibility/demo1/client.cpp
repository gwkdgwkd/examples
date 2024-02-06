#include "handler1.h"
#include "handler2.h"
#include "handler3.h"

// Chain of Resposibility 职责链

// 动机（Motivation）：
// 1.在软件构建过程中，一个请求可能被多个对象处理，但是每个请求在运行时只能有一个接受者，
//   如果显示指定，将必不可少地带来请求发送者与接受者的紧耦合。
// 2.如何使请求的发送者不需要指定具体的接受者？
//   让请求的接受者自己在运行时决定来处理请求，从而使两者解耦。

// 定义：
// 使多个对象都有机会处理请求，从而避免请求的发送者和接收者之间的耦合关系。
// 将这些对象连成一条链，并沿着这条链传递请求，直到有一个对象处理它为止。

// 要点总结：
// 1.Chain of Resposibility模式的应用场合在于，一个请求可能有多个接受者，
//   但是最后真正的接受者只有一个，此时请求者与接受者的耦合有可能出现变化脆弱的症状，
//   职责链的目的就是将二者解耦，从而更好地应对变化；
// 2.应用了Chain of Resposibility模式后，对象的职责分派将更具有灵活性，
//   可以在运行时动态添加/修改请求的处理职责；
// 3.如果请求传递到职责链的末尾扔得不到处理，应该有一个合理的缺省机制，
//   这也是每一个接受对象的责任，而不是发出请求的对象的责任；
// 4.应该也算过时的设计模式了，GOF23是94年提出的，Java是95年问世的，
//   随着时代发展，有些模式已经被语言特性取代了。

int main() {
  Handler1 h1;
  Handler2 h2;
  Handler3 h3;
  h2.setNextChain(&h1);
  h1.setNextChain(&h3);
  // h2 -> h1 -> h3

  Reqest req1("process task 1... ", RequestType::REQ_HANDLER1);
  Reqest req2("process task 2... ", RequestType::REQ_HANDLER2);
  Reqest req3("process task 3... ", RequestType::REQ_HANDLER3);

  h2.handle(req1);
  h2.handle(req2);
  h2.handle(req3);
  // Handler1 is handle reqest: process task 1...
  // Handler2 is handle reqest: process task 2...
  // Handler3 is handle reqest: process task 3...

  h1.handle(req1);
  h1.handle(req2);
  h1.handle(req3);
  // Handler1 is handle reqest: process task 1...
  // Handler3 is handle reqest: process task 3...

  h3.handle(req1);
  h3.handle(req2);
  h3.handle(req3);
  // Handler3 is handle reqest: process task 3...

  return 0;
}