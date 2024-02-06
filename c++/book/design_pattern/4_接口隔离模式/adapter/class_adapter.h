#ifndef CLASS_ADAPTER_H
#define CLASS_ADAPTER_H

#include <string>

#include "adaptee.h"
#include "target.h"

// 类适配器，多继承
class ClassAdapter : public Target, private Adaptee {  // 多继承
 public:
  ~ClassAdapter() {}
  void Request() { this->SpecificRequest(); }
};
#endif  // CLASS_ADAPTER_H