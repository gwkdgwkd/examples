#include <iostream>

#include "composite.h"
#include "leaf.h"

// 在开发中，经常可能要递归构建树状的组合结构，Composite模式则提供了很好的解决方案。

// Composite模式和Decorator模式有着类似的结构图，但是Composite模式旨在构造类，
// 而Decorator模式重在不生成子类即可给对象添加职责。
// Decorator模式重在修饰，而Composite模式重在表示。

int main(int argc, char* argv[]) {
  Leaf* l = new Leaf();
  l->Operation();
  // Leaf

  Composite* com = new Composite();
  com->Add(l);
  com->Operation();
  // Composite
  // Leaf

  Component* ll = com->GetChild(0);
  ll->Operation();
  // Leaf
}