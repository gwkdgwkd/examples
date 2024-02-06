#ifndef COMPONENT_H
#define COMPONENT_H

class Component {
 public:
  Component() = default;
  virtual ~Component() = default;

 public:
  virtual void Operation() = 0;
  virtual void Add(const Component&) {}
  virtual void Remove(const Component&) {}
  virtual Component* GetChild(int) { return nullptr; }
};
#endif  // COMPONENT_H