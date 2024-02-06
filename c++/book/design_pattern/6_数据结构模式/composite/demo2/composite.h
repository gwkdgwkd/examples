#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "component.h"

class Composite : public Component {
 public:
  Composite() = default;
  ~Composite() = default;

 public:
  void Operation() {
    std::cout << "Composite" << std::endl;

    for (auto comIter = comVec.begin(); comIter != comVec.end(); comIter++) {
      (*comIter)->Operation();
    }
  }
  void Add(Component* com) { comVec.push_back(com); }
  void Remove(Component* com) {
    auto itr = std::find(comVec.begin(), comVec.end(), com);
    if (itr != comVec.end()) {
      comVec.erase(itr);
    }
  }
  Component* GetChild(int index) { return comVec[index]; }

 private:
  // Composite模式在实现中有一个问题就是要提供对于子节点（Leaf）的管理策略，
  // 这里使用vector，可以提供其他的实现方式，如数组、链表、Hash表等：
  std::vector<Component*> comVec;
};
#endif  // COMPOSITE_H