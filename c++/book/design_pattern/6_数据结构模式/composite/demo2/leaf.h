#ifndef LEAF_H
#define LEAF_H

#include <iostream>

#include "component.h"

class Leaf : public Component {
 public:
  Leaf() = default;
  ~Leaf() = default;
  void Operation() { std::cout << "Leaf" << std::endl; }
};
#endif  // LEAF_H