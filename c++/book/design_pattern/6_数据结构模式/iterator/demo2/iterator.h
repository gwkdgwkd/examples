#ifndef ITERATOR_H
#define ITERATOR_H

#include "aggregate.h"

typedef int Object;

class Iterator {
 public:
  virtual ~Iterator() = default;
  virtual void First() = 0;
  virtual void Next() = 0;
  virtual bool IsDone() = 0;
  virtual Object CurrentItem() = 0;

 protected:
  Iterator() = default;
};
#endif  // ITERATOR_H