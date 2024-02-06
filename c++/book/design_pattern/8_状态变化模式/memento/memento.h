#ifndef MEMENTO_H
#define MEMENTO_H

#include <string>

#include "memento.h"

class Originator;
class Memento {
 private:
  // Memento模式的关键就是：
  friend class Originator;
  // Memento的接口都声明为private，而将Originator声明为Memento的友元类。
  // 将Originator的状态保存在Memento类中，而将Memento接口private起来，
  // 也就达到了封装的功效。

  typedef std::string State;
  Memento() = default;
  Memento(const State& sdt) : sdt_(sdt) {}
  ~Memento() = default;
  void SetState(const State& sdt) { sdt_ = sdt; }
  State GetState() { return sdt_; }

 private:
  State sdt_;  // 与Originator中数据一样
};
#endif  // MEMENTO_H