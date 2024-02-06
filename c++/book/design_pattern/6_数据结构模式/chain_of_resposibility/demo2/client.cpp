#include <iostream>

#include "concrete_handle_a.h"
#include "concrete_handle_b.h"

int main() {
  Handle *pa = new ConcreteHandleA();
  // pa没有后继节点，所以pa自己处理消息：
  pa->HandleRequest();  // ConcreteHandleA::HandleRequest

  Handle *pb = new ConcreteHandleB();
  pa->SetSuccessor(pb);  // 设置pa的后继节点pb

  // pa有后继节点，所以由后继节点pb来处理消息：
  pa->HandleRequest();  // ConcreteHandleB::HandleRequest

  // Chain of Responsibility模式的最大的一个有点就是给系统降低了耦合性，
  // 请求的发送者完全不必知道该请求会被哪个应答对象处理，极大地降低了系统的耦合性。

  return 0;
}