#ifndef ORIGINATOR_H
#define ORIGINATOR_H

#include <iostream>
#include <string>

#include "memento.h"

class Originator {
 public:
  typedef std::string State;
  Originator() : sdt_(""), mt_(nullptr) {}
  Originator(const State& sdt) : sdt_(sdt), mt_(nullptr) {}
  ~Originator() = default;
  Memento* CreateMemento() { return new Memento(sdt_); }
  void SetMemento(Memento* men) {}
  void RestoreToMemento(Memento* mt) { sdt_ = mt->GetState(); }
  State GetState() { return sdt_; }
  void SetState(const State& sdt) { sdt_ = sdt; }
  void PrintState() { std::cout << sdt_ << std::endl; }

 private:
  State sdt_;
  Memento* mt_;
};
#endif  // ORIGINATOR_H