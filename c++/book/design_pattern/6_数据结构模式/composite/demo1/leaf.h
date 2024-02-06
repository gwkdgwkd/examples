#ifndef LEAF_H
#define LEAF_H

#include <iostream>
#include <string>

#include "component.h"

// 叶子节点
class Leaf : public Component {
 public:
  Leaf(std::string s) : name(s) {}

  void process() {
    // process current node
    std::cout << "Leaf: " << name << std::endl;
  }

 private:
  std::string name;
};
#endif  // LEAF_H