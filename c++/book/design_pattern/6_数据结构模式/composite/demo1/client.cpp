#include "composite.h"
#include "leaf.h"

// Composite 模式

// 动机（Motivation）：
// 1.在软件在某些情况下，客户代码过多地依赖于对象容器复杂的内部实现结构，
//   对象容器内部实现结构（而非抽象接口）的变化将引起客户代码的频繁变化，
//   带来了代码的维护性、扩展性等弊端；
// 2.如何将客户代码与复杂的对象容器结构解耦？让对象容器自己来实现自身的复杂结构，
//   从而使得客户代码就像处理简单对象一样来处理复杂的对象容器？

// 定义：
// 将对象组合成树形结构以表示部分-整体的层次结构。
// Composite使得用户对单个对象和组合对象的使用具有一致性（稳定）。

// 要点总结：
// 1.Composite模式采用树形结构来实现普遍存在的对象容器，
//   从而将一对多转换为一对一，使得客户代码可以一致地（复用）处理对象和对象容器，
//   无需关系处理的是单个的对象，还是组合的对象；
// 2.将客户代码与复杂的对象容器结构解耦是Composite的核心思想，解耦之后，
//   客户代码将与纯粹的抽象接口而非对象容器的内部实现结构发生依赖，从而更能应对变化；
// 3.Composite模式在具体实现中，可以让父对象中的子对象反向追溯，
//   如果父对象有频繁的遍历需求，可以使用缓存技巧来改善效率。

void Invoke(Component& c) {
  //...
  c.process();
  //...
}

int main() {
  Composite root("root");
  Composite treeNode1("treeNode1");
  Composite treeNode2("treeNode2");
  Composite treeNode3("treeNode3");
  Composite treeNode4("treeNode4");
  Leaf leaf1("left1");
  Leaf leaf2("left2");

  root.add(&treeNode1);
  treeNode1.add(&treeNode2);
  treeNode2.add(&leaf1);

  root.add(&treeNode3);
  treeNode3.add(&treeNode4);
  treeNode4.add(&leaf2);

  Invoke(root);
  // Composite: root
  // Composite: treeNode1
  // Composite: treeNode2
  // Leaf: left1
  // Composite: treeNode3
  // Composite: treeNode4
  // Leaf: left2

  Invoke(leaf2);
  // Leaf: left2

  Invoke(treeNode3);
  // Composite: treeNode3
  // Composite: treeNode4
  // Leaf: left2
}