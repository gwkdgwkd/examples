#ifndef BENZ_BUILDER_H_
#define BENZ_BUILDER_H_

#include "car_builder.h"

// 具体建造者：奔驰
class BenzBuilder : public CarBuilder {
 public:
  // 具体实现方法
  void buildTire() override { car_.setCarTire("benz_tire"); }
  void buildSteeringWheel() override {
    car_.setCarSteeringWheel("benz_steering_wheel");
  }
  void buildEngine() override { car_.setCarEngine("benz_engine"); }
  void buildWindows() override {
    for (int i = 0; i < 4; ++i) {
      car_.setCarEngine("benz_window_" + std::to_string(i + 1));
    }
  }
};
#endif  // BENZ_BUILDER_H_
