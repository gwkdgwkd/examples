#ifndef COLLEAGE_H
#define COLLEAGE_H

#include <string>

#include "mediator.h"

class Colleage {
 public:
  virtual ~Colleage() = default;
  virtual void Aciton() = 0;
  virtual void SetState(const std::string& sdt) = 0;
  virtual std::string GetState() = 0;

 protected:
  Colleage() = default;
  Colleage(Mediator* mdt) { this->mdt_ = mdt; }
  Mediator* mdt_;
};

#endif  // COLLEAGE_H