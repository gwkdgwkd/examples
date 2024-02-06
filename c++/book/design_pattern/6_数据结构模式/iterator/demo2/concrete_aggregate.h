#ifndef CONCRETE_AGGREGATE_H
#define CONCRETE_AGGREGATE_H

#include "aggregate.h"
#include "concrete_iterator.h"

class ConcreteAggregate : public Aggregate {
 public:
  enum { SIZE = 3 };
  ConcreteAggregate() {
    for (int i = 0; i < SIZE; i++) {
      objs_[i] = i;
    }
  }
  ~ConcreteAggregate() = default;
  Iterator* CreateIterator() { return new ConcreteIterator(this); }
  Object GetItem(int idx) {
    if (idx < this->GetSize()) {
      return objs_[idx];
    } else {
      return -1;
    }
  }
  int GetSize() { return SIZE; }

 private:
  Object objs_[SIZE];
};
#endif  // CONCRETE_AGGREGATE_H