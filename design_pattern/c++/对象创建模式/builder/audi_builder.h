#ifndef AUDI_BUILDER_H
#define AUDI_BUILDER_H

#include "car_builder.h"

// 具体建造者：奔驰
class AudiBuilder : public CarBuilder {
 public:
  // 具体实现方法
  void buildTire() override { car_.setCarTire("audi_tire"); }
  void buildSteeringWheel() override {
    car_.setCarSteeringWheel("audi_steering_wheel");
  }
  void buildEngine() override { car_.setCarEngine("audi_engine"); }
  void buildWindows() override {
    for (int i = 0; i < 2; ++i) {
      car_.setCarEngine("audi_window_" + std::to_string(i + 1));
    }
  }
};
#endif  // AUDI_BUILDER_H_
