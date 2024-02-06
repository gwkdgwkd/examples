#ifndef HANDLE_H
#define HANDLE_H

#include <iostream>

class Handle {
 public:
  virtual ~Handle() { delete succ_; }
  virtual void HandleRequest() = 0;
  void SetSuccessor(Handle* succ) { succ_ = succ; }
  Handle* GetSuccessor() { return succ_; }

 protected:
  Handle() : succ_(nullptr) {}
  Handle(Handle* succ) : succ_(succ) {}

 private:
  Handle* succ_;
};

#endif  // HANDLE_H