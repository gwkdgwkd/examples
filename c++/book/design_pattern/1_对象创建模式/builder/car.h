#ifndef CAR_H
#define CAR_H

#include <iostream>
#include <string>
#include <vector>

// 产品类：车
class Car {
 public:
  Car() {}
  void setCarTire(const std::string &t) {
    tire_ = t;
    std::cout << "set tire: " << tire_ << std::endl;
  }
  void setCarSteeringWheel(const std::string &sw) {
    steering_wheel_ = sw;
    std::cout << "set steering wheel: " << steering_wheel_ << std::endl;
  }
  void setCarEngine(const std::string &e) {
    engine_ = e;
    std::cout << "set engine: " << engine_ << std::endl;
  }
  void setWindow(const std::string &w) {
    windows_.push_back(w);
    std::cout << "set engine: " << engine_ << std::endl;
  }

 private:
  std::string tire_;                  // 轮胎
  std::string steering_wheel_;        // 方向盘
  std::string engine_;                // 发动机
  std::vector<std::string> windows_;  // 窗户
};

#endif  // CAR_H
