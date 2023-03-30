#ifndef CAR_BUILDER_H_
#define CAR_BUILDER_H_

#include "car.h"

// 抽象建造者
class CarBuilder {
 public:
  Car getCar() { return car_; }

  // 抽象方法
  virtual void buildTire() = 0;
  virtual void buildSteeringWheel() = 0;
  virtual void buildEngine() = 0;
  virtual void buildWindows() = 0;

 protected:
  Car car_;
};

#endif  // CAR_BUILDER_H_
