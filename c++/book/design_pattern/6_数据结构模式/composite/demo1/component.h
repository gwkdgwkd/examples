#ifndef COMPONENT_H
#define COMPONENT_H

class Component {
 public:
  virtual void process() = 0;
  virtual ~Component() {}
};
#endif  // COMPONENT_H