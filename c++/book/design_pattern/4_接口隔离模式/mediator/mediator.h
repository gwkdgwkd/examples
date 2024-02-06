#ifndef MEDIATOR_H
#define MEDIATOR_H

class Mediator {
 public:
  virtual ~Mediator() = default;
  virtual void DoActionFromAtoB() = 0;
  virtual void DoActionFromBtoA() = 0;

 protected:
  Mediator() = default;
};
#endif  // MEDIATOR_H