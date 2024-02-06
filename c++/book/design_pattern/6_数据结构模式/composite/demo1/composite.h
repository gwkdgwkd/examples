#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <iostream>
#include <list>
#include <string>

#include "component.h"

// 树节点
class Composite : public Component {
 public:
  Composite(const std::string& s) : name(s) {}
  void add(Component* element) { elements.push_back(element); }
  void remove(Component* element) { elements.remove(element); }

  void process() {
    // 1. process current node
    std::cout << "Composite: " << name << std::endl;
  
    // 2. process leaf nodes
    for (auto& e : elements) {
      e->process();  // 多态调用
    }
  }

 private:
  std::string name;
  std::list<Component*> elements;
};
#endif  // COMPOSITE_H