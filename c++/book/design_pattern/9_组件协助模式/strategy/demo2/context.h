#ifndef CONTEXT_H
#define CONTEXT_H

#include "strategy.h"

// 这个类是Strategy模式的关键，也是Strategy模式和Template模式的根本区别所在。
// Strategy 通过组合（委托）方式实现算法（实现）的异构，
// 而Template模式则采取的是继承的方式
// 这两个模式的区别也是继承和组合两种实现接口重用的方式的区别。

class Context {
 public:
  Context(Strategy* stg) : stg_(stg) {}
  ~Context() {
    if (!stg_) {
      delete stg_;
    }
  }
  void DoAction() { stg_->AlgrithmInterface(); }

 private:
  Strategy* stg_;
};

#endif  // CONTEXT_H