#include <iostream>

namespace n1 {  // 不好的实现方式
class Element {
 public:
  virtual void Func1() = 0;

  // 基类需要频繁的修改
  virtual void Func2(int data) = 0;
  virtual void Func3(int data) = 0;
  virtual ~Element() {}
};

class ElementA : public Element {
 public:
  void Func1() override {}
  void Func2(int data) override {}
};

class ElementB : public Element {
 public:
  void Func1() override {}
  void Func2(int data) override {}
};
}  // namespace n1

// Visitor 访问器

// 动机（Motivation）：
// 1.由于需求的改变，某些类层次结构中常常需要增加新的行为（方法），
//   如果直接修改基类，将会给子类带来很繁重的变更负担，甚至破坏原有设计；
// 2.如何在不更改类层次结构的前提下，避免上述问题？
//   即在运行时根据需要透明地为类层次结构上的各个类动态添加新的操作。

// 定义：
// 表示一个作用于某对象结构中的各元素的操作。
// 使得可以在不改变（稳定）各元素的类的前提下定义（扩展）作用于这些元素的新操作（变化）。

// 要点总结：
// 1.Visitor模式通过所谓双重分发（double dispatch），
//   来实现在不更改（不添加新的操作-编译时）Element类层次结构的前提下，
//   在运行时透明地为类层次结构上的各个类动态添加新的操作（支持变化）；
// 2.所谓双重分发即Visitor模式中间包括了两个多态分发（注意其中的多态机制）：
//   第一个为accept方法的多态辨析；
//   第二个为visitElementX方法的多态辨析。
// 3.Visitor模式的最大缺点在于扩展类层次结构（增添新的Element子类），
//   会导致Visitor类的改变，因此Vistor模式适用于：
//   Element类层次结构稳定，而其中的操作却经常面临频繁改的。

#define USE1

#include "element_a.h"
#include "element_b.h"
#ifdef USE1
#include "visitor1.h"
#else
#include "visitor2.h"
#endif

void func(Element &element, Visitor &visitor) { element.accept(visitor); }

int main() {
  ElementA elementA;
  ElementB elementB;

#ifdef USE1
  Visitor1 visitor;
  func(elementA, visitor);
  func(elementB, visitor);
  // Visitor1 is processing ElementB
  // Visitor1 is processing ElementA
#else
  Visitor2 visitor;
  func(elementA, visitor);
  func(elementB, visitor);
  // Visitor2 is processing ElementB
  // Visitor2 is processing ElementA
#endif

  return 0;
}