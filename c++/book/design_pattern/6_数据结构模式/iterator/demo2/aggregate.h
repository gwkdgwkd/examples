#ifndef AGGREGATE_H
#define AGGREGATE_H

#include "iterator.h"

typedef int Object;

class Aggregate {
 public:
  virtual ~Aggregate() = default;
  virtual Iterator* CreateIterator() = 0;
  virtual Object GetItem(int idx) = 0;
  virtual int GetSize() = 0;

 protected:
  Aggregate() = default;
};
#endif  // AGGREGATE_H