#ifndef TARGET_H
#define TARGET_H

#include <iostream>

// 目标接口（新接口）
class Target {
 public:
  virtual ~Target() {}
  virtual void Request() { std::cout << "Target::Request" << std::endl; }
};
#endif  // TARGET_H