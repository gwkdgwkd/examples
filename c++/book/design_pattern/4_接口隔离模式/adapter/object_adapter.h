#ifndef OBJECT_ADAPTER_H
#define OBJECT_ADAPTER_H

#include <string>

#include "adaptee.h"
#include "target.h"

// 对象适配器
class ObjectAdapter : public Target {  // 继承
 public:
  ObjectAdapter(Adaptee* ade) : ade_(ade) {}
  ~ObjectAdapter() {}
  void Request() { ade_->SpecificRequest(); }

 private:
  Adaptee* ade_;  // 组合
};
#endif  // OBJECT_ADAPTER_H