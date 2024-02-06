#ifndef CONTEXT_H
#define CONTEXT_H

class State;
class Context {
 public:
  Context(State* state) : state_(state) {}
  ~Context();
  void Handle();
  void OperationForStateA();
  void OperationForStateB();

 private:
  friend class State;
  void ChangeState(State* state);

  State* state_;
};
#endif  // CONTEXT_H