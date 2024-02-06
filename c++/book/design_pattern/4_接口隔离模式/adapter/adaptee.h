#ifndef ADAPTEE_H
#define ADAPTEE_H

#include <iostream>

// 遗留接口（老接口）
class Adaptee {
 public:
  Adaptee() = default;
  ~Adaptee() = default;
  void SpecificRequest() {
    std::cout << "Adaptee::SpecificRequest" << std::endl;
  }
};
#endif  // ADAPTEE_H