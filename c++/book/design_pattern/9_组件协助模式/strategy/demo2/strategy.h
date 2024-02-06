#ifndef STRATEGY_H
#define STRATEGY_H

class Strategy {
 public:
  Strategy() = default;
  virtual ~Strategy() = default;
  virtual void AlgrithmInterface() = 0;
};
#endif  // STRATEGY_H