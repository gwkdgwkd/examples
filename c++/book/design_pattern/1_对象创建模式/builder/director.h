#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "car_builder.h"

class Director {
 public:
  Director() : builder_(nullptr) {}

  void set_builder(CarBuilder* cb) { builder_ = cb; }

  // 组装汽车
  Car ConstructCar() {
    builder_->buildTire();
    builder_->buildSteeringWheel();
    builder_->buildEngine();
    builder_->buildWindows();
    return builder_->getCar();
  }

 private:
  CarBuilder* builder_;
};

#endif  // DIRECTOR_H
