#ifndef CONCRETE_ITERATOR_H
#define CONCRETE_ITERATOR_H

#include "iterator.h"

class ConcreteIterator : public Iterator {
 public:
  ConcreteIterator(Aggregate* ag, int idx = 0) : ag_(ag), idx_(idx) {}
  ~ConcreteIterator() = default;
  void First() { idx_ = 0; }
  void Next() {
    if (idx_ < ag_->GetSize()) {
      idx_++;
    }
  }
  bool IsDone() { return (idx_ == ag_->GetSize()); }
  Object CurrentItem() { return ag_->GetItem(idx_); }

 private:
  Aggregate* ag_;
  int idx_;
};
#endif  // CONCRETE_ITERATOR_H